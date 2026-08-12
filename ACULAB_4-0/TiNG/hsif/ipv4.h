#ifndef TiNGK_INCLUDED_HSIF_IPV4_H
#define TiNGK_INCLUDED_HSIF_IPV4_H

#define MSG_SET_INIPV4 0xcc0
	/*
	 * SET_INIPV4(ip)
	 * configures ip to listen on
	 * h->s only
	 */

#define MSG_SET_OUTIPV4 0xcc1
	/*
	 * SET_OUTIPV4(rem-ip, rem-port, local-ip, local-port)
	 * configures ip/port info.
	 * the addresses are in network byte order.
	 * h->s only
	 */

#endif
