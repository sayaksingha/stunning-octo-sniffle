/* px_ioctl_dest.h - ioctl code for read/write destination */

#ifndef INCLUDED_PX_IOCTL_DEST_H
#define INCLUDED_PX_IOCTL_DEST_H

#include "px_conn.h"
#include "smtypes.h"
#include "cardconn.h"
#include "../TiNG.h"

LOCALDEC int px_rddest(PX_CARDCONN_VTBL_DATA *pvp, struct ting_destination *p);
LOCALDEC int px_wrdest(PX_CARDCONN_VTBL_DATA *pvp, struct ting_destination *p);

#endif
