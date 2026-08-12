/* tonedet.h - tone detection messages passed between host and sharc */

#ifndef INCLUDED_TONEDET_H
#define INCLUDED_TONEDET_H

#define MSG_DETECT_TONE 0xc83
	/*
	 * DETECT_TONE(code, duration)
	 * tone was detected. 'duration' = 0 at start, non-zero (mS) at end
	 *
	 * s->h only
	 */

#define MSG_SET_GENPAR 0xc84
	/*
	 * SET_GENPAR(3rd_peak, SNR, minpower, twist)
	 * sets the general parameters for tone detection
	 *
	 * h->s only
	 */

#define MSG_SET_DETTONE 0xc85
	/*
	 * SET_DETTONE(set, slot, minfreq, maxfreq)
	 * sets the thresholds for a tone
	 * tones are grouped into tonesets (max 2), within which
	 * are slots (one per tone) 0..N where N+1 tones can be
	 * detected
	 *
	 * h->s only
	 */

#define MSG_SET_HYST 0xc86
	/*
	 * SET_HYST(hyst[,hyst_off])
	 * sets the hyst value which counts the number of 256-sample epochs
	 * that a state must persist for before it is recognised
	 *
	 * h->s only
	 */

#define MSG_TONEBAND 0xc87
	/*
	 * TONEBAND(min, max)
	 * sets the minimum and maximum frequencies of the band examined
	 * for tones
	 *
	 * h->s only
	 */

#define MSG_DETECT_CONT 0xc88
	/*
	 * DETECT_CONT(code, duration)
	 * tone is being detected. This is generated about every second.
	 * The duration is cumulative
	 *
	 * s->h only
	 */

#define MSG_SET_CONT_TIM 0xc89
	/*
	 * SET_CONT_TIM(duration)
	 * Sets the interval at which DETECT_CONT messages should be
	 * produced (in mS).
	 *
	 * h->s only
	 */

#define MSG_TONES_NARROW 0xc8a
	/*
	 * MSG_TONES_NARROW(narrow)
	 * if narrow is non zero then tones with close frequencies
	 * (difference of 62.5 - 93.75 Hz) are to be detected
	 *
	 * h->s only
	 */

#define MSG_ANS_DETECT_TONE 0xc82
	/*
	 * ANS_DETECT_TONE(code [, phase_reversals])
	 * tone was detected. code = 0: nothing, code = 1: ANS, code = 2: ANSam
	 * phase_reversals being non zero means /ANS or /ANSam
	 * s->h only
	 */

#define MSG_SET_TDELIMHYST 0xc8b
	/*
	 * SET_TDELIMHYST(hyst)
	 * sets the hyst value which counts the number of 256-sample epochs
	 * that a state must persist for before it is recognised
	 * (target tone elim path element)
	 * h->s only
	 */

#endif
