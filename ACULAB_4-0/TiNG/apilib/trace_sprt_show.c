/* trace_sprt_show.c - helper functions for SPRT API tracing */

#include "trace_sprt_show.h"
#include "trace.h"
#include <stdio.h>

STATIC void datastring(char *buf, unsigned buflen, unsigned char *dp, unsigned dlen)
{
 while (buflen > 1) {
 	if (!dlen--) {
 		*buf = 0;
 		return;
	}
 	sprintf(buf, "%02x", *dp++);
	buf += 2;
 	buflen -= 2;
 }
 buf[-1] = 0;
 buf[-2] = '-';
}

LOCALDEF void show_sprt_message_tx_in(SM_SPRT_SEND_MESSAGE_PARMS *pp)
{
 char ds[512];
 datastring(ds, sizeof(ds),(unsigned char *)pp->message, pp->length);
 olog(", message=%s)", ds);
}

LOCALDEF void show_sprt_message_rx_in(SM_SPRT_READ_MESSAGE_PARMS *pp)
{
 olog(", message=%p)", pp->message);
}

LOCALDEF void show_sprt_message_rx_out(SM_SPRT_READ_MESSAGE_PARMS *pp)
{
 char ds[512];
 datastring(ds, sizeof(ds),(unsigned char *)pp->message, pp->length);
 olog(", message=%s\n", ds);
}

