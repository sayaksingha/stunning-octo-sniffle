/* px_ioctl_dbg.h - ioctl code for read/write dbg */

#ifndef INCLUDED_PX_IOCTL_DBG_H
#define INCLUDED_PX_IOCTL_DBG_H

#include "px_conn.h"
#include "../TiNG.h"

LOCALDEC int px_rddbg(PX_CARDCONN_VTBL_DATA *pvp, struct ting_debug *p);
LOCALDEC int px_wrdbg(PX_CARDCONN_VTBL_DATA *pvp, struct ting_debug *p);

#endif
