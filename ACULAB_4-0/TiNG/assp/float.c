#define TiNG_EMULATE_FLOAT
#include "float.h"
#include <stdint.h>

#ifdef TiNG_EMULATE_FLOAT

// #define FLOATDBG
#ifdef FLOATDBG
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#endif

static const FLOAT zero = { 0, FP_BASE_MIN, };

#ifdef TiNGTYPE_STARCORE
//#include "starcorehelp.c"

#define HAVE_DEFINED_CLB64
asm int16_t clb64(uint64_t v)
{
asm_header
	.arg _v in $dd0;
	return in $d0;
	.reg $d0,$d1,$d2;
asm_body
		// d1 is low word, d0 is high word
	[
	tsteq d0		// clb(0) = 0, so must check for zero
	extract #1,#31,d1,d2	// d2 is all zeroes or all ones
	sxt.l d1		// fill top 8 bits of d1 so that clb skips them
	sxt.l d0		// fill top 8 bits of d0 so that clb skips them
	]
	[
	eor d0,d2		// zero if high word is just sign extension
	clb d0,d0
	]
	[
	ift move #-31,d0	// handle stupid clb(0) = 0
	ifa tsteq d1		// clb(0) = 0, so must check for zero
	]
	clb d1,d1
	[
	ift move #-31,d1	// handle stupid clb(0) = 0
	ifa tsteq d2
	ifa move #1,d2		// prepare to fix results
	]
	[
	sub d0,d2,d0		// fix results to compensate for silly clb
	sub d1,d2,d1		// and to subtract 8, since d0 & d1 are 40 bits
	]
	ift add d0,d1,d0	// if top was only sign extension, add bottom
asm_end
}

asm int16_t normshift64nz(uint64_t v)
{
asm_header
	.arg _v in $dd0;
	return in $d0;
	.reg $d0,$d1,$d2;
asm_body
		// d1 is low word, d0 is high word
	[
	tsteq d0		// clb(0) = 0, so must check for zero
	extract #1,#31,d1,d2	// d2 is all zeroes or all ones
	sxt.l d1		// fill top 8 bits of d1 so that clb skips them
	sxt.l d0		// fill top 8 bits of d0 so that clb skips them
	]
	[
	eor d0,d2		// zero if high word is just sign extension
	clb d0,d0
	]
	[
	ift move #-31,d0	// handle stupid clb(0) = 0
	ifa tsteq d1		// clb(0) = 0, so must check for zero
	]
	clb d1,d1
	[
	ift move #-31,d1	// handle stupid clb(0) = 0
	ifa tsteq d2
	ifa move #1,d2		// prepare to fix results
	]
	[
	sub d0,d2,d0		// fix results to compensate for silly clb
	sub d1,d2,d1		// and to subtract 8, since d0 & d1 are 40 bits
	]
	ift add d0,d1,d0	// if top was only sign extension, add bottom
	sub d0,d2,d0
asm_end
}

#define HAVE_DEFINED_CLB32
asm int16_t clb32(uint32_t v)
{
asm_header
	.arg _v in $d0;
	return in $d0;
	.reg $d0,$d1,$d2;
asm_body
		// d1 is low word, d0 is high word
	[
	tsteq d0		// clb(0) = 0, so must check for zero
	sxt.l d0		// fill top 8 bits of d0 so that clb skips them
	]
	[
	clb d0,d0
	move #1,d2		// prepare to fix results
	]
	[
	ift move #32,d0		// handle stupid clb(0) = 0
	iff sub d0,d2,d0	// fix results to compensate for silly clb
	]
asm_end
}

#define HAVE_DEFINED_NORMSHIFT32
asm int16_t normshift32(uint32_t v)
{
asm_header
	.arg _v in $d0;
	return in $d0;
	.reg $d0,$d1,$d2;
asm_body
	sxt.l d0		// fill top 8 bits of d0 so that clb skips them
	clb d0,d0
asm_end
}

#endif

#ifndef HAVE_DEFINED_CLB32
static int lg(uint64_t v)
{
 uint32_t v2, v3;
 int z = 0;
 int sh;
 v2 = v >> 32;
 if (v2) z = 32;
 else v2 = v;
 	// lg(v) == z + lg(v2)
 for (sh=16; sh; sh >>= 1) {
	v3 = v2 >> sh;
	if (v3) {
		v2 = v3;
		z += sh;
 	}
 }
 if (!v2) z--;	// only if v==0
 return z;
}

static int clo(uint64_t v)
{
 uint32_t v2, v3;
 int z = 0;
 v2 = v >> 32;
 if (v2 == 0xffffffff) {
	z = 32;
 	v2 = v;
 }
 		// v has z + clo(v2) leading zeroes
 v2 &= 0xffffffff;
 v3 = v2 >> 16;
 if (v3 != 0xffff) v2 = v3;
 else z += 16;
 		// v has z + clo(v2) leading zeroes
 v2 &= 0xffff;
 v3 = v2 >> 8;
 if (v3 != 0xff) v2 = v3;
 else z += 8;
 		// v has z + clo(v2) leading zeroes
 v2 &= 0xff;
 v3 = v2 >> 4;
 if (v3 != 0xf) v2 = v3;
 else z += 4;
 		// v has z + clo(v2) leading zeroes
 v2 &= 0xf;
 v3 = v2 >> 2;
 if (v3 != 3) v2 = v3;
 else z += 2;
 		// v has z + clo(v2) leading zeroes
 v2 &= 0x3;
 v3 = v2 >> 1;
 if (v3 != 1) v2 = v3;
 else z++;
 		// v has z + clo(v2) leading zeroes
 v2 &= 0x1;
 if (v2) z++;
 return z;
}

static int clb64(int64_t m)
{
 if (m > 0) return 63 - lg(m);
 else if (m < 0) return clo(m);
 return 64;
}

static int clb32(int32_t m)
{
 if (m > 0) return 31 - lg(m);
 else if (m < 0) return clo(m) - 32;
 return 32;
}

static int16_t normshift32(uint32_t v)
{
 return 1 - clb32(v);
}

static int16_t normshift64nz(uint64_t v)
{
 return 1 - clb64(v);
}
#endif

FLOAT float_int32(int32_t m)
{
 FLOAT f;
 int rs = normshift32(m);
 if (rs >= 0) {
	f.m = m >> rs;
 } else {
	f.m = m << -rs;
 }
 f.e = rs;
 return f;
}

static int32_t float_2int32_round(FLOAT f)
{
 int s = -f.e;
 int mask, r;
 if (s >= 32) return 0;	// -0.5 <= f <= 0.5, round to zero
 mask = 1 << (s-1);
 r = f.m & mask && (f.m & (mask << 1) || f.m & (mask - 1));
 if (-2 >> 1 == -1) {	// right shift works correctly
	f.m >>= s;
 } else {
 	f.m /= (1 << -f.e);
 }
 if (r) f.m++;
 return f.m;
}

int32_t float_2int32(FLOAT f)
{
 if (f.e >= 0) {
 	if (!f.e) return f.m;	// exact result
 	return f.m < 0 ? FP_BASE_MIN : FP_BASE_MAX;
 }
 return float_2int32_round(f);
}

int64_t float_2int64(FLOAT f)
{
 if (f.e >= 0) {
	int64_t m = f.m;
 	if (f.e <= 32) return m << f.e;
	return f.m < 0 ? -((1uLL << 63) - 1) - 1 : (1uLL << 63) - 1;
 }
 	// fits in 32 bits, so just use that
 return float_2int32_round(f);
}

FLOAT float_int64e(int64_t m, int e)
{
 FLOAT f;
 int rs;
 if (!m) return zero;
 rs = 32 + normshift64nz(m);
 f.e = e + rs;
 if (rs > 0) {	// NB: not rs==0 because of shift by rs-1
	uint32_t r = m;
	uint32_t rmask = 1 << (rs - 1);
	f.m = m >> rs;
	if (r & rmask) {
		if (f.m & 1 || r & (rmask - 1)) {
			if (!++f.m) {
				f.m = 0x80000000;
				f.e++;
			} else if (f.m + 0u == 0x80000000) {
				f.m = 0x40000000;
				f.e++;
			}
		}
	}
 } else {
	f.m = m << -rs;
 }
 return f;
}

FLOAT float_neg(FLOAT f)
{
 if (f.m + 0u == 0x80000000) {
	f.m = 0x40000000;
	f.e++;
 } else {
	f.m = -f.m;
	if (f.m + 0u == 0xc0000000) {
		f.m <<= 1;
		f.e--;
 	}
 }
 return f;
}

FLOAT float_add(FLOAT f, FLOAT g)
{
 int64_t r, fm;
 if (f.e < g.e) return float_add(g, f);
 if (g.e < FP_BASE_MAX - 31) {	// else they *must* overlap
	if (f.e > 31 + g.e) {	// no bits overlap
			// FIXME: must round properly
		return f;
	}
 }
 r = g.m;
 r <<= 31 - (f.e - g.e);
 fm = f.m;
 fm <<= 31;
 r += fm;
 return float_int64e(r, f.e - 31);
}

FLOAT float_mul(FLOAT f, FLOAT g)
{
 uint64_t p = f.m;
 return float_int64e(p * g.m, f.e + g.e);
}

// Goldschmidt Division
FLOAT float_div(FLOAT n, FLOAT d)
{
 FLOAT approx[16] = {
#include "gen/divtbl.i"
 };
 FLOAT two = float_int32(2);
 unsigned u;
 FLOAT r;
#ifdef FLOATDBG
 printf("%g (%x:%x) / %g (%x:%x)\n",
 	ldexp(n.m, n.e), n.m, n.e,
 	ldexp(d.m, d.e), d.m, d.e);
#endif
 if (d.m < 0) {
 	d = float_neg(d);
 	n = float_neg(n);
#ifdef FLOATDBG
	printf("\t%g (%x:%x) / %g (%x:%x)\n",
		ldexp(n.m, n.e), n.m, n.e,
		ldexp(d.m, d.e), d.m, d.e);
#endif
 }
 n.e -= d.e;
 d.e = 0;
 r = approx[(d.m >> 26) - 16];
#ifdef FLOATDBG
 printf("\tapprox[%d] = %g (%x:%x)\n", (d.m >> 26) - 16,
 	ldexp(r.m, r.e), r.m, r.e);
 printf("\t%g (%x:%x) / %g (%x:%x) = %g\n",
 	ldexp(n.m, n.e), n.m, n.e,
 	ldexp(d.m, d.e), d.m, d.e,
	ldexp(n.m, n.e) / ldexp(d.m, d.e) );
#endif
 for (u=0; u < 7; u++) {
	n = float_mul(n, r); d = float_mul(d, r);
	r = float_add(two, float_neg(d));
#ifdef FLOATDBG
	printf("\t%g (%x:%x) / %g (%x:%x) = %g\n\t r = %g (%x:%x)\n",
		ldexp(n.m, n.e), n.m, n.e,
		ldexp(d.m, d.e), d.m, d.e,
		ldexp(n.m, n.e) / ldexp(d.m, d.e),
		ldexp(r.m, r.e), r.m, r.e
		);
#endif
	if (r.m == 0x40000000) break;
 }
 r = float_mul(r, n);
#ifdef FLOATDBG
 printf("\t= %g (%x:%x)\n", ldexp(r.m, r.e), r.m, r.e);
#endif
 return r;
}

FLOAT float_shl(FLOAT a, int b)
{
 if (a.m) a.e += b;
 return a;
}

#endif
