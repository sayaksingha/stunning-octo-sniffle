/* TiNGvideo.c - implementation of video */

#include "TiNGcommon.h"
#include "../hsif/kernel.h"
#include "../hsif/subtask.h"
#include "../hsif/typecode.h"
#include "../hsif/av.h"
#include "../hsif/hbuf.h"
#include "../hsif/samplerate.h"
#include "../hsif/vmp.h"
#include "TiNGo_vidplay.h"
#include "TiNGo_vidrec.h"
#include "smgroove.h"
#include "smvideo.h"
#include "timestamp.h"
#include "trace.h"
#include "trace_smvideo_show.h"

STATIC int real_sm_avfplay_create(struct sm_avfplay_create_parms *pcp)
{
 struct module *mod = modid2lockedmodule(pcp->module);
 tSMAVFplayId avfplay;
 int sts = 0;
 if (!mod) return err(ERR_SM_NO_SUCH_MODULE);
 avfplay = init_avfplay(mod);
 if (!avfplay){
 	sts = err(ERR_SM_NO_RESOURCES);
 }
 else {
 	make_object(avfplay);
 	pcp->avfplay = avfplay;
 }
 unlock_module(mod);
 return sts;
}

STATIC int real_sm_avfplay_sync(struct sm_avfplay_sync_parms *asp)
{
 const tSMAVFplayId avfplay = asp->avfplay;
 int sts = 0;
 if (asp->sync_avfplay == avfplay) return err(ERR_SM_BAD_PARAMETER);
 lock_module(avfplay->mod);
 lock_object(avfplay);
 if (asp->sync_avfplay) {
 	lock_object(asp->sync_avfplay);
		//Only allow assignment when idle
	if (avfplay->state != ST_REPLAY_IDLE || asp->sync_avfplay->state != ST_REPLAY_IDLE) {
		sts = err(ERR_SM_WRONG_CHANNEL_STATE);
	}
		//if we already have partners - don't allow assignment over the top
	if (avfplay->sync_partner || asp->sync_avfplay->sync_partner) {
		sts = err(ERR_SM_WRONG_CHANNEL_STATE);
	}

 	if (!sts) {
		avfplay->sync_partner = asp->sync_avfplay;
		asp->sync_avfplay->sync_partner = avfplay;
	}
 	unlock_object(asp->sync_avfplay);
 } else { //disassociate
	if (avfplay->sync_partner) {
		//if we have a partner, remove it
		const tSMAVFplayId partner = avfplay->sync_partner;
		lock_object(partner);
		if (avfplay->state != ST_REPLAY_IDLE) {
			//if we were waiting for a partner to start we need to be run now
			tSMGroove *grv = &avfplay->groove;
			sts = grv->setfriend(grv, NULL, FRIEND_TYPE_PLAYAVF); //send NULL if no sync_partner to allow the task to process data
		}
		if (partner->state != ST_REPLAY_IDLE) {
			//the partner is waiting to start, it needs to be run now
			tSMGroove *grv = &avfplay->sync_partner->groove;
			sts = grv->setfriend(grv, NULL, FRIEND_TYPE_PLAYAVF); //send NULL if no sync_partner to allow the task to process data
		}
		avfplay->sync_partner = NULL;
		partner->sync_partner = NULL;
		unlock_object(partner);
	}
 }
 unlock_object(avfplay);
 unlock_object(avfplay->mod);
 return sts;
}

static int type2subtaskid(tSM_INT out_not_in, tSM_INT type)
{
 switch (type) {
 case kSMAudioDataFormatALawPCM:
 	if (out_not_in) return SUBTASKTC_PLAYALAW;
 	return SUBTASKTC_RECALAW;
 case kSMAudioDataFormatULawPCM:
 	if (out_not_in) return SUBTASKTC_PLAYMULAW;
 	return SUBTASKTC_RECMULAW;
 case kSMAudioDataFormatAMRNB:
 	if (out_not_in) return SUBTASKTC_AMRNBDEC_C;
 	return SUBTASKTC_AMRNBENC_C;
 case kSMAudioDataFormat16bit:
	if (out_not_in) return SUBTASKTC_PLAY16BIT;
	return SUBTASKTC_REC16BIT;
 case kSMAudioDataFormat8bit:
	if (out_not_in) return SUBTASKTC_PLAYMS8BIT;
	return SUBTASKTC_RECMS8BIT;
 default:
 	break;
 }
 return -1;
}

#define VID_SAMPLING_RATE 90000
#define DFLT_AUD_SAMPLING_RATE 8000
#define DFLT_VID_HBUF_SIZE (32768 - 4)
#define DFLT_AUD_HBUF_SIZE (8192 - 4)

// Note: sasp->test_mode ignored until functionality for it is implemented.
STATIC int real_sm_avfplay_start(struct sm_avfplay_start_parms *sasp)
{
 tSMAVFplayId avfplay = sasp->avfplay;
 int sts = 0;
 // interpret parms
 unsigned long typecode;
 tSM_INT sampling_rate;
 tSM_INT hbuf_size = sasp->hbuf_size;
 int st_id;
 if (sasp->dest_type == kSMAVFOutputAVFVideo) {
	typecode = TASKTC_OUTVID;
	sampling_rate = VID_SAMPLING_RATE;
	if (!hbuf_size) hbuf_size = DFLT_VID_HBUF_SIZE;
	st_id = -1;
 } else {
	typecode = sasp->dest_type == kSMAVFOutputAVFAudio ? TASKTC_OUTVID : TASKTC_OUTCHAN;
	sampling_rate = sasp->u_dest_type.audio.sampling_rate;
	if (!sampling_rate) sampling_rate = DFLT_AUD_SAMPLING_RATE;
	if (!hbuf_size) hbuf_size = DFLT_AUD_HBUF_SIZE;
	st_id = type2subtaskid(1, sasp->u_dest_type.audio.src_type);
	if (st_id == -1) return err(ERR_SM_BAD_PARAMETER);
	if (st_id == SUBTASKTC_AMRNBENC_C && sampling_rate != 8000) return err(ERR_SM_BAD_PARAMETER);
 }
 lock_object(avfplay->mod);
 lock_object(avfplay);
 if (avfplay->sync_partner) lock_object(avfplay->sync_partner);
 if (avfplay->state != ST_REPLAY_IDLE) sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 if (!sts) {
 	tSMGroove *grv = &avfplay->groove;
 	sts = grv->starttask(grv, typecode, 0);
 	if (!sts && nonnull_datafeed(avfplay->dataf)) {
 		sts = avfplay->groove.engage_df(&avfplay->groove, avfplay->dataf, DFC_OUT);
 	}
 	// Start subtasks, including AMR-NB frame splitter (or other audio pre-processing) if required
 	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_SAMPLERATE, 2), sampling_rate);
 	if (!sts && typecode == TASKTC_OUTCHAN) sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), st_id);
	if (!sts && st_id == SUBTASKTC_AMRNBDEC_C) sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_AVFAMRSPLIT);
 	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_PLAYAVF);
 	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_HBUFFROMHOST);
 	if (!sts && typecode == TASKTC_OUTVID) sts = grv->sendmsg(grv, MESSAGE(MSG_SET_BUFSIZE, 2), hbuf_size);
 	if (!sts) {
 		S32 minimum_bits = 2048 * 8;
 		U32 timeout_milliseconds = 0;
 		sts = cx_setxfer(grv->cardconn_cx(grv), minimum_bits, timeout_milliseconds);
 	}
 	if (!sts && !avfplay->sync_partner) 
	{
		sts = grv->setfriend(grv, NULL, FRIEND_TYPE_PLAYAVF); //send NULL if no sync_partner to allow the task to process data
		avfplay->state = ST_REPLAY_RUN;
	} else if (!sts) { // implies avfplay->sync_partner != NULL
		if (avfplay->sync_partner->state == ST_REPLAY_WAITSYNC) { //sync_partner is waiting for us
			sts = grv->setfriend(grv, &avfplay->sync_partner->groove, FRIEND_TYPE_PLAYAVF);
			if (!sts) {
				avfplay->state = ST_REPLAY_RUN;
				avfplay->sync_partner->state = ST_REPLAY_RUN;
			}
		} else if (avfplay->sync_partner->state != ST_REPLAY_IDLE) { //Can't sync unless sync_partner is waiting or idle
			sts = ERR_SM_WRONG_CHANNEL_STATE;
		} else { //We're waiting for sync_partner
			avfplay->state = ST_REPLAY_WAITSYNC;		
		}
	}
 }
 if (avfplay->sync_partner) unlock_object(avfplay->sync_partner);
 unlock_object(avfplay);
 unlock_object(avfplay->mod);
 return sts;
}

STATIC int real_sm_avfplay_stop(struct sm_avfplay_stop_parms *asp)
{
 tSMAVFplayId avfplay = asp->avfplay;
 tSMGroove *grv = &avfplay->groove;
 int sts = 0;
 lock_object(avfplay);
 if (avfplay->state != ST_REPLAY_RUN && avfplay->state != ST_REPLAY_LAST && avfplay->state != ST_REPLAY_WAITSYNC) sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 if (!sts) {
	if (avfplay->state != ST_REPLAY_LAST) {
 		CARDCONN *cx = grv->cardconn_cx(grv);
 		if (ERR(cx_nomore(cx))) sts = err(ERR_SM_DEVERR);
	}
 	if (!sts) {
 		avfplay->state = ST_REPLAY_LAST;
 		if (asp->immediate) sts = grv->sendmsg(grv, MESSAGE(MSG_PLAY_STOP, 1));
 	}
 }
 unlock_object(avfplay);
 return sts;
}

STATIC int unsafe_sm_avfplay_status(struct sm_avfplay_status_parms *statusp)
{
 tSMAVFplayId avfplay = statusp->avfplay;
 tSMGroove *grv = &avfplay->groove;
 uint32_t msg[16];
 unsigned long code;
 int sts;
 sts = grv->rdmsg(grv, msg, 1);
 if (sts) return sts;
 statusp->status = kSMAVFplayStatusRunning;
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
		case MSG_DEAD_CAUSE_INVALIDDATA:
			return err(ERR_SM_FILE_FORMAT);
		}
	}
	avfplay->state = ST_REPLAY_DONE;
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
	}
 } else if (code == MESSAGE(MSG_PLAY_STOP, 2)) {
	 // number of bytes discarded from hbuf2d is: msg[1] / 8
	sts = grv->starttask(grv, 0, 0);
	if (sts) return sts;
 } else if (code == MESSAGE(MSG_UNDERRUN, 1)) {
	 statusp->status = kSMAVFplayStatusUnderrun;
 }

 if (avfplay->state == ST_REPLAY_LAST && statusp->status == kSMAVFplayStatusRunning) {
	 statusp->status = kSMAVFplayStatusCompleteData;
 } else if (avfplay->state == ST_REPLAY_DONE) {
	 avfplay->state = ST_REPLAY_IDLE;
	 statusp->status = kSMAVFplayStatusComplete;
 }

 return 0;
}

STATIC int real_sm_avfplay_status(struct sm_avfplay_status_parms *statusp)
{
 int sts;
 lock_object(statusp->avfplay);
 sts = unsafe_sm_avfplay_status(statusp);
 unlock_object(statusp->avfplay);
 return sts;
}

STATIC int real_sm_avfplay_get_event(struct sm_avfplay_event_parms *eventp)
{
 tSMAVFplayId avfplay = eventp->avfplay;
 int sts;
 lock_object(avfplay);
 sts = avfplay->groove.getev(&avfplay->groove, &eventp->event);
 unlock_object(avfplay);
 return sts;
}

STATIC int real_sm_avfplay_get_datafeed(struct sm_avfplay_datafeed_parms *datafeedp)
{
 tSMAVFplayId avfplay = datafeedp->avfplay;
 tSMGroove *grv;
 int sts;
 lock_object(avfplay);
 grv = &avfplay->groove;
 sts = grv->get_df(grv,avfplay->dataf, avfplay->mod);
 if (!sts) datafeedp->datafeed = avfplay->dataf.vtbl_data;
 unlock_object(avfplay);
 return sts;
}

STATIC int real_sm_avfplay_destroy(tSMAVFplayId avfplay)
{
 if (avfplay) {
	tSMGroove *grv;
	int sts = 0;
 	lock_object(avfplay->mod);
 	lock_object(avfplay);
	if (avfplay->sync_partner) {
		lock_object(avfplay->sync_partner);
		if (avfplay->sync_partner->state != ST_REPLAY_IDLE) { //could check more states here but should be harmless to send if !IDLE
			grv = &avfplay->groove;	
			sts = grv->setfriend(grv, NULL, FRIEND_TYPE_PLAYAVF); //send NULL to sync_partner in case the dtor hasn't done so
			//FIXME: Check sts?
		}	
		avfplay->sync_partner->sync_partner = NULL;
		unlock_object(avfplay->sync_partner);
	}
 	unlock_object(avfplay);
 	unlock_object(avfplay->mod);
 	avfplay_dtor(avfplay);
 }
 return 0;
}

STATIC int real_sm_avfplay_put_data(struct sm_avfplay_put_data_parms *datap)
{
 tSMAVFplayId avfplay = datap->avfplay;
 int sts = 0;
 lock_object(avfplay);
 sts = avfplay->groove.write(&avfplay->groove, datap->data, datap->max_length, (unsigned*)&datap->done_length);
 unlock_object(avfplay);
 return sts;
}

STATIC int real_sm_avfrec_create(struct sm_avfrec_create_parms *pcp)
{
 struct module *mod = modid2lockedmodule(pcp->module);
 tSMAVFrecId avfrec;
 int sts = 0;
 if (!mod) return err(ERR_SM_NO_SUCH_MODULE);
 avfrec = init_avfrec(mod);
 if (!avfrec) {
 	sts = err(ERR_SM_NO_RESOURCES);
 } else {
 	make_object(avfrec);
 	pcp->avfrec = avfrec;
 }
 unlock_module(mod);
 return sts;
}

STATIC int real_sm_avfrec_sync(struct sm_avfrec_sync_parms *asp)
{
 const tSMAVFrecId avfrec = asp->avfrec;
 int sts = 0;
 if (asp->sync_avfrec == avfrec) return err(ERR_SM_BAD_PARAMETER);
 lock_module(avfrec->mod);
 lock_object(avfrec);
 if (asp->sync_avfrec) {
 	lock_object(asp->sync_avfrec);
		//Only allow assignment when idle
	if (avfrec->state != ST_RECORD_IDLE || asp->sync_avfrec->state != ST_RECORD_IDLE) {
		sts = err(ERR_SM_WRONG_CHANNEL_STATE);
	}
		//if we already have partners - don't allow assignment over the top
	if (avfrec->sync_partner || asp->sync_avfrec->sync_partner) {
		sts = err(ERR_SM_WRONG_CHANNEL_STATE);
	}

 	if (!sts) {
		avfrec->sync_partner = asp->sync_avfrec;
		asp->sync_avfrec->sync_partner = avfrec;
	}
 	unlock_object(asp->sync_avfrec);
 } else { //disassociate
	if (avfrec->sync_partner) {
		//if we have a partner, remove it
		const tSMAVFrecId partner = avfrec->sync_partner;
		lock_object(partner);
		if (avfrec->state != ST_RECORD_IDLE) {
			//if we were waiting for a partner to start we need to be run now
			tSMGroove *grv = &avfrec->groove;
			sts = grv->setfriend(grv, NULL, FRIEND_TYPE_RECAVF); //send NULL if no sync_partner to allow the task to process data
		}
		if (partner->state != ST_RECORD_IDLE) {
			//the partner is waiting to start, it needs to be run now
			tSMGroove *grv = &partner->groove;
			sts = grv->setfriend(grv, NULL, FRIEND_TYPE_RECAVF); //send NULL if no sync_partner to allow the task to process data
		}
		avfrec->sync_partner = NULL;
		partner->sync_partner = NULL;
		unlock_object(partner);
	}
 }
 unlock_object(avfrec);
 unlock_object(avfrec->mod);
 return sts;
}

static int real_sm_avfrec_start(struct sm_avfrec_start_parms *sasp)
{
 tSMAVFrecId avfrec = sasp->avfrec;
 int sts = 0;
 // interpret parms
 unsigned long typecode;
 int st_id;
 tSM_INT hbuf_size = sasp->hbuf_size;
 if (sasp->src_type == kSMAVFInputAVFVideo) {
	 typecode = TASKTC_INVID;
	 st_id = -1;
	 if (!hbuf_size) hbuf_size = DFLT_VID_HBUF_SIZE;
 } else {
	if (sasp->src_type == kSMAVFInputAVFAudio) {
		typecode = TASKTC_INVID;
		st_id = -1;
	} else {
		typecode = TASKTC_INCHAN;
		st_id = type2subtaskid(0, sasp->u_src_type.audio.dest_type);
		if (st_id == -1) return err(ERR_SM_BAD_PARAMETER);
	}
	if (!hbuf_size) hbuf_size = DFLT_AUD_HBUF_SIZE;
 }
 if (!sts && (typecode == TASKTC_INCHAN) && (sasp->u_src_type.audio.payload_type < 0 || sasp->u_src_type.audio.payload_type > 127)) {
	 return err(ERR_SM_BAD_PARAMETER);
 }
 lock_object(avfrec->mod);
 lock_object(avfrec);
 if (avfrec->sync_partner) lock_object(avfrec->sync_partner);
 if (!sts && (avfrec->state != ST_RECORD_IDLE)) sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 if (!sts) {
 	tSMGroove *grv = &avfrec->groove;
	sts = grv->starttask(grv, typecode, 0);
 	if (!sts && nonnull_datafeed(avfrec->dataf)) {
 		sts = avfrec->groove.engage_df(&avfrec->groove, avfrec->dataf, DFC_IN);
 	}
 	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_RECAVF);
 	if (typecode == TASKTC_INCHAN) {
		if (!sts && sasp->test_mode == 1) sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_RECAVF_TESTGATELIN);
 		if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), st_id);
 		switch (st_id) {
 		case SUBTASKTC_RECALAW:
 		case SUBTASKTC_RECMULAW:
		case SUBTASKTC_REC16BIT:
		case SUBTASKTC_RECMS8BIT:
 			if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_RECAVF_CFG, 3), 20, sasp->u_src_type.audio.payload_type); // 20ms PP
 			break;
 		case SUBTASKTC_AMRNBENC_C:
	 		// note: codec starts to run following payload type send, so do this last
 			if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_AMRNB_ALIGN, 2), 1); // force aligned
			if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_AMRNB_MODE, 2), sasp->u_src_type.audio.u_dest_type.amrnb.bitrate);
			if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_VAD_MODE, 2), sasp->u_src_type.audio.u_dest_type.amrnb.vad_enable);
 			if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_AMRNBENC_C, sasp->u_src_type.audio.payload_type, 20); // 1 FPP
 			break;
 		default:
 			return err(ERR_SM_BAD_PARAMETER);
 		}
 	}
 	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_HBUFTOHOST);
 	if (!sts && typecode == TASKTC_INVID) sts = grv->sendmsg(grv, MESSAGE(MSG_SET_BUFSIZE, 2), hbuf_size);
 	if (!sts) {
 		S32 minimum_bits = 2048 * 8;
 		U32 timeout_milliseconds = 0;
		if (sasp->src_type == kSMAVFInputAVFAudio) minimum_bits = 1024 * 8;
 		sts = cx_setxfer(grv->cardconn_cx(grv), minimum_bits, timeout_milliseconds);
 	}
 	if (!sts && !avfrec->sync_partner) {
		sts = grv->setfriend(grv, NULL, FRIEND_TYPE_RECAVF); //send NULL if no sync_partner to allow the task to process data
		avfrec->state = ST_RECORD_RUN;
	} else if (!sts) { // implies avfrec->sync_partner != NULL
		if (avfrec->sync_partner->state == ST_RECORD_WAITSYNC) { //sync_partner is waiting for us
			sts = grv->setfriend(grv, &avfrec->sync_partner->groove, FRIEND_TYPE_RECAVF);
			if (!sts) {
				avfrec->state = ST_RECORD_RUN;
				avfrec->sync_partner->state = ST_RECORD_RUN;
			}
		} else if (avfrec->sync_partner->state != ST_RECORD_IDLE) { //Can't sync unless sync_partner is waiting or idle
			sts = ERR_SM_WRONG_CHANNEL_STATE;
		} else { //We're waiting for sync_partner
			avfrec->state = ST_RECORD_WAITSYNC;		
		}
	}
 }
 if (avfrec->sync_partner) unlock_object(avfrec->sync_partner);
 unlock_object(avfrec);
 unlock_object(avfrec->mod);
 return sts;
}

STATIC int real_sm_avfrec_stop(struct sm_avfrec_stop_parms *satp)
{
 tSMAVFrecId avfrec = satp->avfrec;
 tSMGroove *grv = &avfrec->groove;
 int sts = 0;
 lock_object(avfrec);
 if (avfrec->state != ST_RECORD_RUN && avfrec->state != ST_RECORD_WAITSYNC) sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 if (!sts) {
 	CARDCONN *cx = grv->cardconn_cx(grv);
 	if (ERR(cx_nomore(cx))) sts = err(ERR_SM_DEVERR);
 	if (!sts) {
 		avfrec->state = ST_RECORD_LAST;
 		if (satp->immediate) {
 			sts = grv->starttask(grv, 0, 0);
 		} else {
 			sts = grv->sendmsg(grv, MESSAGE(MSG_PLAY_STOP, 1));
 		}
 	}
 }
 unlock_object(avfrec);
 return sts;
}

STATIC int unsafe_sm_avfrec_status(struct sm_avfrec_status_parms *statusp)
{
 tSMAVFrecId avfrec = statusp->avfrec;
 tSMGroove *grv = &avfrec->groove;
 uint32_t msg[16];
 unsigned long code;
 int sts;
 sts = grv->rdmsg(grv, msg, 1);
 if (sts) return sts;
 statusp->status = kSMAVFrecordStatusRunning;
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
	avfrec->state = ST_RECORD_DONE;
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
	}
 } else if (code == MESSAGE(MSG_PLAY_STOP, 2)) {
	 sts = grv->starttask(grv, 0, 0);
	 if (sts) return sts;
 } else if (code == MESSAGE(MSG_UNDERRUN, 1)) {
	 statusp->status = kSMAVFrecordStatusOverrun;
 }

 if (avfrec->state == ST_RECORD_LAST && statusp->status == kSMAVFrecordStatusRunning) {
	 statusp->status = kSMAVFrecordStatusCompleting;
 } else if(avfrec->state == ST_RECORD_DONE) {
	 avfrec->state = ST_RECORD_IDLE;
	 statusp->status = kSMAVFrecordStatusComplete;
 }
 return 0;
}

STATIC int real_sm_avfrec_status(struct sm_avfrec_status_parms *statusp)
{
 int sts;
 lock_object(statusp->avfrec);
 sts = unsafe_sm_avfrec_status(statusp);
 unlock_object(statusp->avfrec);
 return sts;
}

STATIC int real_sm_avfrec_get_data(struct sm_avfrec_get_data_parms *datap)
{
 tSMAVFrecId avfrec = datap->avfrec;
 int sts;
 lock_object(avfrec);
 sts = avfrec->groove.read(&avfrec->groove, datap->data, datap->max_length, (unsigned*)&datap->done_length);
 unlock_object(avfrec);
 return sts;
}

STATIC int real_sm_avfrec_get_event(struct sm_avfrec_event_parms *eventp)
{
 tSMAVFrecId avfrec = eventp->avfrec;
 int sts;
 lock_object(avfrec);
 sts = avfrec->groove.getev(&avfrec->groove, &eventp->event);
 unlock_object(avfrec);
 return sts;
}

STATIC int real_sm_avfrec_datafeed_connect(struct sm_avfrec_datafeed_connect_parms *datafeedp)
{
 tSMAVFrecId avfrec = datafeedp->avfrec;
 tSMGroove *grv;
 DATAFEED df;
 int sts;
 lock_object(avfrec);
 grv = &avfrec->groove;
 df.vtbl_data = datafeedp->data_source;
 sts = grv->connect_df(grv, &avfrec->dataf, df);
 if (!sts) sts = grv->engage_df(grv, df, DFC_IN);
 unlock_object(avfrec);
 return sts;
}

STATIC int real_sm_avfrec_destroy(tSMAVFrecId avfrec)
{
 if (avfrec) {
	tSMGroove *grv;
	int sts = 0;
 	lock_object(avfrec->mod);
 	lock_object(avfrec);
	if (avfrec->sync_partner) {
		lock_object(avfrec->sync_partner);
		if (avfrec->sync_partner->state != ST_RECORD_IDLE) { //could check more states here but should be harmless to send if !IDLE
			grv = &avfrec->groove;	
			sts = grv->setfriend(grv, NULL, FRIEND_TYPE_RECAVF); //send NULL to sync_partner in case the dtor hasn't done so
			//FIXME: check sts?
		}	
		avfrec->sync_partner->sync_partner = NULL;
		unlock_object(avfrec->sync_partner);
	}
 	unlock_object(avfrec);
 	unlock_object(avfrec->mod);
 	avfrec_dtor(avfrec);
 }
 return 0;
}

#include "../pubdoc/gen/prosvid.i"
