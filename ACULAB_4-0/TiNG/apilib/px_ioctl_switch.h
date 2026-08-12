/* px_ioctl_switch.h - ioctl code for read/write switch */

#ifndef INCLUDED_PX_IOCTL_SWITCH_H
#define INCLUDED_PX_IOCTL_SWITCH_H

#include "px_conn.h"
#include "../TiNG.h"

LOCALDEC int px_rdswitch(PX_CARDCONN_VTBL_DATA *pvp, struct ting_switch *p);

#endif
