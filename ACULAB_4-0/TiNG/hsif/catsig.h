/* catsig.h - messages used by signat categorisation */

#ifndef INCLUDED_CATSIG_H
#define INCLUDED_CATSIG_H

#define MSG_SET_CATSIG_ANS_COEFFS 0xc90
	/*
	 * CATSIG_ANS_COEFFS(ln_nrg_coeff, min_Lmin, Lmin_decay,
	 * 	speech_thresh, debounce [,initial_Lmax])
	 * sets algorithm parameters
	 * all values are fixed point, scaled as follows:
	 *	ln_nrg_coeff	2^14
	 *	min_Lmin	2^2
	 *	Lmin_decay	2^14
	 *	speech_thresh	2^10
	 *	debounce	2^15
	 *	initial_Lmax	2^0 (if 6 parameter msg used rather than 5)
	 * h->s only
	 */

#define MSG_SET_CATSIG_LSD_COUNT 0xc91
	/*
	 * CATSIG_LSD_COUNT(min_valid_period_count, min_valid_count,
	 *	 glitch_count, qualify_count, alter_duration,
	 *	 max_valid_tone_cnt, max_valid_speech_cnt,
	 *	 threshold_samp_cnt, delay_time, period_time,
	 *	 max_off_count, min_period_off)
	 * sets algorithm parameters
	 * all values are integers in mS
	 * h->s only
	 */

#define MSG_CATSIG_DETECTED 0xc92
	/*
	 * CATSIG_DETECTED(type)
	 * type=0 for machine, 1 for live speaker, 2 for tone start, 3 for tone end
	 * s->h only
	 */

#define MSG_CATSIG_PARAM 0xc93
	/*
	 * CATSIG_PARAM(param, value)
	 * sets a catsig parameter
	 * h->s only
	 */

#define CATSIG_P_REPORT_TONES 0

#define MSG_CATSIG_SIGHINT 0xcb0
	/*
	 * MSG_CATSIG_SIGHINT()
	 * provide hint from signalling (pre/post CONNECT)
	 *
	 * h->s only
	 */

#define MSG_CATSIG_TIMEOUT 0xcba
	/*
	 * MSG_CATSIG_TIMEOUT()
	 * set timeout for catsig from final ring (or from start if no ring)
	 *
	 * h->s only
	 */

#endif

