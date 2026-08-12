#include "../mutx.h"
#include "../TiNGcommon.h"

#include <pthread.h>

#ifdef DEBUG_MUTX

#define die_e(fn, e) do { fprintf(stderr, "@%s:%d %s() returned %d\n", __FILE__, __LINE__, fn, e); abort(); } while (0)

void mutx_init(MUTX *mx)
{
 pthread_mutexattr_t attr;
 int e;
 e = pthread_mutexattr_init(&attr);
 if (e) die_e("pthread_mutexattr_init", e);
 e = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK_NP);
 if (e) die_e("pthread_mutexattr_settype", e);
 e = pthread_mutex_init(&mx->mx, 0);
 if (e) die_e("pthread_mutex_init", e);
 e = pthread_mutexattr_destroy(&attr);
 if (e) die_e("pthread_mutexattr_destroy", e);
 mx->owner = 0;
}

void dbg_mutx_enter(MUTX *mx, char *file, unsigned line)
{
 int e = pthread_mutex_lock(&mx->mx);
 if (e) die_e("pthread_mutex_lock", e);
 if (mx->owner) die("recursive mutx use");
 mx->owner = pthread_self();
 mx->file = file;
 mx->line = line;
}

void mutx_owner(MUTX *mx)
{
 if (!pthread_equal(mx->owner, pthread_self())) die("mutx not owned");
}
 
void dbg_mutx_leave(MUTX *mx, char *file, unsigned line)
{
 int e;
 mutx_owner(mx);
 mx->owner = 0;
 mx->file = file;
 mx->line = line;
 e = pthread_mutex_unlock(&mx->mx);
 if (e) die_e("pthread_mutex_unlock", e);
}
 
void mutx_dtor(MUTX *mx)
{
 if (mx->owner) die("destroying locked mutx");
 pthread_mutex_destroy(&mx->mx);
}

#endif
