/*vmp.h*/
#ifndef TiNGK_INCLUDED_HSIF_VMP_H
#define TiNGK_INCLUDED_HSIF_VMP_H

// include ipv4.h here for compatibility
#include "ipv4.h"

#define MSG_SET_RTCPOUTIPV4 0xcc2
	/*
	 * SET_RTCPOUTIPV4(rem-ip, rem-port, local-ip, local-port)
	 * configures ip/port info. for RTCP of VMP[out]
	 * the addresses are in network byte order.
	 * h->s only
	 */

#define MSG_SET_RTCPOUTIPV6 0xd07
	/*
	 * SET_RTCPOUTIPV6(rem-ip[0], rem-ip[1], rem-ip[2], rem-ip[3], rem-port, local-ip[0], local-ip[1], local-ip[2], local-ip[3], local-port)
	 * configures ip/port info. for RTCP of VMP[out]
	 * the address parts are in host order.
	 * h->s only
	 */

#define MSG_JBCREATE 0xcc8
    /* 
	 * JBCREATE(junk, initial, max)
	 * tells the vmprx to create a jitter buffer with sizes (in mS)
	 * as specified. The 'junk' parameter is ignored.
	 * h->s only
	 */
#define MSG_VAD_MODE 0xcc9
	/* 
	 * VAD_MODE(mode)
	 * Tells the vmptx which vad mode to use
	 * h->s only
	 */
#define RTP_VAD_MODE_DISABLED 0
#define RTP_VAD_MODE_ENABLED 1
#define RTP_VAD_MODE_COMFORT_NOISE 2

#define MSG_ADDVMPSUB 0xcd9
	/* ADDVMPSUB(typecode)
	 * Creates a subtask
	 * h->s only
	 */

#define MSG_TIMESTAMPRATE 0xcdc
	/*
	 * TIMESTAMPRATE(rate)
	 * sets the RTP timestamp rate
	 * passing rate==0 means default to timestamp rate matching sample rate
	 * h->s only
	 */

#define MSG_CONFIG_CODEC 0xcd8
	/* CONFIG_CODEC(typecode, payload_type_mapping[[, plc_mode], ptime_ms])
	 * Gives a VMP[rx/tx] codec its parameters (post-Px300)
	 * h->s only
	 */

#define MSG_ADD_CODEC 0xcca
	/*
	 * ADD_CODEC(codec, payload_type_mapping)
	 * Tells the VMP[rx/tx] to use the specified 'codec' and to map it
	 * to the value 'payload_type_mapping'
	 * h->s only
	 */
#define RTP_CODEC_TYPE_UNKN 0
#define RTP_CODEC_TYPE_ALAW 1
#define RTP_CODEC_TYPE_ULAW 2
#define RTP_CODEC_TYPE_G729 3
#define RTP_CODEC_TYPE_2833 4
#define RTP_CODEC_TYPE_COMFORT_NOISE 5
#define RTP_CODEC_TYPE_G726 6
#define RTP_CODEC_TYPE_G723_1 7
#define RTP_CODEC_TYPE_ILBC 8
#define RTP_CODEC_TYPE_GSMFR 9
#define RTP_CODEC_TYPE_G728 10
#define RTP_CODEC_TYPE_RFC4040 11
#define RTP_CODEC_TYPE_AMRNB 12
#define RTP_CODEC_TYPE_EVRC 13
#define RTP_CODEC_TYPE_GSMEFR 14
#define RTP_CODEC_TYPE_SMV 15
#define RTP_CODEC_TYPE_G729I 16
#define RTP_CODEC_TYPE_MELPe 17
#define RTP_CODEC_TYPE_ISAC 18
#define RTP_CODEC_TYPE_L8 19
#define RTP_CODEC_TYPE_L16 20
#define RTP_CODEC_TYPE_AMRWB 21
#define RTP_CODEC_TYPE_TETRA 22
#define RTP_CODEC_TYPE_SSE 23
#define RTP_CODEC_TYPE_SPEEX 24
#define RTP_CODEC_TYPE_G722 25
#define RTP_CODEC_TYPE_G722_1 26
#define RTP_CODEC_TYPE_SILK 27
#define RTP_CODEC_TYPE_OPUS 28
#define RTP_CODEC_TYPE_GSMHR 29

#define MSG_GET_UDPPORTS 0xccb
	/*
	 * h->s
	 * GET_UDPPORTS()
	 * Requests the UDP ports in use by a VMP[rx]
	 * s->h
	 * GET_UDPPORTS(rtp_port, rtcp_port, address)
	 * GET_UDPPORTS(rtp_port, rtcp_port, addrv6[0], addrv6[1], addrv6[2], addrv6[3])
	 * GET_UDPPORTS(t38_port, address)
	 * GET_UDPPORTS(t38_port, addrv6[0], addrv6[1], addrv6[2], addrv6[3])
	 * Informs the host of the UDP ports that are in use by a VMP[rx]
	 * the address is in network byte order.
	 */

#define MSG_VMPRX_DATALOSS 0xccc
	/*
	 * h->s
	 * VMPRX_DATALOSS(maxloss)
	 * Sets the maximum loss in mS, or zero to disable loss reporting
	 * s->h
	 * VMPRX_DATALOSS(losstime)
	 * Reports start or end of packet loss. losstime is zero to report
	 * the start of a loss, non-zero to report arrival of first packet
	 * after a period of loss (or first packet after construction)
	 */

#define MSG_ILBC_FRAME_LENGTH 0xce5
	/*
	 * h->s
	 * MSG_ILBC_FRAME_LENGTH(N)
	 * Configures iLBC to use N milliseconds per frame.
	 */

#define MSG_ILBC_USE_ENHANCER 0xce6
	/*
	 * h->s
	 * MSG_ILBC_USE_ENHANCER(enh)
	 * Turns iLBC enhancer on/off.
	 */

#define MSG_G728_FILTER 0xce7
	/*
	 * G728_FILTER(en)
	 * configures post filter en=0 disables, en=1 enables
	 * h->s only
	 */

#define MSG_G728_RATE 0xce8
	/*
	 * G728_RATE(r)
	 * configures rate. r=9600, 12800 or 16000
	 * h->s only
	 */

#define MSG_GSMFR_MODE 0xce9
	/*
	 * GSMFR_MODE(m)
	 * configures mode. m=0 standard RFC3551 section 4.5.8.1 bit packing
	 * m= 1 MS-GSM bit packing
	 * h->s only
	 */

#define MSG_EVRC_FILTER 0xcea
	/*
	 * EVRC_FILTER(en[, noise_supr])
	 * configures filters: post-filter for decoder, high-pass filter and
	 * noise suppression for encoder
	 * h->s only
	 */

#define EVRC_RTP_HEADERLESS  0
#define EVRC_RTP_RFC2658     1
#define EVRC_RTP_RFC3558     2

#define MSG_EVRC_RTP_MODE 0xceb
	/*
	 * EVRC_RTP_MODE(m)
	 * configures mode.
	 * h->s only
	 */

#define EVRC_RATE_FULL   4
#define EVRC_RATE_HALF   3
#define EVRC_RATE_EIGHTH 1

#define MSG_EVRC_RATE 0xcec
	/*
	 * EVRC_RATE(min, max)
	 * configures bitrate.
	 * h->s only
	 */

#define MSG_CONFTONE_RX 0xccd
	/*
	 * CONFTONE_RX(payload_mapping, regen_tones)
	 * Supplies configuration information to a VMP[rx] for use with tone regeneration
	 * h->s only
	 */

#define MSG_CONFTONE_ENFORCE 0xcdb
	/* 
	 * CONFTONE_ENFORCE()
	 * Enables or disables enforcement of 40-50ms silence between regenereated tones
	 * h -> s only
	 */
#define CODEC_PLC_DISABLE 0
#define CODEC_PLC_ENABLE  1

#define MSG_CONFTONE_TX 0xcce
	/*
	 * CONFTONE_TX(payload_mapping, convert_tones, elim_tones)
	 * Supplies configuration information to a VMP[tx] for use with tone conversion / elimination
	 * h->s only
	 */

#define MSG_VMP_RX_DET_TONE 0xccf
	/*
	 * VMP_RX_DET_TONE(digit)
	 * Suppl;ies to detection information to the api from the vmprx firmware
	 * s -> h only
	 */
#define MSG_TONE_PARAMS 0xcd0
	/*
	 * MSF_TONE_PARAMS(on, off)
	 * used to set the on/off time for sequencies of outgoing rfc2833 tones.
	 * h -> s only
	 */

#define MSG_TONE_GEN 0xcd1
	/*
	 * MSG_TONE_GEN(tone, volume)
	 * used to generate a digit, tone is the rfc2833 id of the tone +1, 0 means silence,
	 * and -1 causes the firmware to send an event, eg completion
	 * h->s used to generatte tone,
	 * MSG_TONE_GEN()
	 * used to signal completion of tonegeneration.
	 * s->h
	 */

#define MSG_TONE_GEN_ABORT 0xcda
	/*
	 * MSG_TONE_GEN_ABORT
	 * abort all current and pending rfc2833 tone packet generation
	 * (doesn't affect detection of inband DTMF)
	 * h->s only
	 */



#define MSG_SET_TOS 0xcd2
	/*
	 * SET_TOS(TOS_RTP, RTCP_port, TOS_RTCP)
	 * configures TOS fields in RTP and RTCP packets
	 * RTCP_port is DEPRECATED - MSG_SET_RTCPOUTIPV4 sets RTCP ports and addresses
	 * h->s only
	 */

#define MSG_RTP_SSRC 0xcd3
	/*
	 * RTP_SSRC(ssrc, [addr, port])
	 * RTP_SSRC(ssrc, [addrv6[0], addrv6[1], addrv6[2], addrv6[3], port])
	 * reports that a SSRC is being used
	 * addr is in little endian host's network byte order (ie. not network order or host order on big endian)
	 * the addrv6 parts are in host order (rather than the silly and confusing way ipv4 is done)
	 * and addrv6[0] is the first part of the address, so addrv6[0] and addrv6[1] are the prefix
	 */

#define MSG_HMAC_TAG_LEN 0xcd4
	/*
	 * HMAC_TAG_LEN(len)
	 * sets the length in bits of the authenication tag to be used
	 * h->s only
	 */

#define MSG_AMRNB_MODE 0xcd5
	/*
	 * MSG_AMRNB_MODE(speed)
	 * sets the AMR-NB codec speed in bps
	 * h->s only
	 */

#define MSG_AMRNB_ALIGN 0xcd6
	/*
	 * MSG_AMRNB_ALIGN(packet_alignment)
	 * sets the packet alignment bool to yes or no
	 * h->s only
	 */

#define MSG_AMRNB_CMR 0xcf6
	/*
	 * MSG_AMRWB_CMR(cmr)
	 * for tx sets the cmr on subsequent packets to the nearest bit rate in bps
	 * for rx sets reporting of CMR changes if cmr is non-zero
	 * h->s only
	 */

#define MSG_AMRNB_GSMEFR 0xcd7
	/*
	 * MSG_AMRNB_GSMEFR (gsmefr_mode)
	 * do gsmefr equivalent encode/decode in AMR-NB
	 * h->s only
	 */

#define MSG_NEW_CSRCS 0xcf0
	/*
	 * MSG_NEW_CSRC (num_csrc)
	 * start new csrc list replacing current when num_csrc have been added
	 * h->s only
	 */

#define MSG_ADD_CSRC 0xcf1
	/*
	 * MSG_ADD_CSRC (csrc)
	 * add csrc to current list
	 * h->s only
	 */

#define MSG_VMPRX_PAYLOAD 0xcf2
	/*
	 * s->h
	 * MSG_VMPRX_PAYLOAD ( payload id )
	 * reports the payload id of an unhandled packet
	 *
	 * h->s
	 * MSG_VMPRX_PAYLOAD ( delay )
	 * how long to wait between reporting unhandled payloads in ms
	 * a -1 delay means disable reporting
	 */

#define MSG_CSRCS_CONFIG 0xcf3
	/*
	 * MSG_CSRCS_CONFIG (repeat_delay, burst count, [exclude flag, excluded ssrc])
	 * configure when to include CSRCs in packet
	 * h->s only
	 */

#define MSG_AMRWB_MODE 0xcf4
	/*
	 * MSG_AMRWB_MODE(bitrate)
	 * sets the AMR-WB codec bitrate in bps
	 * h->s only
	 */

#define MSG_AMRWB_ALIGN 0xcf5
	/*
	 * MSG_AMRWB_ALIGN(packet_alignment)
	 * sets the packet alignment bool to yes or no
	 * h->s only
	 */

#define MSG_AMRWB_CMR 0xcf7
	/*
	 * MSG_AMRWB_CMR(cmr)
	 * for tx sets the cmr on subsequent packets to the nearest bit rate in bps
	 * for rx sets reporting of CMR changes if cmr is non-zero
	 * h->s only
	 */

#define MSG_G722_1_MODE 0xcdf
	/*
	 * MSG_G722_1_MODE(bitrate)
	 * sets the bitrate on all packets to 24000 or 32000 bps
	 * h->s only
	 */

#define MSG_SILK_SETPARMS 0xcde
	/*
	 * MSG_SILK_SETPARMS(sample_rate,internal_rate,bit_rate,frames_per_packet,packet_loss,complexity,use_fec,use_dtx)
	 * re-configures the codec
	 * h->s only
	 */

#define MSG_OPUS_SETPARMS 0xd11
	/*
	 * MSG_OPUS_SETPARMS(sample_rate,bit_rate,ptime,packet_loss,complexity,use_fec,use_dtx,rate_control,sigtype,apptype,disable_predict)
	 * re-configures the codec
	 * h->s only
	 */	 
#define MSG_OPUS_DECODE_FEC 0xd12
	/*
	 * MSG_OPUS_DECODE_FEC(decode_fec)
	 * control if vmprx decodes received Opus FEC
	 * h->s only
	 */	 
	 
#define SMV_RTP_HEADERLESS    0
#define SMV_RTP_RFC2658       1
#define SMV_RTP_RFC3558       2

#define SMV_MODE_FULL         0
#define SMV_MODE_HALF         1
#define SMV_MODE_QUARTER      2
#define SMV_MODE_EIGHTH       3
#define SMV_MODE_CAPPED_FULL  4
#define SMV_MODE_CAPPED_HALF  5

#define SMV_DATA_NONE         0
#define SMV_DATA_HALF         1
#define SMV_DATA_SPEECH       2
#define SMV_DATA_FULL         3

#define MSG_SMV_RTP_MODE 0xe25
  /*
   * SMV_RTP_MODE: Sets the encoder/decoder RTP packet format to one of the following:
   *   SMV_RTP_HEADERLESS (0)
   *   SMV_RTP_RFC2658    (1)
   *   SMV_RTP_RFC3558    (2)
   *
   * h->s (decoder/encoder) only
   */

#define MSG_SMV_CODEC_MODE 0xe26
  /*
   * SMV_CODEC_MODE - Sets the SMV encoder rate to one of:
   *   SMV_MODE_FULL        (0)   (full-rate)
   *   SMV_MODE_HALF        (1)   (half-rate)
   *   SMV_MODE_QUARTER     (2)   (quarter-rate)
   *   SMV_MODE_EIGHTH      (3)   (eighth-rate)
   *   SMV_MODE_CAPPED_FULL (4)   (half-rate capped version of mode 0)
   *   SMV_MODE_CAPPED_HALF (5)   (half-rate capped version of mode 1)
   *
   * h->s (encoder) only
   */

#define MSG_SMV_DATA_MODE 0xe27
  /*
   * SMV_DATA_MODE - Sets the SMV encoder mode for transmitting data:
   *   SMV_DATA_NONE    (0)   (no data, just speech)
   *   SMV_DATA_HALF    (1)   (61 bits of data but no speech in a mode 1 packet)
   *   SMV_DATA_SPEECH  (2)   (64 bits of data with half-rate speech in a mode 0 packet)
   *   SMV_DATA_FULL    (3)   (144 bits of data but no speech in a mode 0 packet)
   *
   * h->s (encoder) only
   */

#define MSG_SMV_MODE_REQUEST 0xe28
  /*
   * MSG_SMV_MODE_REQUEST - Specifies/indicates the contents of the SMV mode request field
   * of the RTP packet. One of:
   *   SMV_MODE_FULL        (0)   (full-rate)
   *   SMV_MODE_HALF        (1)   (half-rate)
   *   SMV_MODE_QUARTER     (2)   (quarter-rate)
   *   SMV_MODE_EIGHTH      (3)   (eighth-rate)
   *   SMV_MODE_CAPPED_FULL (4)   (half-rate capped version of mode 0)
   *   SMV_MODE_CAPPED_HALF (5)   (half-rate capped version of mode 1)
   *
   * h->s (encoder) and s->h (decoder)
   */

#define JBMODE_NORMAL 0
#define JBMODE_ADAPTIVE 1

#define MSG_JBMODE 0xe36
	/*
	 * MSG_JBMODE(mode, parm0, parm1, parm2) - set jitter buffer mode
	 * the parms definition depends on the mode
	 * for JBMODE_NORMAL - all parms are undefined
	 * for JBMODE_ADAPTIVE
	 *		parm0 = target bias
	 *		parm1 = samp clk freq tol upper as proportion scaled by 65536
	 *		parm2 = samp clk freq tol lower as proportion scaled by 65536
	 *
	 * h->s only
	 */

#define MSG_CONFIG_TAG_MODE_ULAW 0
#define MSG_CONFIG_TAG_MODE_ALAW 1

#define MSG_CONFIG_TAG 0xe37
	/*
	 * MSG_CONFIG_TAG(payloadtype, mode, min_spacing, max_spacing)
	 * spacings are in ms
	 * h->s only
	 */

#define MSG_CONFIG_REDUNDANCY 0xe38
	/*
	 * MSG_CONFIG_REDUNDANCY(task_code,spacing, repeats)
	 * task_code is used to identify which codec is being configured
	 * spacing is in ms, repeats is number of repeats (zero means no redundancy)
	 * h->s only
	 */

#define MSG_INTERNAL_ERROR 0xe39
	/*
	 * MSG_INTERNAL_ERROR(num_errs)
	 * Informs the host that one or more internal errors has been encountered by
	 * the VidMPrx encoder or decoder. Rate of msg generation is throttled to
	 * avoid DOS attacks. An internal error might be, for example, reception of
	 * packets for an AVF frame which would, if processed, cause a buffer
	 * overflow.
	 * s->h only
	 */

#define MSG_JBRESYNC_NOTIFY 0xe3a
	/*
	 * MSG_JBRESYNC_NOTIFY(enable) - control jitter buffer resync notify
	 *		enable = 0 - disables notifications of jbresync
	 *		enable = 1 - enables notifications of jbresync with timestamp
	 *		enable = 2 - enables notifications of jbresync with timestamp and localtimeref
	 *
	 * h->s
	 *
	 * MSG_JBRESYNC_NOTIFY(timestamp) - report VMPRx resync
	 *		timestamp = the new timestamp to which VMPRx is synchronised
	 * MSG_JBRESYNC_NOTIFY(timestamp,localtimeref_lsw,localtimeref_msw) - report VMPRx resync
	 *		timestamp = the new timestamp to which VMPRx is synchronised
	 *		localtimeref = epoch time of sync
	 *
	 * s->h only
	 */

#define MSG_VMPTX_PROP_RTCP_SR_NTP 0xe3b
	/*
	 * MSG_VMPTX_PROP_RTCP_SR_NTP(ntplsw,ntpmsw,ltrlsw,ltrmsw)
	 * associate VMPTx NTP time with kernel epoch time
	 * h->s only
	 */

#define MSG_SET_RTCP_MUX 0xe3c
	/*
	 * MSG_SET_RTCP_MUX()
	 * indicate VMPRx RTCP port should be multiplexed with RTP port.
	 * h->s only
	 */
	 
#define VMPRXFWD_TRANS_NONE 0
#define VMPRXFWD_TRANS_ATOU 1
#define VMPRXFWD_TRANS_UTOA 2

#define MSG_VMPRXFWD_ADD_TYPE 0xd03
	/*
	 * MSG_VMPRXFWD_ADD_TYPE(in_pt, out_pt, transcoding)
	 * add a payload type mapping and transcoding
	 * h->s only
	 */

#define MSG_VMPRXFWD_SET_OUTIPV4 0xd04
	/*
	 * MSG_VMPRXFWD_SET_OUTIPV4(rem-ip, rem-port, local-ip, local-port, TOS)
	 * configures ip/port info.
	 * the addresses are in network byte order.
	 * h->s only
	 */

#define MSG_VMPRXFWD_SET_OUTIPV6 0xd07
	/*
	 * MSG_VMPRXFWD_SET_OUTIPV6(rem-ip[0], rem-ip[1], rem-ip[2], rem-ip[3], rem-port, local-ip[0], local-ip[1], local-ip[2], local-ip[3], local-port, TOS)
	 * configures ip/port info.
	 * the addresses are in network byte order.
	 * h->s only
	 */

#define MSG_RTTEXT_TX_CONFIG 0xd08
	/*
	 * MSG_RTTEXT_TX_CONFIG(redpt, rednum, buftime, idletime)
	 * configures real-time text transmitter
	 * h->s only
	 */

#define MSG_RTTEXT_RX_CONFIG 0xd09
	/*
	 * MSG_RTTEXT_RX_CONFIG(redpt, outofordertime)
	 * configures real-time text receiver
	 * h->s only
	 */

#define MSG_GEN_JITTER 0xd10
	/*
	 * MSG_GEN_JITTER(ext_thresh, gap, jitter, max_age)
	 * configures a VMP[tx] to add jitter to the transmitted packets
	 * h->s only
	 */

#endif
