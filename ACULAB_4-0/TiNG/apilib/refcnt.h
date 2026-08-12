#ifndef INCLUDED_REFCNT_H
#define INCLUDED_REFCNT_H

#include "paranoia.h"

typedef struct {
	PARAMUTX pm;
	unsigned refs;
	void (*freefn)(void *par);	// called when refs becomes zero
	void *freepar;			// parameter for freefn
} REFCNT;

	// create a reference count
LOCALDEC void refcnt(REFCNT *rp, void (*freefn)(void *), void *p);
	// increment a reference count
LOCALDEC void refcnt_inc(REFCNT *rp);
	// decrement a reference count
	// if reference count has become zero, destroys reference
	// count and returns non-zero. Otherwise returns zero.
LOCALDEC int refcnt_dec(REFCNT *rp);

#endif
