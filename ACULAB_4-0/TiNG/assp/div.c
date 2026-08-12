#include <stdio.h>

#include "div32.c"

int main(int argc, char **argv)
{
 uint_fast32_t n = 0xcccccccd;
 uint_fast32_t d = 0x14;
 uint_fast32_t s = -35;
 uint_fast32_t q, sr;
 double f, r;
 div32(&q, &sr, n, d, s);
 f = n;
 f = ldexp(f, s);
 f /= d;
 r = q;
 r = ldexp(r, sr);
 printf("n=%u, d=%u, s=%d => q=%u sr=%d == %g, /= %g, e=%g\n", 
 	n, d, s, q, sr, r, f, (r - f) / f);
 return 0;
}
