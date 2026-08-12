/* testfp.c - test code for FLOAT implementation */
	// force emulation - otherise we are comparing the native
	// floats against themselves - not very useful
#define TiNG_EMULATE_FLOAT

#include "float.h"
#include "../libutil/random.h"

#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#ifdef TiNGTYPE_LINUX
#define isfinite finite
#endif


#define arlen(x) (sizeof(x)/sizeof(*(x)))

static FLOAT float_double(double d)
{
 FLOAT f;
 f.m = frexp(d, &f.e) * (1 << 30);
 f.e -= 30;
 return f;
}

static int checkfloat(int32_t a)
{
 union {
 	float f;
 	int32_t i;
 } u;
 FLOAT f;
 u.i = a;
 if (!isfinite(u.f)) return 0;
 f = float_double(u.f);
 if (float_todouble(f) != u.f) {
 	printf("FAIL: float_double(%g %x) = %g (%x:%x), (d=%g)\n", u.f, u.i, float_todouble(f), f.m, f.e, float_todouble(f) - u.f);
 	return 1;
 }
 return 0;
}

static int checkint(int32_t a)
{
 FLOAT f = float_int32(a);
 if (float_todouble(f) != a) {
 	printf("FAIL: float_int32(%d) = %g (%x:%x), not %g (d=%g)\n", a, float_todouble(f), f.m, f.e, (double) a, float_todouble(f) - a);
 	return 1;
 }
 if (float_2int32(f) != a) {
 	printf("FAIL: float_2int32(float_int32(%d) = (%x:%x)) = %d\n", a, f.m, f.e, float_2int32(f));
 	return 1;
 }
 return 0;
}

static int check64e(int64_t a, int e)
{
 FLOAT f = float_int64e(a, e);
 double d = ldexp(a, e);
 if (float_todouble(f) != d) {
 	printf("FAIL: float_int64e(%" PRId64 ", %d) = %g (%x:%x), not %g (d=%g)\n", a, e, float_todouble(f), f.m, f.e, d, float_todouble(f) - d);
 	return 1;
 }
 return 0;
}

// difference between fv and rv expressed as the number of bits
// which are different
static int biterr(FLOAT fv, double rv)
{
 double dfv = float_todouble(fv);
 int de, rve;
 double d;
 if (dfv == rv) return 0;
 d = dfv - rv;
 frexp(d, &de);
 frexp(rv, &rve);
 return 31 - (rve - de);
}

static int checkfneg(FLOAT fa)
{
 FLOAT fneg = float_neg(fa);
 double a = float_todouble(fa);
 double neg = -a;
 int be = biterr(fneg, neg);
 if (be) {
	printf("FAIL: be=%d float_neg(%g) = %g (%x:%x), not %g (d=%g)\n", be, a, float_todouble(fneg), fneg.m, fneg.e, neg, float_todouble(fneg) - neg);
	printf("%g = %x:%x\n", a, fa.m, fa.e);
	return 1;
 }
 // printf("-%x:%x = %x:%x\n", fa.m, fa.e, fneg.m, fneg.e);
 return 0;
}

static int checkfloat2int32(FLOAT fa)
{
 int_fast32_t v = float_2int32(fa);
 double a = float_todouble(fa);
 double err = fabs(a - v);
 if (err > 0.5) {
 	if (a >= 0x7fffffff + 0.5) return 0;
 	if (a <= -0x7fffffff - 0.5) return 0;
	printf("FAIL: err=%g float_2int32(%g) = %d (0x%x)\n", err, a, v, v);
	printf("%g = %x:%x\n", a, fa.m, fa.e);
	return 1;
 }
 // printf("-%x:%x = %x:%x\n", fa.m, fa.e, fneg.m, fneg.e);
 return 0;
}

static int checkfloat2int64(FLOAT fa)
{
 int_fast64_t v = float_2int64(fa);
 double a = float_todouble(fa);
 double err = fabs(a - v);
 if (err > 0.5) {
 	if (a >= 0x7fffffff + 0.5) return 0;
 	if (a <= -0x7fffffff - 0.5) return 0;
	printf("FAIL: err=%g float_2int64(%g) = %" PRId64 " (0x%" PRIx64 ")\n", err, a, v, v);
	printf("%g = %x:%x\n", a, fa.m, fa.e);
	return 1;
 }
 // printf("-%x:%x = %x:%x\n", fa.m, fa.e, fneg.m, fneg.e);
 return 0;
}

static int checkineg(int32_t a)
{
 return checkfneg(float_int32(a));
}

static int checkfadd(FLOAT fa, FLOAT fb)
{
 FLOAT fsum = float_add(fa, fb);
 double a = float_todouble(fa);
 double b = float_todouble(fb);
 double sum = a + b;
 int be = biterr(fsum, sum);
 if (be > 1) {
	printf("FAIL: be=%d float_add(%g, %g) = %g (%x:%x), not %g (d=%g)\n", be, a, b, float_todouble(fsum), fsum.m, fsum.e, sum, float_todouble(fsum) - sum);
	printf("%g = %x:%x\n", a, fa.m, fa.e);
	printf("%g = %x:%x\n", b, fb.m, fb.e);
	return 1;
 }
 // printf("%x:%x + %x:%x = %x:%x\n", fa.m, fa.e, fb.m, fb.e, fsum.m, fsum.e);
 return 0;
}

static int checkiadd(int32_t a, int32_t b)
{
 return checkfadd(float_int32(a), float_int32(b));
}

static int checkfmul(FLOAT fa, FLOAT fb)
{
 FLOAT fprod = float_mul(fa, fb);
 double a = float_todouble(fa);
 double b = float_todouble(fb);
 double prod = a * b;
 int be = biterr(fprod, prod);
 if (be > 1) {
	printf("FAIL: be=%d float_mul(%g, %g) = %g (%x:%x), not %g (d=%g)\n", be, a, b, float_todouble(fprod), fprod.m, fprod.e, prod, float_todouble(fprod) - prod);
	printf("%g = %x:%x\n", a, fa.m, fa.e);
	printf("%g = %x:%x\n", b, fb.m, fb.e);
	return 1;
 }
 // printf("%x:%x + %x:%x = %x:%x\n", fa.m, fa.e, fb.m, fb.e, fprod.m, fprod.e);
 return 0;
}

static int checkimul(int32_t a, int32_t b)
{
 return checkfmul(float_int32(a), float_int32(b));
}

static int checkfdiv(FLOAT fa, FLOAT fb)
{
 FLOAT fq = float_div(fa, fb);
 double a = float_todouble(fa);
 double b = float_todouble(fb);
 double q = a / b;
 int be = biterr(fq, q);
 if (be > 3) {
	printf("FAIL: be=%d float_div(%g, %g) = %g (%x:%x), not %g (d=%g)\n", be, a, b, float_todouble(fq), fq.m, fq.e, q, float_todouble(fq) - q);
	printf("%g = %x:%x\n", a, fa.m, fa.e);
	printf("%g = %x:%x\n", b, fb.m, fb.e);
	return 1;
 }
 // printf("%x:%x / %x:%x = %x:%x\n", fa.m, fa.e, fb.m, fb.e, fq.m, fq.e);
 return 0;
}

static int checkidiv(int32_t a, int32_t b)
{
 return checkfdiv(float_int32(a), float_int32(b));
}

static FLOAT muller(FLOAT y, FLOAT z)
{
 FLOAT c_108 = float_int32(108);
 FLOAT c_815 = float_int32(815);
 FLOAT c_1500 = float_int32(1500);
 return float_add(c_108, float_neg(float_div(float_add(c_815, float_neg(float_div(c_1500, z))), y)));
}

static void test_muller(void)
{
 	// the correct answer is 5, but (unless the precision is
 	// absurdly high) the expected answer is 100
 FLOAT z = float_int32(4);
 FLOAT y = float_div(float_int32(4.25*4), float_int32(4));
 unsigned u;
 for (u=0; u < 80; u++) {
 	FLOAT x = muller(y, z);
	printf("%u -> %g\n", u, float_todouble(y));
 	z = y;
 	y = x;
 }
 printf("muller(%u) -> %g\n", u, float_todouble(z));
}

int main(int argc, char **argv)
{
 static int itests[] = {0, 9, -1, -2, -16, -256, -32768, -65536, -65536*2, 1, 4,
 	0x7fffffff, 0xffffffff, 0x80000000, 0x80000001, };
 RANDMT rmt;
 unsigned u;
 randmt(&rmt, 1);
 for (u=0; u < arlen(itests); u++) {
	if (checkint(itests[u])) return 1;
	if (checkineg(itests[u])) return 1;
	if (check64e(itests[u], 0)) return 1;
	if (check64e(itests[u], 1)) return 1;
	if (check64e(itests[u], -1)) return 1;
	if (check64e(itests[u], 2)) return 1;
	if (check64e(itests[u], -2)) return 1;
	if (check64e(itests[u], 20000)) return 1;
	if (check64e(itests[u], -20000)) return 1;
 }
 for (u=0; u < 10000; u++) {
 	uint_fast32_t v = randmt_int32(&rmt);
 	if (checkint(v)) return 1;
 	if (checkfloat(v)) return 1;
 	if (checkineg(v)) return 1;
 }
 if (checkiadd(0x7ffffffe, 1)) return 1;
 if (checkfadd(float_int64e(0,0), float_int64e(0xbfb49900, -77))) return 1;
 if (checkfadd(float_int32(1), float_int32(0))) return 1;
 if (checkfadd(float_int64e(0x98a79000, 0x5b), float_int32(0))) return 1;
 for (u=61; u < 64; u++) {
	static uint_fast32_t r2list[] = { 0x80000000, 0x80000001, 0xc0000000, 0xfffffffc, 0x7ffffffe, 0x40000000};
	unsigned v;
	for (v=0; v < arlen(r2list); v++) {
		if (checkfadd(float_int64e(1, u), float_int64e(r2list[v], 0))) return 1;
		if (checkfadd(float_int64e(0x40000001, u-30), float_int64e(r2list[v], 0))) return 1;
	}
 }
 if (checkidiv(0x904440d0, -256)) return 1;
 //test_muller();
 for (u=0; ; ) {
	int_fast32_t r0 = randmt_int32(&rmt);
	int_fast32_t r1 = randmt_int32(&rmt);
	FLOAT f0 = float_int64e(r0 / 256, (r0 & 0xff) - 128);
	FLOAT f1 = float_int64e(r1 / 256, (r1 & 0xff) - 128);
 	if (checkfloat2int32(f0)) return 1;
 	if (checkfloat2int32(f1)) return 1;
 	if (checkfloat2int64(f0)) return 1;
 	if (checkfloat2int64(f1)) return 1;
	if (checkiadd(r0, r1)) return 1;
	if (checkfadd(f0, f1)) return 1;
	if (checkimul(r0, r1)) return 1;
	if (checkfmul(f0, f1)) return 1;
	if (r1 && checkidiv(r0, r1)) return 1;
	if (f1.m && checkfdiv(f0, f1)) return 1;
	if (!++u) break;
 }
 return 0;
}
