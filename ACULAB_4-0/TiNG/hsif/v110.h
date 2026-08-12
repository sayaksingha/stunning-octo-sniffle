/* v110.h - messages passed between host and sharc for V110 tx & rx */

#ifndef INCLUDED_V110_H
#define INCLUDED_V110_H

#define MSG_SET_ASYNC 0xc86
	/*
	 * SET_ASYNC(async)
	 * sets data format: 0=sync, 1=async
	 * h->s only
	 */

#define MSG_SET_SPEED 0xc87
	/*
	 * SET_SPEED(speed)
	 * sets speed in bits per second
	 * h->s only
	 */

#define MSG_V110_PATTERN 0xc88
	/*
	 * V110_PATTERN(pat)
	 * sets a pettern to watch for, which triggers completion of
	 * a ReadFile when detected. pat is the pattern
	 * h->s: only
	 */

#define MSG_V110_NEW_AUX 0xc89
	/*
	 * V110_NEW_AUX(sbits)
	 * reports that new S/X/E-bits have been received
	 * refers to DTX bit for RLP
	 * s->h only
	 */

#define MSG_V110_SYNC 0xc8a
	/*
	 * V110_SYNC(status)
	 * reports new synchronisation status.
	 *	0=synced
	 *	1=lost sync
	 * s->h only
	 */
#define MSG_V110_SYNC_LOST 0
#define MSG_V110_SYNC_OK 1


#endif
