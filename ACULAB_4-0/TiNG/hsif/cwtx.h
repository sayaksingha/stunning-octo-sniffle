/* cw.h - messages passed between host and sharc for cw tx & rx */

#ifndef INCLUDED_CW_H
#define INCLUDED_CW_H

#define MSG_CARRIER_FREQ 0xc91
	/*
	 * CARRIER_FREQ(re, im)
	 * sets carrier frequency, which is specified
	 * as real and imaginary parts. Each part is scaled by 0x80000000
	 * h->s only
	 */

#endif
