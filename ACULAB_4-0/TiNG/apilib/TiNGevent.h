#ifndef INCLUDED_TINGEVENT_H
#define INCLUDED_TINGEVENT_H

#include "smbesp.h"
#include "TiNGo_chan.h"

LOCALDEC int real_smd_ev_create(tSMEventId *ep, tSMChannelId chan, int evtyp, int evbind);
LOCALDEC int real_smd_ev_wait(tSMEventId event);
LOCALDEC int real_smd_ev_free(tSMEventId event);

#ifdef TiNGTYPE_LINUX
#define TiNG_API_EVENTS_ARE_POSIX_PIPES
#endif

#ifdef TiNGTYPE_QNX
#define TiNG_API_EVENTS_ARE_POSIX_PIPES
#endif


#ifdef TiNG_API_EVENTS_ARE_POSIX_PIPES
struct tSMEventId_internals {
	tSM_INT evbind;
	int (*smd_ev_free)(tSMEventId ep);
	void *vtbl_data;
};
#endif

#endif
