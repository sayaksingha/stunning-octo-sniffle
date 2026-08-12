#include "smdrvr.h"

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef TiNGTYPE_LINUX
#include <poll.h>
#include <unistd.h>
#define USE_POLL
#define USE_PIPE
#endif

#ifdef TiNGTYPE_QNX
#include <sys/poll.h>
#include <unistd.h>
#define USE_POLL
#define USE_PIPE
#endif


#include "error.h"
#include "eventset.h"

/* see also the application note "Prosody application note: waiting on
 * multiple channels"
 */

/*
 * To wake up the worker we use a pipe on Unix style systems. A write to
 * the pipe wakes the worker, who reads from the pipe before waiting
 * again.
 * On Windows we use an event. Since it is an auto-reset event, the
 * worker doesn't need to do anything to reset it before waiting
 * again.
 */
err_t eventset(EVENTSET *evs)
{
 err_t err = 0;
 int e = pthread_mutex_init(&evs->mx, 0);
 if (e) {
	err = error_errno(e, "pthread_mutex_init() failed");
 } else {
#ifdef USE_PIPE
	if (pipe(evs->changed)) {
		err = error_errno(errno, "pipe() failed");
	}
#endif
#ifdef TiNGTYPE_WINNT
	evs->changed = CreateEvent(0, 0, 0, 0);
	if (!evs->changed) {
		err = error_last("CreateEvent() failed");
	}
	evs->capacity = 63;		// 64, less one for event
#endif
	evs->head = 0;
	evs->stop = 0;
	evs->nextp = &evs->head;
	if (err) pthread_mutex_destroy(&evs->mx);
 }
 if (err) err = error(err, "cannot create eventset");
 return err;
}

static void remove_job(ACTIVE_JOB *ajp)
{
 	// unlink from list
 *ajp->prevnextp = ajp->next;
 if (ajp->next) ajp->next->prevnextp = ajp->prevnextp;
}

err_t eventset_wait(EVENTSET *evs)
{
 ACTIVE_JOB *chanlist = 0;		// the list of channels we are handling
 /* The number of items to wait for. This is used to allocate a big
  * enough list and may be an over-estimate after a deletion
  */
 unsigned nwait = 1;
 err_t err = 0;
 for (;;) {
 	// loop used when changes to list may have happened
#ifdef USE_POLL
		// first, build list of things to wait for
	struct pollfd *waitfor = malloc(nwait * sizeof(*waitfor));
	if (!waitfor) {
		err = error_errno(errno, "malloc() failed");
		err = error(err, "cannot make event list");
		break;
	}
	{	// prepare waiting list of channels
	 ACTIVE_JOB *cp;
	 int i;
		// add the pipe as the first element
	 waitfor[0].fd = evs->changed[0];
	 waitfor[0].events = POLLIN;
	 for (i=1, cp=chanlist; cp; i++, cp=cp->next) {
		waitfor[i].fd = cp->event.fd;
		waitfor[i].events = cp->event.mode;
	 }
	}
	for (;;) {
			// loop using same list until it changes
		int changed = 0;
		int n = poll(waitfor, nwait, -1);
		if (n == -1) {
			if (errno == EINTR) continue;
			err = error_errno(errno, "poll() failed");
			break;
		}
		{	// find signalled channels
		ACTIVE_JOB *cp;
		int i;
		for (i=1, cp=chanlist; n && cp; i++) {
			ACTIVE_JOB *next = cp->next; // save next as cp may be removed (and possibly freed)
			if (waitfor[i].revents) {
				if (cp->handler(cp)) {
					remove_job(cp);
					cp->cleanup_fn(cp);
					changed = 1;
					nwait--;
				}
				n--;
			}
			cp = next;
		}
			// FIXME: do not assert !n || i == nwait - 1
		}
		if (n && waitfor[0].revents) {
				// pipe indicates a work order is waiting
			char c;
			int i = read(evs->changed[0], &c, 1);
			if (i == -1) {
				err = error_errno(errno, "read() failed");
				break;
			}
			if (!i) {
				err = error(0, "read() got EOF");
				break;
			}
			break;
		}
		if (changed) break;
	}
#endif
#ifdef TiNGTYPE_WINNT
	/* Of course the Windows version is crippled by the inability
	 * of WaitForMultipleObjects to handle more than 64 items.
	 * This means you'll need a worker for every two trunks
	 * (E1 = 60 channels + the extra event = 61),
	 * (T1 = 48 channels + the extra event = 49).
	 */
		// first, build list of things to wait for
	HANDLE *waitfor = malloc(nwait * sizeof(*waitfor));
		/* mapping to get quickly from position of handle in
		 * 'waitfor' to the channel itself.
		 * waitfor[i] == chanmap[i]->event
		 */
	ACTIVE_JOB **chanmap = malloc(nwait * sizeof(*chanmap));
	if (!waitfor) {
		err = error(0, "malloc() failed");
		err = error(err, "cannot make event list");
		break;
	}
	{	// prepare waiting list of channels
	ACTIVE_JOB *wp;
	int i;
		// add the event first
	waitfor[0] = evs->changed;
	for (i=1, wp=chanlist; wp; i++, wp=wp->next) {
		waitfor[i] = wp->event;
		chanmap[i] = wp;
	}
	}
	for (;;) {
		DWORD n = WaitForMultipleObjects(nwait, waitfor, 0, INFINITE);
		if (n > WAIT_OBJECT_0 && n <= WAIT_OBJECT_0 + nwait - 1) {
			int i = n - WAIT_OBJECT_0;
			if (chanmap[i]->handler(chanmap[i])) {
					int e;
					remove_job(chanmap[i]);
					chanmap[i]->cleanup_fn(chanmap[i]);
					e = pthread_mutex_lock(&evs->mx);
					if (e) {
						err = error_errno(e, "pthread_mutex_lock() failed");
						break;
					}
					evs->capacity++;
					e = pthread_mutex_unlock(&evs->mx);
					if (e) {
						err = error_errno(e, "pthread_mutex_unlock() failed");
						break;
					}
					nwait--; 
					break;
			}
#if 0
			if (i < nwait - 2) {	// not last
				// swap with last to avoid starvation
				ACTIVE_JOB *c = chanmap[i];
				HANDLE *w;
				chanmap[i] = chanmap[nwait - 2];
				chanmap[nwait - 2] = c;
				w = waitfor[i];
				waitfor[i] = waitfor[nwait - 2];
				waitfor[nwait - 2] = w;
			}
#endif
		} else if (n == WAIT_OBJECT_0) { //first element is used to signal now
				// a work order is waiting
			break;
		} else {
			err = error_last("WaitForMultipleObjects() failed");
			break;
		}
	}
	free(chanmap);
#endif
		// free the list as we'll make a new one after any
		// changes are made
	free(waitfor);
	if (err) break;
	for (;;) {	// loop over the work orders
		WORK_ORDER *work;
		int stop;
		int e = pthread_mutex_lock(&evs->mx);
		if (e) {
			err = error_errno(e, "pthread_mutex_lock() failed");
			break;
		}
		work = evs->head;
		if (work) {	// remove it from the list
			evs->head = work->next;
			if (!evs->head) evs->nextp = &evs->head;
		}
		stop = evs->stop;
		e = pthread_mutex_unlock(&evs->mx);
		if (e) {
			err = error_errno(e, "pthread_mutex_unlock() failed");
			break;
		}
		if (!work) {
			if (!chanlist && stop) return 0;
			break;
		}
		if (work->cmd == WCMD_ADD) {
			ACTIVE_JOB *cp = work->ajp;
			cp->next = chanlist;
			if (chanlist) chanlist->prevnextp = &cp->next;
			cp->prevnextp = &chanlist;
			chanlist = cp;
			nwait++;
		} else if (work->cmd == WCMD_DEL) {
			ACTIVE_JOB *cp = work->ajp;
			remove_job(cp);
			cp->cleanup_fn(cp);
			nwait--; 
		} else {
			err = error(0, "internal error: bogus work item on queue");
			break;
		}
		free(work);
	}
	if (err) break;
 }
 	/* we get here only on error.
 	 * Maybe we should do something here to the list of channels?
 	 */
 return error(err, "eventset_wait() failed");
}

static err_t eventset_insdel(EVENTSET *evs, ACTIVE_JOB *ajp, enum work_cmd cmd)
{
 WORK_ORDER *w = malloc(sizeof(*w));
 err_t err = 0;
 int e;
#ifdef USE_POLL
 char c = '*';			// any random junk will do
 int n;
#endif
 if (!w) {
	err = error_errno(errno, "malloc() failed");
	err = error(err, "cannot make event list");
 } else {
	w->cmd = cmd;
	w->ajp = ajp;
	w->next = 0;
	e = pthread_mutex_lock(&evs->mx);
	if (e) {
		err = error_errno(e, "pthread_mutex_lock() failed");
	} else {
#ifdef TiNGTYPE_WINNT
		if (cmd == WCMD_DEL) evs->capacity++;
		else if (!evs->capacity) {
			err = error(0, "Exceeded event list capacity");
		} else evs->capacity--;
#endif
			// link it onto the list
		if (!err) {
			*evs->nextp = w;
			evs->nextp = &w->next;
		}
		e = pthread_mutex_unlock(&evs->mx);
		if (e) {
			err_t nerr = error_errno(e, "pthread_mutex_unlock() failed");
			if (!err) err = nerr;
			else err = error_join(err, nerr);
		} else {
#ifdef USE_POLL
			n = write(evs->changed[1], &c, 1);
			if (n < 0) {
				err = error_errno(errno, "erite() failed");
			} else if (!n) {
				err = error(0, "erite() wrote nothing");
			}
#endif
#ifdef TiNGTYPE_WINNT
			if (!SetEvent(evs->changed)) {
				err = error_last("SetEvent() failed");
			}
#endif
		}
	}
 }
 if (err) err = error(err, "eventset_insdel cannot queue work item");
 return err;
}

err_t eventset_insert(EVENTSET *evs, ACTIVE_JOB *ajp)
{
 return eventset_insdel(evs, ajp, WCMD_ADD);
}

err_t eventset_delete(EVENTSET *evs, ACTIVE_JOB *ajp)
{
 return eventset_insdel(evs, ajp, WCMD_DEL);
}

err_t eventset_stop(EVENTSET *evs)
{
 err_t err = 0;
 int e;
#ifdef USE_POLL
 char c = 'X';			// any random junk will do
 int n;
#endif
 e = pthread_mutex_lock(&evs->mx);
 if (e) {
	err = error_errno(e, "pthread_mutex_lock() failed");
 } else {
	evs->stop = 1;
	e = pthread_mutex_unlock(&evs->mx);
	if (e) {
		err_t nerr = error_errno(e, "pthread_mutex_unlock() failed");
		if (!err) err = nerr;
		else err = error_join(err, nerr);
	} else {
#ifdef USE_POLL
		n = write(evs->changed[1], &c, 1);
		if (n < 0) {
			err = error_errno(errno, "write() failed");
		} else if (!n) {
			err = error(0, "write() wrote nothing");
		}
#endif
#ifdef TiNGTYPE_WINNT
		if (!SetEvent(evs->changed)) {
			err = error_last("SetEvent() failed");
		}
#endif
	}
 }
 return err;
}

void eventset_dtor(EVENTSET *evs)
{
 int e;
 if (evs->head) {
	fprintf(stderr, "eventset_dtor: work queue not empty\n");
	abort();
 }
 e = pthread_mutex_destroy(&evs->mx);
 if (e) {
	fprintf(stderr, "eventset_dtor: pthread_mutex_destroy() returned %d\n", e);
	abort();
 }
#ifdef USE_PIPE
 if (close(evs->changed[0])) {
	fprintf(stderr, "eventset_dtor: close(pipe[0]) failed, errno=%d\n", errno);
	abort();
 }
 if (close(evs->changed[1])) {
	fprintf(stderr, "eventset_dtor: close(pipe[1]) failed, errno=%d\n", errno);
	abort();
 }
#endif
#ifdef TiNGTYPE_WINNT
 if (!CloseHandle(evs->changed)) {
	fprintf(stderr, "eventset_dtor: CloseHandle() failed, GetLastError() = %d\n", GetLastError());
	abort();
 }
#endif
}
