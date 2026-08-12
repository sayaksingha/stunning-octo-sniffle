/* h223.h - messages for H.223 mux and demux  */

#ifndef TiNGK_INCLUDED_HSIF_H223_H
#define TiNGK_INCLUDED_HSIF_H223_H

#define MSG_H223CTL 0xce5
	/*
	 * H223CTL(key, val)
	 * Sends a control message to the H223 mux or demux.
	 * h->s only
	 */

#define MSG_NEW_SESSION 0xce6
	/*
	 * NEW_SESSION()
	 * Used to send a create new H.223 session request to the H223 mux/demux
	 * h->s only
	 */

#define MSG_SESSION_STARTED 0xce7
	/*
	 * SESSION_STARTED(level)
	 * Supplies the level negotiated during session startup to the API
	 * s->h only
	 */

#define MSG_CREATE_SESSION_FAILED 0xce8
	/*
	 * SESSION_FAILED()
	 * Passes session startup failure message to the API
	 * s->h only
	 */

#define MSG_SET_CAPS1 0xce9
	/*
	 * SET_CAPS()
	 * Used to send first batch of capabilities to the H.223 mux/demux
	 * Capabilities structure is very big and must be split
	 * h->s only
	 */

#define MSG_SET_CAPS2 0xcea
	/*
	 * SET_CAPS()
	 * Used to send second batch of capabilities to the H.223 mux/demux
	 * Capabilities structure is very big and must be split
	 * h->s only
	 */

#define MSG_SET_CAPS_MOBILE_OP 0xceb
	/*
	 * SET_CAPS()
	 * Used to send third batch of capabilities to the H.223 mux/demux
	 * Capabilities structure is very big and must be split
	 * h->s only
	 */

#define MSG_SET_CAPS_MOBILE_MULTILINK 0xcec
	/*
	 * SET_CAPS()
	 * Used to send fourth batch of capabilities to the H.223 mux/demux
	 * Capabilities structure is very big and must be split
	 * h->s only
	 */

#define MSG_SET_CAPS_ANNEXC 0xced
	/*
	 * SET_CAPS()
	 * Used to send fifth and final batch of capabilities to the H.223 mux/demux
	 * Capabilities structure is very big and must be split
	 * h->s only
	 */

#define MSG_SET_CAPS_CALL 0xcee
	/*
	 * SET_CAPS()
	 * Used to invoke actual set_caps function call when we have sent all
	 * the structure information
	 * h->s only
	 */

#define MSG_SET_CAPS_FAILED 0xcef
	/*
	 * SET_CAPS_FAILED()
	 * Passes set capability failure message to the API
	 * s->h only
	 */

#define MSG_ADD_ENTRY 0xcf0
	/*
	 * ADD_ENTRY()
	 * Used to send add multiplex table entry request to the H.223 mux/demux
	 * h->s only
	 */

#define MSG_ADD_ENTRY_FAILED 0xcf1
	/*
	 * ADD_ENTRY_FAILED()
	 * Passes add multiplex table entry failure message to the API
	 * s->h only
	 */

#define LC_DIR_TX 0
#define LC_DIR_RX 1

#define LC_TYPE_UNI 0
#define LC_TYPE_BI 1

#define LC_ADAPT_TYPE_NONSTD 0
#define LC_ADAPT_TYPE_AL1FRM 1
#define LC_ADAPT_TYPE_AL1NOF 2
#define LC_ADAPT_TYPE_AL2NOS 3
#define LC_ADAPT_TYPE_AL2SEQ 4
#define LC_ADAPT_TYPE_AL3    5

#define LC_TX_MARK_UNIT_BYTES 0
#define LC_TX_MARK_UNIT_ALSDU 1

#define MSG_OPEN_LC1 0xcf2
	/*
	 * OPEN_LC()
	 * Used to send open logical channel request to the H.223 mux/demux
	 * Sent in batches due to parameter limitation of 14 32bit words
	 * h->s only
	 */

#define MSG_OPEN_LC2 0xcf3
	/*
	 * OPEN_LC()
	 * Used to send open logical channel request to the H.223 mux/demux
	 * h->s only
	 */

#define MSG_OPEN_LC_FAILED 0xcf4
	/*
	 * OPEN_LC_FAILED()
	 * Passes open logical channel failure message to the API
	 * s->h only
	 */

#define MSG_CLOSE_LC 0xcf5
	/*
	 * CLOSE_LC()
	 * Used to send close logical channel request to the H.223 mux/demux
	 * h->s only
	 */

#define MSG_CLOSE_LC_FAILED 0xcf6
	/*
	 * CLOSE_LC_FAILED()
	 * Passes close logical channel failure message to the API
	 * s->h only
	 */

#define MSG_DELETE_SESSION 0xcf7
	/*
	 * DELETE_SESSION()
	 * Used to send a delete H.223 session request to the H.223 mux/demux
	 * h->s only
	 */

#define MSG_DELETE_SESSION_FAILED 0xcf8
	/*
	 * DELETE_SESSION_FAILED()
	 * Passes delete H.223 session failure message to the API
	 * s->h only
	 */

#define MSG_RECONFIG 0xcf9
	/*
	 * RECONFIG()
	 * Used to reconfigure the H.223 mux/demux
	 * h->s only
	 */

#define MSG_RECONFIG_LEVEL_CHANGE 0xcfa
	/*
	 * RECONFIG_LEVEL_CHANGE()
	 * Passes reconfigure level change notification message to the API
	 * s->h only
	 */

#define MSG_RECONFIG_FAILED 0xcfb
	/*
	 * RECONFIG_FAILED()
	 * Passes reconfigure failure message to the API
	 * s->h only
	 */

#define MSG_H223_INTERNAL_ERROR 0xcfc
	/*
	 * INTERNAL_ERROR()
	 * Passes internal error message to the API
	 * s->h only
	 */

#define MSG_H223_PFRAME_DROP 0xcfd
	/*
	 * INTERNAL_ERROR()
	 * Passes video P-frame dump notification message to the API
	 * s->h only
	 */

#define MSG_H223_PFRAME_SEND 0xcfe
	/*
	 * INTERNAL_ERROR()
	 * Passes video P-frame send resumed notification message to the API
	 * s->h only
	 */

/* defines for optional parameters bitmask */
#define H223MUX_H245_CAP_O_MOBILE_OPERATION_TRANSMIT_CAP	0x04
#define H223MUX_H245_CAP_O_ANNEX_C							0x08
#define H223MUX_H245_CAP_O_BIT_RATE							0x10
#define H223MUX_H245_CAP_O_MOBILE_MULTILINK_FRAME_CAP		0x20

#endif
