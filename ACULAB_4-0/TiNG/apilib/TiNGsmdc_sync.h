/* TiNGsmdc_sync.h - sync data format */

#ifndef INCLUDED_TINGSMDC_SYNC_H
#define INCLUDED_TINGSMDC_SYNC_H

#include "smdc_sync.h"
#include "smtypes.h"

	// configure sync encoding on the channel
LOCALDEC int sync_encode(tSMChannelId chan, int io, void *par, unsigned len);
LOCALDEC int sync_stop(tSMChannelId chan, int io);

#endif
