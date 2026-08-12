#ifndef INCLUDED_PX_MOD_H
#define INCLUDED_PX_MOD_H

#include "paranoia.h"
#include "assp_group.h"
#include "refcnt.h"

typedef struct {
	REFCNT ref;
	ASSP_GROUP modgroup;
	unsigned modmask;
	PX_CARDID *cid;
} PX_MOD;

#include "px_cardid.h"

LOCALDEC PX_MOD *allocateProsodyXModule(PX_CARDID *cid, int modmask, char* remaddr, int port);
LOCALDEC void shutdownProsodyXModule(PX_MOD *pxmod);
LOCALDEC void freeProsodyXModule(PX_MOD *pxmod);

#endif
