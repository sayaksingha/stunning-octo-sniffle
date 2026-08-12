/* px_ioctl_allocts.h - ioctl code for read/write allocts */

#ifndef INCLUDED_PX_IOCTL_ALLOCTS_H
#define INCLUDED_PX_IOCTL_ALLOCTS_H

#include "px_conn.h"
#include "../TiNG.h"

LOCALDEC int px_rdallocts(PX_CHAN *pxchan, struct ting_allocts *p);
LOCALDEC int px_wrallocts(PX_CHAN *pxchan, struct ting_allocts *p);

#endif
