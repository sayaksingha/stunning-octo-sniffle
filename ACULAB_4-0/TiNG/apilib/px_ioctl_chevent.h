/* px_ioctl_chevent.h - ioctl code for read/write channel event */

#ifndef INCLUDED_PX_IOCTL_CHEVENT_H
#define INCLUDED_PX_IOCTL_CHEVENT_H

#include "px_conn.h"
#include "../TiNG.h"

LOCALDEC int px_rdchevent(PX_CHAN *pxchan, struct ting_chevent *p);
LOCALDEC int px_wrchevent(PX_CHAN *pxchan, struct ting_chevent *p);

#endif
