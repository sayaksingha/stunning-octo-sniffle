/* thread.hpp - pthread stuff with assertions */

#include "thread.hpp"
#include <iostream>
#include <stdlib.h>

void mutex_init(pthread_mutex_t *mx)
{
 int e = pthread_mutex_init(mx, 0);
 if (e) {
	std::cerr << "pthread_mutex_init() failed: " << e << std::endl;
	abort();
 }
}

void mutex_lock(pthread_mutex_t *mx)
{
 int e = pthread_mutex_lock(mx);
 if (e) {
	std::cerr << "pthread_mutex_lock() failed: " << e << std::endl;
	abort();
 }
}

void mutex_unlock(pthread_mutex_t *mx)
{
 int e = pthread_mutex_unlock(mx);
 if (e) {
	std::cerr << "pthread_mutex_unlock() failed: " << e << std::endl;
	abort();
 }
}

void mutex_destroy(pthread_mutex_t *mx)
{
 int e = pthread_mutex_destroy(mx);
 if (e) {
	std::cerr << "pthread_mutex_destroy() failed: " << e << std::endl;
	abort();
 }
}
