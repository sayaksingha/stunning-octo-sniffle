/* TiNGo_framer.h - TiNG Framer objects */

#ifndef INCLUDED_TINGO_FRAMER_H
#define INCLUDED_TINGO_FRAMER_H

#include "../TiNG.h"
#include "paranoia.h"
#include "smgroove.h"
#include "smtypes.h"

struct tSMFramerId_struct {
	PARAMUTX pm;
	struct module *mod;
	tSMGroove groove;
	enum {
		FRAMER_STATE_IDLE, FRAMER_STATE_RUNNING, FRAMER_STATE_STOPPING
	} state;
	DATAFEED dataf[2];
	DATAFEED dfin[2];
};

LOCALDEC void framer_dtor(struct tSMFramerId_struct *vtp);
LOCALDEC struct tSMFramerId_struct *init_framer(struct module *mod);

#include "TiNGo_mod.h"

#endif

