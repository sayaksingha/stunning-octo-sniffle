/* TiNGsmdiag.h - Diagnostic API */

#ifndef INCLUDED_SMDIAG_H
#define INCLUDED_SMDIAG_H

#include "smdiag.h"

LOCALDEC int real_sm_dspreg(struct sm_dspreg_parms *pp);
LOCALDEC int real_sm_dspregall(struct sm_dspregall_parms *pp);
LOCALDEC int real_sm_mem(struct sm_mem_parms *pp);

#endif
