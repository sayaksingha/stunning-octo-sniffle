/* smdc_emcode.c - find encoding to use */

#include "smdc_encode.h"
#include "TiNGsmdc_async.h"
#include "TiNGsmdc_hdlc.h"
#include "TiNGsmdc_none.h"
#include "TiNGsmdc_sync.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

STATIC ENCTBL enctable[] = {
	{async_encode, async_stop, kSMDCConfigEncodingAsync, },
	{sync_encode, sync_stop, kSMDCConfigEncodingSync, },
	{hdlc_encode, hdlc_stop, kSMDCConfigEncodingHDLC, },
	{none_encode, none_stop, kSMDCConfigEncodingNone, },
};

LOCALDEF ENCTBL *smdc_encode(int enctype)
{
 unsigned u;
 for (u=0; u < arlen(enctable); u++) {
 	if (enctable[u].enctype == enctype) return &enctable[u];
 }
 return 0;
}
