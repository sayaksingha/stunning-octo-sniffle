/* vseprintf.h - printf style formatting */

#ifndef INCLUDED_VSEPRINTF_H
#define INCLUDED_VSEPRINTF_H

#include <stdarg.h>

char *vseprintf(char *sp, char *ep, char *fmt, va_list ap);
char *seprintf(char *sp, char *ep, char *fmt, ...);

#endif
