/* TiNGsmdc_hdlc.c - HDLC data format */

#include "TiNGsmdc_hdlc.h"
#include "TiNGcommon.h"
#include "TiNGo_chan.h"
#include "../hsif/hdlc.h"
#include "../hsif/kernel.h"
#include "../hsif/subtask.h"
#include "../hsif/typecode.h"
#include <string.h>

struct smdc_hdlc_format_parms_m1272 {
	tSM_INT crcsize;	// size of CRC (bits)
	tSM_INT crc;		// CRC polynomial: bit N is x^N term
};

LOCALDEF int hdlc_encode(tSMChannelId chan, int io, void *par, unsigned len)
{
 CARDCONN *cx = &chan->chan[io ? HANDLE_READ : HANDLE_WRITE].cx;
 struct smdc_hdlc_format_parms hp;
 unsigned shift;
 unsigned mask;
 int sts;
 if (len < sizeof(struct smdc_hdlc_format_parms_m1272)) return err(ERR_SM_BAD_PARAMETER);
 memcpy(&hp, par, len);
 if (len < sizeof(hp)) {
	hp.rx_min_initial_flags = 0; // set up added fields
	hp.rx_min_ones = 0;
 }
 if (hp.rx_min_initial_flags < 0) return err(ERR_SM_BAD_PARAMETER);
 if ((unsigned) hp.crcsize > 32) return err(ERR_SM_BAD_PARAMETER);
 sts = sendmsg2(cx, MSG_ADDSUB, io ? SUBTASKTC_HDLCRX : SUBTASKTC_HDLCTX);
 if (sts) return sts;
 if (io) { // min flags is rx only
	sts = sendmsg2(cx, MSG_SET_MIN_HDLC_FLAGS, hp.rx_min_initial_flags);
	if (!sts) sts = sendmsg2(cx, MSG_SET_MIN_ONES, hp.rx_min_ones);
	if (sts) return sts;
 }
 shift = 32 - hp.crcsize;
 if (hp.crcsize) mask = 0xffffffff << shift;
 else mask = 0;
 sts = sendmsg3(cx, MSG_SET_CRC, hp.crc << shift, mask);
 if (sts) return sts;
 return 0;
}

LOCALDEF int hdlc_stop(tSMChannelId chan, int io)
{
 CARDCONN *cx = &chan->chan[io ? HANDLE_READ : HANDLE_WRITE].cx;
 return sendmsg2(cx, MSG_DELSUB, io ? SUBTASKTC_HDLCRX : SUBTASKTC_HDLCTX);
}
