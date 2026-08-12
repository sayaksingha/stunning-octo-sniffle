/* emph.h - emph detection messages passed between host and sharc */

#ifndef INCLUDED_EMPH_H
#define INCLUDED_EMPH_H

#define EMPH_MODE_HF_DEEMPHASIS 0
#define EMPH_MODE_HF_EMPHASIS 1

#define MSG_EMPH_SETMODE 0xc97
	/*
	 * EMPH_SETMODE(mode, gain) 
	 * mode selects emphasis/deemphasis
	 * gain is extra gain to apply after filter
	 *
	 * h->s only
	 */

#endif
