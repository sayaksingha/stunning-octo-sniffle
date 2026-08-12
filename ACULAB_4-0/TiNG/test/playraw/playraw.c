

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
 sc.protocol = kSMDCProtocolRawTx;
 sc.encoding = kSMDCConfigEncodingSync;
 sc.config_length = 0;
 sc.config_data = 0;
 r = smdc_channel_config(&sc);
 if (r) return printerr_sm("smdc_channel_config", r);
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

struct txparam {
	pthread_mutex_t mx;
	enum cstate {CHAN_DEAD, CHAN_STOPPING, CHAN_RUNNING} cstate;
	int busy;
	FILE *txfile;		// unprotected - private after thread start
	tSMChannelId chan;	// unprotected - globally readonly
	struct waitthread *wt;	// unprotected - globally readonly
	char endmsg[256];	// only valid after thread temination
};

static void *runtxtest(void *p)
{
 struct txparam *txp = p;
 struct chaninfo {
	struct timeb txstart;
	unsigned long txlen;
 } txchan, *tc = &txchan;
 struct timeb now;
 tc->txlen=0;
 *txp->endmsg = 0;
 for (;;) {
	unsigned char buff[1024];
	int p0;
	p0 = fread(buff, 1, sizeof(buff), txp->txfile);
	reperrno("pthread_mutex_lock(&txp->mx)", pthread_mutex_lock(&txp->mx));
	if (!p0) {
		if (ferror(txp->txfile)) {
			perror("fread failed");
		}
		break;
	}
	if (txp->cstate != CHAN_RUNNING) break;
	reperrno("pthread_mutex_unlock(&txp->mx)",
		pthread_mutex_unlock(&txp->mx));
	if (chan_tx_data(txp->chan, buff, p0)) break;
	reperrno("pthread_mutex_lock(&txp->mx)", pthread_mutex_lock(&txp->mx));
	txp->busy = 1;
	reperrno("pthread_mutex_unlock(&txp->mx)",
		pthread_mutex_unlock(&txp->mx));
	if (!tc->txlen) ftime(&tc->txstart);
	tc->txlen += p0;
 } 
 reperrno("pthread_mutex_unlock(&txp->mx)", pthread_mutex_unlock(&txp->mx));
 ftime(&now);
 if (tc->txlen) {
	double txtim;
	txtim = now.millitm - tc->txstart.millitm;
	txtim /= 1000.0;
	txtim += now.time - tc->txstart.time;
	if (!txtim) txtim = 0.001;
	sprintf(txp->endmsg, "sent %ld bytes in %g S = %g bytes/sec (%g bps)",
		tc->txlen, txtim, tc->txlen / txtim, 8.0 * tc->txlen / txtim);
 }
 exitting(txp->wt);
 return 0;
}

	// initialisation

static int startchan(struct waitthread *wt, struct txparam *txp, SM_CHANNEL_ALLOC_PLACED_PARMS *sa, MVIP *outts, char *txfname, pthread_t *tidp)
{
 SM_SWITCH_CHANNEL_PARMS swp;
 int r;
 r = sm_channel_alloc_placed(sa);
 if (r) return printerr_sm("sm_channel_alloc_placed", r);
 memset(&swp, 0, sizeof(swp));
 swp.channel = txp->chan = sa->channel;
 if (txfname) {
	txp->txfile = fopen(txfname, "rb");
	if (!txp->txfile) {
		perror("Cannot open file");
		fprintf(stderr, "File: %s\n", txfname);
		return 1;
	}
 } else txp->txfile = stdin;
 if (chan_config_raw(txp->chan)) return 1;
 swp.st = outts->stream;
 swp.ts = outts->timeslot;
 swp.type = outts->type;
 if (++outts->timeslot >= 32) {
	outts->timeslot = 0;
	outts->stream++;
 }
 r = sm_switch_channel_output(&swp);
 if (r) return printerr_sm("sm_switch_channel_input", r);
 txp->wt = wt;
 txp->busy = 0;
 txp->cstate = CHAN_RUNNING;
 if (startthread(tidp, wt, runtxtest, txp)) {
	fprintf(stderr, "cannot start tx thread\n");
	return 1;
 }
 return 0;
}

	// periodic status display

static void showsts(struct txparam *txp, int numchan)
{
 static char twiddle[] = "-\\|/";
 static int twidpos;
 char *disp = malloc(numchan*3+sizeof(" \r"));
 char *cp = disp;
 int chan;
 if (!disp) return;
 *cp++ = '\r';
 for (chan=0; chan<numchan; chan++) {
	reperrno("sts:pthread_mutex_lock(&txp[chan].mx)",
		pthread_mutex_lock(&txp[chan].mx));
	switch (txp[chan].cstate) {
	case CHAN_DEAD: *cp++ = '_'; break;
	case CHAN_STOPPING: *cp++ = 'x'; break;
	case CHAN_RUNNING:
		if (txp[chan].busy) *cp++ = 'T';
		else *cp++ = '.';
	}
	txp[chan].busy = 0;
	reperrno("sts:pthread_mutex_unlock(&txp[chan].mx)",
		pthread_mutex_unlock(&txp[chan].mx));
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
};

static void *statusthread(void *p)
{
 struct statusparam *sp = p;
 for (;;) {
	showsts(sp->txp, sp->maxchan);
	Sleep(1000);
	if (sp->stop) break;
 }
 exitting(sp->wt);
 return 0;
}

	// starting everything up and shutting it down

static int playraw(tSMModuleId module, MVIP *outts, char *pfx, unsigned numchan)
{
 unsigned ntids = numchan+1;	// N*tx+sts
 pthread_t *tids = (pthread_t *) malloc(ntids * sizeof(*tids));
 SM_CHANNEL_ALLOC_PLACED_PARMS sa;
 struct statusparam sp;
 struct waitthread wt;
 unsigned chan;
 int r = 0;
 	// general initialisation
 wait_init(&wt);
 // sp.txp[X] is thread X
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
 for (chan=0; chan<numchan; chan++) {
	reperrno("pthread_mutex_init(&sp.txp[chan].mx, 0)",
		pthread_mutex_init(&sp.txp[chan].mx, 0));
 }
 if (startthread(tids+numchan, &wt, statusthread, &sp)) {
 	fprintf(stderr, "cannot start status thread\n");
 	return 1;
 }
 	// initialisation needed by data transfer threads
 memset(&sa, 0, sizeof(sa));
 sa.type = kSMChannelTypeOutput;
 sa.module = module;
 for (chan=0; chan < numchan; chan++) {
	int sts;
	reperrno("start:pthread_mutex_lock(&sp.txp[chan].mx)",
		pthread_mutex_lock(&sp.txp[chan].mx));
	sts = startchan(&wt,
		sp.txp+chan,
		&sa,
		outts,
		pfx,
		tids+chan);
	if (sts) return 1;
	reperrno("start:pthread_mutex_unlock(&sp.txp[chan].mx)",
		pthread_mutex_unlock(&sp.txp[chan].mx));
 }
 for (; wt.numthreads;) {
 	void *status;
 	pthread_t tid;
 	int sts = waitany(&tid, &status, &wt);
	chan = numchan;
	if (sts) {
		fprintf(stderr, "error waiting for thread: %s\n",
			strerror(sts));
		r = 1;
		break;
	}
		// a thread has died
	if (pthread_equal(tid, tids[chan])) {		// it's status
		if (wt.numthreads) {
			printf("!?? status thread died with %d threads running\n", wt.numthreads);
		}
	} else while (chan--) {
		if (pthread_equal(tid, tids[chan])) {	// tx
			if (status) printf("%dt: %s\n", chan, (char *) status);
			printf("%dt: %s\n", chan, sp.txp[chan].endmsg);
			reperrno("end:pthread_mutex_lock(&sp.txp[chan].mx)",
				pthread_mutex_lock(&sp.txp[chan].mx));
			r = sm_channel_release(sp.txp[chan].chan);
			if (r) return printerr_sm("sm_channel_release", r);
			sp.txp[chan].cstate = CHAN_DEAD;
			reperrno("end:pthread_mutex_unlock(&sp.txp[chan].mx)",
				pthread_mutex_unlock(&sp.txp[chan].mx));
			break;
		}
	}
	if (wt.numthreads == 1) sp.stop = 1;
 }
 free(sp.txp);
 free(tids);
 wait_close(&wt);
 return r;
}

#include "gen/playraw.args.i"

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
 sts = playraw(module, &arg.outtimeslot, arg.fileprefix, arg.numchan);
 if (arg.serialnumber) {
	modclose(module);
	cardclose(card);
 }
 return sts;
}
