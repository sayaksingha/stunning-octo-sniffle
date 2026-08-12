/* rtcp.h */
#ifndef TiNGK_INCLUDED_HSIF_RTCP_H
#define TiNGK_INCLUDED_HSIF_RTCP_H

#define MSG_SET_RTCPBW 0xcd2
	/*
	 * SET_RTCPBW (tot, rx, tx)
	 * configures RTCP bandwidth (in bps):
	 *	tot=total, rx=for receivers, tx=for senders
	 * h->s only
	 */

#define MSG_RTCP_GETSTATS 0xcd3
	/*
	 * RTCP_GETSTATS (code)
	 * requests immediate statistics of type 'code'
	 * h->s only
	 */

#define MSG_RTCP_REPORTS 0xcd4
	/*
	 * RTCP_REPORTS(mask)
	 * h->s only
	 * configures the bitmask of which reports to generate automatically
	 */

#define MSG_RTCP_OPTIONS 0xcd5
	/*
	 * RTCP_REPORTS(rtcptx_suppress_xr)
	 * h->s only
	 * control suppression of XR elements in tx RTCP
	 */
	 
#endif
