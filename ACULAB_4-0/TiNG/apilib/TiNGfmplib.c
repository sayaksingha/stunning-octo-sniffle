/* TiNGfmplib.c */

#include "TiNGo_fmprx.h"
#include "TiNGo_fmptx.h"
#include "TiNGo_datafeed.h"
#include "smfmp.h"
#include "cardconn_px.h"
#include "../hsif/kernel.h"
#include "../hsif/kernel_x.h"
#include "../hsif/subtask.h"
#include "../hsif/typecode.h"
#include "../hsif/ifp.h"
#include "../hsif/ipv6.h"
#include "../hsif/t38.h"
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

STATIC int real_sm_fmprx_create(struct sm_fmprx_create_parms *fmprxp)
{
 struct module *mod;
 tSMFMPrxId fmprx;
 int sts = 0;
 if (fmprxp->type != kSMFMPrxTypeIPv4 && fmprxp->type != kSMFMPrxTypeIPv6) return err(ERR_SM_BAD_PARAMETER);
 mod = modid2lockedmodule(fmprxp->module);
 if (!mod) return err(ERR_SM_NO_SUCH_MODULE);
 fmprx = init_fmprx(mod);
 if (!fmprx) sts = err(ERR_SM_NO_RESOURCES);
 else {
	tSMGroove *grv = &fmprx->groove;
	fmprxp->fmprx = fmprx;
	sts = grv->starttask(grv, TASKTC_FMPRX, 0);
	if (fmprxp->type == kSMFMPrxTypeIPv4) {
		if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_SET_INIPV4, 2), ip2msg(fmprxp->address.s_addr));
	} else { // IPv6
		if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_SET_INIPV6, 5), ipv6tomsg(fmprxp->ipv6_address));
	}
	if (!sts) {
		sts = mod->card->vtbl->setup_df(fmprx->dataf, mod, 0); // Datafeed type is TX even for fmprx
		if (!sts) sts = grv->engage_df(grv, fmprx->dataf, DFC_OUT);
	}
	fmprx->state = FMPRX_STATE_RUNNING;
	if (sts) fmprx_dtor(fmprx);
	else make_object(fmprx);
 }
 unlock_module(mod);
 return sts;
}

STATIC int real_sm_fmprx_destroy(tSMFMPrxId fmprx)
{
 if (fmprx) fmprx_dtor(fmprx);
 return 0;
}

STATIC int real_sm_fmprx_get_datafeed(struct sm_fmprx_datafeed_parms *datafeedp)
{
 tSMFMPrxId fmprx = datafeedp->fmprx;
 lock_object(fmprx);
 datafeedp->datafeed = fmprx->dataf.vtbl_data;
 unlock_object(fmprx);
 return 0;
}

STATIC int real_sm_fmprx_get_event(struct sm_fmprx_event_parms *eventp)
{
 int sts;
 lock_object(eventp->fmprx);
 sts = eventp->fmprx->groove.getev(&eventp->fmprx->groove, &eventp->event);
 unlock_object(eventp->fmprx);
 return sts;
}

STATIC int unsafe_sm_fmprx_status(struct sm_fmprx_status_parms *statusp)
{
	int sts;
	tSMFMPrxId fmprx = statusp->fmprx;
	tSMGroove *grv = &fmprx->groove;
	uint32_t msg[16];
	unsigned long code;
	if (fmprx->state == FMPRX_STATE_IDLE) {
		return err(ERR_SM_WRONG_CHANNEL_STATE);
	}
	statusp->status = kSMFMPrxStatusRunning;
	sts = grv->rdmsg(grv, msg, SMGROOVE_RDMSG_FLAG_NOWAIT);
	if (sts) return sts;
	code = msg[0] & 0xffff0000;
	if (code == MESSAGE(MSG_GET_UDPPORTS, 3)) {
		statusp->u.port.T38_port = msg[1];
		statusp->u.port.address.s_addr = msg2ip(msg[2]);
		statusp->status = kSMFMPrxStatusGotPorts;
	} else if (code == MESSAGE(MSG_GET_UDPPORTS, 6)) {
		statusp->u.port_ipv6.T38_port = msg[1];
		msgtoipv6(&statusp->u.port_ipv6.address, msg[2], msg[3], msg[4], msg[5]);
		statusp->status = kSMFMPrxStatusGotPortsIPv6;
	} else if (code == MESSAGE(MSG_T38SRC, 3)) {
		statusp->u.source.address.s_addr = msg2ip(msg[1]);
		statusp->u.source.port = msg[2];
		statusp->status = kSMFMPrxStatusSource;
	} else if (code == MESSAGE(MSG_T38SRC, 6)) {
		msgtoipv6(&statusp->u.source_ipv6.address, msg[1], msg[2], msg[3], msg[4]);
		statusp->u.source_ipv6.port = msg[5];
		statusp->status = kSMFMPrxStatusSourceIPv6;
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
		fmprx->state = FMPRX_STATE_IDLE;
		statusp->status = kSMFMPrxStatusStopped;
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
		} else if ((msg[1] & 0xffff0000) == MESSAGE(MSG_SET_INIPV4, 2)) {
			if (msg[2] == MSG_BADMSG_CAUSE_RANGE) {
				return err(ERR_SM_BAD_PARAMETER);
			}
		} else if ((msg[1] & 0xffff0000) == MESSAGE(MSG_SET_INIPV6, 5)) {
			if (msg[2] == MSG_BADMSG_CAUSE_RANGE) {
				return err(ERR_SM_BAD_PARAMETER);
			}
		}
	} else if (code == MESSAGE(MSG_GETLIC_INFO, 2)) {
		if (msg[1] == 0) {
			return err(ERR_SM_NO_LICENCE);
		}
	}
	return sts;
}

STATIC int real_sm_fmprx_status(struct sm_fmprx_status_parms *statusp)
{
	int sts;
	tSMFMPrxId fmprx = statusp->fmprx;
	lock_object(fmprx);
	sts = unsafe_sm_fmprx_status(statusp);
	unlock_object(fmprx);
	return sts;
}

STATIC int real_sm_fmprx_stop(struct sm_fmprx_stop_parms *stopp)
{
 int sts = 0;
 if (stopp->fmprx) {
	lock_object(stopp->fmprx);
	if (stopp->fmprx->state != FMPRX_STATE_RUNNING) {
		sts = err(ERR_SM_WRONG_CHANNEL_STATE);
	}
	if (!sts) sts = stopp->fmprx->groove.sendmsg(&stopp->fmprx->groove, MESSAGE(MSG_DEAD, 1));
	if (!sts) stopp->fmprx->state = FMPRX_STATE_STOPPING;
	unlock_object(stopp->fmprx);
 }
 return sts;
}

STATIC int real_sm_fmptx_config(struct sm_fmptx_config_parms *configp)
{
 const tSMFMPtxId fmptx = configp->fmptx;
 int sts = 0;
 tSMGroove *grv;
 if (configp->Recovery != kSMFMPRecoveryTypeRED) {
	 return err(ERR_SM_NOT_IMPLEMENTED);
 }
 lock_object(fmptx);
 grv = &fmptx->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_SET_OUTIPV4, 5), ip2msg(configp->destination.sin_addr.s_addr), ntohs(configp->destination.sin_port), ip2msg(configp->source.sin_addr.s_addr), ntohs(configp->source.sin_port));
 // if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_SET_TOS, 2), configp->TOS);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_FMP_RECOVERY, 3), configp->RecoveryLevel, configp->Recovery);// configuring the recovery type and level
 unlock_object(fmptx);
 return sts;
}

STATIC int real_sm_fmptx_config_ipv6(struct sm_fmptx_config_ipv6_parms *configp)
{
 const tSMFMPtxId fmptx = configp->fmptx;
 int sts = 0;
 tSMGroove *grv;
 if (configp->Recovery != kSMFMPRecoveryTypeRED) {
	 return err(ERR_SM_NOT_IMPLEMENTED);
 }
 lock_object(fmptx);
 grv = &fmptx->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_SET_OUTIPV6, 11),
	ipv6tomsg(configp->destination.sin6_addr),
	ntohs(configp->destination.sin6_port),
	ipv6tomsg(configp->source.sin6_addr),
	ntohs(configp->source.sin6_port));
 // if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_SET_TOS, 2), (ntohl(configp->destination.sin6_flowinfo) >> 20 ) & 0xffu);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_FMP_RECOVERY, 3), configp->RecoveryLevel, configp->Recovery);// configuring the recovery type and level
 unlock_object(fmptx);
 return sts;
}

STATIC int real_sm_fmptx_create(struct sm_fmptx_create_parms *fmptxp)
{
 struct module *mod = modid2lockedmodule(fmptxp->module);
 tSMFMPtxId fmptx;
 int sts = 0;
 if (!mod) return err(ERR_SM_NO_SUCH_MODULE);
 fmptx = init_fmptx(mod);
 if (!fmptx) sts = err(ERR_SM_NO_RESOURCES);
 else {
 	fmptxp->fmptx = fmptx;
	sts = fmptx->groove.starttask(&fmptx->groove, TASKTC_FMPTX, 0);
	fmptxp->fmptx->state = FMPTX_STATE_RUNNING;
	if (sts) fmptx_dtor(fmptx);
	else make_object(fmptx);
 }
 unlock_module(mod);
 return sts;
}

STATIC int real_sm_fmptx_datafeed_connect(struct sm_fmptx_datafeed_connect_parms *datafeedp)
{
 tSMFMPtxId fmptx = datafeedp->data_dest;
 DATAFEED df;
 tSMGroove *grv;
 int sts;
 lock_object(fmptx);
 grv = &fmptx->groove;
 df.vtbl_data = datafeedp->data_source;
 sts = grv->connect_df(grv, &fmptx->dfin, df);
 if (!sts) sts = grv->engage_df(grv, df, DFC_IN);
 unlock_object(fmptx);
 return sts;
}

STATIC int real_sm_fmptx_destroy(tSMFMPtxId fmptx)
{
 if (fmptx) fmptx_dtor(fmptx);
 return 0;
}

STATIC int real_sm_fmptx_get_event(struct sm_fmptx_event_parms *eventp)
{
 int sts;
 lock_object(eventp->fmptx);
 sts = eventp->fmptx->groove.getev(&eventp->fmptx->groove, &eventp->event);
 unlock_object(eventp->fmptx);
 return sts;
}

STATIC int unsafe_sm_fmptx_status(struct sm_fmptx_status_parms *statusp)
{
	int sts;
	tSMFMPtxId fmptx = statusp->fmptx;
	tSMGroove *grv = &fmptx->groove;
	uint32_t msg[16];
	unsigned long code;
	if (fmptx->state == FMPTX_STATE_IDLE) {
		return err(ERR_SM_WRONG_CHANNEL_STATE);
	}
	statusp->status = kSMFMPtxStatusRunning;
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
			case MSG_DEAD_CAUSE_LICENCE:
				return err(ERR_SM_NO_LICENCE);
			}
		}
		fmptx->state = FMPTX_STATE_IDLE;
		statusp->status = kSMFMPtxStatusStopped;
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
	} else if (code == MESSAGE(MSG_GETLIC_INFO, 2)) {
		if (msg[1] == 0) {
			return err(ERR_SM_NO_LICENCE);
		}
	}
	return sts;
}

STATIC int real_sm_fmptx_status(struct sm_fmptx_status_parms *statusp)
{
	int sts;
	tSMFMPtxId fmptx = statusp->fmptx;
	lock_object(fmptx);
	sts = unsafe_sm_fmptx_status(statusp);
	unlock_object(fmptx);
	return sts;
}

STATIC int real_sm_fmptx_stop(struct sm_fmptx_stop_parms *stopp)
{
 int sts = 0;
 if (stopp->fmptx) {
	lock_object(stopp->fmptx);
	if (stopp->fmptx->state != FMPTX_STATE_RUNNING) {
		sts = err(ERR_SM_WRONG_CHANNEL_STATE);
	}
	if (!sts) sts = stopp->fmptx->groove.sendmsg(&stopp->fmptx->groove, MESSAGE(MSG_DEAD, 1));
	if (!sts) stopp->fmptx->state = FMPTX_STATE_STOPPING;
	unlock_object(stopp->fmptx);
 }
 return sts;
}

#include "../pubdoc/gen/prosfmpapi.i"
