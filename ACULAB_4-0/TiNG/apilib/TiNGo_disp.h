/* TiNGo_disp.h - TiNG dispatcher objects */

#ifndef INCLUDED_TINGO_DISPATCHER_H
#define INCLUDED_TINGO_DISPATCHER_H

#include "paranoia.h"
#include "TiNGo_mod.h"
#include "smgroove.h"

typedef struct tSMDispatcherId_struct {
	PARAMUTX pm;
	struct module *mod;
	tSMGroove groove;
} DISPATCHER;

LOCALDEC void dispatcher_dtor(struct tSMDispatcherId_struct *vtp);
LOCALDEC struct tSMDispatcherId_struct *init_dispatcher(struct module *mod);

#endif
