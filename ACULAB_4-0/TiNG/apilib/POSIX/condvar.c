#include "../condvar.h"
#include "../TiNGcommon.h"
#include <errno.h>

#ifdef DEBUG_MUTX

void cv_init(CV *cv)
{
 pthread_cond_init(&cv->cv, 0);
}

#define die_e(fn, e) do { olog("@%s:%d %s() returned %d\n", __FILE__, __LINE__, fn, e); abort(); } while (0)

int cv_wait(CV *cv, MUTX *mx)
{
 int sts;
 mutx_owner(mx);
 mx->owner = 0;
#ifdef DEBUG_CONDVAR
{
 struct timespec ts;
 struct timeval now;
 if (gettimeofday(&now, 0)) die_e("gettimeofday", errno);
 ts.tv_sec = now.tv_sec + 20;
 ts.tv_nsec = now.tv_usec * 1000;
 sts = pthread_cond_timedwait(&cv->cv, &mx->mx, &ts);
 }
 if (sts == ETIMEDOUT) {
 	printf("cv timeout on %p\n", cv);
 	sts = 0;
 }
 if (sts) die_e("pthread_cond_timedwait", sts);
#else
 sts = pthread_cond_wait(&cv->cv, &mx->mx);
 if (sts) die_e("pthread_cond_wait", sts);
#endif
 if (mx->owner) die("recursive mutx use");
 mx->owner = pthread_self();
 return sts;
}

void cv_broadcast(CV *cv)
{
 pthread_cond_broadcast(&cv->cv);
}

void cv_dtor(CV *cv)
{
 pthread_cond_destroy(&cv->cv);
}

#endif
