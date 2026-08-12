/* px_ioctl_io.h - ioctl code for read/write io */

#ifndef INCLUDED_PX_IOCTL_IO_H
#define INCLUDED_PX_IOCTL_IO_H

#include "px_conn.h"
#include "../TiNG.h"

LOCALDEC int px_rdio(PX_CARDCONN_VTBL_DATA *pvp, struct ting_cardreg *p);

#endif
