/* px_ioctl_friend.h - ioctl code for read/write friend */

#ifndef INCLUDED_PX_IOCTL_FRIEND_H
#define INCLUDED_PX_IOCTL_FRIEND_H

#include "px_conn.h"
#include "../TiNG.h"

LOCALDEC int px_rdfriend(PX_CHAN *pxchan, struct ting_friend *p);
LOCALDEC int px_wrfriend(PX_CHAN *pxchan, struct ting_friend *p);

#endif
