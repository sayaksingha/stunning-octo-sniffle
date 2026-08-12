/* gain.c - apply fixed gain to floating-point stream */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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

static int gain(FILE *of, FILE *inf, double gaindB)
{
 double gainfac = pow(10.0, gaindB/20.0);
 for (;;) {
	float samp;
	if (getflo(inf, &samp)) break;
	putflo(of, samp * gainfac);
 }
 return 0;
}

#include "gen/gain.args.i"

int main(int argc, char **argv)
{
 FILE *outf;
 FILE *inf;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || *argv) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
	return 1;
 }
 if (arg.infile) {
	inf = fopen(arg.infile, "rb");
	if (!inf) {
		perror("fopen");
		fprintf(stderr, "Cannot open file: %s\n", arg.infile);
		return 1;
 	}
 } else inf = stdin;
 if (arg.outfile) {
	outf = fopen(arg.outfile, "wb");
	if (!outf) {
		perror("fopen");
		fprintf(stderr, "Cannot create file: %s\n", arg.outfile);
		return 1;
	}
 } else outf = stdout;
 return gain(outf, inf, arg.gain);
}
