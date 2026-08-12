/* TiNGsmdc_v34.c - Data communications functions for V.34 */

#include "TiNGsmdc_v34.h"
#include "TiNGcommon.h"
#include "TiNGo_chan.h"
#include "../hsif/kernel.h"
#include "../hsif/typecode.h"
#include "../hsif/subtask.h"
#include "../hsif/v34hdx.h"
#include "../hsif/modem.h"
#include "../hsif/speed.h"
#include <string.h>

LOCALDEF int real_smdc_rx_config_v34(struct smdc_rx_config_v34_parms *pp)
{
 int sts = 0;
 U32 subtask = 0;
 tSMChannelId cp = pp->channel;
 CARDCONN *cx = &cp->chan[HANDLE_READ].cx;
 lock_object(cp);
 if (cp->rxtx[1].func != RXTXF_NONE) {
	if (cp->rxtx[1].func == RXTXF_INVALID)
		sts = err(ERR_SM_WRONG_CHANNEL_TYPE);
	else
 		sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 }
 if (pp->duplex == kSMDCV34HalfDuplex) {
	 subtask = SUBTASKTC_V34RX;
 } else if (pp->duplex == kSMDCV34FullDuplex) {
	 subtask = SUBTASKTC_V34FDRX;
 } else {
	 sts = err(ERR_SM_BAD_PARAMETER);
 }
 if (!sts) {
	memset(&cp->ur, 0, sizeof(cp->ur));
	cp->rxtx[1].protocol = &protable_v34rx;
	cp->rxtx[1].func = RXTXF_DATA;
	cp->rxtx[1].state = RXTXS_DONE;
	cp->rxtx[1].timeout_milliseconds = 100;
	cp->rxtx[1].gotready = 0;
	sts = startin(cp, TASKTC_INCHAN);
 }
 if (!sts) sts = sendmsg2(cx, MSG_ADDSUB, subtask);
 if (!sts) sts = sendmsg3(cx, MSG_SPEED, pp->min_bitrate, pp->max_bitrate);
 if (!sts) sts = sendmsg2(cx, MSG_V8FSKMETRIC, pp->v8fsk_metric);
 if (!sts) {
	if (cp->rxtx[0].state == RXTXS_RUN && cp->rxtx[0].func == RXTXF_DATA && cp->rxtx[0].protocol == &protable_v34tx) {
		sts = setfriend(&cp->chan[HANDLE_WRITE].cx, &cp->chan[HANDLE_READ], 0);
	}
 }
 if (!sts) cp->rxtx[1].state = RXTXS_RUN;
 if (!sts) cp->ur.dc.u.v34.link_status = kSMDCLinkStatusConnecting;
 unlock_object(cp);
 return sts;
}

LOCALDEF int real_smdc_tx_config_v34(struct smdc_tx_config_v34_parms *pp)
{
 int sts = 0;
 U32 subtask = 0;
 tSMChannelId cp = pp->channel;
 CARDCONN *cx = &cp->chan[HANDLE_WRITE].cx;
 lock_object(cp);
 if (cp->rxtx[0].func != RXTXF_NONE) {
	if (cp->rxtx[0].func == RXTXF_INVALID) {
		sts = err(ERR_SM_WRONG_CHANNEL_TYPE);
	} else sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 }
 if (pp->duplex == kSMDCV34HalfDuplex) {
	subtask = SUBTASKTC_V34TX;
	if ((pp->mode == kSMDCV34ModeAnswering) == (pp->polling == kSMDCV34ModePolling)) {
		cp->ut.dc.u.v34.dir = eV34HDPrimaryTx;
	} else {
		cp->ut.dc.u.v34.dir = eV34HDPrimaryRx;
	}
 } else if (pp->duplex == kSMDCV34FullDuplex) {
	subtask = SUBTASKTC_V34FDTX;
	cp->ut.dc.u.v34.dir = eV34FD;
 } else {
	sts = err(ERR_SM_BAD_PARAMETER);
 }
 if (!sts) {
	memset(&cp->ut, 0, sizeof(cp->ut));
	cp->rxtx[0].protocol = &protable_v34tx;
	cp->rxtx[0].func = RXTXF_DATA;
	cp->rxtx[0].state = RXTXS_DONE;
	cp->rxtx[0].timeout_milliseconds = 100;
	cp->rxtx[0].gotready = 0;
	cp->ut.dc.u.v34.user_capacity = cp->rxtx[0].minimum_bits >> 3; // user_capacity is in octets
	cp->ut.dc.u.v34.user_blocking = 1;
	sts = startout(cp, TASKTC_OUTCHAN);
 }
 if (!sts) sts = sendmsg2(cx, MSG_ADDSUB, subtask);
 if (!sts) sts = sendmsg3(cx, MSG_SPEED, pp->min_bitrate, pp->max_bitrate);
 if (!sts) sts = sendmsg3(cx, MSG_DIR, pp->mode == kSMDCV34ModeAnswering, pp->polling == kSMDCV34ModePolling);
 if (!sts) {
	if (cp->rxtx[1].state == RXTXS_RUN && cp->rxtx[1].func == RXTXF_DATA && cp->rxtx[1].protocol == &protable_v34rx) {
		sts = setfriend(cx, &cp->chan[HANDLE_READ], 0);
	}
 }
 if (!sts) cp->rxtx[0].state = RXTXS_RUN;
 unlock_object(cp);
 return sts;
}

STATIC int v34_setencrx(tSMChannelId cp, ENCTBL *enctbl, unsigned cflen, void *cfdata)
{
 ENCTBL *old = cp->ur.dc.encoding;
 int sts;
 if (old) {
	sts = old->stopfn(cp, 1);
	if (sts) return sts;
	cp->ur.dc.encoding = 0;
 }
 sts = enctbl->encfn(cp, 1, cfdata, cflen);
 if (!sts) cp->ur.dc.encoding = enctbl;
 if (!sts && !cp->rxtx[1].gotready) setup_coll_disp(cp, enctbl, 0, 1);
 return 0;
}

STATIC int v34_setenctx(tSMChannelId cp, ENCTBL *enctbl, unsigned cflen, void *cfdata)
{
 ENCTBL *old = cp->ut.dc.encoding;
 int sts;
 if (old) {
	sts = old->stopfn(cp, 0);
	if (sts) return sts;
	cp->ut.dc.encoding = 0;
 }
 sts = enctbl->encfn(cp, 0, cfdata, cflen);
 if (!sts) cp->ut.dc.encoding = enctbl;
 if (!sts && !cp->rxtx[0].gotready) setup_coll_disp(cp, enctbl, 1, 0);
 return 0;
}

STATIC int v34_lcontrol(struct smdc_line_control_parms *pp)
{
 switch (pp->cmd) {
 case kSMDCLineCtlCmdSetPrefixSuffix:
 case kSMDCLineCtlCmdAssertRTS:
 case kSMDCLineCtlCmdUnassertRTS:
	return 0;
 default:
	return err(ERR_SM_BAD_PARAMETER);
 }
}

STATIC int v34_update_xfer(struct tSMChannelId_struct *cp)
{
 int sts = 0;
 CARDCONN *cx = &cp->chan[HANDLE_WRITE].cx;
 struct ting_xfer xfer;
 xfer.ioctl_length = sizeof(xfer);
 if (cp->ut.dc.u.v34.tx_data_disabled) {
	xfer.minimum_bits = 0;
 } else {
	xfer.minimum_bits = 8 * cp->ut.dc.u.v34.user_capacity;
 }
 xfer.timeout_milliseconds = 0;
 xfer.nomore = 0;
 if (cp->rxtx[0].state == RXTXS_RUN) {
	if (ERR(cx_setblock(cx, cp->ut.dc.u.v34.user_blocking))) {
		sts = err(ERR_SM_DEVERR);
	} else if (ERR(cx_ioctl(cx, IOCTL_TiNG_WRXFER, &xfer, sizeof(xfer)))) {
		sts = err(ERR_SM_DEVERR);
	}
 }
 return sts;
}

LOCALDEF int real_smdc_line_control_v34(struct smdc_line_control_v34_parms *pp)
{
 int sts = 0;
 tSMChannelId cp = pp->channel;
 CARDCONN *cx;
 lock_object(cp);
 cx = &cp->chan[HANDLE_WRITE].cx;
 if (cp->rxtx[1].func != RXTXF_DATA || cp->rxtx[0].func != RXTXF_DATA ||
	 cp->rxtx[1].protocol != &protable_v34rx || cp->rxtx[0].protocol != &protable_v34tx) {
	 sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 }
 if (!sts) switch (pp->cmd) {
 case kSMDCV34LineCtlCmdEnablePrimary:
	if (cp->ut.dc.u.v34.dir == eV34FD) {
		sts = err(ERR_SM_WRONG_CHANNEL_STATE);
	} else {
		cp->ut.dc.u.v34.changing_rate = 1;
		cp->ut.dc.u.v34.tx_data_disabled = 1;
		sts = sendmsg2(cx, MSG_V34_SWITCH_CHANNEL, 1);
		if (!sts) sts = v34_update_xfer(cp);
	}
	break;
 case kSMDCV34LineCtlCmdEnableControl:
	if (cp->ut.dc.u.v34.dir == eV34FD) {
		sts = err(ERR_SM_WRONG_CHANNEL_STATE);
	} else {
		cp->ut.dc.u.v34.changing_rate = 1;
		cp->ut.dc.u.v34.tx_data_disabled = 1;
		sts = sendmsg2(cx, MSG_V34_SWITCH_CHANNEL, 0);
		if (!sts) sts = v34_update_xfer(cp);
	}
	break;
 case kSMDCV34LineCtlCmdRetrain:
	cp->ur.dc.u.v34.snr_status = 0;
	sts = sendmsg2(cx, MSG_RENEGOTIATE, V34_FULL_RETRAIN);
	break;
 case kSMDCV34LineCtlCmdRenegotiateUp:
	cp->ur.dc.u.v34.snr_status = 0;
	sts = sendmsg2(cx, MSG_RENEGOTIATE, V34_RENEGOTIATE_UP);
	break;
 case kSMDCV34LineCtlCmdRenegotiateDown:
	cp->ur.dc.u.v34.snr_status = 0;
	sts = sendmsg2(cx, MSG_RENEGOTIATE, V34_RENEGOTIATE_DOWN);
	break;
 case kSMDCV34LineCtlCmdBitrateTx:
	sts = sendmsg3(cx, MSG_SPEED, pp->u.bitrate.min_bitrate, pp->u.bitrate.max_bitrate);
	break;
 case kSMDCV34LineCtlCmdBitrateRx:
	cp->ur.dc.u.v34.snr_status = 0;
	sts = sendmsg3(&cp->chan[HANDLE_READ].cx, MSG_SPEED, pp->u.bitrate.min_bitrate, pp->u.bitrate.max_bitrate);
	break;
 default:
	sts = err(ERR_SM_BAD_PARAMETER);
 }
 unlock_object(cp);
 return sts;
}

STATIC int v34_lstatus(struct smdc_line_status_parms *pp)
{
 tSMChannelId cp = pp->channel;
 pp->rx_status = cp->ur.dc.u.v34.status;
 pp->tx_status = cp->ut.dc.u.v34.status;
 pp->link_status = cp->ur.dc.u.v34.link_status;
 return 0;
}

LOCALDEF int real_smdc_line_status_v34(struct smdc_line_status_v34_parms *pp)
{
 struct smdc_line_status_parms lsp;
 int sts = 0;
 tSMChannelId cp = pp->channel;
 lock_object(cp);
 lsp.channel = pp->channel;
 sts = v34_lstatus(&lsp);
 pp->link_status = lsp.link_status;
 pp->rx_status = lsp.rx_status;
 pp->tx_status = lsp.tx_status;
 pp->control_bitrate = cp->ur.dc.u.v34.a_bitrate;
 pp->primary_bitrate = cp->ur.dc.u.v34.b_bitrate;
 pp->snr_status = cp->ur.dc.u.v34.snr_status;
 pp->fdx_rx_bitrate = cp->ur.dc.u.v34.a_bitrate;
 pp->fdx_tx_bitrate = cp->ur.dc.u.v34.b_bitrate;
 unlock_object(cp);
 return sts;
}

STATIC int v34_tstatus(struct smdc_tx_status_parms *pp, U32 *msg)
{
 tSMChannelId cp = pp->channel;
 (void) msg;
 if (cp->ut.dc.u.v34.changing_rate) {
	pp->status = kSMDCTxStatusNoCapacity;
	pp->flow = 1;
 }
 return 0;
}

STATIC int v34_tcontrol(struct smdc_tx_control_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 switch (pp->cmd) {
 case kSMDCTxCtlNotifyOnCapacity:
	cp->ut.dc.u.v34.user_capacity = pp->capacity;
	cp->ut.dc.u.v34.user_blocking = pp->blocking;
	if (cp->ut.dc.u.v34.tx_data_disabled) return 0; // if data ev is not wanted just store settings for later use
	break;
 default:
	break;
 }
 return 1;
}

STATIC int v34_rstatus(struct smdc_rx_status_parms *pp, U32 *msg)
{
 (void) pp;
 (void) msg;
 return 0;
}

STATIC int v34_rcontrol(struct smdc_rx_control_parms *pp)
{
 (void) pp;
 return err(ERR_SM_BAD_PARAMETER);
}

STATIC int v34_msghand(tSMChannelId chan, U32 *msg, int is_rx)
{
 if (*msg & 0x80000000) {
	unsigned long code = *msg & 0xffff0000;
	switch (code) {
	case MESSAGE(MSG_SPEED, 3):
		if (is_rx) { // only believe the rx side
			chan->ur.dc.u.v34.a_bitrate = msg[1];
			chan->ur.dc.u.v34.b_bitrate = msg[2];
		}
		return 0;
	case MESSAGE(MSG_CARRIER, 2):
		chan->ur.dc.u.v34.carrier = msg[1];
		return 0;
	case MESSAGE(MSG_V34_LINE_STATUS, 3):
		if (is_rx) { // only believe the rx side
			chan->ur.dc.u.v34.link_status = kSMDCLinkStatusConnected;
			if (msg[1] == V34_LINE_SILENT) {
				chan->ut.dc.u.v34.status = kSMDCTxStatusSilent;
			} else if (msg[1] == V34_LINE_NEGOTIATING) {
				chan->ut.dc.u.v34.status = kSMDCTxStatusSendingCarrier;
				chan->ur.dc.u.v34.link_status = kSMDCLinkStatusConnecting;
			} else chan->ut.dc.u.v34.status = kSMDCTxStatusSendingData;
			if (msg[2] == V34_LINE_SILENT) {
				chan->ur.dc.u.v34.status = kSMDCRxStatusNoCarrier;
			} else if (msg[2] == V34_LINE_NEGOTIATING) {
				chan->ur.dc.u.v34.status = kSMDCRxStatusCarrierPresent;
				chan->ur.dc.u.v34.link_status = kSMDCLinkStatusConnecting;
			} else chan->ur.dc.u.v34.status = kSMDCRxStatusReceivingData;
		}
		return 0;
	case MESSAGE(MSG_V34_SWITCH_CHANNEL, 2):
		chan->ut.dc.u.v34.changing_rate = 0;
		if (msg[1] != 1 || chan->ut.dc.u.v34.dir != eV34HDPrimaryRx) {
			// if not primary or primary is not rx, enable data transfer
			chan->ut.dc.u.v34.tx_data_disabled = 0;
			// now reset threshold
			return v34_update_xfer(chan);
		}
		return 0;
	case MESSAGE(MSG_SNR, 3):
		if (msg[1] < msg[2]) {
			chan->ur.dc.u.v34.snr_status = msg[1] - msg[2];
		}
		return 0;
	default:
		return 1;
	}
 } else return 0;
}

STATIC int v34_msghand_tx(tSMChannelId chan, U32 *msg)
{
 return v34_msghand(chan, msg, 0);
}

STATIC int v34_msghand_rx(tSMChannelId chan, U32 *msg)
{
 return v34_msghand(chan, msg, 1);
}


LOCALDEF struct protable protable_v34rx = {
	0,		// protocol enum
	NULL,	// no config function
	v34_setencrx,
	v34_lcontrol,
	v34_lstatus,
	v34_lcontrol,
	v34_lstatus,
	v34_tcontrol,
	v34_tstatus,
	v34_rcontrol,
	v34_rstatus,
	{ v34_msghand_tx, v34_msghand_rx, },
	0,
};

LOCALDEF struct protable protable_v34tx = {
	0,		// protocol enum
	NULL,	// no config function
	v34_setenctx,
	v34_lcontrol,
	v34_lstatus,
	v34_lcontrol,
	v34_lstatus,
	v34_tcontrol,
	v34_tstatus,
	v34_rcontrol,
	v34_rstatus,
	{ v34_msghand_tx, v34_msghand_rx, },
	0,
};
