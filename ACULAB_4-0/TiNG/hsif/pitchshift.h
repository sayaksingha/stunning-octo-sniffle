/* pitchshift.h - messages passed between host and sharc for pitchshift */

#ifndef INCLUDED_PITCHSHIFT_H
#define INCLUDED_PITCHSHIFT_H

#define MSG_PITCHSHIFT 0xe24
	/*
	 * PITCHSHIFT(amount)
	 * sets the amount of pitch shifting: x=amount/65536 where x < 0
	 * means shift down, so that x=-1 halves the frequency
	 * h->s only
	 */

#endif
