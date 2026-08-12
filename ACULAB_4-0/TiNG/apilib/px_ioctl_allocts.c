/* px_ioctl_allocts.c - ioctl code for read/write allocts */

#include "px_ioctl_allocts.h"
#include "TiNGcommon.h"
#include "trace.h"

LOCALDEF int px_rdallocts(PX_CHAN *pxchan, struct ting_allocts *p)
{
 (void) pxchan;
 (void) p;
 if (TiNGtrace) olog("rdallocts: not implemented\n");
 return 1;
}

LOCALDEF int px_wrallocts(PX_CHAN *pxchan, struct ting_allocts *p)
{
 (void) pxchan;
 (void) p;
 if (TiNGtrace) olog("wrallocts: not implemented\n");
 return 1;
}
