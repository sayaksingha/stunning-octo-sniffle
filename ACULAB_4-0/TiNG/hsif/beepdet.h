/* beepdet.h - beep detection messages passed between host and module */

#ifndef INCLUDED_BEEPDET_H
#define INCLUDED_BEEPDET_H

#define MSG_BEEPDET_PARMS 0xe41
	/*
	 * BEEPDET_PARMS(min_tone_length, min_tone_freq, max_tone_freq)
	 * sets the minimum length and the minumum and maximum frequencies 
	 * of the tone
	 * h->s only
	 */

#define MSG_BEEP_DETECTED 0xe42
	/*
	 * BEEP_DETECTED(freq, duration)
	 * tone was detected. duration = 0: started
	 * s->h only
	 */

#endif
