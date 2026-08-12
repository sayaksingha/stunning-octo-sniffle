/* TiNGrtplib.c */

#include "../hsif/g729.h"
#include "../hsif/g723.h"
#include "../hsif/g722.h"
#include "../hsif/g726.h"
#include "../hsif/ipv6.h"
#include "../hsif/iSAC.h"
#include "../hsif/kernel.h"
#include "../hsif/kernel_x.h"
#include "../hsif/msgstring.h"
#include "../hsif/rtcp.h"
#include "../hsif/samplerate.h"
#include "../hsif/stun.h"
#include "../hsif/subtask.h"
#include "../hsif/tonedet.h"
#include "../hsif/typecode.h"
#include "../hsif/speex_vmp.h"
#include "../hsif/vidmp.h"
#include "../hsif/vmp.h"
#include "TiNGevent.h"
#include "TiNGo_datafeed.h"
#include "TiNGo_rtcph.h"
#include "TiNGo_sprt.h"
#include "TiNGo_vmpcsrc.h"
#include "TiNGo_vmprx.h"
#include "TiNGo_vidmprx.h"
#include "TiNGo_vmptone.h"
#include "TiNGo_vmptx.h"
#include "TiNGo_vidmptx.h"
#include "cardconn_px.h"
#include "smrtp.h"
#include "timestamp.h"
#include "trace.h"
#include "trace_rtp_show.h"
#include "TiNGcommon.h"

#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include <math.h>

#ifdef TiNGTYPE_LINUX
#include "netinet/in.h"
#endif

#ifdef TiNGTYPE_QNX
#include "netinet/in.h"
#endif


#ifdef TiNGTYPE_WINNT
#include "../libutil/WINNT/wind.h"
#endif

STATIC int real_sm_rtcphand_create(struct sm_rtcphand_create_parms *rtcphandp)
{
 struct module *mod = modid2lockedmodule(rtcphandp->module);
 tSMRTCPHandId rtcphand;
 int sts = 0;
 if (!mod) return err(ERR_SM_NO_SUCH_MODULE);
 rtcphand = init_rtcphand(mod);
 if (!rtcphand) sts = err(ERR_SM_NO_RESOURCES);
 rtcphandp->rtcphand = rtcphand;
 if (!sts) {
	tSMGroove *grv = &rtcphand->groove;
	sts = grv->starttask(grv, TASKTC_RTCPHAND, 0);
	if (sts) rtcphand_dtor(rtcphand);
	else make_object(rtcphand);
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_HBUFTOHOST);
 }
 unlock_module(mod);
 return sts;
}

STATIC int real_sm_vmprx_create(struct sm_vmprx_create_parms *vmprxp)
{
 struct module *mod;
 tSMVMPrxId vmprx;
 int sts = 0;
 if (vmprxp->type != kSMVMPrxTypeIPv4 && vmprxp->type != kSMVMPrxTypeIPv6) return err(ERR_SM_BAD_PARAMETER);
 mod = modid2lockedmodule(vmprxp->module);
 if (!mod) return err(ERR_SM_NO_SUCH_MODULE);
 vmprx = init_vmprx(mod);
 if (!vmprx) sts = err(ERR_SM_NO_RESOURCES);
 vmprxp->vmprx = vmprx;
 if (!sts) {
	tSMGroove *grv = &vmprx->groove;
	sts = grv->starttask(grv, TASKTC_VMPRX, 0);
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADDVMPSUB, 2), SUBTASKTC_VMPRXJB);
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADDVMPSUB, 2), SUBTASKTC_VMPRXHI);
	if (!sts && vmprxp->mux_rtcp) grv->sendmsg(grv, MESSAGE(MSG_SET_RTCP_MUX, 1));
	if (vmprxp->type == kSMVMPrxTypeIPv4) {
		vmprx->is_ipv6 = 0;
		if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_SET_INIPV4, 2), ip2msg(vmprxp->address.s_addr));
	} else { // IPv6
		vmprx->is_ipv6 = 1;
		if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_SET_INIPV6, 5), ipv6tomsg(vmprxp->ipv6_address));
	}
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_JBCREATE, 4), 10, 50, 100);
	if (!sts) {
		sts = mod->card->vtbl->setup_df(vmprx->dataf, mod, 0); //Datafeed type is TX even for vmprx
		if (!sts) {
			sts = grv->engage_df(grv, vmprx->dataf, DFC_OUT);
		}
	}
	vmprx->state = VMPRX_STATE_RUNNING;
	if (sts) vmprx_dtor(vmprx);
	else make_object(vmprx);
 }
 unlock_module(mod);
 return sts;
}

STATIC int real_sm_vidmprx_create(struct sm_vidmprx_create_parms *vidmprxp)
{
 struct module *mod;
 tSMVidMPrxId vidmprx;
 int sts = 0;
 if (vidmprxp->type != kSMVidMPrxTypeIPv4 && vidmprxp->type != kSMVidMPrxTypeIPv6) return err(ERR_SM_BAD_PARAMETER);
 mod = modid2lockedmodule(vidmprxp->module);
 if (!mod) return err(ERR_SM_NO_SUCH_MODULE);
 vidmprx = init_vidmprx(mod);
 if (!vidmprx) sts = err(ERR_SM_NO_RESOURCES);
 vidmprxp->vidmprx = vidmprx;
 if (!sts) {
	tSMGroove *grv = &vidmprx->groove;
	sts = grv->starttask(grv, TASKTC_VIDMPRX, 0);
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_VIDMPRXJB);
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_VMPRXHI);
	if (vidmprxp->type == kSMVidMPrxTypeIPv4) {
		vidmprx->is_ipv6 = 0;
		if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_SET_INIPV4, 2), ip2msg(vidmprxp->address.s_addr));
	} else { // IPv6
		vidmprx->is_ipv6 = 1;
		if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_SET_INIPV6, 5), ipv6tomsg(vidmprxp->ipv6_address));
	}
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_JBCREATE, 4), 10, 50, 100);
	if (!sts) {
		sts = mod->card->vtbl->setup_df(vidmprx->dataf, mod, 0); //Datafeed type is TX even for vidmprx
		if (!sts) {
			sts = grv->engage_df(grv, vidmprx->dataf, DFC_OUT);
		}
	}
	vidmprx->state = VIDMPRX_STATE_RUNNING;
	if (sts) vidmprx_dtor(vidmprx);
	else make_object(vidmprx);
 }
 unlock_module(mod);
 return sts;
}

STATIC int real_sm_vmptx_create(struct sm_vmptx_create_parms *vmptxp)
{
 struct module *mod = modid2lockedmodule(vmptxp->module);
 tSMVMPtxId vmptx;
 int sts;
 if (!mod) return err(ERR_SM_NO_SUCH_MODULE);
 vmptx = init_vmptx(mod);
 if (!vmptx) sts = err(ERR_SM_NO_RESOURCES);
 else {
	tSMGroove *grv = &vmptx->groove;
	vmptxp->vmptx = vmptx;
	vmptx->state = VMPTX_STATE_RUNNING;
	sts = grv->starttask(grv, TASKTC_VMPTX, 0);
	if (!sts) make_object(vmptx);
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADDVMPSUB, 2), SUBTASKTC_VMPTXHI);
	if (sts) vmptx_dtor(vmptx);
 }
 unlock_module(mod);
 return sts;
}

STATIC int real_sm_vidmptx_create(struct sm_vidmptx_create_parms *vidmptxp)
{
 struct module *mod = modid2lockedmodule(vidmptxp->module);
 tSMVidMPtxId vidmptx;
 int sts;
 if (!mod) return err(ERR_SM_NO_SUCH_MODULE);
 vidmptx = init_vidmptx(mod);
 if (!vidmptx) sts = err(ERR_SM_NO_RESOURCES);
 else {
	tSMGroove *grv = &vidmptx->groove;
	vidmptxp->vidmptx = vidmptx;
	vidmptx->state = VIDMPTX_STATE_RUNNING;
	sts = grv->starttask(grv, TASKTC_VIDMPTX, 0);
	if (!sts) make_object(vidmptx);
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_VMPTXHI);
	if (sts) vidmptx_dtor(vidmptx);
 }
 unlock_module(mod);
 return sts;
}

STATIC int real_sm_vmprx_stop(struct sm_vmprx_stop_parms *stopp)
{
 tSMVMPrxId vmprx = stopp->vmprx;
 int sts = 0;
 lock_object(vmprx);
 if (vmprx->state != VMPRX_STATE_RUNNING) {
	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else {
	tSMGroove *grv = &vmprx->groove;
	sts = grv->sendmsg(grv, MESSAGE(MSG_DEAD, 1));
	if (!sts) vmprx->state = VMPRX_STATE_STOPPING;
 }
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vidmprx_stop(struct sm_vidmprx_stop_parms *stopp)
{
 tSMVidMPrxId vidmprx = stopp->vidmprx;
 int sts = 0;
 lock_object(vidmprx);
 if (vidmprx->state != VIDMPRX_STATE_RUNNING) {
	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else {
	tSMGroove *grv = &vidmprx->groove;
	sts = grv->sendmsg(grv, MESSAGE(MSG_DEAD, 1));
	if (!sts) vidmprx->state = VIDMPRX_STATE_STOPPING;
 }
 unlock_object(vidmprx);
 return sts;
}

STATIC int real_sm_vmptx_stop(struct sm_vmptx_stop_parms *stopp)
{
 tSMVMPtxId vmptx = stopp->vmptx;
 int sts = 0;
 lock_object(vmptx);
 if (vmptx->state != VMPTX_STATE_RUNNING) {
	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else {
	tSMGroove *grv = &vmptx->groove;
	sts = grv->sendmsg(grv, MESSAGE(MSG_DEAD, 1));
	if (!sts) vmptx->state = VMPTX_STATE_STOPPING;
 }
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vidmptx_stop(struct sm_vidmptx_stop_parms *stopp)
{
 tSMVidMPtxId vidmptx = stopp->vidmptx;
 int sts = 0;
 lock_object(vidmptx);
 if (vidmptx->state != VIDMPTX_STATE_RUNNING) {
	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else {
	tSMGroove *grv = &vidmptx->groove;
	sts = grv->sendmsg(grv, MESSAGE(MSG_DEAD, 1));
	if (!sts) vidmptx->state = VIDMPTX_STATE_STOPPING;
 }
 unlock_object(vidmptx);
 return sts;
}

STATIC int real_sm_vidmprx_sync_media(struct sm_vidmprx_sync_media_parms *syncmediap)
{
 tSMVidMPrxId vidmprx = syncmediap->vidmprx;
 tSMVMPrxId vmprx = syncmediap->vmprx;
 int sts = 0;
 if (vmprx) {
	if (vidmprx->mod != vmprx->mod) return err(ERR_SM_NOT_SAME_MODULE);
	lock_module(vidmprx->mod);
	lock_object(vmprx);
 }
 lock_object(vidmprx);
 if (vidmprx->state != VIDMPRX_STATE_RUNNING || (vmprx && vmprx->state != VMPRX_STATE_RUNNING)) {
	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else {
	tSMGroove *grv = &vidmprx->groove;
	tSMGroove *vmprxgrv = (vmprx) ? &vmprx->groove : 0;
	sts = grv->setfriend(grv, vmprxgrv, FRIEND_TYPE_SYNCED_AUDIO_VIDEO_BUFFER);
 }
 unlock_object(vidmprx);
 if (vmprx) {
	unlock_object(vmprx);
	unlock_module(vidmprx->mod);
 }
 return sts;
}

STATIC int real_sm_rtcphand_destroy(tSMRTCPHandId rtcphand)
{
 if (rtcphand) rtcphand_dtor(rtcphand);
 return 0;
}

STATIC int real_sm_vmprx_destroy(tSMVMPrxId vmprx)
{
 if (vmprx) vmprx_dtor(vmprx);
 return 0;
}

STATIC int real_sm_vidmprx_destroy(tSMVidMPrxId vidmprx)
{
 if (vidmprx) vidmprx_dtor(vidmprx);
 return 0;
}

STATIC int real_sm_vmptx_destroy(tSMVMPtxId vmptx)
{
 if (vmptx) vmptx_dtor(vmptx);
 return 0;
}

STATIC int real_sm_vidmptx_destroy(tSMVidMPtxId vidmptx)
{
 if (vidmptx) vidmptx_dtor(vidmptx);
 return 0;
}

STATIC int real_sm_rtcphand_config_bandwidth(struct sm_rtcphand_config_bandwidth_parms *configp)
{
 const tSMRTCPHandId rtcphand = configp->rtcphand;
 tSMGroove *grv;
 int sts = 0;
 lock_object(rtcphand);
 grv = &rtcphand->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_SET_RTCPBW, 4), configp->rtcp_bw, configp->rtcprx_bw, configp->rtcptx_bw);
 unlock_object(rtcphand);
 return sts;
}

STATIC int real_sm_rtcphand_config_reports(struct sm_rtcphand_config_reports_parms *configp)
{
 const tSMRTCPHandId rtcphand = configp->rtcphand;
 tSMGroove *grv;
 int sts;
 lock_object(rtcphand);
 grv = &rtcphand->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_RTCP_REPORTS, 2), configp->reports);
 unlock_object(rtcphand);
 return sts;
}

STATIC int real_sm_rtcphand_config_options(struct sm_rtcphand_config_options_parms *configp)
{
 const tSMRTCPHandId rtcphand = configp->rtcphand;
 tSMGroove *grv;
 int sts;
 lock_object(rtcphand);
 grv = &rtcphand->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_RTCP_OPTIONS, 2), configp->rtcptx_suppress_xr);
 unlock_object(rtcphand);
 return sts;
}

STATIC int real_sm_rtcphand_config_sdes(struct sm_rtcphand_config_sdes_parms *configp)
{
 const tSMRTCPHandId rtcphand = configp->rtcphand;
 tSMGroove *grv;
 unsigned id;
 int sts = 0;
 lock_object(rtcphand);
 grv = &rtcphand->groove;
 id = MSGSTRING_ID(MSGSTRING_RTCP, configp->itemnum);
 sts = grv->sendmsgstring(grv, id, configp->stringlen, configp->stringval);
 unlock_object(rtcphand);
 return sts;
}

STATIC int real_sm_rtcphand_request_statistics(struct sm_rtcphand_request_statistics_parms *pp)
{
 const tSMRTCPHandId rtcphand = pp->rtcphand;
 tSMGroove *grv;
 int sts = 0;
 lock_object(rtcphand);
 grv = &rtcphand->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_RTCP_GETSTATS, 2), pp->statcode);
 unlock_object(rtcphand);
 return sts;
}

STATIC int real_sm_vmprx_set_rtcphand(struct sm_vmprx_set_rtcphand_parms *vap)
{
 tSMGroove *grv;
 tSMGroove *rtcpgrv;
 int sts = 0;
 if (vap->rtcphand) {
	lock_module(vap->vmprx->mod);
	lock_object(vap->rtcphand);
	rtcpgrv = &vap->rtcphand->groove;
 } else {
	rtcpgrv = 0;
 }
 lock_object(vap->vmprx);
 grv = &vap->vmprx->groove;
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), SUBTASKTC_RTCPRXPKTS);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), SUBTASKTC_RTCPRXSTATS);
 if (rtcpgrv) {
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADDVMPSUB, 2), SUBTASKTC_RTCPRXPKTS);
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADDVMPSUB, 2), SUBTASKTC_RTCPRXSTATS);
 }
 if (!sts) sts = grv->setfriend(grv, rtcpgrv, FRIEND_TYPE_DEFAULT);
 unlock_object(vap->vmprx);
 if (vap->rtcphand) {
	unlock_object(vap->rtcphand);
	unlock_module(vap->vmprx->mod);
 }
 return sts;
}

STATIC int real_sm_vmptx_set_rtcphand(struct sm_vmptx_set_rtcphand_parms *vap)
{
 tSMGroove *grv;
 tSMGroove *rtcpgrv;
 int sts = 0;
 if (vap->rtcphand) {
	lock_module(vap->vmptx->mod);
	lock_object(vap->rtcphand);
	rtcpgrv = &vap->rtcphand->groove;
 } else {
	rtcpgrv = 0;
 }
 lock_object(vap->vmptx);
 grv = &vap->vmptx->groove;
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), SUBTASKTC_RTCPTXPRESEND);
 if (rtcpgrv) {
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADDVMPSUB, 2), SUBTASKTC_RTCPTXPRESEND);
 }
 if (!sts) sts = grv->setfriend(grv, rtcpgrv, FRIEND_TYPE_DEFAULT);
 unlock_object(vap->vmptx);
 if (vap->rtcphand) {
	unlock_object(vap->rtcphand);
	unlock_module(vap->vmptx->mod);
 }
 return sts;
}

STATIC int real_sm_vmprx_get_ports(struct sm_vmprx_port_parms *portp)
{
 int sts = 0;
 tSMVMPrxId vmprx = portp->vmprx;
 tSMGroove *grv = &vmprx->groove;

 lock_object(vmprx);
 //Only ask the task for the ports once - otherwise use the stored values
 if (portp->vmprx->udpports[0] && portp->vmprx->udpports[1]) {
	if (vmprx->is_ipv6) {
		portp->ipv6_address = portp->vmprx->address.ipv6;
		memset(&portp->address, 0xff, sizeof(portp->address)); // make the IPv4 address 255.255.255.255
	} else {
		portp->address = portp->vmprx->address.ipv4;
	}
	portp->RTP_port = portp->vmprx->udpports[0];
	portp->RTCP_port = portp->vmprx->udpports[1];
	unlock_object(portp->vmprx);
	return 0;
 }
 //wait for a response here
 for(;;) {
	uint32_t msg[16];
	unsigned long code; 
	sts = grv->rdmsg(grv, msg, (portp->nowait ? SMGROOVE_RDMSG_FLAG_NOWAIT : 0) );
	if (sts) break;
	code = msg[0] & 0xffff0000;	
	if (code == MESSAGE(MSG_GET_UDPPORTS, 4) && !portp->vmprx->is_ipv6) {
		portp->RTP_port = portp->vmprx->udpports[0] = msg[1];
		portp->RTCP_port = portp->vmprx->udpports[1] = msg[2];
		portp->vmprx->address.ipv4.s_addr = msg2ip(msg[3]);
		portp->address = portp->vmprx->address.ipv4;
		break;
	} else if (code == MESSAGE(MSG_GET_UDPPORTS, 7) && portp->vmprx->is_ipv6) {
		portp->RTP_port = portp->vmprx->udpports[0] = msg[1];
		portp->RTCP_port = portp->vmprx->udpports[1] = msg[2];
		msgtoipv6(&portp->vmprx->address.ipv6, msg[3], msg[4], msg[5], msg[6]);
		portp->ipv6_address = portp->vmprx->address.ipv6;
		memset(&portp->address, 0xff, sizeof(portp->address)); // make the IPv4 address 255.255.255.255
		break;
	} else if (MSG2ANYCODE(code) == MSG_DEAD) {
		if ((code & 0xffff0000) != MESSAGE(MSG_DEAD, 1)) {
			switch (msg[1]) {
			case MSG_DEAD_CAUSE_BADFUNC:
				sts = err(ERR_SM_NO_SUCH_FIRMWARE);
				break;
			case MSG_DEAD_CAUSE_NOMEM:
				sts = err(ERR_SM_NO_RESOURCES);
				break;
			case MSG_DEAD_CAUSE_PARAM:
				sts = err(ERR_SM_BAD_PARAMETER);
				break;
			case MSG_DEAD_CAUSE_NOINIT:
			case MSG_DEAD_CAUSE_FAILED:
			case MSG_DEAD_CAUSE_BADGEN:
			case MSG_DEAD_CAUSE_DUPID:
			default:
				sts = err(ERR_SM_DEVERR);
				break;
			case MSG_DEAD_CAUSE_DISCO:
				sts = err(ERR_SM_DISCONNECTED);
				break;
			case MSG_DEAD_CAUSE_LICENCE:
				sts = err(ERR_SM_NO_LICENCE);
				break;
			}
		} else {
			sts = err(ERR_SM_DEVERR);
		}
		break;
	} else if (code == MESSAGE(MSG_BADMSG, 3)) {
		if ((msg[1] & 0xffff0000) == MESSAGE(MSG_CREATE, 3)
			|| (msg[1] & 0xffff0000) == MESSAGE(MSG_ADDSUB, 2)
			|| (msg[1] & 0xffff0000) == MESSAGE(MSG_ADDVMPSUB, 2)) {
			switch (msg[2]) {
			case MSG_BADMSG_CAUSE_NOTASK:
				sts = err(ERR_SM_NO_SUCH_CHANNEL);
				break;
			case MSG_BADMSG_CAUSE_UNHANDLED:
				if ((msg[1] & 0xffff0000) == MESSAGE(MSG_ADDVMPSUB, 2)) break; // addvmpsub not handled by v2 PX firmware
				sts = err(ERR_SM_WRONG_FIRMWARE_TYPE);
				break;
			case MSG_BADMSG_CAUSE_RANGE:
				sts = err(ERR_SM_NO_SUCH_FIRMWARE);
				break;
			case MSG_BADMSG_CAUSE_MEMORY:
				sts = err(ERR_SM_NO_RESOURCES);
				break;
			default:
				sts = err(ERR_SM_DEVERR);
				break;
			}
			if (sts) break; // out of for loop
		} else if ((msg[1] & 0xffff0000) == MESSAGE(MSG_SET_INIPV4, 2)) {
			if (msg[2] == MSG_BADMSG_CAUSE_RANGE) {
				sts = err(ERR_SM_BAD_PARAMETER);
				break; // out of for loop
			}
		} else if ((msg[1] & 0xffff0000) == MESSAGE(MSG_SET_INIPV6, 5)) {
			if (msg[2] == MSG_BADMSG_CAUSE_RANGE) {
				sts = err(ERR_SM_BAD_PARAMETER);
				break; // out of for loop
			}
		}
	} else if (code == MESSAGE(MSG_GETLIC_INFO, 2)) {
		if (msg[1] == 0) {
			sts = err(ERR_SM_NO_LICENCE);
			break;
		}
	} else {
		if (portp->nowait) break;
	}
 }
 unlock_object(portp->vmprx);
 return sts;
}

STATIC int real_sm_vidmprx_get_ports(struct sm_vidmprx_port_parms *portp)
{
 int sts = 0;
 tSMVidMPrxId vidmprx = portp->vidmprx;
 tSMGroove *grv = &vidmprx->groove;

 lock_object(vidmprx);
 //Only ask the task for the ports once - otherwise use the stored values
 if (portp->vidmprx->udpports[0] && portp->vidmprx->udpports[1]) {
	if (vidmprx->is_ipv6) {
		portp->ipv6_address = portp->vidmprx->address.ipv6;
		memset(&portp->address, 0xff, sizeof(portp->address)); // make the IPv4 address 255.255.255.255
	} else {
		portp->address = portp->vidmprx->address.ipv4;
	}
	portp->RTP_port = portp->vidmprx->udpports[0];
	portp->RTCP_port = portp->vidmprx->udpports[1];
	unlock_object(portp->vidmprx);
	return 0;
 }
 //wait for a response here
 for(;;) {
	uint32_t msg[16];
	unsigned long code; 
	sts = grv->rdmsg(grv, msg, (portp->nowait ? SMGROOVE_RDMSG_FLAG_NOWAIT : 0));
	if (sts) break;
	code = msg[0] & 0xffff0000;	
	if (code == MESSAGE(MSG_GET_UDPPORTS, 4) && !portp->vidmprx->is_ipv6) {
		portp->RTP_port = portp->vidmprx->udpports[0] = msg[1];
		portp->RTCP_port = portp->vidmprx->udpports[1] = msg[2];
		portp->vidmprx->address.ipv4.s_addr = msg2ip(msg[3]);
		portp->address = portp->vidmprx->address.ipv4;
		break;
	} else if (code == MESSAGE(MSG_GET_UDPPORTS, 7) && portp->vidmprx->is_ipv6) {
		portp->RTP_port = portp->vidmprx->udpports[0] = msg[1];
		portp->RTCP_port = portp->vidmprx->udpports[1] = msg[2];
		msgtoipv6(&portp->vidmprx->address.ipv6, msg[3], msg[4], msg[5], msg[6]);
		portp->ipv6_address = portp->vidmprx->address.ipv6;
		memset(&portp->address, 0xff, sizeof(portp->address)); // make the IPv4 address 255.255.255.255
		break;

	} else if (MSG2ANYCODE(code) == MSG_DEAD) {
		if ((code & 0xffff0000) != MESSAGE(MSG_DEAD, 1)) {
			switch (msg[1]) {
			case MSG_DEAD_CAUSE_BADFUNC:
				sts = err(ERR_SM_NO_SUCH_FIRMWARE);
				break;
			case MSG_DEAD_CAUSE_NOMEM:
				sts = err(ERR_SM_NO_RESOURCES);
				break;
			case MSG_DEAD_CAUSE_PARAM:
				sts = err(ERR_SM_BAD_PARAMETER);
				break;
			case MSG_DEAD_CAUSE_NOINIT:
			case MSG_DEAD_CAUSE_FAILED:
			case MSG_DEAD_CAUSE_BADGEN:
			case MSG_DEAD_CAUSE_DUPID:
			default:
				sts = err(ERR_SM_DEVERR);
				break;
			case MSG_DEAD_CAUSE_DISCO:
				sts = err(ERR_SM_DISCONNECTED);
				break;
			case MSG_DEAD_CAUSE_LICENCE:
				sts = err(ERR_SM_NO_LICENCE);
				break;
			}
		} else {
			sts = err(ERR_SM_DEVERR);
		}
		break;
	} else if (code == MESSAGE(MSG_BADMSG, 3)) {
		if ((msg[1] & 0xffff0000) == MESSAGE(MSG_CREATE, 3)
		   || (msg[1] & 0xffff0000) == MESSAGE(MSG_ADDSUB, 2)
		   || (msg[1] & 0xffff0000) == MESSAGE(MSG_ADDVMPSUB, 2)) {
			switch (msg[2]) {
			case MSG_BADMSG_CAUSE_NOTASK:
				sts = err(ERR_SM_NO_SUCH_CHANNEL);
				break;
			case MSG_BADMSG_CAUSE_UNHANDLED:
				if ((msg[1] & 0xffff0000) == MESSAGE(MSG_ADDVMPSUB, 2)) break; // addvmpsub not handled by v2 PX firmware
				sts = err(ERR_SM_WRONG_FIRMWARE_TYPE);
				break;
			case MSG_BADMSG_CAUSE_RANGE:
				sts = err(ERR_SM_NO_SUCH_FIRMWARE);
				break;
			case MSG_BADMSG_CAUSE_MEMORY:
				sts = err(ERR_SM_NO_RESOURCES);
				break;
			default:
				sts = err(ERR_SM_DEVERR);
				break;
			}
			if (sts) break; // out of for loop
		} else if ((msg[1] & 0xffff0000) == MESSAGE(MSG_SET_INIPV4, 2)) {
			if (msg[2] == MSG_BADMSG_CAUSE_RANGE) {
				sts = err(ERR_SM_BAD_PARAMETER);
				break; // out of for loop
			}
		} else if ((msg[1] & 0xffff0000) == MESSAGE(MSG_SET_INIPV6, 5)) {
			if (msg[2] == MSG_BADMSG_CAUSE_RANGE) {
				sts = err(ERR_SM_BAD_PARAMETER);
				break; // out of for loop
			}
		}
	} else if (code == MESSAGE(MSG_GETLIC_INFO, 2)) {
		if (msg[1] == 0) {
			sts = err(ERR_SM_NO_LICENCE);
			break;
		}
	} else {
		if (portp->nowait) break;
	}
 }
 unlock_object(portp->vidmprx);
 return sts;
}

STATIC int real_sm_vmptx_config(struct sm_vmptx_config_parms *configp)
{
 const tSMVMPtxId vmptx = configp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmptx);
 grv = &vmptx->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_SET_OUTIPV4, 5),
	ip2msg(configp->destination_rtp.sin_addr.s_addr),
	ntohs(configp->destination_rtp.sin_port),
	ip2msg(configp->source_rtp.sin_addr.s_addr),
	ntohs(configp->source_rtp.sin_port));
 if (!sts) {
		// for old firmware, RTCP port replaced by MSG_SET_RTCPOUTIPV4
	 sts = grv->sendmsg(grv, MESSAGE(MSG_SET_TOS, 4), 
			configp->TOS_RTP, 
			ntohs(configp->destination_rtcp.sin_port),
			configp->TOS_RTCP);
 }
 if (!sts) {
	 sts = grv->sendmsg(grv, MESSAGE(MSG_SET_RTCPOUTIPV4, 5),
		ip2msg(configp->destination_rtcp.sin_addr.s_addr),
		ntohs(configp->destination_rtcp.sin_port),
		ip2msg(configp->source_rtcp.sin_addr.s_addr),
		ntohs(configp->source_rtcp.sin_port));
 }
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_ipv6(struct sm_vmptx_config_ipv6_parms *configp)
{
 const tSMVMPtxId vmptx = configp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmptx);
 grv = &vmptx->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_SET_OUTIPV6, 11),
	ipv6tomsg(configp->destination_rtp.sin6_addr),
	ntohs(configp->destination_rtp.sin6_port),
	ipv6tomsg(configp->source_rtp.sin6_addr),
	ntohs(configp->source_rtp.sin6_port));
 if (!sts) {
	 sts = grv->sendmsg(grv, MESSAGE(MSG_SET_TOS, 4), 
			(ntohl(configp->destination_rtp.sin6_flowinfo) >> 20 ) & 0xffu,
			0, // RTCP port not used (passed in MSG_SET_RTCPOUTIPV6 message)
			(ntohl(configp->destination_rtcp.sin6_flowinfo) >> 20 ) & 0xffu);  // get suitable value from sin6_flowinfo?
 }
 if (!sts) {
	 sts = grv->sendmsg(grv, MESSAGE(MSG_SET_RTCPOUTIPV6,11),
		ipv6tomsg(configp->destination_rtcp.sin6_addr),
		ntohs(configp->destination_rtcp.sin6_port),
		ipv6tomsg(configp->source_rtcp.sin6_addr),
		ntohs(configp->source_rtcp.sin6_port));
 }
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vidmptx_config(struct sm_vidmptx_config_parms *configp)
{
 const tSMVidMPtxId vidmptx = configp->vidmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vidmptx);
 grv = &vidmptx->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_SET_OUTIPV4, 5),
	ip2msg(configp->destination_rtp.sin_addr.s_addr),
	ntohs(configp->destination_rtp.sin_port),
	ip2msg(configp->source_rtp.sin_addr.s_addr),
	ntohs(configp->source_rtp.sin_port));
 if (!sts) {
		// for old firmware, RTCP port replaced by MSG_SET_RTCPOUTIPV4
	 sts = grv->sendmsg(grv, MESSAGE(MSG_SET_TOS, 4), 
			configp->TOS_RTP, 
			ntohs(configp->destination_rtcp.sin_port),
			configp->TOS_RTCP);
 }
 if (!sts) {
	 sts = grv->sendmsg(grv, MESSAGE(MSG_SET_RTCPOUTIPV4, 5),
		ip2msg(configp->destination_rtcp.sin_addr.s_addr),
		ntohs(configp->destination_rtcp.sin_port),
		ip2msg(configp->source_rtcp.sin_addr.s_addr),
		ntohs(configp->source_rtcp.sin_port));
 }
 unlock_object(vidmptx);
 return sts;
}

STATIC int real_sm_vidmptx_config_ipv6(struct sm_vidmptx_config_ipv6_parms *configp)
{
 const tSMVidMPtxId vidmptx = configp->vidmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vidmptx);
 grv = &vidmptx->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_SET_OUTIPV6, 11),
	ipv6tomsg(configp->destination_rtp.sin6_addr),
	ntohs(configp->destination_rtp.sin6_port),
	ipv6tomsg(configp->source_rtp.sin6_addr),
	ntohs(configp->source_rtp.sin6_port));
 if (!sts) {
	 sts = grv->sendmsg(grv, MESSAGE(MSG_SET_TOS, 4), 
			(ntohl(configp->destination_rtp.sin6_flowinfo) >> 20 ) & 0xffu,
			0, // RTCP port not used (passed in MSG_SET_RTCPOUTIPV6 message)
			(ntohl(configp->destination_rtcp.sin6_flowinfo) >> 20 ) & 0xffu);
 }
 if (!sts) {
	 sts = grv->sendmsg(grv, MESSAGE(MSG_SET_RTCPOUTIPV6,11),
		ipv6tomsg(configp->destination_rtcp.sin6_addr),
		ntohs(configp->destination_rtcp.sin6_port),
		ipv6tomsg(configp->source_rtcp.sin6_addr),
		ntohs(configp->source_rtcp.sin6_port));
 }
 unlock_object(vidmptx);
 return sts;
}

STATIC int real_sm_vmprx_config_authentication_hmac_sha1(struct sm_vmprx_config_authentication_hmac_sha1_parms *pp)
{
 const tSMVMPrxId vmprx = pp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (vmprx->authsubtask) {
	sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), vmprx->authsubtask);
 }
 vmprx->authsubtask = SUBTASK_SRTP_HMAC_DEC;
 sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), vmprx->authsubtask);
 if (!sts) sts =  grv->sendmsgstring(grv,  MSGSTRING_ID(MSGSTRING_HMAD, 1),	pp->keylen, pp->key);
 if (!sts) sts =  grv->sendmsg(grv,  MESSAGE(MSG_HMAC_TAG_LEN, 2),	pp->taglen);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_authentication_null(struct sm_vmprx_config_authentication_null_parms *pp)
{
 const tSMVMPrxId vmprx = pp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (vmprx->authsubtask) {
	sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), vmprx->authsubtask);
	vmprx->authsubtask = 0;
 }
 unlock_object(vmprx);
 return sts;
}

STATIC int unsafe_sm_vmprx_config_codec(tSMGroove *grv, unsigned typecode, unsigned codec_name, int builtin, int payload_type, int plc)
{
 int sts = 0;
 sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), typecode);
 if (payload_type != -1) {
	if (!sts) sts = grv->sendmsg(grv, builtin ? MESSAGE(MSG_ADDVMPSUB, 2) : MESSAGE(MSG_ADDSUB, 2), typecode);
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 5), typecode, payload_type, plc ? CODEC_PLC_ENABLE : CODEC_PLC_DISABLE, 8000);
 }
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), codec_name, payload_type, plc ? CODEC_PLC_ENABLE : CODEC_PLC_DISABLE);
 return sts;
}

STATIC int start_stop_plc(tSMVMPrxId vmprx, unsigned typecode, int start, int plc)
{
 tSMGroove *grv = &vmprx->groove;
 unsigned u;
 int sts = 0;
 if (start) {
	unsigned *list;
	unsigned len;
	unsigned pos = ~0u;
	int needadd = 1;
	if (plc) {
		list = vmprx->plcsubtasks;
		len = arlen(vmprx->plcsubtasks);
	} else {
		list = vmprx->plrsubtasks;
		len = arlen(vmprx->plrsubtasks);
	}
	for (u=0; u<len; u++) {
		if (list[u]) {
			needadd = 0; 
			if (list[u] == typecode) {
				return sts; // already present
			}
		} else if (pos == ~0u) {
			pos = u;
		}
	}
	if (pos == ~0u) return err(ERR_SM_NO_RESOURCES); // no room in array!!
	list[pos] = typecode;

	if (needadd) {
		if (plc) {
			sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_VMPPLC);
			if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 5), SUBTASKTC_VMPPLC, 0, 0, 8000);
		} else {
			sts = grv->sendmsg(grv, MESSAGE(MSG_ADDVMPSUB, 2), SUBTASKTC_VMPRXPLR);
			if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 5), SUBTASKTC_VMPRXPLR, 0, 0, 8000);
		}
	}
 } else {
	int needed = 0;
	int sts2 = 0;
	for (u=0; u<arlen(vmprx->plcsubtasks); u++) {
		if (vmprx->plcsubtasks[u] == typecode) {
			vmprx->plcsubtasks[u] = 0;
		} else if (vmprx->plcsubtasks[u]) {
			needed = 1;
		}
	}
	if (!needed) sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), SUBTASKTC_VMPPLC);

	needed = 0;
	for (u=0; u<arlen(vmprx->plrsubtasks); u++) {
		if (vmprx->plrsubtasks[u] == typecode) {
			vmprx->plrsubtasks[u] = 0;
		} else if (vmprx->plrsubtasks[u]) {
			needed = 1;
		}
	}
	if (!needed) sts2 = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), SUBTASKTC_VMPRXPLR);
	if (!sts) sts = sts2;
 }
 return sts;
}

STATIC int real_sm_vmprx_config_codec_ilbc(struct sm_vmprx_codec_ilbc_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 if (codecp->frame_len != 20 && codecp->frame_len != 30) return err(ERR_SM_BAD_PARAMETER);
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_ILBCDEC, RTP_CODEC_TYPE_ILBC, 0, codecp->payload_type, codecp->plc_mode == kSMPLCModeEnabled);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ILBC_USE_ENHANCER, 2), codecp->enhancer != 0);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ILBC_FRAME_LENGTH, 2), codecp->frame_len);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_codec_smv(struct sm_vmprx_codec_smv_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_SMVDEC, RTP_CODEC_TYPE_SMV, 0, codecp->payload_type, codecp->plc_mode == kSMPLCModeEnabled);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_codec_evrc(struct sm_vmprx_codec_evrc_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 int rtp_mode = EVRC_RTP_HEADERLESS;
 switch(codecp->rtp_mode) {
	case kSMEVRCRTPModeHeaderless: rtp_mode = EVRC_RTP_HEADERLESS; break;
	case kSMEVRCRTPModeRFC2658: rtp_mode = EVRC_RTP_RFC2658; break;
	case kSMEVRCRTPModeRFC3558: rtp_mode = EVRC_RTP_RFC3558; break;
	default: sts = err(ERR_SM_BAD_PARAMETER);
 }
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_EVRCDEC, RTP_CODEC_TYPE_EVRC, 0, codecp->payload_type, codecp->plc_mode == kSMPLCModeEnabled);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_EVRC_RTP_MODE, 2), rtp_mode);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_EVRC_FILTER, 2), codecp->post_filter);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_codec_g723_1(struct sm_vmprx_codec_g723_1_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_G723_1DEC, RTP_CODEC_TYPE_G723_1, 0, codecp->payload_type, codecp->plc_mode == kSMPLCModeEnabled);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_G723_1_FILTER, 2), codecp->post_filter);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_codec_g728(struct sm_vmprx_codec_g728_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 if (codecp->rate != 9600 && codecp->rate != 12800 && codecp->rate != 16000) return err(ERR_SM_BAD_PARAMETER);
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_G728DEC, RTP_CODEC_TYPE_G728, 0, codecp->payload_type, codecp->plc_mode == kSMPLCModeEnabled);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_G728_RATE, 2), codecp->rate);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_G728_FILTER, 2), codecp->post_filter);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_codec_gsmfr(struct sm_vmprx_codec_gsmfr_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_GSMFRDEC, RTP_CODEC_TYPE_GSMFR, 0, codecp->payload_type, codecp->plc_mode == kSMPLCModeEnabled);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_GSMFR_MODE, 2), codecp->variant != 0);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_codec_gsmhr(struct sm_vmprx_codec_gsmhr_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_GSMHRDEC, RTP_CODEC_TYPE_GSMHR, 0, codecp->payload_type, codecp->plc_mode == kSMPLCModeEnabled);
 unlock_object(vmprx);
 return sts;
}


STATIC int real_sm_vmprx_config_codec_gsmefr(struct sm_vmprx_codec_gsmefr_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_GSMEFRDEC, RTP_CODEC_TYPE_GSMEFR, 0, codecp->payload_type, codecp->plc_mode == kSMPLCModeEnabled); // AMR PLC will be on regardless
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_codec_isac(struct sm_vmprx_codec_isac_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_ISACDEC, RTP_CODEC_TYPE_ISAC, 0, codecp->payload_type, 0);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_codec_amrnb(struct sm_vmprx_codec_amrnb_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_AMRNBDEC, RTP_CODEC_TYPE_AMRNB, 0, codecp->payload_type, codecp->plc_mode == kSMPLCModeEnabled); // AMR PLC will be on regardless
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_AMRNB_ALIGN, 2), codecp->aligned);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_AMRNB_GSMEFR, 2), codecp->gsmefr);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_AMRNB_CMR, 2), codecp->report_cmr);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_codec_g722_1(struct sm_vmprx_codec_g722_1_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_G722_1DEC, RTP_CODEC_TYPE_G722_1, 0, codecp->payload_type, codecp->plc_mode == kSMPLCModeEnabled); // G.722.1 PLC will be on regardless
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_G722_1_MODE, 2), codecp->bitrate);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_codec_silk(struct sm_vmprx_codec_silk_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_SILKDEC, RTP_CODEC_TYPE_SILK, 0, codecp->payload_type, codecp->plc_mode == kSMPLCModeEnabled); // SILK PLC will be on regardless
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_codec_opus(struct sm_vmprx_codec_opus_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_TIMESTAMPRATE, 2), 0);
 if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_OPUSDEC, RTP_CODEC_TYPE_OPUS, 0, codecp->payload_type, 1); // OPUS PLC will be on regardless
 if (!sts && codecp->payload_type != -1)  sts = grv->sendmsg(grv, MESSAGE(MSG_OPUS_DECODE_FEC, 2), codecp->decode_fec);
 if (!sts && codecp->payload_type != -1)  sts = grv->sendmsg(grv, MESSAGE(MSG_TIMESTAMPRATE, 2), 48000);
 unlock_object(vmprx);
 return sts;
}


STATIC int real_sm_vmprx_config_codec_amrwb(struct sm_vmprx_codec_amrwb_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_AMRWBDEC, RTP_CODEC_TYPE_AMRWB, 0, codecp->payload_type, codecp->plc_mode == kSMPLCModeEnabled); // AMR PLC will be on regardless
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_AMRWB_ALIGN, 2), codecp->aligned);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_AMRWB_CMR, 2), codecp->report_cmr);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_codec_speex(struct sm_vmprx_codec_speex_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_SPEEXDEC, RTP_CODEC_TYPE_SPEEX, 0,
                                              codecp->payload_type, codecp->plc_mode == kSMPLCModeEnabled);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_SPEEXDEC, 5), 1, 8000, MSG_SPEEX_VMP_NARROWBAND, 1);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_codec_speex_mode(struct sm_vmprx_codec_speex_mode_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_SPEEXDEC_CTRL, 3), MSG_CONFIG_SPEEXDEC_ENHANCED, codecp->enh);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_codec_g722(struct sm_vmprx_codec_g722_parms *codecp)
{
 tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts;
 lock_object(vmprx);
 grv = &vmprx->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_TIMESTAMPRATE, 2), 0);
 if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_G722DEC, RTP_CODEC_TYPE_G722, 0, codecp->payload_type, codecp->plc_mode == kSMPLCModeEnabled);
 if (!sts && codecp->payload_type != -1)  sts = grv->sendmsg(grv, MESSAGE(MSG_TIMESTAMPRATE, 2), 8000);
 if (!sts) sts = start_stop_plc(vmprx, SUBTASKTC_G722DEC, codecp->payload_type != -1, codecp->plc_mode == kSMPLCModeEnabled);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_codec_g726(struct sm_vmprx_codec_g726_parms *codecp)
{
 tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 int pack_mode;
 switch (codecp->variant) {
	case kSMG726VariantRFC3551:
		pack_mode = MSG_G726_PACKING_RFC3551;
		break;
	case kSMG726VariantAAL2:
		pack_mode = MSG_G726_PACKING_AAL2;
		break;
	default:
		return err(ERR_SM_BAD_PARAMETER);
 }
 if (codecp->bits < 2 || codecp->bits > 5) return err(ERR_SM_BAD_PARAMETER);
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_G726DEC, RTP_CODEC_TYPE_G726, 0, codecp->payload_type, codecp->plc_mode == kSMPLCModeEnabled);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_G726_BITS, 2), codecp->bits);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_G726_PACKING, 2), pack_mode);
 if (!sts) sts = start_stop_plc(vmprx, SUBTASKTC_G726DEC, codecp->payload_type != -1, codecp->plc_mode == kSMPLCModeEnabled);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_codec(struct sm_vmprx_codec_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 switch (codecp->codec) {
 case kSMCodecTypeAlaw:
	if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_VMPRXG711A, RTP_CODEC_TYPE_ALAW, 1, codecp->payload_type, codecp->plc_mode == kSMPLCModeEnabled);
	if (!sts) sts = start_stop_plc(vmprx, SUBTASKTC_VMPRXG711A, codecp->payload_type != -1, codecp->plc_mode == kSMPLCModeEnabled);
	break;
 case kSMCodecTypeMulaw:
	if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_VMPRXG711MU, RTP_CODEC_TYPE_ULAW, 1, codecp->payload_type, codecp->plc_mode == kSMPLCModeEnabled);
	if (!sts) sts = start_stop_plc(vmprx, SUBTASKTC_VMPRXG711MU, codecp->payload_type != -1, codecp->plc_mode == kSMPLCModeEnabled);
	break;
 case kSMCodecTypeG729AB:
	if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_G729ABDEC, RTP_CODEC_TYPE_G729, 0, codecp->payload_type, codecp->plc_mode == kSMPLCModeEnabled);
	break;
 case kSMCodecTypeRFC2833:
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 5), TASKTC_VMPRX, codecp->payload_type, CODEC_PLC_DISABLE, 8000);
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_2833, codecp->payload_type, CODEC_PLC_DISABLE);
	break;
 case kSMCodecTypeComfortNoise:
	if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_VMPRXCNG, RTP_CODEC_TYPE_COMFORT_NOISE, 1, codecp->payload_type, codecp->plc_mode == kSMPLCModeEnabled);
	break;
 default:
	sts = err(ERR_SM_BAD_PARAMETER);
 }
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_codec_alaw(struct sm_vmprx_codec_alaw_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_VMPRXG711A, RTP_CODEC_TYPE_ALAW, 1, codecp->payload_type, codecp->plc_mode == kSMPLCModeEnabled);
 if (!sts) sts = start_stop_plc(vmprx, SUBTASKTC_VMPRXG711A, codecp->payload_type != -1, codecp->plc_mode == kSMPLCModeEnabled);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_codec_l8(struct sm_vmprx_codec_l8_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_VMPRXL8, RTP_CODEC_TYPE_L8, 1, codecp->payload_type, codecp->plc_mode == kSMPLCModeEnabled);
 if (!sts) sts = start_stop_plc(vmprx, SUBTASKTC_VMPRXL8, codecp->payload_type != -1, codecp->plc_mode == kSMPLCModeEnabled);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_codec_l16(struct sm_vmprx_codec_l16_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_VMPRXL16, RTP_CODEC_TYPE_L16, 1, codecp->payload_type, codecp->plc_mode == kSMPLCModeEnabled);
 if (!sts) sts = start_stop_plc(vmprx, SUBTASKTC_VMPRXL16, codecp->payload_type != -1, codecp->plc_mode == kSMPLCModeEnabled);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_codec_comfort_noise(struct sm_vmprx_codec_comfort_noise_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_VMPRXCNG, RTP_CODEC_TYPE_COMFORT_NOISE, 1, codecp->payload_type, 0);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_codec_mulaw(struct sm_vmprx_codec_mulaw_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_VMPRXG711MU, RTP_CODEC_TYPE_ULAW, 1, codecp->payload_type, codecp->plc_mode == kSMPLCModeEnabled);
 if (!sts) sts = start_stop_plc(vmprx, SUBTASKTC_VMPRXG711MU, codecp->payload_type != -1, codecp->plc_mode == kSMPLCModeEnabled);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_codec_g729ab(struct sm_vmprx_codec_g729ab_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_G729ABDEC, RTP_CODEC_TYPE_G729, 0, codecp->payload_type, codecp->plc_mode == kSMPLCModeEnabled);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_codec_g729i(struct sm_vmprx_codec_g729i_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_G729IDEC, RTP_CODEC_TYPE_G729I, 0, codecp->payload_type, !0);
 if (!sts) switch (codecp->g729_mode)
 {
  case kSMG729IModeG729D: sts = grv->sendmsg(grv, MESSAGE(MSG_G729I_RATE, 2), MSG_G729I_RATE_G729D); break;
  case kSMG729IModeG729:  sts = grv->sendmsg(grv, MESSAGE(MSG_G729I_RATE, 2), MSG_G729I_RATE_G729); break;
  case kSMG729IModeG729E: sts = grv->sendmsg(grv, MESSAGE(MSG_G729I_RATE, 2), MSG_G729I_RATE_G729E); break;
 }
 
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_codec_melpe(struct sm_vmprx_codec_melpe_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;

 sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_MELPeDEC, RTP_CODEC_TYPE_MELPe, 0, codecp->payload_type, 0);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_codec_rfc2833(struct sm_vmprx_codec_rfc2833_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 5), TASKTC_VMPRX, codecp->payload_type, CODEC_PLC_DISABLE, 8000);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_2833, codecp->payload_type, CODEC_PLC_DISABLE);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_codec_rfc4040(struct sm_vmprx_codec_rfc4040_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_VMPRXRFC4040, RTP_CODEC_TYPE_RFC4040, 1, codecp->payload_type, 0);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_codec_tetra(struct sm_vmprx_codec_tetra_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_TETRADEC, RTP_CODEC_TYPE_TETRA, 0, codecp->payload_type, 0);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_encryption_aes_cm(struct sm_vmprx_config_encryption_aes_cm_parms *pp)
{
 const tSMVMPrxId vmprx = pp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (vmprx->encsubtask) {
	sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), vmprx->encsubtask);
 }
 vmprx->encsubtask = SUBTASK_SRTP_ICM_DEC;
 sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), vmprx->encsubtask);
 if (!sts) {
	sts =  grv->sendmsgstring(grv,  MSGSTRING_ID(MSGSTRING_ICMD, 1),
		pp->keylen, pp->key);
 }
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_encryption_aes_f8(struct sm_vmprx_config_encryption_aes_f8_parms *pp)
{
 const tSMVMPrxId vmprx = pp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (vmprx->encsubtask) {
	sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), vmprx->encsubtask);
 }
 vmprx->encsubtask = SUBTASK_SRTP_CBC_DEC;
 sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), vmprx->encsubtask);
 if (!sts) {
	sts =  grv->sendmsgstring(grv,  MSGSTRING_ID(MSGSTRING_CBCD, 1),
		pp->keylen, pp->key);
 }
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_encryption_null(struct sm_vmprx_config_encryption_null_parms *pp)
{
 const tSMVMPrxId vmprx = pp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (vmprx->encsubtask) {
	sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), vmprx->encsubtask);
	vmprx->encsubtask = 0;
 }
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmptx_config_authentication_hmac_sha1(struct sm_vmptx_config_authentication_hmac_sha1_parms *pp)
{
 const tSMVMPtxId vmptx = pp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmptx);
 grv = &vmptx->groove;
 if (vmptx->authsubtask) {
	sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), vmptx->authsubtask);
 }
 vmptx->authsubtask = SUBTASK_SRTP_HMAC_ENC;
 sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), vmptx->authsubtask);
 if (!sts) sts =  grv->sendmsgstring(grv,  MSGSTRING_ID(MSGSTRING_HMAE, 1),	pp->keylen, pp->key);
 if (!sts) sts =  grv->sendmsg(grv,  MESSAGE(MSG_HMAC_TAG_LEN, 2),	pp->taglen);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_authentication_null(struct sm_vmptx_config_authentication_null_parms *pp)
{
 const tSMVMPtxId vmptx = pp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmptx);
 grv = &vmptx->groove;
 if (vmptx->authsubtask) {
	sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), vmptx->authsubtask);
	vmptx->authsubtask = 0;
 }
 unlock_object(vmptx);
 return sts;
}

STATIC int unsafe_vmptx_set_main_codec(tSMVMPtxId vmptx, unsigned typecode, int payload_type, int builtin, int g711_vad, enum kSMVMPTxVADMode VADMode)
{
	tSMGroove *grv = &vmptx->groove;
	uint32_t vadmode = RTP_VAD_MODE_DISABLED;
	int sts = 0;
	switch(VADMode) {
		case kSMVMPTxVADModeDisabled:
			// use default value
			break;
		case kSMVMPTxVADModeEnabled:
			vadmode = RTP_VAD_MODE_ENABLED;
			break;
		case kSMVMPTxVADModeComfortNoise:
			vadmode = RTP_VAD_MODE_COMFORT_NOISE;
			break;
		default:
			sts = err(ERR_SM_BAD_PARAMETER);
	}
	if (payload_type != -1 || vmptx->maincodecsubtask == typecode) {
		// either setting (new) codec or removing current
		if (vmptx->using_g711_vad) {
			if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), SUBTASKTC_VMPTXVAD);
			if (!sts) vmptx->using_g711_vad = 0;
		}
		if (!sts && vmptx->maincodecsubtask) {
			sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), vmptx->maincodecsubtask);
			if (!sts) vmptx->maincodecsubtask = 0;
		}
	}
	if (payload_type != -1) {
		if (!sts) sts = grv->sendmsg(grv, builtin ? MESSAGE(MSG_ADDVMPSUB, 2) : MESSAGE(MSG_ADDSUB, 2), typecode);
		if (!sts) vmptx->maincodecsubtask = typecode;
		if (g711_vad) {
			if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADDVMPSUB, 2), SUBTASKTC_VMPTXVAD);
			if (!sts) {
				vmptx->using_g711_vad = 1;
				if (vmptx->comfort_noise_pt != -1) { // sm_vmptx_config_codec_comfort_noise already called, so send pt now
					sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_VMPTXVAD, vmptx->comfort_noise_pt, 0);
					if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_COMFORT_NOISE, vmptx->comfort_noise_pt, 0);
				}
			}
		}
		if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_VAD_MODE, 2), vadmode);
	}
	return sts;
}

STATIC int unsafe_sm_vidmptx_config_codec(tSMVidMPtxId vidmptx, unsigned typecode, int payload_type)
{
	tSMGroove *grv= &vidmptx->groove;
	int sts = 0;
	if (payload_type != -1 || vidmptx->maincodecsubtask == typecode) {
		// either setting (new) codec or removing current
		if (vidmptx->maincodecsubtask) {
			sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), vidmptx->maincodecsubtask);
			if (!sts) vidmptx->maincodecsubtask = 0;
		}
	}
	if (payload_type != -1) {
		if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), typecode);
		if (!sts) vidmptx->maincodecsubtask = typecode;
		if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 3), typecode, payload_type);
	}
	return sts;
}

STATIC int real_sm_vmptx_config_codec(struct sm_vmptx_codec_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmptx);
 grv = &vmptx->groove;
 if (codecp->codec == kSMCodecTypeRFC2833) {
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), TASKTC_VMPTX, codecp->payload_type, 0);
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_2833, codecp->payload_type, codecp->ptime);
 } else if (codecp->codec == kSMCodecTypeComfortNoise) {
	vmptx->comfort_noise_pt = codecp->payload_type;
	if (vmptx->using_g711_vad) {
		sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_VMPTXVAD, codecp->payload_type, 0);
		if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_COMFORT_NOISE, codecp->payload_type, 0);
	}
 } else {
	if (!codecp->ptime || (codecp->ptime % 10)) sts = err(ERR_SM_BAD_PARAMETER);
	switch(codecp->codec) {
		case kSMCodecTypeAlaw:
			if (!sts) sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_VMPTXG711A, codecp->payload_type, 1, 1, codecp->VADMode);
			if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_ALAW, codecp->payload_type, codecp->ptime);
			if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_VMPTXG711A, codecp->payload_type, codecp->ptime);
			break;
		case kSMCodecTypeMulaw:
			if (!sts) sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_VMPTXG711MU, codecp->payload_type, 1, 1, codecp->VADMode);
			if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_ULAW, codecp->payload_type, codecp->ptime);
			if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_VMPTXG711MU, codecp->payload_type, codecp->ptime);
			break;
		case kSMCodecTypeG729AB:
			if (!sts) sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_G729ABENC, codecp->payload_type, 0, 0, codecp->VADMode);
			if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_G729, codecp->payload_type, codecp->ptime);
			if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_G729ABENC, codecp->payload_type, codecp->ptime);
			break;
		default:
			sts = err(ERR_SM_BAD_PARAMETER);
	}
 }
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_alaw(struct sm_vmptx_codec_alaw_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmptx);
 grv = &vmptx->groove;
 if (!codecp->ptime || (codecp->ptime % 10)) sts = err(ERR_SM_BAD_PARAMETER);
 if (!sts) sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_VMPTXG711A, codecp->payload_type, 1, 1, codecp->VADMode);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_ALAW, codecp->payload_type, codecp->ptime);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_VMPTXG711A, codecp->payload_type, codecp->ptime);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_l8(struct sm_vmptx_codec_l8_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmptx);
 grv = &vmptx->groove;
 if (!codecp->ptime || (codecp->ptime % 10)) sts = err(ERR_SM_BAD_PARAMETER);
 if (!sts) sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_VMPTXL8, codecp->payload_type, 1, 1, codecp->VADMode);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_L8, codecp->payload_type, codecp->ptime);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_VMPTXL8, codecp->payload_type, codecp->ptime);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_l16(struct sm_vmptx_codec_l16_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmptx);
 grv = &vmptx->groove;
 if (!codecp->ptime || (codecp->ptime % 10)) sts = err(ERR_SM_BAD_PARAMETER);
 if (!sts) sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_VMPTXL16, codecp->payload_type, 1, 1, codecp->VADMode);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_L16, codecp->payload_type, codecp->ptime);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_VMPTXL16, codecp->payload_type, codecp->ptime);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_mulaw(struct sm_vmptx_codec_mulaw_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmptx);
 grv = &vmptx->groove;
 if (!codecp->ptime || (codecp->ptime % 10)) sts = err(ERR_SM_BAD_PARAMETER);
 if (!sts) sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_VMPTXG711MU, codecp->payload_type, 1, 1, codecp->VADMode);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_ULAW, codecp->payload_type, codecp->ptime);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_VMPTXG711MU, codecp->payload_type, codecp->ptime);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_g729ab(struct sm_vmptx_codec_g729ab_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmptx);
 grv = &vmptx->groove;
 if (!codecp->ptime || (codecp->ptime % 10)) sts = err(ERR_SM_BAD_PARAMETER);
 if (!sts) sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_G729ABENC, codecp->payload_type, 0, 0, codecp->VADMode);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_G729, codecp->payload_type, codecp->ptime);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_G729ABENC, codecp->payload_type, codecp->ptime);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_g729i(struct sm_vmptx_codec_g729i_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmptx);
 grv = &vmptx->groove;
 if (!codecp->ptime || (codecp->ptime % 10)) sts = err(ERR_SM_BAD_PARAMETER);
 if (!sts) sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_G729IENC, codecp->payload_type, 0, 0, codecp->VADMode);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_G729I, codecp->payload_type, codecp->ptime);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_G729IENC, codecp->payload_type, codecp->ptime);
 if (!sts) switch (codecp->g729_mode)
 {
  case kSMG729IModeG729D: sts = grv->sendmsg(grv, MESSAGE(MSG_G729I_RATE, 2), MSG_G729I_RATE_G729D); break;
  case kSMG729IModeG729:  sts = grv->sendmsg(grv, MESSAGE(MSG_G729I_RATE, 2), MSG_G729I_RATE_G729); break;
  case kSMG729IModeG729E: sts = grv->sendmsg(grv, MESSAGE(MSG_G729I_RATE, 2), MSG_G729I_RATE_G729E); break;
 }
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_G729I_DTX, 2), codecp->VADMode == kSMVMPTxVADModeEnabled);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_melpe(struct sm_vmptx_codec_melpe_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmptx);
 grv = &vmptx->groove;
 if (!codecp->ptime || (codecp->ptime % 10)) sts = err(ERR_SM_BAD_PARAMETER);
 if (!sts) sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_MELPeENC, codecp->payload_type, 0, 0, 0);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_MELPe, codecp->payload_type, codecp->ptime);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_MELPeENC, codecp->payload_type, codecp->ptime);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_rfc2833(struct sm_vmptx_codec_rfc2833_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmptx);
 grv = &vmptx->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), TASKTC_VMPTX, codecp->payload_type, 0);
 sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_2833, codecp->payload_type, 0);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_rfc4040(struct sm_vmptx_codec_rfc4040_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmptx);
 grv = &vmptx->groove;
 if (!sts) sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_VMPTXRFC4040, codecp->payload_type, 1, 0, kSMVMPTxVADModeDisabled);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_RFC4040, codecp->payload_type, codecp->packetlen);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_VMPTXRFC4040, codecp->payload_type, codecp->packetlen / 8);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_tetra(struct sm_vmptx_codec_tetra_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmptx);
 grv = &vmptx->groove;
 if (!sts) sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_TETRAENC, codecp->payload_type, 0, 0, 0);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_TETRA, codecp->payload_type, 0);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_TETRAENC, codecp->payload_type, 0);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_comfort_noise(struct sm_vmptx_codec_comfort_noise_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmptx);
 vmptx->comfort_noise_pt = codecp->payload_type;
 if (vmptx->using_g711_vad) {
	grv = &vmptx->groove;
	sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_VMPTXVAD, codecp->payload_type, 0);
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_COMFORT_NOISE, codecp->payload_type, 0);
 }
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_evrc(struct sm_vmptx_codec_evrc_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 int min_rate = EVRC_RATE_EIGHTH;
 int max_rate = EVRC_RATE_FULL;
 int rtp_mode = EVRC_RTP_HEADERLESS;
 if (!codecp->frames_per_packet) return err(ERR_SM_BAD_PARAMETER);
 if (codecp->rtp_mode != kSMEVRCRTPModeRFC3558 && codecp->frames_per_packet != 1) return err(ERR_SM_BAD_PARAMETER);
 switch(codecp->rtp_mode) {
	case kSMEVRCRTPModeHeaderless: rtp_mode = EVRC_RTP_HEADERLESS; break;
	case kSMEVRCRTPModeRFC2658: rtp_mode = EVRC_RTP_RFC2658; break;
	case kSMEVRCRTPModeRFC3558: rtp_mode = EVRC_RTP_RFC3558; break;
	default: sts = err(ERR_SM_BAD_PARAMETER);
 }
 switch(codecp->min_rate) {
	case kSMEVRCRateFull: min_rate = EVRC_RATE_FULL; break;
	case kSMEVRCRateHalf: min_rate = EVRC_RATE_HALF; break;
	case kSMEVRCRateEighth: min_rate = EVRC_RATE_EIGHTH; break;
	default: sts = err(ERR_SM_BAD_PARAMETER);
 }
 switch(codecp->max_rate) {
	case kSMEVRCRateFull: max_rate = EVRC_RATE_FULL; break;
	case kSMEVRCRateHalf: max_rate = EVRC_RATE_HALF; break;
	case kSMEVRCRateEighth: max_rate = EVRC_RATE_EIGHTH; break;
	default: sts = err(ERR_SM_BAD_PARAMETER);
 }
 lock_object(vmptx);
 grv = &vmptx->groove;
 if (!sts) sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_EVRCENC, codecp->payload_type, 0, 0, codecp->VADMode);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_EVRC, codecp->payload_type, codecp->frames_per_packet);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_EVRCENC, codecp->payload_type, codecp->frames_per_packet * 20);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_EVRC_RTP_MODE, 2), rtp_mode);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_EVRC_FILTER, 3), codecp->high_pass_filter, codecp->noise_suppression_filter);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_EVRC_RATE, 3), min_rate, max_rate);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_g723_1(struct sm_vmptx_codec_g723_1_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 if (!codecp->frames_per_packet) return err(ERR_SM_BAD_PARAMETER);
 lock_object(vmptx);
 grv = &vmptx->groove;
 if (!sts) sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_G723_1ENC, codecp->payload_type, 0, 0, kSMVMPTxVADModeDisabled);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_G723_1, codecp->payload_type, codecp->frames_per_packet);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_G723_1ENC, codecp->payload_type, codecp->frames_per_packet * 30);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_G723_1_HPF, 2), codecp->high_pass_filter);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_G723_1_RATE, 2), codecp->rate);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_G723_1_SILCOMP, 2), codecp->silence_compression);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_g728(struct sm_vmptx_codec_g728_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 if (!codecp->ptime || (codecp->ptime % 10)) sts = err(ERR_SM_BAD_PARAMETER);
 if (codecp->rate != 9600 && codecp->rate != 12800 && codecp->rate != 16000) return err(ERR_SM_BAD_PARAMETER);
 lock_object(vmptx);
 grv = &vmptx->groove;
 if (!sts) sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_G728ENC, codecp->payload_type, 0, 1, codecp->VADMode);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_G728, codecp->payload_type, codecp->ptime);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_G728ENC, codecp->payload_type, codecp->ptime);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_G728_RATE, 2), codecp->rate);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_gsmfr(struct sm_vmptx_codec_gsmfr_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 if (!codecp->frames_per_packet) return err(ERR_SM_BAD_PARAMETER);
 if (codecp->variant && codecp->frames_per_packet % 2) return err(ERR_SM_BAD_PARAMETER);
 lock_object(vmptx);
 grv = &vmptx->groove;
 if (!sts) sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_GSMFRENC, codecp->payload_type, 0, 0, codecp->VADMode);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_GSMFR, codecp->payload_type, codecp->frames_per_packet);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_GSMFRENC, codecp->payload_type, codecp->frames_per_packet * 20);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_GSMFR_MODE, 2), codecp->variant != 0);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_gsmhr(struct sm_vmptx_codec_gsmhr_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 if (!codecp->frames_per_packet) return err(ERR_SM_BAD_PARAMETER);
 lock_object(vmptx);
 grv = &vmptx->groove;
 if (!sts) sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_GSMHRENC, codecp->payload_type, 0, 0, codecp->VADMode);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_GSMHR, codecp->payload_type, codecp->frames_per_packet);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_GSMHRENC, codecp->payload_type, codecp->frames_per_packet * 20);
 unlock_object(vmptx);
 return sts;
}


STATIC int real_sm_vmptx_config_codec_gsmefr(struct sm_vmptx_codec_gsmefr_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 if (!codecp->frames_per_packet) return err(ERR_SM_BAD_PARAMETER);
 lock_object(vmptx);
 grv = &vmptx->groove;
 if (!sts) sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_GSMEFRENC, codecp->payload_type, 0, 0, codecp->VADMode);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_GSMEFR, codecp->payload_type, codecp->frames_per_packet);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_GSMEFRENC, codecp->payload_type, codecp->frames_per_packet * 20);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_isac(struct sm_vmptx_codec_isac_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *partgrv;
 tSMGroove *grv;
 int sts = 0;
 if (codecp->partner) {
	lock_object(codecp->partner);
	partgrv = &codecp->partner->groove;
	unlock_object(codecp->partner);
 } else {
	partgrv = 0;
 }
 lock_object(vmptx);
 grv = &vmptx->groove;
 if (!sts) sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_ISACENC, codecp->payload_type, 0, 0, 0);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_ISAC, codecp->payload_type, 0);
 if (!sts) sts = grv->setfriend(grv, partgrv, FRIEND_TYPE_ISAC);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_iSAC_CAM, 2), codecp->channel_associated_mode);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_ISACENC, codecp->payload_type, 30); // ptime is 30 or 60 but is only used to set DTMF ptime
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_amrnb(struct sm_vmptx_codec_amrnb_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 if (!codecp->frames_per_packet) return err(ERR_SM_BAD_PARAMETER);
 lock_object(vmptx);
 grv = &vmptx->groove;
 if (!sts) sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_AMRNBENC, codecp->payload_type, 0, 0, codecp->VADMode);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_AMRNB, codecp->payload_type, codecp->frames_per_packet);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_AMRNBENC, codecp->payload_type, codecp->frames_per_packet * 20);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_AMRNB_ALIGN, 2), codecp->aligned);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_AMRNB_GSMEFR, 2), codecp->gsmefr);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_g722_1(struct sm_vmptx_codec_g722_1_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 if (!codecp->frames_per_packet) return err(ERR_SM_BAD_PARAMETER);
 lock_object(vmptx);
 grv = &vmptx->groove;
 if (!sts) sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_G722_1ENC, codecp->payload_type, 0, 0, kSMVMPTxVADModeDisabled);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_G722_1, codecp->payload_type, codecp->frames_per_packet);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_G722_1ENC, codecp->payload_type, codecp->frames_per_packet * 20);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_G722_1_MODE, 2), codecp->bitrate);
 unlock_object(vmptx);
 return sts;
}

static int check_silk_parms(tSM_INT sample_rate, tSM_INT internal_rate, tSM_INT bit_rate, tSM_INT frames_per_packet, tSM_INT packet_loss, tSM_INT complexity)
{
 switch (sample_rate)   // Only allow 8kHz or 16kHz sampling for now - until other algorithms are confirmed to work at other rates
 {
 case 8000:
 // case 12000:
 case 16000:
 // case 24000:
 // case 32000:
 // case 44100:
 // case 48000:
  break;
  
 default:
  return !0;
 }
 
 switch (internal_rate)
 {
 case 8000:
 case 12000:
 case 16000:
 case 24000:
  break;
  
 default:
  return !0;
 }
 
 if (internal_rate > sample_rate
  || bit_rate < 5000 || bit_rate > 100000
  || frames_per_packet < 1 || frames_per_packet > 5
  || packet_loss < 0 || packet_loss > 100
  || complexity < 0 || complexity > 2) return !0;
 
 return 0;
}

STATIC int real_sm_vmptx_config_codec_silk(struct sm_vmptx_codec_silk_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;

 if (check_silk_parms(codecp->sample_rate,
                      codecp->internal_rate,
                      codecp->bit_rate,
                      codecp->frames_per_packet,
                      codecp->packet_loss,
                      codecp->complexity)) return err(ERR_SM_BAD_PARAMETER);
 
 lock_object(vmptx);
 grv = &vmptx->groove;
 sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_SILKENC, codecp->payload_type, 0, 0, kSMVMPTxVADModeDisabled);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_SILK, codecp->payload_type, codecp->frames_per_packet);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_SILKENC, codecp->payload_type, codecp->frames_per_packet * 20);
 if (!sts) sts = grv->sendmsg(grv,
                              MESSAGE(MSG_SILK_SETPARMS, 9),
                              codecp->sample_rate,
                              codecp->internal_rate,
                              codecp->bit_rate,
                              codecp->frames_per_packet,
                              codecp->packet_loss,
                              codecp->complexity,
                              codecp->use_fec,
                              codecp->use_dtx);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_silk_mode(struct sm_vmptx_config_codec_silk_mode_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;

 if (check_silk_parms(codecp->sample_rate,
                      codecp->internal_rate,
                      codecp->bit_rate,
                      codecp->frames_per_packet,
                      codecp->packet_loss,
                      codecp->complexity)) return err(ERR_SM_BAD_PARAMETER);

 lock_object(vmptx);
 grv = &vmptx->groove;
 sts = grv->sendmsg(grv,
                    MESSAGE(MSG_SILK_SETPARMS, 9),
                    codecp->sample_rate,
                    codecp->internal_rate,
                    codecp->bit_rate,
                    codecp->frames_per_packet,
                    codecp->packet_loss,
                    codecp->complexity,
                    codecp->use_fec,
                    codecp->use_dtx);
 unlock_object(vmptx);
 return sts;
}


static int check_opus_parms(tSM_INT sample_rate, tSM_INT bit_rate, tSM_INT ptime, tSM_INT packet_loss, tSM_INT complexity, enum kSMOPUSRate rate_control, enum kSMOPUSSignalTypeHint signal_type_hint, enum kSMOPUSAppType application_type )
{
 switch (sample_rate)   // Only allow 8kHz or 16kHz sampling for now - until other algorithms are confirmed to work at other rates
 {
 case 8000:
 case 16000:
  break;
  
 default:
  return !0;
 }
  
 if (
     (bit_rate < 500 && bit_rate >= 1) || bit_rate > 512000
  || ptime < 10 || ptime > 60
  || packet_loss < 0 || packet_loss > 100
  || complexity < 0 || complexity > 10
  || rate_control > kSMOPUSRateConstant
  || signal_type_hint > kSMOPUSSignalTypeHintMusic
  || application_type > kSMOPUSAppTypeLowDelay ) return !0;
 
 return 0;
}

STATIC int real_sm_vmptx_config_codec_opus(struct sm_vmptx_codec_opus_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;

 if ( (codecp->payload_type != -1) && 
     check_opus_parms(codecp->sample_rate,
                      codecp->bit_rate,
                      codecp->ptime,
                      codecp->packet_loss,
                      codecp->complexity,
					  codecp->rate_control,
                      codecp->signal_type_hint,
                      codecp->application_type		  
					  )) return err(ERR_SM_BAD_PARAMETER);
 
 lock_object(vmptx);
 grv = &vmptx->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_TIMESTAMPRATE, 2), 0);
 if (!sts) sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_OPUSENC, codecp->payload_type, 0, 0, kSMVMPTxVADModeDisabled);
 if (!sts && codecp->payload_type != -1) sts = grv->sendmsg(grv, MESSAGE(MSG_TIMESTAMPRATE, 2), 48000);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_OPUS, codecp->payload_type, codecp->ptime);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_OPUSENC, codecp->payload_type, codecp->ptime * 20);
 if (!sts && (codecp->payload_type != -1)) sts = grv->sendmsg(grv,
                              MESSAGE(MSG_OPUS_SETPARMS, 12),
                              codecp->sample_rate,
                              codecp->bit_rate,
                              codecp->ptime,
                              codecp->packet_loss,
                              codecp->complexity,
                              codecp->use_fec,
                              codecp->use_dtx,
                              codecp->rate_control,
                              codecp->signal_type_hint,
                              codecp->application_type,
                              codecp->disable_prediction );
						  					  
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_opus_mode(struct sm_vmptx_config_codec_opus_mode_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;

 if (check_opus_parms(codecp->sample_rate,
                      codecp->bit_rate,
                      codecp->ptime,
                      codecp->packet_loss,
                      codecp->complexity,
					  codecp->rate_control,
                      codecp->signal_type_hint,
                      codecp->application_type )) return err(ERR_SM_BAD_PARAMETER);

 lock_object(vmptx);
 grv = &vmptx->groove;
 sts = grv->sendmsg(grv,
                    MESSAGE(MSG_OPUS_SETPARMS, 12),
                              codecp->sample_rate,
                              codecp->bit_rate,
                              codecp->ptime,
                              codecp->packet_loss,
                              codecp->complexity,
                              codecp->use_fec,
                              codecp->use_dtx,
                              codecp->rate_control,
                              codecp->signal_type_hint,
                              codecp->application_type,
                              codecp->disable_prediction );					
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_amrwb(struct sm_vmptx_codec_amrwb_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 if (!codecp->frames_per_packet) return err(ERR_SM_BAD_PARAMETER);
 lock_object(vmptx);
 grv = &vmptx->groove;
 if (!sts) sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_AMRWBENC, codecp->payload_type, 0, 0, codecp->VADMode ? kSMVMPTxVADModeEnabled : kSMVMPTxVADModeDisabled);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_AMRWB, codecp->payload_type, codecp->frames_per_packet);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_AMRWBENC, codecp->payload_type, codecp->frames_per_packet * 20);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_AMRWB_ALIGN, 2), codecp->aligned);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_amrnb_mode(struct sm_vmptx_config_codec_amrnb_mode_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmptx);
 grv = &vmptx->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_AMRNB_MODE, 2), codecp->bitrate);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_amrnb_cmr(struct sm_vmptx_config_codec_amrnb_cmr_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmptx);
 grv = &vmptx->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_AMRNB_CMR, 2), codecp->bitrate);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_amrwb_mode(struct sm_vmptx_config_codec_amrwb_mode_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmptx);
 grv = &vmptx->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_AMRWB_MODE, 2), codecp->bitrate);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_amrwb_cmr(struct sm_vmptx_config_codec_amrwb_cmr_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmptx);
 grv = &vmptx->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_AMRWB_CMR, 2), codecp->bitrate);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_speex(struct sm_vmptx_codec_speex_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 if (!codecp->frames_per_packet) return err(ERR_SM_BAD_PARAMETER);
 lock_object(vmptx);
 grv = &vmptx->groove;
 if (!sts) sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_SPEEXENC, codecp->payload_type, 0, 0, codecp->VADMode);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_SPEEX, codecp->payload_type, codecp->frames_per_packet);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_SPEEXENC, codecp->payload_type, codecp->frames_per_packet);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_SPEEXENC, 2), MSG_SPEEX_VMP_NARROWBAND);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_speex_mode(struct sm_vmptx_codec_speex_mode_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmptx);
 grv = &vmptx->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_SPEEXENC_CTRL, 4), MSG_CONFIG_SPEEXENC_COMPLEXITY, codecp->complexity, 0);
 sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_SPEEXENC_CTRL, 4), MSG_CONFIG_SPEEXENC_QUALITY, codecp->quality, 0);
 sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_SPEEXENC_CTRL, 4), MSG_CONFIG_SPEEXENC_BITRATE, codecp->bitrate, codecp->denoiser);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_g722(struct sm_vmptx_codec_g722_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts;
 if (!codecp->packetlen || codecp->packetlen % 160) return err(ERR_SM_BAD_PARAMETER);
 lock_object(vmptx);
 grv = &vmptx->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_TIMESTAMPRATE, 2), 0);
 if (!sts) sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_G722ENC, codecp->payload_type, 0, 0, kSMVMPTxVADModeDisabled);
 if (!sts && codecp->payload_type != -1)  sts = grv->sendmsg(grv, MESSAGE(MSG_TIMESTAMPRATE, 2), 8000);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_G722, codecp->payload_type, codecp->packetlen);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_G722ENC, codecp->payload_type, codecp->packetlen / 16);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_g726(struct sm_vmptx_codec_g726_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 int pack_mode;
 switch (codecp->variant) {
	case kSMG726VariantRFC3551:
		pack_mode = MSG_G726_PACKING_RFC3551;
		break;
	case kSMG726VariantAAL2:
		pack_mode = MSG_G726_PACKING_AAL2;
		break;
	default:
		return err(ERR_SM_BAD_PARAMETER);
 }
 if (!codecp->packetlen || codecp->bits < 2 || codecp->bits > 5 || codecp->packetlen % 80) return err(ERR_SM_BAD_PARAMETER);
 lock_object(vmptx);
 grv = &vmptx->groove;
 if (!sts) sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_G726ENC, codecp->payload_type, 0, 1, codecp->VADMode);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_G726, codecp->payload_type, codecp->packetlen);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_G726ENC, codecp->payload_type, codecp->packetlen / 8);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_G726_BITS, 2), codecp->bits);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_G726_PACKING, 2), pack_mode);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_ilbc(struct sm_vmptx_codec_ilbc_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 if (codecp->frame_len != 20 && codecp->frame_len != 30) return err(ERR_SM_BAD_PARAMETER);
 if (!codecp->frames_per_packet) return err(ERR_SM_BAD_PARAMETER);
 lock_object(vmptx);
 grv = &vmptx->groove;
 if (!sts) sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_ILBCENC, codecp->payload_type, 0, 1, codecp->VADMode);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_ILBC, codecp->payload_type, codecp->frames_per_packet);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_ILBCENC, codecp->payload_type, codecp->frames_per_packet * codecp->frame_len);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ILBC_FRAME_LENGTH, 2), codecp->frame_len);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_smv(struct sm_vmptx_codec_smv_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 if (!codecp->frames_per_packet) return err(ERR_SM_BAD_PARAMETER);
 lock_object(vmptx);
 grv = &vmptx->groove;
 if (!sts) sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_SMVENC, codecp->payload_type, 0, 0, codecp->VADMode);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADD_CODEC, 4), RTP_CODEC_TYPE_SMV, codecp->payload_type, codecp->frames_per_packet);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_SMVENC, codecp->payload_type, codecp->frames_per_packet * 20);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmprx_config_dataloss(struct sm_vmprx_dataloss_parms *dlp)
{
 const tSMVMPrxId vmprx = dlp->vmprx;
 tSMGroove *grv;
 int sts;
 lock_object(vmprx);
 grv = &vmprx->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_VMPRX_DATALOSS, 2), dlp->loss_threshold);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vidmprx_config_dataloss(struct sm_vidmprx_dataloss_parms *dlp)
{
 const tSMVidMPrxId vidmprx = dlp->vidmprx;
 tSMGroove *grv;
 int sts;
 lock_object(vidmprx);
 grv = &vidmprx->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_VMPRX_DATALOSS, 2), dlp->loss_threshold);
 unlock_object(vidmprx);
 return sts;
}

STATIC int real_sm_vmptx_config_isac_rate(struct sm_vmptx_isac_rate_parms *isac_ratep)
{
 tSMVMPtxId vmptx = isac_ratep->vmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmptx);
 grv = &vmptx->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_iSAC_RATE, 3), isac_ratep->rate, isac_ratep->ptime);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_sample_rate(struct sm_vmptx_sample_rate_parms *sample_ratep)
{
 tSMVMPtxId vmptx = sample_ratep->vmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmptx);
 grv = &vmptx->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_SAMPLERATE, 2), sample_ratep->sample_rate);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmprx_config_tones(struct sm_vmprx_tone_parms *tonep)
{
 tSMGroove *grv;
 int sts;
 lock_object(tonep->vmprx);
 grv = &tonep->vmprx->groove;
 sts = grv->sendmsg(grv,MESSAGE(MSG_DELSUB,2),SUBTASKTC_VMPRXTONEGEN);
 if (tonep->regen_tones || tonep->detect_tones) {
	if (!sts) sts = grv->sendmsg(grv,MESSAGE(MSG_ADDVMPSUB,2),SUBTASKTC_VMPRXTONEGEN);
 }
 if (!sts) sts = grv->sendmsg(grv,MESSAGE(MSG_CONFTONE_RX,3),tonep->regen_tones,tonep->detect_tones);
 if (!sts) sts = grv->sendmsg(grv,MESSAGE(MSG_CONFTONE_ENFORCE,2),tonep->enforce_tone_spacing);
 unlock_object(tonep->vmprx);
 return sts; 
}

STATIC int real_sm_vmptx_config_tones(struct sm_vmptx_tone_parms *tonep)
{
 tSMVMPTxToneSetId toneid = tonep->tone_set_id;
 tSMGroove *grv;
 int sts = 0;
 int hyst;
 switch (tonep->min_duration) {
 case kSMVMPtxRFC2833MinDurationNoMinimum:
	hyst = 0;
	break;
 case kSMVMPtxRFC2833MinDuration40:
	hyst = 1;
	break;
 default:
	return err(ERR_SM_BAD_PARAMETER);
 }
 lock_object(tonep->vmptx);
 grv = &tonep->vmptx->groove;
 sts = grv->sendmsg(grv,MESSAGE(MSG_DELSUB,2),SUBTASKTC_TONEDETVMPTX);
 if (tonep->convert_tones || tonep->elim_tones) {
	if (!sts) sts = grv->sendmsg(grv,MESSAGE(MSG_ADDSUB,2),SUBTASKTC_TONEDETVMPTX);
	if (!sts) sts = grv->setfriend(grv,&toneid->groove,FRIEND_TYPE_TONE_DETPAR);
 }
 if (!sts) sts = grv->sendmsg(grv,MESSAGE(MSG_SET_HYST,2), hyst);
 if (!sts) sts = grv->sendmsg(grv,MESSAGE(MSG_CONFTONE_TX,3),tonep->convert_tones,tonep->elim_tones);
 if (!sts) {
	if (tonep->vmptx->toneset) vmptone_dtor(tonep->vmptx->toneset);
	if (toneid != kSMNullVMPTxToneSetId) {
		refcnt_inc(&toneid->ref);
	}
	tonep->vmptx->toneset = toneid;
 }
 unlock_object(tonep->vmptx);
 return sts;
}

STATIC int real_sm_vmptx_generate_tones(struct sm_vmptx_generate_tones_parms *tonep)
{
 tSMGroove *grv;
 int i, sts = 0;
 lock_object(tonep->vmptx);
 grv = &tonep->vmptx->groove;
 if (tonep->duration % 10 || tonep->interval % 10) {
	sts = err(ERR_SM_BAD_PARAMETER);
 }
 if (tonep->num < 1 || ! tonep->tones) {
	sts = err(ERR_SM_BAD_PARAMETER);
 }
 if (tonep->volume > 0 || tonep->volume < -15) {
	sts = err(ERR_SM_BAD_PARAMETER);
 }
 if (!sts) {
	sts = grv->sendmsg(grv,MESSAGE(MSG_TONE_PARAMS, 3), tonep->duration,tonep->interval);
	for(i=0;i<tonep->num;i++) {
		grv->sendmsg(grv,MESSAGE(MSG_TONE_GEN,3),tonep->tones[i]+1, tonep->volume);
		grv->sendmsg(grv,MESSAGE(MSG_TONE_GEN,3),i == tonep->num -1 ? -1: 0,0);
	}
 }
 unlock_object(tonep->vmptx);
 return sts;
}

STATIC int real_sm_vmptx_generate_tones_abort(struct sm_vmptx_generate_tones_abort_parms *tp)
{
 tSMGroove *grv;
 int sts;
 lock_object(tp->vmptx);
 grv = &tp->vmptx->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_TONE_GEN_ABORT,1));
 unlock_object(tp->vmptx);
 return sts;
 
}

STATIC int real_sm_vmptx_create_toneset(struct sm_vmptx_create_toneset_parms* tonesetp)
{
	// create the detpar
 union {
	float f;
	U32 l;
 } u;
 struct module *mod = modid2lockedmodule(tonesetp->module);
 tSMVMPTxToneSetId toneid;
 tSMGroove *grv; 
 U32 parms[4];
 int sts = 0;
 int t;
 if (!mod) return err(ERR_SM_NO_SUCH_MODULE);
 toneid = init_vmptone(mod);
 if (!toneid) sts = err(ERR_SM_NO_RESOURCES);
 tonesetp->tone_set_id = toneid;
 grv = &toneid->groove;
 if (!sts) sts = grv->starttask(grv, TASKTC_TONEDETPAR, 0);
 if (!sts) {
	// ok lets send the relavent msgs.
	u.f = tonesetp->toneset->req_third_peak;
	parms[0] = u.l;
	u.f = tonesetp->toneset->req_signal_to_noise_ratio;
	if (u.f > 1) {
	// it's in dB
		/* S = N * 10^(x/10)
		 * value required is S / (S + N) = 1 / (1 + N/S)
		 * N/S = N / (N * 10^(x/10)) = 10^(-x/10)
		 */
		u.f = 1 / (1 + pow(10.0, -u.f / 10.0));
	}
	parms[1] = u.l;
	u.f = tonesetp->toneset->req_minimum_power;
	if (u.f <= 100) {
		// it's in dBm0
		/* magic number 4.5e11 is determined by experiment
		 * the actual value depends on the frequency (if it hits
		 * the centre of a bin the value is slightly bigger than the
		 * edge of a bin) varying between 4.5e11 and 5.5e11 for 0dBm0
		 * We pick the lower limit as we want to guarantee detection
		 * at the selected level.
		 */
		u.f = 4.5e11 * pow(10.0,  u.f/10.0);
	}
	parms[2] = u.l;
	u.f = tonesetp->toneset->req_twist_for_dual_tone;
	parms[3]= u.l;
	sts = grv->sendmsg(grv,MESSAGE(MSG_SET_GENPAR,5),parms[0],parms[1],parms[2],parms[3]);
	if (!sts) sts = grv->sendmsg(grv,MESSAGE(MSG_TONES_NARROW,2), 0);
	// now we do the tones.
	if (!sts) {
		sts = grv->sendmsg(grv,MESSAGE(MSG_TONEBAND,3),MOD_DEF_DET_TONE_START,MOD_DEF_DET_TONE_STOP);
		if (!sts) {
			for(t = tonesetp->toneset->number_of_tones;t--;){
				u.f = tonesetp->toneset->tones[t].f1_min;
				parms[0] = u.l;
				u.f = tonesetp->toneset->tones[t].f1_max;
				parms[1] = u.l;
				u.f = tonesetp->toneset->tones[t].f2_min;
				parms[2] = u.l;
				u.f = tonesetp->toneset->tones[t].f2_max;
				parms[3] = u.l;
				sts = grv->sendmsg(grv,MESSAGE(MSG_SET_DETTONE,6),tonesetp->toneset->tones[t].id,parms[0],parms[1],parms[2],parms[3]);
				if (sts)  break;
			}
		}
	}
 }
 if (sts) vmptone_dtor(toneid); 
 unlock_module(mod);
 return sts;
}

STATIC int real_sm_vmptx_destroy_toneset(struct sm_vmptx_destroy_toneset_parms *tonep)
{
 if (tonep->tone_set_id) vmptone_dtor(tonep->tone_set_id);
 return 0;
}

STATIC int real_sm_vmptx_config_encryption_aes_cm(struct sm_vmptx_config_encryption_aes_cm_parms *pp)
{
 const tSMVMPtxId vmptx = pp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmptx);
 grv = &vmptx->groove;
 if (vmptx->encsubtask) {
	sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), vmptx->encsubtask);
 }
 vmptx->encsubtask = SUBTASK_SRTP_ICM_ENC;
 sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), vmptx->encsubtask);
 if (!sts) {
	sts =  grv->sendmsgstring(grv,  MSGSTRING_ID(MSGSTRING_ICME, 1),
		pp->keylen, pp->key);
 }
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_encryption_aes_f8(struct sm_vmptx_config_encryption_aes_f8_parms *pp)
{
 const tSMVMPtxId vmptx = pp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmptx);
 grv = &vmptx->groove;
 if (vmptx->encsubtask) {
	sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), vmptx->encsubtask);
 }
 vmptx->encsubtask = SUBTASK_SRTP_CBC_ENC;
 sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), vmptx->encsubtask);
 if (!sts) {
	sts =  grv->sendmsgstring(grv,  MSGSTRING_ID(MSGSTRING_CBCE, 1),
		pp->keylen, pp->key);
 }
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_config_encryption_null(struct sm_vmptx_config_encryption_null_parms *pp)
{
 const tSMVMPtxId vmptx = pp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmptx);
 grv = &vmptx->groove;
 if (vmptx->encsubtask) {
	sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), vmptx->encsubtask);
	vmptx->encsubtask = 0;
 }
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmprx_config_forwarding(struct sm_vmprx_config_forwarding_parms *configp)
{
 const tSMVMPrxId vmprx = configp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 if (configp->dest_type != kSMVMPrxFwdDestTypeIPv4 && configp->dest_type != kSMVMPrxFwdDestTypeIPv6) {
	 return err(ERR_SM_BAD_PARAMETER);
 }
 lock_object(vmprx);
 grv = &vmprx->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), SUBTASKTC_VMPRXFWD);
 if (!sts && configp->mode != kVMPrxFwdModeNone) {
	sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_VMPRXFWD);
	if (!sts) {
		if (configp->mode == kVMPrxFwdModeTypes) {
			int i;
			if (configp->num_types <= 0 || configp->num_types > kVMPrxFwdMaxTypes) {
				sts = err(ERR_SM_BAD_PARAMETER);
			} else {
				for (i = 0; i < configp->num_types; i++) {
					uint32_t in_pt, out_pt, trans = 0;
					in_pt = configp->types[i].incoming_pt;
					out_pt = configp->types[i].outgoing_pt;
					if ((in_pt & ~0x7f) || (out_pt & ~0x7f)) {
						sts = err(ERR_SM_BAD_PARAMETER);
						break;
					}
					if (!sts) switch (configp->types[i].transcoding) {
					case kVMPrxFwdTranscodingNone:
						trans = VMPRXFWD_TRANS_NONE;
						break;
					case kVMPrxFwdTranscodingAlawToMulaw:
						trans = VMPRXFWD_TRANS_ATOU;
						break;
					case kVMPrxFwdTranscodingMulawToAlaw:
						trans = VMPRXFWD_TRANS_UTOA;
						break;
					default:
						sts = err(ERR_SM_BAD_PARAMETER);
						break;
					}
					if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_VMPRXFWD_ADD_TYPE, 4), in_pt,	out_pt, trans);
					if (sts) break;
				}
			}
		} else if (configp->mode != kVMPrxFwdModeAll) {
			sts = err(ERR_SM_BAD_PARAMETER);
		}
		if (!sts) {
			// do this last as the firmware can only start forwarding once it knows where to forward to!
			if (configp->dest_type == kSMVMPrxFwdDestTypeIPv4) {
				sts = grv->sendmsg(grv, MESSAGE(MSG_VMPRXFWD_SET_OUTIPV4, 6),
					ip2msg(configp->u.ipv4.destination.sin_addr.s_addr),
					ntohs(configp->u.ipv4.destination.sin_port),
					ip2msg(configp->u.ipv4.source.sin_addr.s_addr),
					ntohs(configp->u.ipv4.source.sin_port),
					configp->u.ipv4.TOS);
			} else {
				sts = grv->sendmsg(grv, MESSAGE(MSG_VMPRXFWD_SET_OUTIPV6, 12),
					ipv6tomsg(configp->u.ipv6.destination.sin6_addr),
					ntohs(configp->u.ipv6.destination.sin6_port),
					ipv6tomsg(configp->u.ipv6.source.sin6_addr),
					ntohs(configp->u.ipv6.source.sin6_port),
					0);
			}
		}
		if (sts) {
			grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), SUBTASKTC_VMPRXFWD); // config went wrong so remove subtask
		}
	}
 }
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_jitter(struct sm_vmprx_jitter_parms *jitterp)
{
 tSMVMPrxId vmprx = jitterp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 if (jitterp->max_delay_ms <= jitterp->initial_delay_ms) {
	return err(ERR_SM_BAD_PARAMETER);
 }
 lock_object(vmprx);
 grv = &vmprx->groove;
 sts = grv->sendmsg(grv,MESSAGE(MSG_JBCREATE,4),10,jitterp->initial_delay_ms,jitterp->max_delay_ms);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vidmprx_config_jitter(struct sm_vidmprx_jitter_parms *jitterp)
{
 tSMVidMPrxId vidmprx = jitterp->vidmprx;
 tSMGroove *grv;
 int sts = 0;
 if (jitterp->max_delay_ms <= jitterp->initial_delay_ms) {
	return err(ERR_SM_BAD_PARAMETER);
 }
 lock_object(vidmprx);
 grv = &vidmprx->groove;
 sts = grv->sendmsg(grv,MESSAGE(MSG_JBCREATE,4),10,jitterp->initial_delay_ms,jitterp->max_delay_ms);
 unlock_object(vidmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_jitter_mode(struct sm_vmprx_config_jitter_mode_parms *jitterp)
{
 tSMVMPrxId vmprx = jitterp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 tSM_UT32 mode = 0, param0 = 0, param1 = 0, param2 = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 switch(jitterp->mode) {
	case kSMVMPrxJitterBufferModeNormal:
		mode = JBMODE_NORMAL;
		break;
	case kSMVMPrxJitterBufferModeAdaptive:
		// impose some limits on tolerance values
		if (jitterp->u.adaptive.freq_upper_tolerance < 0 
			|| jitterp->u.adaptive.freq_upper_tolerance >= 100
			|| jitterp->u.adaptive.freq_lower_tolerance < 0
			|| jitterp->u.adaptive.freq_lower_tolerance >= 100) {
			sts = err(ERR_SM_BAD_PARAMETER);
		}
		mode = JBMODE_ADAPTIVE;
		param0 = jitterp->u.adaptive.target_delay;
		param1 = jitterp->u.adaptive.freq_upper_tolerance * 655.36;
		param2 = jitterp->u.adaptive.freq_lower_tolerance * 655.36;
		break;
	default:
		sts = err(ERR_SM_BAD_PARAMETER);
		break;
 }
 if (!sts) sts = grv->sendmsg(grv,MESSAGE(MSG_JBMODE,5),mode, param0, param1, param2);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_jitter_resync_notify(struct sm_vmprx_config_jitter_resync_notify_parms *notifyp)
{
 tSMVMPrxId vmprx = notifyp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 tSM_UT32 enable = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 enable = (notifyp->enable) ? 2 : 0; // 2 indicates two parameter result wanted (allows newps to work with oldlib which sent 1)
 if (!sts) sts = grv->sendmsg(grv,MESSAGE(MSG_JBRESYNC_NOTIFY,2),enable);
 unlock_object(vmprx);
 return sts;
}


STATIC int real_sm_vmprx_config_profile_specific( struct sm_vmprx_config_profile_specific_parms *psp )
{
 uint32_t profileSubTaskCode;
 tSMGroove *grv;
 int sts = 0;
 uint8_t* p = (uint8_t*)(psp->plugin);
 unsigned i,n,id;

 // construct type code to match plugin
 // allows different vmprxs to have different or multiple exthdr processing in future.
 profileSubTaskCode = SUBTASKTC_VMPRX_PROFILE_BASIS;
 if (p == 0) return ERR_SM_BAD_PARAMETER;
 n = (unsigned)strlen((char*)p);
 if ((n < 1) || (n > 3)) return ERR_SM_BAD_PARAMETER;
 for (i = 0; i < n; i++) {
 	profileSubTaskCode |= (((uint32_t) *(p+i))<<(i*8));
 }
 if (psp->paramlen < 0) return ERR_SM_BAD_PARAMETER;
 lock_object(psp->vmprx);
 grv = &psp->vmprx->groove;
 sts = grv->sendmsg(grv,MESSAGE(MSG_DELSUB,2),profileSubTaskCode);
 if (psp->enable) {
 	if (!sts) sts = grv->sendmsg(grv,MESSAGE(MSG_ADDSUB,2),profileSubTaskCode);
 	if ((!sts) && (psp->paramlen != 0)) {
 		id = MSGSTRING_ID(MSGSTRING_VMPRX_PROFILE, 0);

 		sts = grv->sendmsgstring(grv, id, psp->paramlen, psp->paramval);
 	}
 }
 unlock_object(psp->vmprx);
 return sts;
}

STATIC int real_sm_vmprx_status_profile_specific( struct sm_vmprx_status_profile_specific_parms *p )
{
 const tSMVMPrxId vmprx = p->vmprx;
 tSMGroove *grv;
 int sts = 0;
 unsigned len;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (!sts) sts = grv->rdmsgstring(grv, MSGSTRING_ID(MSGSTRING_VMPRX_PROFILE, p->notification_code), 0xffffffffu, 
 									p->max_length, p->payload, &len);
 if (!sts) {
 	if (len == ~0u) {
 		p->length = 0;
 	} else {
 		p->length = len;
 	}
 }
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmptx_config_profile_specific( struct sm_vmptx_config_profile_specific_parms *psp )
{
 uint32_t profileSubTaskCode;
 tSMGroove *grv;
 int sts = 0;
 uint8_t* p = (uint8_t*)(psp->plugin);
 unsigned i,n,id;

 // construct type code to match plugin
 // allows different vmptxs to have different or multiple exthdr processing in future.
 profileSubTaskCode = SUBTASKTC_VMPTX_PROFILE_BASIS;
 if (p == 0) return ERR_SM_BAD_PARAMETER;
 n = (unsigned)strlen((char*)p);
 if ((n < 1) || (n > 3)) return ERR_SM_BAD_PARAMETER;
 for (i = 0; i < n; i++) {
 	profileSubTaskCode |= (((uint32_t) *(p+i))<<(i*8));
 }
 if (psp->paramlen < 0) return ERR_SM_BAD_PARAMETER;
 lock_object(psp->vmptx);
 grv = &psp->vmptx->groove;
 sts = grv->sendmsg(grv,MESSAGE(MSG_DELSUB,2),profileSubTaskCode);
 if (psp->enable) {
 	if (!sts) sts = grv->sendmsg(grv,MESSAGE(MSG_ADDSUB,2),profileSubTaskCode);
 	if ((!sts) && (psp->paramlen != 0)) {
 		id = MSGSTRING_ID(MSGSTRING_VMPTX_PROFILE, 0);

 		sts = grv->sendmsgstring(grv, id, psp->paramlen, psp->paramval);
 	}
 }
 unlock_object(psp->vmptx);
 return sts;
}

STATIC int real_sm_vmptx_set_profile_specific( struct sm_vmptx_set_profile_specific_parms *psp )
{
 tSMGroove *grv;
 int sts = 0;
 unsigned id;

 if (psp->paramlen < 0) return ERR_SM_BAD_PARAMETER;
 lock_object(psp->vmptx);
 grv = &psp->vmptx->groove;
 if (psp->paramlen != 0) {
 	id = MSGSTRING_ID(MSGSTRING_VMPTX_PROFILE, 0);

 	sts = grv->sendmsgstring(grv, id, psp->paramlen, psp->paramval);
 }
 unlock_object(psp->vmptx);
 return sts;
}

STATIC int real_sm_vmprx_config_sample_rate(struct sm_vmprx_sample_rate_parms *sample_ratep)
{
 tSMVMPrxId vmprx = sample_ratep->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_SAMPLERATE, 2), sample_ratep->sample_rate);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_unhandled_payload_reporting(struct sm_vmprx_unhandled_payload_reporting_parms* pp)
{
 tSMVMPrxId vmprx = pp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 if (pp->delay < 0) {
	pp->delay = -1;
 }
 lock_object(vmprx);
 grv = &vmprx->groove;
 sts = grv->sendmsg(grv,MESSAGE(MSG_VMPRX_PAYLOAD,2),pp->delay);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vidmprx_config_unhandled_payload_reporting(struct sm_vidmprx_unhandled_payload_reporting_parms* pp)
{
 tSMVidMPrxId vidmprx = pp->vidmprx;
 tSMGroove *grv;
 int sts = 0;
 if (pp->delay < 0) {
	pp->delay = -1;
 }
 lock_object(vidmprx);
 grv = &vidmprx->groove;
 sts = grv->sendmsg(grv,MESSAGE(MSG_VMPRX_PAYLOAD,2),pp->delay);
 unlock_object(vidmprx);
 return sts;
}

STATIC int unsafe_sm_rtcphand_status(struct sm_rtcphand_status_parms *statusp)
{
 tSMRTCPHandId rtcphand = statusp->rtcphand;
 tSMGroove *grv = &rtcphand->groove;
 for (;;) {	// might as well discard irrelevant messages
	uint32_t msg[16];
	unsigned long code; 
	int sts = grv->rdmsg(grv, msg, SMGROOVE_RDMSG_FLAG_NOWAIT);
	if (sts) return sts;
	code = msg[0] & 0xffff0000;	
	if (!code) {
		statusp->status = kSMRTCPHandStatusRunning;
		return 0;
	}
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
		return err(ERR_SM_DEVERR);
	} else if (code == MESSAGE(MSG_BADMSG, 3)) {
		if ((msg[1] & 0xffff0000) == MESSAGE(MSG_CREATE, 3)
			|| (msg[1] & 0xffff0000) == MESSAGE(MSG_ADDSUB, 2)) {
			switch (msg[2]) {
			case MSG_BADMSG_CAUSE_NOTASK:
				return err(ERR_SM_NO_SUCH_CHANNEL);
			case MSG_BADMSG_CAUSE_UNHANDLED:
				if ((msg[1] & 0xffff0000) == MESSAGE(MSG_ADDSUB, 2)) break; // v2 PX rtcp firmware doesn't handle addsubs
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
 }
}

STATIC int real_sm_rtcphand_status(struct sm_rtcphand_status_parms *statusp)
{
 tSMRTCPHandId rtcphand = statusp->rtcphand;
 int sts;
 lock_object(rtcphand);
 sts = unsafe_sm_rtcphand_status(statusp);
 unlock_object(rtcphand);
 return sts;
}

STATIC int unsafe_sm_vmprx_status(struct sm_vmprx_status_parms *statusp)
{
 tSMVMPrxId vmprx = statusp->vmprx;
 tSMGroove *grv = &vmprx->groove;
 int sts;
 if (vmprx->state == VMPRX_STATE_IDLE) return err(ERR_SM_WRONG_CHANNEL_STATE);
 for (;;) {	// might as well discard irrelevant messages
	uint32_t msg[16];
	unsigned long code; 
	sts = grv->rdmsg(grv, msg, SMGROOVE_RDMSG_FLAG_NOWAIT|SMGROOVE_RDMSG_FLAG_KEEPSTRINGS);
	if (sts) return sts;
	code = msg[0] & 0xffff0000;	
	if (!code) {
		statusp->status = kSMVMPrxStatusRunning;
		statusp->u.run.no_data = !vmprx->hasdata;
		return 0;
	}
	if (code == MESSAGE(MSG_GET_UDPPORTS, 4) && !vmprx->is_ipv6) {
		vmprx->udpports[0] = statusp->u.ports.RTP_Port = msg[1];
		vmprx->udpports[1] = statusp->u.ports.RTCP_Port = msg[2];
		vmprx->address.ipv4.s_addr = msg2ip(msg[3]);
		statusp->u.ports.address = vmprx->address.ipv4;
		statusp->status = kSMVMPrxStatusGotPorts;
		return 0;
	} else if (code == MESSAGE(MSG_GET_UDPPORTS, 7) && vmprx->is_ipv6) {
		vmprx->udpports[0] = statusp->u.ports_ipv6.RTP_Port = msg[1];
		vmprx->udpports[1] = statusp->u.ports_ipv6.RTCP_Port = msg[2];
		msgtoipv6(&vmprx->address.ipv6, msg[3], msg[4], msg[5], msg[6]);
		statusp->u.ports_ipv6.address = vmprx->address.ipv6;
		statusp->status = kSMVMPrxStatusGotPortsIPv6;
		return 0;
	} else if (code == MESSAGE(MSG_RTP_SSRC, 4)) {
		statusp->u.ssrc.ssrc = msg[1];
		statusp->u.ssrc.address.s_addr = msg2ip(msg[2]);
		statusp->u.ssrc.port = msg[3];
		statusp->status = kSMVMPrxStatusNewSSRC;
		return 0;
	} else if (code == MESSAGE(MSG_RTP_SSRC, 7)) {
		statusp->u.ssrc_ipv6.ssrc = msg[1];
		msgtoipv6(&statusp->u.ssrc_ipv6.address, msg[2], msg[3], msg[4], msg[5]);
		statusp->u.ssrc_ipv6.port = msg[6];
		statusp->status = kSMVMPrxStatusNewSSRCIPv6;
		return 0;
	} else if (MSG2ANYCODE(code) == MSG_DEAD) {
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
			case MSG_DEAD_CAUSE_LICENCE:
				return err(ERR_SM_NO_LICENCE);
			}
		}
		vmprx->state = VMPRX_STATE_IDLE;
		statusp->status = kSMVMPrxStatusStopped;
		return 0;
	} else if (code == MESSAGE(MSG_BADMSG, 3)) {
		if ((msg[1] & 0xffff0000) == MESSAGE(MSG_CREATE, 3)
			|| (msg[1] & 0xffff0000) == MESSAGE(MSG_ADDSUB, 2)
			|| (msg[1] & 0xffff0000) == MESSAGE(MSG_ADDVMPSUB, 2)) {
			switch (msg[2]) {
			case MSG_BADMSG_CAUSE_NOTASK:
				return err(ERR_SM_NO_SUCH_CHANNEL);
			case MSG_BADMSG_CAUSE_UNHANDLED:
				if ((msg[1] & 0xffff0000) == MESSAGE(MSG_ADDVMPSUB, 2)) break; // addvmpsub not handled by v2 PX firmware
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
	} else if (code == MESSAGE(MSG_DETECT_TONE , 3)) {
			// detected a tone
		statusp->status = kSMVMPrxStatusDetectTone;
		statusp->u.tone.id = msg[1];
		statusp->u.tone.volume = -(double)msg[2];
		statusp->u.tone.duration = 0;
		return 0;
	} else if (code == MESSAGE(MSG_DETECT_TONE , 4)) {
			// detected a tone
		statusp->status = kSMVMPrxStatusEndTone;
		statusp->u.tone.id = msg[1];
		statusp->u.tone.volume = -(double)msg[2];
		statusp->u.tone.duration = msg[3];
		return 0;
		
	} else if (code == MESSAGE(MSG_VMPRX_DATALOSS, 2)) {
		vmprx->hasdata = msg[1];
		statusp->status = kSMVMPrxStatusRunning;
		statusp->u.run.no_data = !vmprx->hasdata;
		return 0;
	} else if (code == MESSAGE(MSG_VMPRX_PAYLOAD, 2)) {
		statusp->status = kSMVMPrxStatusUnhandledPayload;
		statusp->u.payload.type = msg[1];
		return 0;
	} else if (code == MESSAGE(MSG_STRING, 3)) {
		if (MSGSTRING_ID2UID(msg[1]) == MSGSTRING_VMPRX_CODEC) {
			statusp->status = kSMVMPrxStatusCodecSpecific;
			statusp->u.codec_specific.payload_type = MSGSTRING_ID2STRNO(msg[1]);
			return 0;
		} else if (MSGSTRING_ID2UID(msg[1]) == MSGSTRING_VMPRX_PROFILE) {
			statusp->status = kSMVMPrxStatusProfileSpecific;
			statusp->u.profile_specific.notification_code = MSGSTRING_ID2STRNO(msg[1]);
			return 0;
		} else {
			sts = grv->rdmsgstring(grv, msg[1], 0xffffffffu, 0, 0, 0); // discard
			if (sts) return (sts);
		}
	} else if (code == MESSAGE(MSG_GETLIC_INFO, 2)) {
		if (msg[1] == 0) {
			return err(ERR_SM_NO_LICENCE);
		}
	} else if (code == MESSAGE(MSG_STUN_RECV_BIND, 7) || code == MESSAGE(MSG_STUN_RECV_BIND, 10)) {
		statusp->status = kSMVMPrxStatusSTUNBindRequest;
		statusp->u.bind_request.recv_port = msg[1];
		statusp->u.bind_request.port = msg[5];
		statusp->u.bind_request.priority = msg[3];
		statusp->u.bind_request.use_candidate = msg[4] == 0 ? 0 : 1;
		statusp->u.bind_request.remote_is_controlling = msg[2] == STUN_ROLE_CONTROLLING ? 1 : 0;
		if (code == MESSAGE(MSG_STUN_RECV_BIND, 7)) {
			statusp->u.bind_request.address_type = kSMAddressTypeIPv4;
			statusp->u.bind_request.address.ipv4.s_addr = msg2ip(msg[6]);
		} else {
			statusp->u.bind_request.address_type = kSMAddressTypeIPv6;
			msgtoipv6(&statusp->u.bind_request.address.ipv6, msg[6], msg[7], msg[8], msg[9]);
		}
		return 0;
	} else if (code == MESSAGE(MSG_STUN_BIND_RESULT, 10) || code == MESSAGE(MSG_STUN_BIND_RESULT, 13)) {
		uint8_t *ptid = (uint8_t*)&statusp->u.bind_response.transaction_id[0];
		*ptid++ = (msg[2] >> 24) & 0xffu;
		*ptid++ = (msg[2] >> 16) & 0xffu;
		*ptid++ = (msg[2] >> 8) & 0xffu;
		*ptid++ = msg[2] & 0xffu;
		*ptid++ = (msg[3] >> 24) & 0xffu;
		*ptid++ = (msg[3] >> 16) & 0xffu;
		*ptid++ = (msg[3] >> 8) & 0xffu;
		*ptid++ = msg[3] & 0xffu;
		*ptid++ = (msg[4] >> 24) & 0xffu;
		*ptid++ = (msg[4] >> 16) & 0xffu;
		*ptid++ = (msg[4] >> 8) & 0xffu;
		*ptid++ = msg[4] & 0xffu;
		statusp->u.bind_response.recv_port = msg[1];
		statusp->status = kSMVMPrxStatusSTUNBindResponse;
		switch(msg[5]) {
		case STUN_BIND_RESULT_SUCCESS:
			statusp->u.bind_response.result = kSMVMPrxSTUNResultSuccess;
			break;
		case STUN_BIND_RESULT_ERROR:
			statusp->u.bind_response.result = kSMVMPrxSTUNResultError;
			break;
		default:// treat unknown results as invalid
		case STUN_BIND_RESULT_INVALID:
			statusp->u.bind_response.result = kSMVMPrxSTUNResultInvalid;
			break;
		case STUN_BIND_RESULT_TIMEOUT:
			statusp->u.bind_response.result = kSMVMPrxSTUNResultTimeout;
			break;
		}
		statusp->u.bind_response.error_code = msg[6];
		statusp->u.bind_response.local_was_controlling = msg[7] == STUN_ROLE_CONTROLLING ? 1 : 0;
		statusp->u.bind_response.port = msg[8];
		if (code == MESSAGE(MSG_STUN_BIND_RESULT, 10)) {
			statusp->u.bind_response.address_type = kSMAddressTypeIPv4;
			statusp->u.bind_response.address.ipv4.s_addr = msg2ip(msg[9]);
		} else {
			statusp->u.bind_response.address_type = kSMAddressTypeIPv6;
			msgtoipv6(&statusp->u.bind_response.address.ipv6, msg[9], msg[10], msg[11], msg[12]);
		}
		return 0;
	} else if (code == MESSAGE(MSG_JBRESYNC_NOTIFY, 2)) {
		statusp->status = kSMVMPrxStatusJBResync;
		statusp->u.jbresync.timestamp = msg[1];
		statusp->u.jbresync.localtimeref = 0;
		return 0;
	} else if (code == MESSAGE(MSG_JBRESYNC_NOTIFY, 4)) {
		statusp->status = kSMVMPrxStatusJBResync;
		statusp->u.jbresync.timestamp = msg[1];
		statusp->u.jbresync.localtimeref = (((uint64_t)msg[3])<<32)+((uint64_t)msg[2]);
		return 0;
	}
 }
}

STATIC int real_sm_vmprx_status(struct sm_vmprx_status_parms *statusp)
{
 tSMVMPrxId vmprx = statusp->vmprx;
 int sts;
 lock_object(vmprx);
 sts = unsafe_sm_vmprx_status(statusp);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_status_discard_codec_specific(tSMVMPrxId vmprx)
{
 int sts;
 tSMGroove *grv;
 lock_object(vmprx);
 grv = &vmprx->groove;
 sts = grv->rdmsgstring(grv, MSGSTRING_ID(MSGSTRING_VMPRX_CODEC, 0), 0xffff0000u, 0, 0, 0); // discard
 unlock_object(vmprx);
 return sts;
}

STATIC int unsafe_sm_vidmprx_status(struct sm_vidmprx_status_parms *statusp)
{
 tSMVidMPrxId vidmprx = statusp->vidmprx;
 tSMGroove *grv = &vidmprx->groove;
 int sts;
 if (vidmprx->state == VIDMPRX_STATE_IDLE) return err(ERR_SM_WRONG_CHANNEL_STATE);
 for (;;) {	// might as well discard irrelevant messages
	uint32_t msg[16];
	unsigned long code; 
	sts = grv->rdmsg(grv, msg, SMGROOVE_RDMSG_FLAG_NOWAIT);
	if (sts) return sts;
	code = msg[0] & 0xffff0000;	
	if (!code) {
		statusp->status = kSMVidMPrxStatusRunning;
		statusp->u.run.no_data = !vidmprx->hasdata;
		return 0;
	}
	if (code == MESSAGE(MSG_GET_UDPPORTS, 4) && !vidmprx->is_ipv6) {
		vidmprx->udpports[0] = statusp->u.ports.RTP_Port = msg[1];
		vidmprx->udpports[1] = statusp->u.ports.RTCP_Port = msg[2];
		vidmprx->address.ipv4.s_addr = msg2ip(msg[3]);
		statusp->u.ports.address = vidmprx->address.ipv4;
		statusp->status = kSMVidMPrxStatusGotPorts;
		return 0;
	} else if (code == MESSAGE(MSG_GET_UDPPORTS, 7) && vidmprx->is_ipv6) {
		vidmprx->udpports[0] = statusp->u.ports_ipv6.RTP_Port = msg[1];
		vidmprx->udpports[1] = statusp->u.ports_ipv6.RTCP_Port = msg[2];
		msgtoipv6(&vidmprx->address.ipv6, msg[3], msg[4], msg[5], msg[6]);
		statusp->u.ports_ipv6.address = vidmprx->address.ipv6;
		statusp->status = kSMVidMPrxStatusGotPortsIPv6;
		return 0;
	} else if (code == MESSAGE(MSG_RTP_SSRC, 4)) {
		statusp->u.ssrc.ssrc = msg[1];
		statusp->u.ssrc.address.s_addr = msg2ip(msg[2]);
		statusp->u.ssrc.port = msg[3];
		statusp->status = kSMVidMPrxStatusNewSSRC;
		return 0;
	} else if (code == MESSAGE(MSG_RTP_SSRC, 7)) {
		statusp->u.ssrc_ipv6.ssrc = msg[1];
		msgtoipv6(&statusp->u.ssrc_ipv6.address, msg[2], msg[3], msg[4], msg[5]);
		statusp->u.ssrc_ipv6.port = msg[6];
		statusp->status = kSMVidMPrxStatusNewSSRCIPv6;
		return 0;
	} else if (MSG2ANYCODE(code) == MSG_DEAD) {
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
			case MSG_DEAD_CAUSE_LICENCE:
				return err(ERR_SM_NO_LICENCE);
			}
		}
		vidmprx->state = VIDMPRX_STATE_IDLE;
		statusp->status = kSMVidMPrxStatusStopped;
		return 0;
	} else if (code == MESSAGE(MSG_BADMSG, 3)) {
		if ((msg[1] & 0xffff0000) == MESSAGE(MSG_CREATE, 3)
			|| (msg[1] & 0xffff0000) == MESSAGE(MSG_ADDSUB, 2)
			|| (msg[1] & 0xffff0000) == MESSAGE(MSG_ADDVMPSUB, 2)) {
			switch (msg[2]) {
			case MSG_BADMSG_CAUSE_NOTASK:
				return err(ERR_SM_NO_SUCH_CHANNEL);
			case MSG_BADMSG_CAUSE_UNHANDLED:
				if ((msg[1] & 0xffff0000) == MESSAGE(MSG_ADDVMPSUB, 2)) break; // addvmpsub not handled by v2 PX firmware
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
	} else if (code == MESSAGE(MSG_VMPRX_DATALOSS, 2)) {
		vidmprx->hasdata = msg[1];
		statusp->status = kSMVidMPrxStatusRunning;
		statusp->u.run.no_data = !vidmprx->hasdata;
		return 0;
	} else if (code == MESSAGE(MSG_VMPRX_PAYLOAD, 2)) {
		statusp->status = kSMVidMPrxStatusUnhandledPayload;
		statusp->u.payload.type = msg[1];
		return 0;
	} else if (code == MESSAGE(MSG_INTERNAL_ERROR, 2)) {
		statusp->status = kSMVidMPrxStatusInternalError;
		statusp->u.internal_error.num_errors = msg[1];
		return 0;
	} else if (code == MESSAGE(MSG_GETLIC_INFO, 2)) {
		if (msg[1] == 0) {
			return err(ERR_SM_NO_LICENCE);
		}
	} else if (code == MESSAGE(MSG_STUN_RECV_BIND, 7) || code == MESSAGE(MSG_STUN_RECV_BIND, 10)) {
		statusp->status = kSMVidMPrxStatusSTUNBindRequest;
		statusp->u.bind_request.recv_port = msg[1];
		statusp->u.bind_request.port = msg[5];
		statusp->u.bind_request.priority = msg[3];
		statusp->u.bind_request.use_candidate = msg[4] == 0 ? 0 : 1;
		statusp->u.bind_request.remote_is_controlling = msg[2] == STUN_ROLE_CONTROLLING ? 1 : 0;
		if (code == MESSAGE(MSG_STUN_RECV_BIND, 7)) {
			statusp->u.bind_request.address_type = kSMAddressTypeIPv4;
			statusp->u.bind_request.address.ipv4.s_addr = msg2ip(msg[6]);
		} else {
			statusp->u.bind_request.address_type = kSMAddressTypeIPv6;
			msgtoipv6(&statusp->u.bind_request.address.ipv6, msg[6], msg[7], msg[8], msg[9]);
		}
		return 0;
	} else if (code == MESSAGE(MSG_STUN_BIND_RESULT, 10) || code == MESSAGE(MSG_STUN_BIND_RESULT, 13)) {
		uint8_t *ptid = (uint8_t*)&statusp->u.bind_response.transaction_id[0];
		*ptid++ = (msg[2] >> 24) & 0xffu;
		*ptid++ = (msg[2] >> 16) & 0xffu;
		*ptid++ = (msg[2] >> 8) & 0xffu;
		*ptid++ = msg[2] & 0xffu;
		*ptid++ = (msg[3] >> 24) & 0xffu;
		*ptid++ = (msg[3] >> 16) & 0xffu;
		*ptid++ = (msg[3] >> 8) & 0xffu;
		*ptid++ = msg[3] & 0xffu;
		*ptid++ = (msg[4] >> 24) & 0xffu;
		*ptid++ = (msg[4] >> 16) & 0xffu;
		*ptid++ = (msg[4] >> 8) & 0xffu;
		*ptid++ = msg[4] & 0xffu;
		statusp->u.bind_response.recv_port = msg[1];
		statusp->status = kSMVidMPrxStatusSTUNBindResponse;
		switch(msg[5]) {
		case STUN_BIND_RESULT_SUCCESS:
			statusp->u.bind_response.result = kSMVidMPrxSTUNResultSuccess;
			break;
		case STUN_BIND_RESULT_ERROR:
			statusp->u.bind_response.result = kSMVidMPrxSTUNResultError;
			break;
		default:// treat unknown results as invalid
		case STUN_BIND_RESULT_INVALID:
			statusp->u.bind_response.result = kSMVidMPrxSTUNResultInvalid;
			break;
		case STUN_BIND_RESULT_TIMEOUT:
			statusp->u.bind_response.result = kSMVidMPrxSTUNResultTimeout;
			break;
		}
		statusp->u.bind_response.error_code = msg[6];
		statusp->u.bind_response.local_was_controlling = msg[7] == STUN_ROLE_CONTROLLING ? 1 : 0;
		statusp->u.bind_response.port = msg[8];
		if (code == MESSAGE(MSG_STUN_BIND_RESULT, 10)) {
			statusp->u.bind_response.address_type = kSMAddressTypeIPv4;
			statusp->u.bind_response.address.ipv4.s_addr = msg2ip(msg[9]);
		} else {
			statusp->u.bind_response.address_type = kSMAddressTypeIPv6;
			msgtoipv6(&statusp->u.bind_response.address.ipv6, msg[9], msg[10], msg[11], msg[12]);
		}
		return 0;
	}
 }
}

STATIC int real_sm_vidmprx_status(struct sm_vidmprx_status_parms *statusp)
{
 tSMVidMPrxId vidmprx = statusp->vidmprx;
 int sts;
 lock_object(vidmprx);
 sts = unsafe_sm_vidmprx_status(statusp);
 unlock_object(vidmprx);
 return sts;
}

STATIC int unsafe_sm_vmptx_status(struct sm_vmptx_status_parms *statusp)
{
 tSMVMPtxId vmptx = statusp->vmptx;
 tSMGroove *grv = &vmptx->groove;
 if (vmptx->state == VMPTX_STATE_IDLE) return err(ERR_SM_WRONG_CHANNEL_STATE);
 for (;;) { // might as well discard irrelevant messages
	 uint32_t msg[16];
	 unsigned long code;
	 int sts;
	 sts = grv->rdmsg(grv, msg, SMGROOVE_RDMSG_FLAG_NOWAIT);
	 if (sts) return sts;
	 code = msg[0] & 0xffff0000;	
	 if (!code) {
		statusp->status = kSMVMPtxStatusRunning;
		return 0;
	 }
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
			case MSG_DEAD_CAUSE_LICENCE:
				return err(ERR_SM_NO_LICENCE);
			}
		}
		vmptx->state = VMPTX_STATE_IDLE;
		statusp->status = kSMVMPtxStatusStopped;
		return 0;
	 } else if (code == MESSAGE(MSG_BADMSG, 3)) {
		if ((msg[1] & 0xffff0000) == MESSAGE(MSG_CREATE, 3)
			|| (msg[1] & 0xffff0000) == MESSAGE(MSG_ADDSUB, 2)
			|| (msg[1] & 0xffff0000) == MESSAGE(MSG_ADDVMPSUB, 2)
			|| (msg[1] & 0xffff0000) == MESSAGE(MSG_CONFIG_CODEC, 4)) {
			switch (msg[2]) {
			case MSG_BADMSG_CAUSE_NOTASK:
				return err(ERR_SM_NO_SUCH_CHANNEL);
			case MSG_BADMSG_CAUSE_UNHANDLED:
				if ((msg[1] & 0xffff0000) == MESSAGE(MSG_ADDVMPSUB, 2)) break; // addvmpsub not handled by v2 PX firmware
				if ((msg[1] & 0xffff0000) == MESSAGE(MSG_CONFIG_CODEC, 4)) break; // config_codec not handled by v2 PX firmware
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
		} else if ((msg[1] & 0xffff0000) == MESSAGE(MSG_SET_OUTIPV4, 5)
					|| (msg[1] & 0xffff0000) == MESSAGE(MSG_SET_RTCPOUTIPV4, 5)) {
			if (msg[2] == MSG_BADMSG_CAUSE_RANGE) {
				return err(ERR_SM_BAD_PARAMETER);
			}
		}
	 } else if (code == MESSAGE(MSG_RTP_SSRC, 2)) {
		statusp->status = kSMVMPtxStatusSSRC;
		statusp->u.ssrc.ssrc = msg[1];
		return 0;
	 } else if (code == MESSAGE(MSG_TONE_GEN, 1)) {
		statusp->status = kSMVMPtxStatusToneCompleted;
		return 0;
	 } else if (code == MESSAGE(MSG_GETLIC_INFO, 2)) {
		if (msg[1] == 0) {
			return err(ERR_SM_NO_LICENCE);
		}
	 }
 }
}

STATIC int real_sm_vmptx_status(struct sm_vmptx_status_parms *statusp)
{
 int sts;
 lock_object(statusp->vmptx);
 sts = unsafe_sm_vmptx_status(statusp);
 unlock_object(statusp->vmptx);
 return sts;
}

STATIC int unsafe_sm_vidmptx_status(struct sm_vidmptx_status_parms *statusp)
{
 tSMVidMPtxId vidmptx = statusp->vidmptx;
 tSMGroove *grv = &vidmptx->groove;
 if (vidmptx->state == VIDMPTX_STATE_IDLE) return err(ERR_SM_WRONG_CHANNEL_STATE);
 for (;;) { // might as well discard irrelevant messages
	 uint32_t msg[16];
	 unsigned long code;
	 int sts;
	 sts = grv->rdmsg(grv, msg, SMGROOVE_RDMSG_FLAG_NOWAIT);
	 if (sts) return sts;
	 code = msg[0] & 0xffff0000;	
	 if (!code) {
		statusp->status = kSMVidMPtxStatusRunning;
		return 0;
	 }
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
			case MSG_DEAD_CAUSE_LICENCE:
				return err(ERR_SM_NO_LICENCE);
			}
		}
		vidmptx->state = VIDMPTX_STATE_IDLE;
		statusp->status = kSMVidMPtxStatusStopped;
		return 0;
	 } else if (code == MESSAGE(MSG_BADMSG, 3)) {
		if ((msg[1] & 0xffff0000) == MESSAGE(MSG_CREATE, 3)
			|| (msg[1] & 0xffff0000) == MESSAGE(MSG_ADDSUB, 2)
			|| (msg[1] & 0xffff0000) == MESSAGE(MSG_ADDVMPSUB, 2)
			|| (msg[1] & 0xffff0000) == MESSAGE(MSG_CONFIG_CODEC, 4)) {
			switch (msg[2]) {
			case MSG_BADMSG_CAUSE_NOTASK:
				return err(ERR_SM_NO_SUCH_CHANNEL);
			case MSG_BADMSG_CAUSE_UNHANDLED:
				if ((msg[1] & 0xffff0000) == MESSAGE(MSG_ADDVMPSUB, 2)) break; // addvmpsub not handled by v2 PX firmware
				if ((msg[1] & 0xffff0000) == MESSAGE(MSG_CONFIG_CODEC, 4)) break; // config_codec not handled by v2 PX firmware
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
		} else if ((msg[1] & 0xffff0000) == MESSAGE(MSG_SET_OUTIPV4, 5)
					|| (msg[1] & 0xffff0000) == MESSAGE(MSG_SET_RTCPOUTIPV4, 5)) {
			if (msg[2] == MSG_BADMSG_CAUSE_RANGE) {
				return err(ERR_SM_BAD_PARAMETER);
			}
		}
	 } else if (code == MESSAGE(MSG_RTP_SSRC, 2)) {
		statusp->status = kSMVidMPtxStatusSSRC;
		statusp->u.ssrc.ssrc = msg[1];
		return 0;
	 } else if (code == MESSAGE(MSG_INTERNAL_ERROR, 2)) {
		statusp->status = kSMVidMPtxStatusInternalError;
		statusp->u.internal_error.num_errors = msg[1];
		return 0;
	 } else if (code == MESSAGE(MSG_GETLIC_INFO, 2)) {
		if (msg[1] == 0) {
			return err(ERR_SM_NO_LICENCE);
		}
	}
 }
}

STATIC int real_sm_vidmptx_status(struct sm_vidmptx_status_parms *statusp)
{
 int sts;
 lock_object(statusp->vidmptx);
 sts = unsafe_sm_vidmptx_status(statusp);
 unlock_object(statusp->vidmptx);
 return sts;
}

STATIC int real_sm_rtcphand_get_event(struct sm_rtcphand_event_parms *eventp)
{
 tSMRTCPHandId rtcphand = eventp->rtcphand;
 int sts;
 lock_object(rtcphand);
 sts = rtcphand->groove.getev(&rtcphand->groove, &eventp->event);
 unlock_object(rtcphand);
 return sts;
}

STATIC int real_sm_vmprx_get_event(struct sm_vmprx_event_parms *eventp)
{
 tSMVMPrxId vmprx = eventp->vmprx;
 int sts;
 lock_object(vmprx);
 sts = vmprx->groove.getev(&vmprx->groove, &eventp->event);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vidmprx_get_event(struct sm_vidmprx_event_parms *eventp)
{
 tSMVidMPrxId vidmprx = eventp->vidmprx;
 int sts;
 lock_object(vidmprx);
 sts = vidmprx->groove.getev(&vidmprx->groove, &eventp->event);
 unlock_object(vidmprx);
 return sts;
}

STATIC int real_sm_vmptx_get_event(struct sm_vmptx_event_parms *eventp)
{
 tSMVMPtxId vmptx = eventp->vmptx;
 int sts;
 lock_object(vmptx);
 sts = vmptx->groove.getev(&vmptx->groove, &eventp->event);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vidmptx_get_event(struct sm_vidmptx_event_parms *eventp)
{
 tSMVidMPtxId vidmptx = eventp->vidmptx;
 int sts;
 lock_object(vidmptx);
 sts = vidmptx->groove.getev(&vidmptx->groove, &eventp->event);
 unlock_object(vidmptx);
 return sts;
}

STATIC int real_sm_vmprx_get_datafeed(struct sm_vmprx_datafeed_parms *datafeedp)
{
 tSMVMPrxId vmprx = datafeedp->vmprx;
 lock_object(vmprx);
 datafeedp->datafeed = vmprx->dataf.vtbl_data;
 unlock_object(vmprx);
 return 0;
}

STATIC int real_sm_vidmprx_get_datafeed(struct sm_vidmprx_datafeed_parms *datafeedp)
{
 tSMVidMPrxId vidmprx = datafeedp->vidmprx;
 lock_object(vidmprx);
 datafeedp->datafeed = vidmprx->dataf.vtbl_data;
 unlock_object(vidmprx);
 return 0;
}

STATIC int real_sm_vmptx_datafeed_connect(struct sm_vmptx_datafeed_connect_parms *datafeedp)
{
 tSMVMPtxId vmptx = datafeedp->vmptx;
 DATAFEED df;
 tSMGroove *grv;
 int sts;
 lock_object(vmptx);
 grv = &vmptx->groove;
 df.vtbl_data = datafeedp->data_source;
 sts = grv->connect_df(grv, &vmptx->dfin, df);
 if (!sts) sts = grv->engage_df(grv, df, DFC_IN);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vidmptx_datafeed_connect(struct sm_vidmptx_datafeed_connect_parms *datafeedp)
{
 tSMVidMPtxId vidmptx = datafeedp->vidmptx;
 DATAFEED df;
 tSMGroove *grv;
 int sts;
 lock_object(vidmptx);
 grv = &vidmptx->groove;
 df.vtbl_data = datafeedp->data_source;
 sts = grv->connect_df(grv, &vidmptx->dfin, df);
 if (!sts) sts = grv->engage_df(grv, df, DFC_IN);
 unlock_object(vidmptx);
 return sts;
}

STATIC int real_sm_vidmprx_config_codec_rfc3984(struct sm_vidmprx_codec_rfc3984_parms *codecp)
{
 const tSMVidMPrxId vidmprx = codecp->vidmprx;
 tSMGroove *grv;
 int sts, width, height;
 switch (codecp->pic_size) {
 case kSMRFC3984PicSizeSubQCIF:
	 width = 128, height = 96;
	 break;
 case 0: // default QCIF, drop through
 case kSMRFC3984PicSizeQCIF:
	 width = 176, height = 144;
	 break;
 case kSMRFC3984PicSizeCIF:
	 width = 352, height = 288;
	 break;
 case kSMRFC3984PicSize4CIF:
	 width = 704, height = 576;
	 break;
 case kSMRFC3984PicSize16CIF:
	 width = 1408, height = 1152;
	 break;
 default:
	 return err(ERR_SM_BAD_PARAMETER);
 }
 lock_object(vidmprx);
 grv = &vidmprx->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), SUBTASKTC_RFC3984DEC);
 if (codecp->payload_type != -1) {
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_RFC3984DEC);
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 3), SUBTASKTC_RFC3984DEC, codecp->payload_type);
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_RFC3984DEC_CFG, 3), width, height);
 }
 unlock_object(vidmprx);
 return sts;
}

STATIC int real_sm_vidmptx_config_codec_rfc3984(struct sm_vidmptx_codec_rfc3984_parms *codecp)
{
 const tSMVidMPtxId vidmptx = codecp->vidmptx;
 tSMGroove *grv;
 int sts;
 lock_object(vidmptx);
 grv = &vidmptx->groove;
 sts = unsafe_sm_vidmptx_config_codec(vidmptx, SUBTASKTC_RFC3984ENC, codecp->payload_type);
 // Note: first parm was repeat_ps, now removed. May re-use its space later. Space kept for better lib/fmw compat.
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_RFC3984ENC_CFG, 3), 0, codecp->deaggregate);
 unlock_object(vidmptx);
 return sts;
}

STATIC int real_sm_vidmprx_config_codec_rfc4629(struct sm_vidmprx_codec_rfc4629_parms *codecp)
{
 const tSMVidMPrxId vidmprx = codecp->vidmprx;
 tSMGroove *grv;
 int sts;
 lock_object(vidmprx);
 grv = &vidmprx->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), SUBTASKTC_RFC4629DEC);
 if (codecp->payload_type != -1) {
	tSM_INT h263_level = codecp->h263_level ? codecp->h263_level : D_H263_LEVEL;
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_RFC4629DEC);
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 3), SUBTASKTC_RFC4629DEC, codecp->payload_type);
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_RFC4629DEC_CFG, 3), codecp->h263_profile, h263_level);
 }
 unlock_object(vidmprx);
 return sts;
}

STATIC int real_sm_vidmptx_config_codec_rfc4629(struct sm_vidmptx_codec_rfc4629_parms *codecp)
{
 const tSMVidMPtxId vidmptx = codecp->vidmptx;
 tSMGroove *grv;
 int sts;
 lock_object(vidmptx);
 grv = &vidmptx->groove;
 sts = unsafe_sm_vidmptx_config_codec(vidmptx, SUBTASKTC_RFC4629ENC, codecp->payload_type);
 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_RFC4629ENC_CFG, 2), codecp->rem_ext_pic_hdr);
 unlock_object(vidmptx);
 return sts;
}

STATIC int real_sm_rtcphand_get_data(struct sm_rtcphand_get_data_parms *pp)
{
 const tSMRTCPHandId rtcphand = pp->rtcphand;
 tSMGroove *grv;
 unsigned done;
 int sts;
 lock_object(rtcphand);
 grv = &rtcphand->groove;
 sts = grv->read(grv, pp->data, pp->max_length, &done);
 pp->done_length = done;
 unlock_object(rtcphand);
 return sts;
}

STATIC int real_sm_vmptx_propagate_rtcp_sr_ntp(struct sm_vmptx_propagate_rtcp_sr_ntp_parms *propp)
{
 tSMGroove *grv;
 int sts = 0;
 uint32_t ntplsw = (uint32_t) propp->ntp_timestamp;
 uint32_t ntpmsw = (uint32_t) (propp->ntp_timestamp >> 32);
 uint32_t ltrlsw = (uint32_t) propp->localtimeref;
 uint32_t ltrmsw = (uint32_t) (propp->localtimeref >> 32);
 
 lock_object(propp->vmptx);
 grv = &propp->vmptx->groove;
 sts = grv->sendmsg(grv,MESSAGE(MSG_VMPTX_PROP_RTCP_SR_NTP, 5), ntplsw,ntpmsw,ltrlsw,ltrmsw);
 unlock_object(propp->vmptx);
 return sts;
}

STATIC int real_sm_vmptx_set_csrc(struct sm_vmptx_set_csrc_parms *csrcp)
{
 tSMGroove *grv;
 tSMGroove *fr = 0;
 int i, sts = 0;
 lock_object(csrcp->vmptx);
 grv = &csrcp->vmptx->groove;
 switch (csrcp->type) {
 case kSMVMPTxCSRCTypeArray:
	sts = grv->sendmsg(grv,MESSAGE(MSG_NEW_CSRCS, 2), csrcp->u.csrc_array.num_csrc);
	if (!sts) sts = grv->sendmsg(grv,MESSAGE(MSG_CSRCS_CONFIG, 5), csrcp->repeat_delay, csrcp->packet_count, 0, 0);
	for(i = 0; i < csrcp->u.csrc_array.num_csrc; i++) {
		if (!sts) sts = grv->sendmsg(grv,MESSAGE(MSG_ADD_CSRC,2),csrcp->u.csrc_array.csrc[i]);
	}
	break;
 case kSMVMPTxCSRCTypeList:
	if (csrcp->u.csrc_list.csrc_list_id) {
		lock_object(csrcp->u.csrc_list.csrc_list_id);
		fr = &csrcp->u.csrc_list.csrc_list_id->groove;
	}
	sts = grv->sendmsg(grv,MESSAGE(MSG_NEW_CSRCS, 2), 0); // zero out existing array
	if (!sts) sts = grv->sendmsg(grv,MESSAGE(MSG_CSRCS_CONFIG, 5), csrcp->repeat_delay, csrcp->packet_count, csrcp->u.csrc_list.exclude_ssrc, csrcp->u.csrc_list.ssrc);
	if (!sts) grv->setfriend(grv, fr, FRIEND_TYPE_CSRCLIST);
	if (csrcp->u.csrc_list.csrc_list_id) unlock_object(csrcp->u.csrc_list.csrc_list_id);
	break;
 default:
	sts = err(ERR_SM_BAD_PARAMETER);
	break;
 }
 unlock_object(csrcp->vmptx);
 return sts;
}

STATIC int real_sm_vmptx_create_csrc_list(struct sm_vmptx_create_csrc_list_parms *csrcp)
{
 struct module *mod = modid2lockedmodule(csrcp->module);
 tSMVMPTxCSRCListId csrc_list;
 int sts = 0;
 if (!mod) return err(ERR_SM_NO_SUCH_MODULE);
 csrc_list = init_csrc_list(mod);
 if (!csrc_list) sts = err(ERR_SM_NO_RESOURCES);
 if (!sts) {
	tSMGroove *grv = &csrc_list->groove;
	sts = grv->starttask(grv, TASKTC_VMPTXCSRC, 0);
	if (sts) csrc_list_dtor(csrc_list);
	else {
		make_object(csrc_list);
		csrcp->csrc_list = csrc_list;
	}
 }
 unlock_module(mod);
 return sts;
}

STATIC int real_sm_vmptx_csrc_list_set(struct sm_vmptx_csrc_list_set_parms *csrcp)
{
 tSMGroove *grv;
 int i, sts = 0;
 lock_object(csrcp->csrc_list);
 grv = &csrcp->csrc_list->groove;
 sts = grv->sendmsg(grv,MESSAGE(MSG_NEW_CSRCS, 2), csrcp->num_csrc);
 for(i = 0; i < csrcp->num_csrc; i++) {
	grv->sendmsg(grv,MESSAGE(MSG_ADD_CSRC,2),csrcp->csrc[i]);
 }
 unlock_object(csrcp->csrc_list);
 return sts;
}

STATIC int real_sm_vmptx_destroy_csrc_list(struct sm_vmptx_destroy_csrc_list_parms *csrcp)
{
	if (csrcp->csrc_list) csrc_list_dtor(csrcp->csrc_list);
	return 0;
}

STATIC int real_sm_vmptx_config_tag(struct sm_vmptx_tag_parms *tagp)
{
 tSMGroove *grv;
 int sts = 0;
 int type;
 switch (tagp->tag_type) {
 case kSMVMPTxTagTypeUlaw:
	type = MSG_CONFIG_TAG_MODE_ULAW;
	break;
 case kSMVMPTxTagTypeAlaw:
	type = MSG_CONFIG_TAG_MODE_ALAW;
	break;
 default:
	return err(ERR_SM_BAD_PARAMETER);
 }
 lock_object(tagp->vmptx);
 grv = &tagp->vmptx->groove;
 sts = grv->sendmsg(grv,MESSAGE(MSG_DELSUB,2),SUBTASKTC_VMPTXTAG);
 if (tagp->payload_type != -1) {
	if (!sts) sts = grv->sendmsg(grv,MESSAGE(MSG_ADDSUB,2),SUBTASKTC_VMPTXTAG);
	if (!sts) sts = grv->sendmsg(grv,MESSAGE(MSG_CONFIG_TAG,5),
		tagp->payload_type,
		type,
		tagp->min_time,
		tagp->max_time);
 }
 unlock_object(tagp->vmptx);
 return sts;
}

STATIC int real_sm_vmprx_config_codec_sse(struct sm_vmprx_codec_sse_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_SSERX, RTP_CODEC_TYPE_SSE, 0, codecp->payload_type, 0);
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_status_codec_sse(struct sm_vmprx_status_codec_sse_parms *p)
{
 const tSMVMPrxId vmprx = p->vmprx;
 tSMGroove *grv;
 int sts = 0;
 unsigned len;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (!sts) sts = grv->rdmsgstring(grv, MSGSTRING_ID(MSGSTRING_VMPRX_CODEC, p->payload_type), 0xffffffffu, 
									p->max_length, p->payload, &len);
 if (!sts) {
	if (len == ~0u) {
		p->status = kSMVMPrxSSEStatusNone;
	} else {
		p->status = kSMVMPrxSSEStatusMessage;
		p->length = len;
	}
 }
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_sse(struct sm_vmptx_codec_sse_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts;
 lock_object(vmptx);
 grv = &vmptx->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), SUBTASKTC_SSETX);
 if (codecp->payload_type != -1) {
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_SSETX);
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_SSETX, codecp->payload_type, 0);
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_REDUNDANCY, 4), SUBTASKTC_SSETX, codecp->delay, codecp->retransmissions);
 }
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmptx_send_sse(struct sm_vmptx_send_sse_parms *p)
{
 const tSMVMPtxId vmptx = p->vmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmptx);
 grv = &vmptx->groove;
 sts = grv->sendmsgstring(grv, MSGSTRING_ID(MSGSTRING_CODEC_SSE, 1), p->payload_length, p->payload);
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmprx_config_sprt(struct sm_vmprx_config_sprt_parms *configp)
{
 const tSMVMPrxId vmprx = configp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 if (configp->sprt) {
	lock_module(vmprx->mod);
	lock_object(configp->sprt);
 }
 lock_object(vmprx);
 grv = &vmprx->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB, 2), SUBTASKTC_SPRT_GRABBER);
 if (configp->payload_type != -1) {
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB, 2), SUBTASKTC_SPRT_GRABBER);
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 5), SUBTASKTC_SPRT_GRABBER, configp->payload_type, CODEC_PLC_DISABLE, 8000);
	if (!sts) sts = grv->setfriend(grv, &configp->sprt->groove, FRIEND_TYPE_SPRT);
 }
 unlock_object(vmprx);
 if (configp->sprt) {
	unlock_object(configp->sprt);
	unlock_module(vmprx->mod);
 }
 return sts;
}

STATIC int real_sm_vmprx_status_codec_amrnb(struct sm_vmprx_status_codec_amrnb_parms *p)
{
 const tSMVMPrxId vmprx = p->vmprx;
 tSMGroove *grv;
 int sts = 0;
 unsigned len;
 unsigned char buf[4];
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (!sts) sts = grv->rdmsgstring(grv, MSGSTRING_ID(MSGSTRING_VMPRX_CODEC, p->payload_type), 0xffffffffu, 
									4u, (char*)buf, &len);
 if (!sts) {
	if (len != 4) {
		p->status = kSMVMPrxAMRNBStatusNone;
	} else {
		unsigned bitrate = buf[0];
		bitrate += buf[1] << 8;
		bitrate += buf[2] << 16;
		bitrate += buf[3] << 24;
		p->u.cmr.bitrate = bitrate;
		p->status = kSMVMPrxAMRNBStatusNewCMR;
	}
 }
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_status_codec_amrwb(struct sm_vmprx_status_codec_amrwb_parms *p)
{
 const tSMVMPrxId vmprx = p->vmprx;
 tSMGroove *grv;
 int sts = 0;
 unsigned len;
 unsigned char buf[4];
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (!sts) sts = grv->rdmsgstring(grv, MSGSTRING_ID(MSGSTRING_VMPRX_CODEC, p->payload_type), 0xffffffffu, 
									4u, (char*)buf, &len);
 if (!sts) {
	if (len != 4) {
		p->status = kSMVMPrxAMRWBStatusNone;
	} else {
		unsigned bitrate = buf[0];
		bitrate += buf[1] << 8;
		bitrate += buf[2] << 16;
		bitrate += buf[3] << 24;
		p->u.cmr.bitrate = bitrate;
		p->status = kSMVMPrxAMRWBStatusNewCMR;
	}
 }
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_config_codec_rttext(struct sm_vmprx_codec_rttext_parms *codecp)
{
 const tSMVMPrxId vmprx = codecp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;

 if (codecp->out_of_order_delay < 0) sts = err(ERR_SM_BAD_PARAMETER);

 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_TIMESTAMPRATE, 2), 0); // remove the override
 if (!sts) sts = unsafe_sm_vmprx_config_codec(grv, SUBTASKTC_RTTEXTRX, RTP_CODEC_TYPE_UNKN, 0, codecp->payload_type, 0);
 if (codecp->payload_type != -1) {
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_TIMESTAMPRATE, 2), 1000);
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_RTTEXT_RX_CONFIG, 3), codecp->redundant_payload_type, codecp->out_of_order_delay);
 }
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmptx_config_codec_rttext(struct sm_vmptx_codec_rttext_parms *codecp)
{
 const tSMVMPtxId vmptx = codecp->vmptx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmptx);
 grv = &vmptx->groove;

 if (codecp->redundancy_level < 0) sts = err(ERR_SM_BAD_PARAMETER);
 if (codecp->buffer_time < 0) sts = err(ERR_SM_BAD_PARAMETER);
 if (codecp->idle_time < 0) sts = err(ERR_SM_BAD_PARAMETER);

 if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_TIMESTAMPRATE, 2), 0); // remove the override
 if (!sts) sts = unsafe_vmptx_set_main_codec(vmptx, SUBTASKTC_RTTEXTTX, codecp->payload_type, 0, 0, kSMVMPTxVADModeDisabled);
 if (codecp->payload_type != -1) {
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_CONFIG_CODEC, 4), SUBTASKTC_RTTEXTTX, codecp->payload_type, 0);
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_TIMESTAMPRATE, 2), 1000);
	if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_RTTEXT_TX_CONFIG, 5), codecp->redundant_payload_type, codecp->redundancy_level, codecp->buffer_time, codecp->idle_time);
 }
 unlock_object(vmptx);
 return sts;
}

STATIC int real_sm_vmprx_ice_stun_config(struct sm_vmprx_ice_stun_config_parms *configp)
{
 const tSMVMPrxId vmprx = configp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 unsigned id;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (configp->local_role != kSMICERoleNone && vmprx->stun_role != VMPRX_STUN_ROLE_NONE) {
	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else {
	uint32_t tiebreaker_high = configp->role_tiebreaker >> 32;
	uint32_t tiebreaker_low = configp->role_tiebreaker & 0xffffffffu;
	uint32_t role = 0;
	switch(configp->local_role) {
	case kSMICERoleLite:
		role = STUN_ROLE_LITE;
		vmprx->stun_role = VMPRX_STUN_ROLE_LITE;
		break;
	case kSMICERoleControlled:
		role = STUN_ROLE_CONTROLLED;
		vmprx->stun_role = VMPRX_STUN_ROLE_CONTROLLED;
		break;
	case kSMICERoleControlling:
		role = STUN_ROLE_CONTROLLING;
		vmprx->stun_role = VMPRX_STUN_ROLE_CONTROLLING;
		break;
	case kSMICERoleNone:
		if (vmprx->stun_role != VMPRX_STUN_ROLE_NONE) {
			sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB,2), SUBTASKTC_STUN);
			if (!sts) vmprx->stun_role = VMPRX_STUN_ROLE_NONE;
		}
		break;
	default:
		sts = err(ERR_SM_BAD_PARAMETER);
	}
	if (!sts && role) {
		sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB,2), SUBTASKTC_STUN);
		if (!sts) {
			id = MSGSTRING_ID(MSGSTRING_STUN, STUN_STRING_LOCAL_UFRAG);
			if (!sts) sts = grv->sendmsgstring(grv, id, configp->local_ice_ufrag_length, configp->local_ice_ufrag);
			id = MSGSTRING_ID(MSGSTRING_STUN, STUN_STRING_LOCAL_PWD);
			if (!sts) sts = grv->sendmsgstring(grv, id, configp->local_ice_pwd_length, configp->local_ice_pwd);
			if (!sts) grv->sendmsg(grv, MESSAGE(MSG_STUN_CONFIG,4), role, tiebreaker_high, tiebreaker_low);
			if (sts) {
				grv->sendmsg(grv, MESSAGE(MSG_DELSUB,2), SUBTASKTC_STUN);
			}
		}
		if (sts) vmprx->stun_role = VMPRX_STUN_ROLE_NONE;
	}
 }
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_ice_stun_remote_credentials(struct sm_vmprx_ice_stun_remote_credentials_parms *credentialsp)
{
 const tSMVMPrxId vmprx = credentialsp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 unsigned id;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (vmprx->stun_role == VMPRX_STUN_ROLE_NONE) {
	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else {
	id = MSGSTRING_ID(MSGSTRING_STUN, STUN_STRING_REMOTE_UFRAG);
	if (!sts) sts = grv->sendmsgstring(grv, id, credentialsp->remote_ice_ufrag_length, credentialsp->remote_ice_ufrag);
	id = MSGSTRING_ID(MSGSTRING_STUN, STUN_STRING_REMOTE_PWD);
	if (!sts) sts = grv->sendmsgstring(grv, id, credentialsp->remote_ice_pwd_length, credentialsp->remote_ice_pwd);
 }
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_ice_stun_change_role(struct sm_vmprx_ice_stun_change_role_parms *rolep)
{
 const tSMVMPrxId vmprx = rolep->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 switch(rolep->local_role) {
 case kSMICERoleControlled:
	if (vmprx->stun_role == VMPRX_STUN_ROLE_CONTROLLING) {
		sts = grv->sendmsg(grv, MESSAGE(MSG_STUN_CONFIG, 2), STUN_ROLE_CONTROLLED);
		if (!sts) vmprx->stun_role = VMPRX_STUN_ROLE_CONTROLLED;
	} else if (vmprx->stun_role != VMPRX_STUN_ROLE_CONTROLLED) {
		sts = err(ERR_SM_WRONG_CHANNEL_STATE);
	}
	break;
 case kSMICERoleControlling:
	if (vmprx->stun_role == VMPRX_STUN_ROLE_CONTROLLED) {
		sts = grv->sendmsg(grv, MESSAGE(MSG_STUN_CONFIG, 2), STUN_ROLE_CONTROLLING);
		if (!sts) vmprx->stun_role = VMPRX_STUN_ROLE_CONTROLLING;
	} else if (vmprx->stun_role != VMPRX_STUN_ROLE_CONTROLLING) {
		sts = err(ERR_SM_WRONG_CHANNEL_STATE);
	}
	break;
 case kSMICERoleNone:
 case kSMICERoleLite:
 default:
	sts = err(ERR_SM_BAD_PARAMETER);
 }
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vmprx_ice_stun_send_bind_request(struct sm_vmprx_ice_stun_send_bind_request_parms *requestp)
{
 const tSMVMPrxId vmprx = requestp->vmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vmprx);
 grv = &vmprx->groove;
 if (vmprx->stun_role != VMPRX_STUN_ROLE_CONTROLLED && vmprx->stun_role != VMPRX_STUN_ROLE_CONTROLLING) {
	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else {
	uint32_t uc = requestp->use_candidate ? 1 : 0;
	uint32_t rtcp_port = requestp->use_RTCP_port ?  1: 0;
	// pack transaction id into three 32bit blocks
	uint8_t* ptid = (uint8_t*)(&requestp->transaction_id[0]);
	uint32_t tid1;
	uint32_t tid2;
	uint32_t tid3;
	tid1 = (*ptid++) << 24;
	tid1 |= (*ptid++) << 16;
	tid1 |= (*ptid++) << 8;
	tid1 |= (*ptid++);
	tid2 = (*ptid++) << 24;
	tid2 |= (*ptid++) << 16;
	tid2 |= (*ptid++) << 8;
	tid2 |= (*ptid++);
	tid3 = (*ptid++) << 24;
	tid3 |= (*ptid++) << 16;
	tid3 |= (*ptid++) << 8;
	tid3 |= (*ptid++);

	if (!vmprx->is_ipv6) {
		uint32_t dest_port = ntohs(requestp->destination.ipv4.sin_port);
		sts = grv->sendmsg(grv, MESSAGE(MSG_STUN_SEND_BIND, 9), rtcp_port, tid1, tid2, tid3, requestp->priority, uc, 
				dest_port, ip2msg(requestp->destination.ipv4.sin_addr.s_addr));
	} else {
		uint32_t dest_port = ntohs(requestp->destination.ipv6.sin6_port);
		sts = grv->sendmsg(grv, MESSAGE(MSG_STUN_SEND_BIND, 12), rtcp_port, tid1, tid2, tid3, requestp->priority, uc, 
				dest_port, ipv6tomsg(requestp->destination.ipv6.sin6_addr));
	}
 }
 unlock_object(vmprx);
 return sts;
}

STATIC int real_sm_vidmprx_ice_stun_config(struct sm_vidmprx_ice_stun_config_parms *configp)
{
 const tSMVidMPrxId vidmprx = configp->vidmprx;
 tSMGroove *grv;
 int sts = 0;
 unsigned id;
 lock_object(vidmprx);
 grv = &vidmprx->groove;
 if (configp->local_role != kSMICERoleNone && vidmprx->stun_role != VIDMPRX_STUN_ROLE_NONE) {
	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else {
	uint32_t tiebreaker_high = configp->role_tiebreaker >> 32;
	uint32_t tiebreaker_low = configp->role_tiebreaker & 0xffffffffu;
	uint32_t role = 0;
	switch(configp->local_role) {
	case kSMICERoleLite:
		role = STUN_ROLE_LITE;
		vidmprx->stun_role = VIDMPRX_STUN_ROLE_LITE;
		break;
	case kSMICERoleControlled:
		role = STUN_ROLE_CONTROLLED;
		vidmprx->stun_role = VIDMPRX_STUN_ROLE_CONTROLLED;
		break;
	case kSMICERoleControlling:
		role = STUN_ROLE_CONTROLLING;
		vidmprx->stun_role = VIDMPRX_STUN_ROLE_CONTROLLING;
		break;
	case kSMICERoleNone:
		if (vidmprx->stun_role != VIDMPRX_STUN_ROLE_NONE) {
			sts = grv->sendmsg(grv, MESSAGE(MSG_DELSUB,2), SUBTASKTC_STUN);
			if (!sts) vidmprx->stun_role = VIDMPRX_STUN_ROLE_NONE;
		}
		break;
	default:
		sts = err(ERR_SM_BAD_PARAMETER);
	}
	if (!sts && role) {
		sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB,2), SUBTASKTC_STUN);
		if (!sts) {
			id = MSGSTRING_ID(MSGSTRING_STUN, STUN_STRING_LOCAL_UFRAG);
			if (!sts) sts = grv->sendmsgstring(grv, id, configp->local_ice_ufrag_length, configp->local_ice_ufrag);
			id = MSGSTRING_ID(MSGSTRING_STUN, STUN_STRING_LOCAL_PWD);
			if (!sts) sts = grv->sendmsgstring(grv, id, configp->local_ice_pwd_length, configp->local_ice_pwd);
			if (!sts) grv->sendmsg(grv, MESSAGE(MSG_STUN_CONFIG,4), role, tiebreaker_high, tiebreaker_low);
			if (sts) {
				grv->sendmsg(grv, MESSAGE(MSG_DELSUB,2), SUBTASKTC_STUN);
			}
		}
		if (sts) vidmprx->stun_role = VIDMPRX_STUN_ROLE_NONE;
	}
 }
 unlock_object(vidmprx);
 return sts;
}

STATIC int real_sm_vidmprx_ice_stun_remote_credentials(struct sm_vidmprx_ice_stun_remote_credentials_parms *credentialsp)
{
 const tSMVidMPrxId vidmprx = credentialsp->vidmprx;
 tSMGroove *grv;
 int sts = 0;
 unsigned id;
 lock_object(vidmprx);
 grv = &vidmprx->groove;
 if (vidmprx->stun_role == VIDMPRX_STUN_ROLE_NONE) {
	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else {
	id = MSGSTRING_ID(MSGSTRING_STUN, STUN_STRING_REMOTE_UFRAG);
	if (!sts) sts = grv->sendmsgstring(grv, id, credentialsp->remote_ice_ufrag_length, credentialsp->remote_ice_ufrag);
	id = MSGSTRING_ID(MSGSTRING_STUN, STUN_STRING_REMOTE_PWD);
	if (!sts) sts = grv->sendmsgstring(grv, id, credentialsp->remote_ice_pwd_length, credentialsp->remote_ice_pwd);
 }
 unlock_object(vidmprx);
 return sts;
}

STATIC int real_sm_vidmprx_ice_stun_change_role(struct sm_vidmprx_ice_stun_change_role_parms *rolep)
{
 const tSMVidMPrxId vidmprx = rolep->vidmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vidmprx);
 grv = &vidmprx->groove;
 switch(rolep->local_role) {
 case kSMICERoleControlled:
	if (vidmprx->stun_role == VIDMPRX_STUN_ROLE_CONTROLLING) {
		sts = grv->sendmsg(grv, MESSAGE(MSG_STUN_CONFIG, 2), STUN_ROLE_CONTROLLED);
		if (!sts) vidmprx->stun_role = VIDMPRX_STUN_ROLE_CONTROLLED;
	} else if (vidmprx->stun_role != VIDMPRX_STUN_ROLE_CONTROLLED) {
		sts = err(ERR_SM_WRONG_CHANNEL_STATE);
	}
	break;
 case kSMICERoleControlling:
	if (vidmprx->stun_role == VIDMPRX_STUN_ROLE_CONTROLLED) {
		sts = grv->sendmsg(grv, MESSAGE(MSG_STUN_CONFIG, 2), STUN_ROLE_CONTROLLING);
		if (!sts) vidmprx->stun_role = VIDMPRX_STUN_ROLE_CONTROLLING;
	} else if (vidmprx->stun_role != VIDMPRX_STUN_ROLE_CONTROLLING) {
		sts = err(ERR_SM_WRONG_CHANNEL_STATE);
	}
	break;
 case kSMICERoleNone:
 case kSMICERoleLite:
 default:
	sts = err(ERR_SM_BAD_PARAMETER);
 }
 unlock_object(vidmprx);
 return sts;
}

STATIC int real_sm_vidmprx_ice_stun_send_bind_request(struct sm_vidmprx_ice_stun_send_bind_request_parms *requestp)
{
 const tSMVidMPrxId vidmprx = requestp->vidmprx;
 tSMGroove *grv;
 int sts = 0;
 lock_object(vidmprx);
 grv = &vidmprx->groove;
 if (vidmprx->stun_role != VIDMPRX_STUN_ROLE_CONTROLLED && vidmprx->stun_role != VIDMPRX_STUN_ROLE_CONTROLLING) {
	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else {
	uint32_t uc = requestp->use_candidate ? 1 : 0;
	uint32_t rtcp_port = requestp->use_RTCP_port ?  1: 0;
	// pack transaction id into three 32bit blocks
	uint8_t* ptid = (uint8_t*)(&requestp->transaction_id[0]);
	uint32_t tid1;
	uint32_t tid2;
	uint32_t tid3;
	tid1 = (*ptid++) << 24;
	tid1 |= (*ptid++) << 16;
	tid1 |= (*ptid++) << 8;
	tid1 |= (*ptid++);
	tid2 = (*ptid++) << 24;
	tid2 |= (*ptid++) << 16;
	tid2 |= (*ptid++) << 8;
	tid2 |= (*ptid++);
	tid3 = (*ptid++) << 24;
	tid3 |= (*ptid++) << 16;
	tid3 |= (*ptid++) << 8;
	tid3 |= (*ptid++);

	if (!vidmprx->is_ipv6) {
		uint32_t dest_port = ntohs(requestp->destination.ipv4.sin_port);
		sts = grv->sendmsg(grv, MESSAGE(MSG_STUN_SEND_BIND, 9), rtcp_port, tid1, tid2, tid3, requestp->priority, uc, 
				dest_port, ip2msg(requestp->destination.ipv4.sin_addr.s_addr));
	} else {
		uint32_t dest_port = ntohs(requestp->destination.ipv6.sin6_port);
		sts = grv->sendmsg(grv, MESSAGE(MSG_STUN_SEND_BIND, 12), rtcp_port, tid1, tid2, tid3, requestp->priority, uc, 
				dest_port, ipv6tomsg(requestp->destination.ipv6.sin6_addr));
	}
 }
 unlock_object(vidmprx);
 return sts;
}

STATIC int real_sm_vmptx_generate_jitter(struct sm_vmptx_generate_jitter_parms* jitterp)
{
 const tSMVMPtxId vmptx = jitterp->vmptx;
 tSMGroove *grv;
 int sts;
 lock_object(vmptx);
 grv = &vmptx->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_GEN_JITTER, 5), jitterp->external_jitter_threshold, jitterp->inter_jitter_gap, jitterp->added_jitter, jitterp->max_packet_age);
 unlock_object(vmptx);
 return sts;
}

#include "../pubdoc/gen/prosrtpapi.i"

STATIC int real_sm_vmptx_sync_start(struct sm_vmptx_sync_start_parms *pp)
{
 int sts = 0;
 tSMGroove *grv;
 lock_object(pp->vmptx);
 grv = &pp->vmptx->groove;
 sts = grv->sendmsg(grv, MESSAGE(MSG_ADDSUB,2), SUBTASKTC_VMPTXSYNC);
 unlock_object(pp->vmptx);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_sync_start(struct sm_vmptx_sync_start_parms *pp)
{
 int sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog("sm_vmptx_sync_start(vmptx=%p)", pp->vmptx);
 sts = real_sm_vmptx_sync_start(pp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

