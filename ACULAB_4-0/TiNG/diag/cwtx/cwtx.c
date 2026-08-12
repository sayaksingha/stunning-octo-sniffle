/* cw.c - Generate CW modulated data */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../libutil/complx.h"
#include "../../libutil/floio.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

#define SAMPRATE 8000
#define F2W(x) ((x) * M_PI * 2 / SAMPRATE)

static COMPLX f2w(double freq)
{
 return complx_expi(F2W(freq));
}

static int cw(FILE *of, FILE *inf, double spacefreq, double baudrate)
{
 const float sampperbaud = SAMPRATE / baudrate;
 COMPLX c = complx(0.0, 1.0);
 const COMPLX ds = f2w(spacefreq);
 float scount = 0.5;		// round to nearest
 int curchar = 1;
 for (;;) {
	if (--scount <= 0) {
		curchar >>= 1;
		if (curchar <= 1) {
			curchar = getc(inf);
			if (curchar == EOF) break;
			curchar |= 0x100;
		}
		scount += sampperbaud;
	}
	if (curchar & 1) putflo(of, 0.0);
	else putflo(of, c.re);
	c = complx_mul(c, ds);
 }
 return 0;
}

#include "gen/cwtx.args.i"

int main(int argc, char **argv)
{
 FILE *outf;
 FILE *inf;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || (*argv && argv[1])) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE " infile\n", progname);
	return 1;
 }
 if (*argv) {
	inf = fopen(*argv, "rb");
	if (!inf) {
		perror("fopen");
		fprintf(stderr, "Cannot open file: %s\n", *argv);
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
 return cw(outf, inf, arg.spacefreq, arg.baudrate);
}
