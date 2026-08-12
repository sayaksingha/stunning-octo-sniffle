/* byteswap.c - swap bytes in word stream */

#include <stdio.h>
#include <stdlib.h>

static int byteswap(FILE *of, FILE *inf)
{
 for (;;) {
	int a, b, c, d;
	a = getc(inf);
	if (a == EOF) break;
	b = getc(inf);
	if (b == EOF) break;
	c = getc(inf);
	if (c == EOF) break;
	d = getc(inf);
	if (d == EOF) break;
	putc(d, of);
	putc(c, of);
	putc(b, of);
	putc(a, of);
 }
 return 0;
}

#include "gen/byteswap.args.i"

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
 return byteswap(outf, inf);
}
