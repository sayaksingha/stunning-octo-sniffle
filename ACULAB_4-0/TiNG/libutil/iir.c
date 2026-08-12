/* iir.c - IIR filter implementation */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "iir.h"
#include "complx.h"

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

// n = filter order, rej = ratio of signal at frequency (f * sampling rate)
IIR *create_iir(unsigned n, float rej, float f)
{
 const COMPLX w = complx(0, 2*tan(M_PI/2 * f));
 const COMPLX r = complx_powR(
 	complx(1-2*rej*(rej+sqrt(rej*rej-1)), 0.0),
	1.0/2.0/n);
 IIR *ip = malloc(sizeof(*ip) + sizeof(struct coeff) * (n-1));
 COMPLX *zp = malloc(sizeof(*zp) * n);
 COMPLX *pp = malloc(sizeof(*pp) * n);
 unsigned k, m;
 if (!ip || !zp || !pp) {
 	free(ip);
 	free(zp);
 	free(pp);
 	return 0;
 }
 ip->n2 = (n + 1) / 2;
 fprintf(stderr, "n = %d, rej=%g, f=%g, w=%g%+gi, r=%g%+gi\n", n, rej, f, w.re, w.im, r.re, r.im);
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
 ip->gain = 1;
 for (m=0; m < ip->n2; m++) {
	if (m < n-1-m) {
		COMPLX a1 = complx_add(zp[m], zp[n-1-m]);
		COMPLX b1 = complx_add(pp[m], pp[n-1-m]);
		COMPLX a2 = complx_mul(zp[m], zp[n-1-m]);
		COMPLX b2 = complx_mul(pp[m], pp[n-1-m]);
		a1.re = -a1.re;
		b2.re = -b2.re;
		ip->c[m].a1 = a1.re;
			// assert x.im == 0
		ip->c[m].b1 = b1.re;
			// assert x.im == 0
		ip->c[m].a2 = a2.re;
			// assert x.im == 0
		ip->c[m].b2 = b2.re;
			// assert x.im == 0
#if 1
		fprintf(stderr,
			"%d: a1=%g%+gi, a2=%g%+gi, b1=%g%+gi b2=%g%+gi\n",
			m, a1.re, a1.im, a2.re, a2.im,
			b1.re, b1.im, b2.re, b2.im);
#endif
	} else {
		ip->c[m].a1 = -zp[m].re;
			// assert zp[m].im == 0
		ip->c[m].b1 = pp[m].re;
			// assert pp[m].im == 0
		ip->c[m].a2 = 0;
		ip->c[m].b2 = 0;
#if 1
		fprintf(stderr,
			"%d: a1=%g%+gi, a2=%g%+gi, b1=%g%+gi b2=%g%+gi\n",
			m, -zp[m].re, zp[m].im, 0.0, 0.0,
			pp[m].re, pp[m].im, 0.0, 0.0);
#endif
	}
	ip->gain *= (1 - ip->c[m].b1 - ip->c[m].b2)
		/ (1 + ip->c[m].a1 + ip->c[m].a2);
 }
#if 1
 fprintf(stderr, "gain = %g\n", ip->gain);
#endif
 free(zp);
 free(pp);
 return ip;
}

IIR_FILTER *create_iir_filter(IIR *iir)
{
 IIR_FILTER *ifp = malloc(sizeof(*ifp) + sizeof(float) * (iir->n2*2));
 unsigned u;
 if (!ifp) return 0;
 ifp->iir = iir;
 for (u=0; u<iir->n2; u++) {
	ifp->w[u][0] = 0;
	ifp->w[u][1] = 0;
 }
 return ifp;
}

float iir_filter(IIR_FILTER *ifp, float x)
{
 unsigned u;
 for (u=0; u < ifp->iir->n2; u++) {
	float d = x
		+ ifp->iir->c[u].b1 * ifp->w[u][0]
		+ ifp->iir->c[u].b2 * ifp->w[u][1];
	x = d
		+ ifp->iir->c[u].a1 * ifp->w[u][0]
		+ ifp->iir->c[u].a2 * ifp->w[u][1];
	ifp->w[u][1] = ifp->w[u][0];
	ifp->w[u][0] = d;
 }
 return x * ifp->iir->gain;
}

void delete_iir(IIR *iir)
{
 free(iir);
}

void delete_iir_filter(IIR_FILTER *ifp)
{
 free(ifp);
}
