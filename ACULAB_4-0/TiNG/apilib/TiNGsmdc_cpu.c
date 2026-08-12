/* TiNGsmdc_cpu.c - Data communications functions for cpu monitoring */

#include "TiNGsmdc_cpu.h"
#include "TiNGcommon.h"
#include "TiNGo_chan.h"
#include "../hsif/typecode.h"
#include "smdc_cpu.h"

STATIC int cpu_config(struct smdc_channel_config_parms *pp)
{
 int sts = startin(pp->channel, TASKTC_CPUMON);
 if (sts) return sts;
 // FIXME: set line status to rx connected
 pp->channel->rxtx[1].state = RXTXS_RUN;
 return 0;
}

STATIC int cpu_setenc(tSMChannelId cp, ENCTBL *enctbl, unsigned cflen, void *cfdata)
{
 (void) cp;
 (void) cflen;
 (void) cfdata;
 if (enctbl && enctbl->enctype != kSMDCConfigEncodingNone) {
 	return err(ERR_SM_BAD_PARAMETER);
 }
 return 0;
}

STATIC int cpu_lcontrol(struct smdc_line_control_parms *pp)
{
 (void) pp;
 // nothing - always connected
 return 0;
}

STATIC int cpu_lstatus(struct smdc_line_status_parms *pp)
{
 if (pp->channel->rxtx[1].dataready / 8) {
	pp->rx_status = kSMDCRxStatusReceivingData;
 } else {
	pp->rx_status = kSMDCRxStatusCarrierPresent;
 }
 return 0;
}

STATIC int cpu_rstatus(struct smdc_rx_status_parms *pp, U32 *msg)
{
 (void) pp;
 (void) msg;
 return 0;
}

STATIC int cpu_rcontrol(struct smdc_rx_control_parms *pp)
{
 (void) pp;
 return err(ERR_SM_BAD_PARAMETER);
}

STATIC int cpu_msghand(tSMChannelId chan, U32 *msg)
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

LOCALDEF struct protable protable_cpu = {
	kSMDCProtocolCpu,
	cpu_config,
	cpu_setenc,
	0,
	0,
	cpu_lcontrol,
	cpu_lstatus,
	0,
	0,
	cpu_rcontrol,
	cpu_rstatus,
	{ 0, cpu_msghand, },
	0,
};
