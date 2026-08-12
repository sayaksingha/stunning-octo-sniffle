/* TiNGo_sntp.h */

#ifndef INCLUDED_TINGO_SNTP_H
#define INCLUDED_TINGO_SNTP_H

#include "../TiNG.h"
#include "paranoia.h"
#include "smgroove.h"
#include "smtypes.h"

struct tSMSNTPId_struct {
	PARAMUTX pm;
	struct module *mod;
	tSMGroove groove;
};

LOCALDEC void sntp_dtor(struct tSMSNTPId_struct *vtp);
LOCALDEC struct tSMSNTPId_struct *init_sntp(struct module *mod);

#include "TiNGo_mod.h"

#endif

