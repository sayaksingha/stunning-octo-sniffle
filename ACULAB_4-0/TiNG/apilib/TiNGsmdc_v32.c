/* TiNGsmdc_v32.c - Data communications functions for V.32 */

#include "../hsif/kernel.h"
#include "../hsif/modem.h"
#include "../hsif/six2five.h"
#include "../hsif/speed.h"
#include "../hsif/subtask.h"
#include "../hsif/typecode.h"
#include "../hsif/v32.h"
#include "TiNGcommon.h"
#include "TiNGo_chan.h"
#include "TiNGsmdc_v32.h"
#include "smdc_v32.h"

#include <string.h>

struct smdc_v32_config_parms_orig {
	unsigned speed;
	tSM_INT isanswer;
};

struct smdc_v32_config_parms_m1452 {
	unsigned speed;
	tSM_INT isanswer;
	tSM_INT no_ans_tone;
};

STATIC int v32_get_config(struct smdc_channel_config_parms *pp, struct smdc_v32_config_parms *pconf)
{
 unsigned len = (unsigned) pp->config_length;
 if (len != sizeof(*pconf) && len != sizeof(struct smdc_v32_config_parms_orig) && len != sizeof(struct smdc_v32_config_parms_m1452))
	 return err(ERR_SM_BAD_PARAMETER);
 memcpy(pconf, pp->config_data, len);
 memset(pconf + len, 0, sizeof(*pconf) - len);
 return 0;
}

STATIC int v32rx_start(tSMChannelId cp, struct smdc_v32_config_parms *pconf)
{
 CARDCONN *cx = &cp->chan[HANDLE_READ].cx;
 int sts;
 sts = startin(cp, TASKTC_INCHAN);
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_V32RX);
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_SPEED, pconf->speed);
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_DIRECTION, pconf->isanswer);
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_NO_ANS_TONE, pconf->no_ans_tone);
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_ALLOW_AUTO_SPEED_CHANGE, pconf->retrain_can_change_speed);
 if (sts) return sts;
 return 0;
}

STATIC int v32rx_config(struct smdc_channel_config_parms *pp)
{
 struct smdc_v32_config_parms conf;
 const tSMChannelId cp = pp->channel;
 int sts;
 sts = v32_get_config(pp, &conf);
 if (sts) return sts;
 sts = v32rx_start(cp, &conf);
 if (sts) return sts;
 return 0;
}

STATIC int v32_config(struct smdc_channel_config_parms *pp)
{
 struct smdc_v32_config_parms conf;
 const tSMChannelId cp = pp->channel;
 CARDCONN *cx = &cp->chan[HANDLE_WRITE].cx;
 int sts;
 sts = v32_get_config(pp, &conf);
 if (sts) return sts;
 sts = v32rx_start(cp, &conf);
 if (sts) return sts;
 sts = startout(cp, TASKTC_OUTCHAN);
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_V32TX);
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_SPEED, conf.speed);
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_DIRECTION, conf.isanswer);
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_NO_ANS_TONE, conf.no_ans_tone);
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_ALLOW_AUTO_SPEED_CHANGE, conf.retrain_can_change_speed);
 if (sts) return sts;
 return setfriend(&cp->chan[HANDLE_READ].cx, &cp->chan[HANDLE_WRITE], FRIEND_TYPE_V32);
}

STATIC int v32rx_setenc(tSMChannelId cp, ENCTBL *enctbl, unsigned cflen, void *cfdata)
{
 ENCTBL *old = cp->ur.dc.encoding;
 int sts;
 if (old) {
	sts = old->stopfn(cp, 1);
	if (sts) return sts;
	cp->ur.dc.encoding = 0;
 }
 sts = enctbl->encfn(cp, 1, cfdata, cflen);
 if (!sts) cp->ur.dc.encoding = enctbl;
 return sts;
}

STATIC int v32tx_setenc(tSMChannelId cp, ENCTBL *enctbl, unsigned cflen, void *cfdata)
{
 ENCTBL *old = cp->ut.dc.encoding;
 int sts;
 if (old) {
	sts = old->stopfn(cp, 0);
	if (sts) return sts;
	cp->ut.dc.encoding = 0;
 }
 sts = enctbl->encfn(cp, 0, cfdata, cflen);
 if (sts) return sts;
 cp->ut.dc.encoding = enctbl;
 if (cp->ur.dc.encoding) return 0;	// already got one
 	// kludge! first time, set rx and tx the same
 return v32rx_setenc(cp, enctbl, cflen, cfdata);
}

STATIC int v32rx_lcontrol(struct smdc_line_control_parms *pp)
{
 (void) pp;
 // nothing - v32rx is always connected
 return 0;
}

STATIC int v32_lcontrol(struct smdc_line_control_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 switch (pp->cmd) {
 case kSMDCLineCtlCmdSetPrefixSuffix:
	break;
 case kSMDCLineCtlCmdRetrain:
	return sendmsg2(&cp->chan[HANDLE_READ].cx, MSG_RETRAIN, pp->retrain_speed); // NB need to send to RX side!
 case kSMDCLineCtlCmdAssertRTS:
 case kSMDCLineCtlCmdUnassertRTS:
	// for some reason these were allowed, so ignore them for now
	break;
 default:
	return err(ERR_SM_BAD_PARAMETER);
 }
 return 0;
}

STATIC int v32rx_lstatus(struct smdc_line_status_parms *pp)
{
 if (pp->channel->rxtx[1].dataready / 8) {
	pp->rx_status = kSMDCRxStatusReceivingData;
 } else {
	if (pp->channel->ur.dc.u.v32.carrier) {
		pp->rx_status = kSMDCRxStatusCarrierPresent;
	} else pp->rx_status = kSMDCRxStatusNoCarrier;
 }
 if (pp->channel->ur.dc.u.v32.carrier == 1) { // retraining
	pp->link_status = kSMDCLinkStatusConnecting;
 }
 return 0;
}

STATIC int v32_lstatus(struct smdc_line_status_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 if (!cp->rxtx[0].underrun)
 	pp->tx_status = kSMDCTxStatusSendingData;
 else if (!cp->ut.dc.u.v32.carrier)
	pp->tx_status = kSMDCTxStatusSilent;
 else pp->tx_status = kSMDCTxStatusSendingCarrier;
 return v32rx_lstatus(pp);
}

STATIC int v32rx_rstatus(struct smdc_rx_status_parms *pp, U32 *msg)
{
 (void) pp;
 if ((msg[0]&0xffff0000) == MESSAGE(MSG_CARRIER, 2)) {
	 pp->status = kSMDCRxStatusCarrier;
	 pp->u.carrier.carrier = pp->channel->ur.dc.u.v32.carrier;
 }
 return 0;
}

STATIC int v32tx_tstatus(struct smdc_tx_status_parms *pp, U32 *msg)
{
 (void) pp;
 (void) msg;
 return 0;
}

STATIC int v32rx_rcontrol(struct smdc_rx_control_parms *pp)
{
 (void) pp;
 return err(ERR_SM_BAD_PARAMETER);
}

STATIC int v32tx_tcontrol(struct smdc_tx_control_parms *pp)
{
 (void) pp;
 return 1;
}

STATIC int v32rx_msghand(tSMChannelId chan, U32 *msg)
{
 if (*msg & 0x80000000) {
	unsigned long code = *msg & 0xffff0000;
	switch (code) {
	case MESSAGE(MSG_V32_LINE_STATUS, 3):
		return 1;
	case MESSAGE(MSG_CARRIER, 2):
		chan->ur.dc.u.v32.carrier = msg[1];
		return 0;
	case MESSAGE(MSG_RETRAIN, 1):
		chan->ur.dc.u.v32.carrier = 1; // an invalid speed, so we can report retraining
		return 0;
	default:
		return 1;
	}
 } else return 0;
}

STATIC int v32tx_msghand(tSMChannelId chan, U32 *msg)
{
 if (*msg & 0x80000000) {
	unsigned long code = *msg & 0xffff0000;
	switch (code) {
	case MESSAGE(MSG_V32_LINE_STATUS, 3):
		return 1;
	case MESSAGE(MSG_CARRIER, 2):
		chan->ut.dc.u.v32.carrier = msg[1];
		return 0;
	case MESSAGE(MSG_RETRAIN, 1):
		chan->ut.dc.u.v32.carrier = 1; // an invalid speed, so we can report retraining
		return 0;
	default:
		return 1;
	}
 } else return 0;
}

LOCALDEF struct protable protable_v32rx = {
	kSMDCProtocolV32rx,
	v32rx_config,
	v32rx_setenc,
	0,
	0,
	v32rx_lcontrol,
	v32rx_lstatus,
	0,
	0,
	v32rx_rcontrol,
	v32rx_rstatus,
	{ 0, v32rx_msghand, },
	0,
};

LOCALDEF struct protable protable_v32 = {
	kSMDCProtocolV32,
	v32_config,
	v32tx_setenc,
	v32_lcontrol,
	v32_lstatus,
	v32rx_lcontrol,
	v32rx_lstatus,
	v32tx_tcontrol,
	v32tx_tstatus,
	v32rx_rcontrol,
	v32rx_rstatus,
	{ v32tx_msghand, v32rx_msghand, },
	0,
};
