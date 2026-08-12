#include "div32.h"
#include <math.h>
#include <stdlib.h>

// #define CHECK_DIV32

#ifdef CHECK_DIV32
#include <stdio.h>
#define CHECK \
	r2 = ldexp(n / (double) d + q, s); \
	if (abs((r2 - r) / r) > 1e-8) { \
		fprintf(stderr, "\n@%d n=%x, d=%x, s=%d, %g != %g, n=%x, d=%x, s=%d, q=%x\n", \
			__LINE__, nn, dd, ss, r, r2,n, d, s, q); \
		abort(); \
	}
#else
#define CHECK
#endif

	// on return *qp << *sc == (n / d) << s
	// as if all were floating point
void div32(uint_fast32_t *qp, int_fast32_t *sc, uint_fast32_t n, uint_fast32_t d, int_fast32_t s)
{
#ifdef CHECK_DIV32
 uint_fast32_t nn = n, dd = d, ss = s;
 double r = ldexp(n / (double) d, s);
 double r2;
#endif
 	// let V = n2^s/d, 0 <= n < 2^32, 0 <= d < 2^32
 uint_fast32_t q = 0;
 if (!d) {
#ifdef CHECK_DIV32
	fprintf(stderr, "qp=%p, sc=%p, n=%u, d=%u, s=%d\n", qp, sc, n, d, s);
#endif
	abort();
 }
 if (!n) {
 	*qp = 0;
 	*sc = s;
 	return;
 }
 	CHECK
// n2^s/d == V
 while ((int_fast32_t) d >= 0) {	// normalise d
// n2^(s+1)/(2d) == V
 	d <<= 1;
 	s++;
// n2^s/d == V
 }
// n2^s/d == V
	CHECK
// (q + n/d)2^s == V && 2^31 <= d < 2^32, n < 2^32
 if (d == 0x80000000) {		// it's effectively, division by one
// n2^s/2^31 == V
// n2^(s-31) == V
 	*qp = n;
 	*sc = s - 31;
 	return;
 }
CHECK
// (q + n/d)2^s == V && 2^31 < d < 2^32, n < 2^32
 // printf("q=%d\tn=%d\ts=%d\td=%d\n", q, n, s, d);
 for (; n; ) {
// (q + n/d)2^s == V && 2^31 < d < 2^32, n < 2^32
	if (n >= d) {
// (q + n/d)2^s == V && 2^31 < d <= n < 2^32
			n -= d;
// (q + (n+d)/d)2^s == V && n < 2^31 < d < 2^32
// (q+1 + n/d)2^s == V && n < 2^31 < d < 2^32
			q++;
// (q + n/d)2^s == V && n < 2^31 < d < 2^32
			CHECK
	}
// (q + n/d)2^s == V && 2^31 < d < 2^32 && n < d
	// printf("n=%x, d=%x, s=%d, q=%x\n", n, d, s, q);
CHECK
 	if ((int_fast32_t) q < 0) {	// q is full, must stop
// (q + n/d)2^s == V && 2^31 < d < 2^32 && n < d
			// code for rounding using round to even
			// error = n/d
			// round up if n/d > 0.5, or 2n > d
			// round down if n/d < 0.5, or 2n < d
			// round to even if n/d == 0.5, or 2n == d
		uint_fast32_t v = n * 2;	// may overflow if 2^31 <= n
		if ((int_fast32_t) n < 0		// 2^31 <= n < 2^32 <= 2n
			|| v > d
			|| (v == d && q & 1)) {	// exactly half way
			if (!++q) {
					// oops: 0xffffffff + 1 -> 0
				q = 0x80000000;
				s++;
			}
		}
 		break;
	}
// (q + n/d)2^s == V && 2^31 < d < 2^32 && n < d
	q <<= 1;
// (q/2 + n/d)2^s == V && 2^31 < d < 2^32 && n < d
	s--;
// (q/2 + n/d)2^(s+1) == V && 2^31 < d < 2^32 && n < d
// (q + 2n/d)2^s == V && 2^31 < d < 2^32 && n < d
 	if ((int_fast32_t) n < 0) {	// so 2^31 <= n < d
// (q + 2n/d)2^s == V && 2^31 <= n < d < 2^32
		n <<= 1;	// 2^32 <= n < 2d
// (q + 2n/d)2^s == V && 2^31 <= n/2 < d < 2^32
// (q + n/d)2^s == V && 2^31 < d < 2^32 <= n < 2d
		n -= d;
// (q + (n+d)/d)2^s == V && 2^31 < d < 2^32 <= (n+d) < 2d
// (q+1 + n/d)2^s == V && 2^31 < d < 2^32 <= n < d
		q++;
// (q + n/d)2^s == V && 2^31 < d < 2^32 <= n < d
		// could skip n >= d test
// (q + n/d)2^s == V && 2^31 < d < 2^32 && n < 2^32
	} else {	// n < 2^31
// (q + 2n/d)2^s == V && 2^31 < d < 2^32 && n < 2^31
		n <<= 1;
// (q + n/d)2^s == V && 2^31 < d < 2^32 && n < 2^32
	}
		CHECK
// (q + n/d)2^s == V && 2^31 < d < 2^32 && n < 2^32
	// printf("q=%d\tn=%d\ts=%d\td=%d\n", q, n, s, d);
 }
 	// q2^s == V - n2^s / d
 *sc = s;
 *qp = q;
	//printf("\n@%d n=%x, d=%x, s=%d -> s=%d, q=%x\n", __LINE__, nn, dd, ss, s, q);
}
