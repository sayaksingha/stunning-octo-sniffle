/* g729.h - messages for G.729 codec */

#ifndef TiNGK_INCLUDED_G729_1_h
#define TiNGK_INCLUDED_G729_1_h

#define MSG_G729I_DTX 0xced
	/*
	 * G729I_DTX(en)
	 * configures DTX en=0 disables, en=1 enables
	 * h->s only
	 */

#define MSG_G729I_RATE_G729D 0
#define MSG_G729I_RATE_G729 1
#define MSG_G729I_RATE_G729E 2

#define MSG_G729I_RATE 0xcee
	/*
	 * G729_1_HPF(rate)
	 * configures rate: 0=G.729D, 1=G.729, 2=G.729E
	 * h->s only
	 */

#endif
