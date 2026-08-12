#include "trace.h"
#include <stdarg.h>
#include <stdio.h>

#if TiNG_TRACE > 0
ACUAPI unsigned TiNGtrace = 0;

int olog(char *fmt, ...)
{
 va_list ap;
 int r;
 va_start(ap, fmt);
 r = (*TiNG_showtrace)(fmt, ap);
 va_end(ap);
 return r;
}
#endif

ACUAPI int (*TiNG_showtrace)(const char *fmt, va_list ap) = vprintf;

ACUAPI int TiNG_vsnprintf (char * s, size_t n, const char * format, va_list arg )
{
	return vsnprintf(s,n,format,arg);
}
