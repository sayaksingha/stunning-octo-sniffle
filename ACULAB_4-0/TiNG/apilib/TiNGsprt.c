/* TiNGsprt.c */

#include "TiNGo_sprt.h"
#include "TiNGo_datafeed.h"
#include "smsprt.h"
#include "cardconn_px.h"
#include "../hsif/kernel.h"
#include "../hsif/msgstring.h"
#include "../hsif/subtask.h"
#include "../hsif/typecode.h"
#include "../hsif/vmp.h"
#include "../hsif/sprt.h"
#include "../hsif/ipv6.h"
#include "timestamp.h"
#include "trace.h"
#include "trace_sprt_show.h"
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

STATIC int real_sm_sprt_create(struct sm_sprt_create_parms *sprtp)
{
 struct module *mod = modid2lockedmodule(sprtp->module);
 tSMSPRTId sprt;
 int sts = 0;
 if (!mod) return err(ERR_SM_NO_SUCH_MODULE);
 sprt = init_sprt(mod);
 if (!sprt) sts = err(ERR_SM_NO_RESOURCES);
 else {
	tSMGroove *grv = &sprt->groove;
	tSMGroove *grv_pkt = &sprt->groove_packetiser;
	sprt->data_chan = 3;
	sts = grv->starttask(grv, TASKTC_SPRT, 0);
	if (!sts) sts = grv_pkt->starttask(grv_pkt, TASKTC_SPRT_PACKETISER, 0);
	if (!sts) sts = grv->setfriend(grv, grv_pkt, FRIEND_TYPE_SPRT_PACKETISER);
	if (sts) sprt_dtor(sprt);
	else {
		make_object(sprt);
		sprtp->sprt = sprt;
	}
 }
 unlock_module(mod);
 return sts;
}

STATIC int real_sm_sprt_destroy(tSMSPRTId sprt)
{
 if (sprt) sprt_dtor(sprt);
 return 0;
}

STATIC int real_sm_sprt_get_event(struct sm_sprt_get_event_parms *eventp)
{
 int sts;
 lock_object(eventp->sprt);
 sts = eventp->sprt->groove.getev(&eventp->sprt->groove, &eventp->event);
 unlock_object(eventp->sprt);
 return sts;
}

STATIC int unsafe_sm_sprt_status_packetiser(struct sm_sprt_status_parms *statusp)
{
 int sts;
 tSMSPRTId sprt = statusp->sprt;
 tSMGroove *grv = &sprt->groove_packetiser;
 uint32_t msg[16];
 unsigned long code;
 sts = grv->rdmsg(grv, msg, SMGROOVE_RDMSG_FLAG_NOWAIT);
 if (sts) return sts;
 code = msg[0] & 0xffff0000;
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
	}
 }
 return sts;
}

STATIC int unsafe_sm_sprt_status(struct sm_sprt_status_parms *statusp)
{
 int sts;
 tSMSPRTId sprt = statusp->sprt;
 tSMGroove *grv = &sprt->groove;
 uint32_t msg[16];
 unsigned long code;
 statusp->status = kSMSPRTStatusRunning;
 sts = unsafe_sm_sprt_status_packetiser(statusp);
 if (sts) return sts;
 sts = grv->rdmsg(grv, msg, SMGROOVE_RDMSG_FLAG_NOWAIT|SMGROOVE_RDMSG_FLAG_KEEPSTRINGS);
 if (sts) return sts;
 code = msg[0] & 0xffff0000;
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
	statusp->status = kSMSPRTStatusStopped;
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
	}
 } else if (code == MESSAGE(MSG_STRING, 3)) {
	if (msg[1] == MSGSTRING_ID(MSGSTRING_SPRT, 0)) {
		statusp->status = kSMSPRTStatusMessage;
	} else {
		sts = grv->rdmsgstring(grv, msg[1], 0xffffffffu, 0, 0, 0); // discard
	}
 } else if (code == MESSAGE(MSG_SPRT_ERROR, 2)) {
	 if (msg[1] == MSG_SPRT_ERROR_SEND_MESSAGE || msg[1] == MSG_SPRT_ERROR_SEND_DATA) {
		 sts = err(ERR_SM_NO_CAPACITY);
	 }
 }
 return sts;
}

STATIC int real_sm_sprt_status(struct sm_sprt_status_parms *statusp)
{
 int sts;
 tSMSPRTId sprt = statusp->sprt;
 lock_object(sprt);
 sts = unsafe_sm_sprt_status(statusp);
 unlock_object(sprt);
 return sts;
}


STATIC int real_sm_sprt_config(struct sm_sprt_config_parms *configp)
{
 int sts = 0;
 tSMGroove *grv;
 lock_object(configp->sprt);
 grv = &configp->sprt->groove;
 sts = grv->sendmsg(grv, 
			MESSAGE(MSG_SET_OUTIPV4, 5),
			ip2msg(configp->dest.sin_addr.s_addr),
			ntohs(configp->dest.sin_port),
			ip2msg(configp->src.sin_addr.s_addr),
			ntohs(configp->src.sin_port));
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_SPRT_SET_TX_PAYLOAD_TYPE, 2), configp->payload_type);
 unlock_object(configp->sprt);
 return sts;
}

STATIC int real_sm_sprt_config_ipv6(struct sm_sprt_config_ipv6_parms *configp)
{
 int sts = 0;
 tSMGroove *grv;
 lock_object(configp->sprt);
 grv = &configp->sprt->groove;
 sts = grv->sendmsg(grv, 
			MESSAGE(MSG_SET_OUTIPV6, 11),
			ipv6tomsg(configp->dest.sin6_addr),
			ntohs(configp->dest.sin6_port),
			ipv6tomsg(configp->src.sin6_addr),
			ntohs(configp->src.sin6_port));
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_SPRT_SET_TX_PAYLOAD_TYPE, 2), configp->payload_type);
 unlock_object(configp->sprt);
 return sts;
}

STATIC int real_sm_sprt_config_reliable_channel(struct sm_sprt_config_reliable_channel_parms *parms)
{
 int sts = 0;
 int32_t chan = parms->channel;
 tSMGroove *grv;
 if (chan != 1 && chan != 2) return err(ERR_SM_BAD_PARAMETER);
 lock_object(parms->sprt);
 grv = &parms->sprt->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_SPRT_CONFIG_CHANNEL, 6), chan,
								parms->window_size,
								parms->timer_TA01,
								parms->timer_TA02,
								parms->timer_TR03);
 unlock_object(parms->sprt);
 return sts;
}

STATIC int real_sm_sprt_config_data_channel(struct sm_sprt_config_data_channel_parms *parms)
{
 int sts = 0;
 tSMGroove *grv;
 int32_t chan = parms->data_channel;
 if (chan != 1 && chan != 3) return err(ERR_SM_BAD_PARAMETER);
 lock_object(parms->sprt);
 grv = &parms->sprt->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_SPRT_SET_DATA_CHANNEL, 2), chan);
 if (!sts) {
	 tSMGroove *grv_pkt = &parms->sprt->groove_packetiser;
	 sts = grv_pkt->sendmsg(grv_pkt, MESSAGE(MSG_SPRT_CONFIG_DATA, 3), parms->max_payload_length, parms->max_latency);
 }
 if (!sts) parms->sprt->data_chan = chan;
 unlock_object(parms->sprt);
 return sts;
}

STATIC int real_sm_sprt_config_data_format(struct sm_sprt_config_data_format_parms *parms)
{
 int sts = 0;
 tSMGroove *grv;
 tSMGroove *grv_pkt;
 uint32_t tc = 0;
 lock_object(parms->sprt);
 grv = &parms->sprt->groove;
 grv_pkt = &parms->sprt->groove_packetiser;
 if (parms->direction == kSMSPRTDataDirectionTx) {
	switch (parms->format) {
	case kSMSPRTDataFormatNone:
		break;
	case kSMSPRTDataFormatIOctet:
		tc = SUBTASKTC_IOCTET_PACKETISER;
		break;
	case kSMSPRTDataFormatIOctetCS:
		tc = SUBTASKTC_IOCTET_CS_PACKETISER;
		break;
	case kSMSPRTDataFormatIRawOctet:
		tc = SUBTASKTC_IRAW_OCTET_PACKETISER;
		break;
	default:
		sts = err(ERR_SM_BAD_PARAMETER);
		break;
	}
	if (!sts && tc != parms->sprt->data_format_tc[0]) {
		if (parms->sprt->data_format_tc[0]) {
			grv->sendmsg(grv, MESSAGE(MSG_SPRT_PACKET_DATA, 2), 0);
			if (!sts) sts = grv_pkt->sendmsg(grv_pkt, MESSAGE(MSG_DELSUB, 2), parms->sprt->data_format_tc[0]);
		}
		if (!sts && tc != 0) sts = grv_pkt->sendmsg(grv_pkt, MESSAGE(MSG_ADDSUB, 2), tc);
		if (!sts) parms->sprt->data_format_tc[0] = tc;
		if (!sts && tc != 0) grv->sendmsg(grv, MESSAGE(MSG_SPRT_PACKET_DATA, 2), 1);
	}
 } else if (parms->direction == kSMSPRTDataDirectionRx) {
	switch (parms->format) {
	case kSMSPRTDataFormatNone:
		break;
	case kSMSPRTDataFormatIOctet:
		tc = SUBTASKTC_IOCTET_DEPACKETISER;
		break;
	case kSMSPRTDataFormatIOctetCS:
		tc = SUBTASKTC_IOCTET_CS_DEPACKETISER;
		break;
	case kSMSPRTDataFormatIRawOctet:
		tc = SUBTASKTC_IRAW_OCTET_DEPACKETISER;
		break;
	default:
		sts = err(ERR_SM_BAD_PARAMETER);
		break;
	}
	if (!sts && tc != parms->sprt->data_format_tc[1]) {
		 if (parms->sprt->data_format_tc[1]) sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), parms->sprt->data_format_tc[1]);
		 if (!sts && tc != 0) sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), tc);
		 if (!sts) parms->sprt->data_format_tc[1] = tc;
	}
 } else {
	 sts = err(ERR_SM_BAD_PARAMETER);
 }
 unlock_object(parms->sprt);
 return sts;
}

STATIC int real_sm_sprt_datafeed_connect(struct sm_sprt_datafeed_connect_parms *parms)
{
 tSMSPRTId sprt = parms->sprt;
 tSMGroove *grv;
 DATAFEED df;
 int sts;
 lock_object(sprt);
 grv = &sprt->groove_packetiser; // input data to be packetised
 df.vtbl_data = parms->data_source;
 sts = grv->connect_df(grv, &sprt->dfin, df);
 if (!sts) sts = grv->engage_df(grv, df, DFC_IN);
 unlock_object(sprt);
 return sts;
}

STATIC int real_sm_sprt_get_datafeed(struct sm_sprt_get_datafeed_parms *parms)
{
 tSMSPRTId sprt = parms->sprt;
 tSMGroove *grv;
 int sts;
 lock_object(sprt);
 grv = &sprt->groove;
 sts = grv->get_df(grv,sprt->dataf, sprt->mod);
 if (!sts) parms->datafeed = sprt->dataf.vtbl_data;
 unlock_object(sprt);
 return sts;
}

STATIC int real_sm_sprt_read_message(struct sm_sprt_read_message_parms *parms)
{
 int sts = 0;
 uint32_t len;
 tSMGroove *grv;
 lock_object(parms->sprt);
 grv = &parms->sprt->groove;
 sts = grv->rdmsgstring(grv, MSGSTRING_ID(MSGSTRING_SPRT, 0), 0xffffffffu, parms->max_length, parms->message, &len);
 if (!sts) {
	 if (len == ~0u) len = 0; // or should there be an error
	 parms->length = len;
 }
 unlock_object(parms->sprt);
 return sts;
}

STATIC int real_sm_sprt_send_message(struct sm_sprt_send_message_parms *parms)
{
 int sts = 0;
 tSMGroove *grv;
 lock_object(parms->sprt);
 grv = &parms->sprt->groove;
 if (parms->length > 256) // absolute max length for channel 2
	 sts = err(ERR_SM_BAD_DATA_LENGTH);
 else
	sts = grv->sendmsgstring(grv, MSGSTRING_ID(MSGSTRING_SPRT, 1), parms->length, parms->message);
 unlock_object(parms->sprt);
 return sts;
}

#include "../pubdoc/gen/prossprt.i"
