/* ran2.h - random number generator */

#ifndef INCLUDED_RAN2_H
#define INCLUDED_RAN2_H

#define RAN2_NTAB 32

typedef struct {
	long iy;
	long idum1, idum2;
	long iv[RAN2_NTAB];
} RAN2;

/*
 * any value can be used for ipos
 */
void ran2_init(RAN2 *rp, long ipos);

/*
 * returns 0 <= x < 1
 */
float ran2(RAN2 *rp);

#endif
