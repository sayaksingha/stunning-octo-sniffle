/* px_ioctl_tsdest.h - ioctl code for read/write tsdest */

#ifndef INCLUDED_PX_IOCTL_TSDEST_H
#define INCLUDED_PX_IOCTL_TSDEST_H

#include "px_conn.h"
#include "../TiNG.h"

LOCALDEC int px_rdtsdest(PX_CHAN *pxchan, struct ting_tsdest *p);
LOCALDEC int px_wrtsdest(PX_CHAN *pxchan, struct ting_tsdest *p);

#endif
