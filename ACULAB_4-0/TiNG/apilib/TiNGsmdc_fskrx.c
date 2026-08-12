/* TiNGsmdc_fskrx.c - Data communications functions for FSK receive */

#include "TiNGsmdc_fskrx.h"
#include "TiNGcommon.h"
#include "TiNGo_chan.h"
#include "../hsif/async.h"
#include "../hsif/fsk.h"
#include "../hsif/fskrx.h"
#include "../hsif/kernel.h"
#include "../hsif/modem.h"
#include "../hsif/speed.h"
#include "../hsif/subtask.h"
#include "../hsif/typecode.h"
#include "iir4.h"
#include "smdc_fsk.h"
#include <math.h>
#include <string.h>
#include <stddef.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define SAMPRATE 8000.0
#define F2W(x) ((x) * M_PI * 2 / SAMPRATE)
#define W2F(x) ((x) / M_PI / 2 * SAMPRATE)


STATIC int fskrx_config(struct smdc_channel_config_parms *pp)
{
#define FSKRX_CONFIG_FILTER_ORDER 4

 struct smdc_fsk_config_parms *const conf = pp->config_data;
 const tSMChannelId cp = pp->channel;
 const double frequency_shift = -0.5 * (conf->mark_frequency + conf->space_frequency);
	/* Candidate cutoff: frequency to which the the Nyquist frequency
	 * of the input signal will be shifted
	 */
 double cutoff_frequency = SAMPRATE / 2 + frequency_shift;
 const double carrier_frequency = 0.5 * fabs(conf->mark_frequency - conf->space_frequency);
	/* Candidate cutoff: wide enough to include all valid signal
	 * components with no significant group delay distortion
	 */
 const double test_frequency = W2F(atan(8.0 * tan(F2W(carrier_frequency) / 2))) * 2;
 double a1[FSKRX_CONFIG_FILTER_ORDER/2], a2[FSKRX_CONFIG_FILTER_ORDER/2],
        b1[FSKRX_CONFIG_FILTER_ORDER/2], b2[FSKRX_CONFIG_FILTER_ORDER/2];
 CARDCONN *cx = &cp->chan[HANDLE_READ].cx;
 struct ting_message msg;
 double gain;
 int sts;
 unsigned long get_metric;
 unsigned long thresh_metric;
 unsigned long reverse_chan_filter;
 if ((unsigned) pp->config_length < sizeof(*conf)) {
	if (((unsigned) pp->config_length) >= (unsigned) (sizeof(conf->thresh_metric)+offsetof(struct smdc_fsk_config_parms,thresh_metric))) {
		reverse_chan_filter = 0;
		thresh_metric = conf->thresh_metric;
		get_metric    = conf->get_metric;
	} else if (((unsigned) pp->config_length) >= (unsigned) (sizeof(conf->get_metric)+offsetof(struct smdc_fsk_config_parms,get_metric))) {
		thresh_metric = 0;
		reverse_chan_filter = 0;
		get_metric    = conf->get_metric;
	} else if (((unsigned) pp->config_length) >= (unsigned) (sizeof(conf->user_power)+offsetof(struct smdc_fsk_config_parms,user_power))) {
		get_metric = 0;
		thresh_metric = 0;
		reverse_chan_filter = 0;
	} else {
		return err(ERR_SM_BAD_PARAMETER);
	}
 } else {
	get_metric          = conf->get_metric;
	thresh_metric       = conf->thresh_metric;
	reverse_chan_filter = conf->reverse_chan_filter;
 }
 sts = startin(cp, TASKTC_INCHAN);
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_FSKRX);
 if (sts) return sts;
 if (thresh_metric != 0) {
	memset(&msg, 0, sizeof(msg));
	msg.ioctl_length = sizeof(msg);
	msg.flags = 0;
	msg.msg[0] = MESSAGE(MSG_RXSIGNAL_STATS, 2);
	msg.msg[1] = thresh_metric;
	if (ERR(cx_ioctl(cx, IOCTL_TiNG_WRMSG, &msg, sizeof(msg))))
		return err(ERR_SM_DEVERR);
 }
 if (reverse_chan_filter != 0) {
	memset(&msg, 0, sizeof(msg));
	msg.ioctl_length = sizeof(msg);
	msg.flags = 0;
	msg.msg[0] = MESSAGE(MSG_REVERSE_CHAN_FILT, 2);
	msg.msg[1] = reverse_chan_filter;
	if (ERR(cx_ioctl(cx, IOCTL_TiNG_WRMSG, &msg, sizeof(msg))))
		return err(ERR_SM_DEVERR);
 }
 if (conf->mark_frequency > conf->space_frequency) {
	memset(&msg, 0, sizeof(msg));
	msg.ioctl_length = sizeof(msg);
	msg.flags = 0;
	msg.msg[0] = MESSAGE(MSG_INVERT, 2);
	msg.msg[1] = 1;
	if (ERR(cx_ioctl(cx, IOCTL_TiNG_WRMSG, &msg, sizeof(msg))))
		return err(ERR_SM_DEVERR);
 }
 memset(&msg, 0, sizeof(msg));
 msg.ioctl_length = sizeof(msg);
 msg.flags = 0;
 msg.msg[0] = MESSAGE(MSG_FSKRX, 12);
	  // Choose the lowest candidate
 if (test_frequency < cutoff_frequency) cutoff_frequency = test_frequency;
 create_iir_4(a1, a2, b1, b2, &gain, 100, cutoff_frequency / (0.5 * SAMPRATE));
 msg.msg[1] = roundfu(cos(F2W(frequency_shift)) * 0x01000000);
 msg.msg[2] = roundfu(sin(F2W(frequency_shift)) * 0x01000000);
 msg.msg[3] = roundfu(gain * 0x01000000);
 msg.msg[4] = roundfu(b2[1] * 0x01000000);
 msg.msg[5] = roundfu(b1[1] * 0x01000000);
 msg.msg[6] = roundfu(a2[0] * 0x01000000);	// dominant zero first
 msg.msg[7] = roundfu(a1[0] * 0x01000000);
 msg.msg[8] = roundfu(b2[0] * 0x01000000);	// dominant pole last
 msg.msg[9] = roundfu(b1[0] * 0x01000000);
 msg.msg[10] = roundfu(a2[1] * 0x01000000);
 msg.msg[11] = roundfu(a1[1] * 0x01000000);
 if (ERR(cx_ioctl(cx, IOCTL_TiNG_WRMSG, &msg, sizeof(msg))))
	return err(ERR_SM_DEVERR);
 cp->ur.dc.u.fsk.speed = conf->speed;
 cp->ur.dc.u.fsk.filtthresh = roundfu(0.5 * carrier_frequency);
 cp->ur.dc.u.fsk.rx_carrier_off_mS = conf->rx_carrier_off_mS;
 cp->ur.dc.u.fsk.rx_carrier_on_mS = conf->rx_carrier_on_mS;
 cp->ur.dc.u.fsk.gensigstats = get_metric;
 return 0;
#undef FSKRX_CONFIG_FILTER_ORDER
}

STATIC int fskrx_setenc(tSMChannelId cp, ENCTBL *enctbl, unsigned cflen, void *cfdata)
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
	struct smdc_async_format_parms *const ap = cfdata;
	unsigned databits = 8;		// only used for async
	if (cflen < sizeof(*ap)) return err(ERR_SM_BAD_PARAMETER);
	if (ap->databits) databits = ap->databits;
	if (databits > 14) return err(ERR_SM_BAD_PARAMETER);
	sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_FSKASYRX);
	if (sts) return sts;
	if (databits != 8) {
		sts = sendmsg2(cx, MSG_DATABITS, databits);
		if (sts) return sts;
	}
 } else {
	sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_FSKPLL);
	if (sts) return sts;
	sts = enctbl->encfn(cp, 1, cfdata, cflen);
	if (sts) return sts;
 }
 cp->ur.dc.encoding = enctbl;
 sts = sendmsg2(cx, MSG_SPEED, cp->ur.dc.u.fsk.speed);
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_THRESH, cp->ur.dc.u.fsk.filtthresh);
 if (sts) return sts;
 sts = sendmsg3(cx, MSG_CDTIME,
 	cp->ur.dc.u.fsk.rx_carrier_on_mS, cp->ur.dc.u.fsk.rx_carrier_off_mS);
 if (sts) return sts;
 return sts;
}

STATIC int fskrx_lcontrol(struct smdc_line_control_parms *pp)
{
 (void) pp;
 // nothing - fskrx is always connected
 return 0;
}

STATIC int fskrx_lstatus(struct smdc_line_status_parms *pp)
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

STATIC int fskrx_rstatus(struct smdc_rx_status_parms *pp, U32 *msg)
{
 (void) pp;
 if ((msg[0]&0xffff0000) == MESSAGE(MSG_CARRIER, 2)) {
	 pp->status = kSMDCRxStatusCarrier;
	 pp->u.carrier.carrier = pp->channel->ur.dc.u.fsk.carrier;
 }

 if ((msg[0]&0xffff0000) == MESSAGE(MSG_RXSIGNAL_STATS, 2)) {
	 if (pp->channel->ur.dc.u.fsk.gensigstats) {
		pp->status = kSMDCRxStatusSigMetric;
		pp->u.metric.metric = pp->channel->ur.dc.u.fsk.sigstats;
	 }
 }
 return 0;
}

STATIC int fskrx_rcontrol(struct smdc_rx_control_parms *pp)
{
 (void) pp;
 return err(ERR_SM_BAD_PARAMETER);
}

STATIC int fskrx_msghand(tSMChannelId chan, U32 *msg)
{
 if (*msg & 0x80000000) {
	unsigned long code = *msg & 0xffff0000;
	switch (code) {
	case MESSAGE(MSG_CARRIER, 2):
		chan->ur.dc.u.fsk.carrier = msg[1];
		if (chan->ur.dc.u.fsk.carrier && chan->ur.dc.u.fsk.gensigstats) {
			CARDCONN *cx = &chan->chan[HANDLE_READ].cx;

			int sts = sendmsg1(cx, MSG_RXSIGNAL_STATS);
			if (sts) return sts;
		}
		return 0;
	case MESSAGE(MSG_RXSIGNAL_STATS, 2):
		chan->ur.dc.u.fsk.sigstats = msg[1];
		return 0;
	default:
		return 1;
	}
 } else return 0;
}

LOCALDEF struct protable protable_fskrx = {
	kSMDCProtocolFSKrx,
	fskrx_config,
	fskrx_setenc,
	0,
	0,
	fskrx_lcontrol,
	fskrx_lstatus,
	0,
	0,
	fskrx_rcontrol,
	fskrx_rstatus,
	{ 0, fskrx_msghand, },
	0,
};
