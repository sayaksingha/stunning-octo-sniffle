/* px_ioctl_switch.c - ioctl code for read/write switch */

#include "px_ioctl_switch.h"
#include "TiNGcommon.h"
#include "trace.h"

LOCALDEF int px_rdswitch(PX_CARDCONN_VTBL_DATA *pvp, struct ting_switch *p)
{
 (void) pvp;
 (void) p;
 if (TiNGtrace) olog("rdswitch: not implemented\n");
 return 1;
}
