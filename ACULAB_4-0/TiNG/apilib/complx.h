/* complx.h - complex numbers */

#ifndef INCLUDED_COMPLX_H
#define INCLUDED_COMPLX_H

#include <math.h>

typedef struct complx {
	double re;
	double im;
} COMPLX;

static COMPLX complx(double re, double im)
{
 COMPLX x;
 x.re = re;
 x.im = im;
 return x;
}

static COMPLX complx_add(COMPLX x, COMPLX y)
{
 return complx(x.re + y.re, x.im + y.im);
}

static COMPLX complx_sub(COMPLX x, COMPLX y)
{
 return complx(x.re - y.re, x.im - y.im);
}

static COMPLX complx_mul(COMPLX x, COMPLX y)
{
 return complx(x.re * y.re - x.im * y.im,
		x.im * y.re + x.re * y.im);
}

static COMPLX complx_div(COMPLX x, COMPLX y)
{
 double m = y.re * y.re + y.im * y.im;
 return complx((x.re * y.re + x.im * y.im) / m,
		(x.im * y.re - x.re * y.im) / m);
}

// exp(i*d)
static COMPLX complx_expi(double d)
{
 return complx(cos(d), sin(d));
}

// ln(x)
// complex number to the power of a real number
static COMPLX complx_powR(COMPLX x, double p)
{
 double r = sqrt(x.re*x.re + x.im*x.im);
 double theta = atan2(x.im, x.re);
 x = complx_expi(theta*p);
 if (p < 0) {
	r = pow(r, -p);
	x.re /= r;
	x.im /= r;
 } else {
	r = pow(r, p);
	x.re *= r;
	x.im *= r;
 }
 return x;
}

static double complx_mod_sq(COMPLX x)
{
 return x.re * x.re + x.im * x.im;
}

#endif
