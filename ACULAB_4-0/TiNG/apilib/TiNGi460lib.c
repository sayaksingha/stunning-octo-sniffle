/* TiNGi460lib.c */

#include "TiNGo_i460mux.h"
#include "TiNGo_datafeed.h"
#include "smi460.h"
#include "cardconn_px.h"
#include "../hsif/kernel.h"
#include "../hsif/subtask.h"
#include "../hsif/typecode.h"
#include "../hsif/vmp.h"
#include "../hsif/i460.h"
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

STATIC int real_sm_i460mux_get_datafeed(struct sm_i460mux_datafeed_parms *datafeedp)
{
 tSMI460muxId i460mux = datafeedp->i460mux;
 lock_object(i460mux);
 datafeedp->datafeed = i460mux->dataf.vtbl_data;
 unlock_object(i460mux);
 return 0;
}

STATIC int real_sm_i460mux_create(struct sm_i460mux_create_parms *i460muxp)
{
 struct module *mod = modid2lockedmodule(i460muxp->module);
 tSMI460muxId i460mux;
 tSMGroove *grv;
 int sts = 0;
 if (!mod) return err(ERR_SM_NO_SUCH_MODULE);
 i460mux = init_i460mux(mod);
 if (!i460mux) sts = err(ERR_SM_NO_RESOURCES);
 else {
 	i460muxp->i460mux = i460mux;
	sts = i460mux->groove.starttask(&i460mux->groove, TASKTC_I460TX, 0);
	grv = &i460mux->groove;
	if (!sts) {
		sts = mod->card->vtbl->setup_df(i460mux->dataf, mod, 0);
		if (!sts) sts = grv->engage_df(grv, i460mux->dataf, DFC_OUT);

	}
	i460muxp->i460mux->state = I460MUX_STATE_RUNNING;
	if (sts) i460mux_dtor(i460mux);
	else make_object(i460mux);
 }
 unlock_module(mod);
 return sts;
}

STATIC int real_sm_i460mux_datafeed_connect(struct sm_i460mux_datafeed_connect_parms *datafeedp)
{
 const enum dfconntyp dfc[8] = {
	DFC_I460_0_IN, DFC_I460_1_IN, DFC_I460_2_IN, DFC_I460_3_IN,
	DFC_I460_4_IN, DFC_I460_5_IN, DFC_I460_6_IN, DFC_I460_7_IN };
 tSMI460muxId i460mux = datafeedp->i460mux;
 DATAFEED df;
 tSMGroove *grv;
 int sts;
 tSM_INT input_id;

 lock_object(i460mux);
  datafeedp->input_id = -1;
  for(input_id = 0; input_id < 8; input_id++) {
	if(!nonnull_datafeed(i460mux->dfin[input_id])) break;
  }
  if(input_id > 7) sts = err(ERR_SM_NO_RESOURCES);
  else {
 	grv = &i460mux->groove;
 	df.vtbl_data = datafeedp->data_source;
 	sts = grv->connect_df(grv, &i460mux->dfin[input_id], df);
 	if (!sts) sts = grv->engage_df(grv, df, dfc[input_id]);
 	if (!sts) datafeedp->input_id = input_id;
  }
 unlock_object(i460mux);
 return sts;
}

STATIC int real_sm_i460mux_datafeed_disconnect(struct sm_i460mux_datafeed_disconnect_parms *datafeedp)
{
 const enum dfconntyp dfc[8] = {
	DFC_I460_0_IN, DFC_I460_1_IN, DFC_I460_2_IN, DFC_I460_3_IN,
	DFC_I460_4_IN, DFC_I460_5_IN, DFC_I460_6_IN, DFC_I460_7_IN };
 tSMI460muxId i460mux = datafeedp->i460mux;
 tSMGroove *grv;
 int sts;
 if(datafeedp->input_id < 0) return err(ERR_SM_DEVERR);
 if(datafeedp->input_id > 7) return err(ERR_SM_DEVERR);
 lock_object(i460mux);
 grv = &i460mux->groove;
 sts = grv->connect_df(grv, &i460mux->dfin[datafeedp->input_id], NULL_DATAFEED);
 if (!sts) sts = grv->engage_df(grv, NULL_DATAFEED, dfc[datafeedp->input_id]);
 unlock_object(i460mux);
 return sts;
}

STATIC int real_sm_i460mux_destroy(tSMI460muxId i460mux)
{
 if (i460mux) i460mux_dtor(i460mux);
 return 0;
}

STATIC int real_sm_i460mux_get_event(struct sm_i460mux_event_parms *eventp)
{
 int sts;
 lock_object(eventp->i460mux);
 sts = eventp->i460mux->groove.getev(&eventp->i460mux->groove, &eventp->event);
 unlock_object(eventp->i460mux);
 return sts;
}

STATIC int unsafe_sm_i460mux_status(struct sm_i460mux_status_parms *statusp)
{
	int sts;
	tSMI460muxId i460mux = statusp->i460mux;
	tSMGroove *grv = &i460mux->groove;
	uint32_t msg[16];
	unsigned long code;
	if (i460mux->state == I460MUX_STATE_IDLE) {
		return err(ERR_SM_WRONG_CHANNEL_STATE);
	}
	sts = grv->rdmsg(grv, msg, SMGROOVE_RDMSG_FLAG_NOWAIT);
	if (sts) return sts;
	code = msg[0] & 0xffff0000;
	statusp->status = kSMI460muxStatusRunning;
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
		i460mux->state = I460MUX_STATE_IDLE;
		statusp->status = kSMI460muxStatusStopped;
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
	}
	return sts;
}

STATIC int real_sm_i460mux_status(struct sm_i460mux_status_parms *statusp)
{
	int sts;
	tSMI460muxId i460mux = statusp->i460mux;
	lock_object(i460mux);
	sts = unsafe_sm_i460mux_status(statusp);
	unlock_object(i460mux);
	return sts;
}
#include "../pubdoc/gen/prosi460muxapi.i"
