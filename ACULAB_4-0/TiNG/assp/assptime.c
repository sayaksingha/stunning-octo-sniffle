/* assptest.c - test ASSP */

#include <stdlib.h>
#include <stdio.h>
#include "assp.h"
#include "../apilib/globaltime.h"
#include "timefn.h"

ABSTIME time_when(RELTIME until)
{
 ABSTIME a_now;
#ifdef INLINE_TIMEFUNC
 a_now = now_u64();
#else
 a_now.t = now_u64();
#endif
 return time_add(a_now, until);
}

RELTIME time_until(ABSTIME when)
{
 ABSTIME a_now;
#ifdef INLINE_TIMEFUNC
 a_now = now_u64();
#else
 a_now.t = now_u64();
#endif
 return time_sub(when, a_now);
}

// not right, but simple!
int_fast64_t time_tai_uS(void)
{
 return now_u64();
}
