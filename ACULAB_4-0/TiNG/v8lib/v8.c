/* v8.h - the V.8 protocol */

#include "v8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../apilib/smbesp.h"
#include "../apilib/smdc.h"
#include "../apilib/smdc_fsk.h"
#include "../apilib/smdc_sync.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

#define MIN_OCTET_CAPACITY 10

#define CJ_WAIT_COUNT 2

int (*v8_showtrace)(const char *fmt, va_list ap) = 0;

static int stop_v21(struct v8 *v8);

static int outputlog(const char *fmt, ...)
{
 va_list ap;
 int r;
 va_start(ap, fmt);
 if (v8_showtrace == NULL) {
   r = (*TiNG_showtrace)(fmt, ap);
 } else {
   r = (*v8_showtrace)(fmt, ap);
 }
 va_end(ap);
 return r;
}

static int errorin(int rc, char *what)
{
 if (TiNGtrace) outputlog("Cannot %s (%d)\n", what, rc);
 return rc;
}

static int start_ansam(struct v8 *v8)
{
 static struct sm_play_tone_item ansam[] = {
	{ // four seconds of ANSam
	 kSMToneOperationModulate,
	 4000,
	 2100, -12,
	 15, -14,
	},
	{ // two seconds silence, to give...
	 kSMToneOperationSum,
	 2000,
	 0, 0,
	 0, 0,
	},
	{ // ...timeout at six seconds
	 kSMToneOperationStop,
	 0, 0, 0, 0, 0,
	}
 };
 SM_PLAY_TONE_LIST_PARMS ptl;
 int rc;
 memset(&ptl, 0, sizeof(ptl));
 ptl.channel = v8->channel;
 ptl.tones = ansam;
 ptl.tone_count = arlen(ansam);
 rc = sm_play_tone_list(&ptl);
 if (rc) return errorin(rc, "play ANSam tone");
 v8->await_write_event = 1;
 return 0;
}

static int listen_ansam(struct v8 *v8, enum kSMANSMode mode)
{
	SM_ANS_LISTEN_FOR_PARMS alp;
	int rc;
	memset(&alp, 0, sizeof(alp));
	alp.channel = v8->channel;
	alp.detection_mode = mode;
	rc = sm_ans_listen_for(&alp);
	if (!rc) v8->await_recognition_event = (mode != kSMANSModeDisable);
	return rc;
}

static int start_v21rx(struct v8 *v8, int ishigh)
{
 SMDC_CHANNEL_CONFIG_PARMS cfg;
 SMDC_FSK_CONFIG_PARMS fsk;
 SMDC_RX_CONTROL_PARMS rxp;
 int rc;
 memset(&fsk, 0, sizeof(fsk));
 fsk.speed = 300;
 fsk.mark_frequency = ishigh ? 1650.0f : 980.0f;
 fsk.space_frequency = ishigh ? 1850.0f : 1180.0f;
 fsk.rx_carrier_on_mS = 10; // HDLC flags detection needs a threshold less than the time for 6 bits
 fsk.rx_carrier_off_mS = 5;
 fsk.get_metric = 1;
 fsk.reverse_chan_filter = ishigh ? 2 : 1;  // 2 blocks echo of 1180 and below, 1 blocks 1650 and above
 memset(&cfg, 0, sizeof(cfg));
 cfg.channel = v8->channel;
 cfg.protocol = kSMDCProtocolFSKrx;
 cfg.config_data = &fsk;
 cfg.config_length = sizeof(fsk);
 cfg.encoding = kSMDCConfigEncodingSync;
 cfg.encoding_config_data = 0;
 cfg.encoding_config_length = 0;
 rc = smdc_channel_config(&cfg);
 if (rc) return errorin(rc, "set configuration for V.21 rx");
 memset(&rxp, 0, sizeof(rxp));
 rxp.channel = v8->channel;
 rxp.cmd = kSMDCRxCtlNotifyOnData;
 rxp.min_to_collect = 1;
 rc = smdc_rx_control(&rxp);
 if (rc) return errorin(rc, "set threshold for V.21 rx");
 v8->await_read_event = 1;
 return 0;
}

static int start_v21tx(struct v8 *v8, int ishigh)
{
 SMDC_CHANNEL_CONFIG_PARMS cfg;
 SMDC_FSK_CONFIG_PARMS fsk;
 SMDC_TX_CONTROL_PARMS txp;
 SMDC_LINE_CONTROL_PARMS lcp;
 int rc;
 memset(&fsk, 0, sizeof(fsk));
 fsk.speed = 300;
 fsk.mark_frequency = ishigh ? 1650.0f : 980.0f;
 fsk.space_frequency = ishigh ? 1850.0f : 1180.0f;
 fsk.set_power = 1;
 fsk.user_power = -12;
 memset(&cfg, 0, sizeof(cfg));
 cfg.channel = v8->channel;
 cfg.protocol = kSMDCProtocolFSKtx;
 cfg.config_data = &fsk;
 cfg.config_length = sizeof(fsk);
 cfg.encoding = kSMDCConfigEncodingSync;
 cfg.encoding_config_data = 0;
 cfg.encoding_config_length = 0;
 rc = smdc_channel_config(&cfg);
 if (rc) return errorin(rc, "set configuration for V.21 tx");
 memset(&txp, 0, sizeof(txp));
 txp.channel = v8->channel;
 txp.cmd = kSMDCTxCtlNotifyOnCapacity;
 txp.capacity = 0 - MIN_OCTET_CAPACITY;
 rc = smdc_tx_control(&txp);
 if (rc) return errorin(rc, "set threshold for V.21 tx");
 memset(&lcp, 0, sizeof(lcp));
 lcp.channel = v8->channel;
 lcp.cmd = kSMDCLineCtlCmdAssertRTS;
 rc = smdc_line_control(&lcp);
 if (rc) return errorin(rc, "assert RTS V.21 tx");
 v8->await_write_event = 1;
 v8->outpos = 0;
 v8->tottx = 0;
 return 0;
}

static void v8_start(struct v8 *v8, tSMChannelId chan)
{
 v8->channel = chan;
 v8->asyrx.inbits = 0;
 v8->asyrx.cc = 0;
 v8->outpos = 0;
 v8->tottx = 0;
 v8->in_msglen = 0;
 v8->in.len = 0;
 v8->out.len = 0;
 v8->sending_ansam = 0;
 v8->ans_status = V8ANS_STS_NONE;
 v8->await_recognition_event = 0;
 v8->await_read_event = 0;
 v8->await_write_event = 0;
 v8->CJ_wait_count = 0;
 v8->hdlc_prev = 0;
 v8->have_carrier = 0;
 v8->ci[0] = 0;
 v8->ci[1] = 0;
 v8->full_ansam = 0;
 v8->rxfsk_metric = 0;
 v8->flags_detected = 0;
}

int v8_start_answer(struct v8 *v8, tSMChannelId chan)
{
 int rc;
 v8_start(v8, chan);
 rc = start_ansam(v8);
 if (rc) return rc;
 v8->sending_ansam = 1;
 rc = start_v21rx(v8, 0);
 if (rc) return rc;
 v8->status = V8STS_AWAIT_CI;
 return 0;
}

int v8_do_ansam_phase_reversals(struct v8 *v8)
{
 if (v8->sending_ansam) {
	SM_PLAY_TONE_LIST_PHASE_REVERSE_PARMS ptlpr;
	int rc;
	memset(&ptlpr, 0, sizeof(ptlpr));
	ptlpr.channel = v8->channel;
	ptlpr.period = 450;
	rc = sm_play_tone_list_phase_reverse(&ptlpr);
	if (rc) return errorin(rc, "phase reverse ANSam tone");
 }
 return 0;
}


int v8_start_send_ci(struct v8 *v8, tSMChannelId chan)
{
 v8_start(v8, chan);
 v8->status = V8STS_CONFIG_CI;
 return 0;
}

int v8_start_send_cm(struct v8 *v8, tSMChannelId chan)
{
 v8_start(v8, chan);
 v8->status = V8STS_CONFIG_CM;
 return 0;
}

#define kMinFlags 4
#define kMinFlagsNotJM 3

static int hdlc_flags(struct v8 *v8, unsigned in)
{
	int i,bit;

	// HDLC flags should not appear in a V.8 bit stream, even a single flag,
	// see V.8 spec
	// If we receive HDLC flags stop V.8 and hand over to T.30
	// since HDLC is synchronous they should be one after another, eg 0x7e7e

	for (i = 0; i < 8; i++) {
		bit = (in & (1<<i)) ? 1 : 0;

		if (v8->hdlc_prev < 128) {
			int getStuffed = 0;

			unsigned char flagCount  = (v8->hdlc_prev >> 3) & 0xf;
			unsigned char flagPos    = (v8->hdlc_prev & 7);

			if (!bit) {
				if (flagPos == 0) {
					flagPos = 1;
				} else if (flagPos == 7) {
					flagPos = 0;

					if (flagCount < kMinFlags) {
						flagCount += 1;

						if ((v8->status != V8STS_SENDING_CM_AWAIT_JM) && (flagCount >= kMinFlagsNotJM) && (v8->flags_detected != -1)) {
							return 1;
						}
					}
				} else {
					flagCount = 0;
					flagPos = 1;
				}
			} else if ((flagPos >= 1) && (flagPos <= 6)) {
				flagPos += 1;
			} else if ((flagPos == 0) && (flagCount >= kMinFlags)) {
				getStuffed = 1;
			} else {
				flagCount = 0;
				flagPos = 0;
			}

			v8->hdlc_prev = (unsigned char) ((getStuffed) ? 128 : ((flagCount << 3) | flagPos));
		}

		if (v8->hdlc_prev >= 128) {
			int reset = 0;
			unsigned char stuffedPos = (v8->hdlc_prev & 0x7f);

			if ((stuffedPos == 5) || (stuffedPos == 11) || (stuffedPos == 12)) {
				if (!bit) {
					if (stuffedPos == 12) {
						return 1;
					}

					stuffedPos += 1;
				} else {
					reset = 1;
				}
			} else {
				if (bit) {
					stuffedPos += 1;
				} else {
					reset = 1;
				}
			}

			v8->hdlc_prev = (unsigned char) ((reset) ? 0 : (128 + stuffedPos));
		}
	}

	return 0;
}


static int asyrx_byte(struct v8 *v8, unsigned in)
{
	unsigned int in_alt = in;
	unsigned int in_len;
	int rc;

	switch (v8->status) {

	case V8STS_AWAIT_CI:
		/* CI (introduced by 0x00) and CM (by 0xe0) are both valid */
		if (in == 0)
			in_alt = 0xe0;
		/* Fallthrough */
	case V8STS_GOT_CI_AWAIT_CM:
	case V8STS_SENDING_CM_AWAIT_JM:
		in_len = v8->in.len;
		if (in_alt == 0xe0) {
			/* Start of sequence byte */
			if (in_len == 0 || v8->in.buf[0] != in) {
				/* Start of sequence, just save. */
				v8->in_msglen = 0;
				in_len = 0;
			} else if (in_len != v8->in_msglen) {
				/* End of first copy or truncated - wait for 2nd copy */
				v8->in_msglen = in_len;
				in_len = 0;
			} else {
				/* Got a valid sequence (2 copies of the same data) */
				v8->in.len = 0;
				switch (v8->status) {
				case V8STS_AWAIT_CI:
					if (in == 0) {
						/* Got CI - should only be one byte */
						v8->status = V8STS_GOT_CI_AWAIT_CM;
						v8->ci[1] = v8->in.buf[1];
						break;
					}
					/* FALLTHROUGH */
				case V8STS_GOT_CI_AWAIT_CM:
					v8->status = V8STS_GOT_CM_CONFIG_JM;
					rc = sm_play_tone_list_abort(v8->channel);
					if (rc) return errorin(rc, "stop ANSam tone");
					break;
				case V8STS_SENDING_CM_AWAIT_JM:
					v8->status = V8STS_GOT_JM_SENDING_CJ;
					break;
				default:
					break;
				}
				return 0;
			}
		} else {
			/* Mid-sequence byte (or random garbage) */
			if (in_len == 0)
				/* Not inside a CI/CM/JM sequence, ignore */
				return 0;
			if (in_len >= arlen(v8->in.buf)) {
				/* Implausibly long CM/JM - discard */
				v8->in.len = 0;
				return 0;
			}
			if (v8->in.buf[in_len] != in)
				/* Value changed, we need another copy */
				v8->in_msglen = 0;
			if (in_len == 1 && v8->in.buf[0] == 0xe0
					&& v8->status == V8STS_AWAIT_CI && (in & 0x1f) == 1) {
				/* First byte of CM is the call function, save.
				 * In particular this can be used to stop audio transfer
				 * so that the far modem (for V.150 etc) won't see the CM */
				if (v8->in_msglen != 0) {
					/* Two equal call function bytes - assume valid CI */
					v8->ci[1] = in;
				}
			}
		}
		v8->in.buf[in_len] = in;
		v8->in.len = ++in_len;
		break;

	case V8STS_SENDING_JM_MISSING_CJ:
		if (v8->in.len != 0)
			break;
		// found rx async data, try if we can find CJ
		// going back a step in case remote missed JM
		v8->status = V8STS_SENDING_JM_AWAIT_CJ;
		// fall in ...

	case V8STS_SENDING_JM_AWAIT_CJ:
		/* CJ is three 0x00 bytes */
		if (in != 0) {
			/* Reset CJ sequence */
			v8->in.len = 0;
			break;
		}
		// Leave CM in input buffer
		// v8->in.buf[v8->in.len] = 0;
		if (++v8->in.len == 3) {
			v8->status = V8STS_STOPPING;    // saw CJ
			return stop_v21(v8);
		}
		break;

	default:
		break;
	}
	return 0;
}

static int asyrx(struct v8 *v8, unsigned in)
{
 unsigned bits = v8->asyrx.inbits;
 int rc = 0;

 in <<= bits;
 in |= v8->asyrx.cc;
 bits += 8;
 while (bits >= 10) {
	/* Check for valid start (0) and stop (1) bits */
	if ((in & 0x201) == 0x200) {
		rc = asyrx_byte(v8, (in >> 1) & 0xff);
		bits -= 10;
		in >>= 10;
		break;		// bits < 10 - no need to check
	}
	bits--;
	in >>= 1;
 }
 v8->asyrx.inbits = bits;
 v8->asyrx.cc = in;
 return rc;
}

static int stop_v21(struct v8 *v8)
{
 if (v8->await_read_event) {
	SMDC_STOP_PARMS ssp;
	int rc;
	memset(&ssp, 0, sizeof(ssp));
	ssp.channel = v8->channel;
	rc = smdc_rx_stop(&ssp);
	if (rc) return errorin(rc, "stop V.21 rx channel");
 }
 if (!v8->sending_ansam && v8->await_write_event) {
	SMDC_STOP_PARMS ssp;
	int rc;
	memset(&ssp, 0, sizeof(ssp));
	ssp.channel = v8->channel;
	rc = smdc_tx_stop(&ssp);
	if (rc) return errorin(rc, "stop V.21 tx channel");
 }
 return 0;
}

static int v8_check_rx_status(struct v8 *v8)
{
 SMDC_RX_STATUS_PARMS rsp;
 int rc;
 memset(&rsp, 0, sizeof(rsp));
 rsp.channel = v8->channel;
 rc = smdc_rx_status(&rsp);
 if (rc) return errorin(rc, "get V.21 rx status");
 if (rsp.status == kSMDCRxStatusCarrier) {
	v8->have_carrier = rsp.u.carrier.carrier;
	if (!rsp.u.carrier.carrier) {
		v8->in.len = 0;
		v8->asyrx.inbits = 0;
		v8->asyrx.cc = 0;
		v8->hdlc_prev = 0;

		if(v8->status == V8STS_SENDING_JM_AWAIT_CJ)
		{
			// carrier is lost after CM, it is unlikely we will receive CJ
			// send some (CJ_WAIT_COUNT determines the number of write events before stopping)
			// more JMs and if async data not received by then stop
			v8->status = V8STS_SENDING_JM_MISSING_CJ;
		}
	} else {
		v8->CJ_wait_count = 0;
	}
 } else if (rsp.status == kSMDCRxStatusFinished) {
	v8->await_read_event = 0;
	switch (v8->status) {
	case V8STS_STOPPING_CI_GOT_ANSAM:
		if (!v8->await_write_event) v8->status = V8STS_CONFIG_CM;
		break;
	case V8STS_STOPPING:
		if (!v8->await_write_event) v8->status = V8STS_DONE;
		break;
	default:
		break;
	}
 } else if (rsp.status == kSMDCRxStatusSigMetric) {
	v8->rxfsk_metric = rsp.u.metric.metric;
 }
 return 0;
}

#define V8_RAW_RX_LENGTH 32
int v8_handle_read(struct v8 *v8)
{
	SMDC_DATA_PARMS rxd;
	unsigned u;
	unsigned char raw_rx[V8_RAW_RX_LENGTH];
	int rc;

	memset(&rxd, 0, sizeof(rxd));
	rxd.channel = v8->channel;
	rxd.data = (void *)raw_rx;
	rxd.max_length = V8_RAW_RX_LENGTH;
	rc = smdc_rx_data(&rxd);
	if(rc) return errorin(rc, "get V.21 rx data");
	if(!rxd.done_length) return v8_check_rx_status(v8);

	if (!v8->have_carrier) {
		if (v8->status == V8STS_SENDING_JM_MISSING_CJ && ++v8->CJ_wait_count == CJ_WAIT_COUNT) {
			// carrier lost while waiting for CJ and carrier hasn't returned for a while
			// so behave as though we'd seen the CJ
			v8->status = V8STS_STOPPING;
			stop_v21(v8);
		}
		/* V.21 gives us 0xff data when carrier is absent! */
		return 0;
	}

	if (v8->status != V8STS_STOPPING) {
		for (u=0; u < rxd.done_length + 0u; u++) {
			if (hdlc_flags(v8, raw_rx[u])) {
				v8->flags_detected = 1;
				return v8_abort(v8);
			}
			rc = asyrx(v8, raw_rx[u]);
			if (rc) return rc;
		}
	}

	return 0;
}

static int v8_check_tx_status(struct v8 *v8)
{
 SMDC_TX_STATUS_PARMS tsp;
 int rc;
 memset(&tsp, 0, sizeof(tsp));
 tsp.channel = v8->channel;
 rc = smdc_tx_status(&tsp);
 if (rc) return errorin(rc, "get V.21 tx status");
 if (tsp.status == kSMDCTxStatusFinished) {
	v8->await_write_event = 0;
	switch (v8->status) {
	case V8STS_STOPPING_CI_GOT_ANSAM:
		if (!v8->await_read_event) v8->status = V8STS_CONFIG_CM;
		break;
	case V8STS_STOPPING:
		if (!v8->await_read_event) v8->status = V8STS_DONE;
		break;
	default:
		break;
	}
 }
 return 0;
}

int v8_off_ci(struct v8 *v8)
{
	SMDC_TX_CONTROL_PARMS txp;
	memset(&txp, 0, sizeof(txp));
	txp.channel = v8->channel;
	txp.cmd = kSMDCTxCtlFinish;

	v8->status = V8STS_CI_OFF_AWAIT_ANSAM;
	return smdc_tx_control(&txp);
}

int v8_on_ci(struct v8 *v8)
{
	v8->status = V8STS_SENDING_CI_AWAIT_ANSAM;
	return start_v21tx(v8, 0);
}

int v8_handle_write(struct v8 *v8)
{
	int rc;	

	if(!v8->sending_ansam)
	{
		SMDC_DATA_PARMS dp;
		if(v8->status == V8STS_CI_OFF_AWAIT_ANSAM || v8->status == V8STS_STOPPING) return v8_check_tx_status(v8);
		memset(&dp, 0, sizeof(dp));
		dp.channel = v8->channel;
		if(v8->status == V8STS_GOT_JM_SENDING_CJ)
		{
			dp.data = "\377\377\377\377\0\02\010\340"; 
			// this is:
			// padding to ensure we can send a multiple of 32 bits
			// padding to terminate any partially output async char (2,4,6 or 8 bits of 10, so 1 octet is enough) ,
			// 3 zero octets, with start & stop bits 
			// and a little padding to stop carrier drop too soon
			dp.data += v8->tottx & 3;
			dp.max_length = 8 - (v8->tottx & 3); // total length must be multiple of 32 bits
		}
		else
		{
			dp.data = (void *)&v8->out.buf[v8->outpos];
			dp.max_length = v8->out.len - v8->outpos;
			if(dp.max_length + 0u > MIN_OCTET_CAPACITY) dp.max_length = MIN_OCTET_CAPACITY;
		}

		rc = smdc_tx_data(&dp);
		if(rc) return errorin(rc, "send V.21 data");
		if(!dp.done_length) return v8_check_tx_status(v8);

		if( (v8->status == V8STS_GOT_JM_SENDING_CJ))
		{
			SMDC_TX_CONTROL_PARMS txp;
			SMDC_STOP_PARMS stp;
			memset(&stp, 0, sizeof(stp));
			stp.channel = v8->channel;
			rc = smdc_rx_stop(&stp);
			if(rc) return errorin(rc, "Stopping V.21 rx");
			memset(&txp, 0, sizeof(txp));
			txp.channel = v8->channel;
			txp.cmd = kSMDCTxCtlFinish;
			rc = smdc_tx_control(&txp);
			if(rc) return errorin(rc, "finish V.21 tx");
			v8->status = V8STS_STOPPING;
		}
		else
		{
			v8->outpos += dp.done_length;
			v8->tottx += dp.done_length;
			if(v8->outpos >= v8->out.len) v8->outpos = 0;
		}
	}
	else
	{
		SM_PLAY_TONE_LIST_STATUS_PARMS pts;
		memset(&pts, 0, sizeof(pts));
		pts.channel = v8->channel;
		rc = sm_play_tone_list_status(&pts);
		if(rc) return errorin(rc, "get play tone list status");
		if(pts.status == kSMPlayToneListStatusComplete)
		{
			v8->sending_ansam = 0;
			v8->await_write_event = 0;
			switch (v8->status)
			{
			case V8STS_CONFIG_CI:
			case V8STS_CI_OFF_AWAIT_ANSAM:
			case V8STS_SENDING_CI_AWAIT_ANSAM:
			case V8STS_STOPPING_CI_GOT_ANSAM:
			case V8STS_CONFIG_CM:
			case V8STS_SENDING_CM_AWAIT_JM:
			case V8STS_GOT_JM_SENDING_CJ:
			case V8STS_DONE:
				abort();// impossible - caller does not send tones
				break;

			case V8STS_GOT_CM_CONFIG_JM:
				break;
			case V8STS_SENDING_JM_AWAIT_CJ:
			case V8STS_SENDING_JM_MISSING_CJ:
				rc = start_v21tx(v8, 1);
				if(rc) return errorin(rc, "start V.21 transmitter");
				break;

			case V8STS_AWAIT_CI:
			case V8STS_GOT_CI_AWAIT_CM:
				v8->status = V8STS_STOPPING;
				return stop_v21(v8);

			case V8STS_STOPPING:
				if(!v8->await_read_event) v8->status = V8STS_DONE;
				break;
			}
		}
	}

	return 0;
}

int v8_do_full_ansam_detection(struct v8 *v8)
{
	v8->full_ansam = 1;
	return 0;
}

int v8_handle_recognition(struct v8 *v8)
{
	SM_RECOGNISED_PARMS rp;
	int rc;
	memset(&rp, 0, sizeof(rp));
	rp.channel = v8->channel;
	rc = sm_get_recognised(&rp);
	if(rc) return errorin(rc, "get recognition result");

	if (rp.type == kSMRecognisedANS) {
		if (rp.param0 == 2)	{
			// ANSam
			if (v8->ans_status == V8ANS_STS_NONE) v8->ans_status = V8ANS_STS_ANSAM;
			if (v8->ans_status == V8ANS_STS_ANSAM && rp.param1) v8->ans_status = V8ANS_STS_ANSAM_PR;
			if (!v8->full_ansam) {
				rc = listen_ansam(v8, kSMANSModeDisable);
				if (rc) return errorin(rc, "stop ANSam detection");
			}
			if(v8->status == V8STS_CI_OFF_AWAIT_ANSAM || v8->status == V8STS_SENDING_CI_AWAIT_ANSAM) {
				v8->status = V8STS_STOPPING_CI_GOT_ANSAM;
				return stop_v21(v8);
			}
		} else if (rp.param0 == 1) {
			// ANS
			if (v8->ans_status == V8ANS_STS_NONE) v8->ans_status = V8ANS_STS_ANS;
			if (v8->ans_status == V8ANS_STS_ANS && rp.param1) v8->ans_status = V8ANS_STS_ANS_PR;
			if (!v8->full_ansam) {
				return v8_abort(v8);
			}
		} else if (rp.param0 == 0) {
			// ANS or ANSam finished
			if (v8->ans_status == V8ANS_STS_ANS || v8->ans_status == V8ANS_STS_ANS_PR) {
				return v8_abort(v8);
			}
			if (v8->ans_status == V8ANS_STS_ANSAM || v8->ans_status == V8ANS_STS_ANSAM_PR) {
				rc = listen_ansam(v8, kSMANSModeDisable);
				if (rc) return errorin(rc, "stop ANSam detection");
			}
		}
	}
	return 0;
}

int v8_abort(struct v8 *v8)
{
 int rc;
 if (v8->await_recognition_event) {
 	rc = listen_ansam(v8, kSMANSModeDisable);
 	if (rc) return errorin(rc, "stop ANSam detection");
 }
 switch (v8->status)
 {
 case V8STS_CI_OFF_AWAIT_ANSAM:
 case V8STS_SENDING_CI_AWAIT_ANSAM:
 case V8STS_SENDING_CM_AWAIT_JM:
 case V8STS_GOT_JM_SENDING_CJ:
 case V8STS_AWAIT_CI:
 case V8STS_GOT_CI_AWAIT_CM:
	if (v8->sending_ansam) {
		rc = sm_play_tone_list_abort(v8->channel);
		if (rc) return errorin(rc, "stop ANSam tone");
	}
		// fall in ...
 case V8STS_GOT_CM_CONFIG_JM:
 case V8STS_SENDING_JM_AWAIT_CJ:
 case V8STS_SENDING_JM_MISSING_CJ:
	v8->status = V8STS_STOPPING;
	return stop_v21(v8);
 case V8STS_STOPPING:
 case V8STS_DONE:
 	break;
 case V8STS_STOPPING_CI_GOT_ANSAM:
 	v8->status = V8STS_STOPPING;
 	break;
 case V8STS_CONFIG_CM:
 case V8STS_CONFIG_CI:
 	v8->status = V8STS_DONE;
 	break;
 }
 return 0;
}

int v8_fetch_inmessage(struct v8 *v8, unsigned char **msg, unsigned *length)
{
 if (v8->status == V8STS_GOT_CI_AWAIT_CM) {
	// return the CI data
	*msg = v8->ci;
	*length = 2;
	return 0;
 }
 *msg = v8->in.buf;
 *length = v8->in_msglen;
 return 0;
}

int v8_set_outmessage(struct v8 *v8, const unsigned char *msg, unsigned length)
{
 unsigned long nxout = 0;	// bits not yet appended
 unsigned txbit = 0;		// valid bits in nxout
 v8->out.len = 0;
 for (;;) {
	unsigned inlen = length;
	const unsigned char *mp = msg;
 		// 0 <= txbit < 8
	nxout |= 0x3ff << txbit;
	txbit += 10;
 		// 10 <= txbit < 18
	do
	{
		unsigned out = *mp++;
		if (v8->out.len + 4 > sizeof(v8->out.buf)) {
			if (TiNGtrace) outputlog("Out message too long\n");
			return ERR_SM_NO_CAPACITY;
		}
		out |= 0x100;		// stop bit
		out <<= 1;		// add start bit
		nxout |= out << txbit;	// fill nxout
		txbit += 10;
		do {		// 20 <= txbit < 28
			v8->out.buf[v8->out.len++] = (unsigned char) (nxout & 0xff);
			nxout >>= 8;
			txbit -= 8;
		} while (txbit >= 8);
	} while (--inlen);
	if (!txbit) break;
		// not a multiple of 8 bits - add another copy
 }
 switch (v8->status) {
 	int rc;
 case V8STS_GOT_CM_CONFIG_JM:
	v8->status = V8STS_SENDING_JM_AWAIT_CJ;
	if (v8->sending_ansam) return 0;
	return start_v21tx(v8, 1);
 case V8STS_CONFIG_CM:
	v8->status = V8STS_SENDING_CM_AWAIT_JM;
	rc = start_v21rx(v8, 1);
	if(rc) return rc;
	return start_v21tx(v8, 0);
 case V8STS_CONFIG_CI:
	rc = listen_ansam(v8, kSMANSModeDetect);
	if(rc) return errorin(rc, "listen for ANSam tone");
	v8->status = V8STS_CI_OFF_AWAIT_ANSAM;
	return start_v21rx(v8, 1); // look for HDLC flags
 default:
	break;
 }
 return 0;
}
