/* TiNGo_vidplay.h - TiNG vidplay objects */

#ifndef INCLUDED_TINGO_VIDPLAY_H
#define INCLUDED_TINGO_VIDPLAY_H

#include "paranoia.h"
#include "TiNGo_mod.h"
#include "px_prostype.h"
#include "smgroove.h"

struct tSMAVFplayId_struct {
	PARAMUTX pm;
	struct module *mod;
	tSMGroove groove;
	DATAFEED dataf; //equivalent to tSMDatafeedId
	enum {
		ST_REPLAY_IDLE,	// idle
		ST_REPLAY_WAITSYNC,	// started but waiting to be notified of it's sync partner
		ST_REPLAY_RUN,	// configured for a function
		ST_REPLAY_LAST,	// terminating: e.g. awaiting DEAD msg
						// put 0 bytes or abort
		ST_REPLAY_DONE,	// task is DEAD but not yet reported
	} state;
	struct tSMAVFplayId_struct *sync_partner;
};

LOCALDEC void avfplay_dtor(struct tSMAVFplayId_struct *vtp);
LOCALDEC struct tSMAVFplayId_struct *init_avfplay(struct module *mod);

#endif
