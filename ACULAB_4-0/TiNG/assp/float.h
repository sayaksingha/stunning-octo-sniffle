#ifndef INCLUDED_FLOAT_H
#define INCLUDED_FLOAT_H

#include <limits.h>
#include <stdint.h>

#ifndef TiNG_EMULATE_FLOAT

#include <math.h>

typedef float FLOAT;

#define float_int64e(i, e) ldexp((i), (e))
#define float_int32(i) (i)
#define float_2int64(f) ((int64_t) (f))
#define float_2int32(f) ((int32_t) (f))
#define float_neg(f) (-(f))
#define float_add(a, b) ((a)+(b))
#define float_mul(a, b) ((a)*(b))
#define float_div(a, b) ((a)/(b))
#define float_shl(a, b) ((a)*pow(2, (b)))

#define float_todouble(f) (f)

#else

#if INT_MIN < -2147483647 && INT_MAX >= 2147483647
typedef int FP_BASE;
#define FP_BASE_MIN INT_MIN
#define FP_BASE_MAX INT_MAX
#endif

#ifndef FP_BASE_MIN
typedef long FP_BASE;
#define FP_BASE_MIN LONG_MIN
#define FP_BASE_MAX LONG_MAX
#endif

typedef struct {
	FP_BASE m;
	FP_BASE e;
} FLOAT;

FLOAT float_int64e(int64_t i, int e);
FLOAT float_int32(int32_t i);
int32_t float_2int32(FLOAT f);
int64_t float_2int64(FLOAT f);
FLOAT float_neg(FLOAT f);
FLOAT float_add(FLOAT a, FLOAT b);
FLOAT float_mul(FLOAT a, FLOAT b);
FLOAT float_div(FLOAT a, FLOAT b);
FLOAT float_shl(FLOAT a, int b);

#define float_todouble(f) ldexp((f).m, (f).e)

#endif


#endif
