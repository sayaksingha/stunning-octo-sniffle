/* px_ioctl_mem.c - ioctl code for read/write mem */

#include "px_ioctl_mem.h"
#include "TiNGcommon.h"
#include "trace.h"

LOCALDEF int px_rdmem(PX_MOD *pxmod, struct ting_sharcmem *p)
{
 (void) pxmod;
 (void) p;
 if (TiNGtrace) olog("rdmem: not implemented\n");
 return 1;
}

LOCALDEF int px_wrmem(PX_MOD *pxmod, struct ting_sharcmem *p)
{
 (void) pxmod;
 (void) p;
 if (TiNGtrace) olog("wrmem: not implemented\n");
 return 1;
}
