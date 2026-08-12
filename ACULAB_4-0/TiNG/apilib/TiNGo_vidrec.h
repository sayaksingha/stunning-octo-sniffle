/* TiNGo_vidrec.h - TiNG vidrec objects */

#ifndef INCLUDED_TINGO_VIDREC_H
#define INCLUDED_TINGO_VIDREC_H

#include "paranoia.h"
#include "TiNGo_mod.h"
#include "px_prostype.h"
#include "smgroove.h"

struct tSMAVFrecId_struct {
	PARAMUTX pm;
	struct module *mod;
	tSMGroove groove;
	DATAFEED dataf; //equivalent to tSMDatafeedId
	enum {
		ST_RECORD_IDLE,	// idle
		ST_RECORD_WAITSYNC,	// started but waiting for sync_partner to start
		ST_RECORD_RUN,	// configured for a function
		ST_RECORD_LAST,	// terminating: e.g. awaiting DEAD msg
						// put 0 bytes or abort
		ST_RECORD_DONE,	// task is DEAD but not yet reported
	} state;
	struct tSMAVFrecId_struct *sync_partner;
};

LOCALDEC void avfrec_dtor(struct tSMAVFrecId_struct *vtp);
LOCALDEC struct tSMAVFrecId_struct *init_avfrec(struct module *mod);

#endif
