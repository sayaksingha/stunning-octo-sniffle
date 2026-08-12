/* mulaw2flo.c - convert stream from mu-law to floating-point */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../libutil/floio.h"
#include "../libutil/mulaw.h"

#define AMP_0dB (5662.7797)      // amplitude of a cosine at 0dBm0

static int mulaw2flo(FILE *of, FILE *inf)
{
 MULAW2LIN_TABLE mu2lintab;
 init_mulaw_decode(&mu2lintab);
 for (;;) {
	int i = getc(inf);
	if (i == EOF) break;
	putflo(of, mulaw_to_linear(i, &mu2lintab) / AMP_0dB);
 }
 return 0;
}

#include "gen/mulaw2flo.args.i"

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
 return mulaw2flo(outf, inf);
}
