/* worker.hpp - starting and stopping worker threads */

#ifndef INCLUDED_WORKER_HPP
#define INCLUDED_WORKER_HPP

#ifdef TiNGTYPE_LINUX
#include <pthread.h>
#endif
#ifdef TiNGTYPE_QNX
#include <pthread.h>
#endif
#ifdef TiNGTYPE_WINNT
#include "../../libutil/WINNT/pthread.h"
#endif

#include "../../class/eventset.hpp"

	// start the worker thread
int startworker(pthread_t *tidp, EventSet *evs);

int stopworker(pthread_t tid, EventSet *evs);

#endif
