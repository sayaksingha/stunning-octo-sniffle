/* TiNGsmdc_hdlc.c - HDLC data format */

#ifndef INCLUDED_TINGSMDC_HDLC_H
#define INCLUDED_TINGSMDC_HDLC_H

#include "smdc_hdlc.h"
#include "../hsif/hdlc.h"

	// configure HDLC encoding on the channel
LOCALDEC int hdlc_encode(tSMChannelId chan, int io, void *par, unsigned len);
LOCALDEC int hdlc_stop(tSMChannelId chan, int io);

#endif
