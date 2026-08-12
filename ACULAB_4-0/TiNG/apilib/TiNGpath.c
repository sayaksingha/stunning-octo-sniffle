/* TiNGpath.c - implementation of signal processing paths */

#include "TiNGcommon.h"
#include "../hsif/echocan.h"
#include "../hsif/kernel.h"
#include "../hsif/play.h"
#include "../hsif/pitchshift.h"
#include "../hsif/resample.h"
#include "../hsif/subtask.h"
#include "../hsif/tonedet.h"
#include "../hsif/typecode.h"
#include "../hsif/vmp.h"
#include "../hsif/emph.h"
#include "../hsif/pathdelay.h"
#include "TiNGo_path.h"
#include "TiNGdetect.h"
#include "complx.h"
#include "smgroove.h"
#include "smpath.h"
#include "timestamp.h"
#include "trace.h"

STATIC int real_sm_path_create(struct sm_path_create_parms *pcp)
{
 struct module *mod = modid2lockedmodule(pcp->module);
 tSMPathId path;
 int sts = 0;
 if (!mod) return err(ERR_SM_NO_SUCH_MODULE);
 path = init_path(mod);
 if (!path) sts = err(ERR_SM_NO_RESOURCES);
 if (!sts) {
	tSMGroove *grv = &path->groove;
	path->state = PATH_STATE_RUNNING;
	sts = grv->starttask(grv, TASKTC_INCHAN, 0);
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_PASSTHROUGH);
	if (sts) path_dtor(path);
	else {
		make_object(path);
		pcp->path = path;
	}
 }
 unlock_module(mod);
 return sts;
}



STATIC int unsafe_sm_path_delay(struct sm_path_delay_parms *delayp)
{
 tSMPathId path = delayp->path;
 tSMGroove *grv = &path->groove;
 int sts;
 
 if (delayp->enable) {
	if (!path->delay.running) {
		sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_PATHDELAY);
		if (sts) return sts;
		path->delay.running = 1;
	}
	sts = grv->sendmsg(grv, MESSAGE(MSG_PATHDELAY_SETDELAY, 2), (uint32_t)delayp->delay);
	if (sts) return sts;
 } else if (path->delay.running) {
	sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), SUBTASKTC_PATHDELAY);
	if (sts) return sts;
	path->delay.running = 0;
 }
 return 0;
}

STATIC int real_sm_path_delay(struct sm_path_delay_parms *delayp)
{
 int sts;
 lock_object(delayp->path);
 sts = unsafe_sm_path_delay(delayp);
 unlock_object(delayp->path);
 return sts;
}

STATIC int unsafe_sm_path_echocancel(struct sm_path_echocancel_parms *ecp)
{
 tSMPathId path = ecp->path;
 tSMGroove *grv = &path->groove;
 int sts;
 if (!ecp->enable) {
	if (path->ec.running) {
		sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), SUBTASKTC_ECHOCAN);
		if (sts) return sts;
		path->ec.running = 0;
	}
	sts = grv->connect_df(grv, &path->ec.refdf, NULL_DATAFEED);
	if (sts) return sts;
 } else {
	DATAFEED df;
	int nlp;
	if (!path->ec.running) {
		sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_ECHOCAN);
		if (sts) return sts;
		path->ec.running = 1;
	}
	df.vtbl_data = ecp->reference;
	sts = grv->connect_df(grv, &path->ec.refdf, df);
	if (sts) return sts;
	sts = grv->engage_df(grv, df, DFC_ECREF);
	if (sts) return sts;
	sts = grv->sendmsg(grv, MESSAGE(MSG_SET_FIX, 2), ecp->fix_agc);
	if (sts) return sts;
	sts = grv->sendmsg(grv, MESSAGE(MSG_SET_AGC, 2), ecp->use_agc);
	if (sts) return sts;
	if (ecp->non_linear == 0) {
		nlp = 0;
	} else {
		double val = ecp->non_linear;
		if (val == 0) nlp = 0; // turn NLP off
		if (val > 0) nlp = 1;  // use default specified in firmware
		else {
			// interpret val as threshold in dBm0 and scale result by 16
			val = exp((val/10.0) * log(10.0)) * (2853.0*2853.0*16.0);
			// don't pass negative, overflowed, or 0 or 1 as these are special values
			if (val >= 32767.0) {
				nlp = 32767;
			} else if (val <= 2.0) {
				nlp = 2;
			} else {
				nlp = (int) floor(val + 0.5);
			}
		}
		// use negative values to indicate CNG mode
		if(ecp->mode == kSMPathEchoNonLinearModeCNG) {
			nlp = -nlp;
		}
	}
	sts = grv->sendmsg(grv, MESSAGE(MSG_SET_NLP, 2), nlp);
	if (sts) return sts;
	if (ecp->span) sts = grv->sendmsg(grv, MESSAGE(MSG_EC_REINIT, 2), ecp->span);
	if (sts) return sts;
	if (ecp->delay) sts = grv->sendmsg(grv, MESSAGE(MSG_SET_REF_DELAY, 2), ((ecp->delay > 0) ? ecp->delay : 0));
	if (sts) return sts;
	sts = grv->sendmsg(grv, MESSAGE(MSG_SET_EC_TONE_ACTION, 2), ecp->tone_action);
	if (sts) return sts;
 }
 return 0;
}

STATIC int real_sm_path_echocancel(struct sm_path_echocancel_parms *ecp)
{
 int sts;
 lock_object(ecp->path);
 sts = unsafe_sm_path_echocancel(ecp);
 unlock_object(ecp->path);
 return sts;
}

STATIC int unsafe_sm_path_agc(struct sm_path_agc_parms *agcp)
{
 tSMPathId path = agcp->path;
 tSMGroove *grv = &path->groove;
 int sts;
 if (!agcp->agc && !agcp->volume) {
	if (path->agc.running && !path->mix.running) {
		sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), SUBTASKTC_GAINRX);
		if (sts) return sts;
	}
	path->agc.running = 0;
 } else {
	if (!path->agc.running && !path->mix.running) {
		sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_GAINRX);
		if (sts) return sts;
	}
	path->agc.running = 1;
	sts = grv->sendmsg(grv, MESSAGE(MSG_AGC, 2), agcp->agc);
		// FIXME: if we have several volume controls we need
		// different messages here
	sts = grv->sendmsg(grv, MESSAGE(MSG_VOLUME, 2), agcp->volume);
	if (sts) return sts;
 }
 return 0;
}

STATIC int real_sm_path_agc(struct sm_path_agc_parms *agcp)
{
 int sts;
 lock_object(agcp->path);
 sts = unsafe_sm_path_agc(agcp);
 unlock_object(agcp->path);
 return sts;
}

STATIC int unsafe_sm_path_agc_adjust_settings(struct sm_path_agc_adjust_settings_parms *agcp)
{
 tSMPathId path = agcp->path;
 tSMGroove *grv = &path->groove;
 int sts;
 if (!path->agc.running) {
	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else {
	U32 lmax_decay;
	U32 target;
	if (agcp->max_level_decay > 1.0 || agcp->max_level_decay < 0.0) sts = err(ERR_SM_BAD_PARAMETER);
	if (agcp->target_level > 3.0 || agcp->target_level < -90.0) sts = err(ERR_SM_BAD_PARAMETER);
	lmax_decay = 32768.0 * agcp->max_level_decay;
	target = exp((agcp->target_level + 90.0) / 10.0 * log(10));
	sts = grv->sendmsg(grv, MESSAGE(MSG_AGC_ADJ_SETTINGS, 3), lmax_decay, target);
 }
 return sts;
}

STATIC int real_sm_path_agc_adjust_settings(struct sm_path_agc_adjust_settings_parms *agcp)
{
 int sts;
 lock_object(agcp->path);
 sts = unsafe_sm_path_agc_adjust_settings(agcp);
 unlock_object(agcp->path);
 return sts;
}

STATIC int unsafe_sm_path_mix(struct sm_path_mix_parms *mixp)
{
 tSMPathId path = mixp->path;
 tSMGroove *grv = &path->groove;
 int sts;
 if (!mixp->enable) {
	if (path->mix.running && !path->agc.running) {
		sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), SUBTASKTC_GAINRX);
		if (sts) return sts;
	}
	path->mix.running = 0;
	sts = grv->connect_df(grv, &path->mix.bgdf, NULL_DATAFEED);
	if (sts) return sts;
	sts = grv->engage_df(grv, NULL_DATAFEED, DFC_BG);
	if (sts) return sts;
 } else {
	DATAFEED df;
	if (!path->mix.running && !path->agc.running) {
		sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_GAINRX);
		if (sts) return sts;
	}
	path->mix.running = 1;
	df.vtbl_data = mixp->mixin;
	sts = grv->connect_df(grv, &path->mix.bgdf, df);
	if (sts) return sts;
	sts = grv->engage_df(grv, df, DFC_BG);
	if (sts) return sts;
		// FIXME: if we have several volume controls we need
		// different messages here
	sts = grv->sendmsg(grv, MESSAGE(MSG_VOLUME, 2), mixp->volume);
	if (sts) return sts;
 }
 return 0;
}

STATIC int real_sm_path_mix(struct sm_path_mix_parms *mixp)
{
 int sts;
 lock_object(mixp->path);
 sts = unsafe_sm_path_mix(mixp);
 unlock_object(mixp->path);
 return sts;
}

STATIC int unsafe_sm_path_pitchshift(struct sm_path_pitchshift_parms *psp)
{
 tSMPathId path = psp->path;
 tSMGroove *grv = &path->groove;
 int sts;
 if (!psp->shift) {
	if (path->pitchshift.running) {
		sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), SUBTASKTC_PITCHSHIFT);
		if (sts) return sts;
		path->pitchshift.running = 0;
	}
 } else {
	if (!path->pitchshift.running) {
		sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_PITCHSHIFT);
		if (sts) return sts;
		path->pitchshift.running = 1;
	}
	sts = grv->sendmsg(grv, MESSAGE(MSG_PITCHSHIFT, 2), (int) (psp->shift * 65536));
	if (sts) return sts;
 }
 return 0;
}

STATIC int real_sm_path_pitchshift(struct sm_path_pitchshift_parms *psp)
{
 int sts;
 lock_object(psp->path);
 sts = unsafe_sm_path_pitchshift(psp);
 unlock_object(psp->path);
 return sts;
}



STATIC int unsafe_sm_path_tonedetect(struct sm_path_tonedetect_parms *tdp)
{
 tSMPathId path = tdp->path;
 tSMGroove *grv = &path->groove;
 int sts = 0;
 struct module *mp;
 unsigned tsset;
 struct dettoneset *sp;
 int hyst;

 if (path->td.running) {
	sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), SUBTASKTC_TONEDET);
	if (sts) return sts;
	sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), SUBTASKTC_TONEREP);
	if (sts) return sts;
	sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), SUBTASKTC_TONEELIM);
	if (sts) return sts;
	path->td.running = 0;
 }


 if (tdp->min_duration != kSMPathToneDetectModeNone) {
	switch(tdp->min_duration)
	{
	case kSMPathToneDetectModeMinDurationNoMinimum:
		hyst = 1;
		break;
	case kSMPathToneDetectModeMinDuration40:
		hyst = 0;
		break;
	case kSMPathToneDetectModeMinDuration64:
		hyst = 2;
		break;
	default:
		return err(ERR_SM_BAD_PARAMETER);
	}

	sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_TONEDET);
	if (sts) return sts;
	path->td.running = 1;
	mp = path->mod;

	mutx_enter(&mp->det.mx);

	tsset = (unsigned) tdp->tone_set_id;
	if (tsset >= path->mod->det.ntoneset) {
		mutx_leave(&mp->det.mx);
		return err(ERR_SM_BAD_PARAMETER);
	}

	sp = mp->det.tonesetlist[tsset];

	if (sp) {
		int hystoff = hyst; // in case none set
		if (sp->mintime >= 64) {
			hyst = sp->mintime / 32;
			if (sp->mintimeoff >= 64) {
				hystoff = sp->mintimeoff / 32; // both set
			} else {
				hystoff = hyst; // only one set, so make both the same
			}
		} else if (sp->mintimeoff >= 64) {
			hystoff = sp->mintimeoff / 32;
			hyst = hystoff; // only one set, so make both the same
		}
		sts = grv->sendmsg(grv, MESSAGE(MSG_SET_HYST, 2), hyst); // for firmware that doesn't support independent values
		if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_SET_HYST, 3), hyst, hystoff); // for firmware that does support independent values
		if (sts) {
			mutx_leave(&mp->det.mx);
			return sts;
		}
		sts = create_detpar(mp, sp);
		if (sts) {
			mutx_leave(&mp->det.mx);
		 	return sts;
		}

		grv->setfriendcx(grv,&sp->chan.cx,FRIEND_TYPE_TONE_DETPAR);
	}
	

	mutx_leave(&mp->det.mx);

	sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_TONEREP);
	if (sts) return sts;

	grv->setfriend(grv,grv,FRIEND_TYPE_TONE_DETECTOR);

	if (tdp->elim) {
		sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_TONEELIM);
		sts = grv->sendmsg(grv, MESSAGE(MSG_SET_TDELIMHYST, 2), hyst);
		if (sts) return sts;
	}
 }

 return sts;
}

STATIC int real_sm_path_tonedetect(struct sm_path_tonedetect_parms *tdp)
{
 int sts;
 lock_object(tdp->path);
 sts = unsafe_sm_path_tonedetect(tdp);
 unlock_object(tdp->path);
 return sts;
}








#define RESAMPLE_FILTSECT 4
// Number of biquad sections
#define M RESAMPLE_FILTSECT

// Pre-calculated rotation factor, dependent on M.
// Uses formula: exp(i*M_PI/(4*M)), where i^2 = -1.
static COMPLX resample_zx0(double m)
{
 return complx_expi(M_PI/(4*m));
}

// Uses formula: (1 - 2*R*(R + (R^2 - 1)^(1/2)))^(1/(4*M)),
// where R is the desired rejection ratio (e.g. 128 for 42dB).
static COMPLX resample_px0(double R)
{
 return complx_powR(complx(1 - 2*R*(R + sqrt(R*R - 1)), 0), 1/(4.0*M));
}

// Calculate filter coefficients for Chebychev type II low-pass response.
// Values for omitted a2 array are all implicitly unity.
// Biquadratic sections are arranged in order - highest Q poles and lowest
// frequency zeroes last. This is intended to alleviate overflow problems at
// run-time and reduce noise "colouring", and so optimise dynamic range.
static void resample_makefilter(double g[M], double a1[M], double b1[M], double b2[M], unsigned u_ratio, unsigned d_ratio)
{
    const COMPLX px0 = resample_px0(128); // 128 for 42dB rejection ratio
    const COMPLX zx0 = resample_zx0(M);
    const COMPLX k = complx_mul(zx0, zx0);
    const double w = 2.0 * tan((M_PI / 2.0) / (double) (u_ratio > d_ratio ? u_ratio : d_ratio));
    const COMPLX one = complx(1, 0);
    COMPLX x = one;
    unsigned m = M - 1;
    for (m = M; m--; x = complx_mul(x, k)) {
        COMPLX pp;
        double zp;
        COMPLX xx = complx_mul(x, zx0);
        COMPLX wx = complx(w * xx.im, w * xx.re);
        
        xx = complx_mul(xx, xx);
        xx = complx_add(xx, one);
        pp = complx(xx.re + wx.re, xx.im - wx.im);
        xx = complx(xx.re - wx.re, xx.im + wx.im);
        zp = (pp.re * xx.re + pp.im * xx.im ) / complx_mod_sq(xx);

        xx = complx_mul(x, px0);
        wx = complx(w * xx.im, w * xx.re);
        xx = complx_mul(xx, xx);
        xx = complx_add(xx, one);
        pp = complx_div(complx(xx.re + wx.re, xx.im - wx.im),
                        complx(xx.re - wx.re, xx.im + wx.im));

        b1[m] = (-2.0) * pp.re;
        b2[m] = complx_mod_sq(pp);

        a1[m] = (-2.0) * zp;

        g[m] = (1.0 + b1[m] + b2[m]) / (2.0 + a1[m]);
    }
}
#undef M

STATIC int unsafe_sm_path_resample(struct sm_path_resample_parms *rp)
{
 tSMPathId path = rp->path;
 tSMGroove *grv = &path->groove;
 int sts;
 if (rp->uprate == rp->downrate) {
	if (path->resample.running) {
		sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), SUBTASKTC_RESAMPLE);
		if (sts) return sts;
		path->resample.running = 0;
	}
 } else {
	double g[RESAMPLE_FILTSECT];
	double a1[RESAMPLE_FILTSECT];
	double b1[RESAMPLE_FILTSECT];
	double b2[RESAMPLE_FILTSECT];
	unsigned u;
	if (!path->resample.running) {
		sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_RESAMPLE);
		if (sts) return sts;
		path->resample.running = 1;
	}
	resample_makefilter(g, a1, b1, b2, rp->uprate, rp->downrate);
	for (u=0; u < RESAMPLE_FILTSECT; u++) {
		sts = grv->sendmsg(grv, MESSAGE(MSG_RESAMPLE_FILTER, 6), 
			u,
			(uint32_t) (g[u] * 0x01000000),
			(uint32_t) (a1[u] * 0x01000000),
			(uint32_t) (b1[u] * 0x01000000),
			(uint32_t) (b2[u] * 0x01000000));
		if (sts) return sts;
	}
	sts = grv->sendmsg(grv, MESSAGE(MSG_RESAMPLE_RATIO, 3), rp->uprate, rp->downrate);
	if (sts) return sts;
 }
 return 0;
}

STATIC int real_sm_path_resample(struct sm_path_resample_parms *rp)
{
 int sts;
 lock_object(rp->path);
 sts = unsafe_sm_path_resample(rp);
 unlock_object(rp->path);
 return sts;
}


STATIC int unsafe_sm_path_avf_amrnb_dec(struct sm_path_avf_amrnb_dec_parms *amrnbdecp)
{
 tSMPathId path = amrnbdecp->path;
 tSMGroove *grv = &path->groove;
 int sts = 0;

 if (amrnbdecp->enable) {
	switch(path->codec.running) {
	case kPathCodecNone:
		sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_AMRNBDEC_C);
		if (sts) return sts;
		path->codec.running = kPathCodecAMRNBDec;
		break;
	case kPathCodecAMRNBDec:
		break;
	default:
		sts = err(ERR_SM_WRONG_CHANNEL_STATE);
		break;
	}
 } else {
	if (path->codec.running == kPathCodecAMRNBDec) {
		sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), SUBTASKTC_AMRNBDEC_C);
		if (sts) return sts;
		path->codec.running = kPathCodecNone;
	}
 }
 return sts;
}

STATIC int real_sm_path_avf_amrnb_dec(struct sm_path_avf_amrnb_dec_parms *amrnbdecp)
{
 int sts;
 lock_object(amrnbdecp->path);
 sts = unsafe_sm_path_avf_amrnb_dec(amrnbdecp);
 unlock_object(amrnbdecp->path);
 return sts;
}

STATIC int unsafe_sm_path_avf_amrnb_enc(struct sm_path_avf_amrnb_enc_parms *amrnbencp)
{
 tSMPathId path = amrnbencp->path;
 tSMGroove *grv = &path->groove;
 int sts = 0;

 if (amrnbencp->enable) {
	switch(path->codec.running) {
	case kPathCodecNone:
		sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_AMRNBENC_C);
		if (sts) return sts;
		path->codec.running = kPathCodecAMRNBEnc;
		break;
	case kPathCodecAMRNBEnc:
		break;
	default:
		sts = err(ERR_SM_WRONG_CHANNEL_STATE);
		break;
	}
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_AMRNB_ALIGN, 2), 1); // force algined
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_AMRNB_MODE, 2), amrnbencp->bitrate);
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_VAD_MODE, 2), amrnbencp->vad_enable);
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_AMRNBENC_C, amrnbencp->payload_type, 20); // 1 FPP
 } else {
	if (path->codec.running == kPathCodecAMRNBEnc) {
		sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), SUBTASKTC_AMRNBENC_C);
		if (sts) return sts;
		path->codec.running = kPathCodecNone;
	}
 }
 return sts;
}

STATIC int real_sm_path_avf_amrnb_enc(struct sm_path_avf_amrnb_enc_parms *amrnbencp)
{
 int sts;
 lock_object(amrnbencp->path);
 sts = unsafe_sm_path_avf_amrnb_enc(amrnbencp);
 unlock_object(amrnbencp->path);
 return sts;
}


STATIC int unsafe_sm_path_emphasis(struct sm_path_emphasis_parms *emphp)
{
 tSMPathId path = emphp->path;
 tSMGroove *grv = &path->groove;
 int sts;
 uint32_t mode;

 if (emphp->enable) {
	if (!path->emphasis.running) {
		sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_EMPH);
		if (sts) return sts;
		path->emphasis.running = 1;
	}
	switch(emphp->mode) {
	case kSMPathEmphModeHFEmphasis:
		mode = EMPH_MODE_HF_EMPHASIS;
		break;
	case kSMPathEmphModeHFDeemphasis:
		mode = EMPH_MODE_HF_DEEMPHASIS;
		break;
	default:
		return err(ERR_SM_BAD_PARAMETER);
	};
	sts = grv->sendmsg(grv, MESSAGE(MSG_EMPH_SETMODE, 3), mode, (uint32_t)emphp->gain);
	if (sts) return sts;
 } else if (path->emphasis.running) {
	sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), SUBTASKTC_EMPH);
	if (sts) return sts;
	path->emphasis.running = 0;
 }
 return 0;
}

STATIC int real_sm_path_emphasis(struct sm_path_emphasis_parms *emphp)
{
 int sts;
 lock_object(emphp->path);
 sts = unsafe_sm_path_emphasis(emphp);
 unlock_object(emphp->path);
 return sts;
}

STATIC int unsafe_sm_path_ttyeliminate(struct sm_path_ttyeliminate_parms *ttyeliminatep)
{
 tSMPathId path = ttyeliminatep->path;
 tSMGroove *grv = &path->groove;
 int sts;
 if (!ttyeliminatep->enable) {
	if (path->ttyeliminate.running) {
		sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), SUBTASKTC_TTYELIM);
		if (sts) return sts;
	}
	path->ttyeliminate.running = 0;
 } else {
	if (!path->ttyeliminate.running) {
		sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_TTYELIM);
		if (sts) return sts;
	}
	path->ttyeliminate.running = 1;
 }
 return 0;
}

STATIC int real_sm_path_ttyeliminate(struct sm_path_ttyeliminate_parms *ttyeliminatep)
{
 int sts;
 lock_object(ttyeliminatep->path);
 sts = unsafe_sm_path_ttyeliminate(ttyeliminatep);
 unlock_object(ttyeliminatep->path);
 return sts;
}



STATIC int unsafe_sm_path_status(struct sm_path_status_parms *statusp)
{
 tSMPathId path = statusp->path;
 tSMGroove *grv = &path->groove;
 uint32_t msg[16];
 unsigned long code;
 int sts;
 if (path->state == PATH_STATE_IDLE) return err(ERR_SM_WRONG_CHANNEL_STATE);
 sts = grv->rdmsg(grv, msg, SMGROOVE_RDMSG_FLAG_NOWAIT);
 if (sts) return sts;
 code = msg[0] & 0xffff0000;	
 if (MSG2ANYCODE(code) == MSG_DEAD) {
	if ((code & 0xffff0000) != MESSAGE(MSG_DEAD, 1)) {
		switch (msg[1]) {
		case MSG_DEAD_CAUSE_BADFUNC:
			return err(ERR_SM_NO_SUCH_FIRMWARE);
		case MSG_DEAD_CAUSE_NOMEM:
			return err(ERR_SM_NO_RESOURCES);
		case MSG_DEAD_CAUSE_PARAM:
			return err(ERR_SM_BAD_PARAMETER);
		case MSG_DEAD_CAUSE_NOINIT:
		case MSG_DEAD_CAUSE_FAILED:
		case MSG_DEAD_CAUSE_BADGEN:
		case MSG_DEAD_CAUSE_DUPID:
			return err(ERR_SM_DEVERR);
		case MSG_DEAD_CAUSE_DISCO:
			return err(ERR_SM_DISCONNECTED);
		}
	}
	path->state = PATH_STATE_IDLE;
	statusp->status = kSMPathStatusStopped;
	return 0;
 } else if (code == MESSAGE(MSG_BADMSG, 3)) {
	if ((msg[1] & 0xffff0000) == MESSAGE(MSG_CREATE, 3)
		|| (msg[1] & 0xffff0000) == MESSAGE(MSG_ADDSUB, 2)) {
		switch (msg[2]) {
		case MSG_BADMSG_CAUSE_NOTASK:
			return err(ERR_SM_NO_SUCH_CHANNEL);
		case MSG_BADMSG_CAUSE_UNHANDLED:
			return err(ERR_SM_WRONG_FIRMWARE_TYPE);
		case MSG_BADMSG_CAUSE_RANGE:
			return err(ERR_SM_NO_SUCH_FIRMWARE);
		case MSG_BADMSG_CAUSE_MEMORY:
			return err(ERR_SM_NO_RESOURCES);
		}
		return err(ERR_SM_DEVERR);
	} else if ((msg[1] & 0xffff0000) == MESSAGE(MSG_FRIEND, 2)) {
		if (msg[2] == MSG_BADMSG_CAUSE_NOTASK) {
			//This means there was no valid datafeed - shouldn't happen
			return err(ERR_SM_DEVERR);
		}
		if (msg[2] == MSG_BADMSG_CAUSE_WOULD_CYCLE) {
			return err(ERR_SM_BAD_DATAFEED_CONNECT);
		}
	} else if ((msg[1] & 0xffff0000) == MESSAGE(MSG_SET_EC_TONE_ACTION, 2)) {
		if (msg[2] == MSG_BADMSG_CAUSE_RANGE) {
			return err(ERR_SM_WRONG_FIRMWARE_TYPE);
		}
	}
 } else if (code == MESSAGE(MSG_DETECT_TONE, 3)) {
	 statusp->status = kSMPathStatusEndTone;
	 statusp->u.tone.id = msg[1];
	 statusp->u.tone.duration = msg[2];
 } else if (code == MESSAGE(MSG_DETECT_CONT, 3)) {
	 statusp->status = kSMPathStatusOngoingTone;
	 statusp->u.tone.id = msg[1];
	 statusp->u.tone.duration = msg[2];
 }
 return 0;
}

STATIC int real_sm_path_status(struct sm_path_status_parms *statusp)
{
 int sts;
 lock_object(statusp->path);
 sts = unsafe_sm_path_status(statusp);
 unlock_object(statusp->path);
 return sts;
}

STATIC int real_sm_path_get_event(struct sm_path_event_parms *eventp)
{
 tSMPathId path = eventp->path;
 int sts;
 lock_object(path);
 sts = path->groove.getev(&path->groove, &eventp->event);
 unlock_object(path);
 return sts;
}

STATIC int real_sm_path_get_datafeed(struct sm_path_datafeed_parms *datafeedp)
{
 tSMPathId path = datafeedp->path;
 tSMGroove *grv;
 int sts;
 lock_object(path);
 grv = &path->groove;
 sts = grv->get_df(grv,path->dataf, path->mod);
 if (!sts) datafeedp->datafeed = path->dataf.vtbl_data;
 unlock_object(path);
 return sts;
}

STATIC int real_sm_path_datafeed_connect(struct sm_path_datafeed_connect_parms *datafeedp)
{
 tSMPathId path = datafeedp->path;
 tSMGroove *grv;
 DATAFEED df;
 int sts;
 lock_object(path);
 grv = &path->groove;
 df.vtbl_data = datafeedp->data_source;
 sts = grv->connect_df(grv, &path->dfin, df);
 if (!sts) sts = grv->engage_df(grv, df, DFC_IN);
 unlock_object(path);
 return sts;
}

STATIC int real_sm_path_stop(struct sm_path_stop_parms *stopp)
{
 tSMPathId path = stopp->path;
 int sts = 0;
 lock_object(path);
 if (path->state != PATH_STATE_RUNNING) {
	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else {
	tSMGroove *grv = &path->groove;
	sts = grv->sendmsg(grv, MESSAGE(MSG_DEAD, 1));
	if (!sts) path->state = PATH_STATE_STOPPING;
 }
 unlock_object(path);
 return sts;
}

STATIC int real_sm_path_destroy(tSMPathId path)
{
 if (path) path_dtor(path);
 return 0;
}

#include "../pubdoc/gen/prospath.i"
