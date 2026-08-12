/* genchirp.c - Generate a DTMF test signal to a file */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../libutil/floio.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

typedef struct comp {
	double re;
	double im;
} COMP;

static COMP comp(double re, double im)
{
 COMP x;
 x.re = re;
 x.im = im;
 return x;
}

static COMP cmul(COMP x, COMP y)
{
 COMP r;
 r.re = x.re * y.re - x.im * y.im;
 r.im = x.im * y.re + x.re * y.im;
 return r;
}

static COMP crecip(COMP x)
{
 COMP r;
 double m = x.re*x.re + x.im*x.im;
 r.re = x.re / m;
 r.im = -x.im / m;
 return r;
}

static int genchirp(FILE *of, unsigned samples)
{
 unsigned i;
 COMP dw = comp(cos(M_PI / samples / 2), sin(M_PI / samples / 2));
 COMP w = comp(1.0, 0.0);
 COMP c = comp(1.0, 0.0);
 for(i=0; i < samples; i++) {
	double temp = i * c.im / samples;
	putflo(of, temp);
	c = cmul(c, w);
	w = cmul(w, dw);
 }
 dw = crecip(dw);
 for ( ;i ; i--) {
	double temp = i * c.im / samples;
	putflo(of, temp);
	c = cmul(c, w);
	w = cmul(w, dw);
 }
 return 0;
}

#include "gen/genchirp.args.i"

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
 return genchirp(f, arg.samples);
}
