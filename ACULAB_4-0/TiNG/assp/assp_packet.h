/* assp_packet.h - ASSP packet format stuff */

#ifndef DEFINED_ASSP_PACKET_H
#define DEFINED_ASSP_PACKET_H

#include <stdint.h>
#include "timefn.h"

enum ptype {
	PKT_NODATA,	// never acked
	PKT_ACKLESS,	// not usually acked
	PKT_NORMAL,	// usually acked
	PKT_FASTACK,	// instant ack
	NUM_PKT,
};

// in the PACKET structure we want 'data' to be at an offset
// which is a multiple of 64, for better efficiency on the DSP.
// The common packet data amounts to 40 bytes so, as the union 
// won't fit in 24 bytes, we make it 88 bytes.
#define PACKET_UNION_LIMIT 22

#ifndef ASSP_USER_DEFINES_PACKET_UNION_ELEMENTS
#define ASSP_USER_DEFINED_PACKET_UNION_ELEMENTS
	struct assp_packet_union_rtp_tx{
		uint8_t *payloadstart;
		unsigned long send_count; // redundancy retransmission count (if used)
		uint_fast64_t txtime; // last transmit time (if used)
		uint64_t srtp_est_xtd_seq_num; //  estimated xtd_seq_num_t
		struct packet *next; // for redundancy list
		int tx_err; // if serious error encountered on tx
		unsigned char srtp_checked_ok;
	};
	struct assp_packet_union_rtp_rx2{
		struct packet *prev;
		unsigned long timestamp;
		uint8_t *headerstart;
		unsigned short sequenceNumber; // for secure RTP
		unsigned short ipudpheaderlen;
		uint64_t srtp_est_xtd_seq_num; //  estimated xtd_seq_num_t of *hdr
		uint64_t rxtim;
		unsigned char payloadType;
		unsigned char isDtmf;
		uint16_t source_port;
		uint32_t source_ip;
		uint32_t sync_source;
		uint16_t dtmf_shortest_duration; // only valid when isDtmf is true
		unsigned char srtp_checked_ok;
		uint32_t vid_playout_time;
	};
	struct assp_packet_union_t38 {
		unsigned redpackets;
	};
#endif

#ifndef ASSP_USER_DEFINED_PACKET_UNION_ELEMENTS
	struct assp_packet_union_rtp_tx{
		int dummy;
	};
	struct assp_packet_union_rtp_rx2{
		int dummy;
	};
	struct assp_packet_union_t38 {
		int dummy;
	};
#endif

typedef union {
	struct {
		int isfin;
			// fields only for head-of-run packets:
		struct packet *runlast;	// last in this run
		struct packet *runnext;	// head of next run
		unsigned runlen;
	} rx;
	struct {
		uint8_t *seqopt;	// points to current start of sequenceed options
		enum txstatus {
			TS_WTR,		// on txq
			TS_SENTT,	// sent - on tmoq
			TS_SENTA,	// sent - ackless
			TS_DNS,		// not on a queue
		} status;
		enum ptype ptype;
		unsigned txfs;	// flight size when last tx: 0=ignore
		unsigned ethsize;	// size in bits when last tx
		struct packet *qprev;
		ABSTIME txtim;
		unsigned retxcount;
		uint8_t changeover;
		uint8_t payloadplaintext;
		uint8_t mergeinhibit;	// 0 - can merge, 1 - no merge following this data, 2 - no merge at all
	} tx;

	struct assp_packet_union_rtp_tx rtp_tx;
	struct assp_packet_union_rtp_rx2 rtp_rx2;
	struct assp_packet_union_t38 t38;

	char pad[PACKET_UNION_LIMIT*4];
} PACKET_UNION;

#ifdef TiNGTYPE_STARCORE
extern char dummy_object_to_check_PACKET_UNION_size[sizeof(PACKET_UNION) == PACKET_UNION_LIMIT*4];
#endif

#ifndef TiNG_PACKET_SIZE
#define TiNG_PACKET_SIZE 2048
#endif // TiNG_PACKET_SIZE

typedef struct packet {
	struct packet *next;
	struct packet *next_byseq;
	uint32_t seq;
	uint8_t *rawstart;	// points to CID field in 'data'
	uint8_t *datastart;	// points to start of user data
	uint8_t *dataend;	// points to address after user data
	uint8_t *rawend;	// points to address after last possible data
	unsigned dbg_bc;	// general ASSP user's debugging code
	ABSTIME created; // for debugging & stats only
// 10 * 4 bytes offset to here
	PACKET_UNION u;
	uint8_t data[(TiNG_PACKET_SIZE-40-(PACKET_UNION_LIMIT*4)-4)&(~7)];	// keep it under a power of two, structure padded to 8byte alignment
} PACKET;

#ifdef TiNGTYPE_STARCORE
extern char dummy_object_to_check_PACKET_size[sizeof(PACKET) <= TiNG_PACKET_SIZE-4];
#endif

	// fixed fields
#define ASSP_PKTOFS_CID 0
#define ASSP_PKTOFS_SEQ 4
#define ASSP_PKTOFS_ACK 8
#define ASSP_PKTOFS_TXTS 12
#define ASSP_PKTOFS_REFTS 16
#define ASSP_PKTOFS_RESERVED 20
	// allow plenty space for options (NB. WTRDNS can be 128 bytes)
#define ASSP_PKT_OVERHEAD (20+80+128)

	// options
#define ASSP_PKTOFS_OPTIONS 22

	// option codes
#define ASSP_OPTION_CRIT 0x8000u
#define ASSP_OPTION_SEQ 0x4000

enum {
	ASSP_OPT_RST = ASSP_OPTION_CRIT | 1,
	ASSP_OPT_WTRDNS = ASSP_OPTION_CRIT | 2,
	ASSP_OPT_FASTACK = ASSP_OPTION_CRIT | 3,
	ASSP_OPT_ACKLESS = ASSP_OPTION_CRIT | 4,
	ASSP_OPT_COOKIE = ASSP_OPTION_CRIT | 5,

	ASSP_OPT_SYN = ASSP_OPTION_CRIT | ASSP_OPTION_SEQ | 6,
	ASSP_OPT_FIN = ASSP_OPTION_CRIT | ASSP_OPTION_SEQ | 7,
	ASSP_OPT_GETCOOKIE = ASSP_OPTION_CRIT | ASSP_OPTION_SEQ | 8,
	ASSP_OPT_USECOOKIE = ASSP_OPTION_CRIT | ASSP_OPTION_SEQ | 9,
	ASSP_OPT_KEYCODE = ASSP_OPTION_SEQ | 10,
};

#endif
