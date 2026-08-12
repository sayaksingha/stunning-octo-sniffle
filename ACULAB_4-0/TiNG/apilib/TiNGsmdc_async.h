/* TiNGsmdc_async.h - async data format */

#ifndef INCLUDED_TINGSMDC_ASYNC_H
#define INCLUDED_TINGSMDC_ASYNC_H

#include "smdc_async.h"

	// configure async encoding on the channel
LOCALDEC int async_encode(tSMChannelId chan, int io, void *par, unsigned len);
LOCALDEC int async_stop(tSMChannelId chan, int io);

#endif
