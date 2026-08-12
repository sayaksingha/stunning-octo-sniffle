/* rtcp_parser.h - RTCP parsing functions */

#ifndef INCLUDED_RTCP_PARSER_H
#define INCLUDED_RTCP_PARSER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int v;
	int p;
	unsigned count;	// various names depending on pt
	int pt;
	unsigned length;
	unsigned char *start;
	unsigned char *end;	// excluding padding
} RTCP;

typedef struct {	// PT=SR=200
	uint_fast32_t ssrc;
	uint_fast64_t ntp_timestamp;
	uint_fast32_t rtp_timestamp;
	uint_fast32_t senders_packet_count;
	uint_fast32_t senders_octet_count;
} RTCP_SR;

typedef struct {
	uint_fast32_t ssrc;
	int fraction_lost;
	uint_fast32_t packets_lost;
	uint_fast32_t ehsnr;
	uint_fast32_t jitter;
	uint_fast32_t lsr;
	uint_fast32_t dlsr;
} RTCP_SR_REPORT_BLOCK;

typedef struct {	// PT=SR=201
	uint_fast32_t ssrc;
} RTCP_RR;

typedef struct {
	uint_fast32_t ssrc;
	int fraction_lost;
	uint_fast32_t packets_lost;
	uint_fast32_t ehsnr;
	uint_fast32_t jitter;
	uint_fast32_t lsr;
	uint_fast32_t dlsr;
} RTCP_RR_REPORT_BLOCK;

typedef struct {
	uint_fast32_t ssrc_csrc;
	unsigned item_type;
	unsigned item_length;
	unsigned char *item_data;
	unsigned char *next_item;
} RTCP_SDES_CHUNK;

typedef struct {	// PT=BYE=203
	unsigned char *reason_text;
	unsigned reason_length;
} RTCP_BYE;

typedef struct {
	uint_fast32_t ssrc_csrc;
} RTCP_BYE_SSRC_CSRC;

typedef struct {	// PT=APP=204
	uint_fast32_t ssrc_csrc;
	char name[4];
	unsigned char *data;
	unsigned data_length;
} RTCP_APP;

typedef struct {	// PT=XR=207
	uint_fast32_t ssrc;
	unsigned blocktype;	// ~0u if none
	unsigned char *curblock;
	unsigned char *curblockend;
	unsigned char *xr_end;
} RTCP_XR;

typedef struct {
	uint_fast32_t chunk;
} RTCP_XR_BT_1_CHUNK;

typedef struct {
	int t;	// thinning
	uint_fast32_t ssrc;
	uint_fast32_t begin_seq;
	uint_fast32_t end_seq;
	unsigned numchunks;
	unsigned char *chunks;
} RTCP_XR_BT_1;

typedef struct {
	uint_fast32_t chunk;
} RTCP_XR_BT_2_CHUNK;

typedef struct {
	int t;	// thinning
	uint_fast32_t ssrc;
	uint_fast32_t begin_seq;
	uint_fast32_t end_seq;
	unsigned numchunks;
	unsigned char *chunks;
} RTCP_XR_BT_2;

typedef struct {
	uint_fast32_t time;
} RTCP_XR_BT_3_TIME;

typedef struct {
	int t;	// thinning
	uint_fast32_t ssrc;
	uint_fast32_t begin_seq;
	uint_fast32_t end_seq;
	unsigned numtimes;
	unsigned char *times;
} RTCP_XR_BT_3;

typedef struct {
	uint_fast64_t ntp_timestamp;
} RTCP_XR_BT_4;

typedef struct {
	unsigned subblocks;
} RTCP_XR_BT_5;

typedef struct {
	uint_fast32_t ssrc;
	uint_fast32_t lrr;
	uint_fast32_t dlrr;
} RTCP_XR_BT_5_SUBBLOCK;

typedef struct {
	int l;
	int d;
	int j;
	int ToH;
	uint_fast32_t ssrc;
	uint_fast32_t begin_seq;
	uint_fast32_t end_seq;
	uint_fast32_t lost_packets;
	uint_fast32_t dup_packets;
	uint_fast32_t min_jitter;
	uint_fast32_t max_jitter;
	uint_fast32_t mean_jitter;
	uint_fast32_t dev_jitter;
	unsigned min_ttl_or_hl;
	unsigned max_ttl_or_hl;
	unsigned mean_ttl_or_hl;
	unsigned dev_ttl_or_hl;
} RTCP_XR_BT_6;

typedef struct {
	uint_fast32_t ssrc;
	unsigned loss_rate;
	unsigned discard_rate;
	unsigned burst_density;
	unsigned gap_density;
	unsigned burst_duration;
	unsigned gap_duration;
	unsigned round_trip_delay;
	unsigned end_system_delay;
	unsigned signal_level;
	unsigned noise_level;
	unsigned RERL;
	unsigned Gmin;
	unsigned R_factor;
	unsigned ext_R_factor;
	unsigned MOS_LQ;
	unsigned MOS_CQ;
	unsigned RX_config;
	unsigned JB_nominal;
	unsigned JB_maximum;
	unsigned JB_abs_max;
} RTCP_XR_BT_7;

// Aculab specific RTCP data

typedef struct {
	int type;
	unsigned char *start;
	unsigned char *end;	// excluding padding
} RTCP_SUMMARY;

typedef struct {
	uint_fast32_t ssrc;
	uint_fast32_t packetsSent;
	uint_fast32_t octetsSent;
} RTCP_SUMMARY_TX;

typedef struct {
	uint_fast32_t ssrc;
	uint_fast32_t jitter;
	uint_fast32_t expected;
	uint_fast32_t seen;
	uint_fast32_t discarded;
} RTCP_SUMMARY_RX;

typedef struct {
	uint_fast32_t ssrc;
	uint_fast32_t jitter;
	uint_fast32_t expected;
	uint_fast32_t seen;
	uint_fast32_t discarded;
	uint_fast32_t lastResyncTS;
	uint_fast32_t lastPktInTS;
	uint_fast32_t lastPktInVMPTS;
	uint_fast32_t lastPktInMSToEpoch;
} RTCP_SUMMARY_AUGMENTED_RX;

int rtcp_analyse(RTCP *rtcp_out, unsigned char *rtcp);
int rtcp_analyse_sr(RTCP_SR *sr, RTCP *rtcp);
int rtcp_analyse_sr_report_block(RTCP_SR_REPORT_BLOCK *rb, unsigned block_index, RTCP *rtcp);
int rtcp_get_sr_pse(unsigned char **ptr, unsigned *len, RTCP *rtcp);
int rtcp_analyse_rr(RTCP_RR *rr, RTCP *rtcp);
int rtcp_analyse_rr_report_block(RTCP_RR_REPORT_BLOCK *rb, unsigned block_index, RTCP *rtcp);
int rtcp_get_rr_pse(unsigned char **ptr, unsigned *len, RTCP *rtcp);
int rtcp_analyse_sdes(RTCP_SDES_CHUNK *op, RTCP *rtcp);
int rtcp_analyse_sdes_next_item(RTCP_SDES_CHUNK *op);
int rtcp_analyse_sdes_next_chunk(RTCP_SDES_CHUNK *op, RTCP *rtcp);
int rtcp_analyse_bye(RTCP_BYE *bye, RTCP *rtcp);
int rtcp_analyse_bye_ssrc_csrc(RTCP_BYE_SSRC_CSRC *op, unsigned itemno, RTCP *rtcp);
int rtcp_analyse_xr(RTCP_XR *xr, RTCP *rtcp);
int rtcp_analyse_xr_next_block(RTCP_XR *xr);
int rtcp_analyse_xr_bt_1(RTCP_XR_BT_1 *xrb, RTCP_XR *xr);
int rtcp_analyse_xr_bt_1_chunk(RTCP_XR_BT_1_CHUNK *xrb1, RTCP_XR_BT_1 *xrb, unsigned chunk_index);
int rtcp_analyse_xr_bt_2(RTCP_XR_BT_2 *xrb, RTCP_XR *xr);
int rtcp_analyse_xr_bt_2_chunk(RTCP_XR_BT_2_CHUNK *xrb2, RTCP_XR_BT_2 *xrb, unsigned chunk_index);
int rtcp_analyse_xr_bt_3(RTCP_XR_BT_3 *xrb, RTCP_XR *xr);
int rtcp_analyse_xr_bt_3_time(RTCP_XR_BT_3_TIME *xrb3, RTCP_XR_BT_3 *xrb, unsigned time_index);
int rtcp_analyse_xr_bt_4(RTCP_XR_BT_4 *xrb, RTCP_XR *xr);
int rtcp_analyse_xr_bt_5(RTCP_XR_BT_5 *xrb, RTCP_XR *xr);
int rtcp_analyse_xr_bt_5_subblock(RTCP_XR_BT_5_SUBBLOCK *xrb, RTCP_XR *xr);
int rtcp_analyse_xr_bt_6(RTCP_XR_BT_6 *xrb, RTCP_XR *xr);
int rtcp_analyse_xr_bt_7(RTCP_XR_BT_7 *xrb, RTCP_XR *xr);
int rtcp_analyse_app(RTCP_APP *app, RTCP *rtcp);
int rtcp_analyse_summary(RTCP_SUMMARY *rtcp_out, unsigned char *rtcp);
int rtcp_analyse_summary_tx(RTCP_SUMMARY_TX *rtcp_out, RTCP_SUMMARY *rtcp);
int rtcp_analyse_summary_rx(RTCP_SUMMARY_RX *rtcp_out, RTCP_SUMMARY *rtcp);
int rtcp_analyse_summary_augmented_rx(RTCP_SUMMARY_AUGMENTED_RX *rtcp_out, RTCP_SUMMARY *rtcp);

#ifdef __cplusplus
}
#endif

#endif
