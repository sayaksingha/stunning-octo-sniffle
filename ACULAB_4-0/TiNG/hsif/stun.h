/* stun.h - messages for stun */

#ifndef TiNGK_INCLUDED_HSIF_STUN_H
#define TiNGK_INCLUDED_HSIF_STUN_H

#define STUN_ROLE_LITE 1
#define STUN_ROLE_CONTROLLED 2
#define STUN_ROLE_CONTROLLING 3

#define MSG_STUN_CONFIG 0xd0c
	/*
	 * STUN_CONFIG(role)
	 * STUN_CONFIG(role, tiebreaker[high], tiebreaker[low])
	 * configures STUN
	 * h->s only
	 */

/* stun string message types */
#define STUN_STRING_LOCAL_UFRAG 0
#define STUN_STRING_LOCAL_PWD 1
#define STUN_STRING_REMOTE_UFRAG 2
#define STUN_STRING_REMOTE_PWD 3

#define MSG_STUN_SEND_BIND 0xd0d
	/*
	 * STUN_SEND_BIND(use_rtcp_port, trans_id[0], trans_id[1], trans_id[2], priority, use_candidate, port, ipv4)
	 * STUN_SEND_BIND(use_rtcp_port, trans_id[0], trans_id[1], trans_id[2], priority, use_candidate, port, ipv6[0], ipv6[1], ipv6[2], ipv6[3])
	 * send STUN bind request
	 * h->s only
	 */


#define MSG_STUN_RECV_BIND 0xd0e
	/*
	 * STUN_RECV_BIND(rx_port_RTCP, request_role, priority, use_candidate, port, ipv4)
	 * STUN_RECV_BIND(rx_port_RTCP, request_role, priority, use_candidate, port, ipv6[0], ipv6[1], ipv6[2], ipv6[3])
	 * a STUN bind request was received and responded to
	 * s->h only
	 */

#define STUN_BIND_RESULT_SUCCESS 0
#define STUN_BIND_RESULT_ERROR   1
#define STUN_BIND_RESULT_INVALID 2
#define STUN_BIND_RESULT_TIMEOUT 3

#define MSG_STUN_BIND_RESULT 0xd0f
	/*
	 * STUN_BIND_RESULT(rx_port_RTCP, trans_id[0], trans_id[1], trans_id[2], result, error_code, request_role, port, ipv4)
	 * STUN_BIND_RESULT(rx_port_RTCP, trans_id[0], trans_id[1], trans_id[2], result, error_code, request_role, port, ipv6[0], ipv6[1], ipv6[2], ipv6[3])
	 * reports the result of a STUN bind request
	 * s->h only
	 */



#endif
