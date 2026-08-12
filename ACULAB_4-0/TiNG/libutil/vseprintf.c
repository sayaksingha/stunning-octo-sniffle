/* vseprintf.c - printf style formatting */

#include "vseprintf.h"

#ifdef TiNGTYPE_WINNT
#ifdef _MSC_VER
typedef __int64 LONGLONG;
typedef unsigned __int64 ULONGLONG;
#endif

#ifdef __BORLANDC__
typedef __int64 LONGLONG;
typedef unsigned __int64 ULONGLONG;
#endif

#ifdef __DMC__
typedef long long LONGLONG;
typedef unsigned long long ULONGLONG;
#endif

#ifdef __GNUC__
typedef long long LONGLONG;
typedef unsigned long long ULONGLONG;
#endif

/* disable stupid warnings about conversions to smaller types */

#pragma warning(disable:4761)
#pragma warning(disable:4244)
#pragma warning(disable:4305)

#endif

#ifdef TiNGTYPE_LINUX
typedef long long LONGLONG;
typedef unsigned long long ULONGLONG;
#endif

#ifdef TiNGTYPE_QNX
#include <stdint.h>
typedef int64_t LONGLONG;
typedef uint64_t ULONGLONG;
#endif



static char *cvthex(char *bp, char *ep, ULONGLONG val)
{
 *--ep = 0;
 while (val && bp < ep) {
 	*--ep = "0123456789abcdef"[val & 0xf];
 	val >>= 4;
 }
 if (!*ep && bp < ep) *--ep = '0';
 return ep;
}

static char *cvtdec(char *bp, char *ep, LONGLONG val)
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
 if (ismin && bp < ep) *--ep = '-';
 return ep;
}

char *vseprintf(char *sp, char *ep, char *fmt, va_list ap)
{
 ep--;		// reserve space for terminating '\0'
 while (*fmt) {
 	if (*fmt != '%') {
 		if (sp < ep) *sp++ = *fmt;
 		fmt++;
	} else {
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
		};
		fmt++;
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
					? va_arg(ap, LONGLONG)
					: flags & 1<<FLAG_LONG
						? va_arg(ap, long)
						: va_arg(ap, int));
			break;
		case 'x': res = cvthex(buf, buf+sizeof(buf),
					flags & 1<<FLAG_LONGLONG
					? va_arg(ap, LONGLONG)
					: flags & 1<<FLAG_LONG
						? va_arg(ap, unsigned long)
						: va_arg(ap, unsigned int));
			break;
		case 'p': res = cvthex(buf, buf+sizeof(buf),
					(ULONGLONG)
#ifdef TiNGTYPE_WINNT
	// This should be unnecessary, but without it CL sign-extends
	// the pointer. If pointers are bigger than unsigned, this
	// is also wrong and will truncate the value.
					(unsigned)
#endif
					va_arg(ap, void *));
			break;
		case '%':
			res = "%";
			break;
		default:
			res = "<unknown format>";
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
