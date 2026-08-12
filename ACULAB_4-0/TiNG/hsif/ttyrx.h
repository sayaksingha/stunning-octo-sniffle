/* ttyrx.h - messages passed between host and sharc for tty rx */

#ifndef INCLUDED_TTYRX_H
#define INCLUDED_TTYRX_H

#define MSG_TTYASYRX_BITLN 0xd0a
	/*
	 * MSG_TTYASYRX_BITLN(bitlen)
	 * reports the length of bit received in milliseconds
	 * s->h only
	 */

#define MSG_TTYASYRX_BAUDOT_MODE 0xd0b
	/*
	 * MSG_TTYASYRX_BAUDOT_MORE(mode)
	 * sets the current baudot mode to LTRS if mode is zero, FIGS otherwise
	 * h->s only
	 */


#endif
