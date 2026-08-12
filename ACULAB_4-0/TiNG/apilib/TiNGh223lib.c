/* TiNGh223lib.c */

#include "TiNGo_h223mux.h"
#include "TiNGo_datafeed.h"
#include "smh223.h"
#include "cardconn_px.h"
#include "../hsif/kernel.h"
#include "../hsif/subtask.h"
#include "../hsif/typecode.h"
#include "../hsif/vmp.h"
#include "../hsif/h223.h"
#include "../hsif/msgstring.h"
#include "timestamp.h"
#include "trace.h"
#include "TiNGcommon.h"

#include <string.h>
#include <stdio.h>

#ifdef TiNGTYPE_LINUX
#include "netinet/in.h"
#include <poll.h>
#endif

#ifdef TiNGTYPE_QNX
#include <arpa/inet.h>
#endif

STATIC int real_sm_h223mux_get_datafeed(struct sm_h223mux_datafeed_parms *datafeedp)
{
 tSMH223muxId h223mux = datafeedp->h223mux;
 if(datafeedp->port > 3) return err(ERR_SM_DEVERR);
 lock_object(h223mux);
 datafeedp->datafeed = h223mux->dataf[datafeedp->port].vtbl_data;
 unlock_object(h223mux);
 return 0;
}

STATIC int real_sm_h223mux_create(struct sm_h223mux_create_parms *h223muxp)
{
 struct module *mod = modid2lockedmodule(h223muxp->module);
 tSMH223muxId h223mux;
 tSMGroove *grv;
 int sts;
 if (!mod) return err(ERR_SM_NO_SUCH_MODULE);
 h223mux = init_h223mux(mod);
 if (!h223mux) sts = err(ERR_SM_NO_RESOURCES);
 else {
	sts = h223mux->groove.starttask(&h223mux->groove, TASKTC_H223MUX, 0);
	grv = &h223mux->groove;
	if (!sts) {
		sts = mod->card->vtbl->setup_df(h223mux->dataf[0], mod, 0);
		if (!sts) sts = grv->engage_df(grv, h223mux->dataf[0], DFC_H223_H245_OUT);

		if (!sts) sts = mod->card->vtbl->setup_df(h223mux->dataf[1], mod, 0);
		if (!sts) sts = grv->engage_df(grv, h223mux->dataf[1], DFC_H223_AUDIO_OUT);

		if (!sts) sts = mod->card->vtbl->setup_df(h223mux->dataf[2], mod, 0);
		if (!sts) sts = grv->engage_df(grv, h223mux->dataf[2], DFC_H223_VIDEO_OUT);
		if(!sts) sts = mod->card->vtbl->setup_df(h223mux->dataf[3], mod, 0);
		if(!sts) sts = grv->engage_df(grv, h223mux->dataf[3], DFC_OUT);
	}
	h223mux->state = H223MUX_STATE_RUNNING;
	if (sts) h223mux_dtor(h223mux);
	else {
		make_object(h223mux);
	 	h223muxp->h223mux = h223mux;
	}
 }
 unlock_module(mod);
 return sts;
}

STATIC int real_sm_h223mux_datafeed_connect(struct sm_h223mux_datafeed_connect_parms *datafeedp)
{
 const enum dfconntyp dfc[4] = {DFC_H223_H245_IN, DFC_H223_AUDIO_IN, DFC_H223_VIDEO_IN, DFC_IN};
 tSMH223muxId h223mux = datafeedp->h223mux;
 DATAFEED df;
 tSMGroove *grv;
 int sts;
 if(datafeedp->port > 3) return err(ERR_SM_DEVERR);
 lock_object(h223mux);
 grv = &h223mux->groove;
 df.vtbl_data = datafeedp->datafeed;
 sts = grv->connect_df(grv, &h223mux->dfin[datafeedp->port], df);
 if (!sts) sts = grv->engage_df(grv, df, dfc[datafeedp->port]);
 unlock_object(h223mux);
 return sts;
}

STATIC int real_sm_h223mux_destroy(tSMH223muxId h223mux)
{
 if (h223mux) h223mux_dtor(h223mux);
 return 0;
}

STATIC int real_sm_h223mux_get_event(struct sm_h223mux_event_parms *eventp)
{
 int sts;
 lock_object(eventp->h223mux);
 sts = eventp->h223mux->groove.getev(&eventp->h223mux->groove, &eventp->event);
 unlock_object(eventp->h223mux);
 return sts;
}


STATIC int unsafe_sm_h223mux_status(struct sm_h223mux_status_parms *statusp)
{
	int sts;
	tSMH223muxId h223mux = statusp->h223mux;
	tSMGroove *grv = &h223mux->groove;
	uint32_t msg[16];
	unsigned long code;
	if (h223mux->state == H223MUX_STATE_IDLE) {
		return err(ERR_SM_WRONG_CHANNEL_STATE);
	}
	sts = grv->rdmsg(grv, msg, SMGROOVE_RDMSG_FLAG_NOWAIT);
	if (sts) return sts;
	code = msg[0] & 0xffff0000;
	statusp->status = kSMH223muxStatusRunning;
	if (MSG2ANYCODE(code) == MSG_DEAD) {
		if ((code & 0xffff0000) != MESSAGE(MSG_DEAD, 1)) {
			switch(msg[1]) {
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
		h223mux->state = H223MUX_STATE_IDLE;
		statusp->status = kSMH223muxStatusStopped;
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
			default:
				return err(ERR_SM_DEVERR);
			}
		} else if ((msg[1] & 0xffff0000) == MESSAGE(MSG_FRIEND, 2)) {
			if (msg[2] == MSG_BADMSG_CAUSE_NOTASK) {
				//This means there was no valid datafeed - shouldn't happen
				return err(ERR_SM_DEVERR);
			}
			if (msg[2] == MSG_BADMSG_CAUSE_WOULD_CYCLE) {
				return err(ERR_SM_BAD_DATAFEED_CONNECT);
			}
		}
	} else if (code == MESSAGE(MSG_SESSION_STARTED, 2)) {
		statusp->status = kSMH223muxStatusSessionStarted;
		statusp->u.session_transport.level = msg[1];
	} else if (code == MESSAGE(MSG_CREATE_SESSION_FAILED, 1)) {
		statusp->status = kSMH223muxStatusCreateSessionFailed;
	} else if (code == MESSAGE(MSG_DELETE_SESSION_FAILED, 1)) {
		statusp->status = kSMH223muxStatusDeleteSessionFailed;
	} else if (code == MESSAGE(MSG_SET_CAPS_FAILED, 1)) {
		statusp->status = kSMH223muxStatusSetCapsFailed;
	} else if (code == MESSAGE(MSG_ADD_ENTRY_FAILED, 1)) {
		statusp->status = kSMH223muxStatusAddEntryFailed;
	} else if (code == MESSAGE(MSG_OPEN_LC_FAILED, 1)) {
		statusp->status = kSMH223muxStatusOpenLCFailed;
	} else if (code == MESSAGE(MSG_CLOSE_LC_FAILED, 1)) {
		statusp->status = kSMH223muxStatusCloseLCFailed;
	} else if (code == MESSAGE(MSG_RECONFIG_LEVEL_CHANGE, 2)) {
		statusp->status = kSMH223muxStatusReconfigLevelChange;
		statusp->u.session_transport.level = msg[1];
	} else if (code == MESSAGE(MSG_RECONFIG_FAILED, 1)) {
		statusp->status = kSMH223muxStatusReconfigFailed;
	} else if (code == MESSAGE(MSG_H223_INTERNAL_ERROR, 2)) {
		statusp->status = kSMH223muxStatusInternalError;
		statusp->u.internal_error.num_errors = msg[1];
	} else if (code == MESSAGE(MSG_H223_PFRAME_DROP, 1)) {
		statusp->status = kSMH223muxStatusVideoPFrameDrop;
	} else if (code == MESSAGE(MSG_H223_PFRAME_SEND, 1)) {
		statusp->status = kSMH223muxStatusVideoPFrameSend;
	}
	return sts;
}

STATIC int real_sm_h223mux_status(struct sm_h223mux_status_parms *statusp)
{
	int sts;
	tSMH223muxId h223mux = statusp->h223mux;
	lock_object(h223mux);
	sts = unsafe_sm_h223mux_status(statusp);
	unlock_object(h223mux);
	return sts;
}

STATIC int real_sm_h223mux_stop(struct sm_h223mux_stop_parms *stopp)
{
 int sts = 0;
 lock_object(stopp->h223mux);
 if (stopp->h223mux->state != H223MUX_STATE_RUNNING) {
	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else {
	 sts = stopp->h223mux->groove.sendmsg(&stopp->h223mux->groove, MESSAGE(MSG_DEAD, 1));
	 if (!sts) stopp->h223mux->state = H223MUX_STATE_STOPPING;
 }
 unlock_object(stopp->h223mux);
 return sts;
}

STATIC int real_sm_h223mux_new_session(struct sm_h223mux_new_session_parms *sessionp)
{
	int sts = 0;
	lock_object(sessionp->h223mux);
	sts = sessionp->h223mux->groove.sendmsg(
			&sessionp->h223mux->groove,
			MESSAGE(MSG_NEW_SESSION, 3),
			sessionp->level,
			sessionp->bit_order);
	unlock_object(sessionp->h223mux);
	return sts;
}

STATIC int real_sm_h223mux_reconfig(struct sm_h223mux_reconfig_parms *reconfigp)
{
	int sts = 0;
	tSM_UT32 param0 = 0;

	lock_object(reconfigp->h223mux);
	switch(reconfigp->multiplex_reconfiguration.choice)	{
	case kSMH223muxReconfigModeChange :
		param0 = reconfigp->multiplex_reconfiguration.u.mode_change;
		break;
	case kSMH223muxReconfigAnnexADoubleFlag  :
		param0 = reconfigp->multiplex_reconfiguration.u.annex_a_double_flag;
		break;
	default:
		sts = err(ERR_SM_BAD_PARAMETER);
		break;
	}
	if (!sts) sts = reconfigp->h223mux->groove.sendmsg(
						&reconfigp->h223mux->groove,
						MESSAGE(MSG_RECONFIG, 4),
						reconfigp->direction,
						reconfigp->multiplex_reconfiguration.choice,
						param0);
	unlock_object(reconfigp->h223mux);
	return sts;
}

STATIC int real_sm_h223mux_set_caps(struct sm_h223mux_set_caps_parms *capsp)
{
	int sts = 0;
	lock_object(capsp->h223mux);
	// capabilities must be sent over in batches due to the parameter limit on a single
	// message being 14 32bit words
	sts = capsp->h223mux->groove.sendmsg(
				&capsp->h223mux->groove,
				MESSAGE(MSG_SET_CAPS1, 11),
				capsp->transport_with_i_frames,
				capsp->video_with_AL1,
				capsp->video_with_AL2,
				capsp->video_with_AL3,
				capsp->audio_with_AL1,
				capsp->audio_with_AL2,
				capsp->audio_with_AL3,
				capsp->data_with_AL1,
				capsp->data_with_AL2,
				capsp->data_with_AL3);
	if (!sts) sts = capsp->h223mux->groove.sendmsg(
					&capsp->h223mux->groove,
					MESSAGE(MSG_SET_CAPS2, 8),
					capsp->optional_ext,
					capsp->maximum_AL2_sdu_size,
					capsp->maximum_AL3_sdu_size,
					capsp->maximum_delay_jitter,
					capsp->max_MUX_PDU_size_capability,
					capsp->nsrp_support,
					capsp->bit_rate_o);
	if (!sts && (capsp->optional_ext & H223MUX_H245_CAP_O_MOBILE_OPERATION_TRANSMIT_CAP)) {
		sts = capsp->h223mux->groove.sendmsg(
					&capsp->h223mux->groove,
					MESSAGE(MSG_SET_CAPS_MOBILE_OP, 6),
					capsp->mobile_operation_transmit_capability_o.mode_change_capability,
					capsp->mobile_operation_transmit_capability_o.h223_AnnexA,
					capsp->mobile_operation_transmit_capability_o.h223_AnnexA_double_flag,
					capsp->mobile_operation_transmit_capability_o.h223_AnnexB,
					capsp->mobile_operation_transmit_capability_o.h223_AnnexB_with_header);
	}
	if (!sts && (capsp->optional_ext & H223MUX_H245_CAP_O_MOBILE_MULTILINK_FRAME_CAP)) {
		sts = capsp->h223mux->groove.sendmsg(
					&capsp->h223mux->groove,
					MESSAGE(MSG_SET_CAPS_MOBILE_MULTILINK, 3),
					capsp->mobile_multilink_frame_capability_o.maximum_sample_size,
					capsp->mobile_multilink_frame_capability_o.maximum_payload_length);
	}
	if (!sts && (capsp->optional_ext & H223MUX_H245_CAP_O_ANNEX_C)) {
		sts = capsp->h223mux->groove.sendmsg(
				&capsp->h223mux->groove,
				MESSAGE(MSG_SET_CAPS_ANNEXC, 15),
				capsp->H223_AnnexC_capability_o.video_with_AL1M,
				capsp->H223_AnnexC_capability_o.video_with_AL2M,
				capsp->H223_AnnexC_capability_o.video_with_AL3M,
				capsp->H223_AnnexC_capability_o.audio_with_AL1M,
				capsp->H223_AnnexC_capability_o.audio_with_AL2M,
				capsp->H223_AnnexC_capability_o.audio_with_AL3M,
				capsp->H223_AnnexC_capability_o.data_with_AL1M,
				capsp->H223_AnnexC_capability_o.data_with_AL2M,
				capsp->H223_AnnexC_capability_o.data_with_AL3M,
				capsp->H223_AnnexC_capability_o.alpdu_interleaving,
				capsp->H223_AnnexC_capability_o.maximum_AL1M_pdu_size,
				capsp->H223_AnnexC_capability_o.maximum_AL2M_pdu_size,
				capsp->H223_AnnexC_capability_o.maximum_AL3M_pdu_size,
				capsp->H223_AnnexC_capability_o.rs_code_capability);
	}
	// all structure info has been sent down so invoke actual
	// function call now :
	if (!sts) sts = capsp->h223mux->groove.sendmsg(
					&capsp->h223mux->groove,
					MESSAGE(MSG_SET_CAPS_CALL, 2),
					capsp->role);

	unlock_object(capsp->h223mux);
	return sts;
}

STATIC int real_sm_h223mux_add_entry(struct sm_h223mux_add_entry_parms *entryp)
{
	int sts = 0;
	lock_object(entryp->h223mux);
	sts = entryp->h223mux->groove.sendmsg(
			&entryp->h223mux->groove,
			MESSAGE(MSG_ADD_ENTRY, 3),
			entryp->direction,
			entryp->entry_number);
	if (!sts) {
		sts = entryp->h223mux->groove.sendmsgstring(
				&entryp->h223mux->groove,
				MSGSTRING_ID(MSGSTRING_H223_TBL_ENTRY, 1),
				entryp->descriptor_len, entryp->descriptor);
	}
	unlock_object(entryp->h223mux);
	return sts;
}

STATIC int real_sm_h223mux_open_lc(struct sm_h223mux_open_lc_parms *lcp)
{
	int sts = 0;
	tSM_UT32 /*param0 = 0,*/ param1 = 0, /*param2 = 0,*/ param3 = 0,
		param4 = 0, param5 = 0, param6 = 0, param7 = 0, param8 = 0,
		param9 = 0;

	lock_object(lcp->h223mux);
	//forward channel
	switch(lcp->type) {
	case kSMH223muxLogicalChannelTypeUnidirectional:
		param1 = LC_TYPE_UNI;
		break;
	case kSMH223muxLogicalChannelTypeBidirectional:
		param1 = LC_TYPE_BI;
		break;
	default:
		sts = err(ERR_SM_BAD_PARAMETER);
		break;
	}
	if (!sts) {
		switch(lcp->forward_channel.direction) {
		case kSMH223muxLogicalChannelDirectionTransmit:
			param3 = LC_DIR_TX;
			switch(lcp->forward_channel.transmit_lc.mark_unit) {
			case kSMH223muxMarkUnitBytes:
				param4 = LC_TX_MARK_UNIT_BYTES;
				break;
			case kSMH223muxMarkUnitALSDU:
				param4 = LC_TX_MARK_UNIT_ALSDU;
				break;
			default:
				sts = err(ERR_SM_BAD_PARAMETER);
				break;
			};
			param5 = lcp->forward_channel.transmit_lc.hi_mark;
			param6 = lcp->forward_channel.transmit_lc.lo_mark;
			break;
		case kSMH223muxLogicalChannelDirectionReceive:
			// receive direction does not have additional params
			// so leave as zero
			param3 = LC_DIR_RX;
			break;
		default:
			sts = err(ERR_SM_BAD_PARAMETER);
			break;
		}
	}
	if (!sts) {
		switch(lcp->forward_channel.adaptation_layer_type) {		
		case kSMH223muxAdaptationLayerTypeAL3:	
			param7 = LC_ADAPT_TYPE_AL3;
			param8 = lcp->forward_channel.u_al.al3.control_field_octets;
			param9 = lcp->forward_channel.u_al.al3.send_buffer_size;
			break;
		case kSMH223muxAdaptationLayerTypeNonStandard:
			param7 = LC_ADAPT_TYPE_NONSTD;
			break;
		case kSMH223muxAdaptationLayerTypeAL1Framed:
			param7 = LC_ADAPT_TYPE_AL1FRM;
			break;
		case kSMH223muxAdaptationLayerTypeAL1NotFramed:
			param7 = LC_ADAPT_TYPE_AL1NOF;
			break;
		case kSMH223muxAdaptationLayerTypeAL2WithoutSequenceNumbers:
			param7 = LC_ADAPT_TYPE_AL2NOS;
			break;
		case kSMH223muxAdaptationLayerTypeAL2WithSequenceNumbers:
			param7 = LC_ADAPT_TYPE_AL2SEQ;
			break;
		default:
			sts = err(ERR_SM_BAD_PARAMETER);
			break;
		}
	}
	if (!sts) sts = lcp->h223mux->groove.sendmsg(
		&lcp->h223mux->groove,
		MESSAGE(MSG_OPEN_LC1, 11),
		lcp->segmentable_flag, // this is uint32_t type already
		param1,
		(uint32_t)lcp->forward_channel.lcn, // declared as tSM_INT
		param3,
		param4,
		param5,
		param6,
		param7,
		param8,	
		param9);

	if (!sts) {
		// reverse channel
		if (lcp->type == kSMH223muxLogicalChannelTypeBidirectional) {
			switch(lcp->reverse_channel.direction) {
			case kSMH223muxLogicalChannelDirectionTransmit:
				param3 = LC_DIR_TX;
				switch(lcp->reverse_channel.transmit_lc.mark_unit) {
				case kSMH223muxMarkUnitBytes:
					param4 = LC_TX_MARK_UNIT_BYTES;
					break;
				case kSMH223muxMarkUnitALSDU:
					param4 = LC_TX_MARK_UNIT_ALSDU;
					break;
				default:
					sts = err(ERR_SM_BAD_PARAMETER);
					break;
				};
				param5 = lcp->reverse_channel.transmit_lc.hi_mark;
				param6 = lcp->reverse_channel.transmit_lc.lo_mark;
				break;
			case kSMH223muxLogicalChannelDirectionReceive:
				// receive direction does not have additional params
				// so leave as zero
				param3 = LC_DIR_RX;
				break;
			default:
				sts = err(ERR_SM_BAD_PARAMETER);
				break;
			}
			if (!sts) {
				switch(lcp->reverse_channel.adaptation_layer_type) {		
				case kSMH223muxAdaptationLayerTypeAL3:	
					param7 = LC_ADAPT_TYPE_AL3;
					param8 = lcp->reverse_channel.u_al.al3.control_field_octets;
					param9 = lcp->reverse_channel.u_al.al3.send_buffer_size;
					break;
				case kSMH223muxAdaptationLayerTypeNonStandard:
					param7 = LC_ADAPT_TYPE_NONSTD;
					break;
				case kSMH223muxAdaptationLayerTypeAL1Framed:
					param7 = LC_ADAPT_TYPE_AL1FRM;
					break;
				case kSMH223muxAdaptationLayerTypeAL1NotFramed:
					param7 = LC_ADAPT_TYPE_AL1NOF;
					break;
				case kSMH223muxAdaptationLayerTypeAL2WithoutSequenceNumbers:
					param7 = LC_ADAPT_TYPE_AL2NOS;
					break;
				case kSMH223muxAdaptationLayerTypeAL2WithSequenceNumbers:
					param7 = LC_ADAPT_TYPE_AL2SEQ;
					break;
				default:
					sts = err(ERR_SM_BAD_PARAMETER);
					break;
				}
			}
			if (!sts) sts = lcp->h223mux->groove.sendmsg(
				&lcp->h223mux->groove,
				MESSAGE(MSG_OPEN_LC2, 9),	
				(uint32_t)lcp->reverse_channel.lcn,	// declared as tSM_INT
				param3,	
				param4,	
				param5,	
				param6,	
				param7,
				param8,	
				param9);
		} else {
			uint32_t zero = 0;
			if (!sts) sts = lcp->h223mux->groove.sendmsg(		
				&lcp->h223mux->groove,
				MESSAGE(MSG_OPEN_LC2, 9),
				zero,
				zero,
				zero,
				zero,
				zero,
				zero,
				zero,
				zero);
		}
	}
	unlock_object(lcp->h223mux);
	return sts;
}

STATIC int real_sm_h223mux_close_lc(struct sm_h223mux_close_lc_parms *lcp)
{
	int sts = 0;
	lock_object(lcp->h223mux);
	sts = lcp->h223mux->groove.sendmsg(
			&lcp->h223mux->groove,
			MESSAGE(MSG_CLOSE_LC, 3),
			lcp->direction,
			lcp->lcn);
	unlock_object(lcp->h223mux);
	return sts;
}

STATIC int real_sm_h223mux_delete_session(struct sm_h223mux_delete_session_parms *sessionp)
{
	int sts = 0;
	lock_object(sessionp->h223mux);
	sts = sessionp->h223mux->groove.sendmsg(
			&sessionp->h223mux->groove,
			MESSAGE(MSG_DELETE_SESSION, 1));
	unlock_object(sessionp->h223mux);
	return sts;
}

#include "../pubdoc/gen/prosh223api.i"
