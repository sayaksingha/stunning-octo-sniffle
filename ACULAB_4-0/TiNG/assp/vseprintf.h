/* vseprintf.h - printf style formatting */

#ifndef INCLUDED_VSEPRINTF_H
#define INCLUDED_VSEPRINTF_H

#include <stdarg.h>

/* Redefine varargs function names to cope with Metrowerks compiler, which
 * has a nasty combination: no way to warn about using a function
 * which hasn't been declared; and a different calling convention for
 * varargs and non-varargs functions. This workaround ensures that if
 * you accidentally call a varargs function without including the right
 * header, it appears as an undefined function and won't link.
 */
#define vseprintf vseprintf_varargs
#define seprintf seprintf_varargs

char *vseprintf(char *sp, char *ep, const char *fmt, va_list ap);
char *seprintf(char *sp, char *ep, char *fmt, ...);

#endif
