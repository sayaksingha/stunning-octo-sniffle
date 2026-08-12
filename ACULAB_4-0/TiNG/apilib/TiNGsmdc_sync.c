/* TiNGsmdc_sync.c - sync data format */

#include "TiNGsmdc_sync.h"
#include "TiNGcommon.h"
#include "TiNGo_chan.h"
#include "../hsif/kernel.h"
#include "../hsif/subtask.h"
#include "../hsif/typecode.h"

LOCALDEF int sync_encode(tSMChannelId chan, int io, void *par, unsigned len)
{
 CARDCONN *cx = &chan->chan[io ? HANDLE_READ : HANDLE_WRITE].cx;
 int sts;
 (void) par;
 (void) len;
 if (len) return err(ERR_SM_BAD_PARAMETER);
 sts = sendmsg2(cx, MSG_ADDSUB, io ? SUBTASKTC_SYNCRX : SUBTASKTC_SYNCTX);
 if (sts) return sts;
 return 0;
}

LOCALDEF int sync_stop(tSMChannelId chan, int io)
{
 CARDCONN *cx = &chan->chan[io ? HANDLE_READ : HANDLE_WRITE].cx;
 return sendmsg2(cx, MSG_DELSUB, io ? SUBTASKTC_SYNCRX : SUBTASKTC_SYNCTX);
}
