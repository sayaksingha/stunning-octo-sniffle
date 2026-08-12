/* TiNGsmdc_rawrx.c - Data communications functions for raw receive */
#include <stddef.h>

#include "TiNGsmdc_rawrx.h"
#include "TiNGcommon.h"
#include "TiNGo_chan.h"
#include "../hsif/datacomms.h"
#include "../hsif/i460.h"
#include "../hsif/subtask.h"
#include "../hsif/typecode.h"
#include "smdc_raw.h"

#ifndef endof
#define endof(type, member) (offsetof(type, member) + sizeof(((type *) 0)->member))
#endif

STATIC int rawrx_config(struct smdc_channel_config_parms *pp)
{
 int sts = startin(pp->channel, TASKTC_DATARX);
 if (sts) return sts;
 if (pp->config_data) {
 	SMDC_RAW_CONFIG_PARMS *conf = (struct smdc_raw_config_parms *) pp->config_data;
 	CARDCONN *cx = &pp->channel->chan[HANDLE_READ].cx;
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
 		sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_I460RX);
 		if (sts) return sts;
 		sts = sendmsg2(cx, MSG_I460_MASK, i460_mask);
 		if (sts) return sts;
	}
 }
 return 0;
}

STATIC int rawrx_setenc(tSMChannelId cp, ENCTBL *enctbl, unsigned cflen, void *cfdata)
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

STATIC int rawrx_lcontrol(struct smdc_line_control_parms *pp)
{
 (void) pp;
 // nothing - always connected
 return 0;
}

STATIC int rawrx_lstatus(struct smdc_line_status_parms *pp)
{
 if (pp->channel->rxtx[1].dataready / 8) {
	pp->rx_status = kSMDCRxStatusReceivingData;
 } else {
	pp->rx_status = kSMDCRxStatusCarrierPresent;
 }
 return 0;
}

STATIC int rawrx_rstatus(struct smdc_rx_status_parms *pp, U32 *msg)
{
 (void) pp;
 (void) msg;
 return 0;
}

STATIC int rawrx_rcontrol(struct smdc_rx_control_parms *pp)
{
 (void) pp;
 return err(ERR_SM_BAD_PARAMETER);
}

STATIC int rawrx_msghand(tSMChannelId chan, U32 *msg)
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

LOCALDEF struct protable protable_rawrx = {
	kSMDCProtocolRawRx,
	rawrx_config,
	rawrx_setenc,
	0,
	0,
	rawrx_lcontrol,
	rawrx_lstatus,
	0,
	0,
	rawrx_rcontrol,
	rawrx_rstatus,
	{ 0, rawrx_msghand, },
	0,
};
