/* bitrev32.c - swap bits in 32-bit words*/

#include <stdio.h>
#include <stdlib.h>

static int brt[256];

static void brt_init(void)
{
 unsigned u, m = 1, rm = 128;
 brt[0] = 0;
 for (u=1; rm; ) {
	brt[u] = brt[u - m] | rm;
	if (++u == m << 1) m <<= 1, rm >>= 1;
 }
}

static int bitrev32(FILE *of, FILE *inf)
{
 brt_init();
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
	putc(brt[d], of);
	putc(brt[c], of);
	putc(brt[b], of);
	putc(brt[a], of);
 }
 return 0;
}

#include "gen/bitrev32.args.i"

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
 return bitrev32(outf, inf);
}
