#ifndef TiNGK_INCLUDED_HSIF_IPV6_H
#define TiNGK_INCLUDED_HSIF_IPV6_H

#define MSG_SET_INIPV6 0xd05
	/*
	 * SET_INIPV6(ip[0], ip[1], ip[2], ip[3])
	 * configures ip to listen on
	 * h->s only
	 */

#define MSG_SET_OUTIPV6 0xd06
	/*
	 * SET_OUTIPV6(rem-ip[0], rem-ip[1], rem-ip[2], rem-ip[3], rem-port, local-ip[0], local-ip[1], local-ip[2], local-ip[3], local-port)
	 * configures ip/port info.
	 * the address parts are in host order.
	 * h->s only
	 */

#endif
