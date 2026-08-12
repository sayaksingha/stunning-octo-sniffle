/* g723.h - messages for G.723.1 codec */

#ifndef TiNGK_INCLUDED_G723_1_h
#define TiNGK_INCLUDED_G723_1_h

#define MSG_G723_1_FILTER 0xce1
	/*
	 * G723_1_FILTER(en)
	 * configures post filter en=0 disables, en=1 enables
	 * h->s only
	 */
#define MSG_G723_1_HPF 0xce2
	/*
	 * G723_1_HPF(en)
	 * configures high-pass filter en=0 disables, en=1 enables
	 * h->s only
	 */
#define MSG_G723_1_RATE 0xce3
	/*
	 * G723_1_RATE(r)
	 * configures rate. r=6300 or 5300
	 * h->s only
	 */
#define MSG_G723_1_SILCOMP 0xce4
	/*
	 * G723_1_SILCOMP(en)
	 * configures silence compression en=0 disables, en=1 enables
	 * h->s only
	 */

#endif
