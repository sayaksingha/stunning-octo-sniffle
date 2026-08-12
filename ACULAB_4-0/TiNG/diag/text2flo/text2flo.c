/* text2flo.c - convert stream from text to floating-point */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../libutil/floio.h"

static int text2flo(FILE *of, FILE *inf)
{
 for (;;) {
	char buff[80];
	char *ep;
	float f;
	if (!fgets(buff, sizeof(buff)-1, inf)) return 0;
	f = strtod(buff, &ep);
	for (;;) {
		if (*ep == '\n') break;
		if (!*ep) {
			fprintf(stderr, "Line too long: '%s'\n", buff);
			return 1;
		}
		if (*ep != ' ' && *ep != '\t') {
			fprintf(stderr, "Junk after number: '%s' after '%s'\n",
				ep, buff);
			return 1;
		}
		ep++;
	}
	if (putflo(of, f)) break;
 }
 return 0;
}

#include "gen/text2flo.args.i"

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
 return text2flo(outf, inf);
}
