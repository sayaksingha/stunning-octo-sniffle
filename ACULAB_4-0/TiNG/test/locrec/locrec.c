/* locrec.c - demonstration of record on local timeslots with an eventset */

#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef TiNGTYPE_LINUX
#include <poll.h>
#include <sys/time.h>
#include <unistd.h>
#define USE_POLL
#define MULTIPLE_EVENTSETS
#define EVENTSET_LIMIT 256
#endif

#ifdef TiNGTYPE_QNX
#include <sys/poll.h>
#include <sys/time.h>
#include <unistd.h>
#define USE_POLL
#endif


#ifdef TiNGTYPE_WINNT
#include <sys/timeb.h>
#define MULTIPLE_EVENTSETS
#define EVENTSET_LIMIT 60
#endif

#ifdef _MSC_VER

/* disable stupid warnings about doubles being converted to floats.
 */

#pragma warning(disable:4244)
#pragma warning(disable:4305)
/* and about argument conversions (does anyone still use a
   non-prototype compiler?) */
#pragma warning(disable:4761)

#endif

#include "../../libutil/generic_io.h"
#include "../Testlib/error.h"
#include "../Testlib/fmtcode.h"
#include "../Testlib/errcode_sm.h"
#include "../Testlib/eventset.h"
#include "../Testlib/taistamp.h"
#include "../Testlib/cardlist.h"
#include "smbesp.h"
#include "smdrvr.h"
#include "../../apilib/smsync.h"	// unpublished API

#define arlen(x) (sizeof(x)/sizeof(*(x)))

struct appactive {
	tSMChannelId chan;
	FILE *fp;
	enum acstate {
		ACSTATE_IDLE,
		ACSTATE_BUSY,
		ACSTATE_UNDERRUN,
		ACSTATE_DONE,
		ACSTATE_NUM
	} acstate;
#ifdef PROSODY_TiNG
	struct threadsync {
		tSMSynchroniser synchroniser;
		pthread_mutex_t mx;
		pthread_cond_t synced;
		unsigned waitcount;
	} *sync;
#endif
	struct randnum *rp;
	int underrun_flag;
	char id[10];
};

	// we need this because the automatic argument parser
	// refers to a type 'MVIP' when it produces code to
	// handle timeslots
typedef struct {
	int stream;
	int timeslot;
	int type;
} MVIP;
#define MVIP_STREAM_NONE 0xff

	// This conveniently gathers together the configurable
	// parameters for a channel
typedef struct {
	char *fname;
	unsigned curcard;
	unsigned curmod;
	CARDLIST *cards;
	int flood;
	MVIP timeslot;
	enum kSMDataFormat format;
	tSM_UT32 rate;
	unsigned sil_elim;
	unsigned tone_elim;
	unsigned tone_elim_set;
	unsigned max_octets;
	unsigned max_silence;
	unsigned max_elapsed;
	unsigned thresh;
	int agc;
	int volume;
	int underrun_flag;
	unsigned per_mod;
	unsigned thismod;
	unsigned maxts;
	double grunt_min_noise_level;
	double grunt_threshold;
} CHANPAR;

	// to allow quick but reliable startup
static struct {
	pthread_mutex_t mx;
	pthread_cond_t cv;
	unsigned nstate[ACSTATE_NUM];
} statelist;

static void setacstate(struct appactive *mf, enum acstate acstate)
{
 pthread_mutex_lock(&statelist.mx);
 statelist.nstate[mf->acstate]--;
 statelist.nstate[mf->acstate = acstate]++;
 pthread_mutex_unlock(&statelist.mx);
 pthread_cond_broadcast(&statelist.cv);
}

static void waitbusydone(void)
{
 struct timespec giveup;
#ifdef TiNGTYPE_LINUX
 struct timeval now;
 gettimeofday(&now, 0);
 giveup.tv_sec = now.tv_sec + 1;
 giveup.tv_nsec = now.tv_usec * 1000;
#endif
#ifdef TiNGTYPE_QNX
 struct timeval now;
 gettimeofday(&now, 0);
 giveup.tv_sec = now.tv_sec + 1;
 giveup.tv_nsec = now.tv_usec * 1000;
#endif
#ifdef TiNGTYPE_WINNT
 struct _timeb now;
 _ftime(&now);
 giveup.tv_sec = now.time + 1;
 giveup.tv_nsec = now.millitm * 1000 * 1000;
#endif
 pthread_mutex_lock(&statelist.mx);
 for (;;) {
	int e;
	if (!statelist.nstate[ACSTATE_IDLE] && !statelist.nstate[ACSTATE_UNDERRUN]) { putchar('N'); break; }
 	e = pthread_cond_timedwait(&statelist.cv, &statelist.mx, &giveup);
 	if (e == ETIMEDOUT) { putchar('T'); break; }
 	if (e) {
		fprintf(stderr, "pthread_cond_timedwait() failed: %s\n", strerror(e));
		break;
 	}
 }
 pthread_mutex_unlock(&statelist.mx);
}

static void initacstate(struct appactive *mf)
{
 pthread_mutex_lock(&statelist.mx);
 statelist.nstate[mf->acstate = ACSTATE_IDLE]++;
 pthread_mutex_unlock(&statelist.mx);
}

static int initstatelist(void)
{
 int e = pthread_mutex_init(&statelist.mx, 0);
 if (e) {
 	fprintf(stderr, "pthread_mutex_init() failed: %s\n", strerror(e));
 	return 1;
 }
 e = pthread_cond_init(&statelist.cv, 0);
 if (e) {
 	fprintf(stderr, "pthread_cond_init() failed: %s\n", strerror(e));
 	return 1;
 }
 return 0;
}

	// Convert a Prosody error code and an explanation into an err_t
static err_t prosody_error(int rc, char *text)
{
 const char *name = errcode_sm(rc);
 char buf[80];
 if (!name) {
 	sprintf(buf, "Prosody error %d\n", rc);
 	name = buf;
 }
 return error(error(0, name), text);
}

#ifdef TEST_EVENT
static void test_event(tSMEventId ev, char *where)
{
#ifdef USE_POLL
 struct pollfd pfd;
 int i;
 pfd.fd = ev.fd;
 pfd.events = ev.mode;
 i = poll(&pfd, 1, 0);
 printf("event %s %s\n", pfd.revents ? "set" : "clear", where);
#endif
#ifdef TiNGTYPE_WINNT
 DWORD d = WaitForSingleObject(ev, 0);
 printf("event %s %s\n", d == WAIT_OBJECT_0 ? "set" : "clear", where);
#endif
}
#endif

	// the cleanup handler for when the channel gets deleted from 
	// the worker's list of active channels.
	//
	// This is run by the worker either because a work order
	// deleted the channel from the active list or because the
	// channel's event handler finished recording
	//
	// Note that we must not delete the resources since the main
	// thread might be in the middle of adjusting a parameter
static void cleanup_record(ACTIVE_JOB *ajp)
{
 setacstate(ajp->mf, ACSTATE_DONE);
}

	// free any resources associated with the active channel
static void free_resources(ACTIVE_JOB *ajp)
{
 smd_ev_free(ajp->event);
 sm_channel_release(ajp->mf->chan);
 if (ajp->mf->fp) fclose(ajp->mf->fp);
 free(ajp);
}

	// abort a record
static err_t abort_record(tSMChannelId chan, int discard)
{
 struct sm_record_abort_parms rp;
 int e;
 memset(&rp, 0, sizeof(rp));
 rp.channel = chan;
 rp.discard = discard;
 e = sm_record_abort(&rp);
 if (e) return prosody_error(e, "sm_record_abort() failed");
 return 0;
}

	// adjust AGC gain in a record
static err_t agc_adj_record(tSMChannelId chan, float gain)
{
 struct sm_record_agc_adjust_parms rp;
 int e;
 memset(&rp, 0, sizeof(rp));
 rp.channel = chan;
 rp.gain = gain;
 e = sm_record_agc_adjust(&rp);
 if (e) return prosody_error(e, "sm_record_agc_adjust() failed");
 return 0;
}

	// tell main thread that we have started if synchronisation
	// is being used
static err_t checksync(ACTIVE_JOB *ajp)
{
 if (ajp->mf->sync) {
	int e = pthread_mutex_lock(&ajp->mf->sync->mx);
	if (e) return error_errno(e, "pthread_mutex_lock() failed");
	if (!--ajp->mf->sync->waitcount) {
		e = pthread_cond_broadcast(&ajp->mf->sync->synced);
		if (e) return error_errno(e, "pthread_cond_broadcast() failed");
	}
	e = pthread_mutex_unlock(&ajp->mf->sync->mx);
	if (e) return error_errno(e, "pthread_mutex_unlock() failed");
	ajp->mf->sync = 0;
 }
 return 0;
}

	// we have data on a channel - fetch it
static err_t handle_data(ACTIVE_JOB *ajp)
{
 SM_TS_DATA_PARMS dp;
 char buff[2048];
 int e;
 memset(&dp, 0, sizeof(dp));
 dp.channel = ajp->mf->chan;
 dp.data = buff;
 e = sm_get_recorded_data(&dp);
 if (e) return prosody_error(e, "sm_get_recorded_data() failed");
 if (ajp->mf->fp) {
	size_t nc = fwrite(buff, 1, dp.length, ajp->mf->fp);
	if (ferror(ajp->mf->fp)) {
		return error(error_errno(errno, "fwrite()failed"),
			"Cannot write recorded data file");
 	}
 	if (nc < (unsigned) dp.length) {
		return error(error(0, "fwrite() performed partial write"),
			"Cannot write recorded data file");
 	}
 }
 if (ajp->mf->acstate == ACSTATE_IDLE) {
	setacstate(ajp->mf, ACSTATE_BUSY);
 }
 return 0;
}

	// the handler for when a channel's event is signalled
	// note that this is run by the worker thread
static int handle_record(ACTIVE_JOB *ajp)
{
 struct sm_record_status_parms rp;
 err_t err = 0;
 int e;
 memset(&rp, 0, sizeof(rp));
 rp.channel = ajp->mf->chan;
 e = sm_record_status(&rp);
 if (e) {
	err = prosody_error(e, "sm_record_status() failed");
 } else switch (rp.status) {
 case kSMRecordStatusOverrun:
	if (ajp->mf->underrun_flag) {
		char now[32];
		taistamp(now);
		printf("%s %s underrun\n", now, ajp->mf->id);
	}
	setacstate(ajp->mf, ACSTATE_UNDERRUN);
	return 0;
 case kSMRecordStatusComplete:
#ifdef TEST_EVENT
	test_event(ajp->event, "after completion");
#endif
	if (rp.termination_reason == kSMRecordHowTerminatedError) {
		fprintf(stderr, "Record terminated by error\n");
	}
	return 1;
 case kSMRecordStatusCompleteData:
 case kSMRecordStatusData:
	err = handle_data(ajp);
		/* optimisation: no need to re-check status
		 *
		 * Prosody version 2 (TiNG) guarantees that the
		 * event triggered by available data remains
		 * set until the data is read. This means that
		 * when we read some data, we do not need to
		 * worry about whether we have read all the
		 * available data. Therefore we return. If there
		 * is more data available, we will simply find the
		 * event still set and get back here to handle it.
		 *
		 * In version 1, however, the event is only set by
		 * data *becoming* available, if we do not read
		 * all of the data we will never get another
		 * event and the record will stall.
		 */
	if (!err) return 0;	// only Prosody version 2 (TiNG)
	break;
 case kSMRecordStatusNoData:
	err = checksync(ajp);
	if (err) {
		err = error(err, "checksync() failed");
		break;
	}
	return 0;
 case kSMRecordStatusRecognition:
	printf("recognised: type=%d, p0=%d, p1=%d\n",
		rp.recog_type, rp.param0, rp.param1);
	return 0;
 }
	// some sort of error - abort and report
 error_log(stderr, error(err, "handle_record() failed"));
 abort_record(ajp->mf->chan, 1);
 return 1;
}

/* Here are a bunch of little utility functions which really just
 * encapsulate one action each.
 */

	// allocate an ACTIVE_JOB object
static err_t alloc_active(ACTIVE_JOB **ajpp)
{
 *ajpp = malloc(sizeof(**ajpp));
 if (!*ajpp) {
	err_t err = error_errno(errno, "malloc() failed");
	return error(err, "cannot make channel object");
 }
 (*ajpp)->mf = malloc(sizeof(*(*ajpp)->mf));
 if (!(*ajpp)->mf) {
	err_t err = error_errno(errno, "malloc() failed");
	free(*ajpp);
	return error(err, "cannot make private fields for channel object");
 }
 return 0;
}

	// open a file for writing
static err_t open_file(ACTIVE_JOB *ajp, char *fmain, int st, int ts)
{
 if (!fmain) {
	ajp->mf->fp = 0;
 } else {
	char *fname = malloc(strlen(fmain) + sizeof("0123456789")*2);
	err_t err = 0;
	if (!fname) return error_errno(errno, "malloc() failed");
	sprintf(fname, fmain, st, ts);
	ajp->mf->fp = fopen(fname, "wb");
	if (!ajp->mf->fp) {
		char *n = malloc(strlen(fname) + sizeof("File: ''"));
		if (n) sprintf(n, "File: '%s'", fname);
		err = error_errno(errno, "fopen() failed");
		err = error(err, n);
		err = error(err, "Cannot open file");
		free(n);
	}
	free(fname);
	return err;
 }
 return 0;
}

	// allocate a Prosody channel
static err_t alloc_chan(ACTIVE_JOB *ajp, tSMModuleId module)
{
 SM_CHANNEL_ALLOC_PLACED_PARMS ap;
 int e;
 memset(&ap, 0, sizeof(ap));
 ap.type = kSMChannelTypeInput;
 ap.module = module;
 e = sm_channel_alloc_placed(&ap);
 if (e) return prosody_error(e, "sm_channel_alloc_placed() failed");
 ajp->mf->chan = ap.channel;
 return 0;
}

	// assign a specified timeslot
static err_t assign_timeslot(ACTIVE_JOB *ajp, MVIP xts)
{
 SM_SWITCH_CHANNEL_PARMS sw;
 int e;
 memset(&sw, 0, sizeof(sw));
 sw.channel = ajp->mf->chan;
 sw.st = xts.stream;
 sw.ts = xts.timeslot;
 sw.type = xts.type;
 e = sm_switch_channel_input(&sw);
 if (e) return prosody_error(e, "sm_switch_channel_input() failed");
 return 0;
}

	// set the desired data transfer threshold
static err_t set_threshold(ACTIVE_JOB *ajp, unsigned thresh)
{
 if (thresh) {		// value zero leaves default setting
	SM_CHANNEL_SET_INPUT_THRESHOLD_PARMS tp;
	int e;
	memset(&tp, 0, sizeof(tp));
	tp.channel = ajp->mf->chan;
	tp.minimum_bits = thresh * 8;
	e = sm_channel_set_input_threshold(&tp);
	if (e) return prosody_error(e, "sm_channel_set_input_threshold() failed");
 }
 return 0;
}

	// allocate an event
static err_t alloc_event(ACTIVE_JOB *ajp)
{
 int e = smd_ev_create(&ajp->event, ajp->mf->chan,
	kSMEventTypeReadData, kSMChannelSpecificEvent);
 if (e) return prosody_error(e, "smd_ev_create() failed");
 return 0;
}

	// associate event with channel
static err_t associate_event(ACTIVE_JOB *ajp)
{
 SM_CHANNEL_SET_EVENT_PARMS sep;
 int e;
 memset(&sep, 0, sizeof(sep));
 sep.event = ajp->event;
 sep.channel = ajp->mf->chan;
 sep.event_type = kSMEventTypeReadData;
 sep.issue_events = kSMChannelSpecificEvent;
 e = sm_channel_set_event(&sep);
 if (e) return prosody_error(e, "sm_channel_set_event() failed");
 return 0;
}

	// start recording
static err_t start_record(ACTIVE_JOB *ajp, CHANPAR chanpar)
{
 struct sm_record_parms rp;
 int e;
 memset(&rp, 0, sizeof(rp));
 rp.channel = ajp->mf->chan;
 rp.volume = chanpar.volume;
 rp.agc = chanpar.agc;
 rp.type = chanpar.format;
 rp.sampling_rate = chanpar.rate;
 rp.silence_elimination = chanpar.sil_elim;
 rp.tone_elimination_mode = chanpar.tone_elim;
 rp.tone_elimination_set_id = chanpar.tone_elim_set;
 rp.max_silence = chanpar.max_silence;
 rp.max_elapsed_time = chanpar.max_elapsed;
 rp.max_octets = chanpar.max_octets;
 rp.min_noise_level = chanpar.grunt_min_noise_level;
 rp.grunt_threshold = chanpar.grunt_threshold;
#ifdef TEST_EVENT
 test_event(ajp->event, "before start");
#endif
 e = sm_record_start(&rp);
 if (e) return prosody_error(e, "sm_record_start() failed");
#ifdef TEST_EVENT
 test_event(ajp->event, "after start");
 Sleep(1000);
 test_event(ajp->event, "after start + 1S");
#endif
 if (ajp->mf->sync) {
	SM_SYNC_ADD_PARMS sa;
	memset(&sa, 0, sizeof(sa));
	sa.channel = ajp->mf->chan;
	sa.synchroniser = ajp->mf->sync->synchroniser;
	e = sm_sync_add(&sa);
	if (e) return prosody_error(e, "sm_sync_add() failed");
 }
 return 0;
}

	// start a new record
static err_t record(ACTIVE_JOB **ajpp, EVENTSET *evs, CHANPAR chanpar, struct threadsync *ts)
{
 ACTIVE_JOB *ajp;
 err_t err = alloc_active(&ajp);
 if (!err) {
	sprintf(ajp->mf->id, "%d:%d",
		chanpar.timeslot.stream, chanpar.timeslot.timeslot);
	printf("starting record on %s\n", ajp->mf->id);
	*ajpp = ajp;
	initacstate(ajp->mf);
	ajp->mf->sync = ts;
	ajp->mf->underrun_flag = chanpar.underrun_flag;
	ajp->handler = handle_record;
	ajp->cleanup_fn = cleanup_record;
	err = open_file(ajp, chanpar.fname, chanpar.timeslot.stream,
			chanpar.timeslot.timeslot);
	if (!err) {
		MODDESC mod;
		err = cardlist_getmod(&mod, chanpar.cards, chanpar.curcard, chanpar.curmod);
		if (!err) err = alloc_chan(ajp, mod.id);
		if (!err) {
			err = assign_timeslot(ajp, chanpar.timeslot);
			if (!err) {
				err = set_threshold(ajp, chanpar.thresh);
				if (!err) {
					err = alloc_event(ajp);
					if (!err) {
						err = associate_event(ajp);
						if (!err) {
							err = start_record(ajp, chanpar);
							if (!err) {
								err = eventset_insert(evs, ajp);
								if (!err) return 0;
							}
						}
						smd_ev_free(ajp->event);
					}
				}
			}
			sm_channel_release(ajp->mf->chan);
		}
		if (ajp->mf->fp) fclose(ajp->mf->fp);
	}
	free(ajp);
	*ajpp = 0;
 }
 return error(err, "Cannot record file");
}

	// the worker thread is this function
static void *worker_thread(void *p)
{
 EVENTSET *evs = p;
 err_t err;
#ifdef TiNGTYPE_WINNT
 if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS)) {
	error_log(stderr, error_last("SetPriorityClass"));
	return (void *) 1;
 }
#endif
 err = eventset_wait(evs);
 if (err) {
	error_log(stderr, error(err, "worker thread failed"));
	return (void *) 1;
 }
 return 0;
}

	// start the worker thread
static err_t startworker(pthread_t *tidp, EVENTSET *evs)
{
 err_t err = eventset(evs);
 int e;
 if (err) return err;
 e = pthread_create(tidp, 0, worker_thread, evs);
 if (e) {
 	err = error_errno(e, "pthread_create() failed");
 	err = error(err, "cannot start worker thread");
 }
 return err;
}

static err_t stopworker(pthread_t tid, EVENTSET *evs)
{
 void *sts;
 err_t err = eventset_stop(evs);
 int e;
 if (err) return err;
 e = pthread_join(tid, &sts);
 if (e) {
 	err = error_errno(e, "pthread_join() failed");
 	return error(err, "cannot join with worker thread");
 }
 eventset_dtor(evs);
 return 0;
}

	// read an integer from stdin
static int readint(void)
{
 int isneg = 0, ndig=0;
 int v = 0;
 for (;;) {
	int c = getchar();
	if (c >= '0' && c <= '9') {
		v = v * 10 - c + '0';
		ndig++;
	} else if (c == '-' && !ndig) {
		isneg = !isneg;
	} else if (c == '+' && !ndig) {
		// unary +
	} else {
		ungetc(c, stdin);
		return isneg ? v : -v;
	}
 }
}

	// read a float from stdin
static float readfloat(void)
{
 int isneg = 0, ndig=0;
 float v = 0;
 int c;
 for (;;) {
	c = getchar();
	if (c >= '0' && c <= '9') {
		v = v * 10 + c - '0';
		ndig++;
	} else if (c == '-' && !ndig) {
		isneg = !isneg;
	} else if (c == '.') {
		float p = 0.1;
		for (;;) {
			c = getchar();
			if (c >= '0' && c <= '9') {
				v += (c - '0') * p;
				p *= 0.1;
			} else if (c == 'e') {
				v *= pow(10.0, readint());
				c = getchar();
				break;
			} else break;
		}
		break;
	} else break;
 }
 ungetc(c, stdin);
 return isneg ? -v : v;
}

	// read a timeslot from stdin
static err_t readts(MVIP *tsp)
{
 MVIP ts;
 int c;
 ts.type = kSMTimeslotTypeALaw;
 ts.stream = readint();
 c = getchar();
 if (c != ':') return error(0, "no ':' in timeslot");
 ts.timeslot = readint();
 c = getchar();
 if (c == ':') {
	c = getchar();
	switch (c) {
	case 'a': ts.type = kSMTimeslotTypeALaw; break;
	case 'u': ts.type = kSMTimeslotTypeMuLaw; break;
	case 'r': ts.type = kSMTimeslotTypeData; break;
	default: return error(0, "Expected 'a', 'u', or 'r' in timeslot");
	}
 }
 *tsp = ts;
 return 0;
}

	// allow synchronised records to start now
static err_t runsync(struct threadsync *ts)
{
 SM_SYNC_DELETE_PARMS sp;
 int e;
 if (!ts) {
	printf("Not awiting sync\n");
	return 0;
 }
 memset(&sp, 0, sizeof(sp));
 e = pthread_mutex_lock(&ts->mx);
 if (e) return error_errno(e, "pthread_mutex_lock() failed");
 while (ts->waitcount) {
		e = pthread_cond_wait(&ts->synced, &ts->mx);
		if (e) return error_errno(e, "pthread_cond_wait() failed");
 }
 e = pthread_mutex_unlock(&ts->mx);
 if (e) return error_errno(e, "pthread_mutex_unlock() failed");
 e = pthread_mutex_destroy(&ts->mx);
 if (e) return error_errno(e, "pthread_mutex_destoy() failed");
 e = pthread_cond_destroy(&ts->synced);
 if (e) return error_errno(e, "pthread_cond_destoy() failed");
 sp.synchroniser = ts->synchroniser;
 e = sm_sync_delete(&sp);
 if (e) return prosody_error(e, "sm_sync_delete() failed");
 free(ts);
 return 0;
}

/*
 * The main record loop. Since we want to display channel status,
 * the cleanup function above cannot free the resources (in case
 * we were accessing the channel in this thread while it was freed in the
 * worker thread). Therefore we need to check for finished channels and
 * clear up. There is no real penalty in deferring this cleanup since
 * if we are sitting waiting for a command there's nothing useful we
 * could do with the newly freed resources.
 */
static err_t locrec(CHANPAR chanpar, int use_sync, unsigned numchan)
{
 ACTIVE_JOB *ajlist[8192];
 struct threadsync *ts = 0;
 unsigned freeac = 0;		// free entry after all used ones in ajlist
 int waiting = 0;
#ifndef MULTIPLE_EVENTSETS
 EVENTSET evset;
 pthread_t tid;
 err_t err = startworker(&tid, &evset);
 int e;
 if (err) return err;
#else
 EVENTSET evset[(arlen(ajlist) + EVENTSET_LIMIT - 1) / EVENTSET_LIMIT];
 pthread_t tid[arlen(evset)];
 unsigned nextevset = 0;
 err_t err;
 int e;
#endif
 initstatelist();
 if (use_sync) {
 		/*
 		 * This synchroniser uses an unpublished API. It is
 		 * unpublished because the correct use of the
 		 * synchroniser requires detailed knowlege of how
 		 * the operations are implemented to ensure that the
 		 * channel gets up to the right point in processing
 		 * before being frozen awaiting permission to start.
 		 * The information required to do this correctly may
 		 * change arbitrarily from release to release and may
 		 * not work at all in some releases. Therefore it is
 		 * for Aculab use only, and even then only for certain
 		 * specific tests.
 		 */
	SM_SYNC_CREATE_PARMS sp;
	MODDESC mod;
	ts = malloc(sizeof(*ts));
	if (!ts) {
		err = error_errno(errno, "malloc() failed");
		return error(err, "cannot make synchroniser object");
	}
	ts->waitcount = numchan;
	memset(&sp, 0, sizeof(sp));
	err = cardlist_getmod(&mod, chanpar.cards, chanpar.curcard, chanpar.curmod);
	if (err) return err;
	sp.module = mod.id;
	e = sm_sync_create(&sp);
	if (e) return prosody_error(e, "sm_sync_create() failed");
	ts->synchroniser = sp.synchroniser;
	e = pthread_mutex_init(&ts->mx, 0);
	if (e) return error_errno(e, "pthread_mutex_init() failed");
	e = pthread_cond_init(&ts->synced, 0);
	if (e) return error_errno(e, "pthread_cond_init() failed");
 }
 for (;;) {	// the main loop waiting for commands
		/* if we wanted to have a continuous status update
		 * (such as every second), a good way to do it would
		 * be to use a separate thread to read the input and
		 * pass it to a manager thread which uses a wait
		 * with timeout to read the input and prints the new
		 * status on a timeout. Since it's easy enough to get
		 * a status by hitting CR, and printing status would
		 * interfere with measuring CPU usage, we don't
		 * bother.
		 */
	unsigned allok = 1;
	unsigned u;
	int c = '\n';
	putchar('\r');
	if (numchan && !chanpar.flood) waitbusydone();
	for (u=0; u < freeac; u++) {
		if (!ajlist[u]) putchar(' ');
		else switch (ajlist[u]->mf->acstate) {
		case ACSTATE_IDLE: putchar('.'); allok = 0; break;
		case ACSTATE_BUSY:
			putchar('*');
			setacstate(ajlist[u]->mf, ACSTATE_IDLE);
			break;
		case ACSTATE_UNDERRUN:
			allok = 0;
			putchar('u');
			setacstate(ajlist[u]->mf, ACSTATE_IDLE);
			break;
		case ACSTATE_DONE:
			putchar('-');
			free_resources(ajlist[u]);
			ajlist[u] = 0;
			break;
		case ACSTATE_NUM:;	// for warnings only
		}
	}
	while (freeac > 0 && !ajlist[freeac-1]) freeac--;
	if (ts) printf("sync awaiting 'g'");
	putchar(')');
	putchar('>');
	if (numchan) {
			/* we've been asked to pre-load a certain number
			 * of channels - while we have more to create,
			 * pretend that we got a '+' command.
			 */
		if (allok || chanpar.flood) {
			c = '+';
			numchan--;
		} else {
			printf("+%u\n", numchan);
			c = '\n';
		}
	} else if (waiting) {
		static int twiddle;
		if (!freeac) {
			putchar('\n');
			waiting = 0;
		} else {
			putchar('W');
			putchar("-\\|/"[twiddle++]);
			twiddle &= 3;
			fflush(stdout);
#ifdef TiNGTYPE_LINUX
			sleep(1);
#endif
#ifdef TiNGTYPE_QNX
			sleep(1);
#endif
#ifdef TiNGTYPE_WINNT
			Sleep(1000);
#endif
		}
		c = '\n';
	} else c = getchar();
	switch (c) {
		float f;
#ifndef MULTIPLE_EVENTSETS
#define evsetp (&evset)
#else
		EVENTSET *evsetp;
		unsigned evsetno;
#endif
	case '\n': break;
	case '+':	// start new channel
		for (u=0; u < freeac && ajlist[u]; u++);
		if (u == arlen(ajlist)) {
			printf("cannot start: too many channels\n");
			break;
		}
		if (u == freeac) freeac++;
#ifdef MULTIPLE_EVENTSETS
		evsetno = u / EVENTSET_LIMIT;
		evsetp = &evset[evsetno];
		if (evsetno >= nextevset) {
			err = startworker(&tid[evsetno], evsetp);
			nextevset++;
		}
#endif
		err = record(&ajlist[u], evsetp, chanpar, ts);
		if (err) error_log(stderr, err);
		else {
			if (++chanpar.timeslot.timeslot + 0u == chanpar.maxts) {
				chanpar.timeslot.stream++;
				chanpar.timeslot.timeslot = 0;
			}
			if (chanpar.per_mod && ++chanpar.thismod == chanpar.per_mod) {
				cardlist_nextmod(chanpar.cards, &chanpar.curcard, &chanpar.curmod);
				chanpar.thismod = 0;
			}
		}
		break;
	case '?':	// help
		printf("Commands:\n"
			"\t+\tstart record with current agc, and volume\n"
			"\t#...\tcomment\n"
			"\tTN\tset TiNGtrace to N\n"
			"\taN\tset agc (0=off, 1=on)\n"
			"\tGN=V\tset agc gain to V for channel N\n"
			"\tg\tgo - start synced channels\n"
			"\tkN\tkill channel N\n"
			"\tKN\tkill channel N, discarding uncollected data\n"
			"\tsN\tset max silence\n"
			"\ttN\tset timeslot\n"
			"\tvN\tset volume\n"
			"\txN\tset max octets\n"
			"\tw\twait for current records to finish\n"
			"\tq\tquit\n"
			"\t?\tshow command list\n"
			);
		break;
	case '#':
		for (;;) {
			c = getchar();
			if (c == EOF) return 0;
			putchar(c);
			if (c == '\n') break;
		}
		break;
	case 'a':	// agc
		chanpar.agc = readint();
		break;
 	case 'g':	// go
 		err = runsync(ts);
		ts = 0;
 		if (err) return error(err, "runsync() failed");
		break;
	case 'G':	// AGC gain
		u = readint();
		if (u >= freeac || !ajlist[u]) {
			printf("Channel %d not in use\n", u);
			break;
		}
		c = getchar();
		if (c != '=') {
			f = 0;
			ungetc(c, stdin);
		} else {
			f = readfloat();
		}
		err = agc_adj_record(ajlist[u]->mf->chan, f);
		if (err) error_log(stderr, err);
		break;
	case 'k':	// kill
	case 'K':	//   with discard
		u = readint();
		if (u >= freeac || !ajlist[u]) {
			printf("Channel %d not in use\n", u);
			break;
		}
		err = abort_record(ajlist[u]->mf->chan, c == 'K');
		if (err) error_log(stderr, err);
		break;
	case 'l':	// max elapsed time
		chanpar.max_elapsed = readint();
		break;
	case 's':	// max silence
		chanpar.max_silence = readint();
		break;
	case 'T':	// Trace
		TiNGtrace = readint();
		break;
	case 't':	// timeslot
		err = readts(&chanpar.timeslot);
		if (err) error_log(stderr, err);
		break;
	case 'v':	// volume
		chanpar.volume = readint();
		break;
	case 'x':	// max octets
		chanpar.max_octets = readint();
		break;
	case 'w':
		waiting = 1;
		break;
	case 'Q':	// quit
		for (u=0; u < freeac; u++) {
			if (ajlist[u] && ajlist[u]->mf->acstate!=ACSTATE_DONE) {
				err = abort_record(ajlist[u]->mf->chan, 1);
				if (err) error_log(stderr, err);
			}
		}
		break;
	case 'q':	// quit
	case EOF:
#ifndef MULTIPLE_EVENTSETS
		stopworker(tid, &evset);
#else
		for (u=0; u < nextevset; u++) {
			stopworker(tid[u], &evset[u]);
		}
#endif
		return 0;
		return 0;
	}
 }
}

#include "gen/locrec.args.i"

int main(int argc, char **argv)
{
 CARDLIST cards;
 CHANPAR chanpar;
 err_t err;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || (*argv && argv[1])) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE " [file]\nFormats:", progname);
	fmtlist_dump(stderr);
	return 1;
 }
 if (!arg.format) {
 	fprintf(stderr, "-F format option required\n");
 	return 1;
 }
 cardlist(&cards);
 chanpar.curcard = 0;
 chanpar.curmod = 0;
 if (arg.serialnumber) {
	err = cardlist_addcard(&cards, arg.serialnumber);
	if (err) {
		error_log(stderr, err);
		return 1;
	}
 }
 if (!cards.ncard) {
	fprintf(stderr, "No cards specifed\n");
	return 1;
 }
 if (fmtcode(&chanpar.format, &chanpar.rate, arg.format)) {
	fprintf(stderr, "Unknown format: '%s'\n", arg.format);
	return 1;
 }
 if (!arg.numchan) arg.numchan = 1;
 chanpar.sil_elim = arg.sil_elim;
 chanpar.tone_elim = arg.tone_elim_mode;
 chanpar.tone_elim_set = arg.tone_set;
 chanpar.agc = arg.agc;
 chanpar.volume = arg.volume;
 chanpar.cards = &cards;
 chanpar.timeslot = arg.timeslot;
 chanpar.fname = *argv;
 chanpar.max_silence = arg.max_silence;
 chanpar.max_elapsed = arg.max_elapsed_time;
 chanpar.max_octets = arg.maxoctets;
 chanpar.thresh = arg.threshold;
 chanpar.underrun_flag = arg.underrun_flag;
 chanpar.per_mod = arg.per_mod;
 chanpar.thismod = 0;
 chanpar.maxts = arg.maxts;
 chanpar.grunt_min_noise_level = arg.grunt_min_noise_level;
 chanpar.grunt_threshold = arg.grunt_threshold;
 chanpar.flood = arg.flood;
 err = locrec(chanpar, arg.sync, arg.numchan);
 if (err) {
	error_log(stderr, err);
 }
 cardlist_dtor(&cards);
 return err ? 1 : 0;
}
