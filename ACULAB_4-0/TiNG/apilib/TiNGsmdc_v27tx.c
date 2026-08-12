/* TiNGsmdc_v27tx.c - Data communications functions for V.27 transmit */

#include "TiNGsmdc_v27tx.h"
#include "TiNGcommon.h"
#include "TiNGo_chan.h"
#include "../hsif/kernel.h"
#include "../hsif/modem.h"
#include "../hsif/six2five.h"
#include "../hsif/speed.h"
#include "../hsif/subtask.h"
#include "../hsif/typecode.h"
#include "smdc_v27.h"

STATIC int v27tx_config(struct smdc_channel_config_parms *pp)
{
 struct smdc_v27_config_parms *const conf = pp->config_data;
 const tSMChannelId cp = pp->channel;
 CARDCONN *cx = &cp->chan[HANDLE_WRITE].cx;
 int sts;
 if ((unsigned) pp->config_length < sizeof(*conf))
	return err(ERR_SM_BAD_PARAMETER);
 sts = startout(cp, TASKTC_OUTCHAN);
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_6_TO_5);
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_625TYPE, 0);
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_V27TX);
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_SPEED, conf->speed);
 if (sts) return sts;
 return 0;
}

STATIC int v27tx_setenc(tSMChannelId cp, ENCTBL *enctbl, unsigned cflen, void *cfdata)
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

STATIC int v27tx_lcontrol(struct smdc_line_control_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 switch (pp->cmd) {
 case kSMDCLineCtlCmdSetPrefixSuffix:
	break;
 case kSMDCLineCtlCmdAssertRTS:
	cp->ut.dc.u.v27.carrier = 1;
	return sendmsg4(&cp->chan[HANDLE_WRITE].cx, MSG_CARRIER, -3,
			pp->tep, pp->shorttrain);
 case kSMDCLineCtlCmdUnassertRTS:
	return sendmsg2(&cp->chan[HANDLE_WRITE].cx, MSG_CARRIER, -128);
 default:
	return err(ERR_SM_BAD_PARAMETER);
 }
 return 0;
}

STATIC int v27tx_lstatus(struct smdc_line_status_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 if (!cp->rxtx[0].underrun)
 	pp->tx_status = kSMDCTxStatusSendingData;
 else if (!cp->ut.dc.u.v27.carrier)
	pp->tx_status = kSMDCTxStatusSilent;
 else pp->tx_status = kSMDCTxStatusSendingCarrier;
 return 0;
}

STATIC int v27tx_tstatus(struct smdc_tx_status_parms *pp, U32 *msg)
{
 (void) pp;
 (void) msg;
 return 0;
}

STATIC int v27tx_tcontrol(struct smdc_tx_control_parms *pp)
{
 (void) pp;
 return 1;
}

STATIC int v27tx_msghand(tSMChannelId chan, U32 *msg)
{
 if (*msg & 0x80000000) {
	unsigned long code = *msg & 0xffff0000;
	switch (code) {
	case MESSAGE(MSG_CARRIER, 2):
		chan->ut.dc.u.v27.carrier = msg[1];
		return 0;
	default:
		return 1;
	}
 } else return 0;
}

LOCALDEF struct protable protable_v27tx = {
	kSMDCProtocolV27tx,
	v27tx_config,
	v27tx_setenc,
	v27tx_lcontrol,
	v27tx_lstatus,
	0,
	0,
	v27tx_tcontrol,
	v27tx_tstatus,
	0,
	0,
	{ v27tx_msghand, 0, },
	0,
};
