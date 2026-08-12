/* TiNGanyevent.c - 'any channel' event handling */

#include "TiNGanyevent.h"
#include "TiNGcommon.h"
#include "TiNGdetect.h"
#include "TiNGsmbesp.h"
#include "TiNGsmtone.h"
#include "trace.h"
#include <stdio.h>
#include <stdlib.h>

#define arlen(x) (sizeof(x)/sizeof(*(x)))

LOCALDEF struct anychan anychan;

#ifdef TING_APILIB_USE_POLL
#include <errno.h>
#include <sys/poll.h>

typedef struct pollfd WAITABLE;

#endif

#ifdef TING_APILIB_USE_WFMO

typedef HANDLE WAITABLE;

#endif

#ifdef TiNGTYPE_WINNT
static const unsigned max_waiter_items = MAX_WAITER_ITEMS;
#endif

LOCALDEF int anychan_init(void)
{
 mutx_init(&anychan.pm.mx);
 cv_init(&anychan.sawnew);
 make_object(&anychan);
 anychan.inuse = 0;
 anychan.status = 0;
 anychan.havewaiter = 0;
#ifdef TiNGTYPE_WINNT
 anychan.deadthread = INVALID_HANDLE_VALUE;
#endif
 return 0;
}

STATIC void inshead(struct chanev **listp, struct chanev *ce)
{
 if ((ce->next = *listp)) ce->next->prevp = &ce->next;
 *listp = ce;
 ce->prevp = listp;
 if (!*listp) {
 	die("inshead: item became null");
 }
}

STATIC void remlist(struct chanev *ce)
{
 if (*ce->prevp != ce) {
	die("remlist: corrupted list");
 }
 if (ce->next) ce->next->prevp = ce->prevp;
 *ce->prevp = ce->next;
 ce->prevp = &ce->next;
 ce->next = ce;
}

// holding anychan lock
// move channel to new state
//needs lock : anychan
LOCALDEF int anychan_newstate(struct chanev *ce, enum evstate newstate)
{
 if (ce->evstate == newstate) return 0;
 ce->evstate = newstate;
 remlist(ce);
 switch (ce->evstate) {
 case ANYEV_READY:
 case ANYEV_IDLE: return 0;
 case ANYEV_WAIT:
	 if (!ce->waitpos) {	// not in waitset - queue it
		inshead(&FIRSTWAITER->updates, ce);
		if (evnt_signal(FIRSTWAITER->kick)) {
			return anychan.status = err(ERR_SM_OS_OTHER_PROBLEM);
		}
	}
	break;
 case ANYEV_NOTANY:
	 if (ce->waitpos) {
			// in waitset - queue request to remove it
			// and wait for request to be done
		inshead(&FIRSTWAITER->updates, ce);
		if (evnt_signal(FIRSTWAITER->kick)) {
			return anychan.status = err(ERR_SM_OS_OTHER_PROBLEM);
		}
		while (ce->waitpos && ce->evstate == ANYEV_NOTANY) {
			int sts;
			prep_unlock_object(&anychan);
				// might get spurious wakeup here if someone else
				// is also doing this - we'll both be woken when
				// the waiter sees the first update
			sts = cv_wait(&anychan.sawnew, &anychan.pm.mx);
			prep_lock_object(&anychan);
			if (sts) {
				return anychan.status = err(ERR_SM_NO_RESOURCES);
			}
		}
	}
	break;
 }
 return 0;
}

#ifdef TING_APILIB_USE_POLL
// add channel to set of waiting channels
STATIC int waiter_add(struct chanev ***evmapp, WAITABLE **pfdp, unsigned *nwaitp, struct chanev *ce)
{
 unsigned nwait = *nwaitp + 1;
 struct chanev **evmap = alloc(*evmapp, sizeof(*evmap) * nwait);
 WAITABLE *pfd = alloc(*pfdp, sizeof(*pfd) * nwait);
 if (evmap) *evmapp = evmap;
 if (pfd) *pfdp = pfd;
 	// might have increased one and failed on the other
 	// This is harmless as it merely leaves some unused space at
 	// the end and the next adjustment will fix it.
 if (!evmap || !pfd) return err(ERR_SM_NO_RESOURCES);
 ce->waitpos = *nwaitp;	// pos for new is at end
 evmap[ce->waitpos] = ce;
 pfd[ce->waitpos].fd = ce->intev.fd;
 pfd[ce->waitpos].events = ce->intev.mode;
 *nwaitp = nwait;
 return 0;
}

// (permanently) delete channel from set of waiting channels
STATIC int waiter_del(struct chanev ***evmapp, WAITABLE **pfdp, unsigned *nwaitp, struct chanev *ce)
{
 unsigned nwait = *nwaitp - 1;
 struct chanev **evmap = *evmapp;
 WAITABLE *pfd = *pfdp;
 if (!ce->waitpos) {
 	die("waiter_del: already gone");		// already gone
 }
		// use last to fill hole left by removed item
 evmap[ce->waitpos] = evmap[nwait];
 evmap[nwait]->waitpos = ce->waitpos;
 pfd[ce->waitpos] = pfd[nwait];		// NB: copy revents also!
 *nwaitp = nwait;
 ce->waitpos = 0;
 		// channel has been deleted
 evmap = alloc(evmap, sizeof(*evmap) * nwait);
 pfd = alloc(pfd, sizeof(*pfd) * nwait);
 if (evmap) *evmapp = evmap;
 if (pfd) *pfdp = pfd;
 	// might have decreased one and failed on the other
 	// This is harmless as it merely leaves some unused space at
 	// the end and the next adjustment will fix it.
 return 0;
}
#endif

#ifdef TING_APILIB_USE_WFMO

STATIC DWORD WINAPI waiter_thread(void *p);

STATIC int waiter_split(struct waiter *me)
{
 struct waiter *slave = anychan.idle_slaves;
 if (slave) {
	*slave->prevp = slave->next;
	if (slave->next) slave->next->prevp = slave->prevp;
 } else {
	DWORD tid;
	slave = alloc(0, sizeof(*slave));
	if (!slave) return err(ERR_SM_NO_RESOURCES);
	if (evnt_create(&slave->kick)) {
		free(slave);
		if (TiNGtrace) print_lasterr("CreateEvent failed");
		return err(ERR_SM_NO_RESOURCES);
	}
	slave->updates = 0;
	slave->thr = CreateThread(0, 0, waiter_thread, slave, 0, &tid);
	if (!slave->thr) {
		if (TiNGtrace) print_lasterr("CreateThread failed");
		evnt_destroy(&slave->kick);
		free(slave);
		return err(ERR_SM_NO_RESOURCES);
	}
 }
 slave->next = me->next;
 if (slave->next) slave->next->prevp = &slave->next;
 me->next = slave;
 slave->prevp = &me->next;
 anychan.firstwaiter = slave;
 return 0;
}

// update slave's wait set - handles deleting a ready channel
STATIC int slave_upd(struct waiter *me, struct chanev **evmap, WAITABLE *pfd, unsigned *nwaitp)
{
 struct chanev *passhead = 0, **passtail = &passhead;
 int r = 0;
 /*
  * Actions required:
  *	NOTANY: free its slot in wait set
  *	IDLE: none
  *	READY: impossible
  *	WAIT: put into wait set
  */
 for (;;) {
	struct chanev *ce = me->updates;
	if (!ce) break;
	remlist(ce);
	if (ce->evstate == ANYEV_WAIT) {
		if (*nwaitp >= max_waiter_items) {
			*passtail = ce;
			ce->prevp = passtail;
			passtail = &ce->next;
		} else {
			if (ce->waitpos) {
				die("slave_upd: already in wait set");
			}
			if (!anychan.waitcap--) {
				die("slave_upd: waitcap < 0");
			}
			ce->slave = me;
			ce->waitpos = *nwaitp;	// pos for new is at end
			evmap[ce->waitpos] = ce;
			pfd[ce->waitpos] = ce->intev;
			if (++*nwaitp == max_waiter_items) {	// full
				anychan.firstwaiter = me->next;
			}
		}
	} else if (ce->evstate == ANYEV_NOTANY) {
		if (ce->slave != me) {
			*passtail = ce;
			ce->prevp = passtail;
			passtail = &ce->next;
		} else {
			const unsigned nwait = *nwaitp - 1;
			if (!ce->waitpos) {
				die("slave_upd: item vanished");
			}
				// use last to fill hole left by removed item
			evmap[ce->waitpos] = evmap[nwait];
			evmap[nwait]->waitpos = ce->waitpos;
			pfd[ce->waitpos] = pfd[nwait];
			*nwaitp = nwait;
			ce->waitpos = 0;
			anychan.waitcap++;
		}
	} else {
		olog("evstate = %d!\n", ce->evstate);
		die("slave_upd: bogus evstate");
	}
 }
 if (passhead) {
	struct chanev *old;
	if (!anychan.waitcap) {
		//if (passhead->evstate != ANYEV_WAIT) die("slave_upd: item to pass has bogus evstate");
		r = waiter_split(me);
		if (r) return r;
	}
	*passtail = old = me->next->updates;
	if (old) {
		old->prevp = passtail;
			// no need to kick, since it must already be pending
	} else {
		if (evnt_signal(me->next->kick)) r = 1;
	}
	me->next->updates = passhead;
	passhead->prevp = &me->next->updates;
 } else {
	cv_broadcast(&anychan.sawnew);
 }
 return r;
}

//needs lock : anychan
STATIC int main_slave(struct waiter *me)
{
 HANDLE waitset[MAX_WAITER_ITEMS];
 struct chanev *evmap[MAX_WAITER_ITEMS];
 unsigned nwait = 1;
 waitset[0] = evnt_to_listen(me->kick);
 evmap[0] = 0;
 for (;;) {
	unsigned readylist[MAX_WAITER_ITEMS];
	unsigned pos;
	DWORD item;
	if (anychan.status) break;
	if (me->updates) {
		if (slave_upd(me, evmap, waitset, &nwait)) return 1;
	}
	if (nwait == 1 && me != &anychan.waiter) return 0;
	unlock_object(&anychan);
	item = WaitForMultipleObjects(nwait, waitset, 0, INFINITE);
	pos = 0;
	for (;;) {
		unsigned ofs = item - WAIT_OBJECT_0 + 1;
		if (pos >= arlen(readylist)) abort();
		readylist[pos++] = item;
		if (ofs >= nwait) break;
		item = WaitForMultipleObjects(nwait - ofs, waitset + ofs, 0, 0);
		if (item < WAIT_OBJECT_0 || item >= WAIT_OBJECT_0+nwait) break;
		item += ofs;
	}
	lock_object(&anychan);
	for (; pos > 0;) {
		item = readylist[--pos];
		if (item == WAIT_OBJECT_0) {
			if (evnt_clear(me->kick)) {
				return anychan.status = err(ERR_SM_OS_OTHER_PROBLEM);
			}
		} else if (item > WAIT_OBJECT_0 && item < WAIT_OBJECT_0+nwait) {
			struct chanev *ce;
			item -= WAIT_OBJECT_0;
			ce = evmap[item];
			if (item != ce->waitpos) {
				die("main_slave: item woken wasn't in map");
			}
			nwait--;
			if (!ce->waitpos) {
				die("main_slave: item woken wasn't asleep");
			}
				// use last to fill hole left by removed item
			evmap[ce->waitpos] = evmap[nwait];
			evmap[nwait]->waitpos = ce->waitpos;
			waitset[ce->waitpos] = waitset[nwait];		// NB: copy revents also!
			ce->waitpos = 0;
			anychan.waitcap++;
			if (ce->evstate == ANYEV_WAIT) {
				anychan_newstate(ce, ANYEV_READY);
				inshead(&anychan.ev[ce->etyp].ready_new, ce);
				if (evnt_signal(anychan.ev[ce->etyp].event)) {
					anychan.status = err(ERR_SM_OS_OTHER_PROBLEM);
				}
				if (anychan.status) return 1;
			} else if (ce->evstate == ANYEV_NOTANY) {
				remlist(ce);		// remove it from our update list
				cv_broadcast(&anychan.sawnew);
			}
		} else {
			if (TiNGtrace) print_lasterr("WaitForMultipleObjects failed");
			anychan.status = err(ERR_SM_NO_RESOURCES);
			return 1;
		}
	 }
 }
 return 1;
}

//holds lock : anychan
STATIC DWORD WINAPI waiter_thread(void *p)
{
 struct waiter *me = p;
 HANDLE h;
 int r;
//#ifdef TiNGTYPE_WINNT
// if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL)) {
//         return error(error_last("SetThreadPriority() failed"),
//		 "Cannot increase priority of main thread");
// }
//#endif
 lock_object(&anychan);
 for (;;) {
	DWORD item;
		// I'm busy
		// do work
	anychan.waitcap += max_waiter_items - 1;
	r = main_slave(me);
	anychan.waitcap -= max_waiter_items - 1;
	if (anychan.firstwaiter == me) {
		anychan.firstwaiter = me->next;
	}
		// not busy any more
	me->next->prevp = me->prevp;
	*me->prevp = me->next;
	if (r) break;
		// now I'm idle
	me->next = anychan.idle_slaves;
	if (me->next) me->next->prevp = &me->next;
	me->prevp = &anychan.idle_slaves;
	anychan.idle_slaves = me;
	unlock_object(&anychan);
	item = WaitForSingleObject(evnt_to_listen(me->kick), 8000);	// up to 8 seconds
	lock_object(&anychan);
		// and now I'm not idle
	if (!me->updates) {
		if (me->next) me->next->prevp = me->prevp;
		*me->prevp = me->next;
		break;	// I'm told to exit or timed out
	}
 }
 h = anychan.deadthread;
 evnt_destroy(&me->kick);
 if (me == &anychan.waiter) {
	anychan.deadthread = INVALID_HANDLE_VALUE;
 } else {
	anychan.deadthread = me->thr;
 }
 unlock_object(&anychan);
 if (me != &anychan.waiter) free(me);
 if (h != INVALID_HANDLE_VALUE) {
	if (WaitForSingleObject(h, INFINITE) != WAIT_OBJECT_0) {
		if (TiNGtrace) print_lasterr("WaitForSingleObject(deadthread) failed");
		r = 1;
	}
	CloseHandle(h);
 }
 return r;
}

#endif

#ifdef TING_APILIB_USE_POLL
STATIC int waiter_upd(struct chanev ***evmapp, WAITABLE **pfdp, unsigned *nwaitp)
{
 /*
  * Actions required:
  *	NOTANY: free its slot in wait set
  *	IDLE: none
  *	READY: impossible
  *	WAIT: put into wait set
  */
 for (;;) {
	struct chanev *ce = FIRSTWAITER->updates;
	if (!ce) {
			// done everything
		cv_broadcast(&anychan.sawnew);
		return 0;
	}
	remlist(ce);
	if (ce->evstate == ANYEV_WAIT) {
		if (!ce->waitpos) {
			int e = waiter_add(evmapp, pfdp, nwaitp, ce);
			if (e) return e;
		}
		(*evmapp)[ce->waitpos] = ce;
		(*pfdp)[ce->waitpos].fd = ce->intev.fd;
		(*pfdp)[ce->waitpos].events = ce->intev.mode;
	} else if (ce->evstate == ANYEV_NOTANY) {
		int e = waiter_del(evmapp, pfdp, nwaitp, ce);
		if (e) return e;
	}
 }
}

STATIC void *main_waiter(struct chanev ***evmapp, WAITABLE **pfdp)
{
 unsigned nwait = 1;
 (*pfdp)[0].fd = evnt_to_listen(FIRSTWAITER->kick);
 (*pfdp)[0].events = POLLIN|POLLPRI;
 (*evmapp)[0] = 0;
 for (;;) {
	struct chanev *ce;
	int nready;
	unsigned u;
	if (anychan.status) break;
	anychan.status = waiter_upd(evmapp, pfdp, &nwait);
	if (anychan.status) break;
	unlock_object(&anychan);
	nready = poll(*pfdp, nwait, -1);
	lock_object(&anychan);
	if (nready < 0) {
		if (errno == EINTR) continue;
		anychan.status = err(ERR_SM_NO_RESOURCES);
		break;
	}
	if ((*pfdp)[0].revents) {
		if (evnt_clear(FIRSTWAITER->kick)) {
			anychan.status = err(ERR_SM_OS_OTHER_PROBLEM);
			break;
		}
		nready--;
	}
	for (u=1; nready; u++) {
		if ((*pfdp)[u].revents) {
			ce = (*evmapp)[u];
			if (u != ce->waitpos) {
				die("main_waiter: item woken wasn;t in map");
			}
			(*evmapp)[u] = (*evmapp)[--nwait];
			(*evmapp)[u]->waitpos = u;
			(*pfdp)[u] = (*pfdp)[nwait];
			ce->waitpos = 0;
			u--;
			if (ce->evstate == ANYEV_WAIT) {
				ce->evstate = ANYEV_READY;
				inshead(&anychan.ev[ce->etyp].ready_new, ce);
					// got something - signal event
				if (evnt_signal(anychan.ev[ce->etyp].event)) {
					anychan.status = err(ERR_SM_OS_OTHER_PROBLEM);
					break;
				}
			} else if (ce->evstate == ANYEV_NOTANY) {
				remlist(ce);		// remove it from our update list
				cv_broadcast(&anychan.sawnew);
			}
			nready--;
		}
	}
 }
 return 0;
}

STATIC void *waiter_thread(void *p)
{
 struct chanev **evmap;
 WAITABLE *pfd;
 (void) p;
 lock_object(&anychan);
 evmap = alloc(0, sizeof(*evmap));
 pfd = alloc(0, sizeof(*pfd));
 if (!pfd || !evmap) {
 	anychan.status = err(ERR_SM_NO_RESOURCES);
 } else main_waiter(&evmap, &pfd);
 free(pfd);
 free(evmap);
 unlock_object(&anychan);
 return 0;
}
#endif

STATIC int none_ready(enum event_types etyp)
{
	// now let waiter see all our updates
 // move anychan.processing to anychan.waiter.updates
 if (anychan.ev[etyp].processing) {
	for (;;) {
		struct chanev *ce = anychan.ev[etyp].processing;
		if (!ce) break;
		remlist(ce);
		if (ce->evstate == ANYEV_READY) {
			ce->evstate = ANYEV_WAIT;
			inshead(&anychan.waiter.updates, ce);
		} else die("none_ready: processed item wasn't ready");
	} 
 }
 if (anychan.waiter.updates) {
	if (signal_event(evnt_to_sig(anychan.waiter.kick))) {
		return err(ERR_SM_OS_OTHER_PROBLEM);
	}
 }
 if (!anychan.ev[etyp].ready_old) {
		// might be somthing in another category
	if (evnt_clear(anychan.ev[etyp].event)) {
		return err(ERR_SM_OS_OTHER_PROBLEM);
	}
 }
 return 0;
}

//holds lock : anychan
STATIC int check_anystatus(int *ready, enum event_types etyp, enum rxtxf typ, int (*ckfn)(int *ready, tSMChannelId cp, void *p), void *p)
{
 const int dir = etyp == EVENT_READ;
 struct chanev **prevp;
 int sts = 0;
 lock_object(&anychan);
 for (prevp = &anychan.ev[etyp].ready_old; !sts; ) {
	struct chanev *ce = *prevp;
	if (!ce) {
			// ready_old: 1 2 3 4 5
			// ready_new: 9 8 7 6
			// append reversed ready_new to ready_old while
			// keeping prevp at the same point (&5->next in this
			// example)
		for (;;) {
			ce = anychan.ev[etyp].ready_new;
			if (!ce) break;
			remlist(ce);
			inshead(prevp, ce);
		}
		if (!(ce = *prevp)) {
			*ready = 0;
			if (!anychan.ev[etyp].ready_old) sts = none_ready(etyp);
			break;
		}
	}
	lock_object(ce->chan);
	if (ce->chan->rxtx[dir].func == typ) {
		remlist(ce);
		inshead(&anychan.ev[etyp].processing, ce);
		sts = (*ckfn)(ready, ce->chan, p);
		if (*ready) {
			unlock_object(ce->chan);
			break;
		}
			// false alarm - put back to waiting
		sts = anychan_newstate(ce, ANYEV_WAIT); // not yet done
			// this chanev has gone, so *prevp points to
			// the next to consider
	} else if (ce->chan->rxtx[dir].func == RXTXF_NONE) {
		sts = anychan_newstate(ce, ANYEV_IDLE);
			// this chanev has gone, so *prevp points to
			// the next to consider
	} else {
			// skip this - it's ready, but unsuitable
		prevp = &ce->next;
	}
	unlock_object(ce->chan);
 }
 unlock_object(&anychan);
 return sts;
}

STATIC int ready_sm_play_tone_anystatus(int *ready, tSMChannelId cp, void *p)
{
 SM_PLAY_TONE_STATUS_PARMS *pp = p;
 int sts;
 pp->channel = cp;
 sts = unsafe_sm_play_tone_status(pp);
 *ready = sts || pp->status != kSMPlayToneStatusOngoing;
 return sts;
}

LOCALDEF int sm_play_tone_anystatus(SM_PLAY_TONE_STATUS_PARMS *pp)
{
 int ready;
 int sts = check_anystatus(&ready, EVENT_WRITE, RXTXF_GENSTONE, ready_sm_play_tone_anystatus, pp);
 if (!sts && !ready) {
	pp->channel = kSMNullChannelId;
	if (!sts) sts = ERR_SM_NO_SUCH_CHANNEL;
 }
 return sts;
}

STATIC int ready_sm_play_cptone_anystatus(int *ready, tSMChannelId cp, void *p)
{
 SM_PLAY_CPTONE_STATUS_PARMS *pp = p;
 int sts;
 pp->channel = cp;
 sts = unsafe_sm_play_cptone_status(pp);
 *ready = sts || pp->status != kSMPlayCPToneStatusOngoing;
 return sts;
}

LOCALDEF int sm_play_cptone_anystatus(SM_PLAY_CPTONE_STATUS_PARMS *pp)
{
 int ready;
 int sts = check_anystatus(&ready, EVENT_WRITE, RXTXF_GENCPTONE, ready_sm_play_cptone_anystatus, pp);
 if (!sts && !ready) {
	pp->channel = kSMNullChannelId;
	if (!sts) sts = ERR_SM_NO_SUCH_CHANNEL;
 }
 return sts;
}

STATIC int ready_sm_play_digits_anystatus(int *ready, tSMChannelId cp, void *p)
{
 SM_PLAY_DIGITS_STATUS_PARMS *pp = p;
 int sts;
 pp->channel = cp;
 sts = unsafe_sm_play_digits_status(pp);
 *ready = sts || pp->status != kSMPlayDigitsStatusOngoing;
 return sts;
}

LOCALDEF int sm_play_digits_anystatus(SM_PLAY_DIGITS_STATUS_PARMS *pp)
{
 int ready;
 int sts = check_anystatus(&ready, EVENT_WRITE, RXTXF_GENDTMF, ready_sm_play_digits_anystatus, pp);
 if (!sts && !ready) {
	pp->channel = kSMNullChannelId;
	if (!sts) sts = ERR_SM_NO_SUCH_CHANNEL;
 }
 return sts;
}

STATIC int ready_sm_replay_anystatus(int *ready, tSMChannelId cp, void *p)
{
 SM_REPLAY_STATUS_PARMS *pp = p;
 int sts;
 pp->channel = cp;
 sts = unsafe_sm_replay_status(pp);
 *ready = sts || pp->status != kSMReplayStatusNoCapacity;
 return sts;
}

LOCALDEF int sm_replay_anystatus(SM_REPLAY_STATUS_PARMS *pp)
{
 int ready;
 int sts = check_anystatus(&ready, EVENT_WRITE, RXTXF_PLAYREC, ready_sm_replay_anystatus, pp);
 if (!ready) {
	pp->channel = kSMNullChannelId;
	if (!sts) sts = ERR_SM_NO_SUCH_CHANNEL;
 }
 return sts;
}

STATIC int ready_sm_record_anystatus(int *ready, tSMChannelId cp, void *p)
{
 SM_RECORD_STATUS_PARMS *pp = p;
 int sts;
 pp->channel = cp;
 sts = unsafe_sm_record_status(pp);
 *ready = sts || pp->status != kSMRecordStatusNoData;
 return sts;
}

LOCALDEF int sm_record_anystatus(SM_RECORD_STATUS_PARMS *pp)
{
 int ready;
 int sts = check_anystatus(&ready, EVENT_READ, RXTXF_PLAYREC, ready_sm_record_anystatus, pp);
 if (!ready) {
	pp->channel = kSMNullChannelId;
	if (!sts) sts = ERR_SM_NO_SUCH_CHANNEL;
 }
 return sts;
}

//holds lock : anychan
LOCALDEF int sm_get_recognised_anystatus(SM_RECOGNISED_PARMS *pp)
{
 struct chanev **prevp;
 int sts = 0;
 lock_object(&anychan);
 for (prevp = &anychan.ev[EVENT_NOTIFY].ready_old; !sts; ) {
	struct chanev *ce = *prevp;
	if (!ce) {
			// ready_old: 1 2 3 4 5
			// ready_new: 9 8 7 6
			// append reversed ready_new to ready_old while
			// keeping prevp at the same point (&5->next in this
			// example)
		for (;;) {
			ce = anychan.ev[EVENT_NOTIFY].ready_new;
			if (!ce) break;
			remlist(ce);
			inshead(prevp, ce);
		}
		if (!(ce = *prevp)) {
			pp->channel = kSMNullChannelId;
			sts = none_ready(EVENT_NOTIFY);
			if (!sts) sts = ERR_SM_NO_SUCH_CHANNEL;
			break;
		}
	}
	lock_object(ce->chan);
	remlist(ce);
	inshead(&anychan.ev[EVENT_NOTIFY].processing, ce);
	pp->channel = ce->chan;
	sts = unsafe_sm_get_recognised(pp);
	if (sts || pp->type != kSMRecognisedNothing) {
		unlock_object(ce->chan);
		break;
	}
	if (ce->chan->recog[0].detmask) {
			// false alarm - put back to waiting
		sts = anychan_newstate(ce, ANYEV_WAIT); // not yet done
	} else {
		sts = anychan_newstate(ce, ANYEV_IDLE);
	}
	unlock_object(ce->chan);
 }
 unlock_object(&anychan);
 return sts;
}

#ifdef TiNGTYPE_WINNT
STATIC DWORD WINAPI winnt_waiter_thread(void *p)
{
 return (DWORD) waiter_thread(p);
}
#endif

STATIC int any_cleanup(enum event_types etyp)
{
 anychan.inuse &= ~(1 << etyp);
 // wait for FIRSTWAITER->updates == 0
 while (anychan.havewaiter && FIRSTWAITER->updates) {
	int sts;
	if (evnt_signal(FIRSTWAITER->kick)) {
		anychan.status = err(ERR_SM_OS_OTHER_PROBLEM);
		break;
	}
	prep_unlock_object(&anychan);
		// might get spurious wakeup here if someone else
		// is also doing this - we'll both be woken when
		// the waiter sees the first update
	sts = cv_wait(&anychan.sawnew, &anychan.pm.mx);
	prep_lock_object(&anychan);
	if (sts) anychan.status = sts;
 }
 if (!anychan.inuse) {		// tell waiter to exit (kick when updates = 0)
	if (evnt_signal(FIRSTWAITER->kick)) {
		anychan.status = err(ERR_SM_OS_OTHER_PROBLEM);
	}
 }
 /*
  * Note that we must release the anychan mutex here, since we're
  * about to wait for the waiter thread, which may need to acquire it.
  * However a sufficiently bizarre application might be trying to do
  * smd_ev_create() at the same time as smd_ev_free() and get in here
  * so we put a fake error in the status which will make any such
  * attempt fail with an error.
  */
 if (!anychan.status) anychan.status = ERR_SM_PENDING;
 unlock_object(&anychan);
 if (!anychan.inuse && anychan.havewaiter) {
#ifdef TING_APILIB_USE_POLL
	void *sts;
	pthread_join(FIRSTWAITER->tid, &sts);
	evnt_destroy(&FIRSTWAITER->kick);
#endif
#ifdef TiNGTYPE_WINNT
	if (WaitForSingleObject(FIRSTWAITER->thr, INFINITE) != WAIT_OBJECT_0) {
		if (TiNGtrace) print_lasterr("WaitForSingleObjects failed");
		anychan.status = err(ERR_SM_NO_RESOURCES);
		CloseHandle(FIRSTWAITER->thr);
 	}
#endif
	anychan.havewaiter = 0;
 }
 lock_object(&anychan);
 if (anychan.status == ERR_SM_PENDING) anychan.status = 0;
 evnt_destroy(&anychan.ev[etyp].event);
 return anychan.status;
}

LOCALDEF int unsafe_smd_ev_create_any(tSMEventId *ep, enum event_types etyp)
{
 int isfirst = !anychan.inuse;
 if (anychan.inuse & (1 << etyp)) return err(ERR_SM_NO_RESOURCES);
 anychan.inuse |= 1 << etyp;
 anychan.ev[etyp].ready_new = 0;
 anychan.ev[etyp].ready_old = 0;
 anychan.ev[etyp].processing = 0;
 if (evnt_create(&anychan.ev[etyp].event)) return err(ERR_SM_NO_RESOURCES);
 if (isfirst) {
#ifdef TING_APILIB_USE_POLL
	int e;
#endif
#ifdef TiNGTYPE_WINNT
	DWORD tid;
	anychan.waiter.next = &anychan.waiter;
	anychan.waiter.prevp = &anychan.waiter.next;
	anychan.firstwaiter = &anychan.waiter;
#endif
	anychan.waiter.updates = 0;
	if (evnt_create(&anychan.waiter.kick)) {
		any_cleanup(etyp);
		return err(ERR_SM_NO_RESOURCES);
	}
#ifdef TING_APILIB_USE_POLL
	e = pthread_create(&anychan.waiter.tid, 0, waiter_thread, 0);
	if (e) {
		if (TiNGtrace) print_errno(e, "pthread_create failed");
		any_cleanup(etyp);
		return err(ERR_SM_NO_RESOURCES);
 	}
#endif
#ifdef TiNGTYPE_WINNT
	anychan.waiter.thr = CreateThread(0, 0,
		winnt_waiter_thread, &anychan.waiter, 0, &tid);
	if (!anychan.waiter.thr) {
		if (TiNGtrace) print_lasterr("CreateThread failed");
		any_cleanup(etyp);
		return err(ERR_SM_NO_RESOURCES);
	}
	anychan.idle_slaves = 0;
	anychan.waitcap = 0;
#endif
 	anychan.havewaiter = 1;
 }
#ifdef TING_APILIB_USE_POLL
 ep->fd = evnt_to_listen(anychan.ev[etyp].event);
 ep->mode = POLLIN|POLLPRI;
#endif
#ifdef TiNGTYPE_WINNT
 (UINT_PTR)anychan.ev[etyp].event.evh |= ANYCHAN_EVENT_MASK;
 *ep = evnt_to_listen(anychan.ev[etyp].event);
#endif
 return 0;
}

LOCALDEF int smd_ev_create_any(tSMEventId *ep, enum event_types etyp)
{
 int sts;
 lock_object(&anychan);
 sts = unsafe_smd_ev_create_any(ep, etyp);
 unlock_object(&anychan);
 return sts;
}

//needs lock : anychan
LOCALDEF int smd_ev_free_any(tSMEventId ep)
{
 unsigned u;
 int sts;
 for (u=0; ; ) {
#ifdef TING_APILIB_USE_POLL
	if (ep.fd == evnt_to_listen(anychan.ev[u].event)) {
#endif
#ifdef TiNGTYPE_WINNT
	if (ep == evnt_to_listen(anychan.ev[u].event)) {
#endif
		sts = any_cleanup(u);
		break;
	}
	if (++u >= arlen(anychan.ev)) {
		sts = err(ERR_SM_BAD_PARAMETER);
		break;
	}
 }
 return sts;
}
