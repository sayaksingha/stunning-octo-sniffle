/* polltest.c - test various protcols by even-driver polling */

#ifdef TiNGTYPE_LINUX
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
typedef struct pollfd WAITABLE;
#define USE_POLL
#endif

#ifdef TiNGTYPE_QNX
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/poll.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
typedef struct pollfd WAITABLE;
#define USE_POLL
#endif


#ifdef TiNGTYPE_WINNT
#include "../../libutil/WINNT/wind.h"
#include "../../libutil/WINNT/lasterr.h"
typedef HANDLE WAITABLE;
/* disable stupid warnings about conversions to smaller types */

#pragma warning(disable:4761)
#pragma warning(disable:4244)
#pragma warning(disable:4305)

#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/timeb.h>
#include "smdrvr.h"
//#include <conio.h>
#include "../../apilib/smsync.h"	// unpublished API
#include "smdc.h"

#include <malloc.h>

#include "../Testlib/cardopen.h"
#include "../Testlib/modopen.h"
#include "../Testlib/errcode_sm.h"
#include "../Testlib/error.h"
#include "../../libutil/generic_io.h"

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

#define TEST_STRUCTS

static int fn_smdc_channel_config(tSMChannelId chan, int protocol, void *conf, int conflen, int encoding, void *enc, int enclen)
{
 SMDC_CHANNEL_CONFIG_PARMS sc;
 int r;
 memset(&sc, 0, sizeof(sc));
 sc.channel = chan;
 sc.protocol = protocol;
 sc.config_length = conflen;
 sc.config_data = conf;
 sc.encoding = encoding;
 sc.encoding_config_length = enclen;
 sc.encoding_config_data = enc;
 r = smdc_channel_config(&sc);
 if (r) return printerr_sm("smdc_channel_config", r);
 return 0;
}

static int fn_smdc_line_control(tSMChannelId chan, int cmd, int mask, int toggle)
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

static int fn_smdc_tx_status(tSMChannelId chan, int *cap, int *sts, int *flo)
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

static int fn_smdc_tx_control(tSMChannelId chan, int cmd, int cap)
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

static int fn_smdc_tx_data(tSMChannelId chan, void *buf, int len)
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

static int fn_smdc_rx_control(tSMChannelId chan, int cmd, int mincol, int minidl)
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

static int fn_smdc_rx_data(int *rxlen, tSMChannelId chan, void *buf, int len)
{
 SMDC_RX_STATUS_PARMS sp;
 SMDC_DATA_PARMS dp;
 int r;
 memset(&dp, 0, sizeof(dp));
 dp.channel = chan;
 dp.data = buf;
 dp.max_length = len;
 r = smdc_rx_data(&dp);
 if (r) return printerr_sm("smdc_rx_data", r);
 if ((*rxlen = dp.done_length)) return 0;
 memset(&sp, 0, sizeof(sp));
 sp.channel = chan;
 r = smdc_rx_status(&sp);
 if (r) return printerr_sm("smdc_rx_status", r);
 return 0;
}

static int fn_smdc_rx_status(tSMChannelId chan, int *avail, int *sts, int *flo)
{
 int r;
 SMDC_RX_STATUS_PARMS rxs;
 memset(&rxs, 0, sizeof(rxs));
 rxs.channel = chan;
 r = smdc_rx_status(&rxs);
 *avail = rxs.available_octets;
 *sts = rxs.status;
 *flo = rxs.flow;
 if (r) return printerr_sm("smdc_rx_status", r);
 return 0;
}

#define TEST_STRUCTS_TX
	// transmit stuff
struct txparam {
	enum cstate {CHAN_DEAD, CHAN_STOPPING, CHAN_RUNNING} cstate;
	char txbusy;
	enum tstate {TX_UNDERRUN, TX_ZERO, TX_FF, TX_PAT, } tstate;
	int incr;		// readonly
	tSMChannelId chan;	// unprotected - globally readonly
	tSMEventId event;
	char endmsg[128];	// only valid after thread temination
	struct timeb txstart;
	int txlen;
	unsigned ppos;		// position on pattern
	unsigned psiz;		// bytes in pattern (as transmitted)
	unsigned long txpat;
	struct proto *txproto;
	union {
		struct {
#include "hdlctest.i"
		} hdlc;
		struct {
#include "rawtest.i"
		} raw;
		struct {
#include "rlptest.i"
		} rlp;
		struct {
#include "v110test.i"
		} v110;
	} u;
};

#undef TEST_STRUCTS_TX

	// receive stuff
struct rxparam {
	enum cstate cstate;
	int incr;		// readonly
	int rxbusy;
	FILE *rxfile;
	tSMChannelId chan;
	tSMEventId event;
	unsigned long maxrx;
	int check;
	int hadok;		// error rate only starts at first correct
	char endmsg[256];	// only valid after channel closure
	struct timeb rxstart, rxlast;
	unsigned psiz;		// bytes in pattern (as transmitted)
	unsigned long rxdata;	// total bytes received
	unsigned long rxcnt;	// total data bytes received
	unsigned long rxpat;
	unsigned long rxdat;
	int rxpos;
	struct proto *rxproto;
	union {
		struct {
#include "hdlctest.i"
		} hdlc;
		struct {
#include "rawtest.i"
		} raw;
		struct {
#include "rlptest.i"
		} rlp;
		struct {
#include "v110test.i"
		} v110;
	} u;
};

#undef TEST_STRUCTS

static void patfill(unsigned char *buff, unsigned nc, struct txparam *txp)
{
 switch (txp->tstate) {
 case TX_UNDERRUN:
	fprintf(stderr, "Internal error: patfill called tx for underrun\n");
	break;
 case TX_PAT:
 	if (txp->psiz > 1) {
		for (; nc--; ) {
			*buff++ = (txp->txpat >> 7*txp->ppos) & 0x7f;
			if (++txp->ppos >= txp->psiz) {
				buff[-1] |= 0x80;
				txp->txpat += txp->incr;
				txp->ppos = 0;
			}
		}
	} else {
		for (; nc--; ) {
			*buff++ = txp->txpat;
			txp->txpat += txp->incr;
		}
	}
	break;
 case TX_ZERO:
	for (; nc--; ) *buff++ = 0;
	break;
 case TX_FF:
	for (; nc--; ) *buff++ = 0xff;
	break;
 }
}

static unsigned patcheck(struct rxparam *rxp, unsigned char *buff, unsigned nc, int ofs)
{
 unsigned err = 0;
 if (!rxp->check) return 0;
 if (rxp->psiz > 1) {
	for (; nc; nc--) {
		rxp->rxdat |= (*buff & 0x7f) << rxp->rxpos;
		if (((rxp->rxpat >> rxp->rxpos) ^ *buff) & 0x7f) {
			if (rxp->check > 2) {
				if (!err) printf("@%lx", rxp->rxdata+ofs);
				printf(" %02lx!=%02lx[%d:%d]",
					rxp->rxdat, rxp->rxpat, rxp->rxpos+7, rxp->rxpos);
			}
			err++;
		}
		rxp->rxpos += 7;
		ofs++;
		if (*buff++ & 0x80) {
			rxp->rxpat = rxp->rxdat + rxp->incr;
			rxp->rxpat &= (1 << 7*rxp->psiz)-1;
			rxp->rxpos = 0;
			rxp->rxdat = 0;
		}
	}
 } else {
	for (; nc; nc--) {
		if (rxp->rxpat != *buff) {
			if (rxp->check > 2) {
				if (!err) printf("@%lx", rxp->rxdata+ofs);
				printf(" %02x!=%02lx",
					*buff, rxp->rxpat);
			}
			err++;
		}
		rxp->rxpat = (*buff++ + rxp->incr) & 0xff;
		ofs++;
	}
 }
 if (err) {
	rxp->rxbusy = 'r';
	switch (rxp->check) {
	case 0: break;
	case 1: break;
	case 2: printf("ERR\n"); break;
	default: printf("\n");
	}
 }
 if (!err) rxp->hadok = 1;
 else if (!rxp->hadok) return 0;
 return err;
}

static int simple_txsts(struct txparam *txp)
{
 int cap, sts, flo;
 if (txp->cstate != CHAN_RUNNING) return 1;
 if (fn_smdc_tx_status(txp->chan, &cap, &sts, &flo)) return 1;
 printf("Txs: %d %d %d\n", cap, sts, flo);
 return 0;
}

static int simple_rxsts(struct rxparam *rxp)
{
 int p1, p2, p3;
 if (rxp->cstate != CHAN_RUNNING) return 1;
 if (fn_smdc_rx_status(rxp->chan, &p1, &p2, &p3)) return 1;
 printf("Rxs: %d %d %d\n", p1, p2, p3);
 return 0;
}

struct chanconf {
	struct proto *proto;
	MVIP ints;
	MVIP outts;
	long speed;
	int capacity;
	unsigned crc;
	unsigned crcsize;
	int check;
	unsigned long maxrx;
	char *pfx;
	unsigned psiz;
	unsigned sync;
	int startsync;
	tSMSynchroniser synchroniser;
};

#include "hdlctest.i"
#include "rawtest.i"
#include "rlptest.i"
#include "v110test.i"

static struct proto {
	const char *name;
	int (*conf)(struct txparam *txp, struct rxparam *rxp, struct chanconf *confp, SM_CHANNEL_ALLOC_PLACED_PARMS *sa);
	const char *(*txstart)(struct txparam *txp);
	const char *(*rxstart)(struct rxparam *rxp);
	int (*txupd)(struct txparam *txp);
	int (*rxupd)(struct rxparam *rxp, unsigned char *buff, unsigned nc);
	int (*txsts)(struct txparam *txp);
	int (*rxsts)(struct rxparam *rxp);
	void (*rxfini)(struct rxparam *rxp);
	void (*txfini)(struct txparam *txp);
} proto[] = {
	{ "v110", v110_conf, v110tx_start, v110rx_start, v110tx_upd, v110rx_upd, v110tx_sts, v110rx_sts, v110rx_fini, v110tx_fini, },
	{ "rlp", rlp_conf, rlptx_start, rlprx_start, rlptx_upd, rlprx_upd, rlptx_sts, rlprx_sts, rlprx_fini, rlptx_fini, },
	{ "raw", raw_conf, rawtx_start, rawrx_start, rawtx_upd, rawrx_upd, rawtx_sts, rawrx_sts, rawrx_fini, rawtx_fini, },
	{ "hdlc", hdlc_conf, hdlctx_start, hdlcrx_start, hdlctx_upd, hdlcrx_upd, hdlctx_sts, hdlcrx_sts, hdlcrx_fini, hdlctx_fini, },
};


	// test code

static const char *inittx(struct txparam *txp)
{
 *txp->endmsg = 0;
 txp->txbusy = 0;
 txp->tstate = TX_PAT;
 txp->cstate = CHAN_RUNNING;
 txp->txlen = 0;
 txp->txpat = 0;
 txp->ppos = 0;
 return txp->txproto->txstart(txp);
}

static int txsts(struct txparam *txp)
{
 if (txp->cstate != CHAN_RUNNING) return 1;
 return txp->txproto->txsts(txp);
}

static int updatetx(struct txparam *txp)
{
 if (txp->cstate != CHAN_RUNNING) return 1;
 switch (txp->tstate) {
 case TX_UNDERRUN:
	txp->txbusy = 'U';
 	goto nosend;
 case TX_PAT:
	txp->txbusy = 'T';
	break;
 case TX_ZERO:
	txp->txbusy = '0';
	break;
 case TX_FF:
	txp->txbusy = 'F';
	break;
 }
 if (!txp->txlen) ftime(&txp->txstart);
 if (txp->txproto->txupd(txp)) return 1;
nosend:
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
 txp->txproto->txfini(txp);
 return 0;
}

static const char *initrx(struct rxparam *rxp)
{
 *rxp->endmsg = 0;
 rxp->rxdata = 0;
 rxp->rxcnt = 0;
 rxp->rxpos = 0;
 rxp->rxdat = 0;
 rxp->rxpat = 0;
 rxp->rxbusy = 0;
 rxp->hadok = 0;
 rxp->cstate = CHAN_RUNNING;
 return rxp->rxproto->rxstart(rxp);
}

static int rxsts(struct rxparam *rxp)
{
 if (rxp->cstate != CHAN_RUNNING) return 1;
 return rxp->rxproto->rxsts(rxp);
}

static int updaterx(struct rxparam *rxp)
{
 unsigned char buff[1024];
 int nc;
 if (rxp->cstate != CHAN_RUNNING) return 1;
 if (fn_smdc_rx_data(&nc, rxp->chan, buff, sizeof(buff))) return 1;
 if (!rxp->rxbusy) rxp->rxbusy = 'R';
 ftime(&rxp->rxlast);
 if (!rxp->rxdata) rxp->rxstart = rxp->rxlast;
 if (rxp->rxproto->rxupd(rxp, buff, nc)) return 1;
 rxp->rxdata += nc;
 rxp->rxcnt += nc;
 if (rxp->rxfile) {
	if (fwrite(buff, 1, nc, rxp->rxfile) != (unsigned) nc) {
		perror("fwrite failed");
		return 1;
	}
	fflush(rxp->rxfile);
 }
 if (rxp->maxrx && rxp->rxcnt >= rxp->maxrx) {
	sprintf(rxp->endmsg, "[limit] ");
	return 1;
 }
 return 0;
}

static void finishrx(struct rxparam *rxp)
{
 if (rxp->rxfile) fclose(rxp->rxfile);
 rxp->rxproto->rxfini(rxp);
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
		if (txp[chan].txbusy) *cp++ = txp[chan].txbusy;
		else *cp++ = '.';
	}
	if (txp[chan].txbusy != 'U') txp[chan].txbusy = 0;
	switch (rxp[chan].cstate) {
	case CHAN_DEAD: *cp++ = '_'; break;
	case CHAN_STOPPING: *cp++ = 'x'; break;
	case CHAN_RUNNING:
		if (rxp[chan].rxbusy) *cp++ = rxp[chan].rxbusy;
		else *cp++ = '.';
	}
	*cp++ = ' ';
	rxp[chan].rxbusy = 0;
 }
 *cp++ = twiddle[twidpos];
 if (++twidpos == arlen(twiddle)-1) twidpos = 0;
 *cp = 0;
 fputs(disp, stdout);
 free(disp);
}


	// initialisation
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
 case 'd':
	for (chan=0; chan < numchan; chan++)
		fn_smdc_rx_control(rxp[chan].chan, kSMDCRxCtlDiscard, 0, 0);
	break;
 case 'R':
	for (chan=0; chan < numchan; chan++)
		fn_smdc_line_control(txp[chan].chan, kSMDCLineCtlCmdUnassertRTS, 0, 0);
	break;
 case 'r':
	for (chan=0; chan < numchan; chan++)
		fn_smdc_line_control(txp[chan].chan, kSMDCLineCtlCmdAssertRTS, 0, 0);
	break;
 case 'f':
	for (chan=0; chan < numchan; chan++) {
		txp[chan].tstate = TX_FF;
#ifdef USE_POLL
		waitp[chan*2+1].events = POLLOUT|POLLPRI;
#endif
	}
	break;
 case '0':
	for (chan=0; chan < numchan; chan++) {
		txp[chan].tstate = TX_ZERO;
#ifdef USE_POLL
		waitp[chan*2+1].events = POLLOUT|POLLPRI;
#endif
	}
	break;
 case 'U':
	for (chan=0; chan < numchan; chan++) {
		txp[chan].tstate = TX_PAT;
#ifdef USE_POLL
		waitp[chan*2+1].events = POLLOUT|POLLPRI;
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
#ifdef USE_POLL
 // FIXME: handle POLLHUP (but not needed for current protocols)
 if (!(pfd->revents & POLLPRI) || !rxsts(rxp)) {
	// no status change, or it was handled ok
	// return if no input or it had no error
	if (rxp->cstate == CHAN_RUNNING
		&& (! (pfd->revents & POLLIN)
			|| !updaterx(rxp))) return 0;
 }
#endif
#ifdef TiNGTYPE_WINNT
 if (!updaterx(rxp)) return 0;
#endif
 if (rxp->cstate != CHAN_DEAD) {
	SMDC_LINE_STATUS_PARMS lp;
	memset(&lp, 0, sizeof(lp));
	lp.channel = rxp->chan;
	if (!smdc_line_status(&lp)) {
		if (lp.rx_status == kSMDCRxStatusNoCarrier) rxp->rxbusy = 's';
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
 return 0;
}

static int handle_tx(WAITABLE *pfd, struct txparam *txp, struct rxparam *rxp)
{
#ifdef USE_POLL
 // FIXME: handle POLLHUP (but not needed for current protocols)
 if (!(pfd->revents & POLLPRI) || !txsts(txp)) {
	// no status change, or it was handled ok
	// return if no output or it had no error
	if (txp->cstate == CHAN_RUNNING
		&& (! (pfd->revents & POLLOUT)
			|| !updatetx(txp))) return 0;
 }
#endif
#ifdef TiNGTYPE_WINNT
 if (!updatetx(txp)) return 0;
#endif
 if (txp->tstate == TX_UNDERRUN) {
#ifdef USE_POLL
	pfd->events = POLLPRI;
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
 return 0;
}

static int startchan(struct txparam *txp, struct rxparam *rxp, SM_CHANNEL_ALLOC_PLACED_PARMS *sa, struct chanconf *confp, WAITABLE *waitp)
{
 SM_CHANNEL_SET_EVENT_PARMS sep;
 SM_SWITCH_CHANNEL_PARMS swp;
 int r;
 const char *s;
 txp->txproto = confp->proto;
 rxp->rxproto = confp->proto;
 if (txp->txproto->conf(txp, rxp, confp, sa)) return 1;
 	// set up rx
 if (confp->startsync) {
	SM_SYNC_ADD_PARMS sp;
	int sts;
	memset(&sp, 0, sizeof(sp));
	sp.synchroniser = confp->synchroniser;
	sp.channel = rxp->chan;
	sts = sm_sync_add(&sp);
	if (sts) {
		printerr_sm("sm_sync_add (rx)", sts);
		return 1;
	}
 }
 memset(&swp, 0, sizeof(swp));
 swp.channel = rxp->chan;
 swp.st = confp->ints.stream;
 swp.ts = confp->ints.timeslot;
 swp.type = confp->ints.type;
 if (++confp->ints.timeslot >= 32) {
	confp->ints.timeslot = 0;
	confp->ints.stream++;
 }
 r = sm_switch_channel_input(&swp);
 if (r) return printerr_sm("sm_switch_channel_input", r);
 rxp->check = confp->check;
 if (confp->pfx) {
	char *rxfname = malloc(strlen(confp->pfx)+100);
	if (!rxfname) {
		perror("malloc failed");
		return 1;
	}
	sprintf(rxfname, confp->pfx, confp->ints.stream, confp->ints.timeslot);
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
 if (confp->startsync) {
	SM_SYNC_ADD_PARMS sp;
	int sts;
	memset(&sp, 0, sizeof(sp));
	sp.synchroniser = confp->synchroniser;
	sp.channel = txp->chan;
	sts = sm_sync_add(&sp);
	if (sts) {
		printerr_sm("sm_sync_add (tx)", sts);
		return 1;
	}
 }
 swp.channel = txp->chan;
 swp.st = confp->outts.stream;
 swp.ts = confp->outts.timeslot;
 swp.type = confp->outts.type;
 if (++confp->outts.timeslot >= 32) {
	confp->outts.timeslot = 0;
	confp->outts.stream++;
 }
 r = sm_switch_channel_output(&swp);
 if (r) return printerr_sm("sm_switch_channel_output", r);
 txp->incr = rxp->incr = confp->outts.timeslot+1;
 txp->psiz = rxp->psiz = confp->psiz;
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
 if (confp->capacity) {
	if (fn_smdc_tx_control(txp->chan, kSMDCTxCtlNotifyOnCapacity, confp->capacity)) return 1;
 }
 if ( (s = initrx(rxp)) ) {
	fprintf(stderr, "cannot initialise rx: %s\n", s);
	return 1;
 }
 if ( (s = inittx(txp)) ) {
	fprintf(stderr, "cannot initialise tx: %s\n", s);
	return 1;
 }
 return 0;
}

	// starting everything up and shutting it down

static int test(tSMModuleId mod, unsigned numchan, struct chanconf *confp)
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
 // txp[X] is fd 2*X
 // rxp[X] is fd 2*X+1
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
 if (confp->startsync) {
	SM_SYNC_CREATE_PARMS sp;
	int sts;
	memset(&sp, 0, sizeof(sp));
	sp.module = mod;
	sts = sm_sync_create(&sp);
	if (sts) {
		printerr_sm("sm_sync_create", sts);
		return 1;
	}
	confp->synchroniser = sp.synchroniser;
 }
 	// initialisation needed by data transfer threads
 memset(&sa, 0, sizeof(sa));
 sa.module = mod;
 for (chan=0; chan < numchan; chan++) {
	int sts;
	sts = startchan(
		txp+chan,
		rxp+chan,
		&sa,
		confp,
		waitp+chan*2);
	if (sts) return 1;
 }
#ifdef USE_POLL
 waitp[nwaitp].events = POLLIN | POLLHUP;
 waitp[nwaitp].fd = 0;
#endif
 if (confp->startsync) {
	SM_SYNC_DELETE_PARMS sp;
	int sts;
	printf("ready...\n");
	while (getchar() != '\n');
	memset(&sp, 0, sizeof(sp));
	sp.synchroniser = confp->synchroniser;
	sts = sm_sync_delete(&sp);
	if (sts) {
		printerr_sm("sm_sync_delete", sts);
		return 1;
	}
 }
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
		WAITABLE *wp = waitp;
		unsigned j;
		for (j=0; j*2 < nwaitp; j++) {
			if (wp->revents && handle_rx(wp, rxp+j, txp+j))
				return 1;
			wp++;
			if (wp->revents && handle_tx(wp, txp+j, rxp+j))
				return 1;
			wp++;
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
	 	switch (txp[j].cstate) {
		case CHAN_STOPPING:
			if (handle_tx(&waitp[k], txp+j, rxp+j)) return 1;
			break;
	 	case CHAN_RUNNING:
	 		if (txp[j].tstate != TX_UNDERRUN) {
				waitp[k] = txp[j].event;
				indx[k] = j*2+1;
				k++;
			}
			break;
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
	 k = WaitForMultipleObjects(k+1, waitp, 0, i);
	 if (k == WAIT_FAILED) {
		lasterr("WaitForMultipleObjects failed");
		break;
	 }
	 if (k >= WAIT_OBJECT_0 && k <= WAIT_OBJECT_0+nwaitp) {
		k = indx[k-WAIT_OBJECT_0];
		j = k/2;
		if (k == nwaitp) {
			if (handle_kbd(waitp, nwaitp, txp, rxp, numchan)) break;
		} else if (k & 1) {
			if (handle_tx(&waitp[k], txp+j, rxp+j)) return 1;
		} else {
			if (handle_rx(&waitp[k], rxp+j, txp+j)) return 1;
		}
	 } else if (k != WAIT_TIMEOUT) {
		fprintf(stderr, "WaitForMultipleObjects returned 0x%x\n", k);
	 }
	}
#endif
 }
 free(rxp);
 free(txp);
 free(waitp);
 return 0;
}

#include "gen/polltest.args.i"

/* determine CRC size automatically. Fails for 32-bit CRC unless on
 * a 64-bit system because the polynomial would need to be 33 bits.
 * If a size is given explicitly, a 32-bit CRC can be used because the
 * missing 33rd bit is assumed to be '1'.
 */
static unsigned auto_crc_size(unsigned crc)
{
 unsigned i;
 if (!crc) return 0;
 for (i=0; crc; crc >>= 1) i++;
 return i - 1;
}

int main(int argc, char **argv)
{
 struct chanconf conf;
 tSMModuleId mod;
 tSMCardId card;
 unsigned i;
 int iErr;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || *argv) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
	fprintf(stderr, "Protocols:");
	for (i=0; i < arlen(proto); i++) {
		fprintf(stderr, " %s", proto[i].name);
	}
	fprintf(stderr, "\n");
	return 1;
 }
 if (!arg.protocol) {
 	fprintf(stderr, "No protocol specified\n");
 	return 1;
 }
 for (i=0; ; i++) {
	if (i >= arlen(proto)) {
		fprintf(stderr, "Unknown protocol: %s\n", arg.protocol);
		return 1;
	}
	if (!strcmp(arg.protocol, proto[i].name)) break;
 }
 conf.proto = &proto[i];
 conf.ints = arg.intimeslot;
 conf.outts = arg.outtimeslot;
 conf.speed = arg.speed;
 conf.maxrx = arg.maxrx;
 conf.pfx = arg.fileprefix;
 conf.check = arg.check;
 conf.psiz = arg.patsize;
 conf.sync = arg.sync;
 conf.startsync = arg.syncstart;
 conf.crc = arg.crc;
 conf.capacity = arg.txcapacity;
 if (arg.crcsize) conf.crcsize = arg.crcsize;
 else conf.crcsize = auto_crc_size(conf.crc);
 if (!arg.numchan) arg.numchan = 1;
 if (arg.serialnumber) {
	err_t e = modopen(&card, &mod, arg.serialnumber, arg.module);
	if (e) {
		error_log(stderr, e);
		return 1;
	}
 } else {
	fprintf(stderr, "Serial number is required\n Usage %s" ARGS_USAGE "\n",progname);
	return 1;
 }
 iErr = test(mod, arg.numchan, &conf);
 if (arg.serialnumber) {
	modclose(mod);
	cardclose(card);
 }
 return iErr;
}
