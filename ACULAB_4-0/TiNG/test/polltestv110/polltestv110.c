#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/timeb.h>
#include "smdrvr.h"
//#include <conio.h>
#include "smdc.h"
#include "smdc_v110.h"

#include <malloc.h>

#include "../Testlib/cardopen.h"
#include "../Testlib/errcode_sm.h"
#include "../Testlib/modopen.h"
#include "../../libutil/generic_io.h"

#ifdef TiNGTYPE_LINUX
#include <poll.h>
#include <pthread.h>
#endif

#ifdef TiNGTYPE_QNX
#include <sys/poll.h>
#include <pthread.h>
#endif

#include "../apilib/smdc_rlp.h"

#define arlen(s) (sizeof(s)/sizeof(*(s)))

typedef struct {
	int stream;
	int timeslot;
	int type;
} MVIP;
#define MVIP_STREAM_NONE 0xff

	// handy utilities
static char *endstr(char *s)
{
 while (*s) s++;
 return s;
}

	// convenient encapsulations of Prosody API functions
static int chan_config_v110(tSMChannelId chan, long speed, unsigned syncmode)
{
 SMDC_CHANNEL_CONFIG_PARMS sc;
 SMDC_V110_CONFIG_PARMS v110;
 int r;
 memset(&sc, 0, sizeof(sc));
 memset(&v110, 0, sizeof(v110));
 sc.channel = chan;
 sc.protocol = kSMDCProtocolV110;
 sc.config_length = sizeof(v110);
 sc.config_data = &v110;
 v110.speed = speed;
 v110.format = syncmode ? kSMDCFormatSync : kSMDCFormatAsync;
 r = smdc_channel_config(&sc);
 if (r) return printerr_sm("smdc_channel_config", r);
 return 0;
}

static int chan_control(tSMChannelId chan, int cmd, int mask, int toggle)
{
 SMDC_LINE_CONTROL_PARMS cp;
 int r;
 memset(&cp, 0, sizeof(cp));
 cp.channel = chan;
 cp.cmd = cmd;
 cp.aux_mask = mask;
 cp.aux_toggle = toggle;
 r = smdc_line_control(&cp);
 if (r) return printerr_sm("smdc_line_control", r);
 return 0;
}

static int chan_rx_control(tSMChannelId chan, int cmd, int mincol, int minidl)
{
 int r;
 SMDC_RX_CONTROL_PARMS dp;
 memset(&dp, 0, sizeof(dp));
 dp.channel = chan;
 dp.cmd = cmd;
 dp.min_to_collect = mincol;
 dp.min_idle = minidl;
 r = smdc_rx_control(&dp);
 if (r) return printerr_sm("smdc_rx_control", r);
 return 0;
}

static int chan_tx_control(tSMChannelId chan, int cmd, int cap, int blocking)
{
 int r;
 SMDC_TX_CONTROL_PARMS dp;
 memset(&dp, 0, sizeof(dp));
 dp.channel = chan;
 dp.cmd = cmd;
 dp.capacity = cap;
 dp.blocking = blocking;
 r = smdc_tx_control(&dp);
 if (r) return printerr_sm("smdc_tx_control", r);
 return 0;
}

	// test code
struct rxparam {
	enum cstate {CHAN_DEAD, CHAN_STOPPING, CHAN_RUNNING} cstate;
	int psiz;		// bytes in pattern (as transmitted)
	int incr;		// readonly
	int rxbusy;
	int err;
	FILE *rxfile;
	tSMChannelId chan;
	unsigned long maxrx;
	int check;
	char endmsg[256];	// only valid after channel closure
	struct timeb rxstart, rxlast;
	unsigned long rxcnt, toterrs;
	int rxpos;
	unsigned long rxdat, rxpat;
		// last rx status
	int rsts_available_octets;
	unsigned rsts_status;
	int rsts_flow;
		// last line status
	unsigned lsts_link_status;
	unsigned lsts_rx_status;
	unsigned lsts_tx_status;
	int lsts_tx_cts;
	struct txparam *txp;
};

struct txparam {
	char txbusy;
	enum tstate {TX_UNDERRUN, TX_ZERO, TX_FF, TX_PAT} tstate;
	int psiz;		// bytes in pattern (as transmitted)
	int incr;		// readonly
	char endmsg[128];	// only valid after thread temination
	struct timeb txstart;
	int txlen;
	unsigned long txpat;
		// last tx status
	int tsts_capacity;
	unsigned tsts_status;
	int tsts_flow;
	struct rxparam *rxp;
};

static int line_status(struct rxparam *rxp)
{
 SMDC_LINE_STATUS_PARMS lp;
 int r;
 memset(&lp, 0, sizeof(lp));
 lp.channel = rxp->chan;
 r = smdc_line_status(&lp);
 if (r) return printerr_sm("smdc_line_status", r);
 if (rxp->lsts_link_status != lp.link_status) {
	printf("Ls: link_status -> %d\n", lp.link_status);
	rxp->lsts_link_status = lp.link_status;
 }
 if (rxp->lsts_rx_status != lp.rx_status) {
	printf("Ls: rx_status -> %d\n", lp.rx_status);
	rxp->lsts_rx_status = lp.rx_status;
 }
 if (rxp->lsts_tx_status != lp.tx_status) {
	printf("Ls: tx_status -> %d\n", lp.tx_status);
	rxp->lsts_tx_status = lp.tx_status;
 }
 if (rxp->lsts_tx_cts != lp.tx_cts) {
	printf("Ls: tx_cts -> %d\n", lp.tx_cts);
	rxp->lsts_tx_cts = lp.tx_cts;
 }
 return r;
}

static int tx_status(struct txparam *txp)
{
 int r;
 SMDC_TX_STATUS_PARMS txs;
 memset(&txs, 0, sizeof(txs));
 txs.channel = txp->rxp->chan;
 r = smdc_tx_status(&txs);
 if (r) return printerr_sm("smdc_tx_status", r);
 if (txp->tsts_status != txs.status) {
	int rq = 0;
	if (txs.status != kSMDCTxStatusHasCapacity
		&& txs.status != kSMDCTxStatusNoCapacity) rq = 1;
	if (txp->tsts_status != kSMDCTxStatusHasCapacity
		&& txp->tsts_status != kSMDCTxStatusNoCapacity) rq = 1;
	txp->tsts_status = txs.status;
	if (rq) printf("Txs: sts->%d\n", txp->tsts_status);
 }
 if (txp->tsts_flow != txs.flow) {
	txp->tsts_flow = txs.flow;
	printf("Txs: flow->%d\n", txp->tsts_flow);
 }
 txp->tsts_capacity = txs.capacity;
 return r;
}

static int rx_status(struct rxparam *rxp)
{
 SMDC_RX_STATUS_PARMS dp;
 int r;
 memset(&dp, 0, sizeof(dp));
 dp.channel = rxp->chan;
 r = smdc_rx_status(&dp);
 if (r) return printerr_sm("smdc_rx_status", r);
 rxp->rsts_available_octets = dp.available_octets;
 if (rxp->rsts_status != dp.status) {
	printf("Rxs: status->%d\n", dp.status);
	rxp->rsts_status = dp.status;
 }
 if (rxp->rsts_flow != dp.flow) {
	printf("Rxs: flow->%d\n", dp.flow);
	rxp->rsts_flow = dp.flow;
 }
 return 0;
}

static int tx_data(struct txparam *txp, void *buf, int len)
{
 for (; len;) {
	SMDC_DATA_PARMS dp;
	int r;
	memset(&dp, 0, sizeof(dp));
	dp.channel = txp->rxp->chan;
	dp.data = buf;
	dp.max_length = len;
	r = smdc_tx_data(&dp);
	if (r) return printerr_sm("smdc_tx_data", r);
	if (dp.done_length) {
		len -= dp.done_length;
		buf = (char *) buf + dp.done_length;
	} else {
		r = line_status(txp->rxp);
		if (r) return r;
		r = tx_status(txp);
		if (r) return r;
		r = rx_status(txp->rxp);
		if (r) return r;
	}
 }
 return 0;
}

static int chan_rx_data(struct rxparam *rxp, int *rxlen, void *buf, int len)
{
 SMDC_DATA_PARMS dp;
 int r;
 memset(&dp, 0, sizeof(dp));
 dp.channel = rxp->chan;
 dp.data = buf;
 dp.max_length = len;
 r = smdc_rx_data(&dp);
 if (r) return printerr_sm("smdc_rx_data", r);
 if ((*rxlen = dp.done_length)) return 0;
 r = rx_status(rxp);
 if (r) return r;
 r = line_status(rxp);
 if (r) return r;
 r = tx_status(rxp->txp);
 if (r) return r;
 return 0;
}

static const char *inittx(struct txparam *txp)
{
 txp->tsts_capacity=0xaaaa;
 txp->tsts_status=0xaaaa;
 txp->tsts_flow=0xaaaa;
 txp->txlen=0;
 txp->txpat = 0;
 if (chan_control(txp->rxp->chan, kSMDCLineCtlCmdInitiatorConnect, 0, 0)) {
	return "control failed";
 }
 if (chan_tx_control(txp->rxp->chan, kSMDCTxCtlNotifyOnCapacity, 2048, 0)) {
	return "tx control failed";
 }
 return 0;
}

static int updatetx(struct txparam *txp)
{
 char buff[31];
 unsigned i = 0;
 if (txp->rxp->cstate != CHAN_RUNNING) return 1;
 switch (txp->tstate) {
 case TX_UNDERRUN: txp->txbusy = 'U'; break;
 case TX_PAT:
	for (i=0; i <= arlen(buff)-txp->psiz; ) {
		unsigned long pat = txp->txpat;
		int j;
		for (j=0; j < txp->psiz; j++) {
			buff[i++] = pat & 0x7f;
			pat >>= 7;
		}
		buff[i-1] |= 0x80;
		txp->txpat += txp->incr;
	}
	txp->txbusy = 'T';
	break;
 case TX_ZERO:
	for (i=0; i < arlen(buff); i++) buff[i] = 0;
	txp->txbusy = '0';
	break;
 case TX_FF:
	for (i=0; i < arlen(buff); i++) buff[i] = 0xff;
	txp->txbusy = 'F';
	break;
 }
#if 0
	// kludge for testing S/X/E bit override
 if ((txp->txlen ^ (txp->txlen+i)) & 0x1000) {
 	const bits = kSMDCV110Aux_S1
		| kSMDCV110Aux_S3
		| kSMDCV110Aux_S4
		| kSMDCV110Aux_E4
		| kSMDCV110Aux_E5
		| kSMDCV110Aux_E6
		| kSMDCV110Aux_E7;
	chan_control(txp->chan, kSMDCLineCtlCmdSetAux, ~bits,
		txp->txlen & 0x1000 ? bits : 0);
 }
#endif
 if (txp->tstate != TX_UNDERRUN) {
	if (!txp->txlen) ftime(&txp->txstart);
	txp->txlen += i;
	if (tx_data(txp, buff, i)) return 1;
 } else tx_status(txp);
 // txp->tstate = TX_UNDERRUN;
 return 0;
}

static char *finishtx(struct txparam *txp)
{
 struct timeb now;
 ftime(&now);
 *txp->endmsg = 0;
 if (txp->txlen) {
	double txtim;
	txtim = now.millitm - txp->txstart.millitm;
	txtim /= 1000.0;
	txtim += now.time - txp->txstart.time;
	if (!txtim) txtim = 0.001;
	sprintf(txp->endmsg, "sent %d bytes in %g S = %g bytes/sec (%g bps)",
		txp->txlen, txtim, txp->txlen / txtim, 8.0 * txp->txlen / txtim);
 }
 return 0;
}

static const char *initrx(struct rxparam *rxp)
{
 rxp->rsts_available_octets=0xaaaa;
 rxp->rsts_status=0xaaaa;
 rxp->rsts_flow=0xaaaa;
 rxp->lsts_link_status=0xaaaa;
 rxp->lsts_rx_status=0xaaaa;
 rxp->lsts_tx_status=0xaaaa;
 rxp->lsts_tx_cts=0xaaaa;
 rxp->rxpos = 0;
 rxp->rxdat = 0;
 rxp->rxcnt=0;
 rxp->rxpat = 0;
 if (chan_rx_control(rxp->chan, kSMDCRxCtlNotifyOnData, 1024, 100)) {
	return "control failed";
 }
 *rxp->endmsg = 0;
 return 0;
}

static int updaterx(struct rxparam *rxp)
{
 unsigned char buff[1024];
 int haderr=0, lasterr=0;
 int p0;
 //int p1, p2;
 if (rxp->cstate != CHAN_RUNNING) return 1;
 //if (chan_rx_status(&p0, &p1, &p2, rxp->chan)) return 1;
 //printf("Rxs: %d %d %d\n", p0, p1, p2);
 if (chan_rx_data(rxp, &p0, buff, sizeof(buff))) return 1;
 //printf("got %d\n", p0);
 if (p0) rxp->rxbusy = 'R';
 ftime(&rxp->rxlast);
 if (!rxp->rxcnt) rxp->rxstart = rxp->rxlast;
 if (0 && p0 != sizeof(buff)) {
	fprintf(stderr, "Partial read: %d of %ld\n",
		p0, (long) sizeof(buff));
	return 1;
 }
 if (rxp->check) {
	int i;
	for (i=0; i < p0; i++) {
		if (buff[i] & 0x80) {
			rxp->rxdat |= (buff[i] & 0x7f) << rxp->rxpos;
			if (rxp->rxdat != rxp->rxpat) {
				if (rxp->check > 1) {
					if (!haderr) printf("Error: ");
					if (lasterr+1 != i)
						printf(" @%ld:", rxp->rxcnt+i);
					printf(" %02lx!=%02lx",
						rxp->rxdat, rxp->rxpat);
				}
				lasterr = i;
				rxp->toterrs++;
				haderr=1;
			}
			rxp->rxpat = rxp->rxdat + rxp->incr;
			rxp->rxpat &= (1 << 7*rxp->psiz)-1;
			rxp->rxpos = 0;
			rxp->rxdat = 0;
		} else {
			rxp->rxdat |= buff[i] << rxp->rxpos;
			rxp->rxpos += 7;
		}
	}
	if (haderr) {
		if (rxp->check > 1) printf("(+%d)\n", p0-lasterr);
		rxp->err = 1;
	}
 }
 if (rxp->rxfile) {
	if (fwrite(buff, 1, p0, rxp->rxfile) != (unsigned) p0) {
		perror("fwrite failed");
		return 1;
	}
	fflush(rxp->rxfile);
 }
 rxp->rxcnt += p0;
 if ((unsigned) p0 > rxp->maxrx) rxp->maxrx = 0;
 else rxp->maxrx -= p0;
 if (!rxp->maxrx) {
	sprintf(rxp->endmsg, "[limit] ");
	return 1;
 }
 return 0;
}

static void finishrx(struct rxparam *rxp)
{
 if (rxp->rxfile) fclose(rxp->rxfile);
 if (rxp->rxcnt) {
	double rxtim;
	rxtim = rxp->rxlast.millitm - rxp->rxstart.millitm;
	rxtim /= 1000.0;
	rxtim += rxp->rxlast.time - rxp->rxstart.time;
	sprintf(endstr(rxp->endmsg), "Got %ld bytes in %g S",
		rxp->rxcnt,
		rxtim);
	if (rxtim) sprintf(endstr(rxp->endmsg), " = %g bytes/sec (%g bps)",
		rxp->rxcnt / rxtim, 8.0 * rxp->rxcnt / rxtim);
 } else sprintf(endstr(rxp->endmsg), "Got 0 bytes");
 if (rxp->toterrs) sprintf(endstr(rxp->endmsg), "\nError rate: 1/%g", rxp->rxcnt / (double) rxp->toterrs);
}

	// initialisation

static int startchan(struct txparam *txp, struct rxparam *rxp, SM_CHANNEL_ALLOC_PLACED_PARMS *sa, MVIP *ints, MVIP *outts, long speed, unsigned syncmode, int psiz, int check, unsigned long maxrx, char *pfx, struct pollfd *fdp)
{
 SM_SWITCH_CHANNEL_PARMS swp;
 tSMEventId ev;
 int r;
 const char *s;
 r = sm_channel_alloc_placed(sa);
 if (r) return printerr_sm("sm_channel_alloc_placed", r);
 memset(&swp, 0, sizeof(swp));
 swp.channel = rxp->chan = sa->channel;
 if (pfx) {
	char *rxfname = malloc(strlen(pfx)+sizeof(".000"));
	if (!rxfname) {
		perror("malloc failed");
		return 1;
	}
	sprintf(rxfname, "%s-%d-%d", pfx, ints->stream, ints->timeslot);
	rxp->rxfile = fopen(rxfname, "wb");
	if (!rxp->rxfile) {
		perror("Cannot create file");
		fprintf(stderr, "File: %s\n", rxfname);
		free(rxfname);
		return 1;
	}
	free(rxfname);
 } else rxp->rxfile = 0;
 if (chan_config_v110(rxp->chan, speed, syncmode)) return 1;
 swp.st = ints->stream;
 swp.ts = ints->timeslot;
 swp.type = ints->type;
 if (++ints->timeslot >= 32) {
	ints->timeslot = 0;
	ints->stream++;
 }
 r = sm_switch_channel_input(&swp);
 if (r) return printerr_sm("sm_switch_channel_input", r);
 swp.st = outts->stream;
 swp.ts = outts->timeslot;
 swp.type = outts->type;
 if (++outts->timeslot >= 32) {
	outts->timeslot = 0;
	outts->stream++;
 }
 r = sm_switch_channel_output(&swp);
 if (r) return printerr_sm("sm_switch_channel_output", r);
 rxp->maxrx = maxrx;
 if (psiz) txp->tstate = TX_PAT;
 else txp->tstate = TX_UNDERRUN, psiz = 2;
 rxp->psiz = psiz;
 rxp->check = check;
 txp->psiz = psiz;
 txp->txbusy = rxp->rxbusy = rxp->err = 0;
 txp->incr = rxp->incr = outts->timeslot+1;
 rxp->cstate = CHAN_RUNNING;
 rxp->txp = txp;
 txp->rxp = rxp;
 if ( (s = initrx(rxp)) ) {
	fprintf(stderr, "cannot initialise rx: %s\n", s);
	return 1;
 }
 if ( (s = inittx(txp)) ) {
	fprintf(stderr, "cannot initialise tx: %s\n", s);
	return 1;
 }
 smd_ev_create(&ev, rxp->chan, kSMEventTypeReadData, kSMChannelSpecificEvent);
 fdp->fd = ev.fd;
 fdp->events = ev.mode;
 fdp++;
 smd_ev_create(&ev, rxp->chan, kSMEventTypeWriteData, kSMChannelSpecificEvent);
 fdp->fd = ev.fd;
 fdp->events = ev.mode;
 return 0;
}

	// periodic status display

static void showsts(struct txparam *txp, struct rxparam *rxp, int numchan)
{
 static char twiddle[] = "-\\|/";
 static int twidpos;
 char *disp = malloc(numchan*3+sizeof(" \r"));
 char *cp = disp;
 int chan;
 if (!disp) return;
 *cp++ = '\r';
 for (chan=0; chan<numchan; chan++) {
	switch (rxp[chan].cstate) {
	case CHAN_DEAD: *cp++ = '_'; *cp++ = '_'; break;
	case CHAN_STOPPING: *cp++ = 'x'; *cp++ = 'x'; break;
	case CHAN_RUNNING:
		if (txp[chan].txbusy) *cp++ = txp[chan].txbusy;
		else {
			if (txp->tsts_flow) *cp++ = 'f';
			else *cp++ = '.';
		}
		if (txp[chan].txbusy != 'U') txp[chan].txbusy = 0;
		if (rxp[chan].err) *cp++ = 'r';
		else if (rxp[chan].rxbusy) *cp++ = rxp[chan].rxbusy;
		else *cp++ = '.';
	}
	*cp++ = ' ';
	rxp[chan].rxbusy = 0;
	rxp[chan].err = 0;
 }
 *cp++ = twiddle[twidpos];
 if (++twidpos == arlen(twiddle)-1) twidpos = 0;
 *cp = 0;
 fputs(disp, stdout);
 free(disp);
}

	// starting everything up and shutting it down

static int testv110(tSMModuleId modnum, long speed, unsigned syncmode, int psiz, int check, unsigned long maxrx, MVIP *ints, MVIP *outts, char *pfx, unsigned numchan)
{
 unsigned nfds = 2*numchan;	// N*(tx+rx)+kbd
 unsigned minfd = 0;
 struct pollfd *fds = (struct pollfd *) malloc((nfds+1) * sizeof(*fds));
 SM_CHANNEL_ALLOC_PLACED_PARMS sa;
 struct timeb laststs;
 struct txparam *txp;
 struct rxparam *rxp;
 unsigned chan;
 	// general initialisation
 // txp[X] is fd 2*X
 // rxp[X] is fd 2*X+1
 if (!fds) {
	fprintf(stderr, "Cannot allocate memory for file descriptors\n");
	return 1;
 }
 txp = (struct txparam *) malloc(numchan * sizeof(*txp));
 if (!txp) {
	fprintf(stderr, "Cannot allocate memory for tx channel params\n");
	return 1;
 }
 rxp = (struct rxparam *) malloc(numchan * sizeof(*rxp));
 if (!rxp) {
	fprintf(stderr, "Cannot allocate memory for rx channel params\n");
	return 1;
 }
 for (chan=0; chan<numchan; chan++) {
	rxp[chan].cstate = CHAN_DEAD;
 }
 	// initialisation needed by data transfer threads
 memset(&sa, 0, sizeof(sa));
 sa.type = kSMChannelTypeFullDuplex;
 sa.module = modnum;
 for (chan=0; chan < numchan; chan++) {
	int sts;
	sts = startchan(
		txp+chan,
		rxp+chan,
		&sa,
		ints,
		outts,
		speed,
		syncmode,
		psiz,
		check,
		maxrx,
		pfx,
		fds+chan*2);
	if (sts) return 1;
 }
 fds[nfds].events = POLLIN | POLLHUP;
 fds[nfds].fd = 0;
 ftime(&laststs);
 for (;;) {
	struct timeb now;
	int i;
	ftime(&now);
	i = 1 + laststs.time - now.time;	// seconds until next status due
	i *= 1000;				// convert to mS
	i += laststs.millitm - now.millitm;	// include mS
	if (i <= 0) {
		showsts(txp, rxp, numchan);
		i = 1000;
		laststs = now;
	}
	while (fds[minfd].fd == -1) minfd++;	// trim initial closed chans
	if (minfd >= nfds) break;	// only kbd left
	fflush(stdout);
	i = poll(fds, nfds+1, i);
	if (i < 0) {
		perror("poll() failed");
		break;
	}
	if (fds[nfds].revents & POLLIN) {	// kbd
		char c;
		fds[nfds].revents &= ~POLLIN;
		i = read(fds[nfds].fd, &c, 1);
		if (i < 0) {
			perror("read() failed");
			break;
		}
		if (!i) break;
		switch (c) {
		case 'f':
			for (chan=0; chan < numchan; chan++) {
				txp[chan].tstate = TX_FF;
				fds[chan*2+1].events = POLLOUT;
			}
			break;
		case '0':
			for (chan=0; chan < numchan; chan++) {
				txp[chan].tstate = TX_ZERO;
				fds[chan*2+1].events = POLLOUT;
			}
			break;
		case 'U':
			for (chan=0; chan < numchan; chan++) {
				txp[chan].tstate = TX_PAT;
				fds[chan*2+1].events = POLLOUT;
			}
			break;
		case 'u':
			for (chan=0; chan < numchan; chan++) {
				txp[chan].tstate = TX_UNDERRUN;
			}
			break;
		case '-':
			for (chan=0; chan < numchan; chan++) {
				if (rxp[chan].cstate == CHAN_RUNNING) {
					rxp[chan].cstate = CHAN_STOPPING;
					printf("Stopping %d \n", chan);
					break;
				}
			}
			break;
		case 'q':
			for (chan=0; chan < numchan; chan++) {
				if (rxp[chan].cstate != CHAN_DEAD) {
					rxp[chan].cstate = CHAN_STOPPING;
				}
			}
			break;
		}
	} else {
		struct pollfd *pfd;
		unsigned j;
		for (pfd=fds, j=0; j*2 < nfds; pfd+=2, j++) {
			int end = 0;
			if (pfd->revents & (POLLIN|POLLPRI|POLLHUP|POLLNVAL)) {
				end = updaterx(rxp+j);
				pfd->revents &= ~(POLLIN|POLLHUP|POLLNVAL);
			} else {
				end = rxp[j].cstate == CHAN_STOPPING;
			}
			if (pfd[1].revents & (POLLOUT|POLLPRI|POLLHUP|POLLNVAL)) {	// tx
				if (pfd[1].revents & POLLOUT) {
					if (updatetx(txp+j)) end = 1;
				} else tx_status(txp+j);
				if (txp[j].tsts_status == kSMDCTxStatusUnderrun) {
					txp[j].txbusy = 'u';
				}
				pfd[1].revents &= ~(POLLOUT|POLLHUP|POLLNVAL);
			} else {
				if (txp[j].tstate == TX_UNDERRUN) {
					pfd[1].events = 0;
				}
			}
			if (end) {
				int r;
				pfd->fd = -1;
				pfd[1].fd = -1;
				finishrx(rxp+j);
				printf("%dr: %s\n", j, rxp[j].endmsg);
				finishtx(txp+j);
				printf("%dt: %s\n", j, txp[j].endmsg);
				rxp[j].cstate = CHAN_DEAD;
				if (rxp[j].chan) {
					r = sm_channel_release(rxp[j].chan);
					if (r) return printerr_sm("sm_channel_release", r);
					rxp[j].chan = 0;
				}
			}
		}
	}
 }
 free(rxp);
 free(txp);
 free(fds);
 return 0;
}

#include "gen/polltestv110.args.i"

int main(int argc, char **argv)
{
 tSMModuleId mod;
 tSMCardId card;
 int sts;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || *argv) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
	return 1;
 }
 if (!arg.numchan) arg.numchan = 1;
 if (!arg.speed) arg.speed = 38400;
 if (arg.maxrx) arg.maxrx *= arg.speed / 10;
 else arg.maxrx = ~0;
 if (arg.serialnumber) {
	err_t e = modopen(&card, &mod, arg.serialnumber, arg.module);
	if (e) {
		error_log(stderr, e);
		return 1;
	}
 } else {
	fprintf(stderr, "%s: no Prosody card specified\n", progname);
	return 1;
 }
 sts = testv110(mod, arg.speed, arg.sync, arg.patsize, arg.check, arg.maxrx, &arg.intimeslot, &arg.outtimeslot, arg.fileprefix, arg.numchan);
 if (arg.serialnumber) {
	modclose(mod);
	cardclose(card);
 }
 return sts;
}
