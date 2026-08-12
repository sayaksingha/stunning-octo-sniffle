/* TiNGsmdc_ifptx.c - Data communications functions for T.38 transmit */

#include "TiNGsmdc_ifptx.h"
#include "../hsif/ifp.h"
#include "../hsif/kernel.h"
#include "../hsif/modem.h"
#include "../hsif/subtask.h"
#include "../hsif/t38.h"
#include "../hsif/typecode.h"
#include "smdc_ifp.h"
#include "TiNGcommon.h"
#include "TiNGsmdc.h"
#include <string.h>


STATIC int ifptx_lcontrol(struct smdc_line_control_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 CARDCONN *cx = &cp->chan[HANDLE_WRITE].cx;
 switch (pp->cmd) {
 case kSMDCLineCtlCmdSetPrefixSuffix:
	break;
 case kSMDCLineCtlCmdAssertRTS:
	cp->ut.dc.u.ifp.carrier = 1;
	return sendmsg4(cx, MSG_CARRIER, -3, pp->tep, pp->shorttrain);
 case kSMDCLineCtlCmdUnassertRTS:
	return sendmsg2(cx, MSG_CARRIER, -128);
 default:
	return err(ERR_SM_BAD_PARAMETER);
 }
 return 0;
}

STATIC int ifptx_lstatus(struct smdc_line_status_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 if (!cp->ut.dc.u.ifp.carrier)
	pp->tx_status = kSMDCTxStatusSilent;
 else if (!cp->rxtx[0].underrun)
 	pp->tx_status = kSMDCTxStatusSendingData;
 else pp->tx_status = kSMDCTxStatusSendingCarrier;
 return 0;
}

STATIC int ifptx_tstatus(struct smdc_tx_status_parms *pp, U32 *msg)
{
 (void) pp;
 (void) msg;
 return 0;
}

STATIC int ifptx_tcontrol(struct smdc_tx_control_parms *pp)
{
 (void) pp;
 return 1;
}

STATIC int ifptx_msghand(tSMChannelId chan, U32 *msg)
{
 if (*msg & 0x80000000) {
	unsigned long code = *msg & 0xffff0000;
	switch (code) {
	case MESSAGE(MSG_CARRIER, 2):
		chan->ut.dc.u.ifp.carrier = msg[1];
		return 0;
	default:
		return 1;
	}
 } else return 0;
}

STATIC int ifptx_setenc(tSMChannelId cp, ENCTBL *enctbl, unsigned cflen, void *cfdata)
{
 CARDCONN *cx = &cp->chan[HANDLE_WRITE].cx;
 int hdlc;
 (void) cflen;
 (void) cfdata;
 switch (enctbl->enctype) {
 case kSMDCConfigEncodingHDLC: hdlc = 1; break;
 case kSMDCConfigEncodingSync: hdlc = 0; break;
 default:
 	return err(ERR_SM_BAD_PARAMETER);
 }
 return sendmsg2(cx, MSG_T38HDLC, hdlc);
}

STATIC struct protable protable_ifptx = {
	0,
	0,
	ifptx_setenc,
	ifptx_lcontrol,
	ifptx_lstatus,
	0,
	0,
	ifptx_tcontrol,
	ifptx_tstatus,
	0,
	0,
	{ 0, ifptx_msghand, },
	0,
};

LOCALDEF int real_smdc_tx_config_t38(struct smdc_tx_config_t38_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 int sts = 0;
 lock_object(cp);
 if (cp->rxtx[0].func != RXTXF_NONE) {
	if (cp->rxtx[0].func == RXTXF_INVALID)
		sts = err(ERR_SM_WRONG_CHANNEL_TYPE);
	else
		sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else {
	CARDCONN *cx = &cp->chan[HANDLE_WRITE].cx;
	memset(&cp->ut, 0, sizeof(cp->ut));
	cp->rxtx[0].protocol = &protable_ifptx;
	cp->rxtx[0].func = RXTXF_DATA;
	cp->rxtx[0].state = RXTXS_DONE;
	cp->rxtx[0].timeout_milliseconds = 100;
	sts = startout(cp, TASKTC_IFPTX);
	if (!sts) sts = sendmsg3(cx, MSG_T38CFG, pp->PreCorrigendum, pp->Protocol);
	if (!sts) sts = sendmsg2(cx, MSG_T38_MAX_PACKET_LEN, pp->max_packet_length);
	if (!sts) {
		cp->rxtx[0].state = RXTXS_RUN;
		cp->rxtx[0].gotready = 0;
		cp->ut.dc.encoding = smdc_encode(kSMDCConfigEncodingNone);
		sts = sendmsg2(cx, MSG_ADDHSUB, SUBTASKTC_HBUFFROMHOST);
		if (!sts) sts = chan_setxfer(cp, 0);
	}
 }
 unlock_object(cp);
 return sts;
}

LOCALDEF int real_smdc_tx_tone_indicator_t38(struct smdc_tx_tone_indicator_t38_parms *tonep)
{
 const tSMChannelId cp = tonep->channel;
 int sts = 0;
 lock_object(cp);
 if (cp->rxtx[0].func != RXTXF_DATA) sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 if (!sts) {
	CARDCONN *cx = &cp->chan[HANDLE_WRITE].cx;
	if (tonep->tone_type == kSMDCT38ToneTypeCNG) {
		sts = sendmsg2(cx, MSG_T38TONE, T38TONE_CNG);
	} else if (tonep->tone_type == kSMDCT38ToneTypeCED) {
		sts = sendmsg2(cx, MSG_T38TONE, T38TONE_CED);
	} else if (tonep->tone_type == kSMDCT38ToneTypeANSAM) {
		sts = sendmsg2(cx, MSG_T38TONE, T38TONE_ANSAM);
	}
 }	
 unlock_object(cp);
 return sts; 
}

LOCALDEF int real_smdc_tx_v34_line_control_t38(struct smdc_tx_v34_line_control_t38_parms *lcp)
{
 const tSMChannelId cp = lcp->channel;
 int sts = 0;
 lock_object(cp);
 if (!sts) {
	CARDCONN *cx = &cp->chan[HANDLE_WRITE].cx;
	int rate = lcp->primary_bitrate;
	sts = sendmsg2(cx, MSG_T38RATE, rate);
 }	
 unlock_object(cp);
 return sts; 
}

LOCALDEF int real_smdc_tx_v34_indicator_t38(struct smdc_tx_v34_indicator_t38_parms *v34p)
{
 const tSMChannelId cp = v34p->channel;
 int sts = 0;
 lock_object(cp);
 if (!sts) {
	CARDCONN *cx = &cp->chan[HANDLE_WRITE].cx;
	if (v34p->v34_type == kSMDCT38_V34Type_V34_CC_1200) {
		sts = sendmsg2(cx, MSG_T38_V34, V34_CC_1200);
	} else if (v34p->v34_type == kSMDCT38_V34Type_V34_PRI_CHAN) {
		sts = sendmsg2(cx, MSG_T38_V34, V34_PRI_CHAN);
	} else if (v34p->v34_type == kSMDCT38_V34Type_V34_CC_RETRAIN) {
		sts = sendmsg2(cx, MSG_T38_V34, V34_CC_RETRAIN);
	}
 }	
 unlock_object(cp);
 return sts; 
}
