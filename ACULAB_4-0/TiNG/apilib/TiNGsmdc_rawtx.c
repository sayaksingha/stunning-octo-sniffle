/* TiNGsmdc_rawtx.c - Data communications functions for raw transmit */
#include <stddef.h>

#include "TiNGsmdc_rawtx.h"
#include "TiNGcommon.h"
#include "TiNGo_chan.h"
#include "../hsif/datacomms.h"
#include "../hsif/i460.h"
#include "../hsif/typecode.h"
#include "smdc_raw.h"

#ifndef endof
#define endof(type, member) (offsetof(type, member) + sizeof(((type *) 0)->member))
#endif

STATIC int rawtx_config(struct smdc_channel_config_parms *pp)
{
 int sts = startout(pp->channel, TASKTC_DATATX);
 if (sts) return sts;
 if (pp->config_data) {
 	SMDC_RAW_CONFIG_PARMS *conf = (struct smdc_raw_config_parms *) pp->config_data;
 	CARDCONN *cx = &pp->channel->chan[HANDLE_WRITE].cx;
 	unsigned i460_mask = 0;
 	unsigned xorval = 0;
 	if ((unsigned) pp->config_length < sizeof(*conf)) {
 		return err(ERR_SM_BAD_PARAMETER);
	}
 	if (pp->config_length + 0u >= endof(SMDC_RAW_CONFIG_PARMS, i460_mask)) {
 		i460_mask = conf->i460_mask;
		if (i460_mask > 0xff) return err(ERR_SM_BAD_PARAMETER);
		if (i460_mask == 0xff) i460_mask = 0;
	}
 	if (pp->config_length + 0u >= endof(SMDC_RAW_CONFIG_PARMS, xorval)) {
 		xorval = conf->xorval;
		if (xorval > 0xff) return err(ERR_SM_BAD_PARAMETER);
	}
 	if (xorval) sts = sendmsg2(cx, MSG_XORVAL, xorval);
 	if (i460_mask) {
 		sts = sendmsg2(cx, MSG_I460_MASK, i460_mask);
 		if (sts) return sts;
	}
 }
 return 0;
}

STATIC int rawtx_setenc(tSMChannelId cp, ENCTBL *enctbl, unsigned cflen, void *cfdata)
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

STATIC int rawtx_lcontrol(struct smdc_line_control_parms *pp)
{
 switch (pp->cmd) {
 case kSMDCLineCtlCmdSetPrefixSuffix:
	break;
 default:
	// no generic handling
	return err(ERR_SM_BAD_PARAMETER);
 }
 return 0;
}

STATIC int rawtx_lstatus(struct smdc_line_status_parms *pp)
{
 if (!pp->channel->rxtx[0].underrun)
 	pp->tx_status = kSMDCTxStatusSendingData;
 else pp->tx_status = kSMDCTxStatusSendingCarrier;
 return 0;
}

STATIC int rawtx_tstatus(struct smdc_tx_status_parms *pp, U32 *msg)
{
 (void) pp;
 (void) msg;
 return 0;
}

STATIC int rawtx_tcontrol(struct smdc_tx_control_parms *pp)
{
 (void) pp;
 return 1;
}

STATIC int rawtx_msghand(tSMChannelId chan, U32 *msg)
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

LOCALDEF struct protable protable_rawtx = {
	kSMDCProtocolRawTx,
	rawtx_config,
	rawtx_setenc,
	rawtx_lcontrol,
	rawtx_lstatus,
	0,
	0,
	rawtx_tcontrol,
	rawtx_tstatus,
	0,
	0,
	{ rawtx_msghand, 0, },
	0,
};
