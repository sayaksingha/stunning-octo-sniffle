/* TiNGo_i460mux.h - TiNG i460 objects */

#ifndef INCLUDED_TINGO_I460MUX_H
#define INCLUDED_TINGO_I460MUX_H

#include "../TiNG.h"
#include "paranoia.h"
#include "smgroove.h"
#include "smtypes.h"

struct tSMI460muxId_struct {
	PARAMUTX pm;
	struct module *mod;
	tSMGroove groove;
	enum {
		I460MUX_STATE_IDLE, I460MUX_STATE_RUNNING, I460MUX_STATE_STOPPING
	} state;
	DATAFEED dataf;
	DATAFEED dfin[8];
};

LOCALDEC void i460mux_dtor(struct tSMI460muxId_struct *vtp);
LOCALDEC struct tSMI460muxId_struct *init_i460mux(struct module *mod);

#include "TiNGo_mod.h"

#endif

