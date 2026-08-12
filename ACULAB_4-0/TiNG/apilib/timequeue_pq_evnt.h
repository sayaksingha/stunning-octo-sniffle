/* timequeue_pq_evnt.h - timer queues using a priority queue and an event */

#ifndef DEFINED_TIMEQUEUE_PQ_EVNT_H
#define DEFINED_TIMEQUEUE_PQ_EVNT_H

#include "../assp/timequeue_pq.h"
#include "event.h"

// #define TIMEQUEUE_STATS

struct timequeue_impl {
	TIMER *root;
	EVNT newroot;
#ifdef TIMEQUEUE_STATS
	unsigned dbg_numtim;	// for debugging only
	unsigned dbg_maxnum;
	unsigned dbg_nins;
	unsigned dbg_ncancel;
	unsigned dbg_atend;
#endif
};

LOCALDEC void timequeue_pq_init(TIMEQUEUE *tq, TIMEQUEUE_IMPL *tqi, EVNT newroot);

#endif
