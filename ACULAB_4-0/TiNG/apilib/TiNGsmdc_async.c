/* TiNGsmdc_async.c - async data format */

#include "TiNGsmdc_async.h"
#include "TiNGcommon.h"
#include "TiNGo_chan.h"
#include "../hsif/async.h"
#include "../hsif/kernel.h"
#include "../hsif/subtask.h"
#include "../hsif/typecode.h"

LOCALDEF int async_encode(tSMChannelId chan, int io, void *par, unsigned len)
{
 CARDCONN *cx = &chan->chan[io ? HANDLE_READ : HANDLE_WRITE].cx;
 struct smdc_async_format_parms *ap = par;
 unsigned stopbits = 1;
 unsigned databits = 8;
 int sts;
 if (len < sizeof(*ap)) return err(ERR_SM_BAD_PARAMETER);
 if (!io && ap->stopbits) stopbits = ap->stopbits + 1;
 if (ap->databits) databits = ap->databits;
 if (stopbits + databits + 1 > 16) return err(ERR_SM_BAD_PARAMETER);
 sts = sendmsg2(cx, MSG_ADDSUB, io ? SUBTASKTC_ASYRX : SUBTASKTC_ASYTX);
 if (sts) return sts;
 if (stopbits != 1) {
	sts = sendmsg2(cx, MSG_STOPBITS, stopbits);
	if (sts) return sts;
 }
 if (databits != 8) {
	sts = sendmsg2(cx, MSG_DATABITS, databits);
	if (sts) return sts;
 }
 return 0;
}

LOCALDEF int async_stop(tSMChannelId chan, int io)
{
 CARDCONN *cx = &chan->chan[io ? HANDLE_READ : HANDLE_WRITE].cx;
 return sendmsg2(cx, MSG_DELSUB, io ? SUBTASKTC_ASYRX : SUBTASKTC_ASYTX);
}
