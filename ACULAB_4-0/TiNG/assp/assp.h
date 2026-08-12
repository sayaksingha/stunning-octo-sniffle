/* assp.h - interface to ASSP */

#ifndef DEFINED_ASSP_H
#define DEFINED_ASSP_H

#include "md5_stream.h"
#include "timequeue_pq.h"

#define TX_WIN 64
#define MAX_RX_WINDOW 128

#include "assp_packet.h"
#include "float.h"

#include <stdint.h>

typedef struct {
	PACKET *frun;	// head of first run
} RXQUEUE;

typedef struct {
	PACKET *first, *last;
	unsigned qlen;
	int (*okorder)(PACKET *l, PACKET*r);
} TXQUEUE;

typedef struct tcb TCB;

typedef struct tcbpath_impl TCBPATH_IMPL;

struct tcbpath_vtbl {
	uint8_t *(*validate_cookie)(TCBPATH_IMPL *path, TCB *tcb, MD5_STREAM *msp, uint8_t *sp, uint8_t *ecoo);
	uint8_t *(*generate_cookie)(TCBPATH_IMPL *path, TCB *tcb, MD5_STREAM *msp, uint8_t *sp);
	int (*netsendpkt)(TCBPATH_IMPL *path, uint8_t *data, uint8_t *end);
	PACKET *(*rxalloc)(TCBPATH_IMPL *path, unsigned maxrx);
	void (*rxfree)(TCBPATH_IMPL *path, PACKET *pkt);
	void (*path_dtor)(TCBPATH_IMPL *path);
	uint8_t *(*make_keycode)(TCBPATH_IMPL *path, TCB *tcb, uint8_t *sp, uint8_t *ep, uint8_t *op);
#ifdef ASSP_PATH_ETHSIZE
	unsigned (*ethsize)(TCBPATH_IMPL *path, unsigned bytes);
#endif
};

typedef struct {
	struct tcbpath_vtbl *vtbl;
	TCBPATH_IMPL *pi;
} TCBPATH;

typedef struct assp_conn ASSP_CONN;

struct assp_tcb_vtbl {
	int (*sof)(ASSP_CONN *cnx);
	void (*rx_peek)(ASSP_CONN *cnx, unsigned ofs, PACKET *pkt); 
	void (*deliver)(ASSP_CONN *cnx, PACKET *pkt);
	void (*eof)(ASSP_CONN *cnx);
	void (*stopped)(ASSP_CONN *cnx);
	void (*closed)(ASSP_CONN *cnx);
	PACKET *(*nexttx)(ASSP_CONN *cnx, int *flags);
	void (*txack)(ASSP_CONN *cnx, PACKET *txp, int acked);
};

/* usage of curtmo in each state:
 * PS_CLOSED		- not running
 * PS_WAIT_COOKIE	- getcookie re-transmit timer
 * PS_ESTABLISHING	- retransmission of SYN
 * PS_ESTABLISHED	- retransmission (if any), validate, or unused
 * PS_FIN_WAIT		- retransmission (if any), validate, or unused
 * PS_CLOSE_WAIT	- retransmission (if any), validate, or unused
 * PS_LAST_ACK		- retransmission of FIN (cannot be ACKLESS)
 * PS_RST_WAIT		- rst retransmission timer
 * PS_TIME_WAIT		- time-wait timer
 */
#define BPMS

struct tcb {
#ifdef PARANOIA
	PARACHECK pm;
#endif
	struct assp_tcb_vtbl *vtbl;
	TCBPATH *path;
	ASSP_CONN *cnx;
	TIMEQUEUE *tq;
	uint32_t cid;
	enum pstate {
		PS_CLOSED, PS_WAIT_COOKIE, PS_ESTABLISHING,
		PS_ESTABLISHED, PS_FIN_WAIT, PS_CLOSE_WAIT, PS_LAST_ACK,
		PS_RST_WAIT, PS_TIME_WAIT,
	} pstate;
	TIMER curtmo;
	TIMER readytmo;
	ABSTIME txreadytime;
	int since_valid;
	int ackneeded;		// to avoid unnecessary acks in rx
	int sendready;		// send when ready
	struct {
		uint32_t lasttxts;	// last TXTS we sent
		uint32_t nxtseq;	// next SEQ value to use
		uint32_t una;	// first SEQ unacknowledged
		uint32_t basets;	// TXTS = time + this value
		PACKET *minseq, *maxseq;
		TXQUEUE txq;		// awaiting cwnd for re-transmission
		TXQUEUE sent_tmoq;	// packets in flight with timeout
		uint_fast32_t sent_ackl;	// ackless packets in flight
		uint_fast32_t extrawind;	// unused receiver window
		unsigned flightsize;	// bits in flight
	} snd;
	struct {
		uint32_t nxt;	// next SEQ value needed
		uint32_t lasttxts;	// TXTS seen in latest packet
		uint32_t maxrxtim;	// last time packet was received
		uint32_t maxtxts;	// biggest TXTS value seen
		uint32_t lastrefts;// REFTS value received in last packet
		RXQUEUE rxq;		// rx packets awaiting hole-filling
		uint_fast32_t window;	// (probably const) our biggest window
	} rcv;
	struct rtt {
		unsigned long min;	// in uS
		unsigned long av;	// in uS
		unsigned long var;	// in uS
	} rtt;
	struct {
		ABSTIME lastack;
#ifdef BPMS
		unsigned long bpms;
		unsigned long lastack_txts;
#else
		FLOAT usperbit;
#endif
	} ca;
	ABSTIME time_wait_end;		// when time_wait state will end
	ABSTIME nexttimer;		// next check of timer queue
	unsigned rst_tmo;		// in uS
	struct {
		uint_fast32_t numrx_seq;
		uint_fast32_t numrx_unseq;
		uint_fast32_t numrx_unkopt_unseq;
		uint_fast32_t numrx_badopt_unseq;
		uint_fast32_t numrx_in_accepted;
		uint_fast32_t numrx_in_rejected;
			// cookie counters
		uint_fast32_t numrx_coo_invtime;
		uint_fast32_t numrx_coo_badtxts;
		uint_fast32_t numrx_coo_badrefts;
		uint_fast32_t numrx_coo_badsize;
		uint_fast32_t numrx_coo_bogus;
		uint_fast32_t numrx_coo_stale;
			// state-specific counters
				// closed
		uint_fast32_t numrx_closed;
		uint_fast32_t numrx_closed_crq;
				// wait-cookie
		uint_fast32_t numrx_wc_rst;
		uint_fast32_t numrx_wc_badopt;
		uint_fast32_t numrx_wc_unkopt;
		uint_fast32_t numrx_wc_ok;
		uint_fast32_t numrx_wc_bogus;
		uint_fast32_t numrx_wc_noncoo;
				// syn-sent to last-ack
		uint_fast32_t numrx_sync_rst;
		uint_fast32_t numrx_sync_badopt;
		uint_fast32_t numrx_sync_unkopt;
		uint_fast32_t numrx_sync_fresh;
		uint_fast32_t numrx_sync_stale;
		uint_fast32_t numrx_sync_alienack;
		uint_fast32_t numrx_sync_unseq;
		uint_fast32_t numrx_sync_afterfin;
		uint_fast32_t numrx_sync_seq_oow;
		uint_fast32_t numrx_sync_seq_drop;
		uint_fast32_t numrx_sync_seq_next;
		uint_fast32_t numrx_sync_seq_ooo;
		uint_fast32_t numrx_sync_seq_dup;
				// time-wait
		uint_fast32_t numrx_tw;
		uint_fast32_t numrx_tw_crqbad;
			// transmitter counters
		uint_fast32_t numtx_seq;
		uint_fast32_t numtx_coo;
		uint_fast32_t numtx_seqdup;
		uint_fast32_t numtx_unseq;
			// connect break port mortem clue
		char* abort_why;
	} stats;
};

PACKET *assp_txpacket(TCB *tcb, unsigned maxdlen);
uint32_t assp_rx_cid(PACKET *pkt);
void assp_handle_rx(TCB *tcb, PACKET *rxp);
	// flags for write
	// mark packet ACKLESS
#define ASSP_F_ACKLESS 1
	// mark packet FASTACK
#define ASSP_F_FASTACK 2
	// send EOF
#define ASSP_F_EOF 4
	// ASSP must send anyway (passed into txready handler
#define ASSP_F_MUSTSEND 0x8000
int assp_txready(TCB *tcb);
int assp_connect(TCB *tcb, uint32_t cid);
int assp_abort(TCB *tcb, PACKET *txp, char *why);
int assp_validate(TCB *tcb);
void assp_tcb_stats(TCB *tcb);
void assp_tcb_init(TCB *tcb, ASSP_CONN *cnx, TIMEQUEUE *tq, TCBPATH *tcbpath, struct assp_tcb_vtbl *vtbl);
void assp_tcb_dtor(TCB *tcb);

#ifdef ASSP_SHARE_TINGTRACE
#define ASSP_TRACE(x) ((x) << 4)
#else
#define ASSP_TRACE(x) (x)
#endif

	//	STATE	show state transitions
#define T_STATE ASSP_TRACE(1)
	//	CWOK	tx flow control reasoning
#define T_CWOK ASSP_TRACE(2)
	//	RTT	updates of RTT estimates
#define T_RTT ASSP_TRACE(4)
	//	FLOW	window updates derived from WTR/DNS
#define T_FLOW ASSP_TRACE(8)
	//	STALE	handling of nonsequenced part of stale packets
#define T_STALE ASSP_TRACE(0x10)
	//	SEQ	fate of sequenced packets
#define T_SEQ ASSP_TRACE(0x20)
	//	RATEHALVING updates to rate-halving loss recovery
#define T_RATEHALVING ASSP_TRACE(0x40)
	//	CONGAVOID	operation of congestion avoidance
#define T_CONGAVOID ASSP_TRACE(0x80)
	//	TXTIM	tx-to-ack time for each packet
#define T_TXTIM ASSP_TRACE(0x100)
	//	TX	all packets transmitted
#define T_TX ASSP_TRACE(0x200)
	//	RX	all packets received
#define T_RX ASSP_TRACE(0x400)
	//	BADPKT	bad packets received
#define T_BADPKT ASSP_TRACE(0x800)
	//	LOST	lost packets
#define T_LOST ASSP_TRACE(0x1000)
	//	API	API function invocation
#define T_API ASSP_TRACE(0x4000)
	//	TXQ	show whole trsnsmit queue
#define T_TXQ ASSP_TRACE(0x8000)
	//	SACK	WTR/DNS composition & interpretation
#define T_SACK ASSP_TRACE(0x10000)

#ifdef PROVIDE_TRACE
#include <stdarg.h>

void assp_trace(int trace);
extern int (*assp_tracefn)(const char *fmt, va_list ap);
#ifdef _GNUC_
#define PRINTF_LIKE __attribute__ ((format(printf, 1, 2)))
#endif
#ifndef PRINTF_LIKE
#define PRINTF_LIKE
#endif
int assp_log(const char *fmt, ...) PRINTF_LIKE;
#endif

#endif
