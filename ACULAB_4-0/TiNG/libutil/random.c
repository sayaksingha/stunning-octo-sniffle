/* random.c - random number generation */

/* modified into a library from mt19937ar.c at
 * <URL: http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/CODES/mt19937ar.tgz >
 *
 */

#include "random.h"

/* 
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Before using, initialize the state by using init_randmt(seed)  
   or init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


   Any feedback is very welcome.
   http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
*/

/* Period parameters */  
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

/* initializes mt[RANDMT_N] with a seed */
void randmt(RANDMT *rp, unsigned long s)
{
	unsigned long pv;	// previous value during loop
	int mti;
	pv = s & 0xffffffffUL;
	rp->mt[0] = pv;
	for (mti=1; mti<RANDMT_N; mti++) {
			// assert: pv == rp->mt[mti - 1]
		pv = (1812433253UL * (pv ^ (pv  >>  30)) + mti); 
		/* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
		/* In the previous versions, MSBs of the seed affect   */
		/* only MSBs of the array mt[].                        */
		/* 2002/01/09 modified by Makoto Matsumoto             */
		pv &= 0xffffffffUL;
		/* for >32 bit machines */
		rp->mt[mti] = pv;
	}
	rp->mti = mti;
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
/* slight change for C++, 2004/2/26 */
void randmt_by_array(RANDMT *rp, unsigned long init_key[], int key_length)
{
    int i, j, k;
    randmt(rp, 19650218UL);
    i=1; j=0;
    k = (RANDMT_N>key_length ? RANDMT_N : key_length);
    for (; k; k--) {
        rp->mt[i] = (rp->mt[i] ^ ((rp->mt[i-1] ^ (rp->mt[i-1]  >>  30)) * 1664525UL))
          + init_key[j] + j; /* non linear */
        rp->mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++; j++;
        if (i>=RANDMT_N) { rp->mt[0] = rp->mt[RANDMT_N-1]; i=1; }
        if (j>=key_length) j=0;
    }
    for (k=RANDMT_N-1; k; k--) {
        rp->mt[i] = (rp->mt[i] ^ ((rp->mt[i-1] ^ (rp->mt[i-1]  >>  30)) * 1566083941UL))
          - i; /* non linear */
        rp->mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++;
        if (i>=RANDMT_N) { rp->mt[0] = rp->mt[RANDMT_N-1]; i=1; }
    }

    rp->mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */ 
}

/* generates a random number on [0,0xffffffff]-interval */
unsigned long randmt_int32(RANDMT *rp)
{
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (rp->mti >= RANDMT_N) { /* generate RANDMT_N words at one time */
        int kk;
        for (kk=0;kk<RANDMT_N-M;kk++) {
            y = (rp->mt[kk]&UPPER_MASK)|(rp->mt[kk+1]&LOWER_MASK);
            rp->mt[kk] = rp->mt[kk+M] ^ (y  >>  1) ^ mag01[y & 0x1UL];
        }
        for (;kk<RANDMT_N-1;kk++) {
            y = (rp->mt[kk]&UPPER_MASK)|(rp->mt[kk+1]&LOWER_MASK);
            rp->mt[kk] = rp->mt[kk+(M-RANDMT_N)] ^ (y  >>  1) ^ mag01[y & 0x1UL];
        }
        y = (rp->mt[RANDMT_N-1]&UPPER_MASK)|(rp->mt[0]&LOWER_MASK);
        rp->mt[RANDMT_N-1] = rp->mt[M-1] ^ (y  >>  1) ^ mag01[y & 0x1UL];

        rp->mti = 0;
    }
  
    y = rp->mt[rp->mti++];

    /* Tempering */
    y ^= (y  >>  11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y  >>  18);

    return y;
}

/* generates a random number on [0,0x7fffffff]-interval */
long randmt_int31(RANDMT *rp)
{
    return (long)(randmt_int32(rp) >> 1);
}

#ifndef TiNGTYPE_STARCORE
/* generates a random number on [0,1]-real-interval */
double randmt_real1(RANDMT *rp)
{
    return randmt_int32(rp) * (1.0/4294967295.0); 
    /* divided by 2^32-1 */ 
}

/* generates a random number on [0,1)-real-interval */
double randmt_real2(RANDMT *rp)
{
    return randmt_int32(rp) * (1.0/4294967296.0); 
    /* divided by 2^32 */
}

/* generates a random number on (0,1)-real-interval */
double randmt_real3(RANDMT *rp)
{
    return (randmt_int32(rp) + 0.5) * (1.0/4294967296.0); 
    /* divided by 2^32 */
}

/* generates a random number on [0,1) with 53-bit resolution*/
double randmt_res53(RANDMT *rp) 
{ 
    unsigned long a = randmt_int32(rp) >> 5;
    unsigned long b = randmt_int32(rp) >> 6; 
    return (a * 67108864.0 + b) * 1.0 / 9007199254740992.0; 
} 
/* These real versions are due to Isaku Wada, 2002/01/09 added */
#endif
