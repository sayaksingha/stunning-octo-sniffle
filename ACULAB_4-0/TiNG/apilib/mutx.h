/* mutx.h - mutex declarations */

#ifndef INCLUDED_MUTX_H
#define INCLUDED_MUTX_H

#ifdef _REENTRANT

//#define DEBUG_MUTX

#ifdef TiNGTYPE_LINUX
#define TiNG_MUTX_POSIX
#endif

#ifdef TiNGTYPE_QNX
#define TiNG_MUTX_POSIX
#endif


#ifdef TiNG_MUTX_POSIX
#include <pthread.h>

#ifdef DEBUG_MUTX
typedef struct {
	pthread_mutex_t mx;
	pthread_t owner;
	char *file;
	unsigned line;
} MUTX;

void mutx_init(MUTX *mx);
void dbg_mutx_enter(MUTX *mx, char *file, unsigned line);
void mutx_owner(MUTX *mx);
void dbg_mutx_leave(MUTX *mx, char *file, unsigned line);
void mutx_dtor(MUTX *mx);
#define mutx_enter(mx) dbg_mutx_enter((mx), __FILE__, __LINE__)
#define mutx_leave(mx) dbg_mutx_leave((mx), __FILE__, __LINE__)
#else

typedef pthread_mutex_t MUTX;

#define mutx_init(mx) pthread_mutex_init((mx), 0)
#define mutx_enter(mx) pthread_mutex_lock((mx))
#define mutx_owner(mx)
#define mutx_leave(mx) pthread_mutex_unlock((mx))
#define mutx_dtor(mx) pthread_mutex_destroy((mx))

#endif

#endif

#ifdef TiNGTYPE_WINNT
#include "WINNT/mutx.h"
#endif

#else

typedef int MUTX;	/* dummy definition */

#define mutx_init(mx)
#define mutx_enter(mx)
#define mutx_leave(mx)
#define mutx_dtor(mx)

#endif
#endif
