/* onhool.h - messages for on hook detection */

#ifndef TiNGK_INCLUDED_HSIF_ONHOOK_H
#define TiNGK_INCLUDED_HSIF_ONHOOK_H

#define MSG_ONHOOK_CONFIG 0xe43
	/*
	 * ONHOOK_CONFIG(lowpower, highpower, softpower, latitude, cr, tr, dur)
	 * configures onhook detector
	 * lowpower, highpower, and softpower are linear power with 1034912258.0 as dBm0
	 * latitude, cr and tr are ratios scaled by 65536
	 * dur is in milliseconds
	 * h->s only
	 */

#define MSG_ONHOOK_DETECTED 0xe44
	/*
	 * ONHOOK_DETECTED
	 * onhool state detected
	 * s->h only
	 */

#endif
