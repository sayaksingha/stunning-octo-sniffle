/* t38.h - messages for T.38  */

#ifndef TiNGK_INCLUDED_HSIF_T38_H
#define TiNGK_INCLUDED_HSIF_T38_H

#define MSG_T38CFG 0xce5
	/*
	 * T38CFG(pc, prot)
	 * Sets the T.38 code configuration. pc is the pre-corrigendum flag,
	 * 0=1998 version, 1=2002 version. prot is the protocol (a value from 
	 * Table 4/T.38 or -1 for all).
	 * h->s only
	 */

#define MSG_T38HDLC 0xce6
	/*
	 * T38HDLC(en)
	 * Selects HDLC (1) or sync data (0).
	 * h->s only
	 */

#define T38TONE_CNG    0
#define T38TONE_CED    1
#define T38TONE_ANSAM  2

#define V34_CC_1200    0
#define V34_PRI_CHAN   1
#define V34_CC_RETRAIN 2

#define MSG_T38TONE 0xce7
	/*
	 * T38TONE(tone)
	 * h->s
	 * Requests that the tone indicator corresponding to 'tone' is sent
	 * s->h
	 * Indicates receipt of a tone indicator corresponding to 'tone'
	 */

#define MSG_T38SRC 0xce8
	/*
	 * T38SRC(address, port)
	 * T38SRC(addrv6[0], addrv6[1], addrv6[2], addrv6[3], port)
	 * s->h
	 * Indicates the source address and port of incoming packets
	 */

#define MSG_T38PROTO 0xce9
	/*
	 * T38PROTO(proto)
	 * the protocol of the following data (if any)
	 * from Table 4/T.38
	 * s->h only
	 */

#define MSG_T38_V34 0xcea
	/*
	 * T38_V34(v34)
	 * h->s
	 * Requests that the v34 indicator corresponding to 'v34' is sent
	 * s->h
	 * Indicates receipt of a v34 indicator corresponding to 'v34'
	 */

#define MSG_T38RATE 0xceb
	/*
	 * T38 V34-rate
	 * s->h only
	 * Requests a primary channel bitrate change
	 */

#define MSG_FMP_RECOVERY 0xe10
	/*
	 * FMP_RECOVERY(n, t)
	 * sets the type of error recovery to use, where n is the number of
	 * secondaries, and t is zero to indicate redundancy is to be used
	 * h->s only
	 */

#define MSG_T38_MAX_PACKET_LEN 0xe12
	/*
	 * MSG_T38_MAX_PACKET_LEN(n, t)
	 * sets the maximum number of octets in the IFP packet
	 * h->s only
	 */

#endif
