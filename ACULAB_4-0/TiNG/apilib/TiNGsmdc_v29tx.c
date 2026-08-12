/* TiNGsmdc_v29tx.c - Data communications functions for V.29 transmit */

#include "TiNGsmdc_v29tx.h"
#include "TiNGcommon.h"
#include "TiNGo_chan.h"
#include "../hsif/kernel.h"
#include "../hsif/modem.h"
#include "../hsif/six2five.h"
#include "../hsif/speed.h"
#include "../hsif/subtask.h"
#include "../hsif/typecode.h"
#include "smdc_v29.h"

STATIC int v29tx_config(struct smdc_channel_config_parms *pp)
{
 struct smdc_v29_config_parms *const conf = pp->config_data;
 const tSMChannelId cp = pp->channel;
 CARDCONN *cx = &cp->chan[HANDLE_WRITE].cx;
 int sts;
 if ((unsigned) pp->config_length < sizeof(*conf))
	return err(ERR_SM_BAD_PARAMETER);
 sts = startout(cp, TASKTC_OUTCHAN);
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_6_TO_5);
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_625TYPE, 1);
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_V29TX);
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_SPEED, conf->speed);
 if (sts) return sts;
 return 0;
}

STATIC int v29tx_setenc(tSMChannelId cp, ENCTBL *enctbl, unsigned cflen, void *cfdata)
{
 ENCTBL *old = cp->ut.dc.encoding;
 int sts;
 if (old) {
	sts = old->stopfn(cp, 0);
	if (sts) return sts;
	cp->ut.dc.encoding = 0;
 }
 sts = enctbl->encfn(cp, 0, cfdata, cflen);
 if (!sts) cp->ut.dc.encoding = enctbl;
 return sts;
}

STATIC int v29tx_lcontrol(struct smdc_line_control_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 switch (pp->cmd) {
 case kSMDCLineCtlCmdSetPrefixSuffix:
	break;
 case kSMDCLineCtlCmdAssertRTS:
	cp->ut.dc.u.v29.carrier = 1;
	return sendmsg3(&cp->chan[HANDLE_WRITE].cx, MSG_CARRIER, -3, pp->tep);
 case kSMDCLineCtlCmdUnassertRTS:
	return sendmsg2(&cp->chan[HANDLE_WRITE].cx, MSG_CARRIER, -128);
 default:
	return err(ERR_SM_BAD_PARAMETER);
 }
 return 0;
}

STATIC int v29tx_lstatus(struct smdc_line_status_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 if (!cp->rxtx[0].underrun)
 	pp->tx_status = kSMDCTxStatusSendingData;
 else if (!cp->ut.dc.u.v29.carrier)
	pp->tx_status = kSMDCTxStatusSilent;
 else pp->tx_status = kSMDCTxStatusSendingCarrier;
 return 0;
}

STATIC int v29tx_tstatus(struct smdc_tx_status_parms *pp, U32 *msg)
{
 (void) pp;
 (void) msg;
 return 0;
}

STATIC int v29tx_tcontrol(struct smdc_tx_control_parms *pp)
{
 (void) pp;
 return 1;
}

STATIC int v29tx_msghand(tSMChannelId chan, U32 *msg)
{
 if (*msg & 0x80000000) {
	unsigned long code = *msg & 0xffff0000;
	switch (code) {
	case MESSAGE(MSG_CARRIER, 2):
		chan->ut.dc.u.v29.carrier = msg[1];
		return 0;
	default:
		return 1;
	}
 } else return 0;
}

LOCALDEF struct protable protable_v29tx = {
	kSMDCProtocolV29tx,
	v29tx_config,
	v29tx_setenc,
	v29tx_lcontrol,
	v29tx_lstatus,
	0,
	0,
	v29tx_tcontrol,
	v29tx_tstatus,
	0,
	0,
	{ v29tx_msghand, 0, },
	0,
};
