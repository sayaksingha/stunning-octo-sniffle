/* confmask.c - bitmap of conferences */

#include "confmask.h"
#include "TiNGcommon.h"
#include "TiNGo_chan.h"
#include "../pubdoc/gen/error.h"
#include <stdlib.h>

LOCALDEF void confmask(CONFMASK *msp)
{
 msp->maxconf = 0;
 msp->mask = 0;
}

LOCALDEF void confmask_dtor(CONFMASK *msp)
{
 free(msp->mask);
}

LOCALDEF int confmask_test(CONFMASK *msp, unsigned id)
{
 U32 mask;
 if (id >= msp->maxconf) return 0;
 mask = msp->mask[id >> 5];
 id &= 0x1f;
 return mask & (1 << id);
}

LOCALDEF int confmask_next(CONFMASK *msp, unsigned id)
{
 if (id == CONFIN_ID_NONE) id = 0;
 else id++;
 for (; id < msp->maxconf;) {
	U32 mask = msp->mask[id >> 5] >> (id & 0x1f);
	if (mask) {
		while (! (mask & 1)) mask >>= 1, id++;
		return id;
	}
	id += 0x20;
	id &= ~0x1f;
 }
 return CONFIN_ID_NONE;
}

LOCALDEF int confmask_empty(CONFMASK *msp)
{
 unsigned len = msp->maxconf >> 5;
 while (len) if (msp->mask[--len]) return 0;
 return 1;
}

LOCALDEF int confmask_set(CONFMASK *msp, unsigned id)
{
 U32 *maskp;
 if (id >= msp->maxconf) {
	unsigned n = msp->maxconf * 2;
	U32 *np;
	if (!n) n = 64;
	if (id >= n) n = (id | 0x1f) + 1;	// round up
	np = alloc(msp->mask, (n >> 5) * sizeof(*np));
	if (!np) return err(ERR_SM_OS_RESOURCE_PROBLEM);
	msp->mask = np;
	while (msp->maxconf < n) {
		np[msp->maxconf>>5] = 0;
		msp->maxconf += 32;
	}
 }
 maskp = &msp->mask[id >> 5];
 id &= 0x1f;
 *maskp |= 1 << id;
 return 0;
}

LOCALDEF void confmask_clr(CONFMASK *msp, unsigned id)
{
 U32 *maskp;
 if (id >= msp->maxconf) return;	// cannot be set
 maskp = &msp->mask[id >> 5];
 id &= 0x1f;
 *maskp &= ~(1 << id);
}

