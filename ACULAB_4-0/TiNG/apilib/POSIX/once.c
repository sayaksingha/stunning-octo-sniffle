#include "../once.h"
#include "../trace.h"

#include <pthread.h>
#include <string.h>
#include <stdlib.h>

LOCALDEF pthread_mutex_t once_mutex = PTHREAD_MUTEX_INITIALIZER;
LOCALDEF pthread_cond_t once_done = PTHREAD_COND_INITIALIZER;
/*
 * This gets called if we might have to run the function. Note that, since
 * no mutex protects the initial access to the RUN_ONCE struct, several
 * threads may get in here together.
 */
int run_once_fn(RUN_ONCE *op)
{
 int sts;
 int e = pthread_mutex_lock(&once_mutex);
 if (e) {
	olog("run_once: cannot lock once mutex: %s\n", strerror(e));
	abort();
 }
 while (op->status == RUN_ONCE_RUNNING) {
 	e = pthread_cond_wait(&once_done, &once_mutex);
	if (e) {
		olog("run_once: cannot wait on cond: %s\n", strerror(e));
		abort();
	}
 }
 if (op->status == RUN_ONCE_NEW) {
	op->status = RUN_ONCE_RUNNING;
	e = pthread_mutex_unlock(&once_mutex);
	if (e) {
		olog("run_once: cannot unlock once mutex: %s\n", strerror(e));
		abort();
	}
	sts = (*op->fn)();
	e = pthread_mutex_lock(&once_mutex);
	if (e) {
		olog("run_once: cannot re-lock once mutex: %s\n", strerror(e));
		abort();
	}
	if (sts) op->status = RUN_ONCE_NEW;
	else op->status = RUN_ONCE_DONE;
	e = pthread_cond_broadcast(&once_done);
	if (e) {
		olog("run_once: cannot broadcast on done: %s\n", strerror(e));
		abort();
	}
 } else sts = 0;
 e = pthread_mutex_unlock(&once_mutex);
 if (e) {
	olog("run_once: cannot unlock once mutex: %s\n", strerror(e));
	abort();
 }
 return sts;
}
