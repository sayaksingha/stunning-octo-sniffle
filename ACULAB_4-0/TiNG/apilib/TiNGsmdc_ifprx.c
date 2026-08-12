/* TiNGsmdc_ifprx.c - Data communications functions for T.38 receiver */

#include "TiNGsmdc_ifprx.h"
#include "../hsif/ifp.h"
#include "../hsif/kernel.h"
#include "../hsif/modem.h"
#include "../hsif/subtask.h"
#include "../hsif/t38.h"
#include "../hsif/typecode.h"
#include "TiNGcommon.h"
#include "TiNGo_chan.h"
#include "TiNGsmdc.h"
#include "smdc_ifp.h"
#include "trace.h"
#include <string.h>

STATIC int ifprx_lcontrol(struct smdc_line_control_parms *pp)
{
 (void) pp;
 // nothing - IFP is always connected
 return 0;
}

STATIC int ifprx_lstatus(struct smdc_line_status_parms *pp)
{
 if (pp->channel->rxtx[1].dataready / 8) {
	pp->rx_status = kSMDCRxStatusReceivingData;
 } else {
	if (pp->channel->ur.dc.u.ifp.carrier) {
		pp->rx_status = kSMDCRxStatusCarrierPresent;
	} else pp->rx_status = kSMDCRxStatusNoCarrier;
 }
 return 0;
}

STATIC int ifprx_rstatus(struct smdc_rx_status_parms *pp, U32 *msg)
{
 (void) pp;
 (void) msg;
 return 0;
}

STATIC int ifprx_rcontrol(struct smdc_rx_control_parms *pp)
{
 (void) pp;
 return err(ERR_SM_BAD_PARAMETER);
}

STATIC int ifprx_msghand(tSMChannelId chan, U32 *msg)
{
 (void) chan;
 if (*msg & 0x80000000) {
	unsigned long code = *msg & 0xffff0000;
	switch (code) {
	default:
		return 1;
	}
 } else return 0;
}

STATIC int ifprx_setenc(tSMChannelId cp, ENCTBL *enctbl, unsigned cflen, void *cfdata)
{
 (void) cp;
 (void) enctbl;
 (void) cflen;
 (void) cfdata;
 return 0;
}

STATIC struct protable protable_ifprx = {
	0,
	0,
	ifprx_setenc,
	0,
	0,
	ifprx_lcontrol,
	ifprx_lstatus,
	0,
	0,
	ifprx_rcontrol,
	ifprx_rstatus,
	{ 0, ifprx_msghand, },
	0,
};

LOCALDEF int real_smdc_rx_config_t38(struct smdc_rx_config_t38_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 int sts = 0;
 lock_object(cp);
 if (cp->rxtx[1].func != RXTXF_NONE) {
	if (cp->rxtx[1].func == RXTXF_INVALID)
		sts = err(ERR_SM_WRONG_CHANNEL_TYPE);
	else
		sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else {
	CARDCONN *cx = &cp->chan[HANDLE_READ].cx;
	memset(&cp->ur.dc, 0, sizeof(cp->ur.dc));
	cp->rxtx[1].protocol = &protable_ifprx;
	cp->rxtx[1].func = RXTXF_DATA;
	cp->rxtx[1].state = RXTXS_DONE;
	cp->rxtx[1].timeout_milliseconds = 100;
	sts = startin(cp, TASKTC_IFPRX);
	if (!sts) sts = sendmsg3(cx, MSG_T38CFG, pp->PreCorrigendum, pp->Protocol);
	if (!sts) {
		cp->ur.dc.u.t38.protocol = pp->Protocol;
		cp->rxtx[1].state = RXTXS_RUN;
		cp->rxtx[1].gotready = 0;
		cp->ur.dc.encoding = smdc_encode(kSMDCConfigEncodingNone);
		sts = sendmsg2(cx, MSG_ADDHSUB, SUBTASKTC_HBUFTOHOST);
		if (!sts) sts = chan_setxfer(cp, 1);
	}
 }
 unlock_object(cp);
 return sts;
}

STATIC int unsafe_smdc_rx_status_t38(struct smdc_rx_status_t38_parms *statusp)
{
 const tSMChannelId cp = statusp->channel;
 CARDCONN *cx;
 struct ting_message msg;
 int sts = 0;
 cx = &cp->chan[HANDLE_READ].cx;
 if (cp->rxtx[1].func != RXTXF_DATA) return err(ERR_SM_WRONG_CHANNEL_STATE);
 statusp->flow = 0;		// default
 msg.flags = IOCTL_TiNG_RDMSG_FLAG_NOWAIT;
 sts = rdmsg(&msg, cx);
 if (!sts) {
	cp->rxtx[1].dataready = msg.capacity;
	statusp->available_octets = cp->rxtx[1].dataready / 8;
	if (statusp->available_octets) {
		statusp->status = kSMDCRxT38StatusData;
	} else {
		statusp->status = kSMDCRxT38StatusEmpty;
	}
	switch (msg.msg[0] & 0xffff0000) {
	case MESSAGE(MSG_DEAD, 4):
	case MESSAGE(MSG_DEAD, 3):
	case MESSAGE(MSG_DEAD, 2):
		switch (msg.msg[1]) {
		case MSG_DEAD_CAUSE_NOMORE:
			break;  // normal
		case MSG_DEAD_CAUSE_MSG:
			break;	// normal
		case MSG_DEAD_CAUSE_BADFUNC:
			return err(ERR_SM_NO_SUCH_FIRMWARE);
		case MSG_DEAD_CAUSE_NOMEM:
			return err(ERR_SM_NO_RESOURCES);
		case MSG_DEAD_CAUSE_DISCO:
			return err(ERR_SM_DISCONNECTED);
		default:
			return err(ERR_SM_FIRMWARE_PROBLEM);
		}
		// fall in...
	case MESSAGE(MSG_DEAD, 1):
		cp->rxtx[1].state = RXTXS_DONE;
		statusp->status = kSMDCRxT38StatusFinished;
		cp->rxtx[1].func = RXTXF_NONE;
		break;
	case MESSAGE(MSG_UNDERRUN, 1):
		statusp->status = kSMDCRxT38StatusOverrun;
		break;
	case MESSAGE(MSG_T38TONE, 2):
		statusp->status = kSMDCRxT38StatusTone;
		if (msg.msg[1] == T38TONE_CNG) {
			statusp->u.tone.tone_type = kSMDCT38ToneTypeCNG;
		} else if (msg.msg[1] == T38TONE_CED) {
			statusp->u.tone.tone_type = kSMDCT38ToneTypeCED;
		} else if (msg.msg[1] == T38TONE_ANSAM) {
			statusp->u.tone.tone_type = kSMDCT38ToneTypeANSAM;
		}
		break;
	case MESSAGE(MSG_T38_V34, 2):
		statusp->status = kSMDCRxT38StatusV34;
		if (msg.msg[1] == V34_CC_1200) {
			statusp->u.v34.v34_type = kSMDCT38_V34Type_V34_CC_1200;
		} else if (msg.msg[1] == V34_PRI_CHAN) {
			statusp->u.v34.v34_type = kSMDCT38_V34Type_V34_PRI_CHAN;
		} else if (msg.msg[1] == V34_CC_RETRAIN) {
			statusp->u.v34.v34_type = kSMDCT38_V34Type_V34_CC_RETRAIN;
		}
		break;
	case MESSAGE(MSG_T38RATE, 2):
		statusp->status = kSMDCRxT38StatusRate;
		statusp->u.rate.rate = msg.msg[1];
		break;
	case MESSAGE(MSG_CARRIER, 2):
		statusp->status = kSMDCRxT38StatusCarrier;
		statusp->u.carrier.carrier = cp->ur.dc.u.ifp.carrier = msg.msg[1];
		break;
	case MESSAGE(MSG_READY, 4):
		cp->rxtx[1].gotready = 1;
		break;
	case MESSAGE(MSG_BADMSG, 3):
		if ((msg.msg[1] & 0xffff0000) == MESSAGE(MSG_ADDSUB, 2) ||
			(!cp->rxtx[1].gotready && (msg.msg[1] & 0xffff0000) == MESSAGE(MSG_ADDHSUB, 2))) {
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
		break;
	case MESSAGE(MSG_T38PROTO, 2):
		if (cp->ur.dc.u.t38.protocol == kSMDCT38ProtocolAll) {
			statusp->status = kSMDCRxT38StatusProtocol;
			statusp->u.protocol.protocol = msg.msg[1];
		}
		break;
	default:
		break;
	}
 }
 return sts;
}

LOCALDEF int real_smdc_rx_status_t38(struct smdc_rx_status_t38_parms *statusp)
{
 const tSMChannelId cp = statusp->channel;
 int sts;
 lock_object(cp);
 sts = unsafe_smdc_rx_status_t38(statusp);
 unlock_object(cp);
 return sts;
}
