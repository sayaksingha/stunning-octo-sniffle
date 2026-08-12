/* px_ioctl_confin.h - ioctl code for read/write confin */

#ifndef INCLUDED_PX_IOCTL_CONFIN_H
#define INCLUDED_PX_IOCTL_CONFIN_H

#include "px_conn.h"
#include "../TiNG.h"

LOCALDEC int px_rdconfin(PX_CHAN *pxchan, struct ting_confin *p);
LOCALDEC int px_wrconfin(PX_CHAN *pxchan, struct ting_confin *p);

#endif
