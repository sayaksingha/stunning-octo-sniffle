/* prefsuf.h - messages passed between host and sharc for prefix/suffix */

#ifndef INCLUDED_PREFSUF_H
#define INCLUDED_PREFSUF_H

#define MSG_PREFLEN 0xca0
	/*
	 * PREFLEN(length)
	 * sets length of prefix in bits and clears the prefix to all
	 * '1' bits
	 * h->s only
	 */

#define MSG_PREFIX 0xca1
	/*
	 * PREFIX(offset, data)
	 * sets specific data to be used in a prefix
	 * the offset is in multiples of 32-bits
	 * the data is sent least significant bit first
	 * h->s only
	 */

#define MSG_SUFFLEN 0xca2
	/*
	 * SUFFLEN(length)
	 * sets length of suffix in bits and clears the prefix to all
	 * '1' bits
	 * h->s only
	 */

#define MSG_SUFFIX 0xca3
	/*
	 * SUFFIX(offset, data)
	 * sets specific data to be used in a suffix
	 * the offset is in multiples of 32-bits
	 * the data is sent least significant bit first
	 * h->s only
	 */

#define MSG_RTS 0xca4
	/*
	 * RTS(value)
	 * sets RTS indicator. This is needed to make it independent
	 * of the protocol being used.
	 * h->s only
	 */

#endif
