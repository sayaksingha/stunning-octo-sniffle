/* TiNGsmdc_fsktx.c - Data communications functions for FSK transmit */

#include "TiNGsmdc_fsktx.h"
#include "TiNGcommon.h"
#include "TiNGo_chan.h"
#include "../hsif/fsk.h"
#include "../hsif/kernel.h"
#include "../hsif/modem.h"
#include "../hsif/speed.h"
#include "../hsif/subtask.h"
#include "../hsif/typecode.h"
#include "smdc_fsk.h"
#include <math.h>
#include <string.h>
#include <stddef.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define SAMPRATE 8000.0
#define F2W(x) ((x) * M_PI * 2 / SAMPRATE)

STATIC int fsktx_config(struct smdc_channel_config_parms *pp)
{
 struct smdc_fsk_config_parms *const conf = pp->config_data;
 const tSMChannelId cp = pp->channel;
 CARDCONN *cx = &cp->chan[HANDLE_WRITE].cx;
 struct ting_message msg;
 int sts;
 if ((unsigned) pp->config_length < sizeof(*conf)) {
	// smaller than the current struct
	if (((unsigned) pp->config_length) < (unsigned) (sizeof(conf->user_power)+offsetof(struct smdc_fsk_config_parms,user_power))) {
		// also smaller than the old struct!
		return err(ERR_SM_BAD_PARAMETER);
	}
	// no need to default the get_metric as it is rx only
 }
 if (conf->user_power && !conf->set_power) return err(ERR_SM_BAD_PARAMETER);
 sts = startout(cp, TASKTC_OUTCHAN);
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_FSKTX);
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_SPEED, conf->speed);
 if (sts) return sts;
 memset(&msg, 0, sizeof(msg));
 msg.ioctl_length = sizeof(msg);
 msg.flags = 0;
 msg.msg[0] = MESSAGE(MSG_CARRIER_FREQ, 5);
 msg.msg[1] = (U32)((S32)(cos(F2W(conf->mark_frequency)) * 0x80000000));
 msg.msg[2] = (U32)((S32)(sin(F2W(conf->mark_frequency)) * 0x80000000));
 msg.msg[3] = (U32)((S32)(cos(F2W(conf->space_frequency)) * 0x80000000));
 msg.msg[4] = (U32)((S32)(sin(F2W(conf->space_frequency)) * 0x80000000));
 if (ERR(cx_ioctl(cx, IOCTL_TiNG_WRMSG, &msg, sizeof(msg))))
	return err(ERR_SM_DEVERR);
 cp->ut.dc.u.fsk.power = conf->set_power ? conf->user_power : -3.0;
 memset(&msg, 0, sizeof(msg));
 msg.ioctl_length = sizeof(msg);
 msg.msg[0] = MESSAGE(MSG_CARRIER_POWER, 2);
 msg.msg[1] = (U32)((S32)cp->ut.dc.u.fsk.power);
 if (ERR(cx_ioctl(cx, IOCTL_TiNG_WRMSG, &msg, sizeof(msg))))
	return err(ERR_SM_DEVERR);
 return 0;
}

STATIC int fsktx_setenc(tSMChannelId cp, ENCTBL *enctbl, unsigned cflen, void *cfdata)
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

STATIC int fsktx_lcontrol(struct smdc_line_control_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 switch (pp->cmd) {
 case kSMDCLineCtlCmdSetPrefixSuffix:
	break;
 case kSMDCLineCtlCmdAssertRTS:
	cp->ut.dc.u.fsk.carrier = 1;
	return sendmsg2(&cp->chan[HANDLE_WRITE].cx, MSG_CARRIER,
		cp->ut.dc.u.fsk.power);
 case kSMDCLineCtlCmdUnassertRTS:
	return sendmsg2(&cp->chan[HANDLE_WRITE].cx, MSG_CARRIER, -128);
 default:
	return err(ERR_SM_BAD_PARAMETER);
 }
 return 0;
}

STATIC int fsktx_lstatus(struct smdc_line_status_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 if (!cp->ut.dc.u.fsk.carrier)
	pp->tx_status = kSMDCTxStatusSilent;
 else if (!cp->rxtx[0].underrun)
 	pp->tx_status = kSMDCTxStatusSendingData;
 else pp->tx_status = kSMDCTxStatusSendingCarrier;
 return 0;
}

STATIC int fsktx_tstatus(struct smdc_tx_status_parms *pp, U32 *msg)
{
 if ((*msg & 0xffff0000) == MESSAGE(MSG_CARRIER, 2) && !pp->channel->ut.dc.u.fsk.carrier) {
	pp->status = kSMDCTxStatusEmpty; // carrier switched off, so buffer must have been empty
 }
 return 0;
}

STATIC int fsktx_tcontrol(struct smdc_tx_control_parms *pp)
{
 (void) pp;
 return 1;
}

STATIC int fsktx_msghand(tSMChannelId chan, U32 *msg)
{
 if (*msg & 0x80000000) {
	unsigned long code = *msg & 0xffff0000;
	switch (code) {
	case MESSAGE(MSG_CARRIER, 2):
		chan->ut.dc.u.fsk.carrier = msg[1];
		return 0;
	default:
		return 1;
	}
 } else return 0;
}

LOCALDEF struct protable protable_fsktx = {
	kSMDCProtocolFSKtx,
	fsktx_config,
	fsktx_setenc,
	fsktx_lcontrol,
	fsktx_lstatus,
	0,
	0,
	fsktx_tcontrol,
	fsktx_tstatus,
	0,
	0,
	{ fsktx_msghand, 0, },
	0,
};
