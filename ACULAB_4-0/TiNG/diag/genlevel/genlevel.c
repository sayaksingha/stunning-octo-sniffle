/* genlevel.c - Generate a test tone at known levels*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../libutil/alaw.h"
#include "../../libutil/complx.h"
#include "../../libutil/floio.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

#define SAMPRATE 8000
#define F2W(x) ((x) * M_PI * 2 / SAMPRATE)

#ifndef LOG10
#define LOG10(X) (log(X)/2.302585092994)
#define EXP10(X) exp( (X)*2.302585092994 )
#endif

static COMPLX f2w(double freq)
{
 return complx_expi(F2W(freq));
}

static int genlevel(FILE *of, double frequency)
{
 COMPLX c = complx(0.0, 1.0);
 const COMPLX df = f2w(frequency);
 unsigned nsamp;
 	// sample 0 is 0dB, sample 100 is -1dB, sample 100*100 is -100dB
 for (nsamp = 0; nsamp < 100*100; nsamp++) {
	double temp = c.im * EXP10(nsamp/-100.0/20.0);
	putflo(of, temp);
	c = complx_mul(c, df);
 }
 return 0;
}

#include "gen/genlevel.args.i"

int main(int argc, char **argv)
{
 FILE *f;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || *argv) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
	return 1;
 }
 if (arg.filename) {
	f = fopen(arg.filename, "wb");
	if (!f) {
		perror("fopen");
		fprintf(stderr, "Cannot create file: %s\n", arg.filename);
		return 1;
	}
 } else f = stdout;
 return genlevel(f, arg.frequency);
}
