/* complx.h - complex numbers */

#ifndef INCLUDED_COMPLX_H
#define INCLUDED_COMPLX_H

typedef struct complx {
	float re;
	float im;
} COMPLX;

COMPLX complx(float re, float im);
float complx_magsq(COMPLX x);
float complx_abs(COMPLX x);
COMPLX complx_conj(COMPLX x);
COMPLX complx_add(COMPLX x, COMPLX y);
COMPLX complx_sub(COMPLX x, COMPLX y);
COMPLX complx_mul(COMPLX x, COMPLX y);
COMPLX complx_div(COMPLX x, COMPLX y);
COMPLX complx_expi(float d);
COMPLX complx_ln(COMPLX x);
COMPLX complx_exp(COMPLX x);
COMPLX complx_pow(COMPLX x, COMPLX y);
COMPLX complx_powR(COMPLX x, float r);
float complx_arg(COMPLX x);

#endif
