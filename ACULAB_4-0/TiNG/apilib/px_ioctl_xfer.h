/* px_ioctl_xfer.h - ioctl code for read/write xfer */

#ifndef INCLUDED_PX_IOCTL_XFER_H
#define INCLUDED_PX_IOCTL_XFER_H

#include "px_conn.h"
#include "../TiNG.h"

LOCALDEC int px_rdxfer(PX_CHAN *pxchan, struct ting_xfer *p);
LOCALDEC int px_wrxfer(PX_CHAN *pxchan, struct ting_xfer *p);

#endif
