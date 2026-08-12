#ifndef TiNG_HSIF_SPRT_H
#define TiNG_HSIF_SPRT_H


#define MSG_SPRT_PACKET_DATA 0xe50
	/*
	 * MSG_SPRT_PACKET_DATA([expect_packets])
	 * h->s tell sprt task if packetiser is configured or not
	 * s->h report if expecting packet data type but didn't get it
	 */

#define MSG_SPRT_CONFIG_CHANNEL 0xe51
	/*
	 * MSG_SPRT_CONFIG_CHANNEL(channelno, window_size, TA01, TA02, TR03)
	 * h->s only
	 */

#define MSG_SPRT_CONFIG_DATA 0xe52
	/*
	 * MSG_SPRT_CONFIG_DATA(max_data_size, max_packet_delay)
	 * h->s only
	 */

#define MSG_SPRT_SET_DATA_CHANNEL 0xe53
	/*
	 * MSG_SPRT_SET_DATA_CHANNEL(channel_no)
	 * h->s only
	 */

#define MSG_SPRT_SET_TX_PAYLOAD_TYPE 0xe54
	/*
	 * MSG_SPRT_SET_TX_PAYLOAD_TYPE(payload_type)
	 * h->s only
	 */

#define MSG_SPRT_ERROR_SEND_MESSAGE 1
#define MSG_SPRT_ERROR_SEND_DATA 2

#define MSG_SPRT_ERROR 0xe55
	/*
	 * MSG_SPRT_ERROR(error)
	 * s->h only
	 */

#endif
