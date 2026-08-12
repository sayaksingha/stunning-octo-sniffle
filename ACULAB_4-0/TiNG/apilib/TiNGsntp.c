/* TiNGsntp.c */

#include "TiNGo_sntp.h"
#include "TiNGo_datafeed.h"
#include "smsntp.h"
#include "cardconn_px.h"
#include "../hsif/kernel.h"
#include "../hsif/subtask.h"
#include "../hsif/typecode.h"
#include "../hsif/vmp.h"
#include "../hsif/ipv6.h"
#include "../hsif/sntp.h"
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

STATIC int real_sm_sntp_create(struct sm_sntp_create_parms *sntpp)
{
 struct module *mod;
 tSMSNTPId sntp;
 int sts = 0;
 if (sntpp->type != kSMSNTPTypeIPv4 && sntpp->type != kSMSNTPTypeIPv6) return err(ERR_SM_BAD_PARAMETER);
 mod = modid2lockedmodule(sntpp->module);
 if (!mod) return err(ERR_SM_NO_SUCH_MODULE);
 sntp = init_sntp(mod);
 if (!sntp) sts = err(ERR_SM_NO_RESOURCES);
 else {
	tSMGroove *grv = &sntp->groove;
	sts = grv->starttask(grv, TASKTC_SNTP, 0);
	if (sntpp->type == kSMSNTPTypeIPv4) {
		if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_SET_INIPV4, 2), ip2msg(sntpp->address.s_addr));
	} else { // IPv6
		if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_SET_INIPV6, 5), ipv6tomsg(sntpp->ipv6_address));
	}
	if (sts) sntp_dtor(sntp);
	else {
		make_object(sntp);
		sntpp->sntp = sntp;
	}
 }
 unlock_module(mod);
 return sts;
}

STATIC int real_sm_sntp_destroy(tSMSNTPId sntp)
{
 if (sntp) sntp_dtor(sntp);
 return 0;
}

STATIC int real_sm_sntp_get_event(struct sm_sntp_event_parms *eventp)
{
 int sts;
 lock_object(eventp->sntp);
 sts = eventp->sntp->groove.getev(&eventp->sntp->groove, &eventp->event);
 unlock_object(eventp->sntp);
 return sts;
}

STATIC int unsafe_sm_sntp_status(struct sm_sntp_status_parms *statusp)
{
 int sts;
 tSMSNTPId sntp = statusp->sntp;
 tSMGroove *grv = &sntp->groove;
 uint32_t msg[16];
 unsigned long code;
 statusp->status = kSMSNTPStatusRunning;
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
	statusp->status = kSMSNTPStatusStopped;
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
 } else if (code == MESSAGE(MSG_SNTP_SYNC, 2)) {
	statusp->status = kSMSNTPStatusSync;
	statusp->u.sync.has_sync = msg[1];
 }
 return sts;
}

STATIC int real_sm_sntp_status(struct sm_sntp_status_parms *statusp)
{
 int sts;
 tSMSNTPId sntp = statusp->sntp;
 lock_object(sntp);
 sts = unsafe_sm_sntp_status(statusp);
 unlock_object(sntp);
 return sts;
}


STATIC int real_sm_sntp_config(struct sm_sntp_config_parms *configp)
{
 int sts = 0;
 tSMGroove *grv;
 if (configp->no_reply_delay > configp->had_reply_delay) return err(ERR_SM_BAD_PARAMETER);
 lock_object(configp->sntp);
 grv = &configp->sntp->groove;
 sts = grv->sendmsg(grv, 
			MESSAGE(MSG_SNTP_CONFIG, 6),
			ip2msg(configp->server.sin_addr.s_addr),
			ntohs(configp->server.sin_port),
			configp->TOS,
			configp->no_reply_delay,
			configp->had_reply_delay);
 unlock_object(configp->sntp);
 return sts;
}

STATIC int real_sm_sntp_config_ipv6(struct sm_sntp_config_ipv6_parms *configp)
{
 int sts = 0;
 tSMGroove *grv;
 if (configp->no_reply_delay > configp->had_reply_delay) return err(ERR_SM_BAD_PARAMETER);
 lock_object(configp->sntp);
 grv = &configp->sntp->groove;
 sts = grv->sendmsg(grv, 
			MESSAGE(MSG_SNTP_CONFIG, 9),
			ipv6tomsg(configp->server.sin6_addr),
			ntohs(configp->server.sin6_port),
			(ntohl(configp->server.sin6_flowinfo) >> 20 ) & 0xffu,
			configp->no_reply_delay,
			configp->had_reply_delay);
 unlock_object(configp->sntp);
 return sts;
}
#include "../pubdoc/gen/prossntp.i"
