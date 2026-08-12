/* px_ioctl_cardinfo.h - ioctl code for read/write cardinfo */

#ifndef INCLUDED_PX_IOCTL_CARDINFO_H
#define INCLUDED_PX_IOCTL_CARDINFO_H

#include "px_conn.h"
#include "../TiNG.h"

LOCALDEC int px_rdcardinfo(PX_CARDCONN_VTBL_DATA *pvp, struct ting_cardinfo *p);

#endif
