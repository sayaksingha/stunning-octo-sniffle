/* px_ioctl_dbg.c - ioctl code for read/write dbg */

#include "px_ioctl_dbg.h"
#include "TiNGcommon.h"
#include "trace.h"

LOCALDEF int px_rddbg(PX_CARDCONN_VTBL_DATA *pvp, struct ting_debug *p)
{
 (void) pvp;
 (void) p;
 if (TiNGtrace) olog("rddbg: not implemented\n");
 return 1;
}

LOCALDEF int px_wrdbg(PX_CARDCONN_VTBL_DATA *pvp, struct ting_debug *p)
{
 (void) pvp;
 (void) p;
 if (TiNGtrace) olog("wrdbg: not implemented\n");
 return 1;
}
