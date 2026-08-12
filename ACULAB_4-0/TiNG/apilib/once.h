#ifndef INCLUDED_ONCE_H
#define INCLUDED_ONCE_H

#ifdef TiNGTYPE_LINUX
#define TiNG_POSIX_ONCE
#endif

#ifdef TiNGTYPE_QNX
#define TiNG_POSIX_ONCE
#endif


#ifdef TiNG_POSIX_ONCE
#include <pthread.h>

typedef struct {
	enum {
		RUN_ONCE_NEW,
		RUN_ONCE_RUNNING,
		RUN_ONCE_DONE,
	} status;
	int (*fn)(void);
} RUN_ONCE;

#define RUN_ONCE_INITIALISER(fn) { RUN_ONCE_NEW, fn, }
	// must define this for architectures which need it (none yet)
#define membar_loadload() 0

#define run_once(op) \
	(\
	(op)->status == RUN_ONCE_DONE \
	? membar_loadload() \
	: run_once_fn((op)))
#endif


#ifdef TiNGTYPE_WINNT
#include "../libutil/WINNT/wind.h"

typedef struct {
	int completed;
	HANDLE mx;
	int (*fn)(void);
} RUN_ONCE;

#define RUN_ONCE_INITIALISER(fn) { 0, 0, fn, }

#define run_once(op) \
	((op)->completed \
	? 0 \
	: run_once_fn((op)))

#endif

	// used only by the macro run_once() - same semantics, but
	// function call overhead can be avoided nearly always
	// we have to let this be global, so avoid an unqualified name
#define run_once_fn aculab_TiNGcore_run_once_fn
int run_once_fn(RUN_ONCE *op);

	// we have to let this be global, so avoid an unqualified name
#define run_once_init aculab_TiNGcore_run_once_init
	// the run-once block for the global init function
extern RUN_ONCE run_once_init;

#endif
