/* vseprintf.c - printf style formatting */

#include "vseprintf.h"

#ifdef USE_FLOAT_KLUDGE
#include <stdio.h>
#include <string.h>
#endif
#include <stdint.h>

#ifdef _MSC_VER
#ifndef UINT_FAST64_MAX
typedef __int64 int_fast64_t;
typedef unsigned __int64 uint_fast64_t;
#define UINT_FAST64_MAX ~0i64
#endif

/* disable stupid warnings about conversions to smaller types */

#pragma warning(disable:4761)
#pragma warning(disable:4244)
#pragma warning(disable:4305)

#endif

#ifndef UINT_FAST64_MAX
typedef long long int_fast64_t;
typedef unsigned long long uint_fast64_t;
#endif

#ifndef UINTPTR_MAX
#ifndef _UINTPTR_T_DEFINED
#define _UINTPTR_T_DEFINED
typedef unsigned long uintptr_t;
#endif
#endif

static char *cvthex(char *bp, char *ep, uint_fast64_t val, int plus)
{
 *--ep = 0;
 while (val && bp < ep) {
 	*--ep = "0123456789abcdef"[val & 0xf];
 	val >>= 4;
 }
 if (!*ep && bp < ep) *--ep = '0';
 if (plus && bp < ep) *--ep = '+';
 return ep;
}

static char *cvtdec(char *bp, char *ep, int_fast64_t val, int plus)
{
 int ismin = val < 0;
 *--ep = 0;
 if (!ismin) val = -val;
 while (val && bp < ep) {
 	int c = val % 10;
 	if (-1 % 10 == -1) c = -c;
 	else if (-1 % 10 == 9) c = c ? 10-c : c;
 	else if (-1 % 10 == 1) c = c;
 	*--ep = c + '0';
 	val += c;
 	val /= 10;
 }
 if (!*ep && bp < ep) *--ep = '0';
 if (bp < ep) {
	if (ismin) *--ep = '-';
	else if (plus) *--ep = '+';
 }
 return ep;
}

static char *cvtudec(char *bp, char *ep, uint_fast64_t val, int plus)
{
 *--ep = 0;
 while (val && bp < ep) {
 	int c = val % 10;
 	*--ep = c + '0';
 	val -= c;
 	val /= 10;
 }
 if (!*ep && bp < ep) *--ep = '0';
 if (plus && bp < ep) *--ep = '+';
 return ep;
}

char *vseprintf(char *sp, char *ep, const char *fmt, va_list ap)
{
 ep--;		// reserve space for terminating '\0'
 while (*fmt) {
 	if (*fmt != '%') {
 		if (sp < ep) *sp++ = *fmt;
 		fmt++;
	} else {
#ifdef USE_FLOAT_KLUDGE
		const char *const startfmt = fmt;
		char floatbuf[80];
#endif
		char buf[28];
		char *res;
		unsigned flags = 0;
		unsigned minwid = 0, maxwid = ~0u;
		unsigned prec = 0;
		unsigned i;
		enum {
			FLAG_ZERO,
			FLAG_LONG,
			FLAG_LONGLONG,
			FLAG_HADPREC,
			FLAG_PLUS,
		};
		fmt++;
		if (*fmt == '+') flags |= 1 << FLAG_PLUS, fmt++;
		if (*fmt == '0') flags |= 1 << FLAG_ZERO, fmt++;
		while (*fmt >= '0' && *fmt <= '9') {
			minwid = minwid * 10 + *fmt++ - '0';
		}
		if (*fmt == '.') {
			fmt++;
			flags |= 1<<FLAG_HADPREC;
			while (*fmt >= '0' && *fmt <= '9') {
				prec = prec * 10 + *fmt++ - '0';
			}
		}
		if (*fmt == 'l') flags |= 1<<FLAG_LONG, fmt++;
		if (*fmt == 'l') flags |= 1<<FLAG_LONGLONG, fmt++;
		switch (*fmt++) {
#ifdef USE_FLOAT_KLUDGE
			// ugly hack:
			// just get sprintf() to do the work for us
			// and hope that the buffers are big enough
		case 'g':
		case 'e':
		case 'f':
			memcpy(buf, startfmt, fmt - startfmt);
			buf[fmt - startfmt] = '\0';
			sprintf(floatbuf, buf, va_arg(ap, double));
			res = floatbuf;
			if (flags & 1 << FLAG_PLUS && *res != '-' && sp < ep) *sp++ = '+';
			while (*res && sp < ep) *sp++ = *res++;
			continue;
#endif

		case 'c':
			buf[0] = va_arg(ap, int);
			buf[1] = 0;
			res = buf;
			break;
		case 's': res = va_arg(ap, char *);
			if (!res) res = "<null>";
			if (flags & 1<<FLAG_HADPREC) maxwid = prec;
			prec = 0;
			break;
		case 'd': res = cvtdec(buf, buf+sizeof(buf),
					flags & 1<<FLAG_LONGLONG
					? va_arg(ap, int_fast64_t)
					: flags & 1<<FLAG_LONG
						? va_arg(ap, long)
						: va_arg(ap, int),
					flags & 1 << FLAG_PLUS);
			break;
		case 'u': res = cvtudec(buf, buf+sizeof(buf),
					flags & 1<<FLAG_LONGLONG
					? va_arg(ap, uint_fast64_t)
					: flags & 1<<FLAG_LONG
						? va_arg(ap, unsigned long)
						: va_arg(ap, unsigned int),
					flags & 1 << FLAG_PLUS);
			break;
		case 'x': res = cvthex(buf, buf+sizeof(buf),
					flags & 1<<FLAG_LONGLONG
					? va_arg(ap, uint_fast64_t)
					: flags & 1<<FLAG_LONG
						? va_arg(ap, unsigned long)
						: va_arg(ap, unsigned int),
					flags & 1 << FLAG_PLUS);
			break;
		case 'p': res = cvthex(buf, buf+sizeof(buf),
					(uint_fast64_t)
#ifdef _MSC_VER
	// This should be unnecessary, but without it CL sign-extends
	// the pointer. If pointers are bigger than unsigned, this
	// is also wrong and will truncate the value.
					(unsigned)
#endif
					(uintptr_t)
					va_arg(ap, void *),
					flags & 1 << FLAG_PLUS);
			break;
		case '%':
			res = "%";
			break;
		default:
			res = "<unknown format>";
				// undo processing for this item
			maxwid = ~0u;
			prec = 0;
			minwid = 0;
			flags = 0;
#ifdef USE_FLOAT_KLUDGE
				// happen to have saved start of format
			fmt = startfmt + 1;
#else
				// just print the format code itself
			fmt--;
#endif
			break;
		}
		for (i=0; i < maxwid && res[i]; i++);
			// i holds number of characters in field
		if (prec && *res == '-') {
			if (sp < ep) *sp++ = *res++;
			i--;
			prec--;
		}
		for (; i < prec; prec--) {
			if (sp < ep) *sp++ = '0';
			if (minwid) minwid--;
		}
		for (; i < minwid; minwid--) {
			if (sp < ep) *sp++ = flags & 1<<FLAG_ZERO ? '0' : ' ';
		}
		for (; i; i--) {
			if (sp < ep) *sp++ = *res++;
			if (minwid) minwid--;
		}
	}
 }
 *sp = 0;
 return sp;
}

char *seprintf(char *sp, char *ep, char *fmt, ...)
{
 char *rp;
 va_list ap;
 va_start(ap, fmt);
 rp = vseprintf(sp, ep, fmt, ap);
 va_end(ap);
 return rp;
}
