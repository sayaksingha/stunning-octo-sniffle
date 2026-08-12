/* timer.c - timers */

#include <stdio.h>

#include "simevent.h"
#include "simtimer.h"
#include "timequeue_pq.h"
#include "timequeue_pq_basic.h"
#include "vseprintf.h"

#include <inttypes.h>
#include <stdlib.h>

struct timequeue_vtbl *pq_vtbl;

static ABSTIME simtime;
static const ABSTIME starttime;

int_fast64_t time_tai_uS(void)
{
 return -time_rel2longuS(time_until(time_zero()));
}

static void checktimer(void *arg)
{
 int t = pq_vtbl->next_timer_mS(arg);
 (void) t;
 // printf("checked timer, got %d\n", t);
}

static void starttimer(TIMEQUEUE_IMPL *tqi, TIMER *tim, void (*fn)(void *arg), void *arg, ABSTIME when)
{
 pq_vtbl->start(tqi, tim, fn, arg, when);
 if (time_sgn(time_until(when)) < 0) {
	char now[80], targ[80];
	simtime_string(now, now+sizeof(now), simtime);
	simtime_string(targ, targ+sizeof(targ), when);
	printf("%s: Internal error: timeout at %s!\n", now, targ);
	abort();
 }
// printf("starting timeout %p\n", tim);
 eventqueue(when, checktimer, tqi, "timeout");
}

static void stoptimer(TIMEQUEUE_IMPL *tqi, TIMER *tim)
{
// printf("cancel timeout %p\n", tim);
 pq_vtbl->cancel(tqi, tim);
}

ABSTIME time_when(RELTIME until)
{
 return time_add(simtime, until);
}

RELTIME time_until(ABSTIME when)
{
 return time_sub(when, simtime);
}

char *simtime_string(char *sp, char *ep, ABSTIME t)
{
 sp = seprintf(sp, ep, "@%" PRIu64 ": T", time_rel2i64uS(time_sub(t, starttime)));
 return time_string(sp, ep, time_sub(t, starttime));
}

void time_advance(ABSTIME when)
{
 char now[80];
 if (time_cmp(when, simtime) < 0) {
	char targ[80];
	simtime_string(now, now+sizeof(now), simtime);
	simtime_string(targ, targ+sizeof(targ), when);
	printf("%s: Internal error: time moves to %s!\n", now, targ);
	abort();
 }
 simtime = when;
 simtime_string(now, now+sizeof(now), simtime);
// printf("%s:\n", now);
}

static long next_timer_mS(TIMEQUEUE_IMPL *tqi)
{
 return pq_vtbl->next_timer_mS(tqi);
}

static struct timequeue_vtbl timequeue_sim_vtbl = {
	starttimer,
	stoptimer,
	next_timer_mS,
	0,
};

void timequeue_sim_init(TIMEQUEUE *tq, TIMEQUEUE_IMPL *tqi)
{
 timequeue_pq_init(tq, tqi);
 pq_vtbl = tq->vtbl;
 tq->vtbl = &timequeue_sim_vtbl;
}
