/* TiNGo_tdmrx.h - TiNG VMPrx objects */

#ifndef INCLUDED_TINGO_TDMRX_H
#define INCLUDED_TINGO_TDMRX_H

#include "TiNGo_datafeed.h"
#include "TiNGo_mod.h"
#include "paranoia.h"

struct tSMTDMrxId_struct {
 PARAMUTX pm;
 struct module *mod;
 DATAFEED dataf; //equivalent to tSMDatafeedId
 int stream;
 int timeslot;
 enum TiNG_COMPAND_TYPE type;
};

LOCALDEC void tdmrx_dtor(struct tSMTDMrxId_struct *vrp);
LOCALDEC struct tSMTDMrxId_struct *init_tdmrx(struct module *mod);

#endif
