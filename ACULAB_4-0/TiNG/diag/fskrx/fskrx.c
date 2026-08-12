/* fskrx.c - demodulate FSK modulated data */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../libutil/complx.h"
#include "../../libutil/floio.h"
#include "../../libutil/iir.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

#ifndef M_PI
static const double M_PI = 3.14159265358979323846;
#endif

#define AMP_0dB (2*2853.5)      // amplitude of a cosine at 0dBm0

#define SAMPRATE 8000
#define F2W(x) ((x) * M_PI * 2 / SAMPRATE)

#ifndef LOG10
#define LOG10(X) (log(X)/2.302585092994)
#define EXP10(X) exp( (X)*2.302585092994 )
#endif

static COMPLX f2w(float freq)
{
 COMPLX r;
 r.re = cos(F2W(freq)); r.im = sin(F2W(freq));
 return r;
}

static int unfsk(FILE *of, FILE *inf, float markfreq, float spacefreq, unsigned debug)
{
 const COMPLX shif = f2w( (markfreq + spacefreq) / -2 );
 const float filtwid = markfreq > spacefreq
 	? markfreq - spacefreq
 	: spacefreq - markfreq;
 IIR *iir = create_iir(4, 100, 4 * filtwid / SAMPRATE);
 COMPLX prev = complx(0.0, AMP_0dB);
 COMPLX s = complx(0.0, 1.0);
 IIR_FILTER *filre, *filim;
 float inv = markfreq < spacefreq ? 1.0 : -1.0;
 fprintf(stderr, "Shift by %g\n", (markfreq + spacefreq) / -2);
 if (!iir) {
 	fprintf(stderr, "Cannot create IIR filter coefficients\n");
 	return 1;
 }
 {
  unsigned u;
  for (u=0; u < iir->n2; u++) {
	fprintf(stderr, "%d: a1=%g, a2=%g, b1=%g b2=%g\n",
		u, iir->c[u].a1, iir->c[u].a2, iir->c[u].b1, iir->c[u].b2);
  }
 }
 filre = create_iir_filter(iir);
 if (!filre) {
 	delete_iir(iir);
 	fprintf(stderr, "Cannot create IIR filter for reals\n");
 	return 1;
 }
 filim = create_iir_filter(iir);
 if (!filim) {
	delete_iir_filter(filre);
	delete_iir(iir);
	fprintf(stderr, "Cannot create IIR filter for imaginaries\n");
	return 1;
 }
 for (;;) {
	COMPLX c, d, angle;
	float samp;
	if (getflo(inf, &samp)) break;
	samp *= AMP_0dB;		// quick hack
	c.re = s.re * samp;
	c.im = s.im * samp;
	s = complx_mul(s, shif);
	d.re = iir_filter(filre, c.re);
	d.im = iir_filter(filim, c.im);
	angle = complx_mul(d, complx_conj(prev));
	switch (debug) {
	case 0: putflo(of, angle.im * inv); break;	// normal output
	case 1: putflo(of, angle.re); break;	// real part of angle
	case 2: putflo(of, d.im); break;	// filtered im
	case 3: putflo(of, d.re); break;	// filtered re
	case 4: putflo(of, c.im); break;	// shifted im
	case 5: putflo(of, c.re); break;	// shifted re
	case 6: putflo(of, samp); break;	// input
	}
	prev = d;
 }
 delete_iir_filter(filim);
 delete_iir_filter(filre);
 delete_iir(iir);
 return 0;
}

#include "gen/fskrx.args.i"

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
 return unfsk(outf, inf, arg.markfreq, arg.spacefreq, arg.debug);
}
