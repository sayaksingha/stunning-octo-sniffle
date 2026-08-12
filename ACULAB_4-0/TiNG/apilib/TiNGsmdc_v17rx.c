/* TiNGsmdc_v17rx.c - Data communications functions for V.17 receive */

#include "TiNGsmdc_v17rx.h"
#include "TiNGcommon.h"
#include "TiNGo_chan.h"
#include "../hsif/kernel.h"
#include "../hsif/modem.h"
#include "../hsif/speed.h"
#include "../hsif/subtask.h"
#include "../hsif/typecode.h"
#include "smdc_v17.h"

STATIC int v17rx_config(struct smdc_channel_config_parms *pp)
{
 struct smdc_v17_config_parms *const conf = pp->config_data;
 const tSMChannelId cp = pp->channel;
 CARDCONN *cx = &cp->chan[HANDLE_READ].cx;
 int sts;
 if ((unsigned) pp->config_length < sizeof(*conf))
	return err(ERR_SM_BAD_PARAMETER);
 sts = startin(cp, TASKTC_INCHAN);
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_V17RX);
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_SPEED, conf->speed);
 if (sts) return sts;
 return 0;
}

STATIC int v17rx_setenc(tSMChannelId cp, ENCTBL *enctbl, unsigned cflen, void *cfdata)
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

STATIC int v17rx_lcontrol(struct smdc_line_control_parms *pp)
{
 (void) pp;
 // nothing - v17rx is always connected
 return 0;
}

STATIC int v17rx_lstatus(struct smdc_line_status_parms *pp)
{
 if (pp->channel->rxtx[1].dataready / 8) {
	pp->rx_status = kSMDCRxStatusReceivingData;
 } else {
	if (pp->channel->ur.dc.u.v17.carrier) {
		pp->rx_status = kSMDCRxStatusCarrierPresent;
	} else pp->rx_status = kSMDCRxStatusNoCarrier;
 }
 return 0;
}

STATIC int v17rx_rstatus(struct smdc_rx_status_parms *pp, U32 *msg)
{
 (void) pp;
 if ((msg[0]&0xffff0000) == MESSAGE(MSG_CARRIER, 2)) {
	 pp->status = kSMDCRxStatusCarrier;
	 pp->u.carrier.carrier = pp->channel->ur.dc.u.v17.carrier;
 }
 return 0;
}

STATIC int v17rx_rcontrol(struct smdc_rx_control_parms *pp)
{
 (void) pp;
 return err(ERR_SM_BAD_PARAMETER);
}

STATIC int v17rx_msghand(tSMChannelId chan, U32 *msg)
{
 if (*msg & 0x80000000) {
	unsigned long code = *msg & 0xffff0000;
	switch (code) {
	case MESSAGE(MSG_CARRIER, 2):
		chan->ur.dc.u.v17.carrier = msg[1];
		return 0;
	default:
		return 1;
	}
 } else return 0;
}

LOCALDEF struct protable protable_v17rx = {
	kSMDCProtocolV17rx,
	v17rx_config,
	v17rx_setenc,
	0,
	0,
	v17rx_lcontrol,
	v17rx_lstatus,
	0,
	0,
	v17rx_rcontrol,
	v17rx_rstatus,
	{ 0, v17rx_msghand, },
	0,
};
