/* iir4.c - IIR order 4 filter implementation */

#include "iir4.h"

#include <math.h>
#include "complx.h"

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

// order 4 filter, rej = ratio of signal at frequency (f * Nyquist frequency)
// where "Nyquist frequency" is half the sampling rate
LOCALDEF void create_iir_4(double a1[2], double a2[2], double b1[2], double b2[2], double *gain, double rej, double f)
{
 	// filter order
#define n 4
 const COMPLX w = complx(0, 2*tan(M_PI/2 * f));
 const COMPLX r = complx_powR(
 	complx(1-2*rej*(rej+sqrt(rej*rej-1)), 0.0),
	1.0/2.0/n);
 COMPLX zp[n];
 COMPLX pp[n];
 unsigned k, m;
 for (k=0; k<n; k++) {
	COMPLX x = complx_expi((2*k+1)*M_PI/2/n);
	COMPLX wx = complx_mul(x, w);
	COMPLX xx = complx_mul(x, x);
	zp[k] = complx_div(complx_add(complx_sub(xx, wx), complx(1, 0)),
			complx_add(complx_add(xx, wx), complx(1, 0)));
	x = complx_mul(complx_expi(k*M_PI/n), r);
	wx = complx_mul(x, w);
	xx = complx_mul(x, x);
	pp[k] = complx_div(complx_add(complx_sub(xx, wx), complx(1, 0)),
			complx_add(complx_add(xx, wx), complx(1, 0)));
 }
 *gain = 1;
 for (m=0; m < n/2; m++) {	// assumes n is even
	a1[m] = -complx_add(zp[m], zp[n-1-m]).re;
	b1[m] = complx_add(pp[m], pp[n-1-m]).re;
	a2[m] = complx_mul(zp[m], zp[n-1-m]).re;
	b2[m] = -complx_mul(pp[m], pp[n-1-m]).re;
	*gain *= (1 - b1[m] - b2[m]) / (1 + a1[m] + a2[m]);
 }
#undef n
}
