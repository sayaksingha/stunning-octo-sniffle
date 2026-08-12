/* px_ioctl_mstate.c - ioctl code for read/write mstate */

#include "px_ioctl_mstate.h"
#include "TiNGcommon.h"
#include "trace.h"

LOCALDEF int px_rdmstate(PX_MOD *pxmod, struct ting_mstate *p)
{
 (void) pxmod;
 p->mstate = MS_RUNNING;
 p->avgcpuload = ~0u;
 p->maxcpuload = ~0u;
 p->restbl = 0;
 lock_object(&pxmod->modgroup);
 p->connstate = pxmod->modgroup.stop == kASSPGroupStopped;
 unlock_object(&pxmod->modgroup);
 return 0;
}

LOCALDEF int px_wrmstate(PX_MOD *pxmod, struct ting_mstate *p)
{
 (void) pxmod;
 (void) p;
 if (TiNGtrace) olog("wrmstate: not implemented\n");
 return 1;
}
