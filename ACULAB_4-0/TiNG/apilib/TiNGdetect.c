/* TiNGdetect.i - detection library */

#include "TiNGdetect.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "../hsif/catsig.h"
#include "../hsif/beepdet.h"
#include "../hsif/grunt.h"
#include "../hsif/iwr.h"
#include "../hsif/kernel.h"
#include "../hsif/onhook.h"
#include "../hsif/play.h"
#include "../hsif/speed.h"
#include "../hsif/subtask.h"
#include "../hsif/tonedet.h"
#include "../hsif/typecode.h"
#include "TiNGanyevent.h"
#include "TiNGcommon.h"
#include "TiNGo_mod.h"
#include "trace.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

LOCALDEF int create_detpar(struct module *mod, struct dettoneset *sp)
{
 struct ting_message msg;
 struct dettone *dp;
 unsigned t;
 union {
 	float f;
 	U32 l;
 } u;
 int sts;
 if (cardconn_valid(&sp->chan.cx)) return 0;
 sts = open_channel(&sp->chan, mod);
 if (sts) return sts;
 sts = starttask(&sp->chan.cx, TASKTC_TONEDETPAR, 0);
 if (sts) return sts;
 memset(&msg, 0, sizeof(msg));
 msg.ioctl_length = sizeof(msg);
 msg.flags = 0;
 msg.msg[0] = MESSAGE(MSG_SET_GENPAR, 5);
 u.f = sp->req_third_peak;
 msg.msg[1] = u.l;
 u.f = sp->req_signal_to_noise_ratio;
 if (u.f > 1) {
 	// it's in dB
	/* S = N * 10^(x/10)
	 * value required is S / (S + N) = 1 / (1 + N/S)
	 * N/S = N / (N * 10^(x/10)) = 10^(-x/10)
	 */
	u.f = 1 / (1 + pow(10.0, -u.f / 10.0));
 }
 msg.msg[2] = u.l;
 u.f = sp->req_minimum_power;
 if (u.f <= 100) {
 	// it's in dBm0
 	/* magic number 4.5e11 is determined by experiment
 	 * the actual value depends on the frequency (if it hits
 	 * the centre of a bin the value is slightly bigger than the
 	 * edge of a bin) varying between 4.5e11 and 5.5e11 for 0dBm0
 	 * We pick the lower limit as we want to guarantee detection
 	 * at the selected level.
 	 */
 	u.f = 4.5e11 * pow(10.0, u.f / 10.0);
 }
 msg.msg[3] = u.l;
 u.f = sp->req_twist_for_dual_tone;
 msg.msg[4] = u.l;
 if (ERR(cx_ioctl(&sp->chan.cx, IOCTL_TiNG_WRMSG, &msg, sizeof(msg)))) {
	return err(ERR_SM_DEVERR);
 }
 memset(&msg, 0, sizeof(msg));
 msg.ioctl_length = sizeof(msg);
 msg.flags = 0;
 msg.msg[0] = MESSAGE(MSG_TONEBAND, 3);
 msg.msg[1] = sp->start_freq;
 msg.msg[2] = sp->stop_freq;
 if (ERR(cx_ioctl(&sp->chan.cx, IOCTL_TiNG_WRMSG, &msg, sizeof(msg)))) {
	return err(ERR_SM_DEVERR);
 }
 dp = mod->det.tonelist;
 memset(&msg, 0, sizeof(msg));
 msg.ioctl_length = sizeof(msg);
 msg.msg[0] = MESSAGE(MSG_SET_DETTONE, 6);
 	// use descending order for efficiency on SHARC
 for (t=sp->numtones; t--; ) {
	const unsigned high = sp->tonelist[t].thigh;
	const unsigned low = sp->tonelist[t].tlow;
	msg.msg[1] = t;
	if (high == TONEID_NONE) continue;	// silence
	if (low == TONEID_NONE) {
		msg.msg[2] = 0;
		msg.msg[3] = 0;
	} else {
		if (low > mod->det.ntone)
			return err(ERR_SM_BAD_PARAMETER);
		u.f = dp[low].minfreq;
		msg.msg[2] = u.l;
		u.f = dp[low].maxfreq;
		msg.msg[3] = u.l;
	}
	if (high > mod->det.ntone) return err(ERR_SM_BAD_PARAMETER);
	u.f = dp[high].minfreq;
	msg.msg[4] = u.l;
	u.f = dp[high].maxfreq;
	msg.msg[5] = u.l;
	if (ERR(cx_ioctl(&sp->chan.cx, IOCTL_TiNG_WRMSG, &msg, sizeof(msg))))
		return err(ERR_SM_DEVERR);
 }
 return sts;
}

STATIC int unsafe_notify_detpar(tSMChannelId chan, CARDCONN *cx, struct dettoneset *sp, int isrec, int hyst)
{
 int sts;
 int hystoff = hyst; // in case none set
 if (hyst == 2) { // mode is ...MinDuration64, so use toneset min on and off times if set
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
	// else neither set so use 64ms default (i.e. the 2 passed in)
 }
 sts = sendmsg2(cx, MSG_SET_HYST, hyst); // for firmware that doesn't support independent values
 if (sts) return sts;
 sts = sendmsg3(cx, MSG_SET_HYST, hyst, hystoff); // for firmware that does support independent values
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_TONES_NARROW, sp->narrow);
 if (sts) return sts;
 chan->recog[isrec].tone_detect_band1len = sp->band2len ? sp->band1len : 0;
 sts = create_detpar(chan->mod, sp);
 if (sts) return sts;
 sts = setfriend(cx, &sp->chan, FRIEND_TYPE_TONE_DETPAR);
 return sts;
}

LOCALDEF int notify_detpar(tSMChannelId chan, CARDCONN *cx, int toneset, int isrec, int hyst)
{
 struct dettoneset *tset;
 int sts = 0;
 if (!cardconn_valid(cx)) return 0;
 mutx_enter(&chan->mod->det.mx);
 tset = chan->mod->det.tonesetlist[toneset];
 if (tset) sts = unsafe_notify_detpar(chan, cx, tset, isrec, hyst);
 mutx_leave(&chan->mod->det.mx);
 return sts;
}

LOCALDEF int real_sm_add_input_freq_coeffs(struct sm_input_freq_coeffs_parms *pp)
{
 struct module *mp = modid2lockedmodule(pp->module);
 struct dettone *dp;
 if (!mp) return err(ERR_SM_NO_SUCH_MODULE);
 dp = alloc(mp->det.tonelist, sizeof(*dp) * (mp->det.ntone + 1));
 if (!dp) {
	unlock_module(mp);
	return err(ERR_SM_NO_RESOURCES);
 }
 mp->det.tonelist = dp;
 dp += mp->det.ntone;
 dp->minfreq = pp->lower_limit;
 dp->maxfreq = pp->upper_limit;
 pp->id = mp->det.ntone++;
 unlock_module(mp);
 return 0;
}

STATIC int unsafe_sm_add_input_tone_set(struct sm_input_tone_set_parms *pp, struct module *mp)
{
 unsigned band1len = pp->band1_freq_count;
 unsigned band2len = pp->band2_freq_count;
 const unsigned ntp = (band1len ? band1len : 1) * (band2len ? band2len : 1);
 unsigned band1ofs = pp->band1_first_freq_coeffs_id;
 unsigned band2ofs = pp->band2_first_freq_coeffs_id;
 struct dettoneset **tblp;
 struct dettoneset *sp;
 struct tonepair *tp;
 struct dettone *dp;
 unsigned i;
 if (!band1len) {
		// check if both zero length
	if (!band2len) return err(ERR_SM_BAD_PARAMETER);
	band1len = band2len;
	band2len = 0;
	band1ofs = band2ofs;
 }
 if (band1ofs + band1len > mp->det.ntone) {
	return err(ERR_SM_BAD_PARAMETER);
 }
 if (band2len && band2ofs + band2len > mp->det.ntone) {
	return err(ERR_SM_BAD_PARAMETER);
 }
 dp = mp->det.tonelist;
 tp = alloc(0, sizeof(*tp) * ntp);
 if (!tp) return err(ERR_SM_NO_RESOURCES);
 if (!band2len) {
	unsigned b1;
	for (b1 = 0; b1 < band1len; b1++) {
		tp[b1].tlow = TONEID_NONE;
		tp[b1].thigh = band1ofs+b1;
		if (dp[band1ofs + b1].maxfreq == 0) {
			tp[b1].thigh = TONEID_NONE;
		}
	}
 } else {
	unsigned b1;
 	for (b1 = 0; b1 < band1len; b1++) {
		const unsigned tlow = dp[band1ofs + b1].maxfreq == 0
			? TONEID_NONE : band1ofs+b1;
		unsigned b2;
		unsigned slot = b1;
		for (b2 = 0; b2 < band2len; b2++) {
			const unsigned thigh = dp[band2ofs + b2].maxfreq == 0
				? TONEID_NONE : band2ofs+b2;
			if (tlow == TONEID_NONE) {
				tp[slot].tlow = TONEID_NONE;
				tp[slot].thigh = thigh;
			} else if (thigh == TONEID_NONE) {
				tp[slot].tlow = TONEID_NONE;
				tp[slot].thigh = tlow;
			} else if (dp[tlow].minfreq < dp[thigh].minfreq) {
				tp[slot].tlow = tlow;
				tp[slot].thigh = thigh;
			} else {
				tp[slot].tlow = thigh;
				tp[slot].thigh = tlow;
			}
			slot += band1len;
		}
	 }
 }
 tblp = alloc(mp->det.tonesetlist, sizeof(*tblp) * (mp->det.ntoneset + 1));
 if (!tblp) {
	free(tp);
	return err(ERR_SM_NO_RESOURCES);
 }
 mp->det.tonesetlist = tblp;
 	// guaranteed space for a new item, if necessary
 for (i=0; ; i++) {
	if (i >= mp->det.ntoneset) {
		sp = alloc(0, sizeof(*sp));
		if (!sp) {
			free(tp);
			return err(ERR_SM_NO_RESOURCES);
		}
		mp->det.tonesetlist[i] = sp;
		mp->det.ntoneset++;
		break;
	}
	sp = mp->det.tonesetlist[i];
	if (!sp) break;
	if (sp->req_signal_to_noise_ratio == pp->req_signal_to_noise_ratio
		&& sp->req_third_peak == pp->req_third_peak
		&& sp->req_minimum_power == pp->req_minimum_power
		&& sp->req_twist_for_dual_tone == pp->req_twist_for_dual_tone
		&& sp->band1len == band1len
		&& sp->band2len == band2len
		&& sp->numtones == ntp) {
		unsigned t;
		for (t=0; ; t++) {
			if (t >= ntp) {
				pp->id = i;
				free(tp);
				return 0;
			}
			if (sp->tonelist[t].tlow != tp[t].tlow
				|| sp->tonelist[t].thigh != tp[t].thigh) break;
		}
	}
 }
 pp->id = i;
 cardconn(&sp->chan.cx);
 sp->chan.channo = 0;
 sp->start_freq = MOD_DEF_DET_TONE_START;
 sp->stop_freq = MOD_DEF_DET_TONE_STOP;
 sp->mintime = 0;
 sp->mintimeoff = 0;
 sp->narrow = 0;
 sp->req_third_peak = pp->req_third_peak;
 sp->req_signal_to_noise_ratio = pp->req_signal_to_noise_ratio;
 sp->req_minimum_power = pp->req_minimum_power;
 sp->req_twist_for_dual_tone = pp->req_twist_for_dual_tone;
 sp->tonelist = tp;
 sp->numtones = ntp;
 sp->band1len = band1len;
 sp->band2len = band2len;
 return 0;
}

LOCALDEF int real_sm_add_input_tone_set(struct sm_input_tone_set_parms *pp)
{
 struct module *mp = modid2lockedmodule(pp->module);
 int sts;
 if (!mp) return err(ERR_SM_NO_SUCH_MODULE);
 mutx_enter(&mp->det.mx);
 sts = unsafe_sm_add_input_tone_set(pp, mp);
 mutx_leave(&mp->det.mx);
 unlock_module(mp);
 return sts;
}

LOCALDEF int real_sm_add_input_cptone(struct sm_input_cptone_parms *pp)
{
 struct module *mp = modid2lockedmodule(pp->module);
 struct cptone *cpt;
 struct cplist *cpl;
 int i;
 if (!mp) return err(ERR_SM_NO_SUCH_MODULE);
 cpt = alloc(0, sizeof(*cpt) * pp->state_count);
 if (!cpt) {
	unlock_module(mp);
	return err(ERR_SM_NO_RESOURCES);
 }
 cpl = alloc(mp->cplist, (mp->ncptone + 1) * sizeof(*cpl));
 if (!cpl) {
 	free(cpt);
	unlock_module(mp);
 	return err(ERR_SM_NO_RESOURCES);
 }
 mp->cplist = cpl;
 cpl += mp->ncptone++;
 cpl->tonelist = cpt;
 cpl->listlen = pp->state_count;
 if (mp->maxhist < cpl->listlen) mp->maxhist = cpl->listlen;
 cpl->id = pp->id;
 cpl->dbgname = "user";
 for (i=0; i<pp->state_count; cpt++, i++) {
 	cpt->id = pp->states[i].freq_id;
 	cpt->maxdur = pp->states[i].maximum_cadence;
 	cpt->mindur = pp->states[i].minimum_cadence;
	if (cpt->maxdur == ~0u && cpt->mindur < mp->mincont) {
		mp->mincont = cpt->mindur;
	}
 }
 unlock_module(mp);
 return 0;
}

LOCALDEF int real_sm_reset_input_cptones(struct sm_reset_input_cptones_parms *pp)
{
 struct module *mp = modid2lockedmodule(pp->module);
 struct cplist *cpl;
 if (!mp) return err(ERR_SM_NO_SUCH_MODULE);
 for (cpl = mp->cplist; mp->ncptone--; cpl++) {
	free(cpl->tonelist);
 }
 free(mp->cplist);
 mp->cplist = 0;
 mp->ncptone = 0;
 mp->maxhist = 0;
 mp->mincont = 60000;
 mp->cptoneset = pp->tone_set_id;
 unlock_module(mp);
 return 0;
}

LOCALDEF int start_listen(struct sm_listen_for_parms *pp, enum handles h, int isrec)
{
	// below only used in non-proxied stuff
 const tSMChannelId cp = pp->channel;
 unsigned hyst=0, end=0, len=0 ,usedetchan=0;
 unsigned tsset = ~0u;
 int sts = 0;
 if (!sts && pp->enable_cptone_recognition) {
	if (pp->tone_detection_mode != kSMToneDetectionNone) {
		sts = err(ERR_SM_BAD_PARAMETER);
	} else {
			// alloc hist buffer as long as longest sequence
		cp->recog[isrec].histlen = cp->mod->maxhist;
		cp->recog[isrec].hist = alloc(0, sizeof(*cp->recog[isrec].hist) * cp->recog[isrec].histlen);
		if (!cp->recog[isrec].hist) sts = err(ERR_SM_NO_RESOURCES);
		memset(cp->recog[isrec].hist, 0, sizeof(*cp->recog[isrec].hist) * cp->recog[isrec].histlen);
		cp->recog[isrec].hist_cont = 1;	// might as well - saves time
	}
	if (!sts) sts = startdet(cp, h, DETTYPE_TONE_NORMAL, isrec);
	if (!sts) sts = notify_detpar(cp, &cp->chan[h].cx, cp->mod->cptoneset, isrec, 1); // 1 for 32ms detector
	if (!sts) sts = sendmsg2(&cp->chan[h].cx, MSG_SET_CONT_TIM, cp->mod->mincont);
	if (!sts) sts = startdet(cp, h, DETTYPE_TONEREP, isrec);
	if (!sts) sts = setfriend(&cp->chan[h].cx, &cp->chan[h], FRIEND_TYPE_TONE_DETECTOR);
 }
 if (pp->enable_grunt_detection) {
	double mnl = pp->min_noise_level;
	double thresh = pp->grunt_threshold;
	U32 hoff = pp->grunt_holdoff;
	if (!sts) sts = startdet(cp, h, DETTYPE_GRUNT, isrec);
	if (hoff) {
		if (!sts) sts = sendmsg4(&cp->chan[h].cx,MSG_GRUNT_LATENCY, pp->grunt_latency, 0, hoff);
	} else {
		if (!sts) sts = sendmsg2(&cp->chan[h].cx,MSG_GRUNT_LATENCY, pp->grunt_latency);
	}
	if (!pp->min_noise_level) {
		mnl = -55.0;
		thresh = 15.0;
	}
	mnl = exp((mnl + 90) / 10 * log(10)); // 10^9 adjusted by dB
	// conveniently this scale gives almost exactly the values need for the sharc firmware at 64 samples per epoch!
	thresh = 65536 * exp(thresh/10 * log(10)); // dB to factor scaled by 65536, NB power not amplitude
	if (!sts) sts = sendmsg3(&cp->chan[h].cx, MSG_GRUNT_LEVELS, mnl, thresh);
 }
 tsset = pp->active_tone_set_id;
 if (tsset >= cp->mod->det.ntoneset) tsset = ~0u;
 switch (pp->tone_detection_mode) {
 case kSMToneDetectionNone:
	return sts;
 case kSMToneLenDetectionNoMinDuration:
 	len = 1;
 	// fall in...
 case kSMToneEndDetectionNoMinDuration:
 	end = 1;
 	// fall in...
 case kSMToneDetectionNoMinDuration:
 	hyst = 1;
	break;
 case kSMToneLenDetectionMinDuration40:
 	len = 1;
 	// fall in...
 case kSMToneEndDetectionMinDuration40:
 	end = 1;
 	// fall in...
 case kSMToneDetectionMinDuration40:
	break;
 case kSMToneLenDetectionMinDuration64:
 	len = 1;
 	// fall in...
 case kSMToneEndDetectionMinDuration64:
 	end = 1;
 	// fall in...
 case kSMToneDetectionMinDuration64:
 	hyst = 2;
	break;
 case kSMToneDetectionAsListenFor:
	if (!isrec) sts = err(ERR_SM_BAD_PARAMETER); // only valid for recordings
	usedetchan=1;
	break;
 default: sts = err(ERR_SM_NOT_IMPLEMENTED);
 	if (TiNGtrace > 1) olog("(no detection mode %d)", pp->tone_detection_mode);
 }
 if (!sts && !usedetchan) {
	if (tsset == ~0u) sts = err(ERR_SM_BAD_PARAMETER);
	cp->recog[isrec].tone_detect_end = end;
	cp->recog[isrec].tone_detect_map_digits =
		pp->map_tones_to_digits == kSMDTMFToneSetDigitMapping;
	cp->recog[isrec].tone_detect_len = len;
	if (!sts) sts = startdet(cp, h, DETTYPE_TONE_NORMAL, isrec);
	if (!sts) sts = notify_detpar(cp, &cp->chan[h].cx, tsset, isrec, hyst);
	if (!sts) sts = sendmsg2(&cp->chan[h].cx, MSG_SET_CONT_TIM, 60000);
 }
 if (!sts){
	RAWCHAN *rcp = &cp->chan[usedetchan?HANDLE_DET:h];
	if (cardconn_valid(&rcp->cx)){
		sts = startdet(cp, h, DETTYPE_TONEREP, isrec);
		if (!sts) sts = setfriend(&cp->chan[h].cx, rcp, FRIEND_TYPE_TONE_DETECTOR);
		if (!sts && usedetchan) { // need to copy some parameters to get reporting right
			cp->recog[1].tone_detect_end=cp->recog[0].tone_detect_end;
			cp->recog[1].tone_detect_len=cp->recog[0].tone_detect_len;
			cp->recog[1].tone_detect_band1len=cp->recog[0].tone_detect_band1len;
		}
	}
 }
 return sts;
}

//holds lock : anychan
LOCALDEF int real_sm_listen_for(struct sm_listen_for_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 int sts = 0;
#ifndef OMIT_ANYCHAN
 const int lockany = anychan.inuse & (1 << EVENT_NOTIFY);
 if (lockany) lock_object(&anychan);
#endif
 lock_object(cp);
 	// stop any previous detections
 if (!sts) sts = stopdet(cp, HANDLE_DET, DETTYPE_GRUNT, 0);
 if (!sts) sts = stopdet(cp, HANDLE_DET, DETTYPE_TONE_NORMAL, 0);
 if (!sts) sts = stopdet(cp, HANDLE_DET, DETTYPE_TONEREP, 0);
 free(cp->recog[0].hist);
 cp->recog[0].hist = 0;
#ifndef OMIT_ANYCHAN
 if (!sts && cp->anyev[EVENT_NOTIFY].evstate != ANYEV_NOTANY) {
	sts = anychan_newstate(&cp->anyev[EVENT_NOTIFY], ANYEV_IDLE);
 }
#endif
 if (!sts) sts = start_listen(pp, HANDLE_DET, 0);
#ifndef OMIT_ANYCHAN
 if (!sts && cp->anyev[EVENT_NOTIFY].evstate == ANYEV_IDLE) {
	sts = anychan_newstate(&cp->anyev[EVENT_NOTIFY], ANYEV_WAIT);
 }
#endif
 unlock_object(cp);
#ifndef OMIT_ANYCHAN
 if (lockany) unlock_object(&anychan);
#endif
 return sts;
}

//holds lock : anychan
LOCALDEF int real_sm_ans_listen_for(struct sm_ans_listen_for_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 int sts = 0;
#ifndef OMIT_ANYCHAN
 const int lockany = anychan.inuse & (1 << EVENT_NOTIFY);
 if (lockany) lock_object(&anychan);
#endif
 lock_object(cp);
 	// stop any previous detections
 if (!sts) sts = stopdet(cp, HANDLE_DET, DETTYPE_ANSAM, 0);
#ifndef OMIT_ANYCHAN
 if (!sts && cp->anyev[EVENT_NOTIFY].evstate != ANYEV_NOTANY) {
	sts = anychan_newstate(&cp->anyev[EVENT_NOTIFY], ANYEV_IDLE);
 }
#endif
 switch (pp->detection_mode) {
 case kSMANSModeDisable:
	break;
 case kSMANSModeDetect:
	if (!sts) sts = startdet(cp, HANDLE_DET, DETTYPE_ANSAM, 0);
	break;
 default: sts = err(ERR_SM_NOT_IMPLEMENTED);
 	if (TiNGtrace > 1) olog("(no detection mode %d)", pp->detection_mode);
 }
#ifndef OMIT_ANYCHAN
 if (!sts && cp->anyev[EVENT_NOTIFY].evstate == ANYEV_IDLE) {
	sts = anychan_newstate(&cp->anyev[EVENT_NOTIFY], ANYEV_WAIT);
 }
#endif
 unlock_object(cp);
#ifndef OMIT_ANYCHAN
 if (lockany) unlock_object(&anychan);
#endif
 return sts;
}

STATIC int checkhist(struct sm_recognised_parms *pp, int isrec)
{
 const tSMChannelId cp = pp->channel;
 struct cplist *cpl = cp->mod->cplist;
 unsigned cplen = cp->mod->ncptone;
 if (!cp->recog[isrec].hist_cont && cp->recog[isrec].histlen > 1 && cp->recog[isrec].hist[0].id == cp->recog[isrec].hist[1].id) {
		// tone broken by glitch - merge back together
	cp->recog[isrec].hist[1].dur += cp->recog[isrec].hist[0].dur;
	memmove(cp->recog[isrec].hist, cp->recog[isrec].hist+1, sizeof(*cp->recog[isrec].hist) *
	(cp->recog[isrec].histlen - 1));
	cp->recog[isrec].hist[cp->recog[isrec].histlen-1].dur = 0;
 }
 if (TiNGtrace > 3) {
	unsigned i;
	olog("Tones:");
	if (cp->recog[isrec].hist_cont) olog(" ...");
	for (i=0; i<cp->recog[isrec].histlen; i++) {
		olog(" %d@%d", cp->recog[isrec].hist[i].id, cp->recog[isrec].hist[i].dur);
	}
	olog("\n");
 }
 for (; cplen--; cpl++) {	// try each sequence
 	unsigned ps;
 	for (ps=0; ps < cpl->listlen; ps++) {	// each possible start
 		struct cptone *tp = &cpl->tonelist[ps];
 		unsigned j,k;
		if (cp->recog[isrec].hist_cont && tp->maxdur != ~0U)
			continue;	// don't match continuous if max
 		for (j=0,k=0; ; ) {
			if (k == cpl->listlen) {
				pp->type = kSMRecognisedCPTone;
				pp->param0 = cpl->id;
				if (TiNGtrace > 1) {
					olog("CPtone(%s)", cpl->dbgname);
				}
				cp->recog[isrec].hist[0].id = ~0U;
				return 0;
			}
			if (tp->id != cp->recog[isrec].hist[j].id) {
				if (tp->mindur) break;
			} else if (tp->mindur > cp->recog[isrec].hist[j].dur
				|| cp->recog[isrec].hist[j].dur > tp->maxdur) {
				break;
			} else j++;
			k++;
			tp++;
			if (tp == &cpl->tonelist[cpl->listlen])
				tp = cpl->tonelist;
		}
	}
 }
 return 0;
}

LOCALDEF int handle_detected(int *done, struct sm_recognised_parms *pp, unsigned code, struct ting_message *msg, int isrec)
{
 const tSMChannelId cp = pp->channel;
 int endtone = 0;
 *done = 1;
 pp->type = kSMRecognisedNothing;
 switch (code) {
 case MESSAGE(MSG_DETECT_TONE, 3):
 	endtone = msg->msg[2];
		// fall in ...
 case MESSAGE(MSG_DETECT_CONT, 3):
	if (cp->recog[isrec].hist) {
		int i;
		if (cp->recog[isrec].hist[0].id != msg->msg[1]) {
			memmove(cp->recog[isrec].hist+1,
				cp->recog[isrec].hist,
				sizeof(*cp->recog[isrec].hist) * (cp->recog[isrec].histlen - 1));
			cp->recog[isrec].hist[0].id = msg->msg[1];
		}
		cp->recog[isrec].hist[0].dur = msg->msg[2];
		cp->recog[isrec].hist_cont = code == MESSAGE(MSG_DETECT_CONT, 3);
		i = checkhist(pp, isrec);
		if (i || pp->type != kSMRecognisedNothing) return i;
	} else if (msg->msg[1]	// don't count unrecognised periods
		&& !cp->recog[isrec].tone_detect_end == !endtone) {
		STATIC char dtmfnames[] = "\000147*2580369#ABCD";
		unsigned bl = cp->recog[isrec].tone_detect_band1len;
		if (cp->recog[isrec].tone_detect_map_digits
			&& msg->msg[1] < arlen(dtmfnames)) {
			pp->type = kSMRecognisedDigit;
			pp->param0 = dtmfnames[msg->msg[1]];
			if (cp->recog[isrec].tone_detect_len) {
				pp->param1 = msg->msg[2];
			} else {
				pp->param1 = kSMDTMFDigits;
			}
			return 0;
		}
		pp->type = kSMRecognisedTone;
		pp->param1 = msg->msg[1] - 1;
		if (bl) {
			pp->param0 = pp->param1 % bl;
			pp->param1 /= bl;
		} else pp->param0 = 0;
		if (cp->recog[isrec].tone_detect_len) {
			pp->param0 += 256 * pp->param1;
			pp->param1 = msg->msg[2];
		}
		return 0;
	}
	break;
 case MESSAGE(MSG_GRUNT, 3):
	if (!msg->msg[1]) {
		pp->type = kSMRecognisedGruntStart;
		pp->param0 = msg->msg[2];
		pp->param1 = 0;
	} else {
		pp->type = kSMRecognisedGruntEnd;
		pp->param0 = msg->msg[1];
		pp->param1 = msg->msg[2];
	}
	return 0;
 case MESSAGE(MSG_IWR_DETECT, 4):
	switch(msg->msg[1]) {
	case IWR_RESULT_ACCEPT:
		pp->type = kSMRecognisedASRResult;
		break;
	case IWR_RESULT_UNCERTAIN:
		pp->type = kSMRecognisedASRUncertain;
		break;
	case IWR_RESULT_REJECT:
	case IWR_WARNING_FAILED:
		pp->type = kSMRecognisedASRRejected;
		break;
	case IWR_WARNING_TIMEOUT:
		pp->type = kSMRecognisedASRTimeout;
		break;
	case IWR_ERROR_NONOISE:
		return err(ERR_SM_IWR_NO_NOISE);
	case IWR_ERROR_EXCESSNOISE:
		return err(ERR_SM_IWR_EXCESS_NOISE);
	case IWR_ERROR_NOMODELS:
		return err(ERR_SM_IWR_NO_MODEL);
	case IWR_ERROR_EXCESSMODELS:
		return err(ERR_SM_IWR_EXCESS_MODELS);
	case IWR_ERROR_EXCESSSTATES:
		return err(ERR_SM_IWR_EXCESS_STATES);
	case IWR_ERROR_MODELCHANGED:
		return err(ERR_SM_IWR_MODEL_CHANGED);
	case IWR_ERROR_MISSING:
		return err(ERR_SM_IWR_MISSING);
	default:
		return 0;
	}
	if (msg->msg[2] < cp->recog[0].numrecog) {
		pp->param0 = cp->recog[0].recog_id[msg->msg[2]];
	} else pp->param0 = 0;
	if (msg->msg[3] < cp->recog[0].numrecog) {
		pp->param1 = cp->recog[0].recog_id[msg->msg[3]];
	} else pp->param1 = 0;
	return 0;
 case MESSAGE(MSG_CATSIG_DETECTED, 2):
	pp->type = kSMRecognisedCatSig;
	pp->param0 = cp->catsig;
	pp->param1 = msg->msg[1];
	return 0;
 case MESSAGE(MSG_ANS_DETECT_TONE, 2):
 case MESSAGE(MSG_ANS_DETECT_TONE, 3):
	pp->type = kSMRecognisedANS;
	pp->param0 = msg->msg[1];
	pp->param1 = (code == MESSAGE(MSG_ANS_DETECT_TONE, 2)) ? 0 : msg->msg[2];
	return 0;
 case MESSAGE(MSG_BEEP_DETECTED, 3):
	pp->type = kSMRecognisedBeep;
	pp->param0 = msg->msg[1];
	pp->param1 = msg->msg[2];
	return 0;
 case MESSAGE(MSG_ONHOOK_DETECTED, 1):
	pp->type = kSMRecognisedOnHook;
	pp->param0 = 0;
	pp->param1 = 0;
	return 0;
 default:
	*done = 0;
	break;
 }
 return 0;
}

STATIC int get_detected(struct sm_recognised_parms *pp, tSMChannelId chan)
{
 CARDCONN *cx = &chan->chan[HANDLE_DET].cx;
 if (!cardconn_valid(cx)) return 0;
 for (;;) {
	struct ting_message msg;
	unsigned long code;
	int done;
	int i;
	msg.flags = IOCTL_TiNG_RDMSG_FLAG_NOWAIT;
	i = rdmsg(&msg, cx);
	if (i) return i;
	code = msg.msg[0] & ~0xffff;
	if (! (code & 0x80000000) ) break;
	i = handle_detected(&done, pp, code, &msg, 0);
	if (i || pp->type != kSMRecognisedNothing) return i;
	if (!done) switch (code) {
	case MESSAGE(MSG_BADMSG, 3):
		if ((msg.msg[1] & 0xffff0000) == MESSAGE(MSG_ADDSUB, 2)) {
			switch (msg.msg[2]) {
			case MSG_BADMSG_CAUSE_NOTASK:
				return err(ERR_SM_NO_SUCH_CHANNEL);
			case MSG_BADMSG_CAUSE_UNHANDLED:
				return err(ERR_SM_WRONG_FIRMWARE_TYPE);
			case MSG_BADMSG_CAUSE_RANGE:
				return err(ERR_SM_NO_SUCH_FIRMWARE);
			case MSG_BADMSG_CAUSE_MEMORY:
				return err(ERR_SM_NO_RESOURCES);
			case MSG_BADMSG_CAUSE_LICENCE:
				return err(ERR_SM_NO_LICENCE);			
			}
			return err(ERR_SM_DEVERR);
		}
		break;
	case MESSAGE(MSG_OVERRUN, 1):
		pp->type = kSMRecognisedOverrun;
		return 0;
	case MESSAGE(MSG_DEAD, 4):
	case MESSAGE(MSG_DEAD, 3):
	case MESSAGE(MSG_DEAD, 2):
		switch (msg.msg[1]) {
		case MSG_DEAD_CAUSE_BADFUNC:
			return err(ERR_SM_NO_SUCH_FIRMWARE);
		case MSG_DEAD_CAUSE_NOMEM:
			return err(ERR_SM_NO_RESOURCES);
		case MSG_DEAD_CAUSE_DISCO:
			return err(ERR_SM_DISCONNECTED);
		case MSG_DEAD_CAUSE_MSG:
			break; // stopping a detection may cause this
		default:
			return err(ERR_SM_DEVERR);
		}
		break;
	case MESSAGE(MSG_DEAD, 1):
		if (!chan->recog[0].detmask)
			return 0; // ignore fake deads if we're not running detection, to be compatible with old library versions
		else
			return err(ERR_SM_DEVERR);
	default:
		// ignore it
		break;
	}
 }
 return 0;
}

LOCALDEF int unsafe_sm_get_recognised(struct sm_recognised_parms *pp)
{
 const tSMChannelId chan = pp->channel;
 pp->type = kSMRecognisedNothing;
 return get_detected(pp, chan);
}

LOCALDEF int real_sm_get_recognised(struct sm_recognised_parms *pp)
{
 const tSMChannelId chan = pp->channel;
 int sts;
 if (!chan) {
#ifndef OMIT_ANYCHAN
	if (! (anychan.inuse & (1 << EVENT_NOTIFY)) ) {
		return err(ERR_SM_WRONG_CHANNEL_STATE);
	}
	return sm_get_recognised_anystatus(pp);
#else
	return err(ERR_SM_NOT_IMPLEMENTED);
#endif
 }
 lock_object(chan);
 sts = unsafe_sm_get_recognised(pp);
 unlock_object(chan);
 return sts;
}

LOCALDEF int real_sm_discard_recognised(tSMChannelId chan)
{
 int sts;
 lock_object(chan);
 for (;;) {
	struct sm_recognised_parms rp;
	memset(&rp, 0, sizeof(rp));
	rp.channel = chan;
 	sts = unsafe_sm_get_recognised(&rp);
	if (sts || rp.type == kSMRecognisedNothing) break;
 }
 unlock_object(chan);
 return sts;
}

LOCALDEF int real_sm_adjust_input_tone_set(struct sm_adjust_tone_set_parms *pp)
{
 struct module *mp = modid2lockedmodule(pp->module);
 struct dettoneset *sp;
 int sts = 0;
 if (!mp) return err(ERR_SM_NO_SUCH_MODULE);
 if ((unsigned) pp->tone_set_id >= mp->det.ntoneset) {
	sts = err(ERR_SM_BAD_PARAMETER);
 } else {
	mutx_enter(&mp->det.mx);
	sp = mp->det.tonesetlist[pp->tone_set_id];
	switch (pp->parameter_id) {
	case kAdjustToneSetFPParamId3rdPeak:
		sp->req_third_peak = pp->parameter_value.fp_value;
		break;
	case kAdjustToneSetFPParamIdSNRatio:
		sp->req_signal_to_noise_ratio  = pp->parameter_value.fp_value;
		break;
	case kAdjustToneSetFPParamIdMinPower:
		sp->req_minimum_power = pp->parameter_value.fp_value;
		break;
	case kAdjustToneSetFPParamIdTwist:
		sp->req_twist_for_dual_tone = pp->parameter_value.fp_value;
		break;
	case kAdjustToneSetIntParamIdMinOnTime:
		sp->mintime = pp->parameter_value.int_value;
		break;
	case kAdjustToneSetIntParamIdMinOffTime:
		sp->mintimeoff = pp->parameter_value.int_value;
		break;
	case kAdjustToneSetFPParamIdStartFreq:
		sp->start_freq = pp->parameter_value.fp_value;
		break;
	case kAdjustToneSetFPParamIdStopFreq:
		sp->stop_freq = pp->parameter_value.fp_value;
		break;
 	default:
 		sts = err(ERR_SM_BAD_PARAMETER);
 		break;
 	}
	if (!sts && cardconn_valid(&sp->chan.cx)) {
		cx_close(&sp->chan.cx);
	}
 	mutx_leave(&mp->det.mx);
 }
 unlock_module(mp);
 return sts;
}

//holds lock : anychan
LOCALDEF int real_sm_beep_listen_for(struct sm_beep_listen_for_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 int sts = 0;
#ifndef OMIT_ANYCHAN
 const int lockany = anychan.inuse & (1 << EVENT_NOTIFY);
 if (lockany) lock_object(&anychan);
#endif
 lock_object(cp);
 	// stop any previous detections
 if (!sts) sts = stopdet(cp, HANDLE_DET, DETTYPE_BEEP, 0);
 if (!sts) sts = stopdet(cp, HANDLE_DET, DETTYPE_TONE_BEEP, 0);
#ifndef OMIT_ANYCHAN
 if (!sts && cp->anyev[EVENT_NOTIFY].evstate != ANYEV_NOTANY) {
	sts = anychan_newstate(&cp->anyev[EVENT_NOTIFY], ANYEV_IDLE);
 }
#endif
 if (pp->min_duration > 0) {
 	int need_detpar = !(cp->recog[0].detmask & TONEDET_DETMASK);
	if (!sts) sts = startdet(cp, HANDLE_DET, DETTYPE_TONE_BEEP, 0);
	if (need_detpar) {
		// if we started tonedet, send 'reasonable' default parameters
		if (!sts) sts = notify_detpar(cp, &cp->chan[HANDLE_DET].cx, 0, 0, 1);
		if (!sts) sts = sendmsg2(&cp->chan[HANDLE_DET].cx, MSG_SET_CONT_TIM, 60000);
	}
	if (!sts) sts = startdet(cp, HANDLE_DET, DETTYPE_BEEP, 0);
	if (!sts) sts = sendmsg4(&cp->chan[HANDLE_DET].cx, MSG_BEEPDET_PARMS, pp->min_duration, pp->lower_limit, pp->upper_limit);
 } else if (pp->min_duration < 0) {
	sts = err(ERR_SM_BAD_PARAMETER);
 }
#ifndef OMIT_ANYCHAN
 if (!sts && cp->anyev[EVENT_NOTIFY].evstate == ANYEV_IDLE) {
	sts = anychan_newstate(&cp->anyev[EVENT_NOTIFY], ANYEV_WAIT);
 }
#endif
 unlock_object(cp);
#ifndef OMIT_ANYCHAN
 if (lockany) unlock_object(&anychan);
#endif
 return sts;
}

//////////////////////////////////////////////////////////
// Convert dBm0 to equivalent 32-bit unsigned power value.
//////////////////////////////////////////////////////////
STATIC uint32_t onhook_dBm0convert(double dBm0)
{
//   return (uint32_t) floor(0.5 + 1034912258.0*pow(10.0, dBm0/10.0));
   return (uint32_t) floor(0.5 + (2.0*(2*2853.0)*(2*2853.0))*pow(10.0, dBm0/10.0)); // 4 * mulaw rms [(2*alaw)*(2*alaw)/2]
}


//holds lock : anychan
LOCALDEF int real_sm_onhook_listen_for(struct sm_onhook_listen_for_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 int sts = 0;
#ifndef OMIT_ANYCHAN
 const int lockany = anychan.inuse & (1 << EVENT_NOTIFY);
 if (lockany) lock_object(&anychan);
#endif
 lock_object(cp);
 	// stop any previous detections
 if (!sts) sts = stopdet(cp, HANDLE_DET, DETTYPE_ONHOOK, 0);
#ifndef OMIT_ANYCHAN
 if (!sts && cp->anyev[EVENT_NOTIFY].evstate != ANYEV_NOTANY) {
	sts = anychan_newstate(&cp->anyev[EVENT_NOTIFY], ANYEV_IDLE);
 }
#endif
 if (pp->enable) {
	if (!sts) sts = startdet(cp, HANDLE_DET, DETTYPE_ONHOOK, 0);
	if (!sts) {
		struct ting_message msg;
		msg.ioctl_length = sizeof(msg);
		msg.flags = 0;
		msg.msg[0] = MESSAGE(MSG_ONHOOK_CONFIG, 8);
		msg.msg[1] = onhook_dBm0convert((pp->pre_pulse_max_power!=0.0)?pp->pre_pulse_max_power:-48.0);
		msg.msg[2] = onhook_dBm0convert((pp->pulse_min_power!=0.0)?pp->pulse_min_power:-18.0);
		msg.msg[3] = onhook_dBm0convert((pp->post_pulse_floor!=0.0)?pp->post_pulse_floor:-42.0);
		msg.msg[4] = (uint32_t) floor(0.5 + 65536.0*pow(10.0, ((pp->latitude!=0.0)?pp->latitude:-6.0)/10.0));
		msg.msg[5] = (uint32_t) (((pp->count_ratio!=0.0)?pp->count_ratio:4.0)*65536.0);
		msg.msg[6] = (uint32_t) (((pp->total_ratio!=0.0)?pp->total_ratio:4.0)*65536.0);
		msg.msg[7] = (pp->max_duration!=0)?pp->max_duration:60;
		if (ERR(cx_ioctl(&cp->chan[HANDLE_DET].cx, IOCTL_TiNG_WRMSG,
		&msg, sizeof(struct ting_message))))
			sts = err(ERR_SM_DEVERR);
	}
 }
#ifndef OMIT_ANYCHAN
 if (!sts && cp->anyev[EVENT_NOTIFY].evstate == ANYEV_IDLE) {
	sts = anychan_newstate(&cp->anyev[EVENT_NOTIFY], ANYEV_WAIT);
 }
#endif
 unlock_object(cp);
#ifndef OMIT_ANYCHAN
 if (lockany) unlock_object(&anychan);
#endif
 return sts;
}
