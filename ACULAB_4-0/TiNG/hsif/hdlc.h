/* hdlc.h - messages passed between host and sharc for hdlc tx & rx */

#ifndef INCLUDED_HDLC_H
#define INCLUDED_HDLC_H

#define MSG_SET_CRC 0xc8b
	/*
	 * SET_CRC(poly)
	 * sets CRC polynomial
	 * h->s only
	 */

#define MSG_SET_MIN_HDLC_FLAGS 0xc8e
	/*
	 * SET_MIN_HDLC_FLAGS(flags)
	 * set the minimum number of consecutive flags to be received before
	 * frames can be received, reset when idle/junk
	 * h->s only
	 */

#define MSG_SET_MIN_ONES 0xc9c
	/*
	 * SET_MIN_ONES(count)
	 * set the minimum number of consecutive ones to be received before
	 * reporting idle
	 * h->s only
	 */

#endif
