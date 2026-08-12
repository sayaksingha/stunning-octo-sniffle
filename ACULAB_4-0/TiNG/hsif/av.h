/* av.h - messages passed between host and sharc to config an AVF payload play or record */

#ifndef INCLUDED_AV_H
#define INCLUDED_AV_H

#define MSG_RECAVF_CFG 0xc10
	/*
	 * RECAVF_CFG(m, n)
	 * m: time to store (will round down to multiple of epoch time)
	 * n: payload type for recorded AVF
	 * h->s only
	 */

#endif
