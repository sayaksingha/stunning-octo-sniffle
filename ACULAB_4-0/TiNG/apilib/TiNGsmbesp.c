/* TiNGsmbesp.c - BESP library */

#include "TiNGsmbesp.h"
#include "../hsif/6k.h"
#include "../hsif/catsig.h"
#include "../hsif/echocan.h"
#include "../hsif/echogen.h"
#include "../hsif/ectest.h"
#include "../hsif/grunt.h"
#include "../hsif/kernel.h"
#include "../hsif/msgstring.h"
#include "../hsif/play.h"
#include "../hsif/redirect.h"
#include "../hsif/samplerate.h"
#include "../hsif/speed.h"
#include "../hsif/speexrp.h"
#include "../hsif/subtask.h"
#include "../hsif/timerx.h"
#include "../hsif/tonedet.h"
#include "../hsif/typecode.h"
#include "smbesp.h"
#include "TiNGanyevent.h"
#include <string.h>
#include <math.h>
#include "TiNGo_mod.h"
#include "TiNGcommon.h"
#include "TiNGdetect.h"
#include "TiNGsmtone.h"
#include "trace.h"

STATIC int sendmsg5(CARDCONN *cx, U16 message, U32 p1, U32 p2, U32 p3, U32 p4 )
{
 struct ting_message msg;
 msg.ioctl_length = sizeof(msg);
 msg.flags = 0;
 msg.msg[0] = MESSAGE(message, 5);
 msg.msg[1] = p1;
 msg.msg[2] = p2;
 msg.msg[3] = p3;
 msg.msg[4] = p4;
 if (ERR(cx_ioctl(cx, IOCTL_TiNG_WRMSG, &msg, sizeof(struct ting_message)))) {
	return err(ERR_SM_DEVERR);
 }
 return 0;
}

STATIC int sendmsg6(CARDCONN *cx, U16 message, U32 p1, U32 p2, U32 p3, U32 p4, U32 p5)
{
 struct ting_message msg;
 msg.ioctl_length = sizeof(msg);
 msg.flags = 0;
 msg.msg[0] = MESSAGE(message, 6);
 msg.msg[1] = p1;
 msg.msg[2] = p2;
 msg.msg[3] = p3;
 msg.msg[4] = p4;
 msg.msg[5] = p5;
 if (ERR(cx_ioctl(cx, IOCTL_TiNG_WRMSG, &msg, sizeof(struct ting_message)))) {
	return err(ERR_SM_DEVERR);
 }
 return 0;
}

STATIC unsigned gcd(unsigned x, unsigned y)
{
 unsigned p2 = 0;
 if (!x) return y;
 if (!y) return x;
 while (! ((x | y) & 1) ) {	// common factors of two
	x >>= 1;
	y >>= 1;
	p2++;
 }
 	// at least one of x, y is odd
 while (! (x & 1) ) x >>= 1;
 do {		// x is odd
	while (! (y & 1)) y >>= 1;
	if (x > y) {
		int oy = y;
		y = x;
		x = oy;
	}
	y = (y - x) >> 1;
 } while (y);
 return x << p2;
}

STATIC void rxtxstart(tSMChannelId chan, int dir)
{
 struct rxtx *const rxtxp = dir ? &chan->ur.besp.rxtx : &chan->ut.besp.rxtx;
 rxtxp->maxlength = 0;
 rxtxp->capacity = 0;
 rxtxp->length = 0;
 chan->rxtx[dir].underrun = 0;
}

STATIC void rxtxtransfer(tSMChannelId chan, unsigned bytes, int dir)
{
 struct rxtx *const rxtxp = dir ? &chan->ur.besp.rxtx : &chan->ut.besp.rxtx;
 rxtxp->capacity -= bytes*8;
 rxtxp->length += bytes;
}

STATIC int handlerxtxmsgs(SM_RECOGNISED_PARMS *rp, tSMChannelId chan, int dir, struct ting_message *msg, CARDCONN *cx)
{
 struct rxtx *const rxtxp = dir ? &chan->ur.besp.rxtx : &chan->ut.besp.rxtx;
 rxtxp->capacity = msg->capacity;
 // NB while capacity is unsigned, the buffers are smaller than 2 gigabits!
 if ((int)rxtxp->capacity < chan->rxtx[dir].minimum_bits
 	&& (!msg->hasmsg || !dir)) {
		// For positive thresholds, ignore capacity if less than threshold unless
		// we're forced to read data because a message follows.
		// There is no minimum capacity when using negative thresholds.
	rxtxp->capacity = 0;
 }
 if (msg->msg[0] & 0x80000000) {
	unsigned long code = msg->msg[0] & 0xffff0000;
	if (rp) {
		int done;
		int sts = handle_detected(&done, rp, code, msg, 1);
		if (sts || done) return sts;
	}
	switch (code) {
	case MESSAGE(MSG_READY, 4):
		chan->rxtx[dir].gotready = 1;
		break;
	case MESSAGE(MSG_TIMEOUT, 1):
			// got max elapsed time for terminating a record
		if (dir == 1 && chan->ur.besp.reason == RECEND_RUNNING) {
			int sts;
			chan->ur.besp.reason = RECEND_MAXTIME;
			chan->rxtx[1].state = RXTXS_LAST;
			sts = starttask(cx, 0, 0);
			if (sts) return sts;
		}
		break;
	case MESSAGE(MSG_PLAY_STOP, 2): {
		int sts;
		rxtxp->laststop = msg->msg[1] / 8;	// convert to bytes
		sts = starttask(cx, 0, 0);
		chan->rxtx[dir].state = RXTXS_LAST;
		if (sts) return sts;
	}
		break;
	case MESSAGE(MSG_SAMPLERATE, 2):
		if (dir == 1) chan->ur.besp.sample_rate = msg->msg[1];
		break;
	case MESSAGE(MSG_GRUNT_CONT, 2):
			// got silence for terminating a record
		if (dir == 1 && chan->ur.besp.reason == RECEND_RUNNING) {
			int sts;
			chan->ur.besp.reason = RECEND_SILENCE;
			rxtxp->laststop = msg->msg[1];
			chan->rxtx[1].state = RXTXS_LAST;
			sts = starttask(cx, 0, 0);
			if (sts) return sts;
		}
		break;
	case MESSAGE(MSG_OVERRUN,1):
		if (dir == 1 && chan->ur.besp.reason == RECEND_ABORT) {
			// message queue overrun while aborting, make sure we get a MSG_PLAY_STOP
			int sts = sendmsg1(cx, MSG_PLAY_STOP);
			if (sts) return sts;
		}
		break;
	case MESSAGE(MSG_DEAD, 4):
	case MESSAGE(MSG_DEAD, 3):
	case MESSAGE(MSG_DEAD, 2):
			/* These leave the operation running, so the user
			 * is forced to check the status again, when the
			 * synthesised DEAD message will indicate that
			 * the operation is finished.
			 */
		switch (msg->msg[1]) {
		case MSG_DEAD_CAUSE_BADFUNC:
			return err(ERR_SM_NO_SUCH_FIRMWARE);
		case MSG_DEAD_CAUSE_NOMEM:
			return err(ERR_SM_NO_RESOURCES);
		case MSG_DEAD_CAUSE_PARAM:
			return err(ERR_SM_BAD_PARAMETER);
		case MSG_DEAD_CAUSE_DISCO:
			return err(ERR_SM_DISCONNECTED);
		}
			// fall in ...
	case MESSAGE(MSG_DEAD, 1):
		chan->rxtx[dir].state = RXTXS_DONE;
		chan->rxtx[dir].gotready = 0;
		chan->mod->card->vtbl->datafeed_dtor(chan->rxtx[dir].auxdf);
		chan->rxtx[dir].auxdf = NULL_DATAFEED;
		if (dir == 1
			&& chan->ur.besp.reason == RECEND_RUNNING) {
				// if we didn't expect termination it must be
				// because of an error
			chan->ur.besp.reason = RECEND_ERROR;
		}
		break;
	case MESSAGE(MSG_BADMSG, 3): {
		const unsigned mcode = msg->msg[1] & 0xffff0000;
		const int iscreate = mcode == MESSAGE(MSG_CREATE, 3);
		const int isaddsub = mcode == MESSAGE(MSG_ADDSUB, 2);
		const int isaddhsub = mcode == MESSAGE(MSG_ADDHSUB, 2);
		if (msg->msg[2] == MSG_BADMSG_CAUSE_REDUNDANT) return 0;
		if (iscreate || isaddsub || isaddhsub) {
			switch (msg->msg[2]) {
			case MSG_BADMSG_CAUSE_NOTASK:
				return err(ERR_SM_NO_SUCH_CHANNEL);
			case MSG_BADMSG_CAUSE_UNHANDLED:
				if (isaddhsub && chan->rxtx[dir].gotready) {
					return 0;
				}
				return err(ERR_SM_WRONG_FIRMWARE_TYPE);
			case MSG_BADMSG_CAUSE_RANGE:
				if (isaddhsub && chan->rxtx[dir].gotready) {
					return 0;
				}
				return err(ERR_SM_NO_SUCH_FIRMWARE);
			case MSG_BADMSG_CAUSE_MEMORY:
				return err(ERR_SM_NO_RESOURCES);
			}
			return err(ERR_SM_DEVERR);
		}
		break;
	}
	case MESSAGE(MSG_UNDERRUN, 1):
		chan->rxtx[dir].underrun = 1;
		break;
	default:
		break;
	}
 }
 return 0;
}

STATIC int checkrxtxmsgs(SM_RECOGNISED_PARMS *rp, tSMChannelId chan, CARDCONN *cx, int dir)
{
 struct ting_message msg;
 int sts;
 msg.flags = IOCTL_TiNG_RDMSG_FLAG_NOWAIT;
 sts = rdmsg(&msg, cx);
 if (sts) return sts;
 return handlerxtxmsgs(rp, chan, dir, &msg, cx);
}

LOCALDEF int real_sm_replay_adjust(struct sm_replay_adjust_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 const tSMChannelId bgc = pp->background;
 unsigned speedcode;
 CARDCONN *cx;
 int sts = 0;
 if (bgc) lock_module(cp->mod);	// locking hierarchy rule
 lock_object(cp);
 cx = &cp->chan[HANDLE_WRITE].cx;
 if (cp->rxtx[0].func != RXTXF_PLAYREC) sts = err(ERR_SM_NO_REPLAY_IN_PROGRESS);
 if (bgc || pp->volume || pp->agc) {
	if (!cp->ut.besp.gainbg) {
		if (!sts) sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_GAINTX);
		if (!sts) cp->ut.besp.gainbg = 1;
	}
	if (!sts) sts = sendmsg2(cx, MSG_VOLUME, pp->volume);
	if (!sts) sts = sendmsg2(cx, MSG_AGC, pp->agc);
	if (!sts) {
		if (bgc) {
			if (bgc == cp) {
				sts = err(ERR_SM_BAD_PARAMETER);
			} else {
				lock_object(bgc);
				sts = cp->mod->card->vtbl->connect_df(&cp->rxtx[0].auxdf, bgc->df[0]);
				if (!sts) sts = cp->mod->card->vtbl->engage_df(bgc->df[0], cx, DFC_BG);
				unlock_object(bgc);
			}
		} else {
			sts = cp->mod->card->vtbl->connect_df(&cp->rxtx[0].auxdf, NULL_DATAFEED);
			if (!sts) sts = cp->mod->card->vtbl->engage_df(NULL_DATAFEED, cx, DFC_BG);
		}
	}
 } else if (cp->ut.besp.gainbg) {
	if (!sts) sts = sendmsg2(cx, MSG_DELSUB, SUBTASKTC_GAINTX);
	if (!sts) cp->ut.besp.gainbg = 0;
 }
 if ( !(speedcode = pp->speed) ) speedcode = 100;
 if (cp->ut.besp.speed != speedcode) { // change speed
	if (cp->ut.besp.speed < 100 && speedcode >= 100) {
		if (!sts) sts = sendmsg2(cx, MSG_DELSUB, SUBTASKTC_SLOW);
		if (!sts) cp->ut.besp.speed = 100;
	} else if (cp->ut.besp.speed > 100 && speedcode <= 100) {
		if (!sts) sts = sendmsg2(cx, MSG_DELSUB, SUBTASKTC_FAST);
		if (!sts) cp->ut.besp.speed = 100;
	}
	if (speedcode != 100) {
		if (cp->ut.besp.speed == 100) {
			U32 mid;
			if (speedcode > 100) {
				mid = SUBTASKTC_FAST;
			} else {
				mid = SUBTASKTC_SLOW;
			}
			if (!sts) sts = sendmsg2(cx, MSG_ADDSUB, mid);
		}
		if (!sts) sts = sendmsg2(cx, MSG_SPEED, speedcode);
	}
	if (!sts) cp->ut.besp.speed = speedcode;
 }
 unlock_object(cp);
 if (bgc) unlock_module(cp->mod);
 return sts;
}

//holds lock : anychan
LOCALDEF int real_sm_replay_start(struct sm_replay_parms *pp)
{
 struct sm_replay_adjust_parms adj;
 const tSMChannelId cp = pp->channel;
 enum kSMDataFormat type = kSMDataFormatNone;
 int task_started = 0;
 unsigned rate = 8000;
 U32 endsub = 0;
 CARDCONN *cx;
 int sts = 0;
#ifndef OMIT_ANYCHAN
 const int lockany = anychan.inuse & (1 << EVENT_WRITE);
#endif
 	// convert compatibility codes
 switch (pp->type) {
 case kSMDataFormat8KHzSigned8bitMono:
	type = kSMDataFormatSigned8bit;
	break;
 case kSMDataFormat8KHz8bitMono:
	type = kSMDataFormat8bit;
	break;
 case kSMDataFormat8KHz16bitMono:
	type = kSMDataFormat16bit;
	break;
 case kSMDataFormat8KHzALawPCM:
	type = kSMDataFormatALawPCM;
	break;
 case kSMDataFormat6KHzALawPCM:
	type = kSMDataFormatALawPCM;
 	rate = 6000;
	break;
 case kSMDataFormat8KHzULawPCM:
	type = kSMDataFormatULawPCM;
	break;
 case kSMDataFormat6KHzULawPCM:
	type = kSMDataFormatULawPCM;
 	rate = 6000;
	break;
 case kSMDataFormat8KHzOKIADPCM:
	type = kSMDataFormatOKIADPCM;
	break;
 case kSMDataFormat6KHzOKIADPCM:
	type = kSMDataFormatOKIADPCM;
 	rate = 6000;
	break;
 case kSMDataFormat8KHzACUBLKPCM:
	type = kSMDataFormatACUBLKPCM;
	break;
 case kSMDataFormat6KHzACUBLKPCM:
	type = kSMDataFormatACUBLKPCM;
 	rate = 6000;
	break;
 default:
 	break;
 }
 if (type == kSMDataFormatNone) {
	type = pp->type;
	rate = pp->sampling_rate;
 } else if (pp->sampling_rate) return err(ERR_SM_BAD_PARAMETER);
 switch (type) {
 case kSMDataFormatNone:
	break;
 case kSMDataFormat16bit:
	endsub = SUBTASKTC_PLAY16BIT;
	break;
 case kSMDataFormatSigned8bit:
	endsub = SUBTASKTC_PLAY8BIT;
	break;
 case kSMDataFormat8bit:
	endsub = SUBTASKTC_PLAYMS8BIT;
	break;
 case kSMDataFormatALawPCM:
	endsub = SUBTASKTC_PLAYALAW;
	break;
 case kSMDataFormatULawPCM:
	endsub = SUBTASKTC_PLAYMULAW;
	break;
 case kSMDataFormatOKIADPCM:
	endsub = SUBTASKTC_PLAYOKI;
	break;
 case kSMDataFormatACUBLKPCM:
	endsub = SUBTASKTC_PLAYACUBLK;
	break;
 case kSMDataFormatIMAADPCM:
	if (!rate) rate = 8000;
	endsub = SUBTASKTC_PLAYIMA;
	break;
 case kSMDataFormatSpeex:
	if (!rate) rate = 8000;
	endsub = SUBTASKTC_PLAYSPEEX;
	break;
 default:
	if (TiNGtrace > 1) olog("(no replay type %d)", pp->type);
	return err(ERR_SM_NOT_IMPLEMENTED);
 }
#ifndef OMIT_ANYCHAN
 if (lockany) lock_object(&anychan);
#endif
 lock_object(cp);
 if (cp->rxtx[0].func != RXTXF_NONE) {
	 if (cp->rxtx[0].func == RXTXF_INVALID)
		 sts = err(ERR_SM_WRONG_CHANNEL_TYPE);
	 else
		 sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 }
 if (!sts) {
	sts = startout(cp, TASKTC_OUTCHAN);
	if (!sts) task_started = 1;
 }
 cx = &cp->chan[HANDLE_WRITE].cx;
 if (!sts) rxtxstart(cp, 0);
 cp->rxtx[0].gotready = 0;
 if (!sts && endsub) sts = sendmsg2(cx, MSG_ADDSUB, endsub);
 if (!sts && endsub) {
	if (nonnull_datafeed(cp->redirect_tx)) {
		if (!sts) sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_REDIRECT_TX_LO);
		if (!sts) sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_REDIRECT_TX_HI);
		if (!sts) sts = sendmsg3(cx, MSG_REDIRECT_TX_CONFIG, cp->redirect_tx_threshold, cp->redirect_tx_timeout);
		if (!sts) sts = cp->mod->card->vtbl->engage_df(cp->redirect_tx, cx, DFC_REDIRECT_TX);
	} else 	if (cp->collector) {
		tSMGroove *grv = &cp->collector->groove;
		CARDCONN *cocx = grv->cardconn_cx(grv);
		sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_FROMUDP);
		if (!sts) sts = setfriend_cx(cx, cocx, FRIEND_TYPE_COLL);
	} else { 	
 		sts = sendmsg2(cx, MSG_ADDHSUB, SUBTASKTC_HBUFFROMHOST);
 	}
 }
 if (!sts) {
	unsigned g = gcd(rate, cp->out_sample_rate);
	unsigned down = rate / g;
	unsigned up = cp->out_sample_rate / g;
	if (down == 1 && up == 1) {
		// do nothing
	} else if (down == 3 && up == 4) {	// special
		sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_6_TO_8);
		if (!sts) sts = sendmsg2(cx, MSG_6K, 1);
	} else if (1 <= down && down <= 6 && 1 <= up && up <= 6) {
			// FIXME: add resampler
		sts = err(ERR_SM_BAD_PARAMETER);
	} else if ((up == 8 && down == 11) || (up == 320 && down == 441)) {
		// allow 11000 or 11025 on 8000, or 22000 on 16K, etc
		sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_11_TO_8);
	} else {
		sts = err(ERR_SM_BAD_PARAMETER);
	}
 }
 adj.channel = cp;
 adj.background = pp->background;
 adj.volume = pp->volume;
 adj.agc = pp->agc;
 adj.speed = pp->speed;
 if (!sts) {
	cp->ut.besp.rxtx.maxlength = pp->data_length;
	cp->ut.besp.rxtx.laststop = ~0U;
	cp->ut.besp.speed = 100;
	cp->ut.besp.gainbg = 0;
	cp->rxtx[0].func = RXTXF_PLAYREC;
	cp->rxtx[0].state = RXTXS_RUN;
	sts = chan_setxfer(cp, 0);
 }
#ifndef OMIT_ANYCHAN
 if (!sts && cp->anyev[EVENT_WRITE].evstate == ANYEV_IDLE) {
	sts = anychan_newstate(&cp->anyev[EVENT_WRITE], ANYEV_WAIT);
 }
#endif
 if (!sts && cp->event[EVENT_WRITE].nonidle) {
 	sts = engage_event(cp, EVENT_WRITE, HANDLE_WRITE);
 }
 if (sts && task_started) {
	int sts2 = starttask(&cp->chan[HANDLE_WRITE].cx, 0, 0);
	for (;;) {
		struct ting_message msg;
		msg.flags = 0;
		sts2 = rdmsg(&msg, &cp->chan[HANDLE_WRITE].cx);
		if (sts2) break;
		if ((msg.msg[0] & 0xffff0000) == MESSAGE(MSG_DEAD, 1)) break;
	}
	cp->rxtx[0].func = RXTXF_NONE;
	cp->rxtx[0].state = RXTXS_DONE;
 }
 unlock_object(cp);
#ifndef OMIT_ANYCHAN
 if (lockany) unlock_object(&anychan);
#endif
 if (sts) return sts;
 return real_sm_replay_adjust(&adj);
}

STATIC int unsafe_sm_put_replay_data(struct sm_ts_data_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 struct rxtx *const rxtxp = &cp->ut.besp.rxtx;
 unsigned rqlen = pp->length;
 char *data = pp->data;
 CARDCONN *cx = &cp->chan[HANDLE_WRITE].cx;
 if (cp->rxtx[0].func != RXTXF_PLAYREC) return err(ERR_SM_NO_REPLAY_IN_PROGRESS);
 if (rxtxp->maxlength && rxtxp->maxlength - rxtxp->length < rqlen) {
	rqlen = rxtxp->maxlength - rxtxp->length;
 }
 if (rqlen) {
	unsigned wlen;
	ERRCODE e;
	switch (cp->rxtx[0].state) {
	case RXTXS_RUN:
		break;
	case RXTXS_LAST:
		return err(ERR_SM_BAD_DATA_LENGTH);
	case RXTXS_DONE:
		return err(ERR_SM_NO_REPLAY_IN_PROGRESS);
	}
	for ( ;rqlen ;) {
		e = cx_write(cx, data, rqlen, &wlen);
		if (ERR(e))  {
			return err(ERR_SM_DEVERR);
		}
		rxtxtransfer(cp, wlen, 0);
		if (!wlen) return err(ERR_SM_NO_CAPACITY);
		rqlen -= wlen;
		data += wlen;
	}
 }
 if (rxtxp->maxlength && rxtxp->length >= rxtxp->maxlength) {
 	if (ERR(cx_nomore(cx))) return err(ERR_SM_DEVERR);
	cp->rxtx[0].state = RXTXS_LAST;
 }
 return 0;
}

LOCALDEF int real_sm_put_replay_data(struct sm_ts_data_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 int sts;
 lock_object(cp);
 sts = unsafe_sm_put_replay_data(pp);
 unlock_object(cp);
 return sts;
}

LOCALDEF int real_sm_put_last_replay_data(struct sm_ts_data_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 struct rxtx *const rxtxp = &cp->ut.besp.rxtx;
 int sts;
 lock_object(cp);
 rxtxp->maxlength = rxtxp->length + pp->length;
 if (!rxtxp->maxlength) rxtxp->maxlength = rxtxp->length = 1;
 sts = unsafe_sm_put_replay_data(pp);
 unlock_object(cp);
 return sts;
}



STATIC int unsafe_sm_put_audio_data(struct sm_audio_data_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 struct rxtx *const rxtxp = &cp->ut.besp.rxtx;
 unsigned rqlen = pp->max_length;
 char *data = pp->data;
 CARDCONN *cx = &cp->chan[HANDLE_WRITE].cx;
 pp->done_length = 0;
 if (cp->rxtx[0].func != RXTXF_PLAYREC) return err(ERR_SM_NO_REPLAY_IN_PROGRESS);
 if (rxtxp->maxlength && rxtxp->maxlength - rxtxp->length < rqlen) {
	rqlen = rxtxp->maxlength - rxtxp->length;
 }
 if (rqlen) {
	unsigned wlen;
	ERRCODE e;
	switch (cp->rxtx[0].state) {
	case RXTXS_RUN:
		break;
	case RXTXS_LAST:
		return err(ERR_SM_BAD_DATA_LENGTH);
	case RXTXS_DONE:
		return err(ERR_SM_NO_REPLAY_IN_PROGRESS);
	}
	for ( ;rqlen ;) {
		e = cx_write(cx, data, rqlen, &wlen);
		if (ERR(e))  {
			return err(ERR_SM_DEVERR);
		}
		rxtxtransfer(cp, wlen, 0);
		if (!wlen) break;
		rqlen -= wlen;
		data += wlen;
		pp->done_length += wlen;
	}
 }
 if (rxtxp->maxlength && rxtxp->length >= rxtxp->maxlength) {
 	if (ERR(cx_nomore(cx))) return err(ERR_SM_DEVERR);
	cp->rxtx[0].state = RXTXS_LAST;
 }

 return 0;
}

LOCALDEF int real_sm_put_audio_data(struct sm_audio_data_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 int sts;
 lock_object(cp);
 sts = unsafe_sm_put_audio_data(pp);
 unlock_object(cp);
 return sts;
}






//needs lock : anychan
LOCALDEF int unsafe_sm_replay_status(struct sm_replay_status_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 struct rxtx *const rxtxp = &cp->ut.besp.rxtx;
 int sts = checkrxtxmsgs(0, cp, &cp->chan[HANDLE_WRITE].cx, 0);
 if (sts) return sts;
 if (cp->rxtx[0].state == RXTXS_DONE) {
	cp->rxtx[0].func = RXTXF_NONE;
	pp->status = kSMReplayStatusComplete;
#ifndef OMIT_ANYCHAN
	if (cp->anyev[EVENT_WRITE].evstate != ANYEV_NOTANY) {
		sts = anychan_newstate(&cp->anyev[EVENT_WRITE], ANYEV_IDLE);
 	}
#endif
	if (!sts && cp->event[EVENT_WRITE].nonidle) {
		sts = disengage_event(cp, EVENT_WRITE, HANDLE_WRITE);
	}
	pp->offset = rxtxp->length;
	if (rxtxp->laststop != ~0U) {
		pp->offset -= rxtxp->laststop;
	}
 } else if (cp->rxtx[0].underrun) {
	cp->rxtx[0].underrun = 0;
	pp->status = kSMReplayStatusUnderrun;
 } else if (cp->rxtx[0].state == RXTXS_LAST) {
	pp->status = kSMReplayStatusCompleteData;
 } else if (rxtxp->capacity) {
	pp->status = kSMReplayStatusHasCapacity;
 } else pp->status = kSMReplayStatusNoCapacity;
 return 0;
}

//hold lock : anychan
LOCALDEF int real_sm_replay_status(struct sm_replay_status_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 if (!cp) {
#ifndef OMIT_ANYCHAN
	if (! (anychan.inuse & (1 << EVENT_WRITE)) ) {
		return err(ERR_SM_WRONG_CHANNEL_STATE);
	}
	return sm_replay_anystatus(pp);
#else
	return err(ERR_SM_NOT_IMPLEMENTED);
#endif
 } else {
	int sts;
		// just in case this channel completes and uses anychan
#ifndef OMIT_ANYCHAN
	const int lockany = anychan.inuse & (1 << EVENT_WRITE);
	if (lockany) lock_object(&anychan);
#endif
	lock_object(cp);
	if (cp->rxtx[0].func != RXTXF_PLAYREC) {
		sts = err(ERR_SM_NO_REPLAY_IN_PROGRESS);
	} else sts = unsafe_sm_replay_status(pp);
	unlock_object(cp);
#ifndef OMIT_ANYCHAN
	if (lockany) unlock_object(&anychan);
#endif
	return sts;
 }
}

LOCALDEF int real_sm_replay_abort(struct sm_replay_abort_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 struct rxtx *const rxtxp = &cp->ut.besp.rxtx;
 int sts = 0;
 CARDCONN *cx;
 lock_object(cp);
 cx = &cp->chan[HANDLE_WRITE].cx;
 if (cp->rxtx[0].func != RXTXF_PLAYREC) sts = err(ERR_SM_NO_REPLAY_IN_PROGRESS);
 if (cp->rxtx[0].state == RXTXS_RUN) {
 	if (ERR(cx_nomore(cx))) sts = err(ERR_SM_DEVERR);
	cp->rxtx[0].state = RXTXS_LAST;
 }
 if (!sts) sts = sendmsg1(cx, MSG_PLAY_STOP);
 if (!sts && !pp->nowait) for (;;) {
	struct ting_message msg;
	msg.ioctl_length = sizeof(msg);
	msg.flags = 0;
	sts = rdmsg(&msg, cx);
	if (sts) break;
	if (msg.msg[0] == 0) break;
	sts = handlerxtxmsgs(0, cp, 0, &msg, cx);
	if (sts) {
		cp->rxtx[0].state = RXTXS_DONE;
		break;
	}
	if (rxtxp->laststop != ~0U) break;
	if (cp->rxtx[0].state == RXTXS_DONE) break;
 }
 pp->offset = rxtxp->length;
 if (!sts && rxtxp->laststop != ~0U) {
	pp->offset -= rxtxp->laststop;
 }
 unlock_object(cp);
 return sts;
}

//holds lock : anychan
LOCALDEF int real_sm_record_start(struct sm_record_parms *pp)
{
 enum kSMDataFormat type = kSMDataFormatNone;
 const tSMChannelId cp = pp->channel;
 enum handles h = HANDLE_READ;
 int task_started = 0;
 unsigned rate = 0;
 CARDCONN *cx;
 U32 endsub = 0;
 int sts = 0;
#ifndef OMIT_ANYCHAN
 const int lockany = anychan.inuse & (1 << EVENT_READ);
#endif
 	// convert compatibility codes
 switch (pp->type) {
 case kSMDataFormat8KHzSigned8bitMono:
	type = kSMDataFormatSigned8bit;
 	rate = 8000;
	break;
 case kSMDataFormat8KHz8bitMono:
	type = kSMDataFormat8bit;
 	rate = 8000;
	break;
 case kSMDataFormat8KHz16bitMono:
	type = kSMDataFormat16bit;
 	rate = 8000;
	break;
 case kSMDataFormat8KHzALawPCM:
	type = kSMDataFormatALawPCM;
 	rate = 8000;
	break;
 case kSMDataFormat6KHzALawPCM:
	type = kSMDataFormatALawPCM;
 	rate = 6000;
	break;
 case kSMDataFormat8KHzULawPCM:
	type = kSMDataFormatULawPCM;
 	rate = 8000;
	break;
 case kSMDataFormat6KHzULawPCM:
	type = kSMDataFormatULawPCM;
 	rate = 6000;
	break;
 case kSMDataFormat8KHzOKIADPCM:
	type = kSMDataFormatOKIADPCM;
 	rate = 8000;
	break;
 case kSMDataFormat6KHzOKIADPCM:
	type = kSMDataFormatOKIADPCM;
 	rate = 6000;
	break;
 case kSMDataFormat8KHzACUBLKPCM:
	type = kSMDataFormatACUBLKPCM;
 	rate = 8000;
	break;
 case kSMDataFormat6KHzACUBLKPCM:
	type = kSMDataFormatACUBLKPCM;
 	rate = 6000;
	break;
 default:
 	break;
 }
 if (type == kSMDataFormatNone) {
	type = pp->type;
	rate = pp->sampling_rate;
 } else if (pp->sampling_rate && pp->sampling_rate != rate) return err(ERR_SM_BAD_PARAMETER);
 switch (type) {
 case kSMDataFormatNone:
	break;
 case kSMDataFormatSigned8bit:
	endsub = SUBTASKTC_REC8BIT;
	break;
 case kSMDataFormat8bit:
	endsub = SUBTASKTC_RECMS8BIT;
	break;
 case kSMDataFormat16bit:
	endsub = SUBTASKTC_REC16BIT;
	break;
 case kSMDataFormatULawPCM:
	endsub = SUBTASKTC_RECMULAW;
 	break;
 case kSMDataFormatALawPCM:
	endsub = SUBTASKTC_RECALAW;
 	break;
 case kSMDataFormatOKIADPCM:
	endsub = SUBTASKTC_RECOKI;
 	break;
 case kSMDataFormatACUBLKPCM:
	endsub = SUBTASKTC_RECACUBLK;
 	break;
 case kSMDataFormatIMAADPCM:
	if (!rate) rate = 8000;
	endsub = SUBTASKTC_RECIMA;
	break;
 case kSMDataFormatSpeex:
	if (!rate) rate = 8000;
	endsub = SUBTASKTC_RECSPEEX;
	break;
 default:
	if (TiNGtrace > 1) olog("(no record type %d)", pp->type);
	return err(ERR_SM_NOT_IMPLEMENTED);
 }
#ifndef OMIT_ANYCHAN
 if (lockany) lock_object(&anychan);
#endif
 if (pp->alt_data_source) lock_module(cp->mod);	// locking hierarchy rule
 lock_object(cp);
 cx = &cp->chan[h].cx;
 if (cp->rxtx[1].func != RXTXF_NONE) {
	 if (cp->rxtx[1].func == RXTXF_INVALID)
		 sts = err(ERR_SM_WRONG_CHANNEL_TYPE);
	 else
		 sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 }
 rxtxstart(cp, 1);
 if (!sts) {
	sts = startin(cp, TASKTC_INCHAN);
	if (!sts) task_started = 1;
 }
 if (!sts && pp->tone_elimination_mode != kSMToneDetectionNone) {
	SM_LISTEN_FOR_PARMS lfp;
	memset(&lfp, 0, sizeof(lfp));
	lfp.channel = cp;
	lfp.tone_detection_mode = pp->tone_elimination_mode;
	lfp.active_tone_set_id = pp->tone_elimination_set_id;
	sts = start_listen(&lfp, h, 1);
 }
 if (!sts && pp->max_elapsed_time) {
	sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_RXTIMEOUT);
	if (!sts) sts = sendmsg2(cx, MSG_TIMEOUT, pp->max_elapsed_time);
 }
 cp->rxtx[1].gotready = 0;
 if (!sts && endsub) sts = sendmsg2(cx, MSG_ADDSUB, endsub);
 if (!sts && endsub) {
	if (nonnull_datafeed(cp->redirect_rx)) {
		if (!sts) sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_REDIRECT_RX);
		if (!sts) sts = cp->mod->card->vtbl->engage_df(cp->redirect_rx, cx, DFC_REDIRECT_RX);
	} else if (cp->dispatcher) {
		tSMGroove *grv = &cp->dispatcher->groove;
		CARDCONN *dicx = grv->cardconn_cx(grv);
		sts = sendmsg2(cx, MSG_ADDHSUB, SUBTASKTC_TOUDP);
		if (!sts) sts = setfriend_cx(cx, dicx, FRIEND_TYPE_DISP);
	} else {
		sts = sendmsg2(cx, MSG_ADDHSUB, SUBTASKTC_HBUFTOHOST);
	}
 }
 cp->ur.besp.sample_rate = 8000; // assume inchan will get 8k data
 cp->ur.besp.rate_factor = rate;
 if (!sts) switch (rate) {
 case 6000:
	sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_8_TO_6);
	if (!sts) sts = sendmsg2(cx, MSG_6K, 1);
	break;
 case 0:
	cp->ur.besp.rate_factor = 8000;
 case 8000:
 	break;
 case 11000:
 case 11025:		// be nice to people!
	sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_8_TO_11);
 	break;
 default:
 	sts = err(ERR_SM_BAD_PARAMETER);
 }
 if (pp->silence_elimination || pp->max_silence) {
	U32 slen = pp->silence_elimination;
	U32 tlen = pp->max_silence;
	U32 hoff = pp->grunt_holdoff;
	U32 maxi = pp->max_initial_silence;
	double mnl = pp->min_noise_level;
	double thresh = pp->grunt_threshold;
	if (!sts) sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_GRUNT);
	if (slen == 1) slen = 1000;	// backward compatibility
	if (tlen) {			// terminate on silence
		if (slen > tlen) slen = tlen;	// cannot eliminate - too long
		if (!slen) slen = tlen;		// no suppression desired
	}
	if (!sts) {
		if (maxi) {
			sts = sendmsg5(cx, MSG_GRUNT_LATENCY, slen, tlen, hoff, maxi);
		} else if (hoff) { 
			sts = sendmsg4(cx, MSG_GRUNT_LATENCY, slen, tlen, hoff);
		} else if (!tlen) {
			sts = sendmsg2(cx, MSG_GRUNT_LATENCY, slen);
		} else {
			sts = sendmsg3(cx, MSG_GRUNT_LATENCY, slen, tlen);
		}
	}
	if (!pp->min_noise_level) {
		mnl = -55.0;
		thresh = 15.0;
	}
	mnl = exp((mnl + 90) / 10 * log(10)); // 10^9 adjusted by dB
	// conveniently this scale gives almost exactly the values need for the sharc firmware at 64 samples per epoch!
	thresh = 65536 * exp(thresh/10 * log(10)); // dB to factor scaled by 65536, NB power not amplitude
	if (!sts) sts = sendmsg3(cx, MSG_GRUNT_LEVELS, mnl, thresh);
 }
 if (!sts) cp->ur.besp.rxtx.maxlength = pp->max_octets;
 if (!sts) {
	const tSMChannelId alt = pp->alt_data_source;
 	DATAFEED src = cp->df[1];
	if (alt) {
		if (alt != cp) lock_object(alt);
		switch (pp->alt_data_source_type) {
		case kSMRecordAltSourceDefault:
			if (alt->type == kSMChannelTypeInput) {
				src = alt->df[1];
			} else {
				src = alt->df[0];
			}
			break;
		case kSMRecordAltSourceInput: src = alt->df[1]; break;
		case kSMRecordAltSourceOutput: src = alt->df[0]; break;
		default:
			sts = err(ERR_SM_BAD_PARAMETER);
		}
		if (!sts && !nonnull_datafeed(src)) sts = err(ERR_SM_BAD_PARAMETER);
	}
	if (!sts) sts = cp->mod->card->vtbl->connect_df(&cp->rxtx[1].auxdf, src);
	if (!sts) sts = cp->mod->card->vtbl->engage_df(src, cx, DFC_IN);
	if (alt && alt != cp) unlock_object(alt);
 }
 if (!sts) sts = chan_setxfer(cp, 1);
 if (pp->agc || pp->volume) {
	if (!sts) sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_GAINRX);
	if (!sts) sts = sendmsg2(cx, MSG_VOLUME, pp->volume);
	if (!sts) sts = sendmsg2(cx, MSG_AGC, pp->agc);
 }
 if (type == kSMDataFormatSpeex) {							// Speex must be configured before it will run
	if (!sts) sts = sendmsg6(cx, MSG_CONFIG_SPEEXREC,
	                         kSpeexNarrowbandMode,			// mode = narrowband - MUST NOT BE CHANGED WITH CURRENT FIRMWARE BUILD
	                         0,								// noise preprocessing = off - MUST NOT BE CHANGED WITH CURRENT FIRMWARE BUILD
	                         0,								// complexity = 0 - MUST NOT BE CHANGED WITH CURRENT FIRMWARE BUILD
	                         8,								// quality = 8 - MUST NOT BE CHANGED WITH CURRENT FIRMWARE BUILD
	                         8000);							// bitrate = 8000 - MUST NOT BE CHANGED WITH CURRENT FIRMWARE BUILD
 }
 if (!sts) {
	cp->rxtx[1].func = RXTXF_PLAYREC;
	cp->rxtx[1].state = RXTXS_RUN;
	cp->ur.besp.reason = RECEND_RUNNING;
 }
#ifndef OMIT_ANYCHAN
 if (!sts && cp->anyev[EVENT_READ].evstate == ANYEV_IDLE) {
	sts = anychan_newstate(&cp->anyev[EVENT_READ], ANYEV_WAIT);
 }
#endif
 if (!sts && cp->event[EVENT_READ].nonidle) {
 	sts = engage_event(cp, EVENT_READ, HANDLE_READ);
 }
 if (sts && task_started) {
	int sts2 = starttask(cx, 0, 0);
	for (;;) {
		struct ting_message msg;
		msg.flags = 0;
		sts2 = rdmsg(&msg, cx);
		if (sts2) break;
		if ((msg.msg[0] & 0xffff0000) == MESSAGE(MSG_DEAD, 1)) break;
	}
	cp->rxtx[1].func = RXTXF_NONE;
	cp->rxtx[1].state = RXTXS_DONE;
 }
 unlock_object(cp);
 if (pp->alt_data_source) unlock_module(cp->mod);	// locking hierarchy rule
#ifndef OMIT_ANYCHAN
 if (lockany) unlock_object(&anychan);
#endif
 return sts;
}

LOCALDEF int real_sm_get_recorded_data(struct sm_ts_data_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 enum handles h = HANDLE_READ;
 CARDCONN *cx;
 int sts = 0;
 if (!cp) {
	if (TiNGtrace > 1) olog("(no any-channel get data)");
	return err(ERR_SM_NOT_IMPLEMENTED);
 }
 lock_object(cp);
 cx = &cp->chan[h].cx;
 if (!sts && cp->rxtx[1].func != RXTXF_PLAYREC) {
	sts = err(ERR_SM_NO_RECORD_IN_PROGRESS);
 }
 if (!sts) {
	unsigned done;
	pp->length = 0; // default to nothing read
	if (cp->rxtx[1].state == RXTXS_RUN) {
		ERRCODE e = cx_read(cx, pp->data, kSMMaxRecordDataBufferSize, &done);
		if (ERR(e))  {
			sts = err(ERR_SM_DEVERR);
		} else if (done) {
			if (cp->ur.besp.rxtx.maxlength) {
				unsigned todo = cp->ur.besp.rxtx.maxlength;
				todo -= cp->ur.besp.rxtx.length;
				if (todo <= done) {
					done = todo;
					sts = starttask(cx, 0, 0);
					cp->ur.besp.reason = RECEND_LENGTH;
					cp->rxtx[1].state = RXTXS_LAST;
					if (ERR(cx_nomore(cx))) sts = err(ERR_SM_DEVERR);
				}
			}
			pp->length = done;
			rxtxtransfer(cp, done, 1);
		}
	}
 }
 unlock_object(cp);
 return sts;
}

//needs lock : anychan
LOCALDEF int unsafe_sm_record_status(struct sm_record_status_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 enum handles h = HANDLE_READ;
 SM_RECOGNISED_PARMS recp;
 struct rxtx *rxtxp;
 int sts = 0;
 rxtxp = &cp->ur.besp.rxtx;
 memset(&recp, 0, sizeof(recp));
 recp.channel = cp;
 sts = checkrxtxmsgs(&recp, cp, &cp->chan[h].cx, 1);
 if (sts) return sts;
 pp->termination_reason = kSMRecordHowTerminatedNotYet;
 if (recp.type != kSMRecognisedNothing) {
	pp->recog_type = recp.type;
	pp->param0 = recp.param0;
	pp->param1 = recp.param1;
	pp->status = kSMRecordStatusRecognition;
 } else if (cp->rxtx[1].state == RXTXS_DONE) {
	pp->status = kSMRecordStatusComplete;
	cp->recog[1].detmask = 0;
	cp->rxtx[1].func = RXTXF_NONE;
#ifndef OMIT_ANYCHAN
	if (cp->anyev[EVENT_READ].evstate != ANYEV_NOTANY) {
		sts = anychan_newstate(&cp->anyev[EVENT_READ], ANYEV_IDLE);
 	}
#endif
	if (!sts && cp->event[EVENT_READ].nonidle) {
		sts = disengage_event(cp, EVENT_READ, HANDLE_READ);
	}
	switch (cp->ur.besp.reason) {
	case RECEND_RUNNING:
		pp->termination_reason = kSMRecordHowTerminatedNotYet;
		break;
	case RECEND_ERROR:
		pp->termination_reason = kSMRecordHowTerminatedError;
		break;
	case RECEND_LENGTH:
		pp->termination_reason = kSMRecordHowTerminatedLength;
		break;
	case RECEND_ABORT:
		pp->termination_reason = kSMRecordHowTerminatedAborted;
		break;
	case RECEND_SILENCE:
		pp->termination_reason = kSMRecordHowTerminatedSilence;
		pp->termination_octets = cp->ur.besp.rxtx.laststop / 8;
		break;
	case RECEND_MAXTIME:
		pp->termination_reason = kSMRecordHowTerminatedMaxTime;
		break;
	}
 } else if (cp->rxtx[1].state == RXTXS_LAST) {
		// must have aborted with 'discard'
		// or reached rx limit
	while (rxtxp->capacity) {
		CARDCONN *cx = &cp->chan[h].cx;
		char buf[512];
		unsigned done;
		ERRCODE e = cx_read(cx, buf, sizeof(buf), &done);
		if (ERR(e))  {
			return err(ERR_SM_DEVERR);
			break;
		}
		if (!done) break; // task may have died - reducing capacity
		rxtxp->capacity -= done * 8;
	}
	pp->status = kSMRecordStatusNoData;
 } else if (cp->rxtx[1].underrun) {
	cp->rxtx[1].underrun = 0;
	pp->status = kSMRecordStatusOverrun;
 } else if (rxtxp->capacity) {
	pp->status = kSMRecordStatusData;
 } else pp->status = kSMRecordStatusNoData;
 pp->sample_rate = cp->ur.besp.sample_rate * cp->ur.besp.rate_factor / 8000;
 return 0;
}

//holds lock : anychan
LOCALDEF int real_sm_record_status(struct sm_record_status_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 if (!cp) {
#ifndef OMIT_ANYCHAN
	if (! (anychan.inuse & (1 << EVENT_READ)) ) {
		return err(ERR_SM_WRONG_CHANNEL_STATE);
	}
	return sm_record_anystatus(pp);
#else
	return err(ERR_SM_NOT_IMPLEMENTED);
#endif
 } else {
	int sts;
		// just in case this channel completes and uses anychan
#ifndef OMIT_ANYCHAN
	const int lockany = anychan.inuse & (1 << EVENT_READ);
	if (lockany) lock_object(&anychan);
#endif
	lock_object(cp);
	if (cp->rxtx[1].func != RXTXF_PLAYREC) {
		sts = err(ERR_SM_NO_RECORD_IN_PROGRESS);
	} else sts = unsafe_sm_record_status(pp);
	unlock_object(cp);
#ifndef OMIT_ANYCHAN
	if (lockany) unlock_object(&anychan);
#endif
	return sts;
 }
}

LOCALDEF int real_sm_record_abort(struct sm_record_abort_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 enum handles h = HANDLE_READ;
 int sts = 0;
 lock_object(cp);
 if (cp->rxtx[1].func != RXTXF_PLAYREC)
 	sts = err(ERR_SM_NO_RECORD_IN_PROGRESS);
 if (!sts) {
	CARDCONN *cx = &cp->chan[h].cx;
	if (ERR(cx_nomore(cx))) sts = err(ERR_SM_DEVERR);
	else {
		cp->ur.besp.reason = RECEND_ABORT;
		if (pp->discard) {
			sts = starttask(cx, 0, 0);
			cp->rxtx[1].state = RXTXS_LAST;
		} else {
			sts = sendmsg1(cx, MSG_PLAY_STOP);
		}
	}
 }
 unlock_object(cp);
 return sts;
}

LOCALDEF int real_sm_record_agc_adjust(struct sm_record_agc_adjust_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 enum handles h = HANDLE_READ;
 int sts = 0;
 lock_object(cp);
 if (cp->rxtx[1].func != RXTXF_PLAYREC)
 	sts = err(ERR_SM_NO_RECORD_IN_PROGRESS);
 if (!sts) sts = sendmsg2(&cp->chan[h].cx, MSG_AGC_SETGAIN, pp->gain);
 unlock_object(cp);
 return sts;
}

LOCALDEF int real_sm_record_agc_adjust_settings(struct sm_record_agc_adjust_settings_parms *cp)
{
 const tSMChannelId chanp = cp->channel;
 int sts;
 lock_object(chanp);
 if (chanp->rxtx[1].func != RXTXF_PLAYREC) {
	sts = err(ERR_SM_NO_RECORD_IN_PROGRESS);
 } else {
	U32 lmax_decay;
	U32 target;
	if (cp->max_level_decay > 1.0 || cp->max_level_decay < 0.0) sts = err(ERR_SM_BAD_PARAMETER);
	if (cp->target_level > 3.0 || cp->target_level < -90.0) sts = err(ERR_SM_BAD_PARAMETER);
	lmax_decay = 32768.0 * cp->max_level_decay;
	target = exp((cp->target_level + 90.0) / 10.0 * log(10));
	sts = sendmsg3(&chanp->chan[HANDLE_READ].cx, MSG_AGC_ADJ_SETTINGS, lmax_decay, target);
 }
 unlock_object(chanp);
 return sts;
}

STATIC int sendfilt(CARDCONN *cx, float *filt, unsigned flen)
{
 struct ting_message msg;
 unsigned pos = 0;
 msg.flags = 0;
 msg.ioctl_length = sizeof(msg);
 msg.msg[0] = MESSAGE(MSG_ECHOGEN_SETFILT, 8);
 msg.msg[1] = 0;	// offset to first coeff
 for (;; filt++, flen--) {
	if (!flen) {
		if (!pos) return 0;
		for (; pos < 6; pos++) msg.msg[pos+2] = 0;
	}
	if (pos == 6) {
		if (ERR(cx_ioctl(cx, IOCTL_TiNG_WRMSG,
			&msg, sizeof(struct ting_message))))
			return err(ERR_SM_DEVERR);
		if (!flen) return 0;
		pos = 0;
		msg.msg[1] += 6;
	}
	if (pos || *filt) {
		union {
			U32 u;
			float f;
		} u;
		u.f = *filt;
		msg.msg[pos++ + 2] = u.u;
	} else msg.msg[1]++;
 }
}

LOCALDEF int real_sm_condition_input(struct sm_condition_input_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 DATAFEED refdf = NULL_DATAFEED;
 DATAFEED outdf = NULL_DATAFEED;
 int sts = 0;
 int isloopback = 0;
 unsigned i;
 if (pp->alt_data_dest && pp->alt_data_dest->mod != cp->mod) {
	return err(ERR_SM_NOT_SAME_MODULE);
 }
 if (pp->reference && pp->reference->mod != cp->mod) {
	return err(ERR_SM_NOT_SAME_MODULE);
 }
 if (pp->reference || pp->alt_data_dest) {
	lock_module(cp->mod);	// locking hierarchy rule
 }
 lock_object(cp);
 if (pp->reference && pp->reference != cp) lock_object(pp->reference);
 if (pp->alt_data_dest && pp->alt_data_dest != cp && pp->alt_data_dest != pp->reference) {
	lock_object(pp->alt_data_dest);
 }
 if (cp->ecloopback) {
	cp->ecloopback = 0;
	for (i = HANDLE_READ; i < HANDLE_NUM; i++) {
		CARDCONN *cx2 = &cp->chan[i].cx;
		if (!sts && cardconn_valid(cx2)) sts = setfriend(cx2, 0, FRIEND_TYPE_ECHOMAIN);
	}
 }
 if (cardconn_valid(&cp->chan[HANDLE_ECHO].cx)
 	&& ERR(cx_close(&cp->chan[HANDLE_ECHO].cx)))
 	sts = err(ERR_SM_DEVERR);
 // free previous ref/altdest
 cp->mod->card->vtbl->datafeed_dtor(cp->ecref);
 cp->ecref = NULL_DATAFEED;
 cp->mod->card->vtbl->datafeed_dtor(cp->ecaltdest);
 cp->ecaltdest = NULL_DATAFEED;

 switch (pp->reference_type) {
 case kSMInputCondRefNone: break;
 case kSMInputCondRefUseInput:
	refdf = pp->reference->df[1];
 	break;
 case kSMInputCondRefUseOutput:
	refdf = pp->reference->df[0];
 	break;
 default:
 	if (!sts) sts = err(ERR_SM_BAD_PARAMETER);
 }
 switch (pp->alt_dest_type) {
 case kSMInputCondAltDestNone:
	isloopback = 1;
	outdf = cp->df[1];
 	break;
 case kSMInputCondAltDestInput:
	outdf = pp->alt_data_dest->df[1];
	if (pp->alt_data_dest == cp) isloopback = 1;
 	break;
 case kSMInputCondAltDestOutput:
	outdf = pp->alt_data_dest->df[0];
 	break;
 default:
 	if (!sts) sts = err(ERR_SM_BAD_PARAMETER);
 }
 switch (pp->conditioning_type) {
 case kSMInputCondNone:
 case kSMInputCondEchoCancelation:
 	break;
 default:
 	if (!sts) sts = err(ERR_SM_BAD_PARAMETER);
 }
 switch (pp->ectest_type) {
 case 0:
 case 1:
 case 2:
 	break;
 default:
 	if (!sts) sts = err(ERR_SM_BAD_PARAMETER);
 }
 if (!sts && (pp->conditioning_type != kSMInputCondNone || pp->ectest_type)) {
 		// if echo cancellation is enabled or we are testing
	RAWCHAN *rcp = &cp->chan[HANDLE_ECHO];
	CARDCONN *cx;
	sts = open_channel(rcp, cp->mod);
	if (!sts) {
		cx = &rcp->cx;
		switch (pp->ectest_type) {
		case 0:		// this is not a test - do the real thing
			if (!sts) sts = starttask(cx, TASKTC_ECHOMAIN, 0);
			break;
		case 1:		// set up test
			if (!sts) sts = starttask(cx, TASKTC_ECTEST, 0);
			if (!sts) sts = sendmsg2(cx, MSG_ECTEST_GAIN, 
				(int) (pp->ectest_gain * 16777216.0) );
			if (!sts) sts = sendmsg2(cx, MSG_ECTEST_DELAY,
				pp->ectest_delay);
			break;
		case 2:		// set up echo generator
			if (!sts) sts = starttask(cx, TASKTC_ECHOGEN, 0);
			if (!sts) sts = sendfilt(cx, pp->ectest_filt, pp->ectest_flen);
			break;
		}
		switch (pp->conditioning_type) {
		case kSMInputCondNone:
			break;
		case kSMInputCondEchoCancelation:
			if (!sts) sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_ECHOCAN);
			break;
		}
		if (!sts) sts = cp->mod->card->vtbl->engage_df(cp->df[1], cx, DFC_IN);
		if (!sts && nonnull_datafeed(refdf)) {
			sts = cp->mod->card->vtbl->engage_df(refdf, cx, DFC_ECREF);
			if (!sts) cp->mod->card->vtbl->connect_df(&cp->ecref, refdf);
		}
		if (!sts && isloopback) {
			cp->ecloopback = 1;
			for (i = HANDLE_READ; i < HANDLE_NUM; i++) {
				CARDCONN *cx2 = &cp->chan[i].cx;
				if (!sts && cardconn_valid(cx2)) sts = setfriend(cx2, &cp->chan[HANDLE_ECHO], FRIEND_TYPE_ECHOMAIN);
			}
		}
		if (!sts) sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_PASSTHROUGH);
		if (!sts) sts = cp->mod->card->vtbl->engage_df(outdf, cx, DFC_OUT);
		if (!sts) cp->mod->card->vtbl->connect_df(&cp->ecaltdest, outdf);
		if (sts) {
			cx_close(cx);
			if (cp->ecloopback) {
				cp->ecloopback = 0;
				for (i = HANDLE_READ; i < HANDLE_NUM; i++) {
					CARDCONN *cx2 = &cp->chan[i].cx;
					if (!sts && cardconn_valid(cx2)) sts = setfriend(cx2, 0, FRIEND_TYPE_ECHOMAIN);
				}
			}
			// free ref/altdest
			cp->mod->card->vtbl->datafeed_dtor(cp->ecref);
			cp->ecref = NULL_DATAFEED;
			cp->mod->card->vtbl->datafeed_dtor(cp->ecaltdest);
			cp->ecaltdest = NULL_DATAFEED;
		}
	}
 }
 if (pp->alt_data_dest && pp->alt_data_dest != cp && pp->alt_data_dest != pp->reference) {
	unlock_object(pp->alt_data_dest);
 }
 if (pp->reference && pp->reference != cp) unlock_object(pp->reference);
 unlock_object(cp);
 if (pp->reference || pp->alt_data_dest) {
	unlock_module(cp->mod);
 }
 return sts;
}

LOCALDEF int real_sm_condition_adjust(struct sm_condition_adjust_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 int sts = 0;
 U16 msg = 0;
 lock_object(cp);
 switch (pp->adjust_type) {
 case kSMInputCondAdjustFixGain:
 	msg = MSG_SET_FIX;
 	break;
 case kSMInputCondAdjustAGC:
 	msg = MSG_SET_AGC;
 	break;
 case kSMInputCondAdjustNonLinearWithMuting:
 case kSMInputCondAdjustNonLinearWithCNG:
	{
		int nlp;
		if (pp->adjust_value == 0) {
			nlp = 0;
		} else {
			double val = pp->adjust_value;
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
			if(pp->adjust_type == kSMInputCondAdjustNonLinearWithCNG) 
				nlp = -nlp;
		}
		sts = sendmsg2(&cp->chan[HANDLE_ECHO].cx, MSG_SET_NLP, nlp);
		break;
	}
 default:
	sts = err(ERR_SM_BAD_PARAMETER);
 }
 if (msg) {
 	switch (pp->adjust_value) {
	case 0:
	case 1:
		sts = sendmsg2(&cp->chan[HANDLE_ECHO].cx,
			msg, pp->adjust_value);
		break;
	default:
		sts = err(ERR_SM_BAD_PARAMETER);
 	}
 }
 unlock_object(cp);
 return sts;
}

LOCALDEF int real_sm_condition_adjust_span(struct sm_condition_adjust_span_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 int sts = 0;
 lock_object(cp);
 sts = sendmsg2(&cp->chan[HANDLE_ECHO].cx, MSG_EC_REINIT, pp->span);
 unlock_object(cp);
 return sts;
}

LOCALDEF int real_sm_condition_reinit(tSMChannelId channel)
{
 int sts;
 lock_object(channel);
 sts = sendmsg1(&channel->chan[HANDLE_ECHO].cx, MSG_EC_REINIT);
 unlock_object(channel);
 return sts;
}

STATIC int notify_catsigpar(tSMChannelId chan)
{
 struct ting_message msg;
 struct catsigpar *csp;
 csp = &chan->mod->catsigpar;
 memset(&msg, 0, sizeof(msg));
 msg.ioctl_length = sizeof(msg);
 msg.flags = 0;
 if (csp->initial_Lmax != MOD_DEF_INITIAL_CATSIG_LMAX) {
	msg.msg[0] = MESSAGE(MSG_SET_CATSIG_ANS_COEFFS, 7);
 } else {
	msg.msg[0] = MESSAGE(MSG_SET_CATSIG_ANS_COEFFS, 6);
 }
 msg.msg[1] = csp->ln_nrg_coeff * (1 << 14);
 msg.msg[2] = csp->min_Lmin * (1 << 2);
 msg.msg[3] = csp->Lmin_decay * (1 << 14);
 msg.msg[4] = csp->speech_thresh * (1 << 10);
 msg.msg[5] = csp->debounce * (1 << 15);
 msg.msg[6] = csp->initial_Lmax * (1 << 0);
 if (ERR(cx_ioctl(&chan->chan[HANDLE_DET].cx, IOCTL_TiNG_WRMSG,
 	&msg, sizeof(msg))))
	return err(ERR_SM_DEVERR);
 memset(&msg, 0, sizeof(msg));
 msg.ioctl_length = sizeof(msg);
 msg.msg[0] = MESSAGE(MSG_SET_CATSIG_LSD_COUNT, 13);
 msg.msg[1] = csp->min_valid_period_count;
 msg.msg[2] = csp->min_valid_count;
 msg.msg[3] = csp->glitch_count;
 msg.msg[4] = csp->qualify_count;
 msg.msg[5] = csp->alter_duration;
 msg.msg[6] = csp->max_valid_tone_cnt;
 msg.msg[7] = csp->max_valid_speech_cnt;
 msg.msg[8] = csp->threshold_samp_cnt;
 msg.msg[9] = csp->delay_time;
 msg.msg[10] = csp->period_time;
 msg.msg[11] = csp->max_off_count;
 msg.msg[12] = csp->min_period_off;
 if (ERR(cx_ioctl(&chan->chan[HANDLE_DET].cx, IOCTL_TiNG_WRMSG,
 	&msg, sizeof(msg))))
	return err(ERR_SM_DEVERR);
 return 0;
}

LOCALDEF int real_sm_catsig_listen_for(struct sm_catsig_listen_for_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 int sts = 0;
 int need_detpar;

#ifndef OMIT_ANYCHAN
 const int lockany = anychan.inuse & (1 << EVENT_NOTIFY);
 if (lockany) lock_object(&anychan);
#endif
 lock_object(cp);
 if (pp->abort_catsig_alg) {
	if (cp->catsig != pp->catsig_alg_id + 0u) sts = err(ERR_SM_WRONG_CHANNEL_STATE);
	else {
		switch (pp->catsig_alg_id) {
		case kBESPCatSigAlgLiveSpeaker:
			sts = stopdet(cp, HANDLE_DET, DETTYPE_CATSIG, 0);
			break;
		case kBESPCatSigAlgLiveSpeakerTone:
		case kBESPCatSigAlgLiveSpeakerToneReport:
			sts = stopdet(cp, HANDLE_DET, DETTYPE_CATSIGTONE, 0);
			if (!sts) sts = stopdet(cp, HANDLE_DET, DETTYPE_TONE_CATSIG, 0);
			break;
		case kBESPCatSigAlgLiveSpeakerPlugin:
			sts = stopdet(cp, HANDLE_DET, DETTYPE_CATSIGPLUGIN, 0);
			break;
		}
		cp->catsig = 0;
#ifndef OMIT_ANYCHAN
		if (!sts && cp->anyev[EVENT_NOTIFY].evstate != ANYEV_NOTANY) {
			sts = anychan_newstate(&cp->anyev[EVENT_NOTIFY], ANYEV_IDLE);
 		}
#endif
	}
 } else {
	if (cp->catsig) sts = err(ERR_SM_WRONG_CHANNEL_STATE);
	else {
		switch (pp->catsig_alg_id) {
		case kBESPCatSigAlgLiveSpeaker:
			sts = startdet(cp, HANDLE_DET, DETTYPE_CATSIG, 0);
			break;
		case kBESPCatSigAlgLiveSpeakerTone:
		case kBESPCatSigAlgLiveSpeakerToneReport:
			need_detpar = !(cp->recog[0].detmask & TONEDET_DETMASK);
			sts = startdet(cp, HANDLE_DET, DETTYPE_TONE_CATSIG, 0);
			if (need_detpar) {
				// if we started tonedet, send 'reasonable' default parameters
				if (!sts) sts = notify_detpar(cp, &cp->chan[HANDLE_DET].cx, 0, 0, 1);
				if (!sts) sts = sendmsg2(&cp->chan[HANDLE_DET].cx, MSG_SET_CONT_TIM, 60000);
			}
			if (!sts) sts = startdet(cp, HANDLE_DET, DETTYPE_CATSIGTONE, 0);
			if (!sts && pp->catsig_alg_id == kBESPCatSigAlgLiveSpeakerToneReport) {
				sts = sendmsg3(&cp->chan[HANDLE_DET].cx, MSG_CATSIG_PARAM, CATSIG_P_REPORT_TONES, 1);
			}
			break;
		case kBESPCatSigAlgLiveSpeakerPlugin:
			if ((pp->catsig_config_length < 0) || (pp->catsig_config_length > 256)) {
				sts = err(ERR_SM_BAD_DATA_LENGTH);
			}
			if (!sts) {
				sts = startdet(cp, HANDLE_DET, DETTYPE_CATSIGPLUGIN, 0);
			}
			if (!sts) {
			   sts = sendmsg2(&cp->chan[HANDLE_DET].cx, MSG_CATSIG_TIMEOUT, pp->catsig_timeout);
			}
			if (!sts && (pp->catsig_config_length != 0)) {
				if (ERR(cx_msgstring(&cp->chan[HANDLE_DET].cx, MSGSTRING_ID(MSGSTRING_CATSIG_PLUGIN, 1), pp->catsig_config_data, pp->catsig_config_length))) 
				{
					sts = err(ERR_SM_DEVERR);
				}
			}
			if (!sts) {
			   sts = sendmsg2(&cp->chan[HANDLE_DET].cx, MSG_CATSIG_SIGHINT, pp->catsig_hint_param);
			}
			break;
		default:
			sts = err(ERR_SM_BAD_PARAMETER);
			break;
		}
		if (pp->catsig_alg_id != kBESPCatSigAlgLiveSpeakerPlugin) {
			if (!sts) sts=notify_catsigpar(cp);
		}
#ifndef OMIT_ANYCHAN
		if (cp->anyev[EVENT_NOTIFY].evstate == ANYEV_IDLE) {
			sts = anychan_newstate(&cp->anyev[EVENT_NOTIFY], ANYEV_WAIT);
		}
#endif
		if (!sts) cp->catsig = pp->catsig_alg_id;
	 }
 }
 unlock_object(cp);
#ifndef OMIT_ANYCHAN
 if (lockany) unlock_object(&anychan);
#endif
 return sts;
}

LOCALDEF int real_sm_catsig_signalling_hint(struct sm_catsig_signalling_hint_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 int sts = 0;
 lock_object(cp);
 if (!cp->catsig) sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 if (!sts && (cp->catsig == kBESPCatSigAlgLiveSpeakerPlugin)) {
   sts = sendmsg2(&cp->chan[HANDLE_DET].cx, MSG_CATSIG_SIGHINT, pp->catsig_hint_param);
 }
 unlock_object(cp);
 return sts;
}

LOCALDEF int real_sm_adjust_catsig_module_params(struct sm_adjust_catsig_module_parms *pp)
{
 struct module *mp = modid2lockedmodule(pp->module);
 struct catsigpar *csp;
 int sts = 0;
 if (!mp) return err(ERR_SM_NO_SUCH_MODULE);
 csp = &mp->catsigpar;
 switch (pp->catsig_alg_id) {
 case kBESPCatSigAlgLiveSpeakerToneReport:
 case kBESPCatSigAlgLiveSpeakerTone:
 case kBESPCatSigAlgLiveSpeaker:
	switch (pp->parameter_id) {
	case kSMBESPCatSigParamF_ln_nrg_coeff:
		csp->ln_nrg_coeff = pp->parameter_value.fp_value;
		break;
	case kSMBESPCatSigParamF_min_Lmin:
		csp->min_Lmin = pp->parameter_value.fp_value;
		break;
	case kSMBESPCatSigParamF_initial_Lmax:
		csp->initial_Lmax = pp->parameter_value.fp_value;
		break;
	case kSMBESPCatSigParamF_Lmin_decay:
		csp->Lmin_decay = pp->parameter_value.fp_value;
		break;
	case kSMBESPCatSigParamF_speech_thresh:
		csp->speech_thresh = pp->parameter_value.fp_value;
		break;
	case kSMBESPCatSigParamF_debounce:
		csp->debounce = pp->parameter_value.fp_value;
		break;
	case kSMBESPCatSigParamI_min_valid_period_count:
		csp->min_valid_period_count = pp->parameter_value.int_value;
		break;
	case kSMBESPCatSigParamI_min_valid_count:
		csp->min_valid_count = pp->parameter_value.int_value;
		break;
	case kSMBESPCatSigParamI_glitch_count:
		csp->glitch_count = pp->parameter_value.int_value;
		break;
	case kSMBESPCatSigParamI_qualify_count:
		csp->qualify_count = pp->parameter_value.int_value;
		break;
	case kSMBESPCatSigParamI_alter_duration:
		csp->alter_duration = pp->parameter_value.int_value;
		break;
	case kSMBESPCatSigParamI_max_valid_tone_cnt:
		csp->max_valid_tone_cnt = pp->parameter_value.int_value;
		break;
	case kSMBESPCatSigParamI_min_valid_speech_cnt:
		csp->max_valid_speech_cnt = pp->parameter_value.int_value;
		break;
	case kSMBESPCatSigParamI_threshold_samp_cnt:
		csp->threshold_samp_cnt = pp->parameter_value.int_value;
		break;
	case kSMBESPCatSigParamI_delay_time:
		csp->delay_time = pp->parameter_value.int_value;
		break;
	case kSMBESPCatSigParamI_period_time:
		csp->period_time = pp->parameter_value.int_value;
		break;
	case kSMBESPCatSigParamI_min_off_count:
		csp->max_off_count = pp->parameter_value.int_value;
		break;
	case kSMBESPCatSigParamI_min_period_off:
		csp->min_period_off = pp->parameter_value.int_value;
		break;
	default: sts = err(ERR_SM_BAD_PARAMETER); break;
	}
	break;
 default:
	sts = err(ERR_SM_BAD_PARAMETER);
	break;
 }
 unlock_module(mp);
 return sts;
}

STATIC int setthresh(tSMChannelId cp, int val, int dir)
{
 int sts = 0;
 lock_object(cp);
 if (val > 2048 * 8) sts = err(ERR_SM_BAD_PARAMETER);
 else if (cp->rxtx[dir].func != RXTXF_NONE) {	 
	if (cp->rxtx[dir].func == RXTXF_INVALID) {
 		sts = err(ERR_SM_WRONG_CHANNEL_TYPE);
	} else if (cp->rxtx[dir].func == RXTXF_PLAYREC && !dir) {
		cp->rxtx[dir].minimum_bits = val;
		sts = chan_setxfer(cp, dir);
	} else {
  		sts = err(ERR_SM_WRONG_CHANNEL_STATE);
	}
 } else {
 	cp->rxtx[dir].minimum_bits = val;
 }
 unlock_object(cp);
 return sts;
}

LOCALDEF int real_sm_channel_set_input_threshold(struct sm_channel_set_input_threshold_parms *pp)
{
 return setthresh(pp->channel, (int)pp->minimum_bits, 1);
}

LOCALDEF int real_sm_channel_set_output_threshold(struct sm_channel_set_output_threshold_parms *pp)
{
 return setthresh(pp->channel, (int)pp->minimum_bits, 0);
}
