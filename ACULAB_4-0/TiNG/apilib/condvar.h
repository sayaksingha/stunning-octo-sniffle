#ifndef INCLUDED_CONDVAR_H
#define INCLUDED_CONDVAR_H

#include "mutx.h"

#ifdef _REENTRANT

#ifdef TiNGTYPE_LINUX
#define TiNG_POSIX_CONDVAR
#endif

#ifdef TiNGTYPE_QNX
#define TiNG_POSIX_CONDVAR
#endif


#ifdef TiNG_POSIX_CONDVAR

#include <pthread.h>

#ifdef DEBUG_MUTX

typedef struct {
	pthread_cond_t cv;
} CV;

void cv_init(CV *cv);
int cv_wait(CV *cv, MUTX *mx);
void cv_broadcast(CV *cv);
void cv_dtor(CV *cv);
#else

typedef pthread_cond_t CV;

#define cv_init(cv) pthread_cond_init(cv, 0)
#define cv_wait(cv, mx) pthread_cond_wait(cv, mx)
#define cv_broadcast(cv) pthread_cond_broadcast(cv)
#define cv_dtor(cv) pthread_cond_destroy(cv)

#endif

#endif

#ifdef TiNGTYPE_WINNT

typedef struct {
	HANDLE event;
} CV;

LOCALDEC void cv_init(CV *cond);
LOCALDEC int cv_wait(CV *cond, MUTX *mx);
LOCALDEC void cv_broadcast(CV *cond);
LOCALDEC void cv_dtor(CV *cond);

#endif

#else

typedef int CV;	/* dummy definition */

#define cv_init(cv)
#define cv_wait(cv)
#define cv_broadcast(cv, mx)
#define cv_dtor(cv)

#endif

#endif
