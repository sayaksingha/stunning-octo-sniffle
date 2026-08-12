/* TiNGdtls.c */

#include "TiNGo_dtlsrx.h"
#include "TiNGo_dtlstx.h"
#include "TiNGo_vmprx.h"
#include "smdtls.h"
#include "cardconn_px.h"
#include "../hsif/kernel.h"
#include "../hsif/kernel_x.h"
#include "../hsif/subtask.h"
#include "../hsif/typecode.h"
#include "../hsif/hbuf.h"
#include "../hsif/ipv6.h"
#include "../hsif/vmp.h"
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

//dtls_usage bitmask values
#define VMPRX_DTLS_USAGE_RTP 0x1
#define VMPRX_DTLS_USAGE_RTCP 0x2

STATIC int real_sm_vmprx_config_dtls(struct sm_vmprx_config_dtls_parms *configp)
{
	const tSMVMPrxId vmprx = configp->vmprx;
	tSMGroove *grv;
	int sts = 0;
	unsigned usage = (configp->use_RTCP_port == 0) ? VMPRX_DTLS_USAGE_RTP : VMPRX_DTLS_USAGE_RTCP;
	if (configp->dtlsrx) {
		lock_module(vmprx->mod);
		lock_object(configp->dtlsrx);
	}
	lock_object(vmprx);
	grv = &vmprx->groove;
	vmprx->dtls_usage &= ~usage;
	if (vmprx->dtls_usage == 0) {
		sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), SUBTASKTC_DTLSRX_GRABBER);
	} else {
		sts = grv->setfriend(grv, NULL, (configp->use_RTCP_port == 0) ? FRIEND_TYPE_DTLSRX : FRIEND_TYPE_DTLSRX_RTCP);
	}
	if (configp->dtlsrx) {
		if (!sts && vmprx->dtls_usage == 0) {
			sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_DTLSRX_GRABBER);
			if (!sts) vmprx->dtls_usage |= usage;
		}
		if (!sts) sts = grv->setfriend(grv, &configp->dtlsrx->groove, (configp->use_RTCP_port == 0) ? FRIEND_TYPE_DTLSRX : FRIEND_TYPE_DTLSRX_RTCP);
	}
	unlock_object(vmprx);
	if (configp->dtlsrx) {
		unlock_object(configp->dtlsrx);
		unlock_module(vmprx->mod);
	}
	return sts;
}

STATIC int real_sm_dtlsrx_create(struct sm_dtlsrx_create_parms *dtlsrxp)
{
	struct module *mod;
	tSMDTLSrxId dtlsrx;
	int sts = 0;
	mod = modid2lockedmodule(dtlsrxp->module);
	if (!mod) return err(ERR_SM_NO_SUCH_MODULE);
	dtlsrx = init_dtlsrx(mod);
	if (!dtlsrx) sts = err(ERR_SM_NO_RESOURCES);
	else {
		tSMGroove *grv = &dtlsrx->groove;
		dtlsrxp->dtlsrx = dtlsrx;
		sts = grv->starttask(grv, TASKTC_DTLSRX, 0);
		dtlsrx->state = DTLSRX_STATE_RUNNING;
		if (!sts) {
			sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_HBUFTOHOST);
 			if (!sts) {
 				S32 minimum_bits = 2048 * 8;
 				U32 timeout_milliseconds = 0;
 				sts = cx_setxfer(grv->cardconn_cx(grv), minimum_bits, timeout_milliseconds);
 			}
		}
		if (sts) dtlsrx_dtor(dtlsrx);
		else make_object(dtlsrx);
	}
	unlock_module(mod);
	return sts;
}

STATIC int real_sm_dtlsrx_destroy(tSMDTLSrxId dtlsrx)
{
	if (dtlsrx) dtlsrx_dtor(dtlsrx);
	return 0;
}

STATIC int real_sm_dtlsrx_get_event(struct sm_dtlsrx_event_parms *eventp)
{
	int sts;
	lock_object(eventp->dtlsrx);
	sts = eventp->dtlsrx->groove.getev(&eventp->dtlsrx->groove, &eventp->event);
	unlock_object(eventp->dtlsrx);
	return sts;
}

STATIC int real_sm_dtlsrx_get_packet(struct sm_dtlsrx_get_packet_parms *readp)
{
	int sts = 0;
	tSMGroove *grv;
	lock_object(readp->dtlsrx);
	grv = &readp->dtlsrx->groove;
	sts = grv->read(grv, readp->data, readp->max_length, (unsigned*)&readp->packet_length);
	unlock_object(readp->dtlsrx);
	return sts;
}

STATIC int unsafe_sm_dtlsrx_status(struct sm_dtlsrx_status_parms *statusp)
{
	int sts;
	tSMDTLSrxId dtlsrx = statusp->dtlsrx;
	tSMGroove *grv = &dtlsrx->groove;
	uint32_t msg[16];
	unsigned long code;
	uint32_t capacity;
	unsigned has_msg;
	if (dtlsrx->state == DTLSRX_STATE_IDLE) {
		return err(ERR_SM_WRONG_CHANNEL_STATE);
	}
	statusp->status = kSMDTLSrxStatusRunning;
	sts = grv->rdmsg_ex(grv, msg, SMGROOVE_RDMSG_FLAG_NOWAIT, &capacity, &has_msg);
	if (sts) return sts;
	code = msg[0] & 0xffff0000;
	if (code == 0 || code == MESSAGE(MSG_BUF_FLUSH, 1)) {
		if (capacity > 0 && has_msg) {
			statusp->status = kSMDTLSrxStatusHasPacket;
		}
	} else if (MSG2ANYCODE(code) == MSG_DEAD) {
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
			case MSG_DEAD_CAUSE_LICENCE:
				return err(ERR_SM_NO_LICENCE);
			}
		}
		dtlsrx->state = DTLSRX_STATE_IDLE;
		statusp->status = kSMDTLSrxStatusStopped;
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
		}
	}
	return sts;
}

STATIC int real_sm_dtlsrx_status(struct sm_dtlsrx_status_parms *statusp)
{
	int sts;
	tSMDTLSrxId dtlsrx = statusp->dtlsrx;
	lock_object(dtlsrx);
	sts = unsafe_sm_dtlsrx_status(statusp);
	unlock_object(dtlsrx);
	return sts;
}

STATIC int real_sm_dtlsrx_stop(struct sm_dtlsrx_stop_parms *stopp)
{
	int sts = 0;
	if (stopp->dtlsrx) {
		lock_object(stopp->dtlsrx);
		if (stopp->dtlsrx->state != DTLSRX_STATE_RUNNING) {
			sts = err(ERR_SM_WRONG_CHANNEL_STATE);
		}
		if (!sts) sts = stopp->dtlsrx->groove.sendmsg(&stopp->dtlsrx->groove, MESSAGE(MSG_DEAD, 1));
		if (!sts) stopp->dtlsrx->state = DTLSRX_STATE_STOPPING;
		unlock_object(stopp->dtlsrx);
	}
	return sts;
}

STATIC int real_sm_dtlstx_config(struct sm_dtlstx_config_parms *configp)
{
	const tSMDTLStxId dtlstx = configp->dtlstx;
	int sts = 0;
	tSMGroove *grv;
	lock_object(dtlstx);
	grv = &dtlstx->groove;
	sts = grv->sendmsg(grv, MESSAGE(MSG_SET_OUTIPV4, 5), ip2msg(configp->destination.sin_addr.s_addr), ntohs(configp->destination.sin_port), ip2msg(configp->source.sin_addr.s_addr), ntohs(configp->source.sin_port));
	// if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_SET_TOS, 2), configp->TOS);
	unlock_object(dtlstx);
	return sts;
}

STATIC int real_sm_dtlstx_config_ipv6(struct sm_dtlstx_config_ipv6_parms *configp)
{
	const tSMDTLStxId dtlstx = configp->dtlstx;
	int sts = 0;
	tSMGroove *grv;
	lock_object(dtlstx);
	grv = &dtlstx->groove;
	sts = grv->sendmsg(grv, MESSAGE(MSG_SET_OUTIPV6, 11),
		ipv6tomsg(configp->destination.sin6_addr),
		ntohs(configp->destination.sin6_port),
		ipv6tomsg(configp->source.sin6_addr),
		ntohs(configp->source.sin6_port));
	// if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_SET_TOS, 2), (ntohl(configp->destination.sin6_flowinfo) >> 20 ) & 0xffu);
	unlock_object(dtlstx);
	return sts;
}

STATIC int real_sm_dtlstx_create(struct sm_dtlstx_create_parms *dtlstxp)
{
	struct module *mod = modid2lockedmodule(dtlstxp->module);
	tSMDTLStxId dtlstx;
	int sts = 0;
	if (!mod) return err(ERR_SM_NO_SUCH_MODULE);
	dtlstx = init_dtlstx(mod);
	if (!dtlstx) sts = err(ERR_SM_NO_RESOURCES);
	else {
		dtlstxp->dtlstx = dtlstx;
		sts = dtlstx->groove.starttask(&dtlstx->groove, TASKTC_DTLSTX, 0);
		dtlstxp->dtlstx->state = DTLSTX_STATE_RUNNING;
		if (!sts) {
			sts = dtlstx->groove.sendmsg(&dtlstx->groove, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_HBUFFROMHOST);
 			if (!sts) {
 				S32 minimum_bits = dtlstx->min_bits;
 				U32 timeout_milliseconds = 0;
 				sts = cx_setxfer(dtlstx->groove.cardconn_cx(&dtlstx->groove), minimum_bits, timeout_milliseconds);
 			}
		}
		if (sts) dtlstx_dtor(dtlstx);
		else make_object(dtlstx);
	}
	unlock_module(mod);
	return sts;
}

STATIC int real_sm_dtlstx_config_notify(struct sm_dtlstx_config_notify_parms* configp)
{
	const tSMDTLStxId dtlstx = configp->dtlstx;
	int sts = 0;
	tSMGroove *grv;
 	S32 minimum_bits = configp->packet_length * 8;
 	U32 timeout_milliseconds = 0;
	lock_object(dtlstx);
	dtlstx->min_bits = minimum_bits;
	grv = &dtlstx->groove;
	if (configp->no_capacity_event) minimum_bits = 0;
 	sts = cx_setxfer(grv->cardconn_cx(grv), minimum_bits, timeout_milliseconds);
	unlock_object(dtlstx);
	return sts;
}

STATIC int real_sm_dtlstx_destroy(tSMDTLStxId dtlstx)
{
	if (dtlstx) dtlstx_dtor(dtlstx);
	return 0;
}

STATIC int real_sm_dtlstx_get_event(struct sm_dtlstx_event_parms *eventp)
{
	int sts;
	lock_object(eventp->dtlstx);
	sts = eventp->dtlstx->groove.getev(&eventp->dtlstx->groove, &eventp->event);
	unlock_object(eventp->dtlstx);
	return sts;
}

STATIC int real_sm_dtlstx_send_packet(struct sm_dtlstx_send_packet_parms *sendp)
{
	int sts = 0;
	unsigned done;
	unsigned len = sendp->packet_length + 4;
	tSMGroove *grv;
	char pkt[2048];
	if (sendp->packet_length > 1500) return err(ERR_SM_BAD_PARAMETER);
	memcpy(&pkt[4], sendp->data, sendp->packet_length);
	pkt[0] = (sendp->packet_length >> 24) & 0xff; 
	pkt[1] = (sendp->packet_length >> 16) & 0xff; 
	pkt[2] = (sendp->packet_length >> 8) & 0xff; 
	pkt[3] = sendp->packet_length & 0xff; 
	lock_object(sendp->dtlstx);
	grv = &sendp->dtlstx->groove;
	sts = grv->write(grv, pkt, len, &done);
	if (!sts && done && done != len) {
		// if done is non-zero and not equal to len
		// then only part of the data has been written.
		// this partial data needs to be discarded
		// NB this may also discard previous packets!
		sts = grv->sendmsg(grv, MESSAGE(MSG_DISCARD_DATA, 1));
		if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_RESUME_DATA, 1));
	}
	unlock_object(sendp->dtlstx);
	return sts;
}

STATIC int unsafe_sm_dtlstx_status(struct sm_dtlstx_status_parms *statusp)
{
	int sts;
	tSMDTLStxId dtlstx = statusp->dtlstx;
	tSMGroove *grv = &dtlstx->groove;
	uint32_t msg[16];
	unsigned long code;
	uint32_t capacity;
	unsigned has_msg;
	if (dtlstx->state == DTLSTX_STATE_IDLE) {
		return err(ERR_SM_WRONG_CHANNEL_STATE);
	}
	statusp->status = kSMDTLStxStatusRunning;
	sts = grv->rdmsg_ex(grv, msg, SMGROOVE_RDMSG_FLAG_NOWAIT, &capacity, &has_msg);
	if (sts) return sts;
	if (capacity > dtlstx->min_bits) {
		statusp->status = kSMDTLStxStatusHasCapacity;
	}
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
			case MSG_DEAD_CAUSE_LICENCE:
				return err(ERR_SM_NO_LICENCE);
			}
		}
		dtlstx->state = DTLSTX_STATE_IDLE;
		statusp->status = kSMDTLStxStatusStopped;
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
		}
	}
	return sts;
}

STATIC int real_sm_dtlstx_status(struct sm_dtlstx_status_parms *statusp)
{
	int sts;
	tSMDTLStxId dtlstx = statusp->dtlstx;
	lock_object(dtlstx);
	sts = unsafe_sm_dtlstx_status(statusp);
	unlock_object(dtlstx);
	return sts;
}

STATIC int real_sm_dtlstx_stop(struct sm_dtlstx_stop_parms *stopp)
{
	int sts = 0;
	if (stopp->dtlstx) {
		lock_object(stopp->dtlstx);
		if (stopp->dtlstx->state != DTLSTX_STATE_RUNNING) {
			sts = err(ERR_SM_WRONG_CHANNEL_STATE);
		}
		if (!sts) sts = stopp->dtlstx->groove.sendmsg(&stopp->dtlstx->groove, MESSAGE(MSG_DEAD, 1));
		if (!sts) stopp->dtlstx->state = DTLSTX_STATE_STOPPING;
		unlock_object(stopp->dtlstx);
	}
	return sts;
}






STATIC void show_rx_packet_in(struct sm_dtlsrx_get_packet_parms *packetp)
{
	olog(", data=%p)", packetp->data);
}

STATIC void show_tx_packet_in(struct sm_dtlstx_send_packet_parms *packetp)
{
	olog(", data=%p)", packetp->data);
}


#include "../pubdoc/gen/prosdtls.i"
