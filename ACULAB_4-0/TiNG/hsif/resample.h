/* resample.h - messages for sample rate conversion */

#ifndef INCLUDED_RESAMPLE_H
#define INCLUDED_RESAMPLE_H

#define MSG_RESAMPLE_RATIO 0xe30
	/*
	 * RESAMPLE_RATIO(n, d)
	 * sets resampling ratio to n/d
	 *
	 * h->s only
	 */

#define MSG_RESAMPLE_FILTER 0xe31
	/*
	 * RESAMPLE_FILTER(n, gain, a1, b1, b2)
	 * sets one resampling filter stage
	 * n is stage number (0..3)
	 * gain is gain
	 * a1, b1, b2 are IIR coefficients (a2 is always 1)
	 * gain, a1, b1, and b2 are scaled by 0x01000000
	 *
	 * h->s only
	 */

#endif
