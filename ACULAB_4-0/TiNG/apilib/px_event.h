/* px_event.h - PX event internals */

#ifndef INCLUDED_PX_EVENT_H
#define INCLUDED_PX_EVENT_H

#include "errcode.h"
#include "px_chan.h"

LOCALDEC void update_minready(PX_CHAN *pxchan);
LOCALDEC void update_capacity(PX_CHAN *pxchan);
LOCALDEC int more_writecap(PX_CHAN *pxchan);
LOCALDEC int capacity(uint32_t *cap, PX_CHAN *pxchan);
LOCALDEC int px_update_event(PX_CHAN *pxchan);

#endif
