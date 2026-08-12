/* power.c - calculate average power (RMS) of signal */

#include <math.h>
#include "../../libutil/floio.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

static int power(FILE *inf)
{
 unsigned n = 0;
 double sum[64];
 unsigned i;
 for (i=0; i < arlen(sum); i++) sum[i] = 0;
 for (;;) {
	unsigned t;
	float f;
	if (getflo(inf, &f)) break;
	f *= f;
	sum[0] += f;
	for (i=0, t=n; t & 1; t>>=1, i++) {
		sum[i+1] += sum[i];
		sum[i] = 0;
	}
	n++;
 }
 for (i=1; i < arlen(sum); i++) sum[0] += sum[i];
 printf("%g\n", log(2 * sum[0] / n) / 2 / log(10) * 10);
 return 0;
}

#include "gen/power.args.i"

int main(int argc, char **argv)
{
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
 return power(inf);
}
