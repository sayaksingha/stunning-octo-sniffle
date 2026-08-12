/* sntp.h - messages for sntp */

#ifndef TiNGK_INCLUDED_HSIF_SNTP_H
#define TiNGK_INCLUDED_HSIF_SNTP_H

#define MSG_SNTP_CONFIG 0xcf9
	/*
	 * SNTP_CONFIG(ip, port, TOS, no_reply_delay, had_reply_delay)
	 * SNTP_CONFIG(ipv6[0], ipv6[1], ipv6[2], ipv6[3], port, TOS, no_reply_delay, had_reply_delay)
	 * configures SNTP client
	 * h->s only
	 */

#define MSG_SNTP_SYNC 0xcfa
	/*
	 * SNTP_SYNC(in_sync)
	 * client is in sync with server if in_sync is nonzero
	 * s->h only
	 */

#endif
