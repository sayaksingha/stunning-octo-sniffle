/* TiNGanyevent.h - 'any channel' event handling */

#ifndef INCLUDED_TINGANYEVENT_H
#define INCLUDED_TINGANYEVENT_H

#ifdef TiNGTYPE_LINUX
#define TING_APILIB_USE_POLL
#endif

#ifdef TiNGTYPE_QNX
#define TING_APILIB_USE_POLL
#endif


#ifdef TiNGTYPE_WINNT
#define TING_APILIB_USE_WFMO
#endif

#include "paranoia.h"
#include "condvar.h"
#include "event.h"
#ifdef TING_APILIB_USE_POLL
#include <pthread.h>
#endif
#ifdef TiNGTYPE_WINNT
#include "../libutil/WINNT/wind.h"
#endif

#include "TiNGo_chan.h"

/* FIXME: must sort out use of anychan.status: it's in danger of getting
 * overwritten
 */

/* There's actually only one of these structures, since the API functions
 * refer to it implicitly.
 */
struct anychan {
	PARAMUTX pm;
	int inuse;			// bitmask of global events in use
	int status;
	CV sawnew;			// waiter has seen all updates
	struct {
		EVNT event;		// the event itself
		struct chanev *ready_new;	// channels in READY state
		struct chanev *ready_old;	// channels in READY state
		struct chanev *processing;	// being processed by user
	} ev[EVENT_NUM];
	int havewaiter;
	struct waiter {
		EVNT kick;
#ifdef TING_APILIB_USE_POLL
		pthread_t tid;		// the waiter
#endif
#ifdef TiNGTYPE_WINNT
		HANDLE thr;
		struct waiter **prevp;
		struct waiter *next;
#endif
		struct chanev *updates;	// waiting for waiter to update
	} waiter;
#ifdef TING_APILIB_USE_POLL
#define FIRSTWAITER (&anychan.waiter)
#endif
#ifdef TiNGTYPE_WINNT
	struct waiter *firstwaiter;
	unsigned waitcap;
#define FIRSTWAITER (anychan.firstwaiter)
#endif
#ifdef TiNGTYPE_WINNT
	// Microsoft Windows cannot wait for more than 64 items, but even that
	// is inefficient. It's better to have more threads, each waiting for
	// fewer items. Measurement shows that 32 is a good number.
#define MAX_WAITER_ITEMS 64
	struct waiter *idle_slaves;
	HANDLE deadthread;
#endif
};

// The only 'anychan' structure
LOCALDEC struct anychan anychan;

	// constructor for 'anychan' object
LOCALDEC int anychan_init(void);

	 /*
	  * NB! NB! you must already hold the anychan lock when calling
	  * any of the functions below except where stated
	  */

	/* move a channel into a new state
	 * if the state is NOTANY, then on return the anychan subsystem
	 * retains no reference to the channel
	 */
LOCALDEC int anychan_newstate(struct chanev *ce, enum evstate newstate);

#include "smbesp.h"

	// find any channel with a play tone status report
LOCALDEC int sm_play_tone_anystatus(SM_PLAY_TONE_STATUS_PARMS *pp);
	// find any channel with a play cptone status report
LOCALDEC int sm_play_cptone_anystatus(SM_PLAY_CPTONE_STATUS_PARMS *pp);
	// find any channel with a play digits status report
LOCALDEC int sm_play_digits_anystatus(SM_PLAY_DIGITS_STATUS_PARMS *pp);
	// find any channel with a replay status report
LOCALDEC int sm_replay_anystatus(SM_REPLAY_STATUS_PARMS *pp);
	// find any channel with a record status report
LOCALDEC int sm_record_anystatus(SM_RECORD_STATUS_PARMS *pp);
	// find any channel with a detection status report
LOCALDEC int sm_get_recognised_anystatus(SM_RECOGNISED_PARMS *pp);
	// create any-channel event
LOCALDEC int unsafe_smd_ev_create_any(tSMEventId *ep, enum event_types etyp);
	// create any-channel event
	// you must NOT hold the anychan lock
LOCALDEC int smd_ev_create_any(tSMEventId *ep, enum event_types etyp);
	// free any-channel event
LOCALDEC int smd_ev_free_any(tSMEventId ep);

#endif
