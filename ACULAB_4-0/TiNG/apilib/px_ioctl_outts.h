/* px_ioctl_outts.h - ioctl code for read/write outts */

#ifndef INCLUDED_PX_IOCTL_OUTTS_H
#define INCLUDED_PX_IOCTL_OUTTS_H

#include "px_conn.h"
#include "../TiNG.h"

LOCALDEC int px_rdoutts(PX_CHAN *pxchan, struct ting_tslist *p);
LOCALDEC int px_wroutts(PX_CHAN *pxchan, struct ting_tslist *p);

#endif
