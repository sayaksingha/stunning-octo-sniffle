/* timequeue_pq.h - timer queues using a priority queue */

#ifndef DEFINED_TIMEQUEUE_PQ_H
#define DEFINED_TIMEQUEUE_PQ_H

#include "timequeue.h"

struct timequeue_timer {
	struct timequeue_timer *left, *right, *up;
	int dist;
	ABSTIME when;
	void (*fn)(void *arg);
	void *arg;
};

#endif
