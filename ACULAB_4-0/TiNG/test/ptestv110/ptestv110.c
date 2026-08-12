#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>

#include <sys/types.h>
#include <sys/timeb.h>
#include "smdrvr.h"
//#include <conio.h>
#include "smdc.h"

#include <malloc.h>

#include "../Testlib/cardopen.h"
#include "../Testlib/errcode_sm.h"
#include "../Testlib/error.h"
#include "../Testlib/modopen.h"
#include "../../libutil/generic_io.h"

#include "../../apilib/smdc_v110.h"

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

static int reperrno(const char *op, int err)
{
 if (err) fprintf(stderr, "%s failed: %s\n", op, strerror(err));
 return err;
}

	// convenient encapsulations of Prosody API functions
static int chan_config_v110(tSMChannelId chan, long speed)
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
 v110.format = kSMDCFormatAsync;
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
 dp.blocking = 1;
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
 dp.blocking = 1;
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

	// thread operations
struct waitthread {
	pthread_mutex_t mx;
	unsigned numthreads;
	int havecorpse;
	pthread_t corpse;
	pthread_cond_t threaddeath_cv;
	pthread_cond_t reaperready_cv;
};

static int wait_init(struct waitthread *wt)
{
 wt->havecorpse = 0;
 wt->numthreads = 0;
 return reperrno("pthread_mutex_init(&wt->mx, 0)",
		pthread_mutex_init(&wt->mx, 0))
	|| reperrno("pthread_cond_init(&wt->threaddeath_cv, 0)",
		pthread_cond_init(&wt->threaddeath_cv, 0))
	|| reperrno("pthread_cond_init(&wt->reaperready_cv, 0)",
		pthread_cond_init(&wt->reaperready_cv, 0));
}

static void wait_close(struct waitthread *wt)
{
 reperrno("pthread_mutex_destroy(&wt->mx)", pthread_mutex_destroy(&wt->mx));
 reperrno("pthread_cond_destroy(&wt->threaddeath_cv)",
	 pthread_cond_destroy(&wt->threaddeath_cv));
 reperrno("pthread_cond_destroy(&wt->reaperready_cv)",
	 pthread_cond_destroy(&wt->reaperready_cv));
}

static void exitting(struct waitthread *wt)
{
 reperrno("pthread_mutex_lock(&wt->mx)", pthread_mutex_lock(&wt->mx));
 while (wt->havecorpse) {
	reperrno("pthread_cond_wait(&wt->reaperready_cv, &wt->mx)",
		pthread_cond_wait(&wt->reaperready_cv, &wt->mx));
 }
 wt->corpse = pthread_self();
 wt->havecorpse = 1;
 reperrno("pthread_cond_signal(&wt->threaddeath_cv)",
	 pthread_cond_signal(&wt->threaddeath_cv));
 reperrno("pthread_mutex_unlock(&wt->mx)", pthread_mutex_unlock(&wt->mx));
}

static int waitany(pthread_t *ptt, void **status, struct waitthread *wt)
{
 reperrno("pthread_mutex_lock(&wt->mx)", pthread_mutex_lock(&wt->mx));
 if (wt->numthreads) {
	reperrno("pthread_cond_signal(&wt->reaperready_cv)",
		 pthread_cond_signal(&wt->reaperready_cv));
	while (!wt->havecorpse) {
		reperrno("pthread_cond_wait(&wt->threaddeath_cv, &wt->mx)",
			pthread_cond_wait(&wt->threaddeath_cv, &wt->mx));
	}
	*ptt = wt->corpse;
	wt->numthreads--;
	wt->havecorpse = 0;
	reperrno("pthread_mutex_unlock(&wt->mx)",
		pthread_mutex_unlock(&wt->mx));
	return reperrno("pthread_join(*ptt, status)", pthread_join(*ptt, status));
 } else {
	reperrno("pthread_mutex_unlock(&wt->mx)",
		pthread_mutex_unlock(&wt->mx));
	return 1;
 }
}

static int startthread(pthread_t *tidp, struct waitthread *wt, void *(*fn)(void *), void *param)
{
 reperrno("pthread_mutex_lock(&wt->mx)", pthread_mutex_lock(&wt->mx));
 if (!reperrno("pthread_create(tidp, 0, fn, param))",
	pthread_create(tidp, 0, fn, param)))
	++wt->numthreads;
 reperrno("pthread_mutex_unlock(&wt->mx)", pthread_mutex_unlock(&wt->mx));
 return 0;
}

	// test code
struct txparam {
	pthread_mutex_t mx;
	enum cstate {CHAN_DEAD, CHAN_STOPPING, CHAN_RUNNING} cstate;
	int busy;
	int underrun;
	int incr;		// readonly
	tSMChannelId chan;	// unprotected - globally readonly
	struct waitthread *wt;	// unprotected - globally readonly
	char endmsg[128];	// only valid after thread temination
};

static void *runtxtest(void *p)
{
 struct txparam *txp = p;
 struct chaninfo {
	struct timeb txstart;
	int sts_link, sts_linktx, sts_linkcts;
	int sts_txflow;
	int txlen;
	unsigned char txpat;
 } txchan, *tc = &txchan;
 struct timeb now;
 tc->sts_link=0xaaaa;
 tc->sts_linktx=0xaaaa;
 tc->sts_linkcts=0xaaaa;
 tc->sts_txflow=0xaaaa;
 tc->txlen=0;
 tc->txpat = 0;
 {	// allow receiver to get ready
  struct timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = 100 * 1000 * 1000;	// 0.1 second
  nanosleep(&ts, 0);
 }
 for (;;) {
	int cap, sts, flo;
	char buff[256];
	unsigned i;
	reperrno("pthread_mutex_lock(&txp->mx)", pthread_mutex_lock(&txp->mx));
	if (txp->cstate != CHAN_RUNNING) break;
	i = txp->underrun;
	if (i) txp->busy = 'U';
	reperrno("pthread_mutex_unlock(&txp->mx)",
		pthread_mutex_unlock(&txp->mx));
	if (!i) {
		//const int n = rand() & 0xff;
		const unsigned n = arlen(buff);
		for (i=0; i < n; i++) {
			buff[i] = tc->txpat;
			tc->txpat = (tc->txpat + txp->incr) & 0xff;
		}
		if (!tc->txlen) ftime(&tc->txstart);
		tc->txlen += i;
		//printf("sent %d\n", i);
		if (chan_tx_data(txp->chan, buff, i)) break;
		reperrno("pthread_mutex_lock(&txp->mx)",
			pthread_mutex_lock(&txp->mx));
		txp->busy = 'T';
		reperrno("pthread_mutex_unlock(&txp->mx)",
			pthread_mutex_unlock(&txp->mx));
		if (0) {
		 struct timespec ts;
		 ts.tv_sec = 0;
		 ts.tv_nsec = (256 - n) * 1000 * 1000;
		 nanosleep(&ts, 0);
		}
	}
	if (chan_tx_status(txp->chan, &cap, &sts, &flo)) break;
	if (sts == kSMDCTxStatusUnderrun) {
		printf("Underrun reported\n");
	}
	if (txp->busy == 'U') Sleep(1000);
 }
 reperrno("pthread_mutex_unlock(&txp->mx)", pthread_mutex_unlock(&txp->mx));	// could hold this until msg composed
 ftime(&now);
 *txp->endmsg = 0;
 if (tc->txlen) {
	double txtim;
	txtim = now.millitm - tc->txstart.millitm;
	txtim /= 1000.0;
	txtim += now.time - tc->txstart.time;
	if (!txtim) txtim = 0.001;
	sprintf(txp->endmsg, "sent %d bytes in %g S = %g bytes/sec (%g bps)",
		tc->txlen, txtim, tc->txlen / txtim, 8.0 * tc->txlen / txtim);
 }
 exitting(txp->wt);
 return 0;
}

struct rxparam {
	pthread_mutex_t mx;
	enum cstate cstate;
	int incr;		// readonly
	int busy;
	int err;
	FILE *rxfile;		// unprotected - private after thread start
	tSMChannelId chan;	// unprotected - globally readonly
	struct waitthread *wt;	// unprotected - globally readonly
	unsigned long maxrx;	// unprotected - globally readonly
	int check;		// unprotected - globally readonly
	char endmsg[256];	// only valid after thread temination
};

static void *runrxtest(void *p)
{
 struct rxparam *rxp = p;
 struct chaninfo {
	struct timeb rxstart, rxlast;
	unsigned long rxcnt;
	int sts_link, sts_linkrx, sts_linkcts;
	int sts_rxflow;
	unsigned char rxpat;
 } rxchan, *tc = &rxchan;
 unsigned long toterrs = 0, totrx = 0;
 tc->rxcnt=0;
 tc->sts_link=0xaaaa;
 tc->sts_linkrx=0xaaaa;
 tc->sts_linkcts=0xaaaa;
 tc->sts_rxflow=0xaaaa;
 tc->rxpat = 0;
 *rxp->endmsg = 0;
 for (;;) {
	unsigned char buff[1024];
	int haderr=0, lasterr=0;
	int p0;
	reperrno("pthread_mutex_lock(&rxp->mx)", pthread_mutex_lock(&rxp->mx));
	if (rxp->cstate != CHAN_RUNNING) break;
	reperrno("pthread_mutex_unlock(&rxp->mx)",
		pthread_mutex_unlock(&rxp->mx));
	if (chan_rx_data(&p0, rxp->chan, buff, sizeof(buff))) break;
	reperrno("pthread_mutex_lock(&rxp->mx)", pthread_mutex_lock(&rxp->mx));
	rxp->busy = 1;
	reperrno("pthread_mutex_unlock(&rxp->mx)",
		pthread_mutex_unlock(&rxp->mx));
	ftime(&tc->rxlast);
	if (!tc->rxcnt) tc->rxstart = tc->rxlast;
	//printf("got %d\n", p0);
	if (0 && p0 != sizeof(buff)) {
		fprintf(stderr, "Partial read: %d of %ld\n",
			p0, (long) sizeof(buff));
		break;
	}
	if (rxp->check) {
		int i;
		for (i=0; i < p0; i++) {
			if (buff[i] != tc->rxpat) {
				if (rxp->check > 1) {
					if (!haderr) printf("Error: ");
					if (lasterr+1 != i)
						printf(" @%ld:", tc->rxcnt+i);
					printf(" %02x!=%02x",
						buff[i], tc->rxpat);
				}
				tc->rxpat = buff[i];
				lasterr = i;
				toterrs++;
				haderr=1;
			}
			tc->rxpat = (tc->rxpat + rxp->incr) & 0xff;
		}
		if (haderr) {
			if (rxp->check > 1) printf("(+%d)\n", p0-lasterr);
			reperrno("pthread_mutex_lock(&rxp->mx)",
				pthread_mutex_lock(&rxp->mx));
			rxp->err = 1;
			reperrno("pthread_mutex_unlock(&rxp->mx)",
				pthread_mutex_unlock(&rxp->mx));
		}
	}
	if (rxp->rxfile) {
		if (fwrite(buff, 1, p0, rxp->rxfile) != (unsigned) p0) {
			perror("fwrite failed");
			exitting(rxp->wt);
			return "fwrite failed";
		}
		fflush(rxp->rxfile);
	}
	tc->rxcnt += p0;
	if ((unsigned) p0 > rxp->maxrx) rxp->maxrx = 0;
	else rxp->maxrx -= p0;
	if (!rxp->maxrx) {
		sprintf(rxp->endmsg, "[limit] ");
		break;
	}
 } 
 reperrno("pthread_mutex_unlock(&rxp->mx)", pthread_mutex_unlock(&rxp->mx));
 if (rxp->rxfile) fclose(rxp->rxfile);
 if (tc->rxcnt) {
	double rxtim;
	rxtim = tc->rxlast.millitm - tc->rxstart.millitm;
	rxtim /= 1000.0;
	rxtim += tc->rxlast.time - tc->rxstart.time;
	sprintf(endstr(rxp->endmsg), "Got %ld bytes in %g S",
		tc->rxcnt,
		rxtim);
	if (rxtim) sprintf(endstr(rxp->endmsg), " = %g bytes/sec (%g bps)",
		tc->rxcnt / rxtim, 8.0 * tc->rxcnt / rxtim);
	totrx += tc->rxcnt;
 } else sprintf(endstr(rxp->endmsg), "Got 0 bytes");
 if (toterrs) sprintf(endstr(rxp->endmsg), "\nError rate: 1/%g", totrx / (double) toterrs);
 exitting(rxp->wt);
 return 0;
}

	// initialisation

static int startchan(struct waitthread *wt, struct txparam *txp, struct rxparam *rxp, SM_CHANNEL_ALLOC_PLACED_PARMS *sa, MVIP *ints, MVIP *outts, long speed, int check, unsigned long maxrx, char *pfx, pthread_t *tidp)
{
 SM_SWITCH_CHANNEL_PARMS swp;
 int r;
 r = sm_channel_alloc_placed(sa);
 if (r) return printerr_sm("sm_channel_alloc_placed", r);
 memset(&swp, 0, sizeof(swp));
 swp.channel = txp->chan = rxp->chan = sa->channel;
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
 if (chan_config_v110(rxp->chan, speed)) return 1;
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
 rxp->wt = wt;
 rxp->maxrx = maxrx;
 rxp->check = check;
 txp->wt = wt;
 txp->busy = rxp->busy = rxp->err = 0;
 txp->incr = rxp->incr = outts->timeslot+1;
 txp->underrun = 0;
 rxp->cstate = CHAN_RUNNING;
 txp->cstate = CHAN_RUNNING;
 if (chan_control(txp->chan, kSMDCLineCtlCmdInitiatorConnect)) {
	return 1;
 }
 if (chan_tx_control(txp->chan, kSMDCTxCtlNotifyOnCapacity, 256)) {
	return 1;
 }
 if (chan_rx_control(rxp->chan, kSMDCRxCtlNotifyOnData, 1, 0)) {
	return 1;
 }
 if (startthread(tidp, wt, runtxtest, txp)) {
	fprintf(stderr, "cannot start tx thread\n");
	return 1;
 }
 if (startthread(tidp+1, wt, runrxtest, rxp)) {
	fprintf(stderr, "cannot start rx thread\n");
	return 1;
 }
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
	reperrno("pthread_mutex_lock(&txp[chan].mx)", pthread_mutex_lock(&txp[chan].mx));
	switch (txp[chan].cstate) {
	case CHAN_DEAD: *cp++ = '_'; break;
	case CHAN_STOPPING: *cp++ = 'x'; break;
	case CHAN_RUNNING:
		if (txp[chan].busy) *cp++ = txp[chan].busy;
		else *cp++ = '.';
	}
	if (txp[chan].busy != 'U') txp[chan].busy = 0;
	reperrno("pthread_mutex_unlock(&txp[chan].mx)",
		pthread_mutex_unlock(&txp[chan].mx));
	reperrno("pthread_mutex_lock(&rxp[chan].mx)",
		pthread_mutex_lock(&rxp[chan].mx));
	switch (rxp[chan].cstate) {
	case CHAN_DEAD: *cp++ = '_'; break;
	case CHAN_STOPPING: *cp++ = 'x'; break;
	case CHAN_RUNNING:
		if (rxp[chan].err) *cp++ = 'r';
		else if (rxp[chan].busy) *cp++ = 'R';
		else *cp++ = '.';
	}
	*cp++ = ' ';
	rxp[chan].busy = 0;
	rxp[chan].err = 0;
	reperrno("pthread_mutex_unlock(&rxp[chan].mx)",
		pthread_mutex_unlock(&rxp[chan].mx));
 }
 *cp++ = twiddle[twidpos];
 if (++twidpos == arlen(twiddle)-1) twidpos = 0;
 *cp = 0;
 fputs(disp, stdout);
 free(disp);
}

struct statusparam {
	unsigned stop;
	struct waitthread *wt;
	unsigned maxchan;
	struct txparam *txp;
	struct rxparam *rxp;
};

static void *statusthread(void *p)
{
 struct statusparam *sp = p;
 for (;;) {
	showsts(sp->txp, sp->rxp, sp->maxchan);
	Sleep(1000);
	if (sp->stop) break;
 }
 exitting(sp->wt);
 return 0;
}

	// reading keyboard commands
struct kbdparam {
	unsigned stop;
	struct waitthread *wt;
	unsigned wantchan, maxchan;
	struct txparam *txp;
	struct rxparam *rxp;
};

static void *kbdthread(void *p)
{
 struct kbdparam *kp = p;
 unsigned chan;
 while (!kp->stop) {
	switch (getchar()) {
	case INKEY_NONE: continue;
	case 'U':
		for (chan=0; chan < kp->maxchan; chan++) {
			reperrno("pthread_mutex_lock(&kp->txp[chan].mx)",
				pthread_mutex_lock(&kp->txp[chan].mx));
			kp->txp[chan].underrun = 0;
			reperrno("pthread_mutex_unlock(&kp->txp[chan].mx)",
				pthread_mutex_unlock(&kp->txp[chan].mx));
		}
		break;
	case 'u':
		for (chan=0; chan < kp->maxchan; chan++) {
			reperrno("pthread_mutex_lock(&kp->txp[chan].mx)",
				pthread_mutex_lock(&kp->txp[chan].mx));
			kp->txp[chan].underrun = 1;
			reperrno("pthread_mutex_unlock(&kp->txp[chan].mx)",
				pthread_mutex_unlock(&kp->txp[chan].mx));
		}
		break;
	case '-':
		kp->wantchan--;
		for (chan=0; chan < kp->maxchan; chan++) {
			reperrno("pthread_mutex_lock(&kp->txp[chan].mx)",
				pthread_mutex_lock(&kp->txp[chan].mx));
			reperrno("pthread_mutex_lock(&kp->rxp[chan].mx)",
				pthread_mutex_lock(&kp->rxp[chan].mx));
			if (kp->txp[chan].cstate == CHAN_RUNNING
				&& kp->rxp[chan].cstate == CHAN_RUNNING) {
				kp->rxp[chan].cstate = CHAN_STOPPING;
				reperrno("pthread_mutex_unlock(&kp->txp[chan].mx)",
					pthread_mutex_unlock(&kp->txp[chan].mx));
				reperrno("pthread_mutex_unlock(&kp->rxp[chan].mx)",
					pthread_mutex_unlock(&kp->rxp[chan].mx));
				printf("Stopping %d \n", chan);
				break;
			}
			reperrno("pthread_mutex_unlock(&kp->txp[chan].mx)",
				pthread_mutex_unlock(&kp->txp[chan].mx));
			reperrno("pthread_mutex_unlock(&kp->rxp[chan].mx)",
				pthread_mutex_unlock(&kp->rxp[chan].mx));
		}
		break;
	case 'q':
		kp->wantchan = 0;
		kp->stop = 1;
		break;
	}
 }
 for (chan=0; chan < kp->maxchan; chan++) {
	pthread_mutex_t *mx = &kp->rxp[chan].mx;
	reperrno("pthread_mutex_lock(mx)", pthread_mutex_lock(mx));
	if (kp->rxp[chan].cstate != CHAN_DEAD) {
		kp->rxp[chan].cstate = CHAN_STOPPING;
	}
	reperrno("pthread_mutex_unlock(mx)", pthread_mutex_unlock(mx));
 }
 exitting(kp->wt);
 return 0;
}

	// starting everything up and shutting it down

static int testv110(tSMModuleId module, long speed, int check, unsigned long maxrx, MVIP *ints, MVIP *outts, char *pfx, unsigned numchan)
{
 unsigned ntids = 2*numchan+2;	// N*(tx+rx)+kbd+sts
 pthread_t *tids = (pthread_t *) malloc(ntids * sizeof(*tids));
 SM_CHANNEL_ALLOC_PLACED_PARMS sa;
 struct statusparam sp;
 struct waitthread wt;
 struct kbdparam kp;
 unsigned chan;
 int r = 0;
 	// general initialisation
 wait_init(&wt);
 // sp.txp[X] is thread 2*X
 // sp.rxp[X] is thread 2*X+1
 if (!tids) {
	fprintf(stderr, "Cannot allocate memory for thread IDs\n");
	return 1;
 }
 	// initialisation needed by status thread
 sp.stop = 0;
 sp.wt = &wt;
 sp.maxchan = numchan;
 sp.txp = (struct txparam *) malloc(numchan * sizeof(*sp.txp));
 if (!sp.txp) {
	fprintf(stderr, "Cannot allocate memory for tx channel params\n");
	return 1;
 }
 sp.rxp = (struct rxparam *) malloc(numchan * sizeof(*sp.rxp));
 if (!sp.rxp) {
	fprintf(stderr, "Cannot allocate memory for rx channel params\n");
	return 1;
 }
 if (startthread(tids+numchan*2, &wt, statusthread, &sp)) {
 	fprintf(stderr, "cannot start status thread\n");
 	return 1;
 }
 	// initialisation needed by keyboard thread
 kp.stop = 0;
 kp.wt = &wt;
 kp.wantchan = numchan;
 kp.maxchan = numchan;
 kp.txp = sp.txp;
 kp.rxp = sp.rxp;
 for (chan=0; chan<numchan; chan++) {
	sp.txp[chan].cstate = CHAN_DEAD;
	sp.rxp[chan].cstate = CHAN_DEAD;
	reperrno("pthread_mutex_init(&sp.txp[chan].mx, 0)",
		pthread_mutex_init(&sp.txp[chan].mx, 0));
	reperrno("pthread_mutex_init(&sp.rxp[chan].mx, 0)",
		pthread_mutex_init(&sp.rxp[chan].mx, 0));
 }
 if (startthread(tids+numchan*2+1, &wt, kbdthread, &kp)) {
 	fprintf(stderr, "cannot start keyboard thread\n");
 	return 1;
 }
 	// initialisation needed by data transfer threads
 memset(&sa, 0, sizeof(sa));
 sa.type = kSMChannelTypeFullDuplex;
 sa.module = module;
 for (chan=0; chan < numchan; chan++) {
	int sts;
	reperrno("pthread_mutex_lock(&sp.txp[chan].mx)",
		pthread_mutex_lock(&sp.txp[chan].mx));
	reperrno("pthread_mutex_lock(&sp.rxp[chan].mx)",
		pthread_mutex_lock(&sp.rxp[chan].mx));
	sts = startchan(&wt,
		sp.txp+chan,
		sp.rxp+chan,
		&sa,
		ints,
		outts,
		speed,
		check,
		maxrx,
		pfx,
		tids+chan*2);
	//if (sts) return 1;
	reperrno("pthread_mutex_unlock(&sp.txp[chan].mx)",
		pthread_mutex_unlock(&sp.txp[chan].mx));
	reperrno("pthread_mutex_unlock(&sp.rxp[chan].mx)",
		pthread_mutex_unlock(&sp.rxp[chan].mx));
 }
 for (; wt.numthreads;) {
 	void *status;
 	pthread_t tid;
 	int sts = waitany(&tid, &status, &wt);
	unsigned c = numchan*2+1;
	if (sts) {
		fprintf(stderr, "error waiting for thread: %s\n",
			strerror(sts));
		r = 1;
		break;
	}
		// a thread has died
	if (pthread_equal(tid, tids[c])) {		// it's kbd
		printf("kbd quit - waiting for statistics\n");
	} else if (pthread_equal(tid, tids[--c])) {	// it's status
		if (wt.numthreads) {
			printf("!?? status thread died with %d threads running\n", wt.numthreads);
		}
	} else while (c--) {
		if (pthread_equal(tid, tids[c])) {	// rx - stop its tx
			c = c / 2;
			if (status) printf("%dr: %s\n", c, (char *) status);
			printf("%dr: %s\n", c, sp.rxp[c].endmsg);
			reperrno("pthread_mutex_lock(&sp.txp[chan].mx)",
				pthread_mutex_lock(&sp.txp[chan].mx));
			reperrno("pthread_mutex_lock(&sp.rxp[chan].mx)",
				pthread_mutex_lock(&sp.rxp[chan].mx));
			if (sp.txp[c].cstate == CHAN_DEAD) {
				r = sm_channel_release(sp.rxp[c].chan);
				if (r) return printerr_sm("sm_channel_release", r);
			} else sp.txp[c].cstate = CHAN_STOPPING;
			sp.rxp[c].cstate = CHAN_DEAD;
			reperrno("pthread_mutex_unlock(&sp.txp[chan].mx)",
				pthread_mutex_unlock(&sp.txp[chan].mx));
			reperrno("pthread_mutex_unlock(&sp.rxp[chan].mx)",
				pthread_mutex_unlock(&sp.rxp[chan].mx));
			break;
		} else if (pthread_equal(tid, tids[--c])) { // tx - stop its rx
			c = c / 2;
			if (status) printf("%dt: %s\n", c, (char *) status);
			printf("%dt: %s\n", c, sp.txp[c].endmsg);
			reperrno("pthread_mutex_lock(&sp.txp[chan].mx)",
				pthread_mutex_lock(&sp.txp[chan].mx));
			reperrno("pthread_mutex_lock(&sp.rxp[chan].mx)",
				pthread_mutex_lock(&sp.rxp[chan].mx));
			if (sp.rxp[c].cstate == CHAN_DEAD) {
				r = sm_channel_release(sp.txp[c].chan);
				if (r) return printerr_sm("sm_channel_release", r);
			} else sp.rxp[c].cstate = CHAN_STOPPING;
			sp.txp[c].cstate = CHAN_DEAD;
			reperrno("pthread_mutex_unlock(&sp.txp[chan].mx)",
				pthread_mutex_unlock(&sp.txp[chan].mx));
			reperrno("pthread_mutex_unlock(&sp.rxp[chan].mx)",
				pthread_mutex_unlock(&sp.rxp[chan].mx));
			break;
		}
	}
	if (wt.numthreads == 1) sp.stop = 1;
 }
 free(sp.rxp);
 free(sp.txp);
 free(tids);
 wait_close(&wt);
 return r;
}

#include "gen/ptestv110.args.i"

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
 if (!arg.speed) arg.speed = 38400;
 if (arg.maxrx) arg.maxrx *= arg.speed / 10;
 else arg.maxrx = ~0;
 if (arg.serialnumber) {
	err_t e = modopen(&card, &module, arg.serialnumber, arg.module);
	if (e) {
		error_log(stderr, e);
		return 1;
	}
 } else {
	fprintf(stderr, "Serial number is required\n Usage %s" ARGS_USAGE "\n",progname);
	return 1;
 }
 sts = testv110(module, arg.speed, arg.check, arg.maxrx, &arg.intimeslot, &arg.outtimeslot, arg.fileprefix, arg.numchan);
 if (arg.serialnumber) {
	modclose(module);
	cardclose(card);
 }
 return sts;
}
