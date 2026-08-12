/* TiNGo_sprt.h */

#ifndef INCLUDED_TINGO_SPRT_H
#define INCLUDED_TINGO_SPRT_H

#include "../TiNG.h"
#include "paranoia.h"
#include "smgroove.h"
#include "smtypes.h"

struct tSMSPRTId_struct {
	PARAMUTX pm;
	struct module *mod;
	tSMGroove groove;
	tSMGroove groove_packetiser;
	DATAFEED dataf;
	DATAFEED dfin;
	int data_chan;
	uint32_t data_format_tc[2]; // 0 for tx, 1 for rx
};

LOCALDEC void sprt_dtor(struct tSMSPRTId_struct *vtp);
LOCALDEC struct tSMSPRTId_struct *init_sprt(struct module *mod);

#include "TiNGo_mod.h"

#endif

