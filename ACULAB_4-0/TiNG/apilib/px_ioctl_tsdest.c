/* px_ioctl_tsdest.c - ioctl code for read/write tsdest */

#include "px_ioctl_tsdest.h"
#include "TiNGcommon.h"
#include "trace.h"

LOCALDEF int px_rdtsdest(PX_CHAN *pxchan, struct ting_tsdest *p)
{
 (void) pxchan;
 (void) p;
 if (TiNGtrace) olog("rdtsdest: not implemented\n");
 return 1;
}

LOCALDEF int px_wrtsdest(PX_CHAN *pxchan, struct ting_tsdest *p)
{
 (void) pxchan;
 (void) p;
 if (TiNGtrace) olog("wrtsdest: not implemented\n");
 return 1;
}
