/* bitunpack.c - unpack bits (LSB first) into characters '0' and '1' */

#include <stdio.h>
#ifdef TiNGTYPE_WINNT
#include <fcntl.h>
#include <io.h>
#endif

static int bitunpack(FILE *of, FILE *inf)
{
#ifdef TiNGTYPE_WINNT
 _setmode(fileno(inf), _O_BINARY);
#endif
 for (;;) {
 	int c = getc(inf);
 	int i;
 	if (c == EOF) return 0;
 	for (i=0; i<8; c>>=1, i++) putc(c & 1 ? '1' : '0', of);
 }
}

#include "gen/bitunpack.args.i"

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
 return bitunpack(outf, inf);
}
