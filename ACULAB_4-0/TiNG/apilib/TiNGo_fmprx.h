/* TiNGo_fmprx.h - TiNG FMPrx objects */

#ifndef INCLUDED_TINGO_FMPRX_H
#define INCLUDED_TINGO_FMPRX_H

#include "../TiNG.h"
#include "paranoia.h"
#include "smgroove.h"
#include "smtypes.h"
#include "TiNGo_datafeed.h"

struct tSMFMPrxId_struct {
 PARAMUTX pm;
 tSMGroove groove;
 struct module* mod;
 DATAFEED  dataf;
 enum {FMPRX_STATE_IDLE, FMPRX_STATE_RUNNING, FMPRX_STATE_STOPPING} state;
};

LOCALDEC void fmprx_dtor(struct tSMFMPrxId_struct *vrp);
LOCALDEC struct tSMFMPrxId_struct *init_fmprx(struct module* mod);

#include "TiNGo_mod.h"

#endif
