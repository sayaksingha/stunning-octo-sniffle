/* timefn.h - time functions */

#ifndef DEFINED_TIMEFN_H
#define DEFINED_TIMEFN_H

#include <stdint.h>

#ifdef INLINE_TIMEFUNC

typedef uint_fast64_t ABSTIME;

typedef int_fast64_t RELTIME;

#define time_sgn(t) ((t) ? (t) > 0 ? 1 : -1 : 0)
#define time_sub(lhs, rhs) ((lhs) - (rhs))

#define time_cmp(lhs, rhs) ((lhs) > (rhs) ? 1 : (lhs) < (rhs) ? -1 : 0)

#define time_longS2rel(s) ((s) * (int_fast64_t) 1000 * 1000)
#define time_longmS2rel(ms) ((ms) * (int_fast64_t) 1000)
#define time_longuS2rel(us) (us)
#define time_longnS2rel(ns) ((ns) / 1000)

#define time_rel2longS(t) ((t) / (int_fast64_t) 1000 / 1000)
#define time_rel2longmS(t) ((t) / (int_fast64_t) 1000)
#define time_rel2longuS(t) (t)
#define time_rel2longnS(t) ((t) * 1000)

#define time_i64S2rel(s) ((s) * (int_fast64_t) 1000 * 1000)
#define time_i64mS2rel(ms) ((ms) * (int_fast64_t) 1000)
#define time_i64uS2rel(us) (us)
#define time_i64nS2rel(ns) ((ns) / 1000)

#define time_rel2i64S(t) ((t) / (int_fast64_t) 1000 / 1000)
#define time_rel2i64mS(t) ((t) / 1000)
#define time_rel2i64uS(t) (t)
#define time_rel2i64nS(t) ((t) * (int_fast64_t) 1000)

#define time_add(lhs, rhs) ((lhs) + (rhs))

#define time_zero() 0
#define time_iszero(t) (!(t))

#define time_never() (~ (uint_fast64_t) 0)
#define time_isnever(t) ((t) == time_never())

#else

typedef struct {
	uint_fast64_t t;
} ABSTIME;

typedef struct {
	int_fast64_t t;
} RELTIME;

#define time_sgn time_sgn_fn
#define time_cmp time_cmp_fn
#define time_add time_add_fn
#define time_sub time_sub_fn
#define time_longS2rel time_longS2rel_fn
#define time_longmS2rel time_longmS2rel_fn
#define time_longuS2rel time_longuS2rel_fn
#define time_longnS2rel time_longnS2rel_fn
#define time_rel2longS time_rel2longS_fn
#define time_rel2longmS time_rel2longmS_fn
#define time_rel2longuS time_rel2longuS_fn
#define time_rel2longnS time_rel2longnS_fn
#define time_i64S2rel time_i64S2rel_fn
#define time_i64mS2rel time_i64mS2rel_fn
#define time_i64uS2rel time_i64uS2rel_fn
#define time_i64nS2rel time_i64nS2rel_fn
#define time_rel2i64S time_rel2i64S_fn
#define time_rel2i64mS time_rel2i64mS_fn
#define time_rel2i64uS time_rel2i64uS_fn
#define time_rel2i64nS time_rel2i64nS_fn
#define time_zero time_zero_fn
#define time_iszero time_iszero_fn
#define time_never time_never_fn
#define time_isnever time_isnever_fn

#endif

int time_sgn_fn(RELTIME t);
int time_cmp_fn(ABSTIME lhs, ABSTIME rhs);
ABSTIME time_add_fn(ABSTIME lhs, RELTIME rhs);
RELTIME time_sub_fn(ABSTIME lhs, ABSTIME rhs);
RELTIME time_longS2rel_fn(long ms);
RELTIME time_longmS2rel_fn(long ms);
RELTIME time_longuS2rel_fn(long ms);
RELTIME time_longnS2rel_fn(long ms);
long time_rel2longS_fn(RELTIME t);
long time_rel2longmS_fn(RELTIME t);
long time_rel2longuS_fn(RELTIME t);
long time_rel2longnS_fn(RELTIME t);
RELTIME time_i64S2rel_fn(int_fast64_t ms);
RELTIME time_i64mS2rel_fn(int_fast64_t ms);
RELTIME time_i64uS2rel_fn(int_fast64_t ms);
RELTIME time_i64nS2rel_fn(int_fast64_t ms);
int_fast64_t time_rel2i64S_fn(RELTIME t);
int_fast64_t time_rel2i64mS_fn(RELTIME t);
int_fast64_t time_rel2i64uS_fn(RELTIME t);
int_fast64_t time_rel2i64nS_fn(RELTIME t);
ABSTIME time_zero_fn(void);
int time_iszero_fn(ABSTIME t);
ABSTIME time_never_fn(void);
int time_isnever_fn(ABSTIME t);
	/* time since TAI64N representation start (1970) in uS */
int_fast64_t time_tai_uS(void);

#define time_now() time_when(time_longS2rel(0))

RELTIME time_until(ABSTIME t);
ABSTIME time_when(RELTIME t);
void time_init(void);
char *time_string_now(char *sp, char *ep);
char *time_string(char *sp, char *ep, RELTIME tim);

	/* Optimisation: to avoid repeatedly calling a system function
	 * to get the time during processing, we can simply use a
	 * global variable. Of course this variable must be updated
	 * when the time changes, so we require the user to call
	 * systick() whenever they have performed an operation which
	 * may have let the clock advance 'significantly'.
	 */
#ifdef USE_GLOBALTIME
void systick(void);
#endif

#endif
