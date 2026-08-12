/* fsk.c - Generate FSK modulated data */

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

static int fsk(FILE *of, FILE *inf, double markfreq, double spacefreq, double baudrate)
{
 const float sampperbaud = SAMPRATE / baudrate;
 COMPLX c = complx(0.0, 1.0);
 const COMPLX ds = f2w(spacefreq);
 const COMPLX dm = f2w(markfreq);
 float scount = 0.5;		// round to nearest
 int curchar = 0;
 for (;;) {
	putflo(of, c.re);
	if (--scount <= 0) {
		int igneol = 0;	// use to skip @....
		for (;;) {
			curchar = getc(inf);
			if (curchar == EOF) return 0;
			if (igneol) {
				if (curchar == '\n') igneol = 0;
			} else if (curchar == '0' || curchar == '1') break;
			else igneol = 1;
		}
		scount += sampperbaud;
	}
	if (curchar == '1') c = complx_mul(c, dm);
	else c = complx_mul(c, ds);
 }
}

#include "gen/fsktx.args.i"

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
 return fsk(outf, inf, arg.markfreq, arg.spacefreq, arg.baudrate);
}
