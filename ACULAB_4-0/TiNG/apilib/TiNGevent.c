/* TiNGevent.c - event handling */

#include "TiNGevent.h"

#ifdef TiNG_API_EVENTS_ARE_POSIX_PIPES
#include <sys/poll.h>
#include <errno.h>
#endif

#ifndef OMIT_ANYCHAN
#include "TiNGanyevent.h"
#endif
#include "TiNGcommon.h"
#include "trace.h"

#ifdef TiNG_USE_EVENT_REGISTRY

/* On some operating systems an 'event' has two parts: one end is
 * used to signal the event and the other is used to listen for the
 * event. Since tSMEventId is *not* opaque, we have no space to store
 * the other end of the event in it. Instead we keep a registry of
 * events which can be searched when necessary. As an optimisation,
 * we note that we can store the whole thing in the channel structure
 * while the event is associated with a channel. Then an event is
 * only in the registry between smd_ev_create() and using it with
 * sm_channel_set_event() and between stopping using it with
 * sm_channel_set_event() and smd_ev_free(). This may often result in
 * at most one event in the registry at any time.
 */

struct {
	PARAMUTX pm;
	struct remevent {
		struct remevent *next, **prev;
		EVNT evnt;
	} *evlist;
} remevents;

STATIC struct remevent *findrem_evnt(SIG_EVNT se)
{
 struct remevent *rep;
 lock_object(&remevents);
 for (rep=remevents.evlist; rep; rep = rep->next) {
	if (evnt_to_sig(rep->evnt) == se) {
		*rep->prev = rep->next;
		if (rep->next) rep->next->prev = rep->prev;
		break;
	}
 }
 unlock_object(&remevents);
 return rep;
}

STATIC void register_evnt(struct remevent *rep)
{
 lock_object(&remevents);
 rep->next = remevents.evlist;
 if (rep->next) rep->next->prev = &rep->next;
 remevents.evlist = rep;
 rep->prev = &remevents.evlist;
 unlock_object(&remevents);
}

STATIC int chan_remote_smd_ev_create(tSMEventId *ep, tSMChannelId chan)
{
 struct remevent *rep = alloc(0, sizeof(*rep));
 (void) ep;
 if (!rep) return err(ERR_SM_NO_RESOURCES);
 if (evnt_create(&chan->recog_evnt)) {
	free(rep);
	return err(ERR_SM_NO_RESOURCES);
 }
 rep->evnt = chan->recog_evnt;
 register_evnt(rep);
 ep->fd = evnt_to_listen(chan->recog_evnt);
 ep->mode = POLLIN;
 return 0;
}
#endif

LOCALDEF int real_smd_ev_create(tSMEventId *ep, tSMChannelId chan, int evtyp, int evbind)
{
 enum event_types etyp;
 int ni = 0;
 switch (evtyp) {
 case kSMEventTypeReadData:
 	etyp = EVENT_READ;
 	break;
 case kSMEventTypeWriteData:
 	etyp = EVENT_WRITE;
 	break;
 case kSMEventTypeRecog:
 	etyp = EVENT_NOTIFY;
 	break;
 default:
	return err(ERR_SM_BAD_PARAMETER);
 }
#ifdef TiNG_API_EVENTS_ARE_POSIX_PIPES
 ep->eip = alloc(0, sizeof(struct tSMEventId_internals));
 if (!ep->eip) return err(ERR_SM_NO_RESOURCES);
 ep->eip->evbind = evbind;
#endif
 switch (evbind) {
 	int sts;
 case kSMChannelSpecificEventNonIdle:
	ni = 1;
		// fall in ....
 case kSMChannelSpecificEvent:
	 if (!chan) {
#ifdef TiNG_API_EVENTS_ARE_POSIX_PIPES
		 free(ep->eip);
		 ep->eip = NULL;
#endif
		 return err(ERR_SM_NO_SUCH_CHANNEL);
	 }
	lock_object(chan);
			// watch out for that else!!!
		sts = chan->mod->card->vtbl->ev_create(ep, chan, etyp, ni);
	chan->event[etyp].nonidle = ni;
	unlock_object(chan);
	if (sts) {
#ifdef TiNG_API_EVENTS_ARE_POSIX_PIPES
		 free(ep->eip);
		 ep->eip = NULL;
#endif
	}
	return sts;
 case kSMAnyChannelEvent:
#ifndef OMIT_ANYCHAN
	sts = smd_ev_create_any(ep, etyp);
	if (sts) {
#ifdef TiNG_API_EVENTS_ARE_POSIX_PIPES
		 free(ep->eip);
		 ep->eip = NULL;
#endif
	}
	return sts;
#else
#ifdef TiNG_API_EVENTS_ARE_POSIX_PIPES
	 free(ep->eip);
	 ep->eip = NULL;
#endif
	return err(ERR_SM_NOT_IMPLEMENTED);
#endif
 default:
	return err(ERR_SM_BAD_PARAMETER);
 }
}

LOCALDEF int real_smd_ev_wait(tSMEventId event)
{
#ifdef TiNGTYPE_WINNT
 int sts = WaitForSingleObject(event, INFINITE);
 if (TiNGtrace > 1) olog("event(%d)", sts);
 if (sts != WAIT_OBJECT_0) return err(ERR_SM_DEVERR);
#endif
#ifdef TiNG_API_EVENTS_ARE_POSIX_PIPES
 struct pollfd pfd;
 pfd.fd = event.fd;
 pfd.events = event.mode;
 while (poll(&pfd, 1, -1) < 0) {
	if (errno != EINTR) return err(ERR_SM_DEVERR);
 }
 if (TiNGtrace > 1) olog("event(%d)", pfd.revents);
#endif
 return 0;
}

//holds lock : anychan
LOCALDEF int real_smd_ev_free(tSMEventId event)
{
#ifdef TiNG_API_EVENTS_ARE_POSIX_PIPES
 if (!event.eip) return 0;
#ifndef OMIT_ANYCHAN
 if (event.eip->evbind == kSMAnyChannelEvent) {
	int sts;
	lock_object(&anychan);
	sts = smd_ev_free_any(event);
	unlock_object(&anychan);
	free(event.eip);
	return sts;
 }
#endif
#ifdef TiNG_USE_EVENT_REGISTRY
 {
  struct remevent *rep = findrem_evnt(event.fd);
  if (rep) {	// it's remote
	evnt_destroy(&rep->evnt);
	free(rep);
	return 0;
  }
 }
#endif
 event.eip->smd_ev_free(event);
 free(event.eip);
#endif
#ifdef TiNGTYPE_WINNT
 if (event == (HANDLE) -1) return 0;
#ifndef OMIT_ANYCHAN
 if ((UINT_PTR) event & ANYCHAN_EVENT_MASK) {
	int sts;
	lock_object(&anychan);
	sts = smd_ev_free_any(event);
	unlock_object(&anychan);
	return sts;
 }
#endif
 {
	if (!CloseHandle(event)) {
		ERRCODE e;
		e.text = "CloseHandle failed";
		e.errnum = GetLastError();
		if (TiNGtrace) printerr(e);
		return err(ERR_SM_DEVERR);
	}
 }
#endif
 return 0;
}
