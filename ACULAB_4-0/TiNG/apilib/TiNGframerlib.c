/* TiNGframerlib.c */

#include "TiNGo_framer.h"
#include "TiNGo_datafeed.h"
#include "smframer.h"
#include "smtypes.h"
#include "cardconn.h"
#include "../hsif/kernel.h"
#include "../hsif/subtask.h"
#include "../hsif/typecode.h"
#include "../hsif/framer.h"
#include "timestamp.h"
#include "trace.h"
#include "TiNGcommon.h"

STATIC int real_sm_framer_get_datafeed(struct sm_framer_datafeed_parms *datafeedp)
{
 tSMFramerId framer = datafeedp->framer;
 if(datafeedp->direction < 0) return err(ERR_SM_NO_RESOURCES);
 if(datafeedp->direction > 1) return err(ERR_SM_NO_RESOURCES);
 lock_object(framer);
 datafeedp->datafeed = framer->dataf[datafeedp->direction].vtbl_data;
 unlock_object(framer);
 return 0;
}

STATIC int real_sm_framer_create(struct sm_framer_create_parms *framerp)
{
 struct module *mod = modid2lockedmodule(framerp->module);
 tSMFramerId framer;
 tSMGroove *grv;
 int sts = 0;
 if (!mod) return err(ERR_SM_NO_SUCH_MODULE);
 framer = init_framer(mod);
 if (!framer) sts = err(ERR_SM_NO_RESOURCES);
 else {
 	framerp->framer = framer;
	sts = framer->groove.starttask(&framer->groove, TASKTC_ASAFRAMER, 0);
	grv = &framer->groove;
	if (!sts) {
		sts = mod->card->vtbl->setup_df(framer->dataf[0], mod, 0);
		if (!sts) sts = grv->engage_df(grv, framer->dataf[0], DFC_FRAMER_TX_OUT);
		sts = mod->card->vtbl->setup_df(framer->dataf[1], mod, 0);
		if (!sts) sts = grv->engage_df(grv, framer->dataf[1], DFC_FRAMER_RX_OUT);

	}
	framerp->framer->state = FRAMER_STATE_RUNNING;
	if (sts) framer_dtor(framer);
	else make_object(framer);
 }
 unlock_module(mod);
 return sts;
}

STATIC int real_sm_framer_datafeed_connect(struct sm_framer_datafeed_connect_parms *datafeedp)
{
 const enum dfconntyp dfc[2] = {DFC_FRAMER_TX_IN, DFC_FRAMER_RX_IN};
 tSMFramerId framer = datafeedp->framer;
 DATAFEED df;
 tSMGroove *grv;
 int sts;

 if(datafeedp->direction < 0) return err(ERR_SM_NO_RESOURCES);
 if(datafeedp->direction > 1) return err(ERR_SM_NO_RESOURCES);

 lock_object(framer);
 grv = &framer->groove;
 df.vtbl_data = datafeedp->data_source;
 sts = grv->connect_df(grv, &framer->dfin[datafeedp->direction], df);
 if (!sts) sts = grv->engage_df(grv, df, dfc[datafeedp->direction]);
 unlock_object(framer);
 return sts;
}

STATIC int real_sm_framer_datafeed_disconnect(struct sm_framer_datafeed_disconnect_parms *datafeedp)
{
 const enum dfconntyp dfc[2] = {DFC_FRAMER_TX_IN, DFC_FRAMER_RX_IN};
 tSMFramerId framer = datafeedp->framer;
 tSMGroove *grv;
 int sts;
 if(datafeedp->direction < 0) return err(ERR_SM_DEVERR);
 if(datafeedp->direction > 1) return err(ERR_SM_DEVERR);
 lock_object(framer);
 grv = &framer->groove;
 sts = grv->connect_df(grv, &framer->dfin[datafeedp->direction], NULL_DATAFEED);
 if (!sts) sts = grv->engage_df(grv, NULL_DATAFEED, dfc[datafeedp->direction]);
 unlock_object(framer);
 return sts;
}

STATIC int real_sm_framer_destroy(tSMFramerId framer)
{
 if (framer) framer_dtor(framer);
 return 0;
}

STATIC int real_sm_framer_get_event(struct sm_framer_event_parms *eventp)
{
 int sts;
 lock_object(eventp->framer);
 sts = eventp->framer->groove.getev(&eventp->framer->groove, &eventp->event);
 unlock_object(eventp->framer);
 return sts;
}

STATIC int unsafe_sm_framer_status(struct sm_framer_status_parms *statusp)
{
	int sts;
	tSMFramerId framer = statusp->framer;
	tSMGroove *grv = &framer->groove;
	uint32_t msg[16];
	unsigned long code;
	if (framer->state == FRAMER_STATE_IDLE) {
		return err(ERR_SM_WRONG_CHANNEL_STATE);
	}
	sts = grv->rdmsg(grv, msg, SMGROOVE_RDMSG_FLAG_NOWAIT);
	if (sts) return sts;
	code = msg[0] & 0xffff0000;
	statusp->status = kSMFramerStatusRunning;
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
		framer->state = FRAMER_STATE_IDLE;
		statusp->status = kSMFramerStatusStopped;
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
	} else if (code == MESSAGE(MSG_FRAMER_NEWSTATE, 2)) {
		statusp->new_state = msg[1];
		statusp->status = kSMFramerStatusNewState;
	}
	return sts;
}

STATIC int real_sm_framer_status(struct sm_framer_status_parms *statusp)
{
	int sts;
	tSMFramerId framer = statusp->framer;
	lock_object(framer);
	sts = unsafe_sm_framer_status(statusp);
	unlock_object(framer);
	return sts;
}
#include "../pubdoc/gen/prosframerapi.i"
