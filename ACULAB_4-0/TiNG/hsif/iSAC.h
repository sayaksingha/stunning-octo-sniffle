/* iSAC.h - messages for iSAC codec */

#ifndef TiNGK_INCLUDED_iSAC_h
#define TiNGK_INCLUDED_iSAC_h

#define MSG_iSAC_RATE 0xcef
	/*
	 * iSAC_RATE(rate, ptime)
	 * configures iSAC codec target bitrate. rate=bits/second,
	 *	ptime=time represented by each packet (30 or 60)
	 * h->s only
	 */

#define MSG_iSAC_CAM 0xcf4
	/*
	 * iSAC_CAM(use_cam)
	 * configures iSAC codec channel associated mode
	 *	use_cam is zero to not use channel associated mode
	 * h->s only
	 */

#endif
