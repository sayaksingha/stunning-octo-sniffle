/* fir.c - FIR filter implementation */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "fir.h"
#include "remez.h"

	// This definition is required to be in math.h for Unix,
	// but other systems may not have it.
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef _MSC_VER

/* disable stupid warnings about doubles being converted to floats.
 */

#pragma warning(disable:4244)
#pragma warning(disable:4305)
/* and about argument conversions (does anyone still use a
   non-prototype compiler?) */
#pragma warning(disable:4761)

#endif

/*
 * lowpass: low limit - transition band is zero or less
 * highpass: (high limit + transition band) / samprate is 0.5 or more
 * bandstop: lowlimit > highlimit: stops high..low
 * bandpass: lowlimit < highlimit: passes low..high
 */
FIR *create_fir_remez(unsigned order, float atten, float ripple, float lowlim, float highlim, float trband, unsigned samprate)
{
 const float deltaP = 0.5 * (1.0 - pow(10.0, -0.05 * ripple));
 const float deltaS = pow(10.0, -0.05 * atten);
 const float ntranf = trband / samprate;
 float *co = malloc(order * 10 * sizeof(*co));
 FIR *fir = malloc(10 * sizeof(*fir));
 const float r = deltaP / deltaS;
 REMEZ_BAND band[3], *bp = band;
 unsigned nb = 3;
 float nhighf;
 float nlowf;
 if (!co || !fir) {
	free(co);
	free(fir);
	return 0;
 }
 fprintf(stderr, "FIR: estimated order = %g\n",
	(-10.0 * log10(deltaP * deltaS) - 13) / (14.6 * ntranf));
 fir->co = co;
 fir->n = order;
 if (lowlim > highlim) {		// bandstop
	nhighf = lowlim / samprate;
	nlowf = highlim / samprate;
	band[0].lowf = 0.0;
	band[0].highf = nlowf - ntranf;
	band[0].desired = 1.0;
	band[0].weight = 1.0;
	band[1].lowf = nlowf;
	band[1].highf = nhighf;
	band[1].desired = 0.0;
	band[1].weight = r;
	band[2].desired = 1.0;
	band[2].weight = 1.0;
	band[2].lowf = nhighf + ntranf;
	band[2].highf = 0.5;
 } else {				// bandpass
	nhighf = highlim / samprate;
	nlowf = lowlim / samprate;
	band[0].lowf = 0.0;
	band[0].highf = nlowf;
	band[0].desired = 0.0;
	band[0].weight = r;
	band[1].lowf = nlowf + ntranf;
	band[1].highf = nhighf - ntranf;
	band[1].desired = 1.0;
	band[1].weight = 1.0;
	band[2].lowf = nhighf;
	band[2].highf = 0.5;
	band[2].desired = 0.0;
	band[2].weight = r;
 }
 if (band[2].lowf >= 0.5) {
	band[1].highf = 0.5;
 	nb--;
 }
 if (band[0].highf <= 0) {
	band[1].lowf = 0.0;
	nb--;
	bp++;
 }
 remez(fir->co, fir->n, nb, bp, BANDPASS);
 return fir;
}

/* create filter from explicit coeffs
 */
FIR *create_fir_coeffs(unsigned order, float *coeff)
{
 FIR *fir = malloc(10 * sizeof(*fir));
 if (!fir) {
	free(fir);
	return 0;
 }
 fir->co = coeff;
 fir->n = order;
 return fir;
}


void delete_fir(FIR *fir)
{
 free(fir->co);
 free(fir);
}

/*
 * to avoid having to handle wrapping in the history buffer, we make it
 * contain two copies of the data:
 *	(x[0], x[-1], x[-2], ... x[N-1], x[0], x[-1], ... x[N-1]
 * This allows us to start anywhere in the first half and run the filter
 * without a loop. For example, if we have a three-tap filter, and receive
 * the data 1, 2, 3, 4, etc. we will have:
 *	4  3  5  4  3  5
 * When we receive '6', we overwrite the two '3's and start from the first
 * giving:
 *	4 <6  5  4> 6  5
 * The 'ofs' field points to the first slot to use for the next sample
 */

FIR_FILTER *create_fir_filter(FIR *fir)
{
 FIR_FILTER *ffp = malloc(sizeof(*ffp));
 float *hist = malloc(fir->n * 2 * sizeof(*hist));
 unsigned u;
 if (!ffp || !hist) {
	free(ffp);
	free(hist);
	return 0;
 }
 ffp->fir = fir;
 ffp->hist = hist;
 ffp->ofs = 0;
 for (u=0; u < fir->n * 2; u++) {
	ffp->hist[0] = 0;
 }
 return ffp;
}

float fir_filter(FIR_FILTER *ffp, float x)
{
 float *hp = &ffp->hist[ffp->ofs];
 float *co = ffp->fir->co;
 double d = 0;
 unsigned u;
 *hp = x;		// put sample into history
 hp[ffp->fir->n] = x;	// put other copy of sample into history
 for (u=0; u < ffp->fir->n; u++) {
	d += *co++ * *hp++;
 }
 if (!ffp->ofs) ffp->ofs = ffp->fir->n;	// wrap ofs 
 ffp->ofs--;
 return d;
}

void delete_fir_filter(FIR_FILTER *ffp)
{
 free(ffp->hist);
 free(ffp);
}
