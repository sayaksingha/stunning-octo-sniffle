/* int2text.c - convert stream from 32-bit integers to text */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static int int2text(FILE *of, FILE *inf)
{
 for (;;) {
	unsigned v = 0;
	int i = getc(inf);
	if (i == EOF) return 0;
	v += i;
	i = getc(inf);
	if (i != EOF) {
		v += i << 8;
		i = getc(inf);
	}
	if (i != EOF) {
		v += i << 16;
		i = getc(inf);
	}
	if (i != EOF) {
		v += i << 24;
	} else {
		fprintf(stderr, "Input has odd bytes left over\n");
		return 1;
	}
	fprintf(of, "%u\n", v);
 }
 return 0;
}

#include "gen/int2text.args.i"

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
 return int2text(outf, inf);
}
