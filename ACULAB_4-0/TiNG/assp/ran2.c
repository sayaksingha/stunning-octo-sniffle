/* ran2.c - random number generator */

#include "ran2.h"

#ifdef _MSC_VER

/* disable stupid warnings about conversions
 */

#pragma warning(disable:4244)
#pragma warning(disable:4305)
/* and about argument conversions (does anyone still use a
   non-prototype compiler?) */
#pragma warning(disable:4761)

#endif

/*
 * Random number generator from Numerical Recipes in C, 2nd Edition
 *
 * Period: 2305842648436451838 (over 9 million years of 8k/sec)
 *
 * Modified:
 *	a) to initialise separately
 *	b) to return 0 <= x < 1, rather than excluding both endpoints
 */

static const long IM1 = 2147483563;
static const long IM2 = 2147483399;
static const long IA1 = 40014;
static const long IA2 = 40692;
static const long IQ1 = 53668;
static const long IQ2 = 52774;
static const long IR1 = 12211;
static const long IR2 = 3791;

/*
 * any value can be used for ipos
 */
void ran2_init(RAN2 *rp, long ipos)
{
 int j;
 if (ipos <= 0) ipos = 1;
 rp->idum1 = ipos;
 rp->idum2 = rp->idum1;
 for (j=RAN2_NTAB+7; j >= 0; j--) {
	long k = rp->idum1 / IQ1;
	rp->idum1 = IA1 * (rp->idum1 - k * IQ1) - k * IR1;
	if (rp->idum1 < 0) rp->idum1 += IM1;
	if (j < RAN2_NTAB) rp->iv[j] = rp->idum1;
 }
 rp->iy = rp->iv[0];
}

/*
 * returns 0 <= x < 1
 */
float ran2(RAN2 *rp)
{
 long k;
 	// Compute idum1 = (IA1 * idum1) % IM1 by Schrage's method
 k = rp->idum1 / IQ1;
 rp->idum1 = IA1 * (rp->idum1 - k * IQ1) - k * IR1;
 if (rp->idum1 < 0) rp->idum1 += IM1;
 	// Compute idum2 = (IA2 * idum2) % IM2 by Schrage's method
 k = rp->idum2 / IQ2;
 rp->idum2 = IA2 * (rp->idum2 - k * IQ2) - k * IR2;
 if (rp->idum2 < 0) rp->idum2 += IM2;
 k = (rp->iy + 1) / (1 + (IM1 - 1) / RAN2_NTAB);
 	// 0 <= k < RAN2_NTAB
 	// 0 < idum1 < IM1
 	// 0 < idum2 < IM2
 rp->iy = rp->iv[k] - rp->idum2;
 rp->iv[k] = rp->idum1;
 if (rp->iy < 0) rp->iy += IM1;
 	// 0 <= iy < IM1
 return rp->iy / (double) IM1;
}
