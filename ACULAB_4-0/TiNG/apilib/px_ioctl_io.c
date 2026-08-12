/* px_ioctl_io.c - ioctl code for read/write io */

#include "px_ioctl_io.h"
#include "TiNGcommon.h"
#include "trace.h"

LOCALDEF int px_rdio(PX_CARDCONN_VTBL_DATA *pvp, struct ting_cardreg *p)
{
 (void) pvp;
 (void) p;
 if (TiNGtrace) olog("rdio: not implemented\n");
 return 1;
}
