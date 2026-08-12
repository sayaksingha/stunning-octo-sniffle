
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/timeb.h>
#include "smdrvr.h"
//#include <conio.h>
#include "smdc.h"
#include "smdc_raw.h"
#include "smdc_hdlc.h"

#include <malloc.h>

#include "../Testlib/cardopen.h"
#include "../Testlib/errcode_sm.h"
#include "../Testlib/error.h"
#include "../Testlib/modopen.h"
#include "../../libutil/generic_io.h"

#define arlen(s) (sizeof(s)/sizeof(*(s)))

#ifdef TiNGTYPE_LINUX
#include <poll.h>
#include <pthread.h>
#include <unistd.h>
typedef struct pollfd WAITABLE;
#define USE_POLL
#endif

#ifdef TiNGTYPE_QNX
#include <sys/poll.h>
#include <pthread.h>
#include <unistd.h>
typedef struct pollfd WAITABLE;
#define USE_POLL
#endif


#ifdef TiNGTYPE_WINNT
#include "../../libutil/WINNT/lasterr.h"
typedef HANDLE WAITABLE;
/* disable stupid warnings about conversions to smaller types */

#pragma warning(disable:4018)
#pragma warning(disable:4761)
#pragma warning(disable:4244)
#pragma warning(disable:4305)

#endif

#include "../../apilib/smdc_hdlc.h"

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
static int chan_config_hdlctx(tSMChannelId chan, unsigned crc, unsigned crcsize)
{
 SMDC_CHANNEL_CONFIG_PARMS sc;
 SMDC_HDLC_FORMAT_PARMS hdlc;
 int r;
 memset(&sc, 0, sizeof(sc));
 sc.channel = chan;
 sc.protocol = kSMDCProtocolRawTx;
 memset(&hdlc, 0, sizeof(hdlc));
 hdlc.crc = crc;
 hdlc.crcsize = crcsize;
 sc.encoding = kSMDCConfigEncodingHDLC;
 sc.encoding_config_length = sizeof(hdlc);
 sc.encoding_config_data = &hdlc;
 r = smdc_channel_config(&sc);
 if (r) return printerr_sm("smdc_channel_config", r);
 return 0;
}

static int chan_config_hdlcrx(tSMChannelId chan, unsigned crc, unsigned crcsize)
{
 SMDC_CHANNEL_CONFIG_PARMS sc;
 SMDC_HDLC_FORMAT_PARMS hdlc;
 int r;
 memset(&sc, 0, sizeof(sc));
 sc.channel = chan;
 sc.protocol = kSMDCProtocolRawRx;
 memset(&hdlc, 0, sizeof(hdlc));
 hdlc.crc = crc;
 hdlc.crcsize = crcsize;
 sc.encoding = kSMDCConfigEncodingHDLC;
 sc.encoding_config_length = sizeof(hdlc);
 sc.encoding_config_data = &hdlc;
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
	tSMChannelId chan;
	tSMEventId event;
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
 if (chan_tx_control(txp->chan, kSMDCTxCtlNotifyOnCapacity, 513)) {
	return "tx control failed";
 }
 return 0;
}

static int updatetx(struct txparam *txp)
{
 int cap, sts, flo;
 char buff[512];
 unsigned j;
 if (txp->cstate != CHAN_RUNNING) return 1;
 for (j=0; j<arlen(buff); j += 32) {
	unsigned i=0;
	buff[j + i++] = 28*8;
	buff[j + i++] = 0;
	buff[j + i++] = 0;
	buff[j + i++] = 0;
	switch (txp->tstate) {
	case TX_UNDERRUN: txp->busy = 'U'; break;
	case TX_PAT:
		for (; i < 32; i++) {
			buff[j + i] = txp->txpat;
			txp->txpat = (txp->txpat + txp->incr) & 0xff;
		}
		break;
	case TX_ZERO:
		for (; i < 32; i++) buff[j + i] = 0;
		break;
	case TX_FF:
		for (; i < 32; i++) buff[j + i] = 0xff;
		break;
	}
 }
 if (txp->tstate != TX_UNDERRUN) {
	if (!txp->txlen) ftime(&txp->txstart);
	txp->txlen += j/32*28;
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
 if (txp->txlen) {
	double txtim;
	txtim = now.millitm - txp->txstart.millitm;
	txtim /= 1000.0;
	txtim += now.time - txp->txstart.time;
	if (!txtim) txtim = 0.001;
	sprintf(endstr(txp->endmsg), "sent %d bytes in %g S = %g bytes/sec (%g bps)",
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
	tSMEventId event;
	unsigned long maxrx;
	int check;
	char endmsg[256];	// only valid after channel closure
	struct timeb rxstart, rxlast;
		// fcnt[had data error][crc said bad]
	unsigned long rxbyt;		// total bytes received
	unsigned long fcnt[2][2];	// frames received
	int sts_link, sts_linkrx, sts_linkcts;
	int sts_rxflow;
	unsigned char rxpat;	// next data byte expected
	unsigned nextcheck;	// next character to check in curfram
	unsigned nc;		// number of characters in curfram
	unsigned char curfram[1024];
	unsigned crcsize;
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
 rxp->rxbyt=0;
 rxp->nextcheck=0;
 rxp->nc=0;
 rxp->sts_link=0xaaaa;
 rxp->sts_linkrx=0xaaaa;
 rxp->sts_linkcts=0xaaaa;
 rxp->sts_rxflow=0xaaaa;
 rxp->rxpat = 0;
 if (chan_rx_control(rxp->chan, kSMDCRxCtlNotifyOnData, 512, 500)) {
	return "control failed";
 }
 return 0;
}

static int updaterx(struct rxparam *rxp)
{
 int p0;
 if (rxp->cstate != CHAN_RUNNING) return 1;
 if (rxp->nc >= sizeof(rxp->curfram)) {
	fprintf(stderr, "rx frame too long\n");
	return 1;
 }
 if (chan_rx_data(&p0, rxp->chan, &rxp->curfram[rxp->nc], sizeof(rxp->curfram)-rxp->nc)) return 1;
 ftime(&rxp->rxlast);
 if (!rxp->rxbyt) rxp->rxstart = rxp->rxlast;
 rxp->busy = 'R';
 rxp->rxbyt += p0;
 if (rxp->rxfile && fwrite(&rxp->curfram[rxp->nc], 1, p0, rxp->rxfile) != (unsigned) p0) {
	perror("fwrite failed");
	return 1;
 }
 if (rxp->check) {
	unsigned int i;
	int lasterr=0;
	rxp->nc += p0;
	for (i = rxp->nextcheck; i+12 <= rxp->nc; i+=4) {
		if (rxp->curfram[i] != 0xff) continue;
			// marker may start at [i-3..i]
//		while (i>0 && rxp->curfram[i-1] == 0xff) i--; marker always on 32bit boundry
		if (rxp->curfram[i+1] == 0xff
			&& rxp->curfram[i+2] == 0xff
			&& rxp->curfram[i+3] == 0xff) {
			if (rxp->curfram[i+4] == 0xff
				&& rxp->curfram[i+5] == 0xff
				&& rxp->curfram[i+6] == 0xff
				&& rxp->curfram[i+7] == 0xff) {	// escaped
				unsigned j;
				for (j=i+4; j < rxp->nc; j++)
					rxp->curfram[j-4] = rxp->curfram[j];
				rxp->nc -= 4;
				rxp->rxbyt -= 4; // not counted as received
			} else {	// it's end of frame
				static char eof[8] = "\076\0\0\0\0\0\0\0";
				int haderr=0;
				int badcrc=0;
				unsigned j;
				for (j=0; j<i; j++) {
					if (rxp->curfram[j] != rxp->rxpat) {
						if (rxp->check > 1) {
							if (!haderr) printf("Error: ");
							if (!lasterr) printf(" @%ld",
								rxp->rxbyt-rxp->nc+j);
							printf(" %02x!=%02x",
								rxp->curfram[j], rxp->rxpat);
						}
						haderr=1;
					} else lasterr=0;
					rxp->rxpat = rxp->curfram[j];
					rxp->rxpat = (rxp->rxpat + rxp->incr) & 0xff;
				}
				i += 4;	// skip 0xffffffff
				i += rxp->crcsize/8; // FIXME crcsize may not be multiple of 8 bits
				for (j=0; j<(8-rxp->crcsize/8); i++, j++) {
					if (rxp->curfram[i] != eof[j]) {
						if (rxp->check > 1) {
							if (!haderr) printf("Error: ");
							if (!lasterr) printf(" @%ld",
								rxp->rxbyt-rxp->nc+i);
							printf(" %02x!=%02x",
								rxp->curfram[i], eof[j]);
						}
						if (j == 4 && rxp->curfram[i] & 0x02) {
							badcrc=1;
						} else haderr=1;
					} else lasterr=0;
				}
				if (haderr || badcrc) {
					if (rxp->check > 1) printf("\n");
				}
				rxp->fcnt[haderr][badcrc]++;
				rxp->busy = haderr ? 'r' : badcrc ? 'c' : 'R';
				rxp->rxbyt -= 12;
				p0 -= 12;
				for (j=0; i < rxp->nc; j++, i++)
					rxp->curfram[j] = rxp->curfram[i];
				rxp->nc = j;
				i = -4;
			}
		}
	}
	rxp->nextcheck = i;
 }
 if (rxp->maxrx) {
	if (p0 > 0 && (unsigned) p0 >= rxp->maxrx) {
		sprintf(endstr(rxp->endmsg), "[limit] ");
		return 1;
	}
	rxp->maxrx -= p0;
 }
 return 0;
}

static void finishrx(struct rxparam *rxp)
{
 if (rxp->rxfile) fclose(rxp->rxfile);
 if (rxp->rxbyt) {
	double rxtim;
	const double rxfram = rxp->fcnt[0][0]
		+ rxp->fcnt[0][1]
		+ rxp->fcnt[1][0]
		+ rxp->fcnt[1][1];
	rxtim = rxp->rxlast.millitm - rxp->rxstart.millitm;
	rxtim /= 1000.0;
	rxtim += rxp->rxlast.time - rxp->rxstart.time;
	sprintf(endstr(rxp->endmsg), "Got %ld bytes in %g S",
		rxp->rxbyt,
		rxtim);
	if (rxtim) sprintf(endstr(rxp->endmsg), " = %g bytes/sec (%g bps, %gfps)",
		rxp->rxbyt / rxtim, 8.0 * rxp->rxbyt / rxtim, rxfram/rxtim);
	if (rxfram != rxp->fcnt[0][0]) {
		sprintf(endstr(rxp->endmsg), "\nErrors: crc:%ld (%g) data:%ld (%g) crc&data: %ld (%g)",
			rxp->fcnt[0][1], rxp->fcnt[0][1] / rxfram,
			rxp->fcnt[1][0], rxp->fcnt[1][0] / rxfram,
			rxp->fcnt[1][1], rxp->fcnt[1][1] / rxfram);
	}
 } else sprintf(endstr(rxp->endmsg), "Data not counted");
}

	// initialisation

static int startchan(struct txparam *txp, struct rxparam *rxp, SM_CHANNEL_ALLOC_PLACED_PARMS *sa, MVIP *ints, MVIP *outts, unsigned crc, unsigned crcsize, int check, unsigned long maxrx, char *pfx, WAITABLE *waitp)
{
 SM_CHANNEL_SET_EVENT_PARMS sep;
 SM_SWITCH_CHANNEL_PARMS swp;
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
 if (chan_config_hdlcrx(rxp->chan, crc, crcsize)) return 1;
 rxp->maxrx = maxrx;
 rxp->check = check;
 rxp->crcsize = crcsize;
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
 if (chan_config_hdlctx(txp->chan, crc, crcsize)) return 1;
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
 memset(&sep, 0, sizeof(sep));
 r = smd_ev_create(&sep.event, rxp->chan, kSMEventTypeReadData, kSMChannelSpecificEvent);
 if (r) printerr_sm("smd_ev_create (rx)", r);
#ifdef TiNGTYPE_WINNT
 *waitp = sep.event;
#if 0
 printf("rxp->event = %x\n", sep.event);
#endif
#endif
#ifdef USE_POLL
 waitp->fd = sep.event.fd;
 waitp->events = sep.event.mode;
#endif
 rxp->event = sep.event;
 sep.channel = rxp->chan;
 sep.event_type = kSMEventTypeReadData;
 sep.issue_events = kSMChannelSpecificEvent;
 r = sm_channel_set_event(&sep);
 if (r) printerr_sm("sm_channel_set_event (rx)", r);
 waitp++;
 memset(&sep, 0, sizeof(sep));
 r = smd_ev_create(&sep.event, txp->chan, kSMEventTypeWriteData, kSMChannelSpecificEvent);
 if (r) printerr_sm("smd_ev_create (tx)", r);
#ifdef TiNGTYPE_WINNT
 *waitp = sep.event;
#if 0
 printf("txp->event = %x\n", sep.event);
#endif
#endif
#ifdef USE_POLL
 waitp->fd = sep.event.fd;
 waitp->events = sep.event.mode;
#endif
 txp->event = sep.event;
 sep.channel = txp->chan;
 sep.event_type = kSMEventTypeWriteData;
 sep.issue_events = kSMChannelSpecificEvent;
 r = sm_channel_set_event(&sep);
 if (r) printerr_sm("sm_channel_set_event (tx)", r);
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

static int handle_kbd(WAITABLE *waitp, unsigned nwaitp, struct txparam *txp, struct rxparam *rxp, unsigned numchan)
{
 unsigned chan;
 char c;
#ifdef USE_POLL
 int i;
 i = read(waitp[nwaitp].fd, &c, 1);
 if (i < 0) {
	perror("read() failed");
	return 1;
 }
 if (!i) return 1;
#endif
#ifdef TiNGTYPE_WINNT
 c = io_inkey(waitp[nwaitp]);
 if (c == EOF) {
 	fprintf(stderr, "Got EOF from kbd\n");
 	return 1;
 }
#endif
 switch (c) {
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
#ifdef USE_POLL
		waitp[chan*2+1].events = POLLOUT;
#endif
	}
	break;
 case '0':
	for (chan=0; chan < numchan; chan++) {
		txp[chan].tstate = TX_ZERO;
#ifdef USE_POLL
		waitp[chan*2+1].events = POLLOUT;
#endif
	}
	break;
 case 'U':
	for (chan=0; chan < numchan; chan++) {
		txp[chan].tstate = TX_PAT;
#ifdef USE_POLL
		waitp[chan*2+1].events = POLLOUT;
#endif
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
 return 0;
}


static int handle_rx(WAITABLE *pfd, struct rxparam *rxp, struct txparam *txp)
{
 int r;
 if (!updaterx(rxp)) return 0;
 if (rxp->cstate != CHAN_DEAD) {
	SMDC_LINE_STATUS_PARMS lp;
	memset(&lp, 0, sizeof(lp));
	lp.channel = rxp->chan;
	if (!smdc_line_status(&lp)) {
		if (lp.rx_status == kSMDCRxStatusNoCarrier) rxp->busy = 's';
	}
 }
#ifdef USE_POLL
 pfd->fd = -1;
#endif
 finishrx(rxp);
 printf("%s\n", rxp->endmsg);
 if (txp->cstate != CHAN_DEAD) {
	txp->cstate = CHAN_STOPPING;
 }
 rxp->cstate = CHAN_DEAD;
 r = sm_channel_release(rxp->chan);
 if (r) return printerr_sm("sm_channel_release", r);
 return 0;
}

static int handle_tx(WAITABLE *pfd, struct txparam *txp, struct rxparam *rxp)
{
 int r;
 if (!updatetx(txp)) return 0;
 if (txp->tstate == TX_UNDERRUN) {
#ifdef USE_POLL
	pfd->events = 0;
#endif
	return 0;
 }
#ifdef USE_POLL
 pfd->fd = -1;
#endif
 finishtx(txp);
 printf("%s\n", txp->endmsg);
 if (rxp->cstate != CHAN_DEAD) {
	rxp->cstate = CHAN_STOPPING;
 }
 txp->cstate = CHAN_DEAD;
 r = sm_channel_release(txp->chan);
 if (r) return printerr_sm("sm_channel_release", r);
 return 0;
}

	// starting everything up and shutting it down

static int testhdlc(tSMModuleId module, unsigned crc, unsigned crcsize, int check, unsigned long maxrx, MVIP *ints, MVIP *outts, char *pfx, unsigned numchan)
{
 unsigned nwaitp = 2*numchan;	// N*(tx+rx)+kbd
 unsigned minwaitp = 0;
 WAITABLE *waitp = (WAITABLE *) malloc((nwaitp+1) * sizeof(*waitp));
 SM_CHANNEL_ALLOC_PLACED_PARMS sa;
 struct timeb laststs;
 struct txparam *txp;
 struct rxparam *rxp;
#ifdef TiNGTYPE_WINNT
 unsigned *indx;
#endif
 unsigned chan;
 	// general initialisation
 // txp[X] is waitp 2*X
 // rxp[X] is waitp 2*X+1
 if (!waitp) {
	fprintf(stderr, "Cannot allocate memory for file descriptors\n");
	return 1;
 }
#ifdef TiNGTYPE_WINNT
 indx = (unsigned *) malloc((nwaitp+1) * sizeof(*indx));
 if (!indx) {
	fprintf(stderr, "Cannot allocate memory for wait index\n");
	return 1;
 }
#endif
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
 sa.module = module;
 for (chan=0; chan < numchan; chan++) {
	int sts;
	sprintf(rxp[chan].endmsg, "%dr: ", chan);
	sprintf(txp[chan].endmsg, "%dt: ", chan);
	sts = startchan(
		txp+chan,
		rxp+chan,
		&sa,
		ints,
		outts,
		crc,
		crcsize,
		check,
		maxrx,
		pfx,
		waitp+chan*2);
	if (sts) return 1;
 }
#ifdef USE_POLL
 waitp[nwaitp].events = POLLIN | POLLHUP;
 waitp[nwaitp].fd = 0;
#endif
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
#ifdef USE_POLL
	while (waitp[minwaitp].fd == -1) minwaitp++;	// trim initial closed chans
	if (minwaitp >= nwaitp) break;	// only kbd left
	i = poll(waitp, nwaitp+1, i);
	if (i < 0) {
		perror("poll() failed");
		break;
	}
	if (waitp[nwaitp].revents & POLLIN) {
		if (handle_kbd(waitp, nwaitp, txp, rxp, numchan)) break;
	} else {
		unsigned j;
		for (j=0; j*2 < nwaitp; j++) {
			if (waitp[j*2].revents & POLLIN && handle_rx(&waitp[j*2], rxp+j, txp+j)) return 1;
			if (waitp[j*2+1].revents & POLLOUT && handle_tx(&waitp[j*2+1], txp+j, rxp+j)) return 1;
		}
	}
#endif
#ifdef TiNGTYPE_WINNT
	{
	 unsigned j, k=0;
	 for (j=0; j*2 < nwaitp; j++) {
	 	switch (rxp[j].cstate) {
	 	case CHAN_STOPPING:
			if (handle_rx(&waitp[k], rxp+j, txp+j)) return 1;
			break;
	 	case CHAN_RUNNING:
	 		waitp[k] = rxp[j].event;
	 		indx[k] = j*2;
	 		k++;
	 		break;
		}
	 	if (txp[j].cstate == CHAN_RUNNING
	 		&& txp[j].tstate != TX_UNDERRUN) {
	 		waitp[k] = txp[j].event;
	 		indx[k] = j*2+1;
	 		k++;
		}
	 }
	 if (!k) break;
	 waitp[k] = GetStdHandle(STD_INPUT_HANDLE);
	 indx[k] = nwaitp;
#if 0
	 {
	  unsigned j;
	  printf("Wait(%d, [", k+1);
	  for (j=0; j<k+1; j++) printf(" %x", waitp[j]);
	  printf("], 0, %d)\n", i);
	 }
#endif
	 i = WaitForMultipleObjects(k+1, waitp, 0, i);
	 if (i == WAIT_FAILED) {
		lasterr("WaitForMultipleObjects failed");
		break;
	 }
	 if (i >= WAIT_OBJECT_0 && i <= WAIT_OBJECT_0+nwaitp) {
		i = indx[i-WAIT_OBJECT_0];
		j = i/2;
		if (i == nwaitp) {
			if (handle_kbd(waitp, nwaitp, txp, rxp, numchan)) break;
		} else if (i & 1) {
			if (handle_tx(&waitp[i], txp+j, rxp+j)) return 1;
		} else {
			if (handle_rx(&waitp[i], rxp+j, txp+j)) return 1;
		}
	 } else if (i != WAIT_TIMEOUT) {
		fprintf(stderr, "WaitForMultipleObjects returned 0x%x\n", i);
	 }
	}
#endif
 }
 free(rxp);
 free(txp);
 free(waitp);
 return 0;
}

#include "gen/polltesthdlc.args.i"

int main(int argc, char **argv)
{
 tSMModuleId module; 
 tSMCardId card;
 int sts;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || *argv) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
	return 1;
 }
 if (!arg.numchan) arg.numchan = 1;
 arg.maxrx *= 8000;
 if (arg.serialnumber) {
	err_t e = modopen(&card, &module, arg.serialnumber, arg.module);
	if (e) {
		error_log(stderr, e);
		return 1;
	}
 } else {
	fprintf(stderr, "Serial number is required\n Usage %s" ARGS_USAGE "\n",progname);
 }
 sts = testhdlc(module, arg.crc, arg.crcsize, arg.check, arg.maxrx, &arg.intimeslot, &arg.outtimeslot, arg.fileprefix, arg.numchan);
 if (arg.serialnumber) {
	modclose(module);
	cardclose(card);
 }
 return sts;
}
