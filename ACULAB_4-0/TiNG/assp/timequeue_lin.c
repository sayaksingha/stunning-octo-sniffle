#include <stdlib.h>
#include <stdio.h>
#include "log.h"
#include "timequeue_lin.h"

typedef struct timequeue_impl TQI;

static void starttimer(TQI *tqi, void (*fn)(void *arg), void *arg, ABSTIME when)
{
 TIMER *tim = malloc(sizeof(*tim));
 TIMER **timp;
 if (!tim) {
 	perror("malloc failed");
	Log(0, "Cannot allocate %d bytes for timer\n", sizeof(*tim));
	abort();
 }
 tim->when = when;
 tim->fn = fn;
 tim->arg = arg;
 for (timp = &tqi->first; ; ) {
 	TIMER *t = *timp;
	if (!t || time_cmp(when, t->when) < 0) {
		tim->next = t;
		*timp = tim;
		break;
	}
	timp = &t->next;
 }
}

static long next_timer_mS(TQI *tqi)
{
 for (;;) {
	TIMER *t = tqi->first;
	long nx;
	if (!t) return -1;
	nx = time_rel2longmS(time_until(t->when)) + 1;
	if (nx > 0) return nx;
	tqi->first = t->next;
	t->fn(t->arg);
	free(t);
 }
}

static struct timequeue_vtbl timequeue_lin_vtbl = {
	starttimer,
	next_timer_mS,
};

void timequeue_lin_init(TIMEQUEUE *tq, TIMEQUEUE_IMPL *tqi)
{
 tq->vtbl = &timequeue_lin_vtbl;
 tq->tqi = tqi;
 tqi->first = 0;
}
