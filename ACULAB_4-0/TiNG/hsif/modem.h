/* modem.h - messages passed between host and sharc for modems */

#ifndef INCLUDED_MODEM_H
#define INCLUDED_MODEM_H

#define MSG_CARRIER 0xc92
	/*
	 * CARRIER(status [, tep [, shorttrain]])
	 * sets (tx) or reports (rx) carrier status.
	 * h->s: power level in dBm0 (-128 = off)
	 *	optional: set talker echo protection (1) or not (0)
	 *	optional: set short training sequence (1) or not (0)
	 * s->h: 0=off, 1=on.
	 */

#define MSG_CARRIER_POWER 0xc96
	/*
	 * CARRIER_POWER(value)
	 * sets default carrier power
	 * this is the power used when carrier comes on automatically
	 * (i.e. when data is supplied with RTS off)
	 * h->s only: power level in dBm0
	 */

#define MSG_CDTIME 0xc94
	/*
	 * CDTIME(on, off)
	 * sets carrier response times in milliseconds
	 * h->s only
	 */

#endif
