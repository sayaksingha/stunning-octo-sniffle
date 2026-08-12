/* TiNGsmdc_bulkrx.c - Data communications functions for bulk receive */

#include "TiNGsmdc_bulkrx.h"
#include "TiNGcommon.h"
#include "TiNGo_chan.h"
#include "../hsif/speed.h"
#include "../hsif/typecode.h"
#include "smdc_bulk.h"

STATIC int bulkrx_config(struct smdc_channel_config_parms *pp)
{
 int sts = startin(pp->channel, TASKTC_BULKRX);
 if (sts) return sts;
 pp->channel->rxtx[1].state = RXTXS_RUN;
 return 0;
}

STATIC int bulkrx_setenc(tSMChannelId cp, ENCTBL *enctbl, unsigned cflen, void *cfdata)
{
 (void) cp;
 (void) cflen;
 (void) cfdata;
 if (enctbl && enctbl->enctype != kSMDCConfigEncodingNone) {
 	return err(ERR_SM_BAD_PARAMETER);
 }
 return 0;
}

STATIC int bulkrx_lcontrol(struct smdc_line_control_parms *pp)
{
 (void) pp;
 // nothing - always connected
 return 0;
}

STATIC int bulkrx_lstatus(struct smdc_line_status_parms *pp)
{
 if (pp->channel->rxtx[1].dataready / 8) {
	pp->rx_status = kSMDCRxStatusReceivingData;
 } else {
	pp->rx_status = kSMDCRxStatusCarrierPresent;
 }
 return 0;
}

STATIC int bulkrx_rstatus(struct smdc_rx_status_parms *pp, U32 *msg)
{
 (void) pp;
 (void) msg;
 return 0;
}

STATIC int bulkrx_rcontrol(struct smdc_rx_control_parms *pp)
{
 (void) pp;
 return err(ERR_SM_BAD_PARAMETER);
}

STATIC int bulkrx_msghand(tSMChannelId chan, U32 *msg)
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

LOCALDEF struct protable protable_bulkrx = {
	kSMDCProtocolBulkRx,
	bulkrx_config,
	bulkrx_setenc,
	0,
	0,
	bulkrx_lcontrol,
	bulkrx_lstatus,
	0,
	0,
	bulkrx_rcontrol,
	bulkrx_rstatus,
	{ 0, bulkrx_msghand, },
	0,
};
