/* px_ioctl_tasklist.c - ioctl code for read/write tasklist */

#include "px_ioctl_tasklist.h"
#include "TiNGcommon.h"
#include "trace.h"

LOCALDEF int px_rdtasklist(PX_MOD *pxmod, struct ting_taskitem *p)
{
 (void) pxmod;
 (void) p;
 if (TiNGtrace) olog("rdtasklist: not implemented\n");
 return 1;
}
