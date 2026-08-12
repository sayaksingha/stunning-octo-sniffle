/* tonegen.h - tone generation messages passed between host and sharc */

#ifndef INCLUDED_TONEGEN_H
#define INCLUDED_TONEGEN_H

#define MSG_TONEGEN_MARK 0xc90
	/*
	 * TONEGEN_MARK()
	 * marker was encountered in tone specification list
	 *
	 * s->h only
	 */

#define MSG_TONE_REVERSE 0xc8f
	/*
	 * TONE_REVERSE(period)
	 * reverse phase of modulated tones everry 'period' milliseconds
	 *
	 * h->s only
	 */

#endif
