/* async.h - messages passed between host and sharc for async tx & rx */

#ifndef INCLUDED_ASYNC_H
#define INCLUDED_ASYNC_H

#define MSG_STOPBITS 0xc8c
	/*
	 * STOPBITS(numbits)
	 * sets number of extra stop bits (i.e. 0 = 1 stop bit)
	 * h->s only
	 */

#define MSG_DATABITS 0xc8d
	/*
	 * DATABITS(numbits)
	 * sets number of data bits
	 * h->s only
	 */

#endif
