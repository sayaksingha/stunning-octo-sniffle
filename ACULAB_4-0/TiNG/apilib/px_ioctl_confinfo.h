/* px_ioctl_confinfo.h - ioctl code for read/write confinfo */

#ifndef INCLUDED_PX_IOCTL_CONFINFO_H
#define INCLUDED_PX_IOCTL_CONFINFO_H

#include "px_conn.h"
#include "../TiNG.h"

LOCALDEC int px_rdconfinfo(PX_CHAN *pxchan, struct ting_confinfo *p);

#endif
