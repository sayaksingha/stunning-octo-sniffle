/* TiNGsmdc_cwrx.c - Data communications functions for CW receive */

#include "TiNGsmdc_cwrx.h"
#include "TiNGcommon.h"
#include "TiNGo_chan.h"
#include "../hsif/cwrx.h"
#include "../hsif/kernel.h"
#include "../hsif/modem.h"
#include "../hsif/speed.h"
#include "../hsif/subtask.h"
#include "../hsif/typecode.h"
#include "smdc_cw.h"
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define SAMPRATE 8000.0
#define F2W(x) ((x) * M_PI * 2 / SAMPRATE)
#define F1W(x) ((x) * M_PI * 1 / SAMPRATE)


STATIC int cwrx_config(struct smdc_channel_config_parms *pp)
{
 struct smdc_cw_config_parms *const conf = pp->config_data;
 const tSMChannelId cp = pp->channel;
 CARDCONN *cx = &cp->chan[HANDLE_READ].cx;
 struct ting_message msg;
 int sts;
 if ((unsigned) pp->config_length < sizeof(*conf))
	return err(ERR_SM_BAD_PARAMETER);
 if (conf->space_frequency < (SAMPRATE/32))
	return err(ERR_SM_BAD_PARAMETER);
 if (conf->space_frequency > (SAMPRATE/4))
	return err(ERR_SM_BAD_PARAMETER);
 sts = startin(cp, TASKTC_INCHAN);
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_CWRX);
 if (sts) return sts;
 memset(&msg, 0, sizeof(msg));
 msg.ioctl_length = sizeof(msg);
 msg.flags = 0;
 msg.msg[0] = MESSAGE(MSG_CWRX, 4);
 msg.msg[1] = (U32)((S32) (2 * cos(F2W(conf->space_frequency)) * 0x01000000));
 msg.msg[2] = (U32)((S32) (2 * cos(F2W(conf->space_frequency + 50)) * 0x01000000));
 msg.msg[3] = (U32)((S32) (2 * cos(F2W(conf->space_frequency - 50)) * 0x01000000));
 if (ERR(cx_ioctl(cx, IOCTL_TiNG_WRMSG, &msg, sizeof(msg))))
	return err(ERR_SM_DEVERR);
 cp->ur.dc.u.cw.speed = conf->speed;
 memset(&msg, 0, sizeof(msg));
 msg.ioctl_length = sizeof(msg);
 msg.flags = 0;
 msg.msg[0] = MESSAGE(MSG_CWRX, 5);
 msg.msg[1] = roundfu(SAMPRATE/conf->space_frequency);
 msg.msg[2] = (U32)((S32) (2 * sin(M_PI/msg.msg[1]) * 0x01000000));
 msg.msg[3] = (U32)((S32) (2 * cos(M_PI/msg.msg[1]) * 0x01000000));
 msg.msg[4] = conf->speed;
 if (ERR(cx_ioctl(cx, IOCTL_TiNG_WRMSG, &msg, sizeof(msg))))
	return err(ERR_SM_DEVERR);
 return 0;
}

STATIC int cwrx_setenc(tSMChannelId cp, ENCTBL *enctbl, unsigned cflen, void *cfdata)
{
 CARDCONN *cx = &cp->chan[HANDLE_READ].cx;
 int sts;
 if (cp->ur.dc.encoding) {
	ENCTBL *old = cp->ur.dc.encoding;
	if (old->enctype == kSMDCConfigEncodingAsync) {
		sts = sendmsg2(cx, MSG_DELSUB, SUBTASKTC_FSKASYRX);
		if (sts) return sts;
	} else {
		sts = sendmsg2(cx, MSG_DELSUB, SUBTASKTC_FSKPLL);
		if (sts) return sts;
		sts = old->stopfn(cp, 1);
		if (sts) return sts;
	}
	cp->ur.dc.encoding = 0;
 }
 if (enctbl->enctype == kSMDCConfigEncodingAsync) {
	sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_FSKASYRX);
 } else {
	sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_FSKPLL);
	if (sts) return sts;
	sts = enctbl->encfn(cp, 1, cfdata, cflen);
 }
 if (!sts) {
 	sts = sendmsg2(cx, MSG_SPEED, cp->ur.dc.u.cw.speed);
	cp->ur.dc.encoding = enctbl;
 }
 return sts;
}

STATIC int cwrx_lcontrol(struct smdc_line_control_parms *pp)
{
 (void) pp;
 // nothing - cwrx is always connected
 return 0;
}

STATIC int cwrx_lstatus(struct smdc_line_status_parms *pp)
{
 if (pp->channel->rxtx[1].dataready / 8) {
	pp->rx_status = kSMDCRxStatusReceivingData;
 } else {
	pp->rx_status = kSMDCRxStatusCarrierPresent;
 }
 return 0;
}

STATIC int cwrx_rstatus(struct smdc_rx_status_parms *pp, U32 *msg)
{
 (void) pp;
 (void) msg;
 return 0;
}

STATIC int cwrx_rcontrol(struct smdc_rx_control_parms *pp)
{
 (void) pp;
 return err(ERR_SM_BAD_PARAMETER);
}

STATIC int cwrx_msghand(tSMChannelId chan, U32 *msg)
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

LOCALDEF struct protable protable_cwrx = {
	kSMDCProtocolCWrx,
	cwrx_config,
	cwrx_setenc,
	0,
	0,
	cwrx_lcontrol,
	cwrx_lstatus,
	0,
	0,
	cwrx_rcontrol,
	cwrx_rstatus,
	{ 0, cwrx_msghand, },
	0,
};
