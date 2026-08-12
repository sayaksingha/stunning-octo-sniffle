/* TiNGo_fmptx.h - TiNG FMPtx objects */

#ifndef INCLUDED_TINGO_FMPTX_H
#define INCLUDED_TINGO_FMPTX_H

#include "../TiNG.h"
#include "paranoia.h"
#include "smgroove.h"
#include "smtypes.h"

struct tSMFMPtxId_struct {
	PARAMUTX pm;
	struct module *mod;
	tSMGroove groove;
	enum {
		FMPTX_STATE_IDLE, FMPTX_STATE_RUNNING, FMPTX_STATE_STOPPING
	} state;
	DATAFEED dfin;
};

LOCALDEC void fmptx_dtor(struct tSMFMPtxId_struct *vtp);
LOCALDEC struct tSMFMPtxId_struct *init_fmptx(struct module *mod);

#include "TiNGo_mod.h"

#endif

