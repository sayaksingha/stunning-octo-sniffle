/* px_ioctl_intslist.h - ioctl code for read/write intslist */

#ifndef INCLUDED_PX_IOCTL_INTSLIST_H
#define INCLUDED_PX_IOCTL_INTSLIST_H

#include "px_conn.h"
#include "../TiNG.h"

LOCALDEC int px_rdintslist(PX_CHAN *pxchan, struct ting_tslist *p);
LOCALDEC int px_wrintslist(PX_CHAN *pxchan, struct ting_tslist *p);

#endif
