/* TiNGsmdc_v110.c - Data communications functions for V.110 */

#include "TiNGsmdc_v110.h"
#include "TiNGcommon.h"
#include "TiNGo_chan.h"
#include "../hsif/kernel.h"
#include "../hsif/v110.h"
#include "../hsif/typecode.h"
#include "smdc_v110.h"

STATIC int msgboth(tSMChannelId cp, struct ting_message *mp)
{
 if (ERR(cx_ioctl(&cp->chan[HANDLE_READ].cx,
		IOCTL_TiNG_WRMSG, mp, sizeof(*mp)))
	|| ERR(cx_ioctl(&cp->chan[HANDLE_WRITE].cx,
		IOCTL_TiNG_WRMSG, mp, sizeof(*mp))))
	return err(ERR_SM_DEVERR);
 return 0;
}

STATIC int v110_config(struct smdc_channel_config_parms *pp)
{
 struct smdc_v110_config_parms *cp = pp->config_data;
 tSMChannelId chanp = pp->channel;
 struct ting_message msg;
 int sts;
 if ((unsigned) pp->config_length < sizeof(*cp) || cp->format > kSMDCFormatSync)
	return err(ERR_SM_BAD_PARAMETER);
 sts = startout(chanp, TASKTC_V110TX);
 if (sts) return sts;
 sts = startin(chanp, TASKTC_V110RX);
 if (sts) return sts;
 msg.ioctl_length = sizeof(msg);
 msg.flags = 0;
 msg.msg[0] = MESSAGE(MSG_SET_SPEED, 2);
 msg.msg[1] = cp->speed;
 sts = msgboth(chanp, &msg);
 if (sts) return err(ERR_SM_DEVERR);
 msg.msg[0] = MESSAGE(MSG_SET_ASYNC, 2);
 msg.msg[1] = !cp->format;
 sts = msgboth(chanp, &msg);
 if (sts) return err(ERR_SM_DEVERR);
 chanp->rxtx[0].state = RXTXS_RUN;
 chanp->rxtx[1].state = RXTXS_RUN;
 chanp->ur.dc.u.v110.last_sxe = ~0u;
 return 0;
}

STATIC int v110_setenc(tSMChannelId cp, ENCTBL *enctbl, unsigned cflen, void *cfdata)
{
 (void) cp;
 (void) enctbl;
 (void) cflen;
 (void) cfdata;
 	// no encoding - like V.110
 return 0;
}

STATIC int v110_lcontrol(struct smdc_line_control_parms *pp)
{
 tSMChannelId cp = pp->channel;
 switch (pp->cmd) {
	int sts;
 case kSMDCLineCtlCmdInitiatorConnect:
 case kSMDCLineCtlCmdResponderConnect:
	return setfriend(&cp->chan[HANDLE_WRITE].cx, &cp->chan[HANDLE_READ], 0);
 case kSMDCLineCtlCmdDisconnect:
 	// FIXME: do something!
 	return 0;
 case kSMDCLineCtlCmdSetAux:
	sts = sendmsg3(&cp->chan[HANDLE_WRITE].cx, MSG_V110_NEW_AUX,
		pp->aux_mask, pp->aux_toggle);
	if (sts) return sts;
 	return 0;
 default:
	return err(ERR_SM_BAD_PARAMETER);
 }
}

STATIC int v110_lstatus(struct smdc_line_status_parms *pp)
{
 tSMChannelId cp = pp->channel;
 	// FIXME: set tx status from flow control & connected status
 if (cp->ur.dc.u.v110.synced) {
	if (cp->rxtx[1].dataready / 8) {
		pp->rx_status = kSMDCRxStatusReceivingData;
	} else {
		pp->rx_status = kSMDCRxStatusCarrierPresent;
	}
 } else {
	pp->rx_status = kSMDCRxStatusNoCarrier;
 }
 pp->tx_status = kSMDCTxStatusSendingCarrier;	// always for V.110
 if (!cp->ur.dc.u.v110.synced) pp->tx_cts = 0;
 if (cp->ur.dc.u.v110.last_sxe & 0x4) pp->tx_cts = 0; // last X bit received
 if (pp->link_status == kSMDCLinkStatusConnected) {
	if (cp->ur.dc.u.v110.last_sxe & 0x780f) {
		pp->link_status = kSMDCLinkStatusConnecting;
	}
 }
 return 0;
}

STATIC int v110_tstatus(struct smdc_tx_status_parms *pp, U32 *msg)
{
 (void) msg;
 pp->flow = pp->channel->ur.dc.u.v110.last_sxe & 0x4;	// last X bit received
 return 0;
}

STATIC int v110_tcontrol(struct smdc_tx_control_parms *pp)
{
 (void) pp;
 return 1;
}

STATIC int v110_rstatus(struct smdc_rx_status_parms *pp, U32 *msg)
{
 (void) msg;
 pp->flow = 0;		// FIXME: implement flow control
 return 0;
}

STATIC int v110_rcontrol(struct smdc_rx_control_parms *pp)
{
 (void) pp;
 return err(ERR_SM_BAD_PARAMETER);
}

STATIC int v110_msghandrx(tSMChannelId chan, U32 *msg)
{
 if (*msg & 0x80000000) {
	unsigned long code = *msg & 0xffff0000;
	switch (code) {
	case MESSAGE(MSG_V110_SYNC, 2):
		chan->ur.dc.u.v110.synced = msg[1];
		return 0;
	case MESSAGE(MSG_V110_NEW_AUX, 2):
		chan->ur.dc.u.v110.last_sxe = msg[1];
		return 0;
	default:
		return 1;
	}
 } else return 0;
}

LOCALDEF struct protable protable_v110 = {
	kSMDCProtocolV110,
	v110_config,
	v110_setenc,
	v110_lcontrol,
	v110_lstatus,
	v110_lcontrol,
	v110_lstatus,
	v110_tcontrol,
	v110_tstatus,
	v110_rcontrol,
	v110_rstatus,
	{ 0, v110_msghandrx, },
	0,
};
