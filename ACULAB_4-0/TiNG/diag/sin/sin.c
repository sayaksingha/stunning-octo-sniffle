/* gensin.c - Generate sine wave */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../libutil/complx.h"
#include "../../libutil/floio.h"

#ifdef _MSC_VER

/* disable stupid warnings about doubles being converted to floats.
 */

#pragma warning(disable:4244)
#pragma warning(disable:4305)
/* and about argument conversions (does anyone still use a
   non-prototype compiler?) */
#pragma warning(disable:4761)

#endif

#define arlen(x) (sizeof(x)/sizeof(*(x)))

#define SAMPRATE 8000
#define F2W(x) ((x) * M_PI * 2 / SAMPRATE)

#ifndef M_PI
static const double M_PI = 3.14159265358979323846;
#endif

static COMPLX f2w(double freq)
{
 return complx_expi(F2W(freq));
}

static int gensin(FILE *of, double spacefreq, unsigned len)
{
 COMPLX c = complx(0.0, 1.0);
 const COMPLX ds = f2w(spacefreq);
 for (;len--;) {
	putflo(of, c.re);
	c = complx_mul(c, ds);
 }
 return 0;
}

#include "gen/sin.args.i"

int main(int argc, char **argv)
{
 FILE *outf;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || *argv) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
	return 1;
 }
 if (arg.outfile) {
	outf = fopen(arg.outfile, "wb");
	if (!outf) {
		perror("fopen");
		fprintf(stderr, "Cannot create file: %s\n", arg.outfile);
		return 1;
	}
 } else outf = stdout;
 return gensin(outf, arg.freq, arg.length);
}
