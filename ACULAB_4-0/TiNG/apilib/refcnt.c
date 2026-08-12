#ifndef CODE_INCLUDED_REFCNT_C
#define CODE_INCLUDED_REFCNT_C

#include "refcnt.h"

LOCALDEF void refcnt(REFCNT *rp, void (*freefn)(void *), void *p)
{
 rp->freefn = freefn;
 rp->freepar = p;
 mutx_init(&rp->pm.mx);
 rp->refs = 1;
 make_object(rp);
}

LOCALDEF void refcnt_inc(REFCNT *rp)
{
 lock_object(rp);
 rp->refs++;
 unlock_object(rp);
}

LOCALDEF int refcnt_dec(REFCNT *rp)
{
 int r;
 lock_object(rp);
 r = !--rp->refs;
 unlock_object(rp);
 if (r) {
	mutx_dtor(&rp->pm.mx);
	rp->freefn(rp->freepar);
 }
 return r;
}

#endif
