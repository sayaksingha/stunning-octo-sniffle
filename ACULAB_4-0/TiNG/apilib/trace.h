#ifndef INCLUDED_TiNGTRACE_H
#define INCLUDED_TiNGTRACE_H

	// default trace level
#ifndef TiNG_TRACE
#define TiNG_TRACE 1
#endif

#if TiNG_TRACE == 0
	// no trace at all
#define TiNGtrace 0
#define err(e) (e)

#else
	// some trace - define trace functions

	// used only from the macro err() - report API generating an error
int aculab_TiNG_trace_error_log(int e, char *file, unsigned line);

#define err(e) aculab_TiNG_trace_error_log((e), __FILE__, __LINE__)

#if TiNG_TRACE > 1
	// remote trace
#define checktrace() checktrace_fn()
#else
	// only standard trace
#define checktrace()
#endif
#endif

#include "smcore.h"
#include "errcode.h"
#include <stdarg.h>

extern ACUAPI int (*TiNG_showtrace)(const char *fmt, va_list ap);
extern ACUAPI int TiNG_vsnprintf (char * s, size_t n, const char * format, va_list arg );

#define olog aculab_TiNG_olog
int olog(char *fmt, ...);

#if TiNG_TRACE > 0
	// trace available

extern ACUAPI unsigned TiNGtrace;
#define oerrno(txt) (TiNGtrace ? print_errno(errno, txt) : (void) 0)
#ifdef TiNGTYPE_LINUX
#define osockerr(txt) (TiNGtrace ? print_errno(errno, txt) : (void) 0)
#endif
#ifdef TiNGTYPE_QNX
#define osockerr(txt) (TiNGtrace ? print_errno(errno, txt) : (void) 0)
#endif
#ifdef TiNGTYPE_WINNT
#define osockerr(txt) (TiNGtrace ? print_winerr(txt, WSAGetLastError()) : (void) 0)
#endif
	// this is used like this: elog("message", ... params ...)
#define elog TiNGtrace && olog

#else
	// no trace

#define olog (void)
#define elog(s)
#define oerrno(s)
#define osockerr(s)

#endif

#endif
