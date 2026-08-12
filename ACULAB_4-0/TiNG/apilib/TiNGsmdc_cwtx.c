/* TiNGsmdc_cwtx.c - Data communications functions for CW transmit */

#include "TiNGsmdc_cwtx.h"
#include "TiNGcommon.h"
#include "TiNGo_chan.h"
#include "../hsif/cwtx.h"
#include "../hsif/kernel.h"
#include "../hsif/modem.h"
#include "../hsif/subtask.h"
#include "../hsif/typecode.h"
#include "../hsif/speed.h"
#include "smdc_cw.h"
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define SAMPRATE 8000.0
#define F2W(x) ((x) * M_PI * 2 / SAMPRATE)

STATIC int cwtx_config(struct smdc_channel_config_parms *pp)
{
 struct smdc_cw_config_parms *const conf = pp->config_data;
 const tSMChannelId cp = pp->channel;
 CARDCONN *cx = &cp->chan[HANDLE_WRITE].cx;
 struct ting_message msg;
 int sts;
 float power;
 if ((unsigned) pp->config_length < sizeof(*conf))
	return err(ERR_SM_BAD_PARAMETER);
 if (conf->user_power && !conf->set_power) return err(ERR_SM_BAD_PARAMETER);
 sts = startout(cp, TASKTC_OUTCHAN);
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_CWTX);
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_SPEED, conf->speed);
 if (sts) return sts;
 memset(&msg, 0, sizeof(msg));
 msg.ioctl_length = sizeof(msg);
 msg.flags = 0;
 msg.msg[0] = MESSAGE(MSG_CARRIER_FREQ, 3);
 msg.msg[1] = (U32)((S32)(cos(F2W(conf->space_frequency)) * 0x80000000));
 msg.msg[2] = (U32)((S32)(sin(F2W(conf->space_frequency)) * 0x80000000));
 if (ERR(cx_ioctl(cx, IOCTL_TiNG_WRMSG, &msg, sizeof(msg))))
	return err(ERR_SM_DEVERR);
 power = conf->set_power ? conf->user_power : -3.0;
 return sendmsg2(cx, MSG_CARRIER, (U32)((S32)power));
}

STATIC int cwtx_setenc(tSMChannelId cp, ENCTBL *enctbl, unsigned cflen, void *cfdata)
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

STATIC int cwtx_lcontrol(struct smdc_line_control_parms *pp)
{
 switch (pp->cmd) {
	case kSMDCLineCtlCmdSetPrefixSuffix:
	break;
 default:
	return err(ERR_SM_BAD_PARAMETER);
 }
 return 0;
}

STATIC int cwtx_lstatus(struct smdc_line_status_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 if (!cp->rxtx[0].underrun)
 	pp->tx_status = kSMDCTxStatusSendingData;
 else pp->tx_status = kSMDCTxStatusSilent;
 return 0;
}

STATIC int cwtx_tstatus(struct smdc_tx_status_parms *pp, U32 *msg)
{
 (void) pp;
 (void) msg;
 return 0;
}

STATIC int cwtx_tcontrol(struct smdc_tx_control_parms *pp)
{
 (void) pp;
 return 1;
}

STATIC int cwtx_msghand(tSMChannelId chan, U32 *msg)
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

LOCALDEF struct protable protable_cwtx = {
	kSMDCProtocolCWtx,
	cwtx_config,
	cwtx_setenc,
	cwtx_lcontrol,
	cwtx_lstatus,
	0,
	0,
	cwtx_tcontrol,
	cwtx_tstatus,
	0,
	0,
	{ cwtx_msghand, 0, },
	0,
};
