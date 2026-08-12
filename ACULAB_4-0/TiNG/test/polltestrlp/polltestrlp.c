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
#include "smdc_rlp.h"

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

#define TXBLOCKS 64
#define RXBLOCKS 64
typedef unsigned char BLOCK[32];

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
static int chan_config_v110rlptx(tSMChannelId chan, long speed)
{
 SMDC_CHANNEL_CONFIG_PARMS sc;
 SMDC_V110RLP_CONFIG_PARMS v110rlp;
 int r;
 memset(&sc, 0, sizeof(sc));
 memset(&v110rlp, 0, sizeof(v110rlp));
 sc.channel = chan;
 sc.protocol = kSMDCProtocolV110RLPtx;
 sc.config_length = sizeof(v110rlp);
 sc.config_data = &v110rlp;
 v110rlp.speed = speed;
 r = smdc_channel_config(&sc);
 if (r) return printerr_sm("smdc_channel_config", r);
 return 0;
}

static int chan_config_v110rlprx(tSMChannelId chan, long speed)
{
 SMDC_CHANNEL_CONFIG_PARMS sc;
 SMDC_V110RLP_CONFIG_PARMS v110rlp;
 int r;
 memset(&sc, 0, sizeof(sc));
 memset(&v110rlp, 0, sizeof(v110rlp));
 sc.channel = chan;
 sc.protocol = kSMDCProtocolV110RLPrx;
 sc.config_length = sizeof(v110rlp);
 sc.config_data = &v110rlp;
 v110rlp.speed = speed;
 r = smdc_channel_config(&sc);
 if (r) return printerr_sm("smdc_channel_config", r);
 return 0;
}

static int chan_control(tSMChannelId chan, int cmd)
{
 SMDC_LINE_CONTROL_PARMS cp;
 int r;
 memset(&cp, 0, sizeof(cp));
 cp.channel = chan;
 cp.cmd = cmd;
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

static int chan_tx_control(tSMChannelId chan, int cmd, int cap)
{
 int r;
 SMDC_TX_CONTROL_PARMS dp;
 memset(&dp, 0, sizeof(dp));
 dp.channel = chan;
 dp.cmd = cmd;
 dp.capacity = cap;
 r = smdc_tx_control(&dp);
 if (r) return printerr_sm("smdc_tx_control", r);
 return 0;
}

static int chan_tx_status(tSMChannelId chan, int *cap, int *sts, int *flo)
{
 int r;
 SMDC_TX_STATUS_PARMS txs;
 memset(&txs, 0, sizeof(txs));
 txs.channel = chan;
 r = smdc_tx_status(&txs);
 *cap = txs.capacity;
 *sts = txs.status;
 *flo = txs.flow;
 if (r) return printerr_sm("smdc_tx_status", r);
 return 0;
}

static int chan_tx_data(tSMChannelId chan, void *buf, int len)
{
 for (;;) {
	SMDC_TX_STATUS_PARMS sp;
	SMDC_DATA_PARMS dp;
	int r;
	memset(&dp, 0, sizeof(dp));
	dp.channel = chan;
	dp.data = buf;
	dp.max_length = len;
	r = smdc_tx_data(&dp);
	if (r) return printerr_sm("smdc_tx_data", r);
	if (! (len -= dp.done_length)) break;
	buf = (char *) buf + dp.done_length;
	memset(&sp, 0, sizeof(sp));
	sp.channel = chan;
	r = smdc_tx_status(&sp);
	if (r) return printerr_sm("smdc_tx_status", r);
 }
 return 0;
}

static int chan_rx_data(int *rxlen, tSMChannelId chan, void *buf, int len)
{
 for (;;) {
	SMDC_RX_STATUS_PARMS sp;
	SMDC_DATA_PARMS dp;
	int r;
	memset(&dp, 0, sizeof(dp));
	dp.channel = chan;
	dp.data = buf;
	dp.max_length = len;
	r = smdc_rx_data(&dp);
	if (r) return printerr_sm("smdc_rx_data", r);
	if ((*rxlen =  dp.done_length)) break;
	memset(&sp, 0, sizeof(sp));
	sp.channel = chan;
	r = smdc_rx_status(&sp);
	if (r) return printerr_sm("smdc_rx_status", r);
 }
 return 0;
}

	// test code
struct txparam {
	enum cstate {CHAN_DEAD, CHAN_STOPPING, CHAN_RUNNING} cstate;
	int busy;
	enum tstate {TX_UNDERRUN, TX_ZERO, TX_FF, TX_PAT} tstate;
	int incr;		// readonly
	tSMChannelId chan;	// unprotected - globally readonly
	char endmsg[128];	// only valid after thread temination
	struct timeb txstart;
	int sts_link, sts_linktx, sts_linkcts;
	int sts_txflow;
	int txlen;
	unsigned char txpat;
};

static const char *inittx(struct txparam *txp)
{
 txp->sts_link=0xaaaa;
 txp->sts_linktx=0xaaaa;
 txp->sts_linkcts=0xaaaa;
 txp->sts_txflow=0xaaaa;
 txp->txlen=0;
 txp->txpat = 0;
 if (chan_tx_control(txp->chan, kSMDCTxCtlNotifyOnCapacity, 32)) {
	return "tx control failed";
 }
 return 0;
}

static int updatetx(struct txparam *txp)
{
 int cap, sts, flo;
 char buff[32];
 unsigned i=0;
 if (txp->cstate != CHAN_RUNNING) return 1;
 switch (txp->tstate) {
 case TX_UNDERRUN: txp->busy = 'U'; break;
 case TX_PAT:
	for (; i < 27; i++) {
		buff[i] = txp->txpat;
		txp->txpat = (txp->txpat + txp->incr) & 0xff;
	}
	break;
 case TX_ZERO:
	for (; i < 27; i++) buff[i] = 0;
	break;
 case TX_FF:
	for (; i < 27; i++) buff[i] = 0xff;
	break;
 }
 if (txp->tstate != TX_UNDERRUN) {
	for (; i < 32; i++) buff[i] = 0xff;
	if (time(0) & 1) buff[30] = 0xfe;
	if (!txp->txlen) ftime(&txp->txstart);
	txp->txlen += 27;
	if (chan_tx_data(txp->chan, buff, sizeof(buff))) return 1;
	txp->busy = 'T';
 }
 if (chan_tx_status(txp->chan, &cap, &sts, &flo)) return 1;
 if (sts == kSMDCTxStatusUnderrun) {
	txp->busy = 'u';
 }
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

struct rxparam {
	enum cstate cstate;
	int incr;		// readonly
	int busy;
	FILE *rxfile;
	tSMChannelId chan;
	unsigned long maxrx;
	int check;
	char endmsg[256];	// only valid after channel closure
	struct timeb rxstart, rxlast;
		// fcnt[had data error][crc said bad]
	unsigned long rxcnt;		// total frames received
	unsigned long zcnt;		// all-zero frames received
	unsigned long lostcnt;		// number of gaps in sequencing
	unsigned long fcnt[2][2];	// frames received
	unsigned rxpos;		// position in current frame
	int lostfrm;		// possible lost frame
	int badfrm;		// current frame has data error
	int badcrc;		// current frame has CRC error
	int rxzero;		// whole frame is zero
	int sts_link, sts_linkrx, sts_linkcts;
	int sts_rxflow;
	unsigned char rxpat;	// next data byte expected
};

static const char *initrx(struct rxparam *rxp)
{
 unsigned i;
 for (i=0; i<arlen(rxp->fcnt); i++) {
	unsigned j;
	for (j=0; j<arlen(rxp->fcnt[i]); j++) {
		rxp->fcnt[i][j]=0;
	}
 }
 rxp->rxcnt=0;
 rxp->zcnt=0;
 rxp->rxpos=0;
 rxp->badfrm=0;
 rxp->badcrc=0;
 rxp->rxzero=1;
 rxp->lostfrm=1;
 rxp->sts_link=0xaaaa;
 rxp->sts_linkrx=0xaaaa;
 rxp->sts_linkcts=0xaaaa;
 rxp->sts_rxflow=0xaaaa;
 rxp->rxpat = -1;
 if (chan_rx_control(rxp->chan, kSMDCRxCtlNotifyOnData, 32, 50)) {
	return "rx control (min/tmo) failed";
 }
 *rxp->endmsg = 0;
 return 0;
}

static int updaterx(struct rxparam *rxp)
{
 unsigned char buff[1024];
 int haderr=0;
 int lasterr=0;
 int p0;
 if (rxp->cstate != CHAN_RUNNING) return 1;
 if (chan_rx_data(&p0, rxp->chan, buff, sizeof(buff))) return 1;
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
		if (rxp->rxpos >= 27) {
			if (rxp->rxpos < 30) {
				if (buff[i]) rxp->rxzero = 0;
				// skip CRC since we don't know what it should be
				lasterr = 0;
			} else if (buff[i] & 0x80) {	// DTX & CRC report
				if (rxp->check > 1) {
					if (buff[i] || !rxp->rxzero) {
						if (!haderr) printf("Error: ");
						if (!lasterr) printf(" @%ld:%d",
							rxp->rxcnt, rxp->rxpos);
						printf(" %02xnz", buff[i]);
					}
					lasterr=1;
				}
				rxp->badcrc = 1;
				rxp->lostfrm = 0;
				haderr=1;
			} else lasterr = 0;
		} else {
			if (buff[i]) rxp->rxzero = 0;
			if (buff[i] != rxp->rxpat) {
				if (!rxp->rxpos && rxp->rxcnt == rxp->zcnt && !buff[i]) {
					// it's ok
					lasterr = 0;
				} else {
					if (rxp->check > 1) {
						if (buff[i] || !rxp->rxzero) {
							if (!haderr) printf("Error: ");
							if (!lasterr) printf(" @%ld:%d",
								rxp->rxcnt, rxp->rxpos);
							printf(" %02x!=%02x",
								buff[i], rxp->rxpat);
							lasterr=1;
						}
					}
					rxp->badfrm = 1;
					haderr=1;
					if (rxp->rxpos) rxp->lostfrm = 0;
				}
				rxp->rxpat = buff[i];
			} else lasterr = 0;
			rxp->rxpat = (rxp->rxpat + rxp->incr) & 0xff;
		}
		if (++rxp->rxpos == 32) {
			rxp->rxpos = 0;
			if (rxp->rxzero) {
				rxp->zcnt++;
				rxp->busy = 'z';
			} else {
				if (rxp->lostfrm && (rxp->badfrm || rxp->badcrc)) {
					rxp->lostcnt++;
					rxp->busy = 'L';
				} else {
					rxp->fcnt[rxp->badfrm][rxp->badcrc]++;
					rxp->busy = rxp->badfrm ? 'r'
						: rxp->badcrc ? 'c' : 'R';
				}
			}
			rxp->badfrm = rxp->badcrc = 0;
			rxp->rxzero = 1;
			rxp->rxcnt++;
			rxp->lostfrm = 1;
		}
	}
	if (haderr) {
		if (rxp->check > 1) printf("\n");
	}
 } else rxp->busy = 'R';
 if (rxp->rxfile && fwrite(buff, 1, p0, rxp->rxfile) != (unsigned) p0) {
	perror("fwrite failed");
	return 1;
 }
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
	sprintf(endstr(rxp->endmsg), "Got %ld (+%ld zero) frames in %g S",
		rxp->rxcnt, rxp->zcnt,
		rxtim);
	if (rxtim) sprintf(endstr(rxp->endmsg), " = %g frames/sec (%g bps)",
		rxp->rxcnt / rxtim, 27 * 8.0 * rxp->rxcnt / rxtim);
	if (rxp->lostcnt) sprintf(endstr(rxp->endmsg), "\nLost: %ld ", rxp->lostcnt);
	if (rxp->fcnt[0][1]
		|| rxp->fcnt[1][0]
		|| rxp->fcnt[1][1]) {
		sprintf(endstr(rxp->endmsg), "%sErrors: crc:%ld (%g) data:%ld (%g) crc&data: %ld (%g)",
			rxp->lostcnt ? "" : "\n",
			rxp->fcnt[0][1], rxp->fcnt[0][1] / (double) rxp->rxcnt,
			rxp->fcnt[1][0], rxp->fcnt[1][0] / (double) rxp->rxcnt,
			rxp->fcnt[1][1], rxp->fcnt[1][1] / (double) rxp->rxcnt);
	}
 } else sprintf(endstr(rxp->endmsg), "Data not counted");
}

	// initialisation

static int startchan(struct txparam *txp, struct rxparam *rxp, SM_CHANNEL_ALLOC_PLACED_PARMS *sa, MVIP *ints, MVIP *outts, long speed, int check, unsigned long maxrx, char *pfx, struct pollfd *fdp)
{
 SM_SWITCH_CHANNEL_PARMS swp;
 tSMEventId ev;
 int r;
 const char *s;
 	// set up rx
 sa->type = kSMChannelTypeInput;
 r = sm_channel_alloc_placed(sa);
 if (r) return printerr_sm("sm_channel_alloc_placed (rx)", r);
 memset(&swp, 0, sizeof(swp));
 swp.channel = rxp->chan = sa->channel;
 swp.st = ints->stream;
 swp.ts = ints->timeslot;
 swp.type = ints->type;
 if (++ints->timeslot >= 32) {
	ints->timeslot = 0;
	ints->stream++;
 }
 r = sm_switch_channel_input(&swp);
 if (r) return printerr_sm("sm_switch_channel_input", r);
 if (chan_config_v110rlprx(rxp->chan, speed)) return 1;
 rxp->maxrx = maxrx;
 rxp->check = check;
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
 	// set up tx
 sa->type = kSMChannelTypeOutput;
 r = sm_channel_alloc_placed(sa);
 if (r) return printerr_sm("sm_channel_alloc_placed (tx)", r);
 swp.channel = txp->chan = sa->channel;
 swp.st = outts->stream;
 swp.ts = outts->timeslot;
 swp.type = outts->type;
 if (++outts->timeslot >= 32) {
	outts->timeslot = 0;
	outts->stream++;
 }
 r = sm_switch_channel_output(&swp);
 if (r) return printerr_sm("sm_switch_channel_output", r);
 if (chan_config_v110rlptx(txp->chan, speed)) return 1;
 txp->busy = rxp->busy = 0;
 txp->incr = rxp->incr = outts->timeslot+1;
 txp->tstate = TX_PAT;
 rxp->cstate = CHAN_RUNNING;
 txp->cstate = CHAN_RUNNING;
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
 smd_ev_create(&ev, txp->chan, kSMEventTypeWriteData, kSMChannelSpecificEvent);
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
	switch (txp[chan].cstate) {
	case CHAN_DEAD: *cp++ = '_'; break;
	case CHAN_STOPPING: *cp++ = 'x'; break;
	case CHAN_RUNNING:
		if (txp[chan].busy) *cp++ = txp[chan].busy;
		else *cp++ = '.';
	}
	if (txp[chan].busy != 'U') txp[chan].busy = 0;
	switch (rxp[chan].cstate) {
	case CHAN_DEAD: *cp++ = '_'; break;
	case CHAN_STOPPING: *cp++ = 'x'; break;
	case CHAN_RUNNING:
		if (rxp[chan].busy) *cp++ = rxp[chan].busy;
		else *cp++ = '.';
	}
	*cp++ = ' ';
	rxp[chan].busy = 0;
 }
 *cp++ = twiddle[twidpos];
 if (++twidpos == arlen(twiddle)-1) twidpos = 0;
 *cp = 0;
 fputs(disp, stdout);
 free(disp);
}

	// starting everything up and shutting it down

static int testrlp(tSMModuleId modnum, long speed, int check, unsigned long maxrx, MVIP *ints, MVIP *outts, char *pfx, unsigned numchan)
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
	txp[chan].cstate = CHAN_DEAD;
	rxp[chan].cstate = CHAN_DEAD;
 }
 	// initialisation needed by data transfer threads
 memset(&sa, 0, sizeof(sa));
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
	i = 1000 * i;				// convert to mS
	i += laststs.millitm - now.millitm;	// include mS
	if (i <= 0) {
		showsts(txp, rxp, numchan);
		i = 1000;
		laststs = now;
	}
	while (fds[minfd].fd == -1) minfd++;	// trim initial closed chans
	if (minfd >= nfds) break;	// only kbd left
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
		case 'd':
			for (chan=0; chan < numchan; chan++)
				chan_rx_control(rxp[chan].chan, kSMDCRxCtlDiscard, 0, 0);
			break;
		case 'R':
			for (chan=0; chan < numchan; chan++)
				chan_control(txp[chan].chan, kSMDCLineCtlCmdUnassertRTS);
			break;
		case 'r':
			for (chan=0; chan < numchan; chan++)
				chan_control(txp[chan].chan, kSMDCLineCtlCmdAssertRTS);
			break;
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
				if (txp[chan].cstate == CHAN_RUNNING
					&& rxp[chan].cstate == CHAN_RUNNING) {
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
		for (pfd=fds, j=0; j*2 < nfds; pfd++, j++) {
			int end = 0;
			if (pfd->revents & POLLIN) {
				pfd->revents &= ~POLLIN;
				end = updaterx(rxp+j);
			} else {
				end = rxp[j].cstate == CHAN_STOPPING;
				if (rxp[j].cstate != CHAN_DEAD) {
					SMDC_LINE_STATUS_PARMS lp;
					memset(&lp, 0, sizeof(lp));
					lp.channel = rxp[j].chan;
					if (!smdc_line_status(&lp)) {
						if (lp.rx_status == kSMDCRxStatusNoCarrier) rxp[j].busy = 's';
					}
				}
			}
			if (end) {
				int r;
				pfd->fd = -1;
				finishrx(rxp+j);
				printf("%dr: %s\n", j, rxp[j].endmsg);
				if (txp[j].cstate != CHAN_DEAD) {
					txp[j].cstate = CHAN_STOPPING;
				}
				rxp[j].cstate = CHAN_DEAD;
				r = sm_channel_release(rxp[j].chan);
				if (r) return printerr_sm("sm_channel_release", r);
			}
			end = 0;
			if ((++pfd)->revents & POLLOUT) {	// tx
				pfd->revents &= ~POLLOUT;
				end = updatetx(txp+j);
			} else {
				end = txp[j].cstate == CHAN_STOPPING;
				if (txp[j].tstate == TX_UNDERRUN) {
					pfd->events = 0;
				}
			}
			if (end) {
				int r;
				pfd->fd = -1;
				finishtx(txp+j);
				printf("%dt: %s\n", j, txp[j].endmsg);
				if (rxp[j].cstate != CHAN_DEAD) {
					rxp[j].cstate = CHAN_STOPPING;
				}
				txp[j].cstate = CHAN_DEAD;
				r = sm_channel_release(txp[j].chan);
				if (r) return printerr_sm("sm_channel_release", r);
			}
		}
		for (j=0; j < nfds; j++) {
			if (fds[j].revents) printf("fd[%d].revents = 0x%x\n", j, fds[j].revents);
		}
	}
 }
 free(rxp);
 free(txp);
 free(fds);
 return 0;
}

#include "gen/polltestrlp.args.i"

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
 sts = testrlp(mod, arg.speed, arg.check, arg.maxrx, &arg.intimeslot, &arg.outtimeslot, arg.fileprefix, arg.numchan);
 if (arg.serialnumber) {
	modclose(mod);
	cardclose(card);
 }
 return sts;
}
