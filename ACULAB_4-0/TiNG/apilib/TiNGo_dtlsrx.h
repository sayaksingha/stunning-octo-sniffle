/* TiNGo_dtlsrx.h - TiNG DTLSrx objects */

#ifndef INCLUDED_TINGO_DTLSRX_H
#define INCLUDED_TINGO_DTLSRX_H

#include "../TiNG.h"
#include "paranoia.h"
#include "smgroove.h"
#include "smtypes.h"
#include "px_prostype.h"

struct tSMDTLSrxId_struct {
 PARAMUTX pm;
 tSMGroove groove;
 struct module* mod;
 enum {DTLSRX_STATE_IDLE, DTLSRX_STATE_RUNNING, DTLSRX_STATE_STOPPING} state;
 tSMVMPrxId vmprx;
};

LOCALDEC void dtlsrx_dtor(struct tSMDTLSrxId_struct *vrp);
LOCALDEC struct tSMDTLSrxId_struct *init_dtlsrx(struct module* mod);

#include "TiNGo_mod.h"

#endif
