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
#include "smdc_raw.h"
#include "smdc_sync.h"

#include <malloc.h>

#include "../Testlib/cardopen.h"
#include "../Testlib/errcode_sm.h"
#include "../Testlib/error.h"
#include "../Testlib/modopen.h"
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

static int reperrno(const char *op, int errnum)
{
 if (errnum) fprintf(stderr, "%s failed: %s\n", op, strerror(errnum));
 return errnum;
}

	// convenient encapsulations of Prosody API functions

static int chan_config_raw(tSMChannelId chan)
{
 SMDC_CHANNEL_CONFIG_PARMS sc;
 int r;
 memset(&sc, 0, sizeof(sc));
 sc.channel = chan;
 sc.protocol = kSMDCProtocolRawRx;
 sc.encoding = kSMDCConfigEncodingSync;
 sc.config_length = 0;
 sc.config_data = 0;
 r = smdc_channel_config(&sc);
 if (r) return printerr_sm("smdc_channel_config", r);
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

static void exitting(struct waitthread *wt, pthread_t tidp)
{
 reperrno("pthread_mutex_lock(&wt->mx)", pthread_mutex_lock(&wt->mx));
 while (wt->havecorpse) {
	reperrno("pthread_cond_wait(&wt->reaperready_cv, &wt->mx)",
		pthread_cond_wait(&wt->reaperready_cv, &wt->mx));
 }
 wt->corpse = tidp;
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

struct rxparam {
	pthread_t tid;
	pthread_mutex_t mx;
	enum cstate {CHAN_DEAD, CHAN_STOPPING, CHAN_RUNNING} cstate;
	int busy;
	FILE *rxfile;		// unprotected - private after thread start
	tSMChannelId chan;	// unprotected - globally readonly
	struct waitthread *wt;	// unprotected - globally readonly
	unsigned long maxrx;	// unprotected - globally readonly
	char endmsg[256];	// only valid after thread temination
};

static void *runrxtest(void *p)
{
 struct rxparam *rxp = p;
 struct chaninfo {
	struct timeb rxstart;
	unsigned long rxcnt;
	int sts_link, sts_linkrx, sts_linkcts;
	int sts_rxflow;
	unsigned char rxpat;
 } rxchan, *tc = &rxchan;
 unsigned long totrx = 0;
 struct timeb now;
 tc->rxcnt=0;
 tc->sts_link=0xaaaa;
 tc->sts_linkrx=0xaaaa;
 tc->sts_linkcts=0xaaaa;
 tc->sts_rxflow=0xaaaa;
 if (chan_rx_control(rxp->chan, kSMDCRxCtlNotifyOnData, 1024, 500)) {
	exitting(rxp->wt, rxp->tid);
	return "control failed";
 }
 *rxp->endmsg = 0;
 for (;;) {
	unsigned char buff[8192];
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
	if (!tc->rxcnt) ftime(&tc->rxstart);
	if (0 && p0 != sizeof(buff)) {
		fprintf(stderr, "Partial read: %d of %ld\n",
			p0, (long) sizeof(buff));
		break;
	}
	if (rxp->rxfile) {
		if (fwrite(buff, 1, p0, rxp->rxfile) != (unsigned) p0) {
			perror("fwrite failed");
			exitting(rxp->wt, rxp->tid);
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
 ftime(&now);
 if (rxp->rxfile) fclose(rxp->rxfile);
 if (tc->rxcnt) {
	double rxtim;
	rxtim = now.millitm - tc->rxstart.millitm;
	rxtim /= 1000.0;
	rxtim += now.time - tc->rxstart.time;
	if (!rxtim) rxtim = 0.001;
	sprintf(endstr(rxp->endmsg), "Got %ld bytes in %g S = %g bytes/sec (%g bps)",
		tc->rxcnt,
		rxtim,
		tc->rxcnt / rxtim, 8.0 * tc->rxcnt / rxtim);
	totrx += tc->rxcnt;
 } else sprintf(endstr(rxp->endmsg), "Got 0 bytes");
 exitting(rxp->wt, rxp->tid);
 return 0;
}

	// initialisation

static int startchan(struct waitthread *wt, struct rxparam *rxp, SM_CHANNEL_ALLOC_PLACED_PARMS *sa, MVIP *ints, unsigned long maxrx, char *pfx, pthread_t *tidp)
{
 SM_SWITCH_CHANNEL_PARMS swp;
 int r;
 r = sm_channel_alloc_placed(sa);
 if (r) return printerr_sm("sm_channel_alloc_placed", r);
 memset(&swp, 0, sizeof(swp));
 swp.channel = rxp->chan = sa->channel;
 if (pfx) {
	char *rxfname = malloc(strlen(pfx)+sizeof("-1234567890-1234567890"));
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
 if (chan_config_raw(rxp->chan)) return 1;
 swp.st = ints->stream;
 swp.ts = ints->timeslot;
 swp.type = ints->type;
 if (++ints->timeslot >= 32) {
	ints->timeslot = 0;
	ints->stream++;
 }
 r = sm_switch_channel_input(&swp);
 if (r) return printerr_sm("sm_switch_channel_input", r);
 rxp->wt = wt;
 rxp->maxrx = maxrx;
 rxp->busy = 0;
 rxp->cstate = CHAN_RUNNING;
 if (startthread(tidp, wt, runrxtest, rxp)) {
	fprintf(stderr, "cannot start rx thread\n");
	return 1;
 }
 rxp->tid = *tidp;
 return 0;
}

	// periodic status display

static void showsts(struct rxparam *rxp, int numchan)
{
 static char twiddle[] = "-\\|/";
 static int twidpos;
 char *disp = malloc(numchan*3+sizeof(" \r"));
 char *cp = disp;
 int chan;
 if (!disp) return;
 *cp++ = '\r';
 for (chan=0; chan<numchan; chan++) {
	reperrno("pthread_mutex_lock(&rxp[chan].mx)",
		pthread_mutex_lock(&rxp[chan].mx));
	switch (rxp[chan].cstate) {
	case CHAN_DEAD: *cp++ = '_'; break;
	case CHAN_STOPPING: *cp++ = 'x'; break;
	case CHAN_RUNNING:
		if (rxp[chan].busy) *cp++ = 'R';
		else *cp++ = '.';
	}
	rxp[chan].busy = 0;
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
	pthread_t tid;
	struct waitthread *wt;
	unsigned maxchan;
	struct rxparam *rxp;
};

static void *statusthread(void *p)
{
 struct statusparam *sp = p;
 for (;;) {
	showsts(sp->rxp, sp->maxchan);
	Sleep(1000);
	if (sp->stop) break;
 }
 exitting(sp->wt, sp->tid);
 return 0;
}

	// reading keyboard commands
struct kbdparam {
	unsigned stop;
	pthread_t tid;
	struct waitthread *wt;
	unsigned wantchan, maxchan;
	struct rxparam *rxp;
};

static void *kbdthread(void *p)
{
 struct kbdparam *kp = p;
 unsigned chan;
 while (!kp->stop) {
	switch (getchar()) {
	case INKEY_NONE: continue;
	case '-':
		kp->wantchan--;
		for (chan=0; chan < kp->maxchan; chan++) {
			reperrno("pthread_mutex_lock(&kp->rxp[chan].mx)",
				pthread_mutex_lock(&kp->rxp[chan].mx));
			if (kp->rxp[chan].cstate == CHAN_RUNNING) {
				kp->rxp[chan].cstate = CHAN_STOPPING;
				reperrno("pthread_mutex_unlock(&kp->rxp[chan].mx)",
					pthread_mutex_unlock(&kp->rxp[chan].mx));
				printf("Stopping %d \n", chan);
				break;
			}
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
 exitting(kp->wt, kp->tid);
 return 0;
}

	// starting everything up and shutting it down

static int recraw(tSMModuleId module, unsigned long maxrx, MVIP *ints, char *pfx, unsigned numchan)
{
 unsigned ntids = numchan+2;	// N*rx+kbd+sts
 pthread_t *tids = (pthread_t *) malloc(ntids * sizeof(*tids));
 SM_CHANNEL_ALLOC_PLACED_PARMS sa;
 struct statusparam sp;
 struct waitthread wt;
 struct kbdparam kp;
 unsigned chan;
 int r = 0;
 	// general initialisation
 wait_init(&wt);
 // sp.rxp[X] is thread X
 if (!tids) {
	fprintf(stderr, "Cannot allocate memory for thread IDs\n");
	return 1;
 }
 	// initialisation needed by status thread
 sp.stop = 0;
 sp.wt = &wt;
 sp.maxchan = numchan;
 sp.rxp = (struct rxparam *) malloc(numchan * sizeof(*sp.rxp));
 if (!sp.rxp) {
	fprintf(stderr, "Cannot allocate memory for rx channel params\n");
	return 1;
 }
 if (startthread(tids+numchan, &wt, statusthread, &sp)) {
 	fprintf(stderr, "cannot start status thread\n");
 	return 1;
 }
 sp.tid = tids[numchan];
 	// initialisation needed by keyboard thread
 kp.stop = 0;
 kp.wt = &wt;
 kp.wantchan = numchan;
 kp.maxchan = numchan;
 kp.rxp = sp.rxp;
 for (chan=0; chan<numchan; chan++) {
	reperrno("pthread_mutex_init(&sp.rxp[chan].mx, 0)",
		pthread_mutex_init(&sp.rxp[chan].mx, 0));
 }
 if (startthread(tids+numchan+1, &wt, kbdthread, &kp)) {
 	fprintf(stderr, "cannot start keyboard thread\n");
 	return 1;
 }
 kp.tid = tids[numchan+1];
 	// initialisation needed by data transfer threads
 memset(&sa, 0, sizeof(sa));
 sa.type = kSMChannelTypeInput;
 sa.module = module;
 for (chan=0; chan < numchan; chan++) {
	int sts;
	reperrno("pthread_mutex_lock(&sp.rxp[chan].mx)",
		pthread_mutex_lock(&sp.rxp[chan].mx));
	sts = startchan(&wt,
		sp.rxp+chan,
		&sa,
		ints,
		maxrx,
		pfx,
		tids+chan);
	if (sts) return 1;
	reperrno("pthread_mutex_unlock(&sp.rxp[chan].mx)",
		pthread_mutex_unlock(&sp.rxp[chan].mx));
 }
 for (; wt.numthreads;) {
 	void *status;
 	pthread_t tid;
 	int sts = waitany(&tid, &status, &wt);
	chan = numchan+1;
	if (sts) {
		fprintf(stderr, "error waiting for thread: %s\n",
			strerror(sts));
		r = 1;
		break;
	}
		// a thread has died
	if (pthread_equal(tid, tids[chan])) {		// it's kbd
		printf("kbd quit - waiting for statistics\n");
	} else if (pthread_equal(tid, tids[--chan])) {	// it's status
		if (wt.numthreads) {
			printf("!?? status thread died with %d threads running\n", wt.numthreads);
		}
	} else while (chan--) {
		if (pthread_equal(tid, tids[chan])) {	// rx
			if (status) printf("%dr: %s\n", chan, (char *) status);
			printf("%dr: %s\n", chan, sp.rxp[chan].endmsg);
			reperrno("pthread_mutex_lock(&sp.rxp[chan].mx)",
				pthread_mutex_lock(&sp.rxp[chan].mx));
			r = sm_channel_release(sp.rxp[chan].chan);
			if (r) return printerr_sm("sm_channel_release", r);
			sp.rxp[chan].cstate = CHAN_DEAD;
			reperrno("pthread_mutex_unlock(&sp.rxp[chan].mx)",
				pthread_mutex_unlock(&sp.rxp[chan].mx));
			break;
		}
	}
	if (wt.numthreads == 1) sp.stop = 1;
 }
 free(sp.rxp);
 free(tids);
 wait_close(&wt);
 return r;
}

#include "gen/recraw.args.i"

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
 if (arg.maxrx) arg.maxrx *= 8000;
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
 sts = recraw(module, arg.maxrx, &arg.intimeslot, arg.fileprefix, arg.numchan);
 if (arg.serialnumber) {
	modclose(module);
	cardclose(card);
 }
 return sts;
}
