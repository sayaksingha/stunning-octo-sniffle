/* timequeue_pq_basic.h - basic timer queues using a priority queue */

#ifndef DEFINED_TIMEQUEUE_PQ_BASIC_H
#define DEFINED_TIMEQUEUE_PQ_BASIC_H

#include "timequeue_pq.h"

struct timequeue_impl {
	TIMER *root;
		unsigned qlen;
		unsigned maxqlen;
#ifdef TIMEQUEUE_STATS
	struct {
		unsigned nstart;
		unsigned nrun;
		unsigned atend;
	} stats;
#endif
};

void timequeue_pq_init(TIMEQUEUE *tq, TIMEQUEUE_IMPL *tqi);

#endif
