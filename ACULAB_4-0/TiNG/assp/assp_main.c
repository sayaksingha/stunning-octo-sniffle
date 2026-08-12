#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef TiNGTYPE_LINUX
#include <errno.h>
	// for debugging only:
#include <inttypes.h>
#include <poll.h>
#endif


#ifdef TiNGTYPE_STARCORE
#pragma pgm_seg_name "assp_code_seg"
#ifndef PROVIDE_TRACE
#define PRId64
#define PRIu64
#endif
#endif

#ifdef TiNGTYPE_WINNT
#include <windows.h>
	// for debugging only:
#include <inttypes.h>
#endif

#define arlen(x) (sizeof(x)/sizeof(*(x)))

#include "assp.h"
#include "assp_impl.h"
#include "asspdbg.h"
#include "cookie.h"
#include "decode_str.h"
#include "pktqueue.h"
#include "randent.h"
#include "txqueue.h"

	// clock constants
#define CLK_EPSRX 1
#define CLK_EPSTX 1
#define CLK_TPS 1000000
#define CLK_MSL 255 * CLK_TPS

#define MSL 255

	// FIXME: very short timeout just for debugging
//#define TW_LEN time_longS2rel(MSL*2)
#define TW_LEN time_longS2rel(4)
	// maximum time to keep re-transmitting RST before just giving up
#define RST_LEN time_longS2rel(MSL*2)

typedef enum { PK_ALIEN, PK_STALE, PK_FRESH, } PK_CLASS;

#ifdef _MSC_VER

/* disable stupid warnings about conversions
 */

#pragma warning(disable:4244)
#pragma warning(disable:4305)
/* and about argument conversions (does anyone still use a
   non-prototype compiler?) */
#pragma warning(disable:4761)

#endif

#ifndef check_object
#define check_object(x) ((void) x)
#endif

#ifndef commit_object
#define commit_object(x) ((void) x)
#endif

static void check_tcb(TCB *tcb)
{
 check_object(*tcb);
}

static void commit_tcb(TCB *tcb)
{
 commit_object(*tcb);
}

#ifdef PROVIDE_TRACE
#ifdef ASSP_SHARE_TINGTRACE
	#ifndef INCLUDED_TiNGTRACE_H
		extern int TiNGtrace;
	#endif
	#define trace TiNGtrace
#else
static unsigned trace;
#endif
int (*assp_tracefn)(const char *fmt, va_list ap) = vprintf;

void assp_trace(int newtrace)
{
#ifdef ASSP_SHARE_TINGTRACE
 (void) newtrace;
#else
 trace = newtrace;
#endif
}

const char *pstate(enum pstate s)
{
 static const char *statename[] = {
	"PS_CLOSED", "PS_WAIT_COOKIE", "PS_ESTABLISHING",
	"PS_ESTABLISHED", "PS_FIN_WAIT", "PS_CLOSE_WAIT", "PS_LAST_ACK",
	"PS_RST_WAIT", "PS_TIME_WAIT", };
 if (s < arlen(statename)) return statename[s];
 assp_log("pstate = %d\n", s);
 return "Bogus";
}

static void showpstate(TCB *tcb, enum pstate s)
{
 assp_log("%p: %s -> %s\n", tcb, pstate(tcb->pstate), pstate(s));
}
#else
static int notracefn(const char *fmt, va_list ap)
{
 (void) fmt;
 (void) ap;
 return 0;
}

static int (*const assp_tracefn)(const char *fmt, va_list ap) = notracefn;
#define trace 0
#define showpstate
#endif

int assp_log(const char *fmt, ...)
{
 va_list ap;
 va_start(ap, fmt);
 (*assp_tracefn)(fmt, ap);
 va_end(ap);
 return 0;
}

static void setpstate(TCB *tcb, enum pstate s)
{
 if (trace & T_STATE) showpstate(tcb, s);
 tcb->pstate = s;
}

static void show_txq(TCB *tcb)
{
 PACKET *txp = tcb->snd.txq.first;
 if (txp) {
	char buf[128], *bp = buf;
	uint32_t seq = txp->seq;
	for (; (txp = txp->next); ) {
		while (++seq != txp->seq) *bp++ = '.';
		*bp++ = '*';
	}
	*bp = 0;
	assp_log("%p: txq: %u %s\n", tcb, seq, buf);
 }
}

#ifdef CHECK_TXQ

static void dumptxq(TCB *tcb)
{
 PACKET *txp = tcb->snd.minseq;
 printf("%p: Q", tcb);
 if (txp) printf(" %u: ", txp->seq);
 for (; txp; txp=txp->next_byseq) {
	switch (txp->u.tx.status) {
	case TS_WTR:	// on txq
		printf("W");
		break;
	case TS_DNS:	// not queued
		printf("D");
		break;
	case TS_SENTT:	// on sent_tmoq
		printf("T(%u)", txp->u.tx.ethsize);
		break;
	case TS_SENTA:	// not queued
		printf("A");
		break;
	}
 }
 printf("\n");
}

static void verify_nonq(TCB *tcb)
{
 uint32_t totpkt = tcb->snd.nxtseq - tcb->snd.una;
 unsigned ndns = 0, nsenta = 0;
 unsigned fs = 0;
 uint32_t seq = tcb->snd.una;
 PACKET *txp = tcb->snd.minseq;
 if (!txp) {
	if (tcb->snd.maxseq) {
		assp_log("%p: Internal error: minseq=0, but maxseq = %p\n",
			tcb, tcb->snd.maxseq);
		assp_error();
	}
 } else {
	for (;;) {
		PACKET *nx;
		if (txp->seq != seq) {
			assp_log("%p: Internal error: %u->next_byseq = %u\n", tcb,
					seq, txp->seq);
			assp_error();
		}
		seq = txp->seq + 1;
		switch (txp->u.tx.status) {
		case TS_WTR:	// on txq
			txq_require(&tcb->snd.txq, txp);
			break;
		case TS_DNS:	// not queued
			ndns++;
			break;
		case TS_SENTT:	// on sent_tmoq
			fs += txp->u.tx.ethsize;
			txq_require(&tcb->snd.sent_tmoq, txp);
			break;
		case TS_SENTA:	// not queued
			nsenta++;
			break;
		}
		nx = txp->next_byseq;
		if (!nx) {
			if (txp != tcb->snd.maxseq) {
				assp_log("%p: Internal error: maxseq (%p) != last (%p)\n",
					tcb, tcb->snd.maxseq, txp);
				assp_error();
			}
			break;
		}
		txp = nx;
	}
 }
 if (seq != tcb->snd.nxtseq) {
	assp_log("%p: Internal error: maxseq (%u) != nxtseq (%u)\n",
		tcb, seq, tcb->snd.nxtseq);
				assp_error();
 }
 if (totpkt != tcb->snd.txq.qlen + tcb->snd.sent_tmoq.qlen + nsenta + ndns) {
	assp_log("%p: Internal error: totpkt (%d) != tx (%d) + tmo (%d) + ackl (%d) + dns (%d)\n", tcb,
		totpkt, tcb->snd.txq.qlen, tcb->snd.sent_tmoq.qlen, nsenta, ndns);
	assp_error();
 }
 if (nsenta != tcb->snd.sent_ackl) {
	assp_log("%p: Internal error: ackl (%d) != tcb->snd.sent_ackl (%d)\n", tcb,
		nsenta, tcb->snd.sent_ackl);
	assp_error();
 }
 if (fs != tcb->snd.flightsize) {
	assp_log("%p: flightsize (%u) != sum of packets (%u)\n", tcb, tcb->snd.flightsize, fs);
	dumptxq(tcb);
	assp_error();
 }
}

static void verify_q(TCB *tcb)
{
 txq_verify(&tcb->snd.txq, TS_WTR);
 txq_verify(&tcb->snd.sent_tmoq, TS_SENTT);
 verify_nonq(tcb);
}
#else
#define verify_q(tcb)
#define txq_verify(q, type)
#endif

static uint32_t ts_now(TCB *tcb)
{
 return tcb->snd.basets - time_rel2longuS(time_until(time_zero())) * (CLK_TPS / 1000000);
}

#ifdef BPMS
static ABSTIME cc_txtime(TCB *tcb)
{
 PACKET *txp = tcb->snd.sent_tmoq.first;
 if (txp && tcb->snd.sent_tmoq.qlen > 1) {
	uint_fast64_t fs = tcb->snd.flightsize;
	uint_fast64_t maxfs = tcb->rtt.min;
	fs *= 1000;
	maxfs *= 3;
	maxfs *= tcb->ca.bpms;
	if (fs > maxfs) {
		if (trace & T_CONGAVOID) {
			assp_log("%p: cc_txtime %" PRIu64 " > %" PRIu64 ", fs = %u bits = %u pkts\n", tcb,
				fs, maxfs, tcb->snd.flightsize, tcb->snd.sent_tmoq.qlen);
		}
		return time_never();
	}
#ifdef CA_PACE_TX
	more stuff here to check queue
#endif
 }
 return time_zero();
}
#else
static ABSTIME cc_txtime(TCB *tcb)
{
 PACKET *txp = tcb->snd.sent_tmoq.first;
 ABSTIME safetx = time_zero();
 int32_t fsus = float_2int32(float_mul(float_int32(tcb->snd.flightsize), tcb->ca.usperbit));
 if (!txp) return safetx;
 if (fsus + 0u > 5 * tcb->rtt.min) {
	if (trace & T_CONGAVOID) {
		assp_log("%p: fsus = %u > %u, fs = %u bits = %u pkts\n", tcb,
			fsus, 2 * tcb->rtt.min,
			tcb->snd.flightsize, tcb->snd.sent_tmoq.qlen);
	}
	return time_never();
 }
#ifdef CA_PACE_TX
	more stuff here to check queue
#endif
 return safetx;
}
#endif


static void check_time_wait(void *arg)
{
 TCB *tcb = arg;
 check_tcb(tcb);
 if (time_sgn(time_until(tcb->time_wait_end)) <= 0) {
	setpstate(tcb, PS_CLOSED);
	commit_tcb(tcb);
	tcb->vtbl->closed(tcb->cnx);
 } else {
	tcb->tq->vtbl->start(tcb->tq->tqi, &tcb->curtmo, check_time_wait, tcb, tcb->time_wait_end);
 }
}

static void txpacket_free(TCB *tcb, PACKET *txp)
{
 tcb->path->vtbl->rxfree(tcb->path->pi, txp);
}

static void txpacket_free_user(TCB *tcb, PACKET *txp, int acked)
{
 commit_tcb(tcb);
 tcb->vtbl->txack(tcb->cnx, txp, acked);
 check_tcb(tcb);
 txpacket_free(tcb, txp);
}

static void discard_rxq(TCB *tcb)
{
 PACKET *p;
	// discard undelivered data
 p = tcb->rcv.rxq.frun;
 for (; p;) {
	PACKET *nxrun = p->u.rx.runnext;
	for (; p;) {
		PACKET *nx = p->next;
		tcb->path->vtbl->rxfree(tcb->path->pi, p);
		p = nx;
	}
	p = nxrun;
 }
 tcb->rcv.rxq.frun = 0;
}

static void discard_txq(TCB *tcb)
{
 	// discard tx data awaiting acknowledgement
 for (;;) {
	PACKET *tp = tcb->snd.minseq;
	if (!tp) break;
	tcb->snd.minseq = tp->next_byseq;
	if (trace & T_TXTIM) assp_log("%p: discard dtime %ld\n", tcb, -time_rel2longuS(time_until(tp->created)));
	switch (tp->u.tx.status) {
	case TS_WTR:
		txq_dequeue(&tcb->snd.txq, tp);
		break;
	case TS_SENTT:
		txq_dequeue(&tcb->snd.sent_tmoq, tp);
		break;
	case TS_SENTA:
		tcb->snd.sent_ackl--;
		break;
	case TS_DNS:;
	}
	txpacket_free_user(tcb, tp, 0);
 }
 tcb->snd.minseq = 0;
 tcb->snd.maxseq = 0;
 tcb->snd.una = tcb->snd.nxtseq;
 verify_q(tcb);
 if (tcb->snd.sent_ackl) {
	assp_log("%p: Internal error: sent_ackl (%d) > 0 after discard\n", tcb, tcb->snd.sent_ackl);
	assp_error();
 }
}

static void discard_data(TCB *tcb)
{
 discard_rxq(tcb);
 discard_txq(tcb);
}

static void time_wait(TCB *tcb)
{
 if (tcb->pstate != PS_TIME_WAIT) {
	setpstate(tcb, PS_TIME_WAIT);
	commit_tcb(tcb);
	tcb->vtbl->stopped(tcb->cnx);
	check_tcb(tcb);
	discard_data(tcb);
 } else {
	tcb->tq->vtbl->cancel(tcb->tq->tqi, &tcb->curtmo);
 }
 tcb->time_wait_end = time_when(TW_LEN);
 tcb->tq->vtbl->start(tcb->tq->tqi, &tcb->curtmo, check_time_wait, tcb, tcb->time_wait_end);
}

#ifndef ASSP_PATH_ETHSIZE
// number of bit times required to send an ethernet packet
static unsigned ethsize(unsigned bytes)
{
 return (bytes + /* UDP */ 8 + /* IP */ 20 + /* eth */ 14) * 8
 	+ /* preamble */ 64 + /* gap */ 96;
}
#endif

static PACKET *txpacket(TCB *tcb, unsigned maxdlen)
{
 const unsigned int maxpkt = ~0u - ASSP_PKT_OVERHEAD;
 PACKET *txp;
 if (maxdlen >= maxpkt) maxdlen = maxpkt;
 else maxdlen += ASSP_PKT_OVERHEAD;
 txp = tcb->path->vtbl->rxalloc(tcb->path->pi, maxdlen);
 if (!txp) assp_error();
 txp->u.tx.ptype = PKT_NODATA;
 txp->u.tx.txfs = 0;
 txp->created = time_when(time_longS2rel(0));
 txp->datastart = txp->dataend = txp->rawstart + ASSP_PKT_OVERHEAD;
 return txp;
}

static PACKET *txpacket0(TCB *tcb, unsigned maxdlen)
{
 PACKET *txp = txpacket(tcb, maxdlen);
 txp->u.tx.seqopt = txp->datastart;
 return txp;
}

PACKET *assp_txpacket(TCB *tcb, unsigned maxdlen)
{
 PACKET *txp;
 check_tcb(tcb);
 txp = txpacket(tcb, maxdlen);
 commit_tcb(tcb);
 return txp;
}

static uint8_t *compose_wtrdns(TCB *tcb, PACKET *pkt, uint8_t *sp)
{
 PACKET *rxp = tcb->rcv.rxq.frun;
 int window = tcb->rcv.window;
 uint8_t wtrdns[MAX_RX_WINDOW];
 uint32_t ackno = tcb->rcv.nxt;
 uint8_t *wdp = wtrdns;
 unsigned len;
 for (; rxp && window > 0; rxp = rxp->u.rx.runnext) {// for each rx run of packets
		// handle WTR part - packets before this run
	int n = rxp->seq - ackno;
	if (trace & T_SACK) assp_log("%p: window=%d, wtr %d+%d\n", tcb, window, ackno, rxp->seq - ackno);
	if (n > window) n = window;
	window -= n;
	while (n > 255) {
		*wdp++ = 255;
		*wdp++ = 0;
		n -= 255;
	}
	*wdp++ = n;
		// handle DNS part - packets queued in this run
	n = rxp->u.rx.runlen;
	if (trace & T_SACK) assp_log("%p:  runlen=%d\n", tcb, n);
	ackno = rxp->seq + n;
	window -= n;
	while (n > 255) {
		*wdp++ = 255;
		*wdp++ = 0;
		n -= 255;
	}
	*wdp++ = n;
 }
	// might have window < 0 if last DNS overlapped end of window
 if (window > 0) {
	if (trace & T_SACK) assp_log("%p: +window=%d\n", tcb, window);
	while (window > 255) {
		*wdp++ = 255;
		*wdp++ = 0;
		window -= 255;
	}
	*wdp++ = window;
	*wdp++ = 0;
 }
 len = wdp - wtrdns;
 if (len > arlen(wtrdns)) {
	assp_log("%p: Internal error: WTR/DNS overflowed buffer\n", tcb);
	assp_error();
 }
 if ((unsigned) (sp - pkt->data) < len) {
	assp_log("%p: Internal error: insufficent space for WTR/DNS\n", tcb);
	assp_error();
 }
 sp -= len;
 memcpy(sp, wtrdns, len);
 len /= 2;			// convert to words
 len++;				// account for option code
 *--sp = ASSP_OPT_WTRDNS & 0xff;
 *--sp = ASSP_OPT_WTRDNS >> 8;
 *--sp = len;
 *--sp = len >> 8;
 return sp;
}

static uint8_t *dec16(uint32_t *v, uint8_t *sp)
{
 *v = (sp[0] << 8) + sp[1];
 return sp + 2;
}

static uint8_t *dec32(uint32_t *v, uint8_t *sp)
{
 *v = (sp[0] << 24) + (sp[1] << 16) + (sp[2] << 8) + sp[3];
 return sp + 4;
}

static uint8_t *enc32r(uint8_t *sp, uint32_t v)
{
 *--sp = v & 0xff;
 *--sp = (v >> 8) & 0xff;
 *--sp = (v >> 16) & 0xff;
 *--sp = (v >> 24) & 0xff;
 return sp;
}

static int sendpkt(TCB *tcb, TCBPATH *path, uint8_t *sp, uint8_t *ep)
{
 if (trace & T_TX) {
	char pktbuf[512];
	decode_str(pktbuf, pktbuf+sizeof(pktbuf), sp, ep);
	assp_log("%p: TX: %s\n", tcb, pktbuf);
 }
 tcb->ackneeded = 0;
 return path->vtbl->netsendpkt(path->pi, sp, ep);
}

static int histsendpkt(TCB *tcb, PACKET *txp)
{
 uint8_t *sp = txp->u.tx.seqopt;
 	// now add the appropriate header, working backwards
	// we don't need to add the end-of-options marker as
	// either it's there already or not needed
 switch (txp->u.tx.ptype) {
 case PKT_FASTACK:
 	*--sp = ASSP_OPT_FASTACK & 0xff;
 	*--sp = ASSP_OPT_FASTACK >> 8;
 	*--sp = 1;			// length
 	*--sp = 0;
	break;
 case PKT_ACKLESS:
 	if (txp->datastart == txp->dataend) {
		*--sp = ASSP_OPT_ACKLESS & 0xff;
		*--sp = ASSP_OPT_ACKLESS >> 8;
		*--sp = 1;			// length
		*--sp = 0;
	}
	break;
 default:;		// no option required
 }
 sp = compose_wtrdns(tcb, txp, sp);
 if (txp->datastart != txp->dataend
 	&& (txp->datastart - sp) & 3) {	// not correctly aligned
	uint8_t *ep = sp + (*sp << 8) + sp[1] + 2;
	memmove(sp - 2, sp, ep - sp);	// move up last option
	*--ep = 0;			// padding
	*--ep = 0;
	if (sp == txp->u.tx.seqopt) txp->u.tx.seqopt -= 2;
	sp--;				// point to length LSB
	if (!++*sp--) ++*sp;		// increment & point to MSB
 }
 *--sp = 0;				// reserved field
 *--sp = 0;
 tcb->snd.lasttxts = ts_now(tcb);
 sp = enc32r(sp, tcb->rcv.lasttxts);	// refts
 sp = enc32r(sp, tcb->snd.lasttxts);	// txts
 sp = enc32r(sp, tcb->rcv.nxt);		// ack
 sp = enc32r(sp, txp->seq);		// seq
 sp = enc32r(sp, tcb->cid);		// cid
 if (txp->u.tx.seqopt != txp->dataend) tcb->stats.numtx_seq++;
 else tcb->stats.numtx_unseq++;
#ifdef ASSP_PATH_ETHSIZE
 txp->u.tx.ethsize = tcb->path->vtbl->ethsize(tcb->path->pi, txp->dataend - sp);
#else
 txp->u.tx.ethsize = ethsize(txp->dataend - sp);
#endif
#if 0
 if (sp < txp->data) {
	fprintf(stderr, "Oops! overwrote start of packet (by %d)\n",
		txp->data - sp);
	abort();
 }
#endif
 return sendpkt(tcb, tcb->path, sp, txp->dataend);
}

static int sendack(TCB *tcb)
{
 PACKET *rst = txpacket0(tcb, 0);
 int sts;
 rst->seq = tcb->snd.nxtseq;
 sts = histsendpkt(tcb, rst);
 txpacket_free(tcb, rst);
 return sts;
}

static int reply(TCB *tcb, RPACKET *rpd, PACKET *txp, uint8_t *sp)
{
 sp = enc32r(sp, rpd->txts);		// refts
 sp = enc32r(sp, rpd->refts);		// txts
 sp = enc32r(sp, rpd->pkt->seq);	// ack
 sp = enc32r(sp, rpd->ack);		// seq
 	// cid
 sp -= 4;
 memcpy(sp, rpd->pkt->rawstart + ASSP_PKTOFS_CID, 4);
 return sendpkt(tcb, tcb->path, sp, txp->dataend);
}

static void alien(TCB *tcb, RPACKET *rpd, char *why)
{
 PACKET *txp;
 uint8_t *sp;
 if (rpd->ackless) return;
 if (trace & T_BADPKT) assp_log("%p: alien(%s)\n", tcb, why);
 txp = txpacket0(tcb, strlen(why)+1);
 sp = txp->u.tx.seqopt;
 strcpy((char *) txp->datastart, why);
 txp->dataend += strlen(why)+1;
 *--sp = 0;			// end of options
 *--sp = 0;
 *--sp = ASSP_OPT_ACKLESS & 0xff;
 *--sp = ASSP_OPT_ACKLESS >> 8;
 *--sp = 1;			// length
 *--sp = 0;
 *--sp = ASSP_OPT_RST & 0xff;
 *--sp = ASSP_OPT_RST >> 8;
 *--sp = 1;			// length
 *--sp = 0;
 *--sp = 0;			// reserved field
 *--sp = 0;
 reply(tcb, rpd, txp, sp);
 txpacket_free(tcb, txp);
 tcb->stats.numtx_unseq++;
}

static ABSTIME pkt_rtot(TCB *tcb, PACKET *pkt)
{
 if (pkt->u.tx.ptype < PKT_NORMAL) return time_never();
 else {
	uint32_t rtouS = tcb->rtt.av + tcb->rtt.var * 4;
	if (tcb->rtt.var * 4 < tcb->rtt.av) rtouS = tcb->rtt.av * 2;
			// cannot have timeout longer than assumed MSL
	if (rtouS > 120000000) rtouS = 120000000;
	if (rtouS < 60000) rtouS = 60000;	// timer granularity
	return time_add(pkt->u.tx.txtim, time_longuS2rel(rtouS));
 }
}

static void verify_tmo(TCB *tcb)
{
#if 0
 PACKET *txp = tcb->snd.sent_tmoq.first;
 if (txp) {
	ABSTIME rto = pkt_rtot(tcb, txp);
	if (time_cmp(rto, tcb->nexttimer) < 0) {
		char s_next[64];
		char s_rto[64];
		time_string(s_next, s_next + arlen(s_next),
			time_until(tcb->nexttimer));
		time_string(s_rto, s_rto + arlen(s_rto), time_until(rto));
		assp_log("%p: first rto is %s but next timer at %s\n", tcb,
			s_rto, s_next);
		assp_error();
	}
	if (!tcb->curtmo.fn) {
		assp_log("%p: verify_tmo(): no timer but non-empty queue\n");
		assp_error();
	}
 } else {
	if (tcb->curtmo.fn) {
		assp_log("%p: verify_tmo(): timer but empty queue\n");
		assp_error();
	}
 }
#else
 (void) tcb;
#endif
}

static int senddata(TCB *tcb, PACKET *pkt)
{
 int r;
 // assert pkt->status == TS_WTR
 pkt->u.tx.txtim = time_when(time_longS2rel(0));
 if (pkt->u.tx.ptype >= PKT_NORMAL) {
	pkt->u.tx.status = TS_SENTT;
	txq_enqueue(&tcb->snd.sent_tmoq, pkt);
 } else {
	pkt->u.tx.status = TS_SENTA;
	tcb->snd.sent_ackl++;
 }
 txq_verify(&tcb->snd.sent_tmoq, TS_SENTT);
 r = histsendpkt(tcb, pkt);
 pkt->u.tx.txfs = tcb->snd.flightsize += pkt->u.tx.ethsize;
 verify_q(tcb);
 return r;
}

static int check_txq(TCB *tcb)
{
 int sts = 0;
 if (trace & T_TXQ) show_txq(tcb);
 verify_q(tcb);
#ifndef CA_PACE_TX
 if (time_isnever(cc_txtime(tcb))) return sts;
#endif
 for (;;) {
	PACKET *txp = tcb->snd.txq.first;
	if (!txp) break;
#ifdef CA_PACE_TX
	if (time_sgn(time_until(cc_txtime(tcb))) > 0) break;
#endif
	txq_dequeue(&tcb->snd.txq, txp);
	txq_verify(&tcb->snd.txq, TS_WTR);
		// txp must be WTR
	if (tcb->pstate != PS_RST_WAIT) {
		++txp->u.tx.retxcount;
#if 0
		if (txp->u.tx.retxcount == 10000) { // min retx time is 60ms, so 10000 gives at least 10 mins
			assp_abort(tcb, txp, "too many retries"); // NB sets tcp->pstate to PS_RST_WAIT
		}
#endif
		if (senddata(tcb, txp)) sts = 1;
		tcb->stats.numtx_seqdup++;
	}
 }
 verify_q(tcb);
 return sts;
}

static void append_txq(TCB *tcb, PACKET *pkt)
{
 pkt->u.tx.status = TS_WTR;
 txq_enqueue(&tcb->snd.txq, pkt);
 txq_verify(&tcb->snd.txq, TS_WTR);
}

static int check_tmoq(TCB *tcb);
static void check_valid(void *arg);

static void t_check_tmoq(void *arg)
{
 TCB *tcb = arg;
 int sts;
 check_tcb(tcb);
 tcb->nexttimer = time_never();
 if (tcb->pstate == PS_RST_WAIT) sts = 0;
 else {
	 int old = tcb->since_valid;
	 tcb->since_valid = 0; // check_valid timer isn't running as a timeout timer was
	 sts = check_tmoq(tcb);
	 tcb->since_valid = old;
 }
 commit_tcb(tcb);
 if (!sts && time_isnever(tcb->nexttimer) && tcb->since_valid) check_valid(tcb);
 if (sts) assp_abort(tcb, 0, "cannot send re-transmission");
}

static int check_tmoq(TCB *tcb)
{
 ABSTIME nxttmo = time_never();
 unsigned ntmo = 0;
 int sts = 0;
 for (;;) {
	PACKET *txp = tcb->snd.sent_tmoq.first;
	ABSTIME rto;
	verify_q(tcb);
	if (!txp) break;
	rto = pkt_rtot(tcb, txp);
	if (time_sgn(time_until(rto)) > 0) {
		if (time_cmp(rto, nxttmo) < 0) nxttmo = rto;
		break;
	}
	txq_dequeue(&tcb->snd.sent_tmoq, txp);
	tcb->snd.flightsize -= txp->u.tx.ethsize;
	txq_verify(&tcb->snd.sent_tmoq, TS_SENTT);
		// txp must be SENT
	append_txq(tcb, txp);
	if (trace & T_LOST) assp_log("%p: lost packet: seq %u  txtim %" PRIu64 " now %" PRIu64 "\n", tcb, txp->seq, txp->u.tx.txtim, time_when(time_longS2rel(0)));
	ntmo++;
 }
 if (ntmo) {
	PACKET *txp;
	if (check_txq(tcb)) sts = 1;
	txp = tcb->snd.sent_tmoq.first;
	if (txp) {
		ABSTIME rto = pkt_rtot(tcb, txp);
		if (time_cmp(rto, nxttmo) < 0) nxttmo = rto;
	}
 }
 if (time_cmp(nxttmo, tcb->nexttimer) < 0 && tcb->pstate < PS_RST_WAIT) {
	if (!time_isnever(tcb->nexttimer) || tcb->since_valid) {
		tcb->tq->vtbl->cancel(tcb->tq->tqi, &tcb->curtmo);
	}
	tcb->nexttimer = nxttmo;
	tcb->tq->vtbl->start(tcb->tq->tqi, &tcb->curtmo, t_check_tmoq, tcb, nxttmo);
 }
 verify_tmo(tcb);
 return sts;
}

// returns non-zero on socket send error
static int composedata(TCB *tcb, PACKET *txp)
{
 if (txp->u.tx.ptype == PKT_NODATA) {
	assp_log("%p: Internal error: composedata with NODATA pkt\n", tcb);
	assp_error();
 }
 if (!tcb->snd.extrawind) {
	assp_log("%p: Internal error: sending with extrawind=0\n", tcb);
	assp_error();
 }
 txp->seq = tcb->snd.nxtseq++;
 if (tcb->snd.maxseq) tcb->snd.maxseq->next_byseq = txp;
 else tcb->snd.minseq = txp;
 tcb->snd.maxseq = txp;
 txp->next_byseq = 0;
 tcb->snd.extrawind--;
 txp->u.tx.status = TS_WTR;
 txp->u.tx.retxcount = 0u;
 return senddata(tcb, txp);
}

#ifdef CA_PACE_TX
static int maybe_newtx(TCB *tcb);

static void t_maybe_newtx(void *arg)
{
 TCB *tcb = arg;
 check_tcb(tcb);
 tcb->txreadytime = time_never();
 maybe_newtx(tcb);
 commit_tcb(tcb);
}
#endif

static int maybe_newtx(TCB *tcb)
{
 int sts = 0;
	// only in suitable state
 switch (tcb->pstate) {
 case PS_CLOSED:
 case PS_WAIT_COOKIE:
 case PS_FIN_WAIT:
 case PS_LAST_ACK:
 case PS_RST_WAIT:
 case PS_TIME_WAIT:
 case PS_ESTABLISHING:
	return sts;
 case PS_ESTABLISHED:
 case PS_CLOSE_WAIT:
	break;
 }
 for (;;) {
	ABSTIME txreadytime;
	int flags = 0;
	PACKET *txp;
		// amount of flow control credit we have
	if (tcb->snd.extrawind < 1) {
		if (trace & T_CWOK) assp_log("%p: extrawind = %d\n", tcb,
			tcb->snd.extrawind);
		break;	// no window - cannot send
	}
		// permitted by congestion control
	txreadytime = cc_txtime(tcb);
	if (time_isnever(txreadytime)) break;	// must wait for ack
#ifdef CA_PACE_TX
	if (time_sgn(time_until(txreadytime)) > 0) {
			// supposed to wait, but if the delay is too short
			// (less than 10 ms) we assume that it's not worth
			// waiting, since the operating system may round
			// up the delay and make us wait far too long.
			// however, if the delay is until after a retx
			// timeout, we assume the ack would wake us
		if (((!time_isnever(tcb->nexttimer)
			&& time_cmp(txreadytime, tcb->nexttimer) >= 0))
			|| time_rel2longmS(time_until(txreadytime)) > 10) {
			if (!time_isnever(tcb->txreadytime)) {
				if (time_cmp(tcb->txreadytime, txreadytime) <= 0) {
					break;
				}
				tcb->tq->vtbl->cancel(tcb->tq->tqi, &tcb->readytmo);
			}
			tcb->txreadytime = txreadytime;
			if (!time_isnever(tcb->txreadytime)) {
				tcb->tq->vtbl->start(tcb->tq->tqi, &tcb->readytmo, t_maybe_newtx, tcb, tcb->txreadytime);
			}
			break;
		}
	}
#endif
	commit_tcb(tcb);
	if (tcb->ackneeded) flags = ASSP_F_MUSTSEND;
	txp = tcb->vtbl->nexttx(tcb->cnx, &flags);
	check_tcb(tcb);
	if (txp) {
		txp->u.tx.seqopt = txp->datastart;
		txp->u.tx.txfs = 0;
	}
	if (trace & T_TXQ) show_txq(tcb);
	if (flags & ASSP_F_EOF) {
		enum pstate newstate = PS_FIN_WAIT;
		if (!txp) txp = txpacket0(tcb, 0);
		if (tcb->pstate == PS_CLOSE_WAIT) newstate = PS_LAST_ACK;
		if (txp->u.tx.seqopt != txp->dataend) {
			*--txp->u.tx.seqopt = 0;	// marker after options
			*--txp->u.tx.seqopt = 0;
		}
		*--txp->u.tx.seqopt = ASSP_OPT_FIN & 0xff;
		*--txp->u.tx.seqopt = ASSP_OPT_FIN >> 8;
		*--txp->u.tx.seqopt = 1;
		*--txp->u.tx.seqopt = 0;		// length
		txp->u.tx.ptype = PKT_FASTACK;
		if (composedata(tcb, txp)) sts = 1;
			/* NB: can never go to TIME_WAIT as we're just about
			 * to send another packet which must be acknowledged
			 */
		setpstate(tcb, newstate);
		break;
	} else if (!txp) {
		break;	// nothing to send
	} else if (txp->u.tx.seqopt == txp->dataend) {
		assp_log("%p: Error: empty data packet\n", tcb);
		assp_error();
	}
	if (flags & ASSP_F_FASTACK) txp->u.tx.ptype = PKT_FASTACK;
	else if (flags & ASSP_F_ACKLESS) txp->u.tx.ptype = PKT_ACKLESS;
	else txp->u.tx.ptype = PKT_NORMAL;
	*--txp->u.tx.seqopt = 0;    // marker after options
	*--txp->u.tx.seqopt = 0;
	if (composedata(tcb, txp)) sts = 1;
 }
 if (trace & T_TXQ) show_txq(tcb);
 check_tmoq(tcb);
 verify_tmo(tcb);
 return sts;
}

static void init_rtt(TCB *tcb, uint32_t rtt)
{
	// handle first packet specially
 tcb->rtt.av = rtt;
 tcb->rtt.var = rtt / 2;
 tcb->rtt.min = rtt;
 if (trace & T_RTT) assp_log("%p: init_rtt %ld\n", tcb, (long) rtt);
}

static void update_rtt(TCB *tcb, PACKET *txp, RPACKET *rpd, uint32_t rtt)
{
 enum ptype ptype = txp->u.tx.ptype;
 char *ismin = "";
 int32_t err;
 if (ptype < PKT_NORMAL) return;
 if (ptype >= NUM_PKT) assp_error();
 if (rpd->refts == tcb->rcv.lastrefts) return;
 tcb->rcv.lastrefts = rpd->refts;
 if (!rtt) rtt = 1;	// don't believe zero RTT
 if (rtt >= 2 * CLK_MSL) return;		// impossible rtt
 if (rtt < tcb->rtt.min) ismin = "min", tcb->rtt.min = rtt;
 err = rtt - tcb->rtt.av;
 tcb->rtt.av += err / 8;
 if (err < 0) err = -err;
 err -= tcb->rtt.var;	// NB: signed result
 tcb->rtt.var += err / 4;
 if (trace & T_RTT) assp_log("%p: %srtt %ld: avrtt=%ld uS, rttvar=%ld uS\n", tcb, ismin, (long) rtt, tcb->rtt.av, tcb->rtt.var);
 check_tmoq(tcb);
}

static void check_extrawind(TCB *tcb)
{
 PACKET *txp;
 for (txp=tcb->snd.minseq; txp; txp=txp->next_byseq) {
	if (!tcb->snd.extrawind) return;
	switch (txp->u.tx.status) {
	case TS_WTR:	// already on txq
		break;
	case TS_DNS:
		append_txq(tcb, txp);
		break;
	case TS_SENTT:	// in transit
		break;
	case TS_SENTA:	// in transit
		break;
	}
	tcb->snd.extrawind--;
 }
}

static unsigned check_wtrdns(TCB *tcb, RPACKET *rpd)
{
 unsigned wdlen = rpd->wtr_dns_len;
 PACKET *txp = tcb->snd.minseq;
 uint8_t *wdp = rpd->wtr_dns;
 unsigned numacked = 0;
 if (!wdlen) {
	check_extrawind(tcb);
	return 0;
 }
 tcb->snd.extrawind = 0;
 verify_q(tcb);
 for (;;) {
	int wtr = 0;
	int dns = 0;
	if (!wdlen) return numacked;
	do {
		wdlen--;
		wtr += *wdp++;
		dns += *wdp++;
	} while (wdlen && (wtr == 0 || dns == 0));
	for (; wtr; wtr--, txp=txp->next_byseq) {	// WTR
		if (!txp) {	// run out of tx
			tcb->snd.extrawind = wtr;
			if (trace & T_FLOW) assp_log("%p: extrawind -> %d\n", tcb, wtr);
			return numacked;
		}
		switch (txp->u.tx.status) {
		case TS_WTR:	// already on txq
			if (trace & T_SACK) assp_log("%p: %u: WTR (same)\n", tcb, txp->seq);
			break;
		case TS_DNS:
			if (trace & T_SACK) assp_log("%p: %u: DNS -> WTR\n", tcb, txp->seq);
			append_txq(tcb, txp);
			break;
		case TS_SENTT:	// in transit
			if (dns && !txp->u.tx.retxcount) {
				if (trace & (T_SACK | T_LOST)) {
					char tbuf[512];
					time_string(tbuf, tbuf+sizeof(tbuf), time_until(pkt_rtot(tcb, txp)));
					assp_log("%p: %u: WTR (SENT & lost) tmo=%s\n", tcb, txp->seq, tbuf);
				}
				tcb->snd.flightsize -= txp->u.tx.ethsize;
				txq_dequeue(&tcb->snd.sent_tmoq, txp);
					// txp must be SENT
				append_txq(tcb, txp);
			} else {
				if (trace & T_SACK) {
					char tbuf[512];
					time_string(tbuf, tbuf+sizeof(tbuf), time_until(pkt_rtot(tcb, txp)));
					assp_log("%p: %u: WTR (already SENT) tmo=%s\n", tcb, txp->seq, tbuf);
				}
			}
			break;
		case TS_SENTA:	// in transit
			if (dns && !txp->u.tx.retxcount) {
				if (trace & (T_SACK | T_LOST)) {
					char tbuf[512];
					time_string(tbuf, tbuf+sizeof(tbuf), time_until(pkt_rtot(tcb, txp)));
					assp_log("%p: %u: WTR (SENT & lost) tmo=%s\n", tcb, txp->seq, tbuf);
				}
				tcb->snd.sent_ackl--;	// because it was lost
					// txp must be SENT
				append_txq(tcb, txp);
			} else {
				if (trace & T_SACK) {
					char tbuf[512];
					time_string(tbuf, tbuf+sizeof(tbuf), time_until(pkt_rtot(tcb, txp)));
					assp_log("%p: %u: WTR (already SENT) tmo=%s\n", tcb, txp->seq, tbuf);
				}
			}
			break;
		}
		verify_q(tcb);
	}
	for (; dns; dns--, txp=txp->next_byseq) {	// DNS
		if (!txp) return numacked;	// run out of tx
		verify_q(tcb);
		switch (txp->u.tx.status) {
		case TS_WTR:	// on txq
			if (trace & T_SACK) assp_log("%p: %u: WTR -> DNS\n", tcb, txp->seq);
			txq_dequeue(&tcb->snd.txq, txp);
			txq_verify(&tcb->snd.txq, TS_WTR);
			break;
		case TS_DNS:	// status unchanged
			if (trace & T_SACK) assp_log("%p: %u: DNS (same)\n", tcb, txp->seq);
			break;
		case TS_SENTT:	// in transit assume SACK
			if (trace & T_SACK) {
				char tbuf[512];
				time_string(tbuf, tbuf+sizeof(tbuf), time_until(pkt_rtot(tcb, txp)));
				assp_log("%p: %u: SENTT -> DNS tmo=%s\n", tcb, txp->seq, tbuf);
			}
			tcb->snd.flightsize -= txp->u.tx.ethsize;
			txq_dequeue(&tcb->snd.sent_tmoq, txp);
			txq_verify(&tcb->snd.sent_tmoq, TS_SENTT);
			numacked++;
			break;
		case TS_SENTA:	// in transit assume SACK
			if (trace & T_SACK) {
				char tbuf[512];
				time_string(tbuf, tbuf+sizeof(tbuf), time_until(pkt_rtot(tcb, txp)));
				assp_log("%p: %u: SENTA -> DNS tmo=%s\n", tcb, txp->seq, tbuf);
			}
			numacked++;
			tcb->snd.flightsize -= txp->u.tx.ethsize;
			tcb->snd.sent_ackl--;	// has arrived
			break;
		}
		txp->u.tx.status = TS_DNS;
		verify_q(tcb);
	}
 }
}

#ifdef BPMS
static unsigned long filt(unsigned long old, unsigned long v)
{
 return (old * 15 + v) >> 4;
}
#else
	// filter new = (old*15 + v) / 16
static FLOAT filt(FLOAT old, FLOAT v)
{
 return float_shl(
		float_add(
			float_mul(old, float_int32(15)),
			v),
		-4);
}
#endif

static void final_ack(TCB *tcb, RPACKET *rpd)
{
 uint32_t numacked = rpd->ack - tcb->snd.una;
 uint32_t bits_acked = 0;
 uint32_t rtt = rpd->rxtim - rpd->refts;
 uint32_t pkttxts = rpd->txts;
 for (; numacked--; ) {
	PACKET *tp = tcb->snd.minseq;
	verify_q(tcb);
	tcb->snd.minseq = tp->next_byseq;
	update_rtt(tcb, tp, rpd, rtt);
	verify_q(tcb);
	tcb->snd.una++;
	if (trace & T_TXTIM) assp_log("%p: dtime %ld\n", tcb, -time_rel2longuS(time_until(tp->created)));
	switch (tp->u.tx.status) {
	case TS_WTR:
		txq_dequeue(&tcb->snd.txq, tp);
		txq_verify(&tcb->snd.txq, TS_WTR);
		break;
	case TS_SENTT:
		tcb->snd.flightsize -= tp->u.tx.ethsize;
		bits_acked += tp->u.tx.ethsize;
		txq_dequeue(&tcb->snd.sent_tmoq, tp);
		txq_verify(&tcb->snd.sent_tmoq, TS_SENTT);
		break;
	case TS_SENTA:
		tcb->snd.flightsize -= tp->u.tx.ethsize;
		bits_acked += tp->u.tx.ethsize;
		tcb->snd.sent_ackl--;	// has arrived
		break;
	case TS_DNS:
		break;
	}
	txpacket_free_user(tcb, tp, 1);
 }
 if (!tcb->snd.minseq) tcb->snd.maxseq = 0;
 switch (tcb->pstate) {
 case PS_ESTABLISHING:
	 setpstate(tcb, PS_ESTABLISHED);
	 // fall thru
 case PS_ESTABLISHED:
 case PS_FIN_WAIT:
 case PS_CLOSE_WAIT:
	if (!time_isnever(tcb->nexttimer)) {
			// might not be running if acknowlegded packets
			// were all ACKLESS
		tcb->tq->vtbl->cancel(tcb->tq->tqi, &tcb->curtmo);
		tcb->nexttimer = time_never();
		if (tcb->since_valid) {
			commit_tcb(tcb);
			check_valid(tcb);
			check_tcb(tcb);
		}
	}
	check_tmoq(tcb);
	break;
 case PS_LAST_ACK:
		 // must have had timer running, since FIN cannot
		 // be ACKLESS if we have already got FIN
	tcb->tq->vtbl->cancel(tcb->tq->tqi, &tcb->curtmo);
	tcb->nexttimer = time_never();
	if (tcb->snd.una == tcb->snd.nxtseq) {
		tcb->since_valid = 0;
		time_wait(tcb);
		return;
	}
	check_tmoq(tcb);
	break;
 case PS_WAIT_COOKIE:
 case PS_CLOSED:
 case PS_RST_WAIT:
 case PS_TIME_WAIT:
	;
 }
 if (bits_acked) {
	ABSTIME now = time_now();
	RELTIME sinceack = time_sub(now, tcb->ca.lastack);
	int_fast64_t sinceack_uS = time_rel2i64uS(sinceack);
#ifdef BPMS
	unsigned long thisbpms = bits_acked * 1000;
	uint32_t deltatxts = pkttxts - tcb->ca.lastack_txts;
	if (deltatxts > sinceack_uS) {
		sinceack_uS = deltatxts;
	}
	if (sinceack_uS) thisbpms /= sinceack_uS; // if sinceack_uS == 0 treat it as == 1
	tcb->ca.bpms = filt(tcb->ca.bpms, thisbpms);
	tcb->ca.lastack = now;
	tcb->ca.lastack_txts = pkttxts;
	if (trace & T_CONGAVOID) {
		assp_log("%p: CA: %" PRId64 "/%u = %g Mb/s -> %g Mb/s\n", tcb, sinceack_uS, bits_acked, thisbpms / 1000.0, tcb->ca.bpms / 1000.0);
	}
#else
	FLOAT upb = float_div(float_int64e(sinceack_uS, 0), float_int32(bits_acked));
	tcb->ca.lastack = now;
	tcb->ca.usperbit = filt(tcb->ca.usperbit, upb);
	if (trace & T_CONGAVOID) {
		double upb_f = float_todouble(upb);
		double tupb_f = float_todouble(tcb->ca.usperbit);
		assp_log("%p: CA: %" PRId64 "/%u = %g us/b = %g Mb/s -> %g Mb/s\n", tcb, sinceack_uS, bits_acked, upb_f, 1.0 / upb_f, 1.0 / tupb_f);
	}
#endif
#if 0
	if (sinceack_uS) {
		double bw = 1000000.0 * bits_acked / (double) sinceack_uS;
	}
#endif
 }
}

static void handle_ack(TCB *tcb, RPACKET *rpd)
{
 uint32_t numacked = rpd->ack - tcb->snd.una;
 uint32_t totacked = numacked;
 verify_q(tcb);
 //fflush(stdout);
 tcb->snd.extrawind += numacked;
 if (numacked) final_ack(tcb, rpd);
 totacked += check_wtrdns(tcb, rpd);
 verify_q(tcb);
 check_txq(tcb);
}

static void send_rst_reply(TCB *tcb)
{
 PACKET *txp = txpacket0(tcb, 0);
 uint8_t *sp = txp->u.tx.seqopt;
 *--sp = ASSP_OPT_ACKLESS & 0xff;
 *--sp = ASSP_OPT_ACKLESS >> 8;
 *--sp = 1;			// length
 *--sp = 0;
 *--sp = ASSP_OPT_RST & 0xff;
 *--sp = ASSP_OPT_RST >> 8;
 *--sp = 1;			// length
 *--sp = 0;
 *--sp = 0;			// reserved field
 *--sp = 0;
 sp = enc32r(sp, tcb->rcv.lasttxts);	// REFTS
 sp = enc32r(sp, ts_now(tcb));	// TXTS
 sp = enc32r(sp, tcb->rcv.nxt);		// ack
 sp = enc32r(sp, tcb->snd.nxtseq);	// seq
 sp = enc32r(sp, tcb->cid);		// cid
 sendpkt(tcb, tcb->path, sp, txp->u.tx.seqopt);
 txpacket_free(tcb, txp);
 tcb->stats.numtx_unseq++;
}

static void resend_abort(void *arg);

static void send_abort_pkt(TCB *tcb, PACKET *txp)
{
 uint8_t *sp = txp->u.tx.seqopt;
 *--sp = ASSP_OPT_FASTACK & 0xff;
 *--sp = ASSP_OPT_FASTACK >> 8;
 *--sp = 1;			// length
 *--sp = 0;
 *--sp = ASSP_OPT_RST & 0xff;
 *--sp = ASSP_OPT_RST >> 8;
 *--sp = 1;			// length
 *--sp = 0;
 *--sp = 0;			// reserved field
 *--sp = 0;
 sp = enc32r(sp, tcb->rcv.lasttxts);	// REFTS
 sp = enc32r(sp, ts_now(tcb));	// TXTS
 sp = enc32r(sp, tcb->rcv.nxt);		// ack
 sp = enc32r(sp, tcb->snd.nxtseq);	// seq
 sp = enc32r(sp, tcb->cid);		// cid
 sendpkt(tcb, tcb->path, sp, txp->u.tx.seqopt);
 txpacket_free(tcb, txp);
}

static void rst_wait(TCB *tcb, PACKET *txp)
{
 uint32_t rtouS = tcb->rtt.av + tcb->rtt.var * 4;
 if (tcb->rtt.var * 4 < tcb->rtt.av) rtouS = tcb->rtt.av * 2;
			// cannot have timeout longer than assumed MSL
 if (rtouS > 120000000) rtouS = 120000000;
 if (rtouS < 60000) rtouS = 60000;	// timer granularity
 tcb->stats.numtx_unseq++;
 tcb->rst_tmo = rtouS;
 setpstate(tcb, PS_RST_WAIT);
 send_abort_pkt(tcb, txp);
 tcb->time_wait_end = time_when(RST_LEN);
 if (tcb->curtmo.fn) {
	assp_log("%p: rst_wait(): timer in use: %p\n", tcb, tcb->curtmo.fn);
	assp_error();
 }
 	// resend every second while TIME_WAIT timer runs or until acked
 tcb->tq->vtbl->start(tcb->tq->tqi, &tcb->curtmo, resend_abort, tcb, time_when(time_longuS2rel(tcb->rst_tmo)));
}

static void resend_abort(void *arg)
{
 TCB *tcb = arg;
 check_tcb(tcb);
 if (tcb->pstate == PS_RST_WAIT) {
	if (time_sgn(time_until(tcb->time_wait_end)) <= 0) {
		time_wait(tcb);
	} else {
		tcb->rst_tmo *= 2;
		if (tcb->rst_tmo > 120000000) tcb->rst_tmo = 120000000;
		send_abort_pkt(tcb, txpacket0(tcb, 0));
			// resend every second while TIME_WAIT timer runs or until acked
		tcb->tq->vtbl->start(tcb->tq->tqi, &tcb->curtmo, resend_abort, tcb, time_when(time_longuS2rel(tcb->rst_tmo)));
	}
 }
 commit_tcb(tcb);
}

static void rx_peek(TCB *tcb, PACKET *pkt)
{
 commit_tcb(tcb);
 tcb->vtbl->rx_peek(tcb->cnx, pkt->seq - tcb->rcv.nxt, pkt);
 check_tcb(tcb);
}

// Entry: rawstart and dataend are valid in rxp
// Exit: datastart is also valid in rxp
static void decode_pkt(TCB *tcb, RPACKET *pkt, PACKET *rxp)
{
 uint8_t *nextopt = rxp->rawstart + ASSP_PKTOFS_OPTIONS;
 pkt->pkt = rxp;
 dec32(&rxp->seq, rxp->rawstart + ASSP_PKTOFS_SEQ);
 dec32(&pkt->ack, rxp->rawstart + ASSP_PKTOFS_ACK);
 dec32(&pkt->txts, rxp->rawstart + ASSP_PKTOFS_TXTS);
 dec32(&pkt->refts, rxp->rawstart + ASSP_PKTOFS_REFTS);
 for (;;) {
	uint8_t *sp;
	uint32_t optcode;
	uint32_t optlen;
	if (nextopt == rxp->dataend) {		// no data
		rxp->datastart = nextopt;
		break;
	}
		// NB: might have only one byte left here
		// if so, overflow check will catch it as lengths
		// can never decode as negative
	sp = dec16(&optlen, nextopt);
	if (rxp->dataend - sp + 0u < optlen * 2) { // overflows packet
		tcb->stats.numrx_badopt_unseq++;
		if (trace & T_BADPKT) assp_log("%p: options overflow\n", tcb);
		pkt->error = "options overflow";
		return;
	}
	nextopt += 2;
	if (!optlen) {				// no more options
		rxp->datastart = nextopt;
		if (nextopt < rxp->dataend) pkt->hasseq = 1;
		break;
	}
	sp = dec16(&optcode, sp);
	nextopt += optlen * 2;
	switch (optcode) {
	case ASSP_OPT_KEYCODE:
		pkt->keycode = sp;
		pkt->keycodelen = nextopt - sp;
		break;
	case ASSP_OPT_COOKIE:
		pkt->cookie = sp;
		pkt->cookielen = nextopt - sp;
		break;
	case ASSP_OPT_RST: pkt->isrst = 1; break;
	case ASSP_OPT_FASTACK: pkt->fastack = 1; break;
	case ASSP_OPT_ACKLESS: pkt->ackless = 1; break;
	case ASSP_OPT_FIN: rxp->u.rx.isfin = 1; pkt->hasseq = 1; break;
	case ASSP_OPT_SYN: pkt->issyn = 1; pkt->hasseq = 1; break;
	case ASSP_OPT_WTRDNS:
		pkt->wtr_dns = sp;
		pkt->wtr_dns_len = (nextopt - sp) / 2;
		break;
	case ASSP_OPT_GETCOOKIE: pkt->getcookie = 1; break;
	case ASSP_OPT_USECOOKIE: pkt->usecookie = sp; break;
	default:
			// note: don't return mmediately with failure if we
			// see an unknown critical option
			// since ACKLESS might follow preventing
			// us from sending RST
		if (optcode & ASSP_OPTION_CRIT) {
			if (trace & T_BADPKT) assp_log("%p: Unknown option: 0x%x\n", tcb, optcode);
			pkt->error = "unknown critical option";
		}
		break;
	}
 }
 if (pkt->error) tcb->stats.numrx_unkopt_unseq++;
}

static int rxdeliver(TCB *tcb, PACKET *pkt)
{
 int isfin = pkt->u.rx.isfin;
 switch (tcb->pstate) {
 case PS_CLOSED:
 case PS_WAIT_COOKIE:
 case PS_ESTABLISHING:
#ifdef PROVIDE_TRACE
	assp_log("%p: Internal error: seq in %s\n", tcb, pstate(tcb->pstate));
#endif
	assp_error();
	break;
 case PS_ESTABLISHED:
 case PS_FIN_WAIT:
	commit_tcb(tcb);
	tcb->vtbl->deliver(tcb->cnx, pkt);
	check_tcb(tcb);
	if (isfin) {
		tcb->vtbl->eof(tcb->cnx);
		check_tcb(tcb);
		if (tcb->pstate == PS_ESTABLISHED) {
			setpstate(tcb, PS_CLOSE_WAIT);
		} else {
			if (tcb->since_valid) {
				tcb->tq->vtbl->cancel(tcb->tq->tqi, &tcb->curtmo);
				tcb->since_valid = 0;
			}
			setpstate(tcb, PS_LAST_ACK);
			if (tcb->snd.nxtseq == tcb->snd.una) time_wait(tcb);
		}
	}
	break;
 case PS_TIME_WAIT:
 case PS_RST_WAIT:
 case PS_CLOSE_WAIT:
 case PS_LAST_ACK:
	{
	 RPACKET rpd;
	 tcb->stats.numrx_sync_afterfin++;
	 if (trace & T_BADPKT) assp_log("%p: alien! seq after FIN\n", tcb);
	 memset(&pkt, 0, sizeof(pkt));
	 decode_pkt(tcb, &rpd, pkt);
	 alien(tcb, &rpd, "data after FIN");
	}
	return 1;
 }
 return 0;
}

static PK_CLASS classify(TCB *tcb, RPACKET *rpd)
{
 uint32_t deltatx = rpd->txts - tcb->rcv.maxtxts;	// mod 2^32, of course
 uint32_t deltarx = rpd->rxtim - tcb->rcv.maxrxtim;
 if (deltatx & 0x80000000) {	// gone backwards
	uint32_t maxdtx = deltatx + CLK_EPSTX + CLK_EPSRX + CLK_MSL;
	uint32_t mindtx = deltatx - CLK_EPSTX - CLK_EPSRX - CLK_MSL;
	if (deltarx - mindtx > maxdtx - mindtx) { // alien
		if (trace & T_BADPKT) assp_log("%p: %d <= dT = %d <= %d, dR = %d\n",
			tcb, mindtx, deltatx, maxdtx, deltarx);
		rpd->error = "bogus TXTS reduction";
		return PK_ALIEN;
	}
	return PK_STALE;
 } else if (deltatx) {	// gone forwards
	uint32_t mindtx = deltatx - CLK_EPSTX - CLK_EPSRX - CLK_MSL;
	uint32_t maxdtx = deltatx + CLK_EPSTX + CLK_EPSRX + CLK_MSL;
	uint32_t numacked = rpd->ack - tcb->snd.una;
	if (deltarx - mindtx > maxdtx - mindtx) { // alien
		if (trace & T_BADPKT) assp_log("%p: %d <= dT = %d <= %d, dR = %d\n",
			tcb, mindtx, deltatx, maxdtx, deltarx);
		rpd->error = "bogus TXTS advance";
		return PK_ALIEN;
	}
	if (numacked <= tcb->snd.nxtseq - tcb->snd.una) return PK_FRESH;
	if (trace & T_BADPKT) assp_log("%p: bogus ack, numacked = %d\n", tcb, numacked);
	rpd->error = "bogus ACK";
	return PK_ALIEN;
 } else {	// no change - might be a reply (e.g. RESET)
	uint32_t mindtx = deltatx - CLK_EPSTX - CLK_EPSRX - CLK_MSL;
	uint32_t maxdtx = deltatx + CLK_EPSTX + CLK_EPSRX + CLK_MSL;
	uint32_t numacked = rpd->ack - tcb->snd.una;
	if (deltarx - mindtx < maxdtx - mindtx) {	// plausible
		if (numacked <= tcb->snd.nxtseq - tcb->snd.una) return PK_FRESH;
		return PK_STALE;
	} else { // not spontaneously generated: alien or reply
		uint32_t deltaref = tcb->snd.lasttxts - rpd->refts;
		if (deltaref < 2 * CLK_MSL
			&& !numacked
			&& rpd->pkt->seq == tcb->rcv.nxt) return PK_FRESH;	// maybe reply
		if (trace & T_BADPKT) assp_log("%p: %d <= dT = %d <= %d, dR = %d\n",
			tcb, mindtx, deltatx, maxdtx, deltarx);
		rpd->error = "bogus TXTS advance";
		return PK_ALIEN;
	}
 }
}

static int valid_continuation(TCB *tcb, RPACKET *rpd)
{
 switch (classify(tcb, rpd)) {
 case PK_ALIEN: return 0;
 case PK_FRESH:
	return 1;
 case PK_STALE:
	break;
 }
 return 0;
}

static void handle_seq(TCB *tcb, RPACKET *rpd)
{
 if (!rpd->hasseq) {	// no data - only marker
	tcb->stats.numrx_sync_unseq++;
 } else {
	uint32_t sincelast = rpd->pkt->seq - tcb->rcv.nxt;
	if (sincelast >= tcb->rcv.window) { // stale
		if (trace & T_SEQ) assp_log("%p: out of window: %d > %d\n", tcb, sincelast, tcb->rcv.window);
		tcb->stats.numrx_sync_seq_oow++;
		tcb->ackneeded = 1;
	} else { // fresh
		// FIXME: ensure ack sent if pkt didn't have ACKLESS and
		// we have reached a threshold or remote wouldn't have
		// enough WTR credit.
		tcb->ackneeded = !rpd->ackless;	// just send it always for now
		if (rpd->pkt->seq == tcb->rcv.nxt) {
			PACKET *pkt;
			tcb->stats.numrx_sync_seq_next++;
			rx_peek(tcb, rpd->pkt);
			rxdeliver(tcb, rpd->pkt);
			rpd->pkt = 0;
			tcb->rcv.nxt++;
			pkt = tcb->rcv.rxq.frun;
			if (pkt && pkt->seq == tcb->rcv.nxt) {
				tcb->rcv.rxq.frun = pkt->u.rx.runnext;
				tcb->rcv.nxt += pkt->u.rx.runlen;
				for (; pkt;) {
					PACKET *nx = pkt->next;
					if (rxdeliver(tcb, pkt)) rpd->ackless = 1;
					pkt = nx;
				}
			}
		} else {
			if (enqueue(&tcb->rcv.rxq, rpd->pkt)) {
				rx_peek(tcb, rpd->pkt);
				if (trace & T_SEQ) assp_log("%p: awaiting %u, got +%d DUP\n", tcb,
					tcb->rcv.nxt, rpd->pkt->seq - tcb->rcv.nxt);
					// duplicate
				tcb->stats.numrx_sync_seq_dup++;
			} else {
				if (trace & T_SEQ) assp_log("%p: awaiting %u, got +%d\n", tcb,
					tcb->rcv.nxt, rpd->pkt->seq - tcb->rcv.nxt);
				rpd->pkt = 0;
				tcb->stats.numrx_sync_seq_ooo++;
			}
		}
	}
 }
}

static int sendsyn(TCB *tcb, RPACKET *rpd);

static int handle_synced(TCB *tcb, RPACKET *rpd)
{
 int sts = 0;
 switch (classify(tcb, rpd)) {
 case PK_ALIEN: return sts;
 case PK_FRESH:
	tcb->stats.numrx_sync_fresh++;
	tcb->rcv.lasttxts = rpd->txts;
	tcb->rcv.maxrxtim = ts_now(tcb);
	tcb->rcv.maxtxts = rpd->txts;
	handle_ack(tcb, rpd);
	break;
 case PK_STALE:
	if (trace & T_BADPKT) assp_log("%p: stale\n", tcb);
	tcb->stats.numrx_sync_stale++;
	tcb->rcv.lasttxts = rpd->txts;
		// a RST can only be stale because we already
		// processed a re-transmission of it, so there should
		// never be anything we need to do
	if (rpd->isrst) return 0;
	break;
 }
 if (tcb->since_valid && !--tcb->since_valid && time_isnever(tcb->nexttimer)
 	&& tcb->pstate != PS_TIME_WAIT) {
	 tcb->tq->vtbl->cancel(tcb->tq->tqi, &tcb->curtmo);
 }
 if (rpd->isrst) {
	tcb->rcv.nxt = rpd->pkt->seq;
	if (!time_isnever(tcb->nexttimer) || tcb->since_valid) {
		tcb->tq->vtbl->cancel(tcb->tq->tqi, &tcb->curtmo);
		tcb->nexttimer = time_never();
		tcb->since_valid = 0;
	}
	if (tcb->pstate == PS_ESTABLISHING && rpd->usecookie) {
		tcb->snd.nxtseq--; // we are resending SYN but with different cookie
		discard_txq(tcb);
		sts = sendsyn(tcb, rpd);
	} else {
		if (rpd->ack != tcb->snd.nxtseq) {
			rst_wait(tcb, txpacket0(tcb, 0));
		} else {
			if (!rpd->ackless) send_rst_reply(tcb);
			time_wait(tcb);
		}
	}
 } else {
	tcb->ackneeded = !rpd->ackless && rpd->fastack;
	tcb->sendready = 0;
	handle_seq(tcb, rpd);
	if (maybe_newtx(tcb)) sts = 1;
	tcb->sendready = 1;
	if (tcb->ackneeded && sendack(tcb)) sts = 1;
 }
 return sts;
}

static int handle_rx_time_wait(TCB *tcb, RPACKET *rpd)
{
 tcb->stats.numrx_tw++;
 switch (classify(tcb, rpd)) {
 case PK_ALIEN: return 0;
 case PK_FRESH:
	tcb->stats.numrx_sync_fresh++;
	tcb->rcv.lasttxts = rpd->txts;
	tcb->rcv.maxrxtim = ts_now(tcb);
	tcb->rcv.maxtxts = rpd->txts;
	if (rpd->isrst) {
		tcb->rcv.nxt = rpd->pkt->seq;
	}
	if (rpd->pkt->seq == tcb->rcv.nxt - 1) sendack(tcb);
	else if (!rpd->ackless) send_rst_reply(tcb);
		// restart time wait timer
	time_wait(tcb);
	break;
 case PK_STALE:
	if (trace & T_BADPKT) assp_log("%p: stale\n");
	tcb->stats.numrx_sync_stale++;
	tcb->rcv.lasttxts = rpd->txts;
		// no need to check for RST:
		// a RST can only be stale because we already
		// processed a re-transmission of it, so there should
		// never be anything we need to do
	// also no need to restart time-wait timer, since this
	// packet is out of order and we have already processed a later one
	break;
 }
 return 0;
}

static int handle_rst_wait(TCB *tcb, RPACKET *rpd)
{
 int sts = 0;
 verify_q(tcb);
 switch (classify(tcb, rpd)) {
 case PK_ALIEN: return sts;
 case PK_FRESH:
	tcb->stats.numrx_sync_fresh++;
	tcb->rcv.lasttxts = rpd->txts;
	tcb->rcv.maxrxtim = ts_now(tcb);
	tcb->rcv.maxtxts = rpd->txts;
	handle_ack(tcb, rpd);
	break;
 case PK_STALE:
	tcb->stats.numrx_sync_stale++;
	tcb->rcv.lasttxts = rpd->txts;
		// a RST can only be stale because we already
		// processed a re-transmission of it, so there should
		// never be anything we need to do
		// but can we get a stale reset here?
	if (rpd->isrst) return 0;
	break;
 }
 if (tcb->since_valid) --tcb->since_valid;
	 // since_valid timer better not be running as the rst tmo should be
 if (rpd->isrst) {
	tcb->rcv.nxt = rpd->pkt->seq;
	if (rpd->ack != tcb->snd.nxtseq) {
		//FIXME: resend or could we rely on retransmission of reset
		tcb->tq->vtbl->cancel(tcb->tq->tqi, &tcb->curtmo); // resend requeues timer
		tcb->rst_tmo >>= 1; // don't want this resend to extend timeout, so reduce it first
		resend_abort(tcb);
	} else {
		if (!rpd->ackless) { 
			send_rst_reply(tcb); // reply to valid reset from other end
		}
			// treat it as a reply to our reset
		tcb->tq->vtbl->cancel(tcb->tq->tqi, &tcb->curtmo);
		time_wait(tcb);
	}
 }
 return sts;
}

static void establish(TCB *tcb, enum pstate p, uint32_t estpkttxts)
{
 setpstate(tcb, p);
 tcb->since_valid = 0;
 tcb->ca.lastack = time_now();
#ifdef BPMS
 tcb->ca.bpms = 1; // 1Kb/s
 tcb->ca.lastack_txts = estpkttxts;
#else
 tcb->ca.usperbit = float_int32(1000);	// = 1Kb/s
#endif
 tcb->rcv.window = 64;		// arbitrary window
 tcb->rtt.min = ~0uL;
 tcb->rtt.av = 3000000;		// default 3 S
 tcb->rtt.var = 0;			// for default 3 S
}

static void established(TCB *tcb, uint32_t estpkttxts)
{
 establish(tcb, PS_ESTABLISHED, estpkttxts);
}

static void sendrstcookie(TCB *tcb, RPACKET *rpd)
{
 PACKET *coo = txpacket0(tcb, 0);
 uint8_t *sp = coo->u.tx.seqopt;
 unsigned optlen;
 sp = bake_cookie(tcb, sp, rpd);
 *--sp = ASSP_OPT_USECOOKIE & 0xff;
 *--sp = ASSP_OPT_USECOOKIE >> 8;
 optlen = coo->u.tx.seqopt - sp;
 if (optlen & 1) {
	assp_log("%p: Internal error: odd cookie length: %u\n", tcb, optlen);
	assp_error();
 }
 optlen /= 2;
 *--sp = optlen & 0xff;
 *--sp = optlen >> 8;
 *--sp = ASSP_OPT_ACKLESS & 0xff;
 *--sp = ASSP_OPT_ACKLESS >> 8;
 *--sp = 1;			// length
 *--sp = 0;
 *--sp = ASSP_OPT_RST & 0xff;
 *--sp = ASSP_OPT_RST >> 8;
 *--sp = 1;			// length
 *--sp = 0;
 *--sp = 0;			// reserved field
 *--sp = 0;
 reply(tcb, rpd, coo, sp);
 tcb->stats.numtx_coo++;
 txpacket_free(tcb, coo);
}

static void sendcookie(TCB *tcb, RPACKET *rpd)
{
 PACKET *coo = txpacket0(tcb, 0);
 uint8_t *sp = coo->u.tx.seqopt;
 unsigned optlen;
 sp = bake_cookie(tcb, sp, rpd);
 *--sp = ASSP_OPT_USECOOKIE & 0xff;
 *--sp = ASSP_OPT_USECOOKIE >> 8;
 optlen = coo->u.tx.seqopt - sp;
 if (optlen & 1) {
	assp_log("%p: Internal error: odd cookie length: %u\n", tcb, optlen);
	assp_error();
 }
 optlen /= 2;
 *--sp = optlen & 0xff;
 *--sp = optlen >> 8;
 *--sp = ASSP_OPT_ACKLESS & 0xff;
 *--sp = ASSP_OPT_ACKLESS >> 8;
 *--sp = 1;			// length
 *--sp = 0;
 *--sp = 0;			// reserved field
 *--sp = 0;
 reply(tcb, rpd, coo, sp);
 tcb->stats.numtx_coo++;
 txpacket_free(tcb, coo);
}

static void establish_incoming(TCB *tcb, RPACKET *rpd, int rtt)
{
 int ok;
 commit_tcb(tcb);
 ok = tcb->vtbl->sof(tcb->cnx);
 check_tcb(tcb);
 if (!ok) {
	rpd->error = "rejected";
	tcb->stats.numrx_in_rejected++;
 	return;
 }
 tcb->stats.numrx_in_accepted++;
 established(tcb, rpd->txts);
 dec32(&tcb->cid, rpd->pkt->rawstart + ASSP_PKTOFS_CID);
 tcb->snd.una = tcb->snd.nxtseq = rpd->ack;
 tcb->rcv.nxt = rpd->pkt->seq;
 tcb->snd.basets += rpd->refts - rpd->rxtim;
 tcb->snd.lasttxts = rpd->refts;
 tcb->rcv.lasttxts = tcb->rcv.maxtxts = rpd->txts;
 tcb->rcv.maxrxtim = rpd->rxtim;
 tcb->rcv.lastrefts = rpd->refts;
 tcb->rcv.rxq.frun = 0;
 init_rtt(tcb, rtt);
 if (handle_synced(tcb, rpd)) {		// sends ACK
 	assp_abort(tcb, 0, "cannot send ack");
 }
}

static int sendsyn(TCB *tcb, RPACKET *rpd)
{
 int sts = 0;
 uint32_t usecookielen;
 uint8_t *sp;
 PACKET *txp;
 PACKET *rxp = rpd->pkt;
 establish(tcb, PS_ESTABLISHING, rpd->txts);
 commit_tcb(tcb);
 tcb->vtbl->sof(tcb->cnx);
 check_tcb(tcb);
 rx_peek(tcb, rxp);
 commit_tcb(tcb);
 check_tcb(tcb);
 txp = txpacket0(tcb, 0);
 sp = txp->datastart;
 tcb->stats.numrx_wc_ok++;
 tcb->rcv.window = MAX_RX_WINDOW;		// arbitrary window
 tcb->rcv.nxt = rpd->pkt->seq + 1;
 init_rtt(tcb, rpd->rxtim - rpd->refts);
 *--sp = 0;	// marker after options
 *--sp = 0;
 *--sp = ASSP_OPT_SYN & 0xff;
 *--sp = ASSP_OPT_SYN >> 8;
 *--sp = 1;
 *--sp = 0;			// length
 dec16(&usecookielen, rpd->usecookie - 4);
 usecookielen--;		// allow for size of option code
 sp -= usecookielen * 2;
 memcpy(sp, rpd->usecookie, usecookielen * 2);
 *--sp = ASSP_OPT_COOKIE & 0xff;
 *--sp = ASSP_OPT_COOKIE >> 8;
 *--sp = usecookielen + 1;
 *--sp = (usecookielen + 1) >> 8;
 {
	uint8_t *ep = tcb->path->vtbl->make_keycode(tcb->path->pi, tcb, sp + 4, sp + 4 + usecookielen * 2, sp - 16);
	if (ep != sp - 16) {
		unsigned len = ep - (sp -= 16); // space for MD5
		if (len != 16) memset(ep, 0, 16 - len);
		*--sp = ASSP_OPT_KEYCODE & 0xff;
		*--sp = ASSP_OPT_KEYCODE >> 8;
		*--sp = 9;	// length = 16 bytes = 8 words + code
		*--sp = 0;
	}
 }
 txp->u.tx.seqopt = sp;
 txp->u.tx.ptype = PKT_FASTACK;
 tcb->snd.extrawind = 1; // can only send SYN
 if (composedata(tcb, txp)) sts = 1;
 if (rxp->datastart != rxp->dataend) {
		// NB: don't deliver this until cookie has been copied
	rxdeliver(tcb, rxp);
	rpd->pkt = 0;
 }
 check_tmoq(tcb);
 tcb->sendready = 1;
 return sts;
}

static int handle_rx_wait_cookie(TCB *tcb, RPACKET *rpd)
{
 int sts = 0;
 uint32_t deltaref = tcb->snd.lasttxts - rpd->refts;
 uint32_t deltats = tcb->snd.lasttxts - tcb->snd.basets;
 if (deltaref > deltats
	|| rpd->txts != tcb->rcv.maxtxts
	|| rpd->pkt->seq != tcb->rcv.nxt
	|| rpd->ack != tcb->snd.una) {
	if (trace & T_STALE) assp_log("%p: Expected: S:%u A:%u T:%u R:%u+%u, Got: S%d A%d T%d R%d\n", tcb,
		tcb->rcv.nxt,		// S
		tcb->snd.una,		// A
		tcb->rcv.maxtxts,	// TXTS
		tcb->snd.basets, tcb->snd.lasttxts - tcb->snd.basets,
		rpd->pkt->seq -  tcb->rcv.nxt,	// dS
		rpd->ack - tcb->snd.una,	// dA
		rpd->txts - tcb->rcv.maxtxts,	// dT
		rpd->refts - tcb->snd.basets);	// dR
	if (rpd->refts == tcb->snd.lasttxts) alien(tcb, rpd, "bad sequence in WAIT_COOKIE"); // not just out of date
	tcb->stats.numrx_wc_bogus++;
	return sts;
 }
 if (rpd->isrst) {
	tcb->tq->vtbl->cancel(tcb->tq->tqi, &tcb->curtmo);
	time_wait(tcb);
	tcb->stats.numrx_wc_rst++;
 } else if (rpd->usecookie) {
	tcb->tq->vtbl->cancel(tcb->tq->tqi, &tcb->curtmo);
	sts = sendsyn(tcb, rpd);
 } else {
 	tcb->stats.numrx_wc_noncoo++;
	if (trace & T_BADPKT) assp_log("%p: alien! no USECOOKIE in wait-cookie\n", tcb);
	alien(tcb, rpd, "no cookie in WAIT_COOKIE");
 }
 return sts;
}

uint32_t assp_rx_cid(PACKET *pkt)
{
 uint32_t cid;
 dec32(&cid, pkt->rawstart + ASSP_PKTOFS_CID);
 return cid;
}

static int check_key(TCB *tcb, RPACKET *pkt)
{
 int rtt = nice_cookie(tcb, pkt->pkt, pkt->cookie, pkt->cookie + pkt->cookielen);
 uint8_t okkey[16];
 unsigned len;
 uint8_t *ep;
 if (rtt <= 0) {
	pkt->error = "bad cookie";
	return rtt;
 }
 ep = tcb->path->vtbl->make_keycode(tcb->path->pi, tcb, pkt->cookie,
  	pkt->cookie + pkt->cookielen, okkey);
 len = &okkey[16] - ep;
 if (len) memset(ep, 0, len);
 len = 16;
 if (len > pkt->keycodelen) len = pkt->keycodelen;
 if (!memcmp(okkey, pkt->keycode, len)) {
	for (ep = okkey + len; ; len++) {
		if (len >= 16) return rtt;
		if (*ep++) break;
	}
 }
 pkt->error = "bad keycode";
 return 0;
}

// On entry, these fields in rxp must be valid: rawstart, dataend
void assp_handle_rx(TCB *tcb, PACKET *rxp)
{
 RPACKET pkt;
 int sts = 0;
 memset(&pkt, 0, sizeof(pkt));
 memset(&rxp->u.rx, 0, sizeof(rxp->u.rx));
 pkt.rxtim = ts_now(tcb);
 check_tcb(tcb);
 randent_addrand(rxp->rawstart, rxp->dataend - rxp->rawstart);
 if (trace & T_RX) {
	char pktbuf[512];
	decode_str(pktbuf, pktbuf+sizeof(pktbuf), rxp->rawstart, rxp->dataend);
	assp_log("%p: RX: %s\n", tcb, pktbuf);
 }
 decode_pkt(tcb, &pkt, rxp);
 if (!pkt.error) {
	if (rxp->dataend != rxp->datastart) tcb->stats.numrx_seq++;
	else tcb->stats.numrx_unseq++;
	switch (tcb->pstate) {
	case PS_CLOSED:
	case PS_TIME_WAIT:
		if (pkt.getcookie) {
			if (tcb->pstate == PS_CLOSED || valid_continuation(tcb, &pkt)) {
				sendcookie(tcb, &pkt);
				tcb->stats.numrx_closed_crq++;
			} else {
				// Note: no response to provoke re-transmission
				// hopefully after time-wait timer has expired
				pkt.error = 0; // prevent any error causing alien reply
				tcb->stats.numrx_tw_crqbad++;
			}
		} else if (pkt.issyn) {
			int rtt = check_key(tcb, &pkt);
			if (rtt == ERR_COOKIE_EXPIRED) {
				// cookie out of date!!
				pkt.error = 0; // clear error
				sendrstcookie(tcb, &pkt); // send a new cookie
			} else if (rtt > 0) {
				if (tcb->pstate == PS_TIME_WAIT) {
					tcb->tq->vtbl->cancel(tcb->tq->tqi, &tcb->curtmo);
				}
				establish_incoming(tcb, &pkt, rtt);
			}
		} else {
			if (tcb->pstate == PS_TIME_WAIT) {
				sts = handle_rx_time_wait(tcb, &pkt);
			} else {
				if (trace & T_BADPKT) assp_log("%p: alien! packet in CLOSED\n", tcb);
				pkt.error = "CLOSED";
				tcb->stats.numrx_closed++;
			}
		}
		break;
	case PS_WAIT_COOKIE:
		if (pkt.getcookie) {
			pkt.error = "busy";
		} else sts = handle_rx_wait_cookie(tcb, &pkt);
		break;
	case PS_RST_WAIT:
		if (pkt.getcookie) {
			pkt.error = "busy";
		} else sts = handle_rst_wait(tcb, &pkt);
		break;
	case PS_ESTABLISHING:
	case PS_ESTABLISHED:
	case PS_FIN_WAIT:
	case PS_CLOSE_WAIT:
	case PS_LAST_ACK:
		if (pkt.getcookie) {
			pkt.error = "busy";
		} else sts = handle_synced(tcb, &pkt);
		break;
	}
 }
 if (pkt.pkt) {
 	if (pkt.error) alien(tcb, &pkt, pkt.error);
 	tcb->path->vtbl->rxfree(tcb->path->pi, pkt.pkt);
 }
 commit_tcb(tcb);
 if (sts) assp_abort(tcb, 0, "cannot send reply");
}

static void check_getcookie(void *arg)
{
 TCB *tcb = arg;
 PACKET *txp;
 uint8_t *sp;
 int sts = 0;
 check_tcb(tcb);
 if (tcb->pstate != PS_WAIT_COOKIE) {
	assp_log("%p: Internal error: check_getcookie but not in PS_WAIT_COOKIE\n", tcb);
	assp_error();
 }
 txp = txpacket0(tcb, 0);
 sp = txp->u.tx.seqopt;
 *--sp = ASSP_OPT_GETCOOKIE & 0xff;
 *--sp = ASSP_OPT_GETCOOKIE >> 8;
 *--sp = 1;
 *--sp = 0;			// length
 txp->u.tx.seqopt = sp;
 txp->u.tx.ptype = PKT_FASTACK;
 txp->seq = tcb->snd.nxtseq;
 sts = histsendpkt(tcb, txp);
 txpacket_free(tcb, txp);
 verify_q(tcb);
 tcb->tq->vtbl->start(tcb->tq->tqi, &tcb->curtmo, check_getcookie, tcb, time_when(time_longmS2rel(60)));
 commit_tcb(tcb);
 if (sts) assp_abort(tcb, 0, "cannot send GETCOOKIE");
}

int assp_connect(TCB *tcb, uint32_t cid)
{
 check_tcb(tcb);
 switch (tcb->pstate) {
 case PS_CLOSED:
	tcb->rcv.maxrxtim = ts_now(tcb);
	tcb->rcv.lastrefts = ts_now(tcb);
	break;
 case PS_TIME_WAIT:
 	if (cid == tcb->cid) {
		tcb->tq->vtbl->cancel(tcb->tq->tqi, &tcb->curtmo);
 		break;
	}
 		// else illegal
 case PS_WAIT_COOKIE:
 case PS_ESTABLISHING:
 case PS_ESTABLISHED:
 case PS_FIN_WAIT:
 case PS_CLOSE_WAIT:
 case PS_LAST_ACK:
 case PS_RST_WAIT:
	assp_log("%p: wrong state for connect", tcb);
	assp_error();
 }
 tcb->rcv.lasttxts = tcb->rcv.maxtxts;
 tcb->cid = cid;
 tcb->snd.extrawind = 1;
 tcb->rcv.window = 16;
 setpstate(tcb, PS_WAIT_COOKIE);
 verify_q(tcb);
 commit_tcb(tcb);
 check_getcookie(tcb);
 return 0;
}

int assp_abort(TCB *tcb, PACKET *txp, char *why)
{
 check_tcb(tcb);
 if (trace & T_API) assp_log("%p: assp_abort(%s)\n", tcb, why);
 tcb->stats.abort_why = why;
 switch (tcb->pstate) {
 case PS_RST_WAIT:
	if (!txp) {
			// stop RST-WAIT timer
		tcb->tq->vtbl->cancel(tcb->tq->tqi, &tcb->curtmo);
		time_wait(tcb);
	}
	break;
 case PS_TIME_WAIT:
 		// nothing to do - already finished with connection
 	break;
 case PS_CLOSED:
	assp_log("%p: wrong state for abort", tcb);
	assp_error();
 case PS_WAIT_COOKIE:
	tcb->tq->vtbl->cancel(tcb->tq->tqi, &tcb->curtmo);
	time_wait(tcb);
	break;
 case PS_ESTABLISHING:
 case PS_ESTABLISHED:
 case PS_FIN_WAIT:
 case PS_CLOSE_WAIT:
 case PS_LAST_ACK:
	if (!time_isnever(tcb->nexttimer) || tcb->since_valid) {
		tcb->tq->vtbl->cancel(tcb->tq->tqi, &tcb->curtmo);
		tcb->nexttimer = time_never();
		tcb->since_valid = 0;
	}
	if (txp) {
		txp->u.tx.seqopt = txp->datastart;
		rst_wait(tcb, txp);
		txp = 0;
	} else {
		time_wait(tcb);
	}
	break;
 }
 if (txp) txpacket_free(tcb, txp);
 commit_tcb(tcb);
 return 0;
}

/* Checking connection validity
 * Note that we require eight replies to make sure the connection is
 * still functioning. This is because when a card is reset the hint we 
 * get to check the connection can be sent several seconds before the
 * modules are actually reset.
 */
static void check_valid(void *arg)
{
 TCB *tcb = arg;
 int sts = 0;
 check_tcb(tcb);
 switch (tcb->pstate) {
 case PS_CLOSED:	// not needed
 case PS_TIME_WAIT:	// time wait timer is running
 case PS_RST_WAIT:	// rst wait timer is running
 case PS_WAIT_COOKIE:	// get cookie timer is running
 case PS_LAST_ACK:	// packet retransmission timer is running
 case PS_ESTABLISHING: // SYN retransmission timer is running
	tcb->since_valid = 0; // can't check for valid in these states
 	break;
 case PS_ESTABLISHED:
 case PS_FIN_WAIT:
 case PS_CLOSE_WAIT:
	if (tcb->since_valid && time_isnever(tcb->nexttimer)) {
		PACKET *ack = txpacket0(tcb, 0);
		ack->seq = tcb->snd.nxtseq;
		ack->u.tx.ptype = PKT_FASTACK;
		sts = histsendpkt(tcb, ack);
		txpacket_free(tcb, ack);
		tcb->tq->vtbl->start(tcb->tq->tqi, &tcb->curtmo, check_valid, tcb, time_when(time_longS2rel(1)));
	}
 }
 commit_tcb(tcb);
 if (sts) assp_abort(tcb, 0, "cannot send check-valid");
}

int assp_validate(TCB *tcb)
{
 int old;
 check_tcb(tcb);
 old = tcb->since_valid;
 tcb->since_valid = 8;
 commit_tcb(tcb);
 if (!old) check_valid(tcb);
 return 0;
}

int assp_txready(TCB *tcb)
{
 check_tcb(tcb);
 if (trace & T_API) assp_log("assp_txready()\n");
 if (tcb->sendready) {
	int sts = maybe_newtx(tcb);
	commit_tcb(tcb);
	if (sts) assp_abort(tcb, 0, "cannot send new data");
 }
 return 0;
}

static int oktxqorder(PACKET *l, PACKET *r)
{
 int32_t dseq = r->seq - l->seq;
 return dseq > 0;
}

static int oktmoqorder(PACKET *l, PACKET *r)
{
 int cmp = time_cmp(l->u.tx.txtim, r->u.tx.txtim);
 if (cmp < 0) return 1;
 if (cmp > 0) return 0;
 return oktxqorder(l, r);
}

void assp_tcb_init(TCB *tcb, ASSP_CONN *cnx, TIMEQUEUE *tq, TCBPATH *tcbpath, struct assp_tcb_vtbl *vtbl)
{
 memset(tcb, 0, sizeof(*tcb));
 tcb->vtbl = vtbl;
 tcb->cnx = cnx;
 tcb->tq = tq;
 tcb->path = tcbpath;
 tcb->snd.minseq = tcb->snd.maxseq = 0;
 txq_init(&tcb->snd.txq, oktxqorder);
 txq_init(&tcb->snd.sent_tmoq, oktmoqorder);
 tcb->snd.sent_ackl = 0;
 tcb->snd.extrawind = 1;
 tcb->rcv.rxq.frun = 0;
 tcb->nexttimer = time_never();
 tcb->txreadytime = time_never();
 verify_q(tcb);
 tcb->sendready = 0;
 tcb->snd.nxtseq = tcb->snd.una = randent32();
 tcb->snd.basets = randent32();
 tcb->rcv.maxtxts = randent32();
 tcb->rcv.nxt = randent32();
 tcb->rcv.maxrxtim = ts_now(tcb);
 tcb->rcv.lastrefts = ts_now(tcb);
#ifdef PARANOIA
 make_object(*tcb);
#endif
}

void assp_tcb_dtor(TCB *tcb)
{
 PACKET *txp;
 check_tcb(tcb);
 if (!time_isnever(tcb->txreadytime)) {
	tcb->tq->vtbl->cancel(tcb->tq->tqi, &tcb->readytmo);
 }
 while (tcb->pstate != PS_CLOSED) {
	long tmo = tcb->tq->vtbl->next_timer_mS(tcb->tq->tqi);
	if (tmo != -1) {
#ifdef TiNGTYPE_LINUX
		int i = poll(0, 0, tmo);
		if (i < 0 && errno != EINTR) {
			perror("poll() failed");
			assp_error();
		}
#endif
#ifdef TiNGTYPE_WINNT
		Sleep(tmo);
#endif
	} else {
#ifdef PROVIDE_TRACE
		assp_log("%p: assp_dtor: no timeout, but pstate=%s\n", tcb, pstate(tcb->pstate));
#else
		assp_log("%p: assp_dtor: no timeout, but pstate=%d\n", tcb, tcb->pstate);
#endif
		assp_error();
	}
 }
 for (txp=tcb->snd.minseq; txp; txp=txp->next_byseq) {
 	assp_log("byseq = seq:%u %p\n", txp->seq, txp);
 }
}

static void showstat(TCB *tcb, char *name, uint32_t val)
{
 if (val) assp_log("%p: \t%s: %d\n", tcb, name, val);
}

void assp_tcb_stats(TCB *tcb)
{
 check_tcb(tcb);
 showstat(tcb, "tx seq", tcb->stats.numtx_seq);
 showstat(tcb, "tx seq dup", tcb->stats.numtx_seqdup);
 showstat(tcb, "tx unseq", tcb->stats.numtx_unseq);
 showstat(tcb, "rx unseq", tcb->stats.numrx_unseq);
 showstat(tcb, "rx malformed unsequenced option", tcb->stats.numrx_badopt_unseq);
 showstat(tcb, "rx unknown critical unsequenced option", tcb->stats.numrx_unkopt_unseq);
 showstat(tcb, "rx seq", tcb->stats.numrx_seq);
 showstat(tcb, "fresh ACK in synced states", tcb->stats.numrx_sync_fresh);
 showstat(tcb, "stale ACK in synced states", tcb->stats.numrx_sync_stale);
 showstat(tcb, "alien ACK in synced states", tcb->stats.numrx_sync_alienack);
 showstat(tcb, "malformed seq option in synced states", tcb->stats.numrx_sync_badopt);
 showstat(tcb, "unknown critial option in synced states", tcb->stats.numrx_sync_unkopt);
 showstat(tcb, "unseq in synced states", tcb->stats.numrx_sync_unseq);
 showstat(tcb, "out of window in synced states", tcb->stats.numrx_sync_seq_oow);
 showstat(tcb, "duplicates in synced states", tcb->stats.numrx_sync_seq_dup);
 showstat(tcb, "out of order in synced states", tcb->stats.numrx_sync_seq_ooo);
 showstat(tcb, "dropped in synced states", tcb->stats.numrx_sync_seq_drop);
 showstat(tcb, "seq after FIN", tcb->stats.numrx_sync_afterfin);
 showstat(tcb, "next SEQ in synced states", tcb->stats.numrx_sync_seq_next);
 showstat(tcb, "RST in WAIT-COOKIE state", tcb->stats.numrx_wc_rst);
 showstat(tcb, "malformed in WAIT-COOKIE state", tcb->stats.numrx_wc_badopt);
 showstat(tcb, "unknown critical option in WAIT-COOKIE state", tcb->stats.numrx_wc_unkopt);
 showstat(tcb, "valid cookies in WAIT-COOKIE state", tcb->stats.numrx_wc_ok);
 showstat(tcb, "no valid cookie in WAIT-COOKIE state", tcb->stats.numrx_wc_noncoo);
 showstat(tcb, "packets in TIME-WAIT state", tcb->stats.numrx_tw);
 showstat(tcb, "packets in CLOSED state", tcb->stats.numrx_closed);
 showstat(tcb, "connections cleared by RST", tcb->stats.numrx_sync_rst);
 showstat(tcb, "cookies with invalid time", tcb->stats.numrx_coo_invtime);
 showstat(tcb, "cookies with bad TXTS", tcb->stats.numrx_coo_badtxts);
 showstat(tcb, "cookies with bad REFTS", tcb->stats.numrx_coo_badrefts);
 showstat(tcb, "bad size cookies", tcb->stats.numrx_coo_badsize);
 showstat(tcb, "cookies with bad hash", tcb->stats.numrx_coo_bogus);
 showstat(tcb, "stale cookies", tcb->stats.numrx_coo_stale);
}
