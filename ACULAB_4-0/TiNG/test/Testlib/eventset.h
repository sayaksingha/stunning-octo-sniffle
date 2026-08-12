#ifndef INCLUDED_EVENTSET_H
#define INCLUDED_EVENTSET_H

#include <pthread.h>
#include <stdio.h>

#include "error.h"
#include "smdrvr.h"

/* see also the application note "Prosody application note: waiting on
 * multiple channels"
 */

typedef struct active_job {
	struct active_job *next, **prevnextp;
	tSMEventId event;
		/* in C++ these would be virtual functions */
		// handle a channel with an active event, must return
		// non-zero if the channel is to be deleted from the
		// list of active channels
	int (*handler)(struct active_job *ajp);
		// run when the channel is deleted from the list of
		// active channels. It could delete the channel or
		// put it into a pool of channels awaiting re-use.
	void (*cleanup_fn)(struct active_job *ajp);
		/* application-specific fields
		 * in C++ these would be in a class derived from
		 * this one
		 */
	struct appactive *mf;
} ACTIVE_JOB;

struct eventset;

typedef struct work_order {
	struct work_order *next;
	enum work_cmd {
		WCMD_ADD,
		WCMD_DEL,
	} cmd;
	ACTIVE_JOB *ajp;
} WORK_ORDER;

typedef struct eventset {
	pthread_mutex_t mx;		// protects stop, head and nextp
	int stop;			// stop when empty
	WORK_ORDER *head, **nextp;	// linked list of unprocessed requests
#ifdef TiNGTYPE_LINUX
	int changed[2];			// for waking the worker
#endif
#ifdef TiNGTYPE_QNX
	int changed[2];			// for waking the worker
#endif
#ifdef TiNGTYPE_WINNT
	HANDLE changed;			// for waking the worker
	unsigned capacity;	// workaround crippled WaitForMultipleObjects
#endif
} EVENTSET;

/*
 * constructor
 */
err_t eventset(EVENTSET *evs);

/*
 * main loop - run in worker thread to process work orders and wait
 * for events to happen to active channels
 */
err_t eventset_wait(EVENTSET *evs);

/*
 * called by manager thread - schedules a work order which will add
 * the active channel to this eventset.
 */
err_t eventset_insert(EVENTSET *evs, ACTIVE_JOB *ajp);

/*
 * called by manager thread - schedules a work order which will
 * remove the active channel from this eventset.
 */
err_t eventset_delete(EVENTSET *evs, ACTIVE_JOB *ajp);

/*
 * called by manager when worker thread should stop. Causes the
 * eventset_wait() called in the worker thread to return when the worker
 * becomes idle. The manager must wait until the worker has returned
 * from eventset_wait() before calling eventset_dtor().
 */
err_t eventset_stop(EVENTSET *evs);

/*
 * destructor
 */
void eventset_dtor(EVENTSET *evs);

#endif
