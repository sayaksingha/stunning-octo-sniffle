/* TiNGo_h223mux.h - TiNG FMPtx objects */

#ifndef INCLUDED_TINGO_H223MUX_H
#define INCLUDED_TINGO_H223MUX_H

#include "../TiNG.h"
#include "paranoia.h"
#include "smgroove.h"
#include "smtypes.h"

struct tSMH223muxId_struct {
	PARAMUTX pm;
	struct module *mod;
	tSMGroove groove;
	enum {
		H223MUX_STATE_IDLE, H223MUX_STATE_RUNNING, H223MUX_STATE_STOPPING
	} state;
	DATAFEED dataf[4];
	DATAFEED dfin[4];
};

LOCALDEC void h223mux_dtor(struct tSMH223muxId_struct *vtp);
LOCALDEC struct tSMH223muxId_struct *init_h223mux(struct module *mod);

#include "TiNGo_mod.h"

#endif

