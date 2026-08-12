#ifndef INCLUDED_SMDC_ENCODE_H
#define INCLUDED_SMDC_ENCODE_H

#include "smdc_async.h"
#include "smdc_hdlc.h"
#include "smdc_none.h"
#include "smdc_sync.h"

typedef struct enctbl {
	int (*encfn)(tSMChannelId c, int io, void *par, unsigned plen);
	int (*stopfn)(tSMChannelId c, int io);
	int enctype;
} ENCTBL;

	// find suitable entry in encoding table to configure encoding,
	// returning zero if none found
LOCALDEC ENCTBL *smdc_encode(int enctype);

#endif
