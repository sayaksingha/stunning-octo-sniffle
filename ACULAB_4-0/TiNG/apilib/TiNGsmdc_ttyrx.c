/* TiNGsmdc_ttyrx.c - Data communications functions for TTY receive */

#include "TiNGsmdc_ttyrx.h"
#include "TiNGcommon.h"
#include "TiNGo_chan.h"
#include "../hsif/kernel.h"
#include "../hsif/ttyrx.h"
#include "../hsif/subtask.h"
#include "../hsif/typecode.h"
#include "smdc_ttyrx.h"
#include <stddef.h>

STATIC int ttyrx_config(struct smdc_channel_config_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 CARDCONN *cx = &cp->chan[HANDLE_READ].cx;
 int sts = startin(cp, TASKTC_INCHAN);
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_TTYASYRX);
 return sts;
}

STATIC int ttyrx_setenc(tSMChannelId cp, ENCTBL *enctbl, unsigned cflen, void *cfdata)
{
 (void) cp;
 (void) cflen;
 (void) cfdata;
 if (enctbl && enctbl->enctype != kSMDCConfigEncodingNone) {
 	return err(ERR_SM_BAD_PARAMETER);
 }
 return 0;
}

STATIC int ttyrx_lcontrol(struct smdc_line_control_parms *pp)
{
 (void) pp;
 // nothing - ttyrx is always connected
 return 0;
}

STATIC int ttyrx_lstatus(struct smdc_line_status_parms *pp)
{
 if (pp->channel->rxtx[1].dataready / 8) {
	pp->rx_status = kSMDCRxStatusReceivingData;
 } else {
	if (pp->channel->ur.dc.u.fsk.carrier) {
		pp->rx_status = kSMDCRxStatusCarrierPresent;
	} else pp->rx_status = kSMDCRxStatusNoCarrier;
 }
 return 0;
}

STATIC int ttyrx_rstatus(struct smdc_rx_status_parms *pp, U32 *msg)
{
 (void) pp;
 if ((msg[0]&0xffff0000) == MESSAGE(MSG_TTYASYRX_BITLN, 2)) {
	 pp->status = kSMDCRxStatusCarrier;
	 pp->u.carrier.carrier = pp->channel->ur.dc.u.tty.carrier;
 }
 return 0;
}

STATIC int ttyrx_rcontrol(struct smdc_rx_control_parms *pp)
{
 if (pp->cmd == kSMDCRxCtlResume) {
	 return sendmsg2(&pp->channel->chan[HANDLE_READ].cx, MSG_TTYASYRX_BAUDOT_MODE, pp->blocking);
 }
 return err(ERR_SM_BAD_PARAMETER);
}

STATIC int ttyrx_msghand(tSMChannelId chan, U32 *msg)
{
 if (*msg & 0x80000000) {
	unsigned long code = *msg & 0xffff0000;
	switch (code) {
	case MESSAGE(MSG_TTYASYRX_BITLN, 2):
		chan->ur.dc.u.tty.carrier = msg[1];
		return 0;
	default:
		return 1;
	}
 } else return 0;
}

LOCALDEF struct protable protable_ttyrx = {
	kSMDCProtocolTTYrx,
	ttyrx_config,
	ttyrx_setenc,
	0,
	0,
	ttyrx_lcontrol,
	ttyrx_lstatus,
	0,
	0,
	ttyrx_rcontrol,
	ttyrx_rstatus,
	{ 0, ttyrx_msghand, },
	0,
};
