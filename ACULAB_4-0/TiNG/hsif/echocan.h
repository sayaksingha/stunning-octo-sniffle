/* echocan.h - messages passed between host and sharc for echo cancellation */

#ifndef INCLUDED_ECHOCAN_H
#define INCLUDED_ECHOCAN_H

#define MSG_SET_REFTS 0xe13
	/*
	 * SET_REFTS(timeslot)
	 * sets the timeslot on which the reference is available
	 * h->s only
	 */

#define MSG_EC_REINIT 0xe14
	/*
	 * EC_REINIT()
	 * re-initialises the echo canceller
	 * h->s only
	 */

#define MSG_SET_AGC 0xe15
	/*
	 * SET_AGC(state)
	 * enable (1) or disable (0) AGC
	 * h->s only
	 */

#define MSG_SET_FIX 0xe16
	/*
	 * SET_FIX()
	 * enable (1) or disable (0) changes in gain
	 * h->s only
	 */

#define MSG_SET_NLP 0xe17
	/*
	 * SET_NLP()
	 * enable (non zero) or disable (0) non-linear processing
	 * value is floor power level with 0dB as 2853*2853*16 (alaw 0dBm0 peak squared * 16)
	 * h->s only
	 */

#define MSG_SET_REF_DELAY 0xe18
	/*
	 * SET_REF_DELAY(delay_ms)
	 * Sets the amount of delay to add to the reference before processing
	 * h->s only
	 */

#define MSG_SET_EC_TONE_ACTION 0xe1a
	/*
	 * SET_EC_TONE_ACTION(tone_action)
	 * Sets the tones to detect and action to take on encountering them (just TTY=1 currently)
	 * h->s only
	 */

#endif
