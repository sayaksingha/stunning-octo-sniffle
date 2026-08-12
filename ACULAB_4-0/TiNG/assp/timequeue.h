/* timequeue.h - timer queues */

#ifndef DEFINED_TIMEQUEUE_H
#define DEFINED_TIMEQUEUE_H

#include "timefn.h"

typedef struct timequeue_impl TIMEQUEUE_IMPL;
typedef struct timequeue_timer TIMER;

struct timequeue_vtbl {
	void (*start)(TIMEQUEUE_IMPL *tq, TIMER *tim, void (*fn)(void *p), void *p, ABSTIME when);
	void (*cancel)(TIMEQUEUE_IMPL *tq, TIMER *tim);
	long (*next_timer_mS)(TIMEQUEUE_IMPL *tq);
	void (*dtor)(TIMEQUEUE_IMPL *tq);
};

typedef struct {
	struct timequeue_vtbl *vtbl;
	TIMEQUEUE_IMPL *tqi;
} TIMEQUEUE;

#endif
