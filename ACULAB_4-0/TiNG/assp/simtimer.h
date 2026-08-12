/* simtimer.h - time in simulator */

#ifndef INCLUDED_SIMTIMER_H
#define INCLUDED_SIMTIMER_H

#include "timefn.h"
#include "timequeue.h"

//struct timequeue_impl {
//	int dummy;
//};

void time_advance(ABSTIME when);
char *simtime_string(char *sp, char *ep, ABSTIME t);
void timequeue_sim_init(TIMEQUEUE *tq, TIMEQUEUE_IMPL *tqi);

#endif
