/* timequeue.h - timer queues */

#ifndef DEFINED_TIMEQUEUE_LIN_H
#define DEFINED_TIMEQUEUE_LIN_H

#include "timequeue.h"

typedef struct timer {
	struct timer *next;
	ABSTIME when;
	void (*fn)(void *arg);
	void *arg;
} TIMER;

struct timequeue_impl {
	TIMER *first;
};

void timequeue_lin_init(TIMEQUEUE *tq, TIMEQUEUE_IMPL *tqi);

#endif
