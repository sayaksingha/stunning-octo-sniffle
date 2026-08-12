/* syn2bit.c - convert stream recorded by SYNC to bits */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static int syn2bit(FILE *of, FILE *inf)
{
 for (;;) {
	int c = getc(inf);
	unsigned i;
	if (c == EOF) break;
	for (i=0; i<8; i++) {
		putc('0' + (c & 1), of);
		c >>= 1;
	}
 }
 return 0;
}

#include "gen/syn2bit.args.i"

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
 return syn2bit(outf, inf);
}
