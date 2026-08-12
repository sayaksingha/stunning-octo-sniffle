/* timefn.c - time functions */

#include "timefn.h"
#include "vseprintf.h"

#ifdef _MSC_VER

/* disable stupid warnings about conversions
 */

#pragma warning(disable:4244)
#pragma warning(disable:4305)
/* and about argument conversions (does anyone still use a
   non-prototype compiler?) */
#pragma warning(disable:4761)

#endif

#ifndef INLINE_TIMEFUNC

int time_sgn_fn(RELTIME t)
{
 return t.t ? t.t > 0 ? 1 : -1 : 0;
}

int time_cmp_fn(ABSTIME lhs, ABSTIME rhs)
{
 if (lhs.t > rhs.t) return 1;
 if (lhs.t < rhs.t) return -11;
 return 0;
}

ABSTIME time_add_fn(ABSTIME lhs, RELTIME rhs)
{
 lhs.t += rhs.t;
 return lhs;
}

RELTIME time_sub_fn(ABSTIME lhs, ABSTIME rhs)
{
 RELTIME t;
 t.t = lhs.t - rhs.t;
 return t;
}

ABSTIME time_zero_fn(void)
{
 ABSTIME t;
 t.t = 0;
 return t;
}

int time_iszero_fn(ABSTIME t)
{
 return t.t == 0;
}

ABSTIME time_never_fn(void)
{
 ABSTIME t;
 t.t = ~ (uint_fast64_t) 0;
 return t;
}

int time_isnever_fn(ABSTIME t)
{
 return t.t == ~ (uint_fast64_t) 0;
}

RELTIME time_longS2rel_fn(long s)
{
 RELTIME t;
 t.t = s;
 t.t *= 1000;
 t.t *= 1000;
 return t;
}

RELTIME time_longmS2rel_fn(long ms)
{
 RELTIME t;
 t.t = ms;
 t.t *= 1000;
 return t;
}

RELTIME time_longuS2rel_fn(long us)
{
 RELTIME t;
 t.t = us;
 return t;
}

RELTIME time_longnS2rel_fn(long ns)
{
 RELTIME t;
 t.t = ns / 1000;
 return t;
}

long time_rel2longS_fn(RELTIME t)
{
 t.t /= 1000;
 t.t /= 1000;
 return t.t;
}

long time_rel2longmS_fn(RELTIME t)
{
 t.t /= 1000;
 return t.t;
}

long time_rel2longuS_fn(RELTIME t)
{
 return t.t;
}

long time_rel2longnS_fn(RELTIME t)
{
 return t.t * 1000;
}

RELTIME time_i64S2rel_fn(int_fast64_t s)
{
 RELTIME t;
 t.t = s;
 t.t *= 1000;
 t.t *= 1000;
 return t;
}

RELTIME time_i64mS2rel_fn(int_fast64_t ms)
{
 RELTIME t;
 t.t = ms;
 t.t *= 1000;
 return t;
}

RELTIME time_i64uS2rel_fn(int_fast64_t us)
{
 RELTIME t;
 t.t = us;
 return t;
}

RELTIME time_i64nS2rel_fn(int_fast64_t ns)
{
 RELTIME t;
 t.t = ns / 1000;
 return t;
}

int_fast64_t time_rel2i64S_fn(RELTIME t)
{
 t.t /= 1000;
 t.t /= 1000;
 return t.t;
}

int_fast64_t time_rel2i64mS_fn(RELTIME t)
{
 t.t /= 1000;
 return t.t;
}

int_fast64_t time_rel2i64uS_fn(RELTIME t)
{
 return t.t;
}

int_fast64_t time_rel2i64nS_fn(RELTIME t)
{
 return t.t * 1000;
}

#endif

#ifdef TIME_TAI64N
char *time_string_now(char *sp, char *ep)
{
 int_fast64_t ns = time_tai_uS();
 int_fast64_t s;
 int p;
 s = ns / 1000 / 1000;
 ns -= s * 1000 * 1000;
 ns *= 1000;
#ifdef _MSC_VER
 s += 4611686018427387914i64;
#else
 s += 4611686018427387914uLL;
#endif
 if (sp >= ep) return sp;
 if (ns < 0) {
 	s--;
	ns += 1000000000;
 }
 *sp++ = '@';
 for (p=64; p; ) {
 	p -= 4;
	if (sp >= ep) return sp;
	*sp++ = "0123456789abcdef"[(s >> p) & 0xf];
 }
 for (p=32; p; ) {
 	p -= 4;
	if (sp >= ep) return sp;
	*sp++ = "0123456789abcdef"[(ns >> p) & 0xf];
 }
 *sp = 0;
 return sp;
}
#endif

char *time_string(char *sp, char *ep, RELTIME tim)
{
 const int_fast64_t msec = 1000;
 const int_fast64_t sec = 1000 * 1000;
 const int_fast64_t min = 60 * 1000 * 1000;
 const int_fast64_t hour = min * 60;
 const int_fast64_t day = hour * 24;
#ifdef INLINE_TIMEFUNC
 int_fast64_t t = tim;
#else
 int_fast64_t t = tim.t;
#endif
	// t is now in microseconds
 if (sp == ep) return sp;
 if (!t) {
 	return seprintf(sp, ep, "now");
 }
 if (t > 0) {
	*sp++ = '+';
 } else {
 	*sp++ = '-';
	t = -t;
 }
 if (t >= day) {
	unsigned d = t / day;
	sp = seprintf(sp, ep, "%dd ", d);
	t -= d * day;
 }
 if (t >= hour) {
	unsigned h = t / hour;
	sp = seprintf(sp, ep, "%dh ", h);
	t -= h * hour;
 }
 if (t >= min) {
	unsigned m = t / min;
	sp = seprintf(sp, ep, "%dm ", m);
	t -= m * min;
 }
 if (t >= sec) {
	unsigned s = t / sec;
	sp = seprintf(sp, ep, "%ds ", s);
	t -= s * sec;
 }
 if (t >= msec) {
	unsigned s = t / msec;
	sp = seprintf(sp, ep, "%dmS ", s);
	t -= s * msec;
 }
 if (t) sp = seprintf(sp, ep, "%duS ", (int) t);
 *--sp = 0;
 return sp;
}
