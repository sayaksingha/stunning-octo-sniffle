/* TiNGsmdc_rlprx.c - Data communications functions for V.110 RLP receive */

#include "TiNGsmdc_rlprx.h"
#include "TiNGcommon.h"
#include "TiNGo_chan.h"
#include "../hsif/kernel.h"
#include "../hsif/v110.h"
#include "../hsif/typecode.h"
#include "smdc_rlp.h"

STATIC int v110rlprx_config(struct smdc_channel_config_parms *pp)
{
 struct smdc_v110rlp_config_parms *cp = pp->config_data;
 tSMChannelId chan = pp->channel;
 int sts;
 if ((unsigned) pp->config_length < sizeof(*cp)) return err(ERR_SM_BAD_PARAMETER);
 sts = startin(chan, TASKTC_V110RLPRX);
 if (sts) return sts;
 return sendmsg2(&chan->chan[HANDLE_READ].cx, MSG_SET_SPEED, cp->speed);
}

STATIC int v110rlprx_setenc(tSMChannelId cp, ENCTBL *enctbl, unsigned cflen, void *cfdata)
{
 (void) cp;
 (void) enctbl;
 (void) cflen;
 (void) cfdata;
 	// no encoding - like V.110
 return 0;
}

STATIC int v110rlprx_lcontrol(struct smdc_line_control_parms *pp)
{
 (void) pp;
 // nothing - RLP is always connected
 return 0;
}

STATIC int v110rlprx_lstatus(struct smdc_line_status_parms *pp)
{
 if (pp->channel->ur.dc.u.rlp.synced) {
	if (pp->channel->rxtx[1].dataready / 8) {
		pp->rx_status = kSMDCRxStatusReceivingData;
	} else {
		pp->rx_status = kSMDCRxStatusCarrierPresent;
	}
 } else {
	pp->rx_status = kSMDCRxStatusNoCarrier;
 }
 return 0;
}

STATIC int v110rlprx_rstatus(struct smdc_rx_status_parms *pp, U32 *msg)
{
 (void) pp;
 if ((msg[0]&0xffff0000) == MESSAGE(MSG_V110_SYNC, 2)) {
	 pp->status = kSMDCRxStatusCarrier;
	 pp->u.carrier.carrier = pp->channel->ur.dc.u.rlp.synced;
 }
 return 0;
}

STATIC int v110rlprx_rcontrol(struct smdc_rx_control_parms *pp)
{
 (void) pp;
 return err(ERR_SM_BAD_PARAMETER);
}

STATIC int v110rlprx_msghand(tSMChannelId chan, U32 *msg)
{
 if (*msg & 0x80000000) {
	unsigned long code = *msg & 0xffff0000;
	switch (code) {
	case MESSAGE(MSG_V110_SYNC, 2):
		chan->ur.dc.u.rlp.synced = msg[1];
		return 0;
	default:
		return 1;
	}
 } else return 0;
}

LOCALDEF struct protable protable_rlprx = {
	kSMDCProtocolV110RLPrx,
	v110rlprx_config,
	v110rlprx_setenc,
	0,
	0,
	v110rlprx_lcontrol,
	v110rlprx_lstatus,
	0,
	0,
	v110rlprx_rcontrol,
	v110rlprx_rstatus,
	{ 0, v110rlprx_msghand, },
	0,
};
