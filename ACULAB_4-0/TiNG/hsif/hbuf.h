/* hbuf.h - messages used for host buffers */

#ifndef INCLUDED_HBUF_H
#define INCLUDED_HBUF_H

#define MSG_BUF_START 0xe08
	/*
	 * BUF_START(timeL, timeH)
	 * reports the point (in mS) at which the first buffer of data starts
	 * s->h only
	 */

#define MSG_SET_BUFSIZE 0x81e
	/*
	 * SET_BUFSIZE(size_in_octets)
	 * Sets the length of the buffer used by hbuftohost or hbuffromhost.
	 * h->s only.
	 */

#define MSG_BUF_FLUSH 0xc02
	/*
	 * BUF_FLUSH
	 * Used by tasks to flush pending data to the host.
	 * s->h only.
	 */

#endif
