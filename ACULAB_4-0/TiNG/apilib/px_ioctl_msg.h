/* px_ioctl_msg.h - ioctl code for read/write msg */

#ifndef INCLUDED_PX_IOCTL_MSG_H
#define INCLUDED_PX_IOCTL_MSG_H

#include "px_conn.h"
#include "../TiNG.h"

LOCALDEC void unsafe_px_rdmsg(PX_CHAN *pxchan, struct ting_message *p);
LOCALDEC int px_rdmsg(PX_CHAN *pxchan, struct ting_message *p);
LOCALDEC int px_wrmsg(PX_CHAN *pxchan, struct ting_message *p);

#endif
