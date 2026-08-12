
#include <stdio.h>
#include "assp.h"
#include "asspdbg.h"
#include "pathsim.h"
#include "randent.h"
#include "ran2.h"
#include "simevent.h"
#include "simtimer.h"
#include "timequeue_pq_basic.h"

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER

/* disable stupid warnings about conversions
 */

#pragma warning(disable:4244)
#pragma warning(disable:4305)
/* and about argument conversions (does anyone still use a
   non-prototype compiler?) */
#pragma warning(disable:4761)

#endif

struct assp_conn {
	TCB *tcb;
	enum astate {
		AS_CLOSED, AS_CONNECTING, AS_RUNNING, AS_EOF, AS_STOPPED,
	} astate;
	struct command {
		enum {
			CMD_DONE,	// no further action - let simulator finish
			CMD_WAIT,	// pause for 'dur'
			CMD_CONNECT,	// connect (instant)
			CMD_VALIDATE,	// initiate validation (instant)
			CMD_TX,		// send at most 'par' packets for at most 'dur'
			CMD_TX_EPOCH,		// send at most 'par' packets for at most 'dur' epochs
			CMD_TX_EOF,	// send EOF (instant)
			CMD_ABORT_PKT,	// abort connection (instant)
			CMD_ABORT_NOPKT,	// abort connection (instant)
			CMD_WAITSTATE,	// wait until astate matches 'par'
		} cmd;
		unsigned par;
		uint_fast32_t dur;
	} *cmd;
	unsigned nsent;
	unsigned nepoch;
	ABSTIME starttx, endrx;
	unsigned nextrxdata;
	int flightsize;
	ABSTIME busy_start;
		// these should be unsigned, but Microsoft compiler
		// cannot handle unsigned 64-bit -> double conversion!
	int_fast64_t tot_txtime;
	int_fast64_t tot_txbits;
	int_fast64_t tot_rxbits;
	int_fast64_t tot_txpkts;
	RAN2 rantx;
	ABSTIME nexttx;
	unsigned nexttxdata;
	unsigned repeat_delay;
};

static int psize = 10;

static void newcmd(void *arg);
static void nextepoch(void *arg)
{
	ASSP_CONN *sep = arg;
	unsigned m = sep->nepoch % 20;
	if (sep->cmd->cmd != CMD_TX_EPOCH) return;
	printf("Next epoch: sent %d of %d (pending %d)\n", sep->nsent, sep->cmd->par * sep->nepoch, sep->cmd->par * sep->nepoch -  sep->nsent);
	sep->nepoch++;
	if (m == 0) {
		psize = 10;
	}
	if (m == 2) {
		psize = 1200;
	}
	if (sep->nepoch > sep->cmd->dur) {
 		sep->cmd++;
		eventqueue(time_when(time_longuS2rel(0)), newcmd, sep, "txepoch-nextcmd");
 		return;
	}
	eventqueue(time_when(time_longuS2rel(10000)), nextepoch, sep, "txepochcmd");
	assp_txready(sep->tcb);
}

static void newcmd(void *arg)
{
 ASSP_CONN *sep = arg;
 for (;;) {
	switch (sep->cmd->cmd) {
	case CMD_DONE: printf("cmd: DONE\n"); return;
	case CMD_WAIT:
		printf("cmd: WAIT\n");
		eventqueue(time_when(time_longuS2rel(sep->cmd->dur)), newcmd, sep, "waitcmd");
		sep->cmd++;
		return;
	case CMD_CONNECT:
		printf("cmd: CONNECT\n");
		printf("connect(%p)\n", sep->tcb);
		if (assp_connect(sep->tcb, 0)) printf("error from connect(%p)\n", sep->tcb);
		sep->astate = AS_CONNECTING;
		break;
	case CMD_VALIDATE:
		printf("cmd: VALIDATE\n");
		assp_validate(sep->tcb);
		break;
	case CMD_TX_EOF:
		printf("cmd: TXEOF\n");
	case CMD_TX:
		printf("cmd: TX\n");
		sep->nsent = 0;
		sep->nepoch = 1;
		if (sep->cmd->dur) {
			eventqueue(time_when(time_longuS2rel(sep->cmd->dur)), newcmd, sep, "txcmd");
		}
		assp_txready(sep->tcb);
		return;
	case CMD_TX_EPOCH:
		printf("cmd: TX epoch\n");
		sep->nsent = 0;
		sep->nepoch = 1;
		eventqueue(time_when(time_longuS2rel(10000)), nextepoch, sep, "txepochcmd");
		assp_txready(sep->tcb);
		return;
	case CMD_ABORT_PKT:
		printf("cmd: ABORT\n");
		assp_abort(sep->tcb, assp_txpacket(sep->tcb, 16), "abort cmd");
		break;
	case CMD_ABORT_NOPKT:
		printf("cmd: ABORT\n");
		assp_abort(sep->tcb, 0, "abort cmd");
		break;
	case CMD_WAITSTATE:
		printf("cmd: WAITSTATE(%d) is %d\n", sep->cmd->par, sep->astate);
		if (sep->astate != sep->cmd->par) {
			eventqueue(time_when(time_longuS2rel(sep->cmd->dur)), newcmd, sep, "txcmd");
			return;
		}
		break;
	default:
		printf("cmd=%u\n", sep->cmd->cmd);
		abort();
	}
	sep->cmd++;
 }
}

static void newstate(ASSP_CONN *sep, enum astate astate)
{
 sep->astate = astate;
 if (sep->cmd->cmd == CMD_WAITSTATE) newcmd(sep);
}

static int sim_sof(ASSP_CONN *sep)
{
 printf("SOF(%p)\n", sep);
 newstate(sep, AS_RUNNING);
 return 1;
}

static void sim_eof(ASSP_CONN *sep)
{
 printf("EOF(%p)\n", sep);
 newstate(sep, AS_EOF);
}

static void sim_stopped(ASSP_CONN *sep)
{
 printf("stopped(%p)\n", sep);
 newstate(sep, AS_STOPPED);
}

static void sim_closed(ASSP_CONN *sep)
{
 printf("CLOSED\n");
 newstate(sep, AS_CLOSED);
}

static void sim_deliver(ASSP_CONN *sep, PACKET *pkt)
{
 TCB *tcb = sep->tcb;
 uint_fast32_t pn;
 sep->endrx = time_when(time_longS2rel(0));
 sep->tot_rxbits += (pkt->dataend - pkt->datastart) * 8;
 if (pkt->dataend - pkt->datastart >= 4) {
	pn = pkt->datastart[0];
	pn += pkt->datastart[1] << 8;
	pn += pkt->datastart[2] << 16;
	pn += pkt->datastart[3] << 24;
//	printf("deliver(%p, %d)\n", tcb, pn);
	if (pn != sep->nextrxdata) {
		printf("Bogus data!! %d != %d\n", pn, sep->nextrxdata);
		abort();
	}
	sep->nextrxdata = pn + 1;
 } else {
	printf("deliver(%p) nodata\n", tcb);
 }
 tcb->path->vtbl->rxfree(tcb->path->pi, pkt);
}

static void sim_rx_peek(ASSP_CONN *sep, unsigned ofs, PACKET *pkt)
{
 (void) pkt;
// printf("peek(%p, %d)\n", sep, ofs);
}

static PACKET *sim_nexttx(ASSP_CONN *sep, int *flags)
{
 PACKET *txp;
 if (sep->cmd->cmd != CMD_TX && sep->cmd->cmd != CMD_TX_EPOCH && sep->cmd->cmd != CMD_TX_EOF) return 0;
 if (sep->nsent++ == sep->cmd->par * sep->nepoch) {
	if (sep->cmd->cmd == CMD_TX_EPOCH) {
		sep->nsent--;
		return 0;
	}
 	sep->cmd++;
	eventqueue(time_when(time_longuS2rel(0)), newcmd, sep, "tx-nextcmd");
 	return 0;
 }
 if (sep->cmd->par * sep->nepoch - sep->nsent > sep->cmd->par * 100) {
	printf("*** UNDERRUN ***\n");
	return 0;
 }
 txp = assp_txpacket(sep->tcb, 1536);
 txp->dataend[0] = sep->nexttxdata;
 txp->dataend[1] = sep->nexttxdata >> 8;
 txp->dataend[2] = sep->nexttxdata >> 16;
 txp->dataend[3] = sep->nexttxdata >> 24;
// txp->dataend += 1200;
 txp->dataend += psize;
 if (psize > 10) {
	psize = 2000 - psize;
 }
 sep->nexttxdata++;
 if (time_iszero(sep->starttx)) {
	sep->starttx = time_when(time_longS2rel(0));
 }
 if (!sep->flightsize) {
	sep->busy_start = time_when(time_longS2rel(0));
 }
 sep->flightsize++;
 sep->tot_txbits += (txp->dataend - txp->data) * 8;
 sep->tot_txpkts++;
 if (sep->cmd->cmd == CMD_TX_EOF) {
	 printf("*********** setting EOF ************\n");
	*flags = ASSP_F_EOF;
	sep->cmd++;
	eventqueue(time_when(time_longS2rel(0)), newcmd, sep, "newcmd");
 }
 return txp;
}

static void sim_txack(ASSP_CONN *sep, PACKET *txp, int acked)
{
 TCB *tcb = sep->tcb;
 (void) txp;
 (void) acked;
 printf("txack (%p) flightsize=%d\n", tcb, sep->flightsize);
 if (--sep->flightsize) return;
 sep->tot_txtime += -time_rel2longuS(time_until(sep->busy_start));
}

static void stats(ASSP_CONN *sep, char *id)
{
 if (sep->tot_txtime) {
	double txsecs = sep->tot_txtime;
	txsecs /= 1000000;
	printf("%s:\n", id);
	printf("\tTxtime: %g\n", txsecs);
	printf("\tPackets/sec: %g\n", sep->nexttxdata / txsecs);
	printf("\tTxbits: %" PRIdFAST64 "\n", sep->tot_txbits);
	printf("\tTxbits/sec: %g\n", sep->tot_txbits / txsecs);
	printf("\tRxbits: %" PRIdFAST64 "\n", sep->tot_rxbits);
	printf("\tRxbits/sec: %g\n", sep->tot_rxbits / txsecs);
 }
}

static const char version[] = "@(#)$from: " __FILE__ " "
#include "../gen/version.i"
	" $";
#include "gen/sim.args.i"

unsigned fixedcwnd;

int TiNGtrace;

int main(int argc, char **argv)
{
 static struct assp_tcb_vtbl assp_c_vtbl = {
	sim_sof,
	sim_rx_peek,
	sim_deliver,
	sim_eof,
	sim_stopped,
	sim_closed,
	sim_nexttx,
	sim_txack,
 };
 static struct assp_tcb_vtbl assp_s_vtbl = {
	sim_sof,
	sim_rx_peek,
	sim_deliver,
	sim_eof,
	sim_stopped,
	sim_closed,
	sim_nexttx,
	sim_txack,
 };
 static struct command s_cmd[] = {
 	{ CMD_WAITSTATE, AS_RUNNING, 1000000, },
// 	{ CMD_TX_EPOCH, 70, 100, },
// 	{ CMD_TX_EPOCH, 1, 100000, },
 	{ CMD_WAIT, 0, 500000000 },
 	{ CMD_WAIT, 0, 500000000 },
 	{ CMD_TX_EOF, 1, 0, },
 	{ CMD_WAITSTATE, AS_CLOSED, 1000000, },
 	{ CMD_DONE, 0, 0, },
 };
 static struct command c_cmd[] = {
 	{ CMD_CONNECT, 0, 0, },
 	{ CMD_WAITSTATE, AS_RUNNING, 1000000, },
// 	{ CMD_TX_EPOCH, 200, 1000, },
// 	{ CMD_WAIT, 0, 500000000 },
 	{ CMD_TX_EPOCH, 1, 100000, },
 	{ CMD_WAIT, 0, 500000000 },
 	{ CMD_WAIT, 0, 500000000 },
// 	{ CMD_TX_EPOCH, 69, 1000, },
// 	{ CMD_TX, 5000, 0, },
// 	{ CMD_TX, 500000, 0, },
 	//{ CMD_WAIT, 0, 5000000 },
 	//{ CMD_VALIDATE, 0, 0,},
 	//{ CMD_WAIT, 0, 5000 },
 	//{ CMD_TX, 1, 0, },
 	//{ CMD_WAIT, 0, 50000 },
 	//{ CMD_TX, 1, 0, },
 	//{ CMD_WAIT, 0, 5000000 },
 	//{ CMD_TX, 1, 0, },
 	//{ CMD_WAIT, 0, 5000000 },
// 	{ CMD_ABORT_NOPKT, 1, 0, },
 	{ CMD_TX_EOF, 1, 0, },
 	{ CMD_WAITSTATE, AS_CLOSED, 1000000, },
 	{ CMD_DONE, 0, 0, },
 };
 struct {
	HOP h1;
	HOP h2;
	HOP h3;
	TCBPATH path;
	TCBPATH_IMPL pathimpl;
 } c2s, s2c;
 struct {
	TCB tcb;
	ASSP_CONN conn;
 } s, c;
 TIMEQUEUE_IMPL tqi;
 TIMEQUEUE tq;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || argv[0]) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
	return 1;
 }
 assp_trace(arg.trace);
 TiNGtrace = arg.trace << 4;
 //printf("sizeof(PACKET) = %d = 0x%x\n", sizeof(PACKET), sizeof(PACKET));
 //printf("sizeof(TCB) = %d = 0x%x\n", sizeof(TCB), sizeof(TCB));
 //printf("sizeof(TCB.stats) = %d = 0x%x\n", sizeof(c.tcb.stats), sizeof(c.tcb.stats));
 memset(&s.conn, 0, sizeof(s.conn));
 memset(&c.conn, 0, sizeof(c.conn));
 ran2_init(&c.conn.rantx, 1);
 ran2_init(&s.conn.rantx, 1);
 fixedcwnd = arg.fixedcwnd;
 randent_init();
 c.conn.repeat_delay = arg.repeat_delay;
 //hop_init(&c2s.h3, "c2sh3", 0, &s.tcb, 1e6, 0.00, 0.00, 0.0000, 0.000, 0.050);
 //hop_init(&c2s.h2, "c2sh2", &c2s.h3, 0, 1e6, 0.00, 0.00, 0.0000, 0.000, 0.050);
 //hop_init(&c2s.h1, "c2sh1", 0, &s.tcb, 1e6, 0, 0, 0.0000, 0.000, 0.050);
// hop_init(&c2s.h1, "c2sh1", 0, &s.tcb, 1e9, 0, 0, 0.000000, 0.000, 0.000050);
// hop_init(&c2s.h1, "c2sh1", 0, &s.tcb, 1e9, 0, 0, 0.000000, 0.000, 0.000050);
// hop_init(&c2s.h1, "c2sh1", 0, &s.tcb, 1e9, 0, 0, 0.000040, 0.000010, 0.000050);
// hop_init(&c2s.h1, "c2sh1", 0, &s.tcb, 1e9, 0.0001, 0, 0.000040, 0.000010, 0.000050);
 hop_init(&c2s.h1, "c2sh1", 0, &s.tcb, 1e9, 0, 0, 0.00000, 0.00000, 0.0350);
// hop_init(&c2s.h1, "c2sh1", 0, &s.tcb, 1e8, 0.0001, 0, 0.000040, 0.000010, 0.000050);
 tcbpath_sim_init(&c2s.path, &c2s.pathimpl, &c2s.h1);
 //hop_init(&s2c.h3, "s2ch3", 0, &c.tcb, 1e6, 0.00, 0.00, 0.0000, 0.000, 0.050);
 //hop_init(&s2c.h2, "s2ch2", &s2c.h3, 0, 1e6, 0.00, 0.00, 0.0000, 0.000, 0.050);
 //hop_init(&s2c.h1, "s2ch1", 0, &c.tcb, 1e6, 0, 0, 0.0100, 0.010, 0.050);
// hop_init(&s2c.h1, "s2ch1", 0, &c.tcb, 1e9, 0, 0, 0.0000, 0.000, 0.000050);
// hop_init(&s2c.h1, "s2ch1", 0, &c.tcb, 1e9, 0, 0, 0.000040, 0.000010, 0.000050);
// hop_init(&s2c.h1, "s2ch1", 0, &c.tcb, 1e9, 0, 0, 0.0000, 0.0000, 0.005);
// hop_init(&s2c.h1, "s2ch1", 0, &c.tcb, 5e7, 0, 0, 0.000300, 0.001500, 0.000050);
// hop_init(&s2c.h1, "s2ch1", 0, &c.tcb, 5e7, 0, 0, 0.000300, 0.000100, 0.000050);
// hop_init(&s2c.h1, "s2ch1", 0, &c.tcb, 1e8, 0, 0, 0.003800, 0.001000, 0.000050);
 hop_init(&s2c.h1, "s2ch1", 0, &c.tcb, 1e9, 0, 0, 0.00000, 0.00000, 0.0350);
// hop_init(&s2c.h1, "s2ch1", 0, &c.tcb, 1e8, 0, 0, 0.000050, 0.000010, 0.000050);
 tcbpath_sim_init(&s2c.path, &s2c.pathimpl, &s2c.h1);
 timequeue_sim_init(&tq, &tqi);
 assp_tcb_init(&c.tcb, &c.conn, &tq, &c2s.path, &assp_c_vtbl);
 assp_tcb_init(&s.tcb, &s.conn, &tq, &s2c.path, &assp_s_vtbl);
// c.tcb.snd.nxtseq = c.tcb.snd.una = 0;
 s.conn.tcb = &s.tcb;
 c.conn.tcb = &c.tcb;
 s.conn.cmd = s_cmd;
 newcmd(&s.conn);
 c.conn.cmd = c_cmd;
 newcmd(&c.conn);
 for (;;) {
 	char *desc = runevent();
	if (!desc) break;
	//poll(0,0,500);
	printf("\n");
 }
 if (c.tcb.pstate != PS_CLOSED) {
	printf("TCB c: %s\n", pstate(c.tcb.pstate));
 }
 if (s.tcb.pstate != PS_CLOSED) {
	printf("TCB s: %s\n", pstate(s.tcb.pstate));
 }
 assp_tcb_stats(&c.tcb);
 assp_tcb_stats(&s.tcb);
 assp_tcb_dtor(&c.tcb);
 assp_tcb_dtor(&s.tcb);
 closepath(&c2s.pathimpl, "c2s");
 closepath(&s2c.pathimpl, "s2c");
 stats(&s.conn, "s->c");
 stats(&c.conn, "c->s");
 return 0;
}
