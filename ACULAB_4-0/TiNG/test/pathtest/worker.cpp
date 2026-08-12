/* worker.cpp - starting and stopping worker threads */

#include "worker.hpp"
#include <string.h>

	// the worker thread is this function
static void *worker_thread(void *p)
{
 EventSet *evs = (EventSet *) p;
#ifdef TiNGTYPE_WINNT
 if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS)) {
	std::cerr << error(error(GetLastError()), "SetPriorityClass");
	return (void *) 1;
 }
#endif
 error err(evs->wait());
 if (err) {
	error(err, "worker thread failed").print(stderr);
	return (void *) 1;
 }
 return 0;
}

	// start the worker thread
int startworker(pthread_t *tidp, EventSet *evs)
{
 int e = pthread_create(tidp, 0, worker_thread, evs);
 if (e) {
 	std::cerr << "pthread_create() failed: "
 		<< e << ": " << strerror(e) << "\n";
 	std::cerr << "cannot start worker thread'n";
 	return 1;
 }
 return 0;
}

int stopworker(pthread_t tid, EventSet *evs)
{
 error err(evs->stop());
 if (err) {
 	std::cerr << "Cannot stop worker thread: " << err << "\n";
 	return 1;
 }
 void *sts;
 int e = pthread_join(tid, &sts);
 if (e) {
 	std::cerr << "pthread_join() failed: "
 		<< e << ": " << strerror(e) << "\n";
 	std::cerr << "cannot join with worker thread\n";
 	return 1;
 }
 return 0;
}
