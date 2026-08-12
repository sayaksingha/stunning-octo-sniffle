/* TiNGsmdc_rlptx.c - Data communications functions for V.110 RLP transmit */

#include "TiNGsmdc_rlptx.h"
#include "TiNGcommon.h"
#include "TiNGo_chan.h"
#include "../hsif/v110.h"
#include "../hsif/typecode.h"
#include "smdc_rlp.h"

STATIC int v110rlptx_config(struct smdc_channel_config_parms *pp)
{
 struct smdc_v110rlp_config_parms *cp = pp->config_data;
 tSMChannelId chan = pp->channel;
 int sts;
 if ((unsigned) pp->config_length < sizeof(*cp)) return err(ERR_SM_BAD_PARAMETER);
 sts = startout(chan, TASKTC_V110RLPTX);
 if (sts) return sts;
 return sendmsg2(&chan->chan[HANDLE_WRITE].cx, MSG_SET_SPEED, cp->speed);
}

STATIC int v110rlptx_setenc(tSMChannelId cp, ENCTBL *enctbl, unsigned cflen, void *cfdata)
{
 (void) cp;
 (void) enctbl;
 (void) cflen;
 (void) cfdata;
 	// no encoding - like V.110
 return 0;
}

STATIC int v110rlptx_lcontrol(struct smdc_line_control_parms *pp)
{
 tSMChannelId chan = pp->channel;
 switch (pp->cmd) {
	int sts;
 case kSMDCLineCtlCmdAssertRTS:
 case kSMDCLineCtlCmdUnassertRTS:
	sts = sendmsg2(&chan->chan[HANDLE_WRITE].cx, MSG_V110_NEW_AUX,
		pp->cmd == kSMDCLineCtlCmdAssertRTS);
	if (sts) return sts;
	break;
 default:
	return err(ERR_SM_BAD_PARAMETER);
 }
 return 0;
}

STATIC int v110rlptx_lstatus(struct smdc_line_status_parms *pp)
{
 	// FIXME: should set tx_status if not silent
 pp->tx_status = kSMDCTxStatusSendingCarrier;
 return 0;
}

STATIC int v110rlptx_tstatus(struct smdc_tx_status_parms *pp, U32 *msg)
{
 (void) pp;
 (void) msg;
 return 0;
}

STATIC int v110rlptx_tcontrol(struct smdc_tx_control_parms *pp)
{
 (void) pp;
 return 1;
}

STATIC int v110rlptx_msghand(tSMChannelId chan, U32 *msg)
{
 (void) chan;
 if (*msg & 0x80000000) {
	unsigned long code = *msg & 0xffff0000;
	switch (code) {
	default:
		return 1;
	}
 } else return 0;
}

LOCALDEF struct protable protable_rlptx = {
	kSMDCProtocolV110RLPtx,
	v110rlptx_config,
	v110rlptx_setenc,
	v110rlptx_lcontrol,
	v110rlptx_lstatus,
	0,
	0,
	v110rlptx_tcontrol,
	v110rlptx_tstatus,
	0,
	0,
	{ v110rlptx_msghand, 0, },
	0,
};
