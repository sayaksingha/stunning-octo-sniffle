/* mulaw.c : routines for converting linear to A-law and back.	*/

#include "mulaw.h"

void init_mulaw_encode(LIN2MULAW_TABLE *atp)
{
 int step = 1;
 int out = 0;
 int val = 0;
 int pos;
 for (pos=0; pos < 128; pos++) {
	int d = step;
	if ((pos & 15) == 15) {
		d += step / 2;
		step += step;
	}
	val += d;
	while (out < val) {
		atp->table[out++] = pos;
		if (out > 7903) return;

	}
	val += d;
 }
}

void init_mulaw_decode(MULAW2LIN_TABLE *atp)
{
 int step = 2;
 int val = 0;
 int pos = 0;
 int seg;
 for (seg=0; seg < 8; seg++) {
	int i;
	for (i=0; i<16; i++) {
		atp->table[pos++] = val;
		val += step;
	}
	val += step / 2;
	step <<= 1;
 }
}

int mulaw_to_linear(unsigned char log, MULAW2LIN_TABLE *atp)
{
	int pos_flag;
	int lin;
	log ^= 0xff;
				 /* Sign bit is 1 for positive values */
	pos_flag = log & 0x80;       /* Copy sign bit */
	log = log & 0x7F;            /* Mask off sign bit */
	lin = atp->table[log];
	if (pos_flag) return lin;
	else return -lin;
}

unsigned char linear_to_mulaw(int lin, LIN2MULAW_TABLE *atp)
{
 int pos_flag = 0;
 if (lin >= 0) pos_flag=0x80;
 else lin = -lin;
 if (lin > 7903) lin = 7903;
 return (pos_flag | atp->table[lin]) ^ 0xff;
}

unsigned char linear_to_mulaw_fn(int lin)
{
 int e = 0;
 int s = lin < 0;
 if (s) lin = -lin;
 lin += 0x21;
 for (;;) {
 	lin >>= 1;
 	if (lin < 0x20) break;
 	e += 0x10;
 }
 lin = e | (lin & 0xf);
 if (s) lin |= 0x80;
 return 255 - lin;
}
