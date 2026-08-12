#ifndef INCLUDED_CONFMASK_H
#define INCLUDED_CONFMASK_H

#include "../cmndev.h"

typedef struct {
	unsigned maxconf;
	U32 *mask;
} CONFMASK;

LOCALDEC void confmask(CONFMASK *msp);
LOCALDEC void confmask_dtor(CONFMASK *msp);
	// test if id is in mask, return non-zero if it is
LOCALDEC int confmask_test(CONFMASK *msp, unsigned id);
	// return next ID in mask, or CONFMASK_ID_NONE if no more
	// CONFMASK_ID_NONE is also the predecessor of the first ID
LOCALDEC int confmask_next(CONFMASK *msp, unsigned id);
	// return non-zero if mask is empty
	// i.e. confmask_next(msp, CONFMASK_ID_NONE) == CONFMASK_ID_NONE
LOCALDEC int confmask_empty(CONFMASK *msp);
	// add ID to confmask, returning prosody error
LOCALDEC int confmask_set(CONFMASK *msp, unsigned id);
	// remove ID from confmask
LOCALDEC void confmask_clr(CONFMASK *msp, unsigned id);

#endif
