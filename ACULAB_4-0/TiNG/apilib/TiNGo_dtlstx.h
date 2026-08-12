/* TiNGo_dtlstx.h - TiNG DTLStx objects */

#ifndef INCLUDED_TINGO_DTLSTX_H
#define INCLUDED_TINGO_DTLSTX_H

#include "../TiNG.h"
#include "paranoia.h"
#include "smgroove.h"
#include "smtypes.h"

struct tSMDTLStxId_struct {
	PARAMUTX pm;
	struct module *mod;
	tSMGroove groove;
	enum {
		DTLSTX_STATE_IDLE, DTLSTX_STATE_RUNNING, DTLSTX_STATE_STOPPING
	} state;
	uint32_t min_bits;
};

LOCALDEC void dtlstx_dtor(struct tSMDTLStxId_struct *vtp);
LOCALDEC struct tSMDTLStxId_struct *init_dtlstx(struct module *mod);

#include "TiNGo_mod.h"

#endif

