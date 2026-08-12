/* alaw2flo.c - convert stream from A-law to floating-point */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../libutil/alaw.h"
#include "../../libutil/floio.h"

#define AMP_0dB (2853.029)      // amplitude of a cosine at 0dBm0

static int alaw2flo(FILE *of, FILE *inf)
{
 ALAW2LIN_TABLE a2lintab;
 init_alaw_decode(&a2lintab);
 for (;;) {
	int i = getc(inf);
	if (i == EOF) break;
	putflo(of, alaw_to_linear(i, &a2lintab) / AMP_0dB);
 }
 return 0;
}

#include "gen/alaw2flo.args.i"

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
 return alaw2flo(outf, inf);
}
