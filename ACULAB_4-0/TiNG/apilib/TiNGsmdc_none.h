/* TiNGsmdc_none.h - no data format */

#ifndef INCLUDED_TINGSMDC_NONE_H
#define INCLUDED_TINGSMDC_NONE_H

#include "smdc_none.h"
#include "smtypes.h"

	// configure no encoding on the channel (dummy function)
LOCALDEC int none_encode(tSMChannelId chan, int io, void *par, unsigned len);
LOCALDEC int none_stop(tSMChannelId chan, int io);

#endif
