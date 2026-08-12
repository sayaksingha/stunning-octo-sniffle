/* timerx.h - messages passed between host and sharc for record timeout */

#ifndef INCLUDED_TIMERX_H
#define INCLUDED_TIMERX_H

#define MSG_TIMEOUT 0xe22
	/*
	 * TIMEOUT(duration)
	 * sets timeout to use
	 * h->s only
	 *
	 * TIMEOUT()
	 * indicates that the timeout has occurred
	 * s->h only
	 */

#endif
