/* flo2text.c - convert stream from floating-point to text */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../libutil/floio.h"

static int flo2text(FILE *of, FILE *inf)
{
 for (;;) {
	float f;
	if (getflo(inf, &f)) break;
	fprintf(of, "%9f\n", f);
 }
 return 0;
}

#include "gen/flo2text.args.i"

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
 return flo2text(outf, inf);
}
