/* fsk.h - messages passed between host and sharc for fsk tx & rx */

#ifndef INCLUDED_FSK_H
#define INCLUDED_FSK_H

#define MSG_CARRIER_FREQ 0xc91
	/*
	 * CARRIER_FREQ(markre, markim, spacere, spaceim)
	 * sets mark and space frequencies, which are specified
	 * as real and imaginary parts. Each part is scaled by 0x80000000
	 * h->s only
	 */

#endif
