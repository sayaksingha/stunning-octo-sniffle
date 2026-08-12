/* TiNGo_coll.h - TiNG collector objects */

#ifndef INCLUDED_TINGO_COLLECTOR_H
#define INCLUDED_TINGO_COLLECTOR_H

#include "paranoia.h"
#include "TiNGo_mod.h"
#include "smgroove.h"

typedef struct tSMCollectorId_struct {
	PARAMUTX pm;
	struct module *mod;
	tSMGroove groove;
} COLLECTOR;

LOCALDEC void collector_dtor(struct tSMCollectorId_struct *vtp);
LOCALDEC struct tSMCollectorId_struct *init_collector(struct module *mod);

#endif
