#ifndef INCLUDED_GLOBALTIME_H
#define INCLUDED_GLOBALTIME_H

#include <stdint.h>

#ifdef LOCALDEC
// if LOCALDEC is not defined we're external to the api lib
LOCALDEC void timecal_start(void);
LOCALDEC uint_fast64_t timecal_update(void); // returns ticks per us
#endif
	// we have to let this be global, so avoid an unqualified name
#define now_u64 aculab_TiNGcore_now_u64
uint_fast64_t now_u64(void);

#endif
