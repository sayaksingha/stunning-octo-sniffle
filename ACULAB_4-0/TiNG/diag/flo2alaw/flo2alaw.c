/* flo2alaw.c - convert stream from floating-point to A-law */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../libutil/alaw.h"
#include "../../libutil/floio.h"

static int flo2alaw(FILE *of, FILE *inf)
{
 LIN2ALAW_TABLE lin2atab;
 init_alaw_encode(&lin2atab);
 for (;;) {
	float f;
	if (getflo(inf, &f)) break;
	putc(linear_to_alaw(f * 2853.5, &lin2atab), of);
 }
 return 0;
}

#include "gen/flo2alaw.args.i"

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
 return flo2alaw(outf, inf);
}
