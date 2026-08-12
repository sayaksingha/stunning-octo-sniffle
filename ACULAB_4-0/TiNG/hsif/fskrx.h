/* fskrx.h - messages passed between host and sharc for fsk rx */

#ifndef INCLUDED_FSKRX_H
#define INCLUDED_FSKRX_H

#define MSG_FSKRX 0xc93
	/*
	 * FSKRX(shiftre, shiftim, gain, b2, b1, a2, a1, b2, b1, a2, a1)
	 * sets shift frequency as real and imaginary parts and filter.
	 * Each value is scaled by 0x01000000
	 * h->s only
	 */

#define MSG_INVERT 0xc94
	/*
	 * INVERT(inv)
	 * sets whether (non-zero) or not (zero) the result should be inverted
	 * h->s only
	 */

#define MSG_THRESH 0xc95
	/*
	 * THRESH(v)
	 * sets threshold between mark/space and no carrier
	 * h->s only
	 */

#define MSG_RXSIGNAL_STATS 0xc99
	/*
	 * RXSIGNAL_STATS()
	 * h->s reset stats and request signal statistics report on FSKrx stop
	 * RXSIGNAL_STATS(stat)
	 * s->h report signal statistics (measure of FSKRx strength)
	 */


#define MSG_REVERSE_CHAN_FILT 0xc9e
	/*
	 * MSG_REVERSE_CHAN_FILT()
	 * selects prefilter to block opposing duplex V.21 channel
	 * h->s only
	 */


#endif
