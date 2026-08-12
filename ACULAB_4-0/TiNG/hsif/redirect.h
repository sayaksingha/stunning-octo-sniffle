#ifndef TiNGK_INCLUDED_REDIRECT_H
#define TiNGK_INCLUDED_REDIRECT_H

#define MSG_REDIRECT_TX_CONFIG 0xca5
	/*
	 * h->s
	 * MSG_REDIRECT_TX_CONFIG(threshold, timeout_ms)
	 */

#define MSG_REDIRECT_OVERRUN 0xca6
	/*
	 * s->h
	 * MSG_REDIRECT_OVERRUN
	 */

#endif
