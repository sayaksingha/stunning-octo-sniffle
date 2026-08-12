/* TiNGo_tdmtx.h - TiNG TDMtx objects */

#ifndef INCLUDED_TINGO_TDMTX_H
#define INCLUDED_TINGO_TDMTX_H

#include "paranoia.h"
#include "TiNGo_mod.h"
#include "TiNGo_datafeed.h"

struct tSMTDMtxId_struct {
 PARAMUTX pm;
 struct module *mod;
 DATAFEED dataf;
 int stream;
 int timeslot;
 enum TiNG_COMPAND_TYPE type;
};

LOCALDEC void tdmtx_dtor(struct tSMTDMtxId_struct *vtp);
LOCALDEC struct tSMTDMtxId_struct *init_tdmtx(struct module *mod);

#endif
