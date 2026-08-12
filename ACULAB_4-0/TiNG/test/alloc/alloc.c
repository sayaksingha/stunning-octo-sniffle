/* alloc.c - test channel allocation and release */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>
#include "pthread.h"

#include "../../libutil/generic_io.h"
#include "../Testlib/cardopen.h"
#include "../Testlib/modopen.h"

#include "smdrvr.h"

#ifdef TiNGTYPE_LINUX
	// for sleep()
#include <unistd.h>
#endif

#ifdef TiNGTYPE_QNX
	// for sleep()
#include <unistd.h>
#endif


#ifdef TiNGTYPE_WINNT
#define sleep(x) Sleep((x)*1000)
#endif

#define arlen(x) (sizeof(x)/sizeof(*(x)))

static tSMEventId anyread;
static tSMEventId anywrite;
static tSMEventId anyrecog;

static int create_any(void)
{
	struct sm_channel_set_event_parms setev;
	int r;
	memset(&setev,0,sizeof(setev));
	setev.channel=kSMNullChannelId;
	setev.issue_events=kSMAnyChannelEvent;

	setev.event_type=kSMEventTypeReadData;
	r = smd_ev_create(&anyread, kSMNullChannelId, setev.event_type, setev.issue_events);
	if (r) {
		fprintf(stderr, "smd_ev_create returned %d\n",r);
		return 1;
	}
	setev.event=anyread;
	r = sm_channel_set_event(&setev);
	if (r) {
		fprintf(stderr, "sm_channel_set_event returned %d\n",r);
		return 1;
	}

	setev.event_type=kSMEventTypeWriteData;
	r = smd_ev_create(&anywrite, kSMNullChannelId, setev.event_type, setev.issue_events);
	if (r) {
		fprintf(stderr, "smd_ev_create returned %d\n",r);
		return 1;
	}
	setev.event=anywrite;
	r = sm_channel_set_event(&setev);
	if (r) {
		fprintf(stderr, "sm_channel_set_event returned %d\n",r);
		return 1;
	}

	setev.event_type=kSMEventTypeRecog;
	r = smd_ev_create(&anyrecog, kSMNullChannelId, setev.event_type, setev.issue_events);
	if (r) {
		fprintf(stderr, "smd_ev_create returned %d\n",r);
		return 1;
	}
	setev.event=anyrecog;
	r = sm_channel_set_event(&setev);
	if (r) {
		fprintf(stderr, "sm_channel_set_event returned %d\n",r);
		return 1;
	}
	return 0;
}

static int free_any(void)
{
	struct sm_channel_set_event_parms setev;
	int r;
	memset(&setev,0,sizeof(setev));
	setev.channel=kSMNullChannelId;
	setev.issue_events=kSMAnyChannelEvent;

	setev.event_type=kSMEventTypeReadData;
	setev.event=anyread;
	r = sm_channel_set_event(&setev);
	if (r) {
		fprintf(stderr, "sm_channel_set_event returned %d\n",r);
		return 1;
	}

	setev.event_type=kSMEventTypeWriteData;
	setev.event=anywrite;
	r = sm_channel_set_event(&setev);
	if (r) {
		fprintf(stderr, "sm_channel_set_event returned %d\n",r);
		return 1;
	}

	setev.event_type=kSMEventTypeRecog;
	setev.event=anyrecog;
	r = sm_channel_set_event(&setev);
	if (r) {
		fprintf(stderr, "sm_channel_set_event returned %d\n",r);
		return 1;
	}

	r = smd_ev_free(anyread);
	if (r) {
		fprintf(stderr, "smd_ev_free returned %d\n",r);
		return 1;
	}
	r = smd_ev_free(anywrite);
	if (r) {
		fprintf(stderr, "smd_ev_free returned %d\n",r);
		return 1;
	}
	r = smd_ev_free(anyrecog);
	if (r) {
		fprintf(stderr, "smd_ev_free returned %d\n",r);
		return 1;
	}

	return 0;
}

static int alloc_events_any(tSMChannelId chan)
{
	struct sm_channel_set_event_parms setev;
	int r;
	memset(&setev,0,sizeof(setev));
	setev.channel=kSMNullChannelId;
	setev.issue_events=kSMAnyChannelEvent;

	setev.event_type=kSMEventTypeReadData;
	setev.channel = chan;
	r = sm_channel_set_event(&setev);
	if (r) {
		fprintf(stderr, "sm_channel_set_event returned %d\n",r);
		return 1;
	}

	setev.event_type=kSMEventTypeWriteData;
	r = sm_channel_set_event(&setev);
	if (r) {
		fprintf(stderr, "sm_channel_set_event returned %d\n",r);
		return 1;
	}

	setev.event_type=kSMEventTypeRecog;
	r = sm_channel_set_event(&setev);
	if (r) {
		fprintf(stderr, "sm_channel_set_event returned %d\n",r);
		return 1;
	}
	return r;
}

static int free_events_any(tSMChannelId chan)
{
	int r = 0;
	// clear events from channel
	struct sm_channel_set_event_parms setev;
	memset(&setev,0,sizeof(setev));
	setev.channel=chan;
	setev.issue_events=kSMChannelNoEvent;

	setev.event_type=kSMEventTypeReadData;
	r = sm_channel_set_event(&setev);
	if (r) {
		fprintf(stderr, "sm_channel_set_event returned %d\n",r);
		return 1;
	}

	setev.event_type=kSMEventTypeWriteData;
	r = sm_channel_set_event(&setev);
	if (r) {
		fprintf(stderr, "sm_channel_set_event returned %d\n",r);
		return 1;
	}

	setev.event_type=kSMEventTypeRecog;
	r = sm_channel_set_event(&setev);
	if (r) {
		fprintf(stderr, "sm_channel_set_event returned %d\n",r);
		return 1;
	}
	return 0;
}

static int alloc_events(tSMChannelId chan, tSMEventId* ev_read, tSMEventId* ev_write, tSMEventId* ev_recog)
{
	struct sm_channel_set_event_parms setev;
	int r;
	memset(&setev,0,sizeof(setev));
	setev.channel=chan;
	setev.issue_events=kSMChannelSpecificEvent;

	setev.event_type=kSMEventTypeReadData;
	r = smd_ev_create(ev_read, chan, setev.event_type, setev.issue_events);
	if (r) {
		fprintf(stderr, "smd_ev_create returned %d\n",r);
		return 1;
	}
	setev.event=*ev_read;
	r = sm_channel_set_event(&setev);
	if (r) {
		fprintf(stderr, "sm_channel_set_event returned %d\n",r);
		return 1;
	}

	setev.event_type=kSMEventTypeWriteData;
	r = smd_ev_create(ev_write, chan, setev.event_type, setev.issue_events);
	if (r) {
		fprintf(stderr, "smd_ev_create returned %d\n",r);
		return 1;
	}
	setev.event=*ev_write;
	r = sm_channel_set_event(&setev);
	if (r) {
		fprintf(stderr, "sm_channel_set_event returned %d\n",r);
		return 1;
	}

	setev.event_type=kSMEventTypeRecog;
	r = smd_ev_create(ev_recog, chan, setev.event_type, setev.issue_events);
	if (r) {
		fprintf(stderr, "smd_ev_create returned %d\n",r);
		return 1;
	}
	setev.event=*ev_recog;
	r = sm_channel_set_event(&setev);
	if (r) {
		fprintf(stderr, "sm_channel_set_event returned %d\n",r);
		return 1;
	}
	return r;
}

static int free_events(tSMChannelId chan, tSMEventId ev_read, tSMEventId ev_write, tSMEventId ev_recog)
{
	int r = 0;
	// clear events from channel
	struct sm_channel_set_event_parms setev;
	memset(&setev,0,sizeof(setev));
	setev.channel=chan;
	setev.issue_events=kSMChannelNoEvent;

	setev.event_type=kSMEventTypeReadData;
	r = sm_channel_set_event(&setev);
	if (r) {
		fprintf(stderr, "sm_channel_set_event returned %d\n",r);
		return 1;
	}

	setev.event_type=kSMEventTypeWriteData;
	r = sm_channel_set_event(&setev);
	if (r) {
		fprintf(stderr, "sm_channel_set_event returned %d\n",r);
		return 1;
	}

	setev.event_type=kSMEventTypeRecog;
	r = sm_channel_set_event(&setev);
	if (r) {
		fprintf(stderr, "sm_channel_set_event returned %d\n",r);
		return 1;
	}

	r = smd_ev_free(ev_read);
	if (r) {
		fprintf(stderr, "smd_ev_free returned %d\n",r);
		return 1;
	}
	r = smd_ev_free(ev_write);
	if (r) {
		fprintf(stderr, "smd_ev_free returned %d\n",r);
		return 1;
	}
	r = smd_ev_free(ev_recog);
	if (r) {
		fprintf(stderr, "smd_ev_free returned %d\n",r);
		return 1;
	}
	return r;
}

static int talloc(unsigned nchan, unsigned maxbatch, tSMModuleId module, int allocevents, int timeslot)
{
 SM_CHANNEL_ALLOC_PLACED_PARMS sap;
 SM_SWITCH_CHANNEL_PARMS scp;
 tSMChannelId *chan;
 struct timeb laststats;
 struct {
	long remms;
	unsigned long avems;
	unsigned long maxms;
	unsigned long n;
 } as, fs;
 tSMEventId *ev_read = 0, *ev_write = 0, *ev_recog = 0;
 ev_read = ev_write = ev_recog = NULL;
 as.remms = as.avems = as.maxms = as.n = 0;
 fs.remms = fs.avems = fs.maxms = fs.n = 0;
 chan = malloc(nchan * sizeof(*chan));
 if (!chan) {
	perror("malloc() failed");
	fprintf(stderr, "Cannot allocate space for %d channels\n", nchan);
	return 1;
 }
 memset(&sap, 0, sizeof(sap));
 sap.type = kSMChannelTypeOutput;
 sap.module = module;
 if (allocevents) {
	 sap.type = kSMChannelTypeFullDuplex;
	 if (allocevents != 2) {
		 ev_read = malloc(nchan * sizeof(*ev_read));
		 if (!ev_read) {
			perror("malloc() failed");
			fprintf(stderr, "Cannot allocate space for %d channels\n", nchan);
			return 1;
		 }
		 ev_write = malloc(nchan * sizeof(*ev_write));
		 if (!ev_write) {
			perror("malloc() failed");
			fprintf(stderr, "Cannot allocate space for %d channels\n", nchan);
			return 1;
		 }
		 ev_recog = malloc(nchan * sizeof(*ev_recog));
		 if (!ev_recog) {
			perror("malloc() failed");
			fprintf(stderr, "Cannot allocate space for %d channels\n", nchan);
			return 1;
		 }
	 }
 }
 ftime(&laststats);
 memset(&scp, 0, sizeof(scp));
 scp.st = 48;
 for (; maxbatch--;) {
	unsigned u;
	for (u=0; u < nchan; u++) {
		struct timeb start, now;
		unsigned long dtim;
		int r;
		{
		 struct sm_channel_alloc_placed_parms t = sap;
		 ftime(&start);
		 r = sm_channel_alloc_placed(&t);
		 ftime(&now);
		 if (r) {
			fprintf(stderr, "sm_channel_alloc_placed returned %d\n", r);
			return 1;
		 }
		 chan[u] = t.channel;
		}
		if (allocevents) {
			if (allocevents != 2)
				r = alloc_events(chan[u],&ev_read[u],&ev_write[u],&ev_recog[u]);
			else
				r = alloc_events_any(chan[u]);
			if (r) return r;
			ftime(&now);
		}
		if (timeslot) {
			SM_SWITCH_CHANNEL_PARMS t = scp;
			t.channel = chan[u];
			r = sm_switch_channel_output(&t);
			if (r) {
				fprintf(stderr, "sm_switch_channel_output returned %d\n", r);
				return 1;
			}
			ftime(&now);
		}
		dtim = (now.time - start.time) * 1000
			+ now.millitm - start.millitm;
		if (dtim > as.maxms) {
			as.maxms = dtim;
		}
		as.remms += dtim - as.avems;
		as.n++;
		while (as.remms < 0) {
			as.remms += as.n;
			as.avems--;
		}
		while (as.remms >= (int) as.n) {
			as.remms -= as.n;
			as.avems++;
		}
	}
	for (u=0; u < nchan; u++) {
		struct timeb start, now;
		unsigned long dtim;
		int r;
		ftime(&start);
		if (allocevents) {
			if (allocevents != 2)
				r = free_events(chan[u],ev_read[u],ev_write[u],ev_recog[u]);
			else
				r = free_events_any(chan[u]);
			if (r) return r;
		}
		r = sm_channel_release(chan[u]);
		ftime(&now);
		if (r) {
			fprintf(stderr, "sm_channel_release returned %d\n", r);
			return 1;
		}
		dtim = (now.time - start.time) * 1000
			+ now.millitm - start.millitm;
		if (dtim > fs.maxms) {
			fs.maxms = dtim;
		}
		fs.remms += dtim - fs.avems;
		fs.n++;
		if (dtim != fs.avems) {
			while (fs.remms < 0) {
				fs.remms += fs.n;
				fs.avems--;
			}
			while (fs.remms >= (int) fs.n) {
				fs.remms -= fs.n;
				fs.avems++;
			}
		}
	}
	{
	 struct timeb now;
	 unsigned long dtim;
	 ftime(&now);
	 dtim = (now.time - laststats.time) * 1000
		+ now.millitm - laststats.millitm;
	 if (dtim >= 1000) {
		printf("alloc\tav:%g\tmx:%g\trelease\tav:%g\tmx:%g\tn:%ld\n",
			as.avems + as.remms / (double) as.n,
			(double) as.maxms,
			fs.avems + fs.remms / (double) fs.n,
			(double) fs.maxms,
			fs.n);
		laststats = now;
	 }
	}
	//sleep(1);
 }
 free(chan);
 free(ev_read);
 free(ev_write);
 free(ev_recog);
 return 0;
}

struct allocthreadparam {
	unsigned nchan;
	unsigned maxbatch;
	tSMModuleId module;
	int allocevents;
	int timeslot;
};

static void *threadalloc(void *p)
{
 struct allocthreadparam *tp = p;
 return (void *) talloc(tp->nchan, tp->maxbatch, tp->module, tp->allocevents, tp->timeslot);
}

static int maintalloc(unsigned nthreads, unsigned nchan, unsigned maxbatch, int timeslot, tSMModuleId module, int allocevents)
{
 pthread_t *tidp = 0;
 unsigned u;
 int sts;
 if (nthreads) {
	struct allocthreadparam ap;
	tidp = malloc(sizeof(*tidp) * nthreads);
	if (!tidp) {
		perror("malloc() failed");
		fprintf(stderr, "Cannot allocate %d thread IDs\n", nthreads);
		return 1;
	}
	ap.nchan = nchan;
	ap.maxbatch = maxbatch / (nthreads + 1);
	ap.module = module;
	ap.allocevents = allocevents;
	ap.timeslot = timeslot;
	for (u=0; u < nthreads; u++) {
		int e = pthread_create(&tidp[u], 0, threadalloc, &ap);
		if (e) {
			fprintf(stderr, "pthread_create() failed; %d\n", e);
			return 1;
		}
		maxbatch -= ap.maxbatch;
 	}
 }
 sts = talloc(nchan, maxbatch, module, allocevents, timeslot);
 for (u=0; u < nthreads; u++) {
	void *tsts;
	int e = pthread_join(tidp[u], &tsts);
	if (e) {
		fprintf(stderr, "pthread_join() failed; %d\n", e);
		return 1;
	}
	if (tsts) sts = 1;
 }
 free(tidp);
 return sts;
}

#include "gen/alloc.args.i"

int main(int argc, char **argv)
{
 tSMCardId card;
 tSMModuleId mod;
 int iErr;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || *argv) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n\tallocevents 0=none, 1=specific, 2=anychannel\n", progname);
	return 1;
 }
 if (!arg.numchan) arg.numchan = 1;
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
 if (arg.allocevents==2 && create_any()) {
	 printf("Failed creating anychannel events\n");
	 return 1;
 }
 iErr = maintalloc(arg.threads, arg.numchan, arg.maxbatch, arg.timeslot, mod, arg.allocevents);
 if (arg.allocevents==2 && free_any()) {
	 printf("Failed freeing anychannel events\n");
	 return 1;
 }
 if (arg.serialnumber) {
	modclose(mod);
	cardclose(card);
 }
 return iErr;
}
