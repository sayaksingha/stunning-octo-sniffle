/* TiNGsmdc.i - Data communications library */

#include "smdc.h"
#include "../hsif/datacomms.h"
#include "../hsif/kernel.h"
#include "../hsif/prefsuf.h"
#include "../hsif/redirect.h"
#include "../hsif/subtask.h"
#include "../hsif/typecode.h"
#include "../hsif/vmp.h"

#include "TiNGsmdc_none.h"
#include "TiNGsmdc_sync.h"
#include "TiNGsmdc_async.h"
#include "TiNGsmdc_hdlc.h"

#include "TiNGcommon.h"
#include "trace.h"
#include <stdlib.h>
#include <string.h>

#include "smfmp.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

#include "TiNGsmdc_speech.h"
#include "TiNGsmdc_bulkrx.h"
#include "TiNGsmdc_cpu.h"
#include "TiNGsmdc_cwrx.h"
#include "TiNGsmdc_cwtx.h"
#include "TiNGsmdc_fskrx.h"
#include "TiNGsmdc_fsktx.h"
#include "TiNGsmdc_heap.h"
#include "TiNGsmdc_rawrx.h"
#include "TiNGsmdc_rawtx.h"
#include "TiNGsmdc_rdbg.h"
#include "TiNGsmdc_rlprx.h"
#include "TiNGsmdc_rlptx.h"
#include "TiNGsmdc_ttyrx.h"
#include "TiNGsmdc_v110.h"
#include "TiNGsmdc_v17rx.h"
#include "TiNGsmdc_v17tx.h"
#include "TiNGsmdc_v27rx.h"
#include "TiNGsmdc_v27tx.h"
#include "TiNGsmdc_v29rx.h"
#include "TiNGsmdc_v29tx.h"
#include "TiNGsmdc_v32.h"

static struct protable *protables[] = {
//	&protable_speech, omit this one as it is not selectable
	&protable_bulkrx,
	&protable_cpu,
	&protable_cwrx,
	&protable_cwtx,
	&protable_fskrx,
	&protable_fsktx,
	&protable_heap,
	&protable_rawrx,
	&protable_rawtx,
	&protable_rdbg,
	&protable_rlprx,
	&protable_rlptx,
	&protable_ttyrx,
	&protable_v110,
	&protable_v17rx,
	&protable_v17tx,
	&protable_v27rx,
	&protable_v27tx,
	&protable_v29rx,
	&protable_v29tx,
	&protable_v32rx,
	&protable_v32,
	0,
};

STATIC int checkmsgs(tSMChannelId chan, int isrx, struct ting_message *msg)
{
 CARDCONN *cx = &chan->chan[isrx ? HANDLE_READ : HANDLE_WRITE].cx;
 int i;
 if (chan->rxtx[isrx].state == RXTXS_DONE) return 0;
 msg->flags = IOCTL_TiNG_RDMSG_FLAG_NOWAIT;
 i = rdmsg(msg, cx);
 if (i) return i;
 chan->rxtx[isrx].dataready = msg->capacity;
 if (! (msg->msg[0] & 0x80000000) ) return 0;
 if ((msg->msg[0] & 0xfff00000) == MESSAGE(MSG_DEAD, 0)) {
 	chan->rxtx[isrx].state = RXTXS_DONE;
 	if ((msg->msg[0] & 0xf0000) > 0x10000) {	// has cause code
 		switch (msg->msg[1]) {
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
 	}
 }
 if ((msg->msg[0] & 0xffff0000) == MESSAGE(MSG_UNDERRUN, 1)) {
 	chan->rxtx[isrx].underrun = 1;
 }
 if ((msg->msg[0] & 0xffff0000) == MESSAGE(MSG_READY, 4)) {
	chan->rxtx[isrx].gotready = 1;
 }
 if ((msg->msg[0] & 0xffff0000) == MESSAGE(MSG_BADMSG, 3)
 	&& ((msg->msg[1] & 0xffff0000) == MESSAGE(MSG_ADDSUB, 2)
	|| (!chan->rxtx[isrx].gotready && (msg->msg[1] & 0xffff0000) == MESSAGE(MSG_ADDHSUB, 2)))) {
 	switch (msg->msg[2]) {
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
 if (chan->rxtx[isrx].protocol->msghand[isrx]) {
 	int sts = chan->rxtx[isrx].protocol->msghand[isrx](chan, &msg->msg[0]);
 	if (sts < 0) {
 		if (TiNGtrace > 1) {
 			olog("msghand(%08lx) = %d\n", (long) msg->msg[0], sts);
 		}
 		return sts;
 	}
 }
 return 0;
}


LOCALDEF int setup_coll_disp(tSMChannelId cp, ENCTBL *enctbl, int istx, int isrx)
{
 if (istx) {
	CARDCONN *cx = &cp->chan[HANDLE_WRITE].cx;
	cp->rxtx[0].state = RXTXS_RUN;
	cp->ut.dc.encoding = enctbl;
	if (nonnull_datafeed(cp->redirect_tx)) {
		int sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_REDIRECT_TX_LO);
		if (sts) return sts;
		sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_REDIRECT_TX_HI);
		if (sts) return sts;
		sts = sendmsg3(cx, MSG_REDIRECT_TX_CONFIG, cp->redirect_tx_threshold, cp->redirect_tx_timeout);
		if (sts) return sts;
		sts = cp->mod->card->vtbl->engage_df(cp->redirect_tx, cx, DFC_REDIRECT_TX);
		if (sts) return sts;
	} else 	if (cp->collector) {
		tSMGroove *grv = &cp->collector->groove;
		CARDCONN *cocx = grv->cardconn_cx(grv);
		int sts = sendmsg2(cx, MSG_ADDHSUB, SUBTASKTC_FROMUDP);
		if (sts) return sts;
		sts = setfriend_cx(cx, cocx, FRIEND_TYPE_COLL);
		if (sts) return sts;
	} else {
		int sts = sendmsg2(cx, MSG_ADDHSUB, SUBTASKTC_HBUFFROMHOST);
		if (sts) return sts;
		sts = chan_setxfer(cp, 0);
		if (sts) return sts;
	}
 }
 if (isrx) {
	CARDCONN *cx = &cp->chan[HANDLE_READ].cx;
	cp->rxtx[1].state = RXTXS_RUN;
	cp->ur.dc.encoding = enctbl;
	if (nonnull_datafeed(cp->redirect_rx)) {
		int sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_REDIRECT_RX);
		if (sts) return sts;
		sts = cp->mod->card->vtbl->engage_df(cp->redirect_rx, cx, DFC_REDIRECT_RX);
		if (sts) return sts;
	} else if (cp->dispatcher) {
		tSMGroove *grv = &cp->dispatcher->groove;
		CARDCONN *dicx = grv->cardconn_cx(grv);
		int sts = sendmsg2(cx, MSG_ADDHSUB, SUBTASKTC_TOUDP);
		if (sts) return sts;
		sts = setfriend_cx(cx, dicx, FRIEND_TYPE_DISP);
		if (sts) return sts;
	} else {
		int sts = sendmsg2(cx, MSG_ADDHSUB, SUBTASKTC_HBUFTOHOST);
		if (sts) return sts;
		sts = chan_setxfer(cp, 1);
		if (sts) return sts;
	}
 }
 return 0;
}

LOCALDEF int real_smdc_channel_config(struct smdc_channel_config_parms *pp)
{
 ENCTBL *enctbl = smdc_encode(pp->encoding);
 const tSMChannelId cp = pp->channel;
 unsigned u;
 int sts = 0;
 if (!enctbl) return err(ERR_SM_BAD_PARAMETER);
 lock_object(cp);
 for (u = 0; ; u++) {
	struct protable *pt = protables[u];
	if (!pt) {
		sts = err(ERR_SM_BAD_PARAMETER);
		break;
	}
	if (pt->protocol == pp->protocol) {
		int isrx = pt->rcontrol || pt->rstatus;
		int istx = pt->tcontrol || pt->tstatus;
		if (isrx && cp->rxtx[1].func != RXTXF_NONE) {
			if (cp->rxtx[1].func == RXTXF_INVALID) {
				sts = err(ERR_SM_WRONG_CHANNEL_TYPE);
			} else sts = err(ERR_SM_WRONG_CHANNEL_STATE);
			break;
		} else if (istx && cp->rxtx[0].func != RXTXF_NONE) {
			if (cp->rxtx[0].func == RXTXF_INVALID) {
				sts = err(ERR_SM_WRONG_CHANNEL_TYPE);
			} else sts = err(ERR_SM_WRONG_CHANNEL_STATE);
			break;
		} else {
			if (isrx) {
				memset(&cp->ur, 0, sizeof(cp->ur));
				cp->rxtx[1].protocol = pt;
				cp->rxtx[1].func = RXTXF_DATA;
				cp->rxtx[1].state = RXTXS_DONE;
				cp->rxtx[1].timeout_milliseconds = 100;
				cp->rxtx[1].gotready = 0;
			}
			if (istx) {
				memset(&cp->ut, 0, sizeof(cp->ut));
				cp->rxtx[0].protocol = pt;
				cp->rxtx[0].func = RXTXF_DATA;
				cp->rxtx[0].state = RXTXS_DONE;
				cp->rxtx[0].timeout_milliseconds = 100;
				cp->rxtx[0].gotready = 0;
			}
			sts = pt->config(pp);
			if (!sts) {
				sts = pt->setenc(cp, enctbl,
					pp->encoding_config_length,
					pp->encoding_config_data);
			}
			if (!sts) sts = setup_coll_disp(cp, enctbl, istx, isrx);
			break;
		}
	}
 }
 unlock_object(cp);
 return sts;
}

LOCALDEF int real_smdc_tx_encoding(struct smdc_encoding_parms *pp)
{
 ENCTBL *enctbl = smdc_encode(pp->encoding);
 const tSMChannelId cp = pp->channel;
 int sts = 0;
 if (!enctbl) return err(ERR_SM_BAD_PARAMETER);
 lock_object(cp);
 if (cp->rxtx[0].func == RXTXF_DATA) {
	struct protable *pt = cp->rxtx[0].protocol;
	sts = sendmsg1(&cp->chan[HANDLE_WRITE].cx,MSG_DISCARD_DATA);
//	if (!sts) cp->rxtx[0].gotready = 0; //TODO: on sharc version driver expects another ready message???
	if (!sts) sts = pt->setenc(cp, enctbl,
		pp->encoding_config_length,
		pp->encoding_config_data);
	if (!sts) sts = sendmsg1(&cp->chan[HANDLE_WRITE].cx,MSG_RESUME_DATA);
 } else if (cp->rxtx[0].func == RXTXF_INVALID) {
	sts = err(ERR_SM_WRONG_CHANNEL_TYPE);
 } else {
	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 }
 unlock_object(cp);
 return sts;
}

LOCALDEF int real_smdc_rx_encoding(struct smdc_encoding_parms *pp)
{
 ENCTBL *enctbl = smdc_encode(pp->encoding);
 const tSMChannelId cp = pp->channel;
 int sts = 0;
 if (!enctbl) return err(ERR_SM_BAD_PARAMETER);
 lock_object(cp);
 if (cp->rxtx[1].func == RXTXF_DATA) {
	struct protable *pt = cp->rxtx[1].protocol;
	sts = sendmsg1(&cp->chan[HANDLE_READ].cx,MSG_DISCARD_DATA);
//	if (!sts) cp->rxtx[1].gotready = 0; //TODO: on sharc version driver expects another ready message???
	if (!sts) sts = pt->setenc(cp, enctbl,
		pp->encoding_config_length,
		pp->encoding_config_data);
	if (!sts) sts = sendmsg1(&cp->chan[HANDLE_READ].cx,MSG_RESUME_DATA);
 } else if (cp->rxtx[1].func == RXTXF_INVALID) {
	sts = err(ERR_SM_WRONG_CHANNEL_TYPE);
 } else {
	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 }
 unlock_object(cp);
 return sts;
}

STATIC int set_prefsuf(CARDCONN *cx, struct smdc_line_control_parms *pp)
{
	// get rid of any old prefix/suffix
 int sts = sendmsg2(cx, MSG_DELSUB, SUBTASKTC_PREFSUF);
 if (sts) return sts;
	// see if any needed
 if (!pp->prefix_bitlength && !pp->suffix_bitlength) return 0;
	// setup new one
 sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_PREFSUF);
 if (sts) return sts;
 if (pp->prefix_bitlength) {
	unsigned u;
	sts = sendmsg2(cx, MSG_PREFLEN, pp->prefix_bitlength);
	if (sts) return sts;
	for (u=0; u*32 < pp->prefix_bitlength; u++) {
		sts = sendmsg3(cx, MSG_PREFIX, u, pp->prefix_data[u]);
		if (sts) return sts;
	}
 }
 if (pp->suffix_bitlength) {
	unsigned u;
	sts = sendmsg2(cx, MSG_SUFFLEN, pp->suffix_bitlength);
	if (sts) return sts;
	for (u=0; u*32 < pp->suffix_bitlength; u++) {
		sts = sendmsg3(cx, MSG_SUFFIX, u, pp->suffix_data[u]);
		if (sts) return sts;
	}
 }
 return 0;
}

LOCALDEF int real_smdc_line_control(struct smdc_line_control_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 int sts = 0;
 lock_object(cp);
 if (cp->rxtx[0].func != RXTXF_DATA && cp->rxtx[1].func != RXTXF_DATA) {
 	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 }
 if (cp->rxtx[0].func == RXTXF_DATA) {
	CARDCONN *cx = &cp->chan[HANDLE_WRITE].cx;
	if (!sts) switch (pp->cmd) {
	case kSMDCLineCtlCmdSetPrefixSuffix:
		sts = set_prefsuf(cx, pp);
		break;
	case kSMDCLineCtlCmdAssertRTS:
		sts = sendmsg2(cx, MSG_RTS, 1);
		break;
	case kSMDCLineCtlCmdUnassertRTS:
		sts = sendmsg2(cx, MSG_RTS, 0);
		break;
	default:
		// no generic handling
		break;
	}
	if (!sts) sts = cp->rxtx[0].protocol->ltcontrol(pp);
 }
 if (!sts && cp->rxtx[1].func == RXTXF_DATA) {
	CARDCONN *cx = &cp->chan[HANDLE_READ].cx;
	if (!sts) switch (pp->cmd) {
	case kSMDCLineCtlCmdSuspend:
		sts = sendmsg1(cx, MSG_SUSPEND);
		break;
	case kSMDCLineCtlCmdResume:
		sts = sendmsg1(cx, MSG_RESUME);
		break;
	case kSMDCLineCtlCmdResumeMonitorCarrier:
		sts = sendmsg1(cx, MSG_RESUME_MONITOR);
		break;
	default:
		sts = cp->rxtx[1].protocol->lrcontrol(pp);
	}
 }
 unlock_object(cp);
 return sts;
}

LOCALDEF int real_smdc_line_status(struct smdc_line_status_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 int rstate = RXTXS_DONE;
 int tstate = RXTXS_DONE;
 int sts = 0;
 lock_object(cp);
 if (cp->rxtx[0].func == RXTXF_DATA) {
 	rstate = cp->rxtx[0].state;
 }
 if (!sts && cp->rxtx[1].func == RXTXF_DATA) {
 	tstate = cp->rxtx[1].state;
 }
 	// fill in default values
 if (sts) {
 	// don't care about values
 } else if (rstate == RXTXS_RUN || tstate == RXTXS_RUN) {
	pp->link_status = kSMDCLinkStatusConnected;
 } else if (rstate == RXTXS_LAST || tstate == RXTXS_LAST) {
	pp->link_status = kSMDCLinkStatusDisconnecting;
 } else {
	pp->link_status = kSMDCLinkStatusNotConnected;
	if (cp->rxtx[0].func == RXTXF_DATA) {
		pp->channel->rxtx[0].func = RXTXF_NONE;
		cp->rxtx[0].protocol = &protable_speech;
	}
	if (cp->rxtx[1].func == RXTXF_DATA) {
		pp->channel->rxtx[1].func = RXTXF_NONE;
		cp->rxtx[1].protocol = &protable_speech;
	}
 }
 pp->rx_status = kSMDCRxStatusNoCarrier;	// all tx use this default rx
 pp->tx_status = kSMDCTxStatusSilent;		// all rx use this default tx
 pp->tx_cts = 1;				// default value
 if (!sts && cp->rxtx[0].func == RXTXF_DATA) {
	sts = cp->rxtx[0].protocol->ltstatus(pp);
 }
 if (!sts && cp->rxtx[1].func == RXTXF_DATA) {
	sts = cp->rxtx[1].protocol->lrstatus(pp);
 }
 unlock_object(cp);
 return sts;
}

LOCALDEF int real_smdc_tx_control(struct smdc_tx_control_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 int sts = 0;
 lock_object(cp);
 if (cp->rxtx[0].func != RXTXF_DATA) sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 if (!sts) {
	if (cp->rxtx[0].protocol->tcontrol) {
		sts = cp->rxtx[0].protocol->tcontrol(pp);
		if (sts > 0) { // unhandled by modem specific code
			CARDCONN *cx = &cp->chan[HANDLE_WRITE].cx;
			sts = 0;
			switch (pp->cmd) {
			case kSMDCTxCtlDiscard:	// nothing to do
				break;
			case kSMDCTxCtlNotifyOnCapacity: {
				struct ting_xfer xfer;
				xfer.ioctl_length = sizeof(xfer);
				xfer.minimum_bits = 8 * pp->capacity;
				xfer.timeout_milliseconds = 0;
				xfer.nomore = 0;
				if (pp->capacity > 2048) {
					sts = err(ERR_SM_BAD_PARAMETER);
				} else if (ERR(cx_setblock(cx, pp->blocking))) {
					sts = err(ERR_SM_DEVERR);
				} else if (ERR(cx_ioctl(cx,
					IOCTL_TiNG_WRXFER, &xfer, sizeof(xfer))))
					sts = err(ERR_SM_DEVERR);
			}
				break;
			case kSMDCTxCtlNotifyOnEmpty:
				// FIXME: use appropriate ioctl
				break;
			case kSMDCTxCtlNoNotifyOnEmpty:
				// FIXME: use appropriate ioctl
				break;
			case kSMDCTxCtlFinish: {
				struct ting_xfer xfer;
				xfer.ioctl_length = sizeof(xfer);
				xfer.minimum_bits = 1<<24;	// very large number
				xfer.timeout_milliseconds = 0;
				xfer.nomore = 1;
				if (ERR(cx_ioctl(cx,
					IOCTL_TiNG_WRXFER, &xfer, sizeof(xfer)))) {
					sts = err(ERR_SM_DEVERR);
					break;
				}
				cp->rxtx[0].state = RXTXS_LAST;
				break;
			}
			default:
				sts = err(ERR_SM_BAD_PARAMETER);
				break;
			}
		}
	} else sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 }
 unlock_object(cp);
 return sts;
}

LOCALDEF int real_smdc_tx_status(struct smdc_tx_status_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 int sts = 0;
 struct ting_message msg;
 lock_object(cp);
 if (cp->rxtx[0].func != RXTXF_DATA) sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 if (!sts) sts = checkmsgs(cp, 0, &msg);
 if (!sts) {
	if (cp->rxtx[0].state == RXTXS_DONE) {
		pp->status = kSMDCTxStatusFinished;
		cp->rxtx[0].func = RXTXF_NONE;
	} else if (cp->rxtx[0].protocol->tstatus) {
			// set up default report
		if (cp->rxtx[0].underrun) {
			pp->status = kSMDCTxStatusUnderrun;
			cp->rxtx[0].underrun = 0;
		} else if (cp->rxtx[0].state != RXTXS_RUN) {
			pp->status = kSMDCTxStatusFinishing;
		} else if ((pp->capacity = cp->rxtx[0].dataready / 8)) {
			// if (?)  pp->status = kSMDCTxStatusEmpty;
			pp->status = kSMDCTxStatusHasCapacity;
		} else pp->status = kSMDCTxStatusNoCapacity;
		sts = cp->rxtx[0].protocol->tstatus(pp, &msg.msg[0]);
	} else sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 }
 unlock_object(cp);
 return sts;
}

LOCALDEF int real_smdc_rx_control(struct smdc_rx_control_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 int sts = 0;
 lock_object(cp);
 if (cp->rxtx[1].func != RXTXF_DATA) sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 if (!sts) {
	if (cp->rxtx[1].protocol->rcontrol) {
		CARDCONN *cx = &cp->chan[HANDLE_READ].cx;
		switch (pp->cmd) {
		case kSMDCRxCtlDiscard:
			for (;;) {
				unsigned int len = cp->rxtx[1].dataready / 8;
				char *bp;
				if (!len) break;
				bp = alloc(0, len);
				if (!bp) {
					sts = err(ERR_SM_OS_RESOURCE_PROBLEM);
					break;
				}
				if (ERR(cx_read(cx, bp, len, &len))) {
					sts = err(ERR_SM_DEVERR);
					free(bp);
					break;
				}
				free(bp);
				if (!len) {
					break;
				} else {
					cp->rxtx[1].dataready -= len * 8;
				}
			}
			break;
		case kSMDCRxCtlNotifyOnData: {
			struct ting_xfer xfer;
			xfer.ioctl_length = sizeof(xfer);
			xfer.minimum_bits = 8 * pp->min_to_collect;
			xfer.timeout_milliseconds = pp->min_idle;
			xfer.nomore = 0;
			if (pp->min_to_collect > 2048) {
				sts = err(ERR_SM_BAD_PARAMETER);
#if 0
			FIXME: default is 'on' since otherwise simply
			calling smdc_rx_data() returns no data.
			However the field is 'blocking' so it would
			default to 'off'. Maybe rename to
			'nonblocking'?
			} else if (ERR(cp->lvtbl->io_setblock(cx, pp->blocking))) {
				sts = err(ERR_SM_DEVERR);
#endif
			} else if (ERR(cx_ioctl(cx,
				IOCTL_TiNG_WRXFER, &xfer, sizeof(xfer))))
				sts = err(ERR_SM_DEVERR);
		}
			break;
		default:
			sts = cp->rxtx[1].protocol->rcontrol(pp);
		}
	} else sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 }
 unlock_object(cp);
 return sts;
}

LOCALDEF int real_smdc_rx_status(struct smdc_rx_status_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 int sts = 0;
 struct ting_message msg;
 lock_object(cp);
 if (cp->rxtx[1].func != RXTXF_DATA) sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 if (!sts) sts = checkmsgs(cp, 1, &msg);
 if (!sts) {
	pp->flow = 0;		// default
	if (cp->rxtx[1].state == RXTXS_DONE) {
		pp->status = kSMDCRxStatusFinished;
		cp->rxtx[1].func = RXTXF_NONE;
	} else if (cp->rxtx[1].protocol->rstatus) {
			// prepare default report
		pp->available_octets = cp->rxtx[1].dataready / 8;
		if (cp->rxtx[1].underrun) {
			pp->status = kSMDCRxStatusOverrun;
			cp->rxtx[1].underrun = 0;
		} else if (pp->available_octets) {
			pp->status = kSMDCRxStatusData;
		} else pp->status = kSMDCRxStatusEmpty;
		sts = cp->rxtx[1].protocol->rstatus(pp, &msg.msg[0]);
	} else sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 }
 unlock_object(cp);
 return sts;
}

LOCALDEF int real_smdc_stop(struct smdc_stop_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 int sts = 0;
 lock_object(cp);
 if (cp->rxtx[0].func != RXTXF_DATA && cp->rxtx[1].func != RXTXF_DATA) {
 	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 }
 if (!sts && cp->rxtx[0].func == RXTXF_DATA) {
	if (cp->rxtx[0].protocol->stop) sts = cp->rxtx[0].protocol->stop(pp);
	else if (cp->rxtx[0].state != RXTXS_DONE) {
		sts = starttask(&cp->chan[HANDLE_WRITE].cx, 0, 0);
		cp->rxtx[0].state = RXTXS_LAST;
	}
 }
 if (!sts && cp->rxtx[1].func == RXTXF_DATA) {
	if (cp->rxtx[1].protocol->stop) sts = cp->rxtx[1].protocol->stop(pp);
	else if (cp->rxtx[1].state != RXTXS_DONE) {
		sts = starttask(&cp->chan[HANDLE_READ].cx, 0, 0);
		cp->rxtx[1].state = RXTXS_LAST;
	}
 }
 unlock_object(cp);
 return sts;
}

LOCALDEF int real_smdc_rx_stop(struct smdc_stop_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 int sts = 0;
 lock_object(cp);
 if (cp->rxtx[1].func != RXTXF_DATA) {
 	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else {
	if (cp->rxtx[1].protocol->stop) sts = cp->rxtx[1].protocol->stop(pp);
	else if (cp->rxtx[1].state != RXTXS_DONE) {
		sts = starttask(&cp->chan[HANDLE_READ].cx, 0, 0);
		cp->rxtx[1].state = RXTXS_LAST;
	}
 }
 unlock_object(cp);
 return sts;
}

LOCALDEF int real_smdc_tx_stop(struct smdc_stop_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 int sts = 0;
 lock_object(cp);
 if (cp->rxtx[0].func != RXTXF_DATA) {
 	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else {
	if (cp->rxtx[0].protocol->stop) sts = cp->rxtx[0].protocol->stop(pp);
	else if (cp->rxtx[0].state != RXTXS_DONE) {
		sts = starttask(&cp->chan[HANDLE_WRITE].cx, 0, 0);
		cp->rxtx[0].state = RXTXS_LAST;
	}
 }
 unlock_object(cp);
 return sts;
}

LOCALDEF int real_smdc_tx_data(struct smdc_data_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 unsigned wlen;
 int sts = 0;
 ERRCODE e;
 if (cp->rxtx[0].func != RXTXF_DATA) return err(ERR_SM_WRONG_CHANNEL_STATE);
 e = cx_write(&cp->chan[HANDLE_WRITE].cx, pp->data, pp->max_length, &wlen);
 if (ERR(e)) {
#ifdef EAGAIN
	if (e.errnum == EAGAIN) sts = err(ERR_SM_NO_CAPACITY);
	else 
#endif
		sts = err(ERR_SM_DEVERR);
 } else pp->done_length = wlen;
 return sts;
}

LOCALDEF int real_smdc_rx_data(struct smdc_data_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 unsigned done;
 int sts = 0;
 ERRCODE e;
 if (cp->rxtx[1].func != RXTXF_DATA) return err(ERR_SM_WRONG_CHANNEL_STATE);
 e = cx_read(&cp->chan[HANDLE_READ].cx, pp->data, pp->max_length, &done);
 if (ERR(e)) {
	sts = err(ERR_SM_DEVERR);
 } else pp->done_length = done;
 return sts;
}
