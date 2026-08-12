/* thread.hpp - pthread stuff with assertions */

#ifdef TiNGTYPE_LINUX
#include <pthread.h>
#endif
#ifdef TiNGTYPE_QNX
#include <pthread.h>
#endif
#ifdef TiNGTYPE_WINNT
#include "../../libutil/WINNT/pthread.h"
#endif

void mutex_init(pthread_mutex_t *mx);
void mutex_lock(pthread_mutex_t *mx);
void mutex_unlock(pthread_mutex_t *mx);
void mutex_destroy(pthread_mutex_t *mx);
