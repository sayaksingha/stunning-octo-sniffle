/* assp_group.c - group of ASSP connections running over same socket */

#include "assp_group.h"
#include "px_assp.h"
#include "px_util.h"
#include "../hsif/kernel.h"
#include "../hsif/kernel_x.h"
#include "mutx.h"
#include "trace.h"
#include "TiNGcommon.h"
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#ifdef TiNGTYPE_LINUX
#define TiNG_POSIX_SOCKETS
#define UNSIGNEDONE64BIT 1ull
#endif

#ifdef TiNGTYPE_QNX
#define TiNG_POSIX_SOCKETS
#define UNSIGNEDONE64BIT 1ull
#endif


#ifdef TiNG_POSIX_SOCKETS
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#ifndef closesocket
#define closesocket(fd) close(fd)
#endif
#define USE_POLL
#include "POSIX/socket.h"
#endif


#ifdef TiNGTYPE_WINNT
#define USE_WIN_EVENTS
typedef int socklen_t;
#define UNSIGNEDONE64BIT 1ui64
#endif

#define arlen(x) (sizeof(x)/sizeof(*(x)))

STATIC void queue_pkt_to_chan(ASSP_CHAN *chancx, PACKET *pkt)
{		// queue the packet to the channel
 *chancx->rxq_last = pkt;
 chancx->rxq_last = &pkt->next;
 pkt->next = 0;
 if (!chancx->ready_fn) {
	if (evnt_signal(chancx->cxevnt) && TiNGtrace) olog("failed to signal delivery evnt");
	cv_broadcast(&chancx->rxcv);
 } else {
	if ((*chancx->ready_fn)(chancx->ready_fn_context)) cv_broadcast(&chancx->rxcv);
 }
}

STATIC void notify_chans(ASSP_GROUP *grp)
{
 unsigned u;
 for (u=0; u < grp->numcxchan; u++) {
	ASSP_CHAN *chancx = grp->cxchan[u];
	if (chancx) {
		if (evnt_signal(chancx->cxevnt) && TiNGtrace) olog("failed to signal delivery evnt");
		cv_broadcast(&chancx->rxcv);
	}
 }
}

STATIC void do_deliver(ASSP_GROUP *grp, PACKET *pkt);

STATIC void *group_thread(void *arg)
{
 ASSP_GROUP *grp = arg;
 int i;
#ifdef USE_POLL
 struct pollfd pfds[1];
 pfds[0].fd = evnt_to_listen(*grp->conn_evnt);
 pfds[0].events = POLLIN;
#endif
 for (;;) {
#ifdef USE_POLL
	i = poll(pfds, 1, -1);
	if (i < 0) {
		if (errno == EINTR) continue;
		die("group_thread wait failed");
	}
#endif
#ifdef TiNGTYPE_WINNT
	HANDLE e;
	e = evnt_to_listen(*grp->conn_evnt);
	i = WaitForSingleObject(e, -1);
	if (i < WAIT_OBJECT_0 || i > WAIT_OBJECT_0) die("group_thread wait failed");
#endif
	lock_object(grp);
#ifdef TiNGTYPE_WINNT
	if (i == WAIT_OBJECT_0) {
#endif
#ifdef USE_POLL
	if (pfds[0].revents) {
#endif
		PACKET *pkt = assp_conn_recv(grp->conn);
		if (pkt) {
			do_deliver(grp, pkt);
		} else {
			grp->conn_status = assp_conn_status(grp->conn);
			if (grp->conn_status >= kAsspConnDisconnected) {
				notify_chans(grp);
			}
			if (grp->conn_status == kAsspConnFinished) {
				break;
			}
		}
	}
	unlock_object(grp);
 }
 grp->stop = kASSPGroupStopped;
 signal_event(evnt_to_sig(grp->stopped_event));
 unlock_object(grp);
 return 0;
}

STATIC void do_deliver(ASSP_GROUP *grp, PACKET *pkt)
{
 unsigned ncnx = 0;
 if (TiNGtrace & 8) dump_assp_pkt(grp->conn, pkt, "Rx");
 for (;;) {
 	unsigned char *sp = pkt->datastart;
 	uint32_t genid_l;
 	ASSP_CHAN *chancx;
 	uint32_t msg;
 	uint32_t id, pos;
 	if (sp == pkt->dataend) {
		assp_conn_free_packet(grp->conn, pkt);
 		return;
	}
	msg = getmsg(&sp);
	genid_l = MSG2TASK_GENID_L(grp->rx_extrabits, msg);
	id = MSG2TASK_ID_L(grp->rx_extrabits, msg);
	pos = id - 1;
	if (!(id & ((1 << TASK_ID_LOW_BITS)-1))) { //special case for the module handle
		if (MSG2CODE(msg) == MSG2CODE(MESSAGE(MSG_TASKID_EXTEND, 2))) {
				//we have an extend msg. Change our rx_extrabits
			unsigned char *cpsp = sp;
			grp->rx_extrabits = getmsg(&cpsp);
			if (TiNGtrace & 8) olog("rx_extrabits = 0x%x\n", grp->rx_extrabits);
		} else if (MSG2CODE(msg) == MSG2CODE(MESSAGE(MSG_SECKEYCHANGE, 12))) {
				// we have key change message
				// everything decoded after this will be with new key
			 int msgParmCount = 12-1;
			 unsigned char *cpsp = sp;
			 struct crypt_parms* crypt = (grp->conn)->crypt;

			 if (crypt) {
				PACKET *p;
				int    keyId;
				 // send keyid (le) plus key
				(crypt->encdec)(kSMSecCryptNewDKey,msgParmCount*sizeof(uint32_t),crypt->context,cpsp);
				keyId = getmsg(&cpsp);
				p = assp_conn_new_packet(grp->conn, 4*4);

				if (p) {
 					p->dataend = mkmsg(p->dataend, MESSAGE(MSG_SECKEYID, 3));
					p->dataend = mkmsg(p->dataend, keyId);
 					p->dataend = mkmsg(p->dataend, grp->conn->crypt->level);
		 			p->dataend = mkmsg(p->dataend, MESSAGE(MSG_PAD, 1));

					p->u.tx.changeover = 1;
					p->u.tx.mergeinhibit = 1;

					grp->conn->crypt->newKeyId = keyId;
					grp->conn->crypt->keyChangePending = 1;

					assp_conn_send(grp->conn, p);
				}

			    if (TiNGtrace & 8) olog("rx_seckeychange - %d\n",crypt->keyId);
			 } else {
			    if (TiNGtrace & 8) olog("rx_seckeychange - nocrypt\n");
			 }
		} else {
			if (TiNGtrace & 8) olog("Discarding msg for tid 0 : %08lx\n", msg);
		}
		pkt->datastart = msgitemend(msg, sp);	// discard the item
		
	} else if (pos >= grp->numcxchan || !(chancx = grp->cxchan[pos])){
			// not for a current channel
		if (TiNGtrace & 8) {
			olog("Discarding msg %08lx : id %x : numcxchan %d\n", msg, id, grp->numcxchan);
		}
		pkt->datastart = msgitemend(msg, sp);	// discard the item
	} else {
		if (MSG2ANYCODE(msg) == MSG_DEAD) {
			chancx->activegenmask &= ~(UNSIGNEDONE64BIT << MSG2TASK_GEN(msg));
			if (TiNGtrace > 5) olog("chancx %p(%u) gen %u is dead (mask %" PRIx64 ")\n", chancx, pos, MSG2TASK_GEN(msg), chancx->activegenmask);
		}
		if (TASK_GENID_L(chancx->taskgen, chancx->taskid) != genid_l || chancx->chanstate != CHAN_OPEN) {
				// not for a current channel
			if (TiNGtrace & 8) {
				olog("Discarding msg %08lx : id %x : taskgenid_l %d, state %d\n", msg, id, TASK_GENID_L(chancx->taskgen, chancx->taskid), chancx->chanstate);
			}
			pkt->datastart = msgitemend(msg, sp);	// discard the item
		} else if (chancx->discardmsgs) {
			if (TiNGtrace & 8) olog("Discarded msg %08lx : id %x\n", msg, id);
			pkt->datastart = msgitemend(msg, sp);	// discard the item
		} else {
			unsigned nlen;
			PACKET *npkt;
			ncnx++;
			for (;;) {	// check further items in this packet
				unsigned char *t;
				sp = msgitemend(msg, sp);
				if (sp >= pkt->dataend) {
					queue_pkt_to_chan(chancx, pkt);
					return;
				}
				t = sp;
				msg = getmsg(&t);
				if (MSG2TASK_GENID_L(grp->rx_extrabits, msg) != genid_l) {
					break;
				}
				if (MSG2ANYCODE(msg) == MSG_DEAD) {
					chancx->activegenmask &= ~(UNSIGNEDONE64BIT << MSG2TASK_GEN(msg));
					if (TiNGtrace > 5) olog("chancx %p(%u) gen %u is dead (mask %" PRIx64 ")\n", chancx, pos, MSG2TASK_GEN(msg), chancx->activegenmask);
				}
				sp = t;
			}
				// found an item for a different channel
				// copy remaining data into a new packet, and
				// start again with it
			nlen = pkt->dataend - sp;
			npkt = assp_conn_new_packet(grp->conn, nlen);
			if (!npkt) die("failed splitting rx packet\n");
			memcpy(npkt->rawstart, sp, nlen);
			npkt->datastart = npkt->rawstart;
			npkt->dataend = npkt->datastart + nlen;
			pkt->dataend = sp;
			queue_pkt_to_chan(chancx, pkt);
			pkt = npkt;
		}
	}
 }
}

#define MAX_CID (((1<<10)-1) - 1)
#define MAX_CHANPERCID ((1 << (TASK_ID_LOW_BITS + TASK_ID_EXTRA_BITS)) - 1)

LOCALDEF void task_nextgen(ASSP_CHAN *chancx)
{
 ++chancx->taskgen;
 chancx->taskgen &= (1 << TASK_GEN_BITS) - 1;
}

LOCALDEF int task_genisactive(ASSP_CHAN *chancx)
{
 return (chancx->activegenmask >> chancx->taskgen) & 1;
}

LOCALDEF void task_activegen(ASSP_CHAN *chancx)
{
 chancx->activegenmask |= UNSIGNEDONE64BIT << chancx->taskgen;
 if (TiNGtrace > 5) olog("chancx %p gen %u is active (mask %" PRIx64 ")\n", chancx, chancx->taskgen, chancx->activegenmask);
}

LOCALDEF ASSP_CHAN *channomagic2chan(ASSP_GROUP *grp, unsigned channo, unsigned magic)
{
 unsigned id = channo & ((1 << TASK_ID_END_L) - 1);
 unsigned cid = channo >> TASK_ID_END_L;
 ASSP_CHAN *chancx;
 if (cid != 0) return 0;
 if (--id >= grp->numcxchan) return 0;
 chancx = grp->cxchan[id];
 if (TASK_GENID_L(chancx->taskgen, chancx->taskid) != magic) return 0;
 return chancx;
}

STATIC ASSP_CHAN *alloc_chancx(ASSP_GROUP *grp)
{
 ASSP_CHAN *chancx;
 unsigned u;
 if (grp->stop) {
	if (TiNGtrace) olog("Stopping: alloc_chancx() refused\n");
 	return 0;
 }
 if (grp->conn_status > kAsspConnRunning) {
	if (TiNGtrace) olog("alloc_chancx(): connection not running\n");
 	return 0;
 }
 for (u=0; ; u++) {
	if (u >= grp->numcxchan) {
		unsigned newnum = 2 * grp->numcxchan + 1;
		ASSP_CHAN **n;
		if (grp->numcxchan == MAX_CHANPERCID) {
			if (TiNGtrace) olog("out of TASKIDs: %u in use\n", grp->numcxchan);
			return 0;
		}
		n = alloc(grp->cxchan, newnum * sizeof(*n));
		if (!n) return 0;
		grp->cxchan = n;
		if (newnum > MAX_CHANPERCID) {
			grp->numcxchan = MAX_CHANPERCID;
		} else {
			grp->numcxchan = newnum;
		}
		do n[u++] = 0; while (u < grp->numcxchan);
		u--;	// points to last
		break;
	}
		//Special case for task 0. No task may have the bottom 10
		//bits clear as this is a reserved pattern in task ids
	if ((u+1) & ((1 << TASK_ID_LOW_BITS)-1)) {
		chancx = grp->cxchan[u];
		if (!chancx) break;
		if (chancx->chanstate == CHAN_CLOSED) {
			if (task_genisactive(chancx)) {
				if (TiNGtrace > 5) olog("alloc_chancx() %p(%u) gen %u is active (mask %" PRIx64 ")\n", chancx, u, chancx->taskgen, chancx->activegenmask);
			} else {
				if (TiNGtrace > 5) olog("alloc_chancx() == %p(%u)\n", chancx, u);
				chancx->chanstate = CHAN_OPEN;
				chancx->discardmsgs = 0;
				chancx->ready_fn = 0;
				return chancx;
			}
		}
	}
 }
 grp->cxchan[u] = chancx = alloc(0, sizeof(*grp->cxchan[u]));
 if (!chancx) return 0;
 chancx->chanstate = CHAN_OPEN;
 chancx->grp = grp;
 chancx->rxq = 0;
 chancx->rxq_last = &chancx->rxq;
 cv_init(&chancx->rxcv);
 evnt(&chancx->cxevnt);
 chancx->taskgen = 0;
 chancx->activegenmask = 0;
 chancx->taskid = u + 1;
 chancx->discardmsgs = 0;
 chancx->ready_fn = 0;
 if (TiNGtrace > 5) olog("alloc_chancx() == %p(%u)\n", chancx, u);
 return chancx;
}



LOCALDEF void kickoffgroup(ASSP_GROUP *grp)
{
 if (grp->stop == kASSPGroupPending) {
	grp->stop = kASSPGroupRunning;
	assp_conn_start(grp->conn);
 }
}

LOCALDEF ASSP_CHAN *alloc_chan(ASSP_GROUP *grp)
{
 kickoffgroup(grp);
 return alloc_chancx(grp);
}

LOCALDEF void assp_chan_close(ASSP_CHAN *chancx)
{
 ASSP_GROUP *grp = chancx->grp;
 chancx->chanstate = CHAN_CLOSED;
 evnt_destroy(&chancx->cxevnt);
 evnt(&chancx->cxevnt);
 for (;;) {
 	PACKET *pkt = chancx->rxq;
 	if (!pkt) break;
	chancx->rxq = pkt->next;
	assp_conn_free_packet(grp->conn, pkt);
 }
 chancx->rxq_last = &chancx->rxq;
 chancx->discardmsgs = 0;
 if (TiNGtrace > 5) olog("assp_chan_close(%p) active mask %" PRIx64 "\n", chancx, chancx->activegenmask);
}

STATIC void assp_group_close(ASSP_GROUP *grp)
{
 unsigned u;
 for (u=0; u < grp->numcxchan; u++) {
 	ASSP_CHAN *chancx = grp->cxchan[u];
 	if (chancx) {
		if (chancx->chanstate != CHAN_CLOSED) {
			die("assp_group_close: chan open");
 		}
		cv_dtor(&chancx->rxcv);
 		free(chancx);
	}
 }
 free(grp->cxchan);
 grp->cxchan = 0;
 grp->numcxchan = 0;
}

LOCALDEF int asspgroup_ctor(ASSP_GROUP *grp, char *remaddr, char *asspmonaddr, int asspmonport, int port, char *key)
{
 int e;
 grp->tx_extrabits = 0;
 grp->rx_extrabits = 0;
 grp->numcxchan = 0;
 grp->cxchan = 0;
 grp->conn = new_conn();
 if (!grp->conn) {
	if (TiNGtrace > 0) olog("Cannot create assp conn\n");
 	return 1;
 }
 if (assp_conn_ctor(grp->conn, remaddr, asspmonaddr, asspmonport, port, key)) {
	if (TiNGtrace > 0) olog("Cannot init assp conn\n");
 	return 1;
 }
 if (evnt_create(&grp->stopped_event)) {
	if (TiNGtrace > 0) olog("Cannot init stopped event\n");
	assp_conn_shutdown(grp->conn);
	assp_conn_dtor(grp->conn);
	grp->conn = 0;
 	return 1;
 }
 
 grp->conn_evnt = assp_conn_get_event(grp->conn);
 grp->conn_status = assp_conn_status(grp->conn);
 grp->stop = kASSPGroupPending;
 mutx_init(&grp->pm.mx);
 make_object(grp);
 e = pthread_create(&grp->tid, 0, group_thread, grp);
 if (e) {
 	if (TiNGtrace > 0) olog("pthread_create() failed: %d\n", e);
	evnt_destroy(&grp->stopped_event);
	mutx_dtor(&grp->pm.mx);
	assp_conn_shutdown(grp->conn);
	assp_conn_dtor(grp->conn);
	grp->conn = 0;
 	return 1;
 }
 return 0;
}

LOCALDEF void asspgroup_shutdown(ASSP_GROUP *grp)
{
 lock_object(grp);
 assp_conn_shutdown(grp->conn);
 grp->stop = kASSPGroupStopping;
 unlock_object(grp);
}

LOCALDEF void asspgroup_dtor(ASSP_GROUP *grp)
{
 void *sts;
 int e;
 lock_object(grp);
 assp_conn_shutdown(grp->conn);
 grp->stop = kASSPGroupStopping;
 unlock_object(grp);
 e = pthread_join(grp->tid, &sts);
 assp_group_close(grp);
 assp_conn_dtor(grp->conn);
 evnt_destroy(&grp->stopped_event);
 mutx_dtor(&grp->pm.mx);
}

LOCALDEF int asspgroup_isconnected(ASSP_GROUP *grp)
{
 if (grp->stop == kASSPGroupPending) return 0;
	// note we must say we're connected when the conn or assp threads
	// haven't been able to run yet
 return grp->conn_status <= kAsspConnRunning;
}
