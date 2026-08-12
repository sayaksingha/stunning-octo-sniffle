/* floio.c - floating point I/O */

#include <math.h>

#include "floio.h"

#ifdef _MSC_VER

/* disable stupid warnings about doubles being converted to floats.
 */

#pragma warning(disable:4244)
#pragma warning(disable:4305)
/* and about argument conversions (does anyone still use a
   non-prototype compiler?) */
#pragma warning(disable:4761)

#endif

// read floating point representation:
//	byte 0	   byte 1     byte 2	 byte 3
//	seee eeee  emmm mmmm  mmmm mmmm  mmmm mmmm
int getflo(FILE *inf, float *f)
{
 long l;
 int i;
 if ((i = getc(inf)) == EOF) return 1;
 l = i;
 if ((i = getc(inf)) == EOF) return 1;
 l |= i << 8;
 if ((i = getc(inf)) == EOF) return 1;
 l |= i << 16;
 if ((i = getc(inf)) == EOF) return 1;
 if (!l && !i) {
 	*f = 0;
 } else {
	i = (i << 1) | (l >> 23);
	l |= 0x800000;		// restore leading '1'
	*f = ldexp(((float) l) / 0x01000000 , (i & 0xff)-126);
	if (i & 0x100) *f = -*f;		// sign bit
 }
 return 0;
}

// write floating point representation:
//	byte 0	   byte 1     byte 2	 byte 3
//	seee eeee  emmm mmmm  mmmm mmmm  mmmm mmmm
int putflo(FILE *of, float f)
{
 unsigned long l = 0;
 int e;
 if (f < 0) {		// sign bit
 	f = -f;
 	l = 0x80000000;
 }
 // 0.5 <= frexp < 1
 // 0x00800000 <= frexp * 0x01000000 < 0x01000000
 if (!f) l = 0;
 else {
	l |= (unsigned long) (frexp(f, &e) * 0x01000000) & 0x007fffff;
	if (e < -126) e = -126;
	else if (e > 126) e = 126;
	l |= (e+126) << 23;
 }
 putc(l & 0xff, of);			// byte 0
 putc((l >> 8) & 0xff, of);		// byte 1
 putc((l >> 16) & 0xff, of);		// byte 2
 putc((l >> 24), of);			// byte 3
 return 0;
}

