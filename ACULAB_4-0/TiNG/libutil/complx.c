/* complx.c - complex numbers */

#include <math.h>

#include "complx.h"

#ifdef _MSC_VER

/* disable stupid warnings about doubles being converted to floats.
 */

#pragma warning(disable:4244)
#pragma warning(disable:4305)
/* and about argument conversions (does anyone still use a
   non-prototype compiler?) */
#pragma warning(disable:4761)

#endif

COMPLX complx(float re, float im)
{
 COMPLX x;
 x.re = re;
 x.im = im;
 return x;
}

float complx_magsq(COMPLX x)
{
 return x.re * x.re + x.im * x.im;
}

COMPLX complx_conj(COMPLX x)
{
 return complx(x.re, -x.im);
}

COMPLX complx_add(COMPLX x, COMPLX y)
{
 return complx(x.re + y.re, x.im + y.im);
}

COMPLX complx_sub(COMPLX x, COMPLX y)
{
 return complx(x.re - y.re, x.im - y.im);
}

COMPLX complx_mul(COMPLX x, COMPLX y)
{
 return complx(x.re * y.re - x.im * y.im,
		x.im * y.re + x.re * y.im);
}

float complx_abs(COMPLX x)
{
 return sqrt(complx_magsq(x));
}

COMPLX complx_div(COMPLX x, COMPLX y)
{
 float m = complx_magsq(y);
 return complx((x.re * y.re + x.im * y.im) / m,
		(x.im * y.re - x.re * y.im) / m);
}

// exp(i*d)
COMPLX complx_expi(float d)
{
 return complx(cos(d), sin(d));
}

// ln(x)
COMPLX complx_ln(COMPLX x)
{
 float r = sqrt(x.re*x.re + x.im*x.im);
 float theta = atan2(x.im, x.re);
 x.re = log(r);
 x.im = theta;
 return x;
}

COMPLX complx_exp(COMPLX x)
{
 float r = exp(x.re);
 x = complx_expi(x.im);
 x.re *= r;
 x.im *= r;
 return x;
}

COMPLX complx_pow(COMPLX x, COMPLX y)
{
 return complx_exp(complx_mul(complx_ln(x), y));
}

// complex number to the power of a real number
COMPLX complx_powR(COMPLX x, float p)
{
 float r = sqrt(x.re*x.re + x.im*x.im);
 float theta = atan2(x.im, x.re);
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

float complx_arg(COMPLX x)
{
 return atan2(x.im, x.re);
}
