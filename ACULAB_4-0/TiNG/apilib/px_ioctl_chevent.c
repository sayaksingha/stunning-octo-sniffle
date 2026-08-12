/* px_ioctl_chevent.c - ioctl code for read/write channel event */

#include "px_ioctl_chevent.h"
#include "TiNGcommon.h"
#include "px_event.h"
#include "trace.h"

LOCALDEF int px_rdchevent(PX_CHAN *pxchan, struct ting_chevent *p)
{
 (void) pxchan;
 (void) p;
 if (TiNGtrace) olog("rdchevent: not implemented\n");
 return 1;
}

LOCALDEF int px_wrchevent(PX_CHAN *pxchan, struct ting_chevent *p)
{
 ASSP_GROUP *grp = pxchan->chancx->grp;
 int sts;
 EVNT ev;
#ifdef TiNG_EVENTS_ARE_POSIX_PIPES
 ev.fd[0] = p->event;
 ev.fd[1] = p->magic;
#endif
#ifdef TiNGTYPE_WINNT
 ev.evh = p->event;
#endif
 lock_object(grp);
 sts = evnt_assign(&pxchan->chancx->cxevnt, ev);
 if (!sts) px_update_event(pxchan);
 unlock_object(grp);
 return sts;
}
