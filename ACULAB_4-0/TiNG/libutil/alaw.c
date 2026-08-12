/* alaw.c : routines for converting linear to A-law and back.	*/

#include "alaw.h"

#ifdef _MSC_VER

/* disable stupid warnings about doubles being converted to floats.
 */

#pragma warning(disable:4244)
#pragma warning(disable:4305)
/* and about argument conversions (does anyone still use a
   non-prototype compiler?) */
#pragma warning(disable:4761)

#endif

/* This routine has been tested against the one which always used to
 * work, and gives an identical table. And is about 10 times
 * quicker (for what it's worth)
 */
void init_alaw_encode(LIN2ALAW_TABLE *atp)
{
    short seg, num, interval, size, i, j;

    i=0;
    j=0;

/*  SEGMENT 1: */
    for(num=0; num<32; num++){
       atp->table[i++] = j;
       atp->table[i++] = j;
       j++;
    }
/*  SEGMENTS 2..7: */
    for(seg=2; seg<=7; seg++){
       size = 1<<seg; 
       for(num=0; num<16; num++){
          for(interval=0; interval<size; interval++){
             atp->table[i++]=j;
          }
          j++;
       }
    }
}

/*  ditto */
void init_alaw_decode(ALAW2LIN_TABLE *atp)
{
    short i, j, seg, num, interval;

/*  SEGMENT 1; */
    j=0;
    i=1;
    interval=2;
    for(num=0; num<32; num++){
       atp->table[j++] = i;
       i += interval;
    }
    for(seg=2; seg<=7; seg++){
       interval = 1<<seg;
       i = (1<<(seg+4)) + interval/2;
       for(num=0; num<16; num++){
          atp->table[j++] = i;
          i += interval;
       } 
    }
}

int alaw_to_linear(unsigned char log, ALAW2LIN_TABLE *atp)
{
	unsigned char pos_flag;
	int lin;

	log = log ^ 0x55;            /* Bitwise XOR - invert even bits back again */
								 /* Sign bit is 1 for positive values */
	pos_flag = log & 0x80;       /* Copy sign bit */
	log = log & 0x7F;            /* Mask off sign bit */

	if (pos_flag)
		lin = atp->table[log];
	else
		lin = -atp->table[log];
	return lin;

} /* log_to_lin */



unsigned char linear_to_alaw(int lin, LIN2ALAW_TABLE *atp)
{
	unsigned char log, pos_flag;

    if(lin>=0){
        pos_flag=0x80;
    }else{
        pos_flag=0;
        lin = -lin;
    }
    if(lin>4095) lin=4095;

	log = pos_flag | atp->table[lin];

	log = log ^ 0x55;                                     /* Invert even bits */
										 /* Sign bit is 1 for positive values */
	return log;

} /* linear_to_log */

unsigned char linear_to_alaw_fn(int lin)
{
 int e = 0;
 int s = lin < 0;
 if (s) lin = -lin;
 for (;;) {
 	lin >>= 1;
 	if (lin < 0x20) break;
 	e += 0x10;
 }
 lin += e;
 if (s) lin |= 0x80;
 return lin;
}
