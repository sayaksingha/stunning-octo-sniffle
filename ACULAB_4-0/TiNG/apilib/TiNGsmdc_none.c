/* TiNGsmdc_none.c - no data format */

#include "TiNGsmdc_none.h"

LOCALDEF int none_encode(tSMChannelId chan, int io, void *par, unsigned len)
{
 (void) chan;
 (void) io;
 (void) par;
 (void) len;
 return 0;
}

LOCALDEF int none_stop(tSMChannelId chan, int io)
{
 (void) chan;
 (void) io;
 return 0;
}
