/* g726.h - messages for G.726 codec */

#ifndef TiNGK_INCLUDED_G726_h
#define TiNGK_INCLUDED_G726_h

#define MSG_G726_BITS 0xce0
	/*
	 * h->s
	 * G726_BITS(N)
	 * Configures G.726 to use N bits per sample.
	 */

#define MSG_G726_PACKING_RFC3551 0
#define MSG_G726_PACKING_AAL2 1

#define MSG_G726_PACKING 0xcf8
	/*
	 * G726_AAL(mode)
	 * configures packing
	 * non zero uses AAL2 (little endian)
	 * zero uses big endian
	 * h->s only
	 */

#endif
