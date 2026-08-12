/* TiNGsmtone.i - tone generation */

#include "TiNGsmtone.h"
#include <stdlib.h>
#include <string.h>
#include "../hsif/tonegen.h"
#include "../hsif/kernel.h"
#include "../hsif/subtask.h"
#include "../hsif/typecode.h"
#include "TiNGanyevent.h"
#include "TiNGo_mod.h"
#include "TiNGcommon.h"
#include "trace.h"

#define MAX_CP_DUR (0x7fffffff/8)
#define ABS(x) ((x)<0?-(x):(x))

STATIC char *cp_int(char *bp, long i)
{
 *bp++ = i & 0xff;
 i >>= 8;
 *bp++ = i & 0xff;
 i >>= 8;
 *bp++ = i & 0xff;
 i >>= 8;
 *bp++ = i & 0xff;
 return bp;
}

STATIC char *cp_dur(char *bp, long i, int modulate)
{
 return cp_int(bp, i*(modulate? -8 : 8));
}

STATIC char *cp_tone(char *bp, struct module *mod, unsigned tnum, double rate)
{
 struct gentone gp;
 if (tnum >= mod->ngentone) {
	if (TiNGtrace > 2) olog("bad tone: %d >= %d\n", tnum, mod->ngentone);
	return 0;
 }
 mkgentone(&gp, mod->gentone[tnum].freq, mod->gentone[tnum].ampl, rate);
 bp = cp_int(bp, gp.freq);
 if (bp) bp = cp_int(bp, gp.ampl);
 return bp;
}

STATIC char *cp_pair(char *bp, struct module *mod, unsigned pnum, double rate)
{
 if (pnum >= mod->ngenpair) {
	if (TiNGtrace > 2) olog("bad tone pair: %d >= %d\n", pnum, mod->ngenpair);
	return 0;
 }
 bp = cp_tone(bp, mod, mod->genpair[pnum].a, rate);
 if (bp) bp = cp_tone(bp, mod, mod->genpair[pnum].b, rate);
 return bp;
}

LOCALDEF int real_sm_add_output_freq(struct sm_output_freq_parms *pp)
{
 struct module *mp = modid2lockedmodule(pp->module);
 struct fgentone *ng;
 int sts = 0;
 if (!mp) return err(ERR_SM_NO_SUCH_MODULE);
 ng = alloc(mp->gentone, sizeof(*mp->gentone) * (mp->ngentone+1));
 if (!ng) sts = err(ERR_SM_NO_RESOURCES);
 else {
	mp->gentone = ng;
	if (mkfgentone(&mp->gentone[mp->ngentone], pp->freq, pp->amplitude)) {
		sts = err(ERR_SM_BAD_PARAMETER);
	} else pp->id = mp->ngentone++;
 }
 unlock_module(mp);
 return sts;
}

LOCALDEF int real_sm_add_output_tone(struct sm_output_tone_parms *pp)
{
 struct module *mp = modid2lockedmodule(pp->module);
 int sts = 0;
 if (!mp) return err(ERR_SM_NO_SUCH_MODULE);
 if ((unsigned) pp->component1_id >= mp->ngentone
	|| (unsigned) pp->component2_id >= mp->ngentone) {
	sts = err(ERR_SM_BAD_PARAMETER);
 } else {
	struct genpair *ng;
	ng = alloc(mp->genpair, sizeof(*mp->genpair) * (mp->ngenpair+1));
	if (!ng) sts = err(ERR_SM_NO_RESOURCES);
	else {
		 mp->genpair = ng;
		 mp->genpair[mp->ngenpair].a = pp->component1_id;
		 mp->genpair[mp->ngenpair].b = pp->component2_id;
		 pp->id = mp->ngenpair++;
	}
 }
 unlock_module(mp);
 return sts;
}

STATIC void dtor_tones(tSMChannelId cp)
{
 	// in case we've aborted - free the buffer
 free(cp->ut.besp.tone.tonebuf);
 cp->ut.besp.tone.started_task = TGEN_NONE;
 cp->ut.besp.tone.tonebuf = 0;
 cp->ut.besp.rxtx.length = 0;
 cp->rxtx[0].func = RXTXF_NONE;
 cp->rxtx[0].dtor = 0;
}
//needs lock : anychan
STATIC int cleanup_play_tones(tSMChannelId cp, int waited)
{
 dtor_tones(cp);
#ifndef OMIT_ANYCHAN
 if (!waited && cp->anyev[EVENT_WRITE].evstate != ANYEV_NOTANY) {
	int sts = anychan_newstate(&cp->anyev[EVENT_WRITE], ANYEV_IDLE);
	if (sts) return sts;
 }
#endif
 if (cp->event[EVENT_WRITE].nonidle) {
	int sts = disengage_event(cp, EVENT_WRITE, HANDLE_WRITE);
 	if (sts) return sts;
 }
 return 0;
}

STATIC int updatecptone(tSMChannelId cp, int nowait)
{
 CARDCONN *cx = &cp->chan[HANDLE_WRITE].cx;
 unsigned long cap;
 unsigned long mincap;
 struct ting_message msg;
 unsigned long code;
 int i;
 msg.flags = 0;
 if ((cp->ut.besp.tone.tonebuf && cp->rxtx[0].gotready) || nowait) {
		// we prefer to wait in the write, if possible
	msg.flags |= IOCTL_TiNG_RDMSG_FLAG_NOWAIT;
 }
 i = rdmsg(&msg, cx);
 if (i) return i;
 code = msg.msg[0] & ~0xffff;
 cap = msg.capacity / 8;
 if (cp->rxtx[0].minimum_bits < 0) {
	mincap = 5 * 4; // tone item length
 } else {
	mincap = cp->rxtx[0].minimum_bits / 8;
 }
 if (cap < mincap) cap = 0;
 if (code == MESSAGE(MSG_TONEGEN_MARK, 1)) {
	cp->ut.besp.tone.started_task = TGEN_NONE;
	return starttask(cx, 0, 0);
 } else if ((code & 0xfff00000) == MESSAGE(MSG_DEAD, 0)) {
	cp->rxtx[0].state = RXTXS_DONE;
	if ((code & 0xffff0000) == MESSAGE(MSG_DEAD, 1)) {
		return 0;
	}
	switch (msg.msg[1]) {
	case MSG_DEAD_CAUSE_BADFUNC:
		if (cp->ut.besp.tone.started_task == TGEN_TASK) {
			int sts;
			cp->ut.besp.tone.started_task = TGEN_SUBTASK;
			sts = startout(cp, TASKTC_OUTCHAN);
			if (!sts) cp->rxtx[0].gotready = 0;
			if (!sts) sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_TONEGEN);
			if (!sts) sts = sendmsg2(cx, MSG_ADDHSUB, SUBTASKTC_HBUFFROMHOST);
			if (!sts) sts = chan_setxfer(cp, 0);
			cp->rxtx[0].state = RXTXS_RUN;
			cp->ut.besp.rxtx.length = 0;
			return sts;
		} else if (cp->ut.besp.tone.started_task == TGEN_NONE) {
			// must have been aborted, so ignore the error
			return 0;
		}
		return err(ERR_SM_NO_SUCH_FIRMWARE);
	case MSG_DEAD_CAUSE_NOMEM:
		return err(ERR_SM_NO_RESOURCES);
	case MSG_DEAD_CAUSE_PARAM:
		return err(ERR_SM_BAD_PARAMETER);
	case MSG_DEAD_CAUSE_DISCO:
		return err(ERR_SM_DISCONNECTED);
	}
	return 0; // other causes are not errors
 } else if ((code & 0xffff0000) == MESSAGE(MSG_READY, 4)) {
	cp->rxtx[0].gotready = 1;
 } else if ((code & 0xffff0000) == MESSAGE(MSG_BADMSG, 3)) {
	if ((msg.msg[1] & 0xffff0000) == MESSAGE(MSG_ADDSUB, 2) || 
		(!cp->rxtx[0].gotready && (msg.msg[1] & 0xffff0000) == MESSAGE(MSG_ADDHSUB, 2))) {
		// subtask failed so kill task
		starttask(&cp->chan[HANDLE_WRITE].cx, 0, 0);
		cp->ut.besp.tone.started_task = TGEN_NONE;
		switch (msg.msg[2]) {
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
	}
	return 0;
 }
 if (!cp->ut.besp.tone.tonebuf) return 0;
 for (;;) {
	unsigned max = cp->ut.besp.rxtx.maxlength;
	unsigned nc = max - cp->ut.besp.rxtx.length;
	unsigned wlen;
	ERRCODE e;
	if (nowait && nc > cap) nc = cap;
	if (!nc) return 0;
	if (nowait && cap < mincap) return 0;
		// send it
	e = cx_write(cx, cp->ut.besp.tone.tonebuf + cp->ut.besp.rxtx.length,
		nc, &wlen);
	if (ERR(e))  {
#ifdef EAGAIN
			// should never happen as we checked capacity
		if (e.errnum == EAGAIN) return err(ERR_SM_NO_CAPACITY);
		else 
#endif
			return err(ERR_SM_DEVERR);
	}
	if (!wlen) return 0;
	cap -= wlen;
	cp->ut.besp.rxtx.length += wlen;
	if (cp->ut.besp.rxtx.length >= cp->ut.besp.rxtx.maxlength) {
		if (cp->ut.besp.tone.repeat) {
			cp->ut.besp.rxtx.length = 0;
		} else {
			struct ting_xfer xfer;
			free(cp->ut.besp.tone.tonebuf);
			cp->ut.besp.tone.tonebuf = 0;
			xfer.ioctl_length = sizeof(xfer);
			xfer.minimum_bits = 1<<24;	// very large number
			xfer.timeout_milliseconds = 0;
			xfer.nomore = 1;
			if (ERR(cx_ioctl(cx, IOCTL_TiNG_WRXFER, &xfer, sizeof(xfer)))) {
				return err(ERR_SM_DEVERR);
			}
			return 0;
		}
	}
 }
}

//needs lock : anychan
STATIC int starttone(tSMChannelId cp, int func, char *ep, int wait)
{
 int sts, sts2;
 if (!cp->ut.besp.tone.tonebuf) return err(ERR_SM_NO_RESOURCES);
 if (!ep) return err(ERR_SM_BAD_PARAMETER);
 cp->ut.besp.rxtx.maxlength = ep - cp->ut.besp.tone.tonebuf;
 cp->ut.besp.rxtx.length = 0;
 cp->ut.besp.tone.started_task = TGEN_TASK;
 sts = startout(cp, TASKTC_TONEGEN);
 if (sts) return sts;
 sts = chan_setxfer(cp, 0);
 if (sts) return sts;
 cp->rxtx[0].gotready = 0;
 cp->rxtx[0].func = func;
 cp->rxtx[0].dtor = dtor_tones;
 cp->rxtx[0].state = RXTXS_RUN;
#ifndef OMIT_ANYCHAN
 if (!wait && cp->anyev[EVENT_WRITE].evstate == ANYEV_IDLE) {
	sts = anychan_newstate(&cp->anyev[EVENT_WRITE], ANYEV_WAIT);
	if (sts) return sts;
 }
#endif
 if (cp->event[EVENT_WRITE].nonidle) {
 	sts = engage_event(cp, EVENT_WRITE, HANDLE_WRITE);
	if (sts) return sts;
 }
 if (wait) {
	while (cp->rxtx[0].state != RXTXS_DONE) {
		sts2 = updatecptone(cp, 0);
		if (!sts) sts = sts2;
	}
	sts2 = cleanup_play_tones(cp, 1);
	if (!sts) sts = sts2;
 }
 return sts;
}

// must pass cadences separately because sm_play_digits needs more
// than the standard size
//holds lock : anychan
STATIC int internal_sm_play_cptone(struct sm_play_cptone_parms *pp, struct sm_cadence *smc, int func)
{
 const tSMChannelId cp = pp->channel;
 unsigned long dur = pp->duration;
 const unsigned itemlen = 5 * 4;
 struct module *mod;
 char *ep = 0;
 int sts = 0;
 CARDCONN *cx;
#ifndef OMIT_ANYCHAN
 const int lockany = anychan.inuse & (1 << EVENT_WRITE) && !pp->wait_for_completion;
 if (lockany) lock_object(&anychan);
#endif
 lock_module(cp->mod);
 lock_object(cp);
 cx = &cp->chan[HANDLE_WRITE].cx;
 if (cp->rxtx[0].func != RXTXF_NONE) {
	 if (cp->rxtx[0].func == RXTXF_INVALID) {
		sts = err(ERR_SM_WRONG_CHANNEL_TYPE);
	 } else {
		sts = err(ERR_SM_WRONG_CHANNEL_STATE);
	}
 }
 mod = cp->mod;
 if (!sts) switch (pp->type) {
 case kSMPlayCPToneTypeRepeat:
	cp->ut.besp.tone.repeat = !dur;
		// fall in ...
 case kSMPlayCPToneTypeOneShot: {
	int i;
 		// duration dictates that we want N whole cadences
 		// plus a residue left over. compose buffer containing:
 		// data for whole cadence followed by data for residue
	if (pp->type == kSMPlayCPToneTypeOneShot) {
		dur = 0;
		cp->ut.besp.tone.repeat = 0;
	}
 	if (dur) {
		unsigned long ttime = 0;
		unsigned u;
		for (i=0; i<pp->tone_count; i++) {
			ttime += ABS(smc[i].on_cadence) + ABS(smc[i].off_cadence);
		}
		u = dur / ttime + 1;	// number of iterations of cadences
		u *= pp->tone_count;	// number of tones per iteration
		u *= 2;			// allow for on and off
		cp->ut.besp.rxtx.maxlength = u * itemlen;
		cp->ut.besp.tone.repeat = 0;
 	} else {
		cp->ut.besp.rxtx.maxlength = pp->tone_count * 2 * itemlen;
	}
	if (!cp->ut.besp.tone.repeat) cp->ut.besp.rxtx.maxlength += itemlen;
	ep = cp->ut.besp.tone.tonebuf = alloc(0, cp->ut.besp.rxtx.maxlength);
	for (i=0; ; i++) {
		unsigned tc;
		if (i >= pp->tone_count) {
			if (!dur) break;
			i = 0;
		}
		tc = ABS(smc[i].on_cadence);
		if (tc) {
			if (tc > MAX_CP_DUR) {
				sts = err(ERR_SM_BAD_PARAMETER);
				break;
			}
			if (dur && tc > dur) tc = dur;
			if (ep) ep = cp_pair(ep, mod, smc[i].tone_id, cp->out_sample_rate);
			if (ep) ep = cp_dur(ep, tc, smc[i].on_cadence<0);
			if (dur) {
				dur -= tc;
				if (!dur) break;
			}
		}
		tc = ABS(smc[i].off_cadence);
		if (tc) {
			if (tc > MAX_CP_DUR) {
				sts = err(ERR_SM_BAD_PARAMETER);
				break;
			}
			if (dur && tc > dur) tc = dur;
			if (ep) ep = cp_tone(ep, mod, 0, cp->out_sample_rate);
			if (ep) ep = cp_tone(ep, mod, 0, cp->out_sample_rate);
			if (ep) ep = cp_dur(ep, tc, smc[i].off_cadence<0);
			if (dur) {
				dur -= tc;
				if (!dur) break;
			}
		}
	}
 }
	break;
 case kSMPlayCPToneTypeContinuous:
	 if (dur > MAX_CP_DUR) {
		sts = err(ERR_SM_BAD_PARAMETER);
		break;
	}
	cp->ut.besp.tone.repeat = !dur;
	cp->ut.besp.rxtx.maxlength = itemlen + itemlen;
	ep = cp->ut.besp.tone.tonebuf = alloc(0, cp->ut.besp.rxtx.maxlength);
	if (ep) ep = cp_pair(ep, cp->mod, smc[0].tone_id, cp->out_sample_rate);
	if (ep) ep = cp_dur(ep, dur ? dur : MAX_CP_DUR, 0);
	break;
 default:
	sts = err(ERR_SM_BAD_PARAMETER);
 }
 unlock_module(cp->mod);
 if (!cp->ut.besp.tone.repeat && ep) {
	memset(ep, 0, itemlen);
	ep += itemlen;
 }
 if (!sts) sts = starttone(cp, func, ep, pp->wait_for_completion);
 unlock_object(cp);
#ifndef OMIT_ANYCHAN
 if (lockany) unlock_object(&anychan);
#endif
 return sts;
}

LOCALDEF int real_sm_play_tone(struct sm_play_tone_parms *pp)
{
 struct sm_play_cptone_parms pcp;
 pcp.channel = pp->channel;
 pcp.duration = pp->duration;
 pcp.wait_for_completion = pp->wait_for_completion;
 pcp.type = kSMPlayCPToneTypeContinuous;
 pcp.tone_count = 1;
 pcp.cadences[0].tone_id = pp->tone_id;
 pcp.cadences[0].on_cadence = pp->duration;
 pcp.cadences[0].off_cadence = 0;
 return internal_sm_play_cptone(&pcp, pcp.cadences, RXTXF_GENSTONE);
}

//needs lock : anychan
LOCALDEF int unsafe_sm_play_tone_status(struct sm_play_tone_status_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 int sts;
 if (cp->rxtx[0].func != RXTXF_GENSTONE) return err(ERR_SM_WRONG_CHANNEL_STATE);
 sts = updatecptone(cp, 1);
 if (sts) return sts;
 if (cp->rxtx[0].state == RXTXS_DONE) {
	pp->status = kSMPlayToneStatusComplete;
	return cleanup_play_tones(cp, 0);
 }
 pp->status = kSMPlayToneStatusOngoing;
 return 0;
}
//holds lock : anychan
LOCALDEF int real_sm_play_tone_status(struct sm_play_tone_status_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 int sts = 0;
 if (!cp) {
#ifndef OMIT_ANYCHAN
	return sm_play_tone_anystatus(pp);
#else
	return err(ERR_SM_NOT_IMPLEMENTED);
#endif
 }
 {
#ifndef OMIT_ANYCHAN
 const int lockany = anychan.inuse & (1 << EVENT_WRITE);
 if (lockany) lock_object(&anychan);
#endif
 lock_object(cp);
 sts = unsafe_sm_play_tone_status(pp);
 unlock_object(cp);
#ifndef OMIT_ANYCHAN
 if (lockany) unlock_object(&anychan);
#endif
 }
 return sts;
}

LOCALDEF int real_sm_play_tone_abort(tSMChannelId cp)
{
 int sts;
 lock_object(cp);
 if (cp->rxtx[0].func != RXTXF_GENSTONE) sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 else {
	sts = starttask(&cp->chan[HANDLE_WRITE].cx, 0, 0);
	cp->ut.besp.tone.started_task = TGEN_NONE;
	cp->rxtx[0].state = RXTXS_LAST;
 }
 unlock_object(cp);
 return sts;
}

LOCALDEF int real_sm_play_cptone_abort(tSMChannelId cp)
{
 int sts;
 lock_object(cp);
 if (cp->rxtx[0].func != RXTXF_GENCPTONE) sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 else {
	sts = starttask(&cp->chan[HANDLE_WRITE].cx, 0, 0);
	cp->ut.besp.tone.started_task = TGEN_NONE;
	cp->rxtx[0].state = RXTXS_LAST;
 }
 unlock_object(cp);
 return sts;
}

LOCALDEF int real_sm_play_cptone(struct sm_play_cptone_parms *pp)
{
 return internal_sm_play_cptone(pp, pp->cadences, RXTXF_GENCPTONE);
}

//needs lock : anychan
LOCALDEF int unsafe_sm_play_cptone_status(struct sm_play_cptone_status_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 int sts;
 if (cp->rxtx[0].func != RXTXF_GENCPTONE) {
	return err(ERR_SM_WRONG_CHANNEL_STATE);
 }
 sts = updatecptone(cp, 1);
 if (sts) return sts;
 if (cp->rxtx[0].state == RXTXS_DONE) {
	pp->status = kSMPlayCPToneStatusComplete;
	return cleanup_play_tones(cp, 0);
 }
 pp->status = kSMPlayCPToneStatusOngoing;
 return 0;
}

//holds lock : anychan
LOCALDEF int real_sm_play_cptone_status(struct sm_play_cptone_status_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 int sts = 0;
 if (!cp) {
#ifndef OMIT_ANYCHAN
	return sm_play_cptone_anystatus(pp);
#else
	return err(ERR_SM_NOT_IMPLEMENTED);
#endif
 }
 {
#ifndef OMIT_ANYCHAN
 const int lockany = anychan.inuse & (1 << EVENT_WRITE);
 if (lockany) lock_object(&anychan);
#endif
 lock_object(cp);
 sts = unsafe_sm_play_cptone_status(pp);
 unlock_object(cp);
#ifndef OMIT_ANYCHAN
 if (lockany) unlock_object(&anychan);
#endif
 }
 return sts;
}

LOCALDEF int real_sm_play_digits(struct sm_play_digits_parms *pp)
{
 unsigned idd = pp->digits.inter_digit_delay;
 unsigned dur = pp->digits.digit_duration;
 struct sm_cadence smc[kSMMaxDigits];
 struct sm_play_cptone_parms pcp;
 unsigned u;
 pcp.channel = pp->channel;
 pcp.wait_for_completion = pp->wait_for_completion;
 pcp.type = kSMPlayCPToneTypeOneShot;
 pcp.tone_count = 0;
 if (!dur) dur = 100;
 if (!idd) idd = 100;
 if (pp->digits.type != kSMDTMFDigits) return err(ERR_SM_BAD_PARAMETER);
 if (pp->digits.qualifier) return err(ERR_SM_BAD_PARAMETER);
 for (u=0; ; u++) {
	char c = pp->digits.digit_string[u];
	if (!c) break;
	if (c >= '0' && c <= '9') c -= '0';
	else if (c >= 'A' && c <= 'D') c -= 'A' - 10;
	else if (c >= 'a' && c <= 'd') c -= 'a' - 10;
	else if (c == '*') c = 14;
	else if (c == '#') c = 15;
	else {
		return err(ERR_SM_BAD_PARAMETER);
	}
	smc[u].tone_id = c;
	smc[u].on_cadence = dur;
	smc[u].off_cadence = idd;
 }
 pcp.tone_count = u;
 pcp.duration = (dur + idd) * u;
 return internal_sm_play_cptone(&pcp, smc, RXTXF_GENDTMF);
}

//needs lock : anychan
LOCALDEF int unsafe_sm_play_digits_status(struct sm_play_digits_status_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 int sts;
 if (cp->rxtx[0].func != RXTXF_GENDTMF) return err(ERR_SM_WRONG_CHANNEL_STATE);
 sts = updatecptone(cp, 1);
 if (sts) return sts;
 if (cp->rxtx[0].state == RXTXS_DONE) {
	pp->status = kSMPlayDigitsStatusComplete;
	return cleanup_play_tones(cp, 0);
 }
 pp->status = kSMPlayDigitsStatusOngoing;
 return 0;
}

//holds lock : anychan
LOCALDEF int real_sm_play_digits_status(struct sm_play_digits_status_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 int sts = 0;
 if (!cp) {
#ifndef OMIT_ANYCHAN
	return sm_play_digits_anystatus(pp);
#else
	return err(ERR_SM_NOT_IMPLEMENTED);
#endif
 }
 {
#ifndef OMIT_ANYCHAN
 const int lockany = anychan.inuse & (1 << EVENT_WRITE);
 if (lockany) lock_object(&anychan);
#endif
 lock_object(cp);
 sts = unsafe_sm_play_digits_status(pp);
 unlock_object(cp);
#ifndef OMIT_ANYCHAN
 if (lockany) unlock_object(&anychan);
#endif
 }
 return sts;
}

STATIC int fill_tonelist_buffers(tSMChannelId cp, char *tonebuf, unsigned length)
{
 int sts = 0;
 char *tp = tonebuf;
 CARDCONN *cx = &cp->chan[HANDLE_WRITE].cx;
 unsigned bytes = cp->rxtx[0].dataready >> 3;
 if (bytes) {
	unsigned wlen;
	if (cp->ut.besp.tone.tonebuf) {
		unsigned bufleft = cp->ut.besp.rxtx.maxlength - cp->ut.besp.rxtx.length;
		if (ERR(cx_write(cx, cp->ut.besp.tone.tonebuf + cp->ut.besp.rxtx.length,
					 bytes > bufleft ? bufleft : bytes, &wlen)))
			return err(ERR_SM_DEVERR);
		bytes -= wlen;
		bufleft -= wlen;
		cp->ut.besp.rxtx.length += wlen;
		if (!bufleft) {
			free(cp->ut.besp.tone.tonebuf);
			cp->ut.besp.tone.tonebuf = 0;
			cp->ut.besp.rxtx.maxlength = 0;
			cp->ut.besp.rxtx.length = 0;
		}
	}
	if (tonebuf) {
		if (ERR(cx_write(cx, tonebuf,
					 bytes > length ? length : bytes, &wlen)))
			return err(ERR_SM_DEVERR);
		bytes -= wlen;
		length -= wlen;
		tp += wlen;
		if (!length) {
			free(tonebuf);
			tonebuf = 0;
			tp = 0;
		}
	}
	cp->rxtx[0].dataready = bytes * 8;
 }
 if (tp) {
	if (cp->ut.besp.tone.tonebuf) {
		unsigned bufleft = cp->ut.besp.rxtx.maxlength - cp->ut.besp.rxtx.length;
		char *newbuf = alloc(0, bufleft + length);
		if (!newbuf) {
			sts = err(ERR_SM_OS_RESOURCE_PROBLEM);
		} else {
			memcpy(newbuf, cp->ut.besp.tone.tonebuf, bufleft);
			memcpy(newbuf + bufleft, tp, length);
			free(cp->ut.besp.tone.tonebuf);
			cp->ut.besp.tone.tonebuf = newbuf;
			cp->ut.besp.rxtx.maxlength = bufleft + length;
			cp->ut.besp.rxtx.length = 0;
		}
		free(tonebuf);
	} else {
		cp->ut.besp.tone.tonebuf = tonebuf;
		cp->ut.besp.rxtx.maxlength = length;
		cp->ut.besp.rxtx.length = 0;
	}
 }
 if (cp->ut.besp.rxtx.laststop && !tonebuf && !cp->ut.besp.tone.tonebuf) {
	struct ting_xfer xfer;
	xfer.ioctl_length = sizeof(xfer);
	xfer.minimum_bits = 1<<24;	// very large number
	xfer.timeout_milliseconds = 0;
	xfer.nomore = 1;
	if (ERR(cx_ioctl(cx, IOCTL_TiNG_WRXFER, &xfer, sizeof(xfer)))) {
		return err(ERR_SM_DEVERR);
	}
	cp->ut.besp.rxtx.laststop = 0;
 }
 return sts;
}

//holds lock : anychan
LOCALDEF int real_sm_play_tone_list(struct sm_play_tone_list_parms *pp)
{
 tSMChannelId cp = pp->channel;
 const unsigned itemlen = 5 * 4;
 char *ep = 0, *tonebuf = 0;
 int sts = 0;
 int last = 0;
 CARDCONN *cx;
 lock_object(cp);
 cx = &cp->chan[HANDLE_WRITE].cx;
 if (cp->rxtx[0].func != RXTXF_NONE && cp->rxtx[0].func != RXTXF_GENTONELIST) {
	if (cp->rxtx[0].func == RXTXF_INVALID) {
		sts = err(ERR_SM_WRONG_CHANNEL_TYPE);
	} else {
		sts = err(ERR_SM_WRONG_CHANNEL_STATE);
	}
 }
 if (!sts && pp->tone_count) {
	int i;
	ep = tonebuf = alloc(0, pp->tone_count * itemlen);
	if (!tonebuf) {
		sts = err(ERR_SM_OS_RESOURCE_PROBLEM);
	} else {
		for (i=0; i<pp->tone_count; i++) {
			switch (pp->tones[i].operation) {
			case kSMToneOperationStop:
				if (ep) {
					memset(ep, 0, itemlen);
					ep += itemlen;
				}
				last = 1;
				break;
			case kSMToneOperationSum:
			case kSMToneOperationModulate: {
				struct gentone gp1, gp2;
				if (!pp->tones[i].duration) {
					sts = err(ERR_SM_BAD_PARAMETER);
				}
				if (mkgentone(&gp1, pp->tones[i].frequency1, pp->tones[i].amplitude1, cp->out_sample_rate)) {
					if (pp->tones[i].frequency1) {
						sts = err(ERR_SM_BAD_PARAMETER);
					}
				}
				if (mkgentone(&gp2, pp->tones[i].frequency2, pp->tones[i].amplitude2, cp->out_sample_rate)) {
					if (pp->tones[i].frequency2) {
						sts = err(ERR_SM_BAD_PARAMETER);
					}
				}
				if (!sts) {
					if (ep) ep = cp_int(ep, gp1.freq);
					if (ep) ep = cp_int(ep, gp1.ampl);
					if (ep) ep = cp_int(ep, gp2.freq);
					if (ep) ep = cp_int(ep, gp2.ampl);
					if (pp->tones[i].operation == kSMToneOperationModulate) {
						if (ep) ep = cp_int(ep, 0u - pp->tones[i].duration * 8);
					} else {
						if (ep) ep = cp_int(ep, pp->tones[i].duration * 8);
					}
				}
			}
				break;
			default:
				sts = err(ERR_SM_BAD_PARAMETER);
			}
		}
	}
 }
 if (!sts && cp->rxtx[0].func != RXTXF_GENTONELIST) {
	sts = startout(cp, TASKTC_TONEGEN);
	if (!sts) sts = chan_setxfer(cp, 0);
	if (!sts) {
		cp->ut.besp.tone.started_task = TGEN_TASK;
		cp->rxtx[0].func = RXTXF_GENTONELIST;
		cp->rxtx[0].dtor = dtor_tones;
		cp->rxtx[0].state = last ? RXTXS_LAST : RXTXS_RUN;
		cp->rxtx[0].dataready = 0;
		cp->ut.besp.tone.tonebuf = 0;
		cp->ut.besp.tone.phase_reversal_period = 0;
		cp->ut.besp.rxtx.maxlength = 0;
		cp->ut.besp.rxtx.length = 0;
		cp->ut.besp.rxtx.laststop = last;
		if (cp->event[EVENT_WRITE].nonidle) {
 			sts = engage_event(cp, EVENT_WRITE, HANDLE_WRITE);
		}
	}
 }
 if (!sts) sts = fill_tonelist_buffers(cp, tonebuf, ep-tonebuf);
 else if (tonebuf) free(tonebuf);

 unlock_object(cp);
 return sts;
}

STATIC int checkmsgs_tonelist(tSMChannelId chan)
{
 CARDCONN *cx = &chan->chan[HANDLE_WRITE].cx;
 for (;;) {
	struct ting_message msg;
	int i;
	if (chan->rxtx[0].state == RXTXS_DONE) return 0;
	msg.flags = IOCTL_TiNG_RDMSG_FLAG_NOWAIT;
	i = rdmsg(&msg, cx);
	if (i) return i;
	chan->rxtx[0].dataready = msg.capacity;
	if (! (msg.msg[0] & 0x80000000) ) return 0;
	if ((msg.msg[0] & 0xfff00000) == MESSAGE(MSG_DEAD, 0)) {
		unsigned islast = chan->rxtx[0].state == RXTXS_LAST;
		chan->rxtx[0].state = RXTXS_DONE;
		if ((msg.msg[0] & 0xf0000) > 0x10000) {	// has cause code
			switch (msg.msg[1]) {
			case MSG_DEAD_CAUSE_NOMORE:
				break;  // normal
			case MSG_DEAD_CAUSE_MSG:
				break;	// normal
			case MSG_DEAD_CAUSE_BADFUNC:
				if (chan->ut.besp.tone.started_task == TGEN_TASK) {
					int sts;
					chan->ut.besp.tone.started_task = TGEN_SUBTASK;
					sts = startout(chan, TASKTC_OUTCHAN);
					if (!sts) chan->rxtx[0].gotready = 0;
					if (!sts) sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_TONEGEN);
					if (!sts) sts = sendmsg2(cx, MSG_ADDHSUB, SUBTASKTC_HBUFFROMHOST);
					if (!sts) sts = chan_setxfer(chan, 0);
					if (!sts && chan->ut.besp.tone.phase_reversal_period) {
						sts = sendmsg2(cx, MSG_TONE_REVERSE, chan->ut.besp.tone.phase_reversal_period);
					}
					chan->rxtx[0].state = islast ? RXTXS_LAST: RXTXS_RUN;
					chan->ut.besp.rxtx.length = 0;
					return sts;
				} else if (chan->ut.besp.tone.started_task == TGEN_NONE) {
					// must have been aborted, so ignore the error
					return 0;
				}
				return err(ERR_SM_NO_SUCH_FIRMWARE);
			case MSG_DEAD_CAUSE_NOMEM:
				return err(ERR_SM_NO_RESOURCES);
			case MSG_DEAD_CAUSE_DISCO:
				return err(ERR_SM_DISCONNECTED);
			default:
				return err(ERR_SM_FIRMWARE_PROBLEM);
			}
		}
	} else if ((msg.msg[0] & 0xffff0000) == MESSAGE(MSG_READY, 4)) {
		chan->rxtx[0].gotready = 1;
	} else if ((msg.msg[0] & 0xffff0000) == MESSAGE(MSG_BADMSG, 3)) {
		if ((msg.msg[1] & 0xffff0000) == MESSAGE(MSG_ADDSUB, 2) || 
			(!chan->rxtx[0].gotready && (msg.msg[1] & 0xffff0000) == MESSAGE(MSG_ADDHSUB, 2))) {
			// subtask failed so kill task
			starttask(&chan->chan[HANDLE_WRITE].cx, 0, 0);
			chan->ut.besp.tone.started_task = TGEN_NONE;
			switch (msg.msg[2]) {
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
		}
		return 0;
	} else if ((msg.msg[0] & 0xffff0000) == MESSAGE(MSG_UNDERRUN, 1)) {
		chan->rxtx[0].underrun = 1;
	}
 }
}

//holds lock : anychan
LOCALDEF int real_sm_play_tone_list_status(struct sm_play_tone_list_status_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 int sts = 0;
 if (!cp) {
	return err(ERR_SM_NOT_IMPLEMENTED);
 }
 lock_object(cp);
 if (cp->rxtx[0].func != RXTXF_GENTONELIST) sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 if (!sts) sts = checkmsgs_tonelist(cp);
 if (!sts) sts = fill_tonelist_buffers(cp, NULL, 0);
 if (!sts) {
	if (cp->rxtx[0].underrun) {
		pp->status = kSMPlayToneListStatusUnderrun;
		cp->rxtx[0].underrun = 0;
	} else if (cp->rxtx[0].state == RXTXS_DONE) {
		pp->status = kSMPlayToneListStatusComplete;
		dtor_tones(cp);
		if (cp->event[EVENT_WRITE].nonidle) {
			sts = disengage_event(cp, EVENT_WRITE, HANDLE_WRITE);
 			if (sts) return sts;
		}
	} else if (cp->rxtx[0].dataready && cp->rxtx[0].state == RXTXS_RUN) {
		pp->status = kSMPlayToneListStatusHasCapacity;
	} else
		pp->status = kSMPlayToneListStatusOngoing;
 }
 unlock_object(cp);
 return sts;
}

LOCALDEF int real_sm_play_tone_list_abort(tSMChannelId cp)
{
 int sts;
 lock_object(cp);
 if (cp->rxtx[0].func != RXTXF_GENTONELIST) sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 else {
	sts = starttask(&cp->chan[HANDLE_WRITE].cx, 0, 0);
	cp->ut.besp.tone.started_task = TGEN_NONE;
	cp->rxtx[0].state = RXTXS_LAST;
 }
 unlock_object(cp);
 return sts;
}

LOCALDEF int real_sm_play_tone_list_phase_reverse(struct sm_play_tone_list_phase_reverse_parms *pp)
{
 int sts;
 lock_object(pp->channel);
 if (pp->channel->rxtx[0].func != RXTXF_GENTONELIST) {
	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else {
	pp->channel->ut.besp.tone.phase_reversal_period = pp->period;
	sts = sendmsg2(&pp->channel->chan[HANDLE_WRITE].cx, MSG_TONE_REVERSE, pp->period);
 }
 unlock_object(pp->channel);
 return sts;
}
