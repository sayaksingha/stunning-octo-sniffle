/* locplay.c - demonstration of play on local timeslots with an eventset */
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef TiNGTYPE_LINUX
#define TiNG_POSIX_THREADS
#define MULTIPLE_EVENTSETS
#define EVENTSET_LIMIT 256
#endif

#ifdef TiNGTYPE_QNX
#define TiNG_POSIX_THREADS
#endif


#ifdef TiNG_POSIX_THREADS
#include <sys/poll.h>
#include <sys/time.h>
#include <unistd.h>
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
#include "../../libutil/random.h"
#include "../Testlib/error.h"
#include "../Testlib/fmtcode.h"
#include "../Testlib/errcode_sm.h"
#include "../Testlib/eventset.h"
#include "../Testlib/perthread_trace.h"
#include "../Testlib/remote_trace.h"
#include "smbesp.h"
#include "smdrvr.h"

#ifdef PROSODY_TiNG
#include "../Testlib/cardlist.h"
#include "../../apilib/smsync.h"	// unpublished API
#else
#include "smosintf.h"
typedef int tSMModuleId;
#endif

#define arlen(x) (sizeof(x)/sizeof(*(x)))

struct appactive {
	tSMChannelId chan;
	FILE *fp;
	int once;	// play only once and then stop
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
	unsigned xfersize;
	EVENTSET* evs;
};

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
#ifdef TiNG_POSIX_THREADS
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
		// parameters which can be changed during replay
	int agc;
	enum {
		ONCE_NOT,		// play file repeatedly
		ONCE_ONLY,		// play file exactly once
		ONCE_REPEAT,		// play once, then restart
	} once;
	int speed;
	int volume;
		// not changeable
	int flood;
	unsigned delay;
	enum kSMDataFormat format;
	tSM_UT32 rate;
	char *fname;
	unsigned curcard;
	unsigned curmod;
	CARDLIST *cards;
	MVIP background;
	MVIP timeslot;
	unsigned max_octets;
	int threshold;
	unsigned per_mod;
	unsigned thismod;
	unsigned maxts;
	tSMChannelId bgchan;
	unsigned out_sample_rate;
} CHANPAR;

	// A thread-safe, high quality random number generator
	// This is used to for the random startup delay option
struct randnum {
	RANDMT rand;
	pthread_mutex_t mx;
};

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
#ifdef TiNG_POSIX_THREADS
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

	// wait a random amount of time
static err_t sleeprand(struct randnum *rp, unsigned delay)
{
 int e = pthread_mutex_lock(&rp->mx);
 double d;
 if (e) return error_errno(e, "pthread_mutex_lock() failed");
 d = randmt_real1(&rp->rand) * delay;
 e = pthread_mutex_unlock(&rp->mx);
 if (e) return error_errno(e, "pthread_mutex_unlock() failed");
 Sleep(d);
 return 0;
}

	// the cleanup handler for when the channel gets deleted from 
	// the worker's list of active channels.
	//
	// This is run by the worker either because a work order
	// deleted the channel from the active list or because the
	// channel's event handler finished playing
	//
	// Note that we must not delete the resources since the main
	// thread might be in the middle of adjusting a parameter
static void cleanup_replay(ACTIVE_JOB *ajp)
{
 setacstate(ajp->mf, ACSTATE_DONE);
}

static void free_active(ACTIVE_JOB *ajp)
{
 free(ajp->mf);
 free(ajp);
}

	// free any resources associated with the active channel
static void free_resources(ACTIVE_JOB *ajp)
{
 smd_ev_free(ajp->event);
 sm_channel_release(ajp->mf->chan);
 if (ajp->mf->fp) fclose(ajp->mf->fp);
 free_active(ajp);
}

	// check the current replay status
static err_t replay_status(ACTIVE_JOB *ajp, enum kSMReplayStatus *sp)
{
 struct sm_replay_status_parms rp;
 int e;
 memset(&rp, 0, sizeof(rp));
 rp.channel = ajp->mf->chan;
 e = sm_replay_status(&rp);
 if (e) return prosody_error(e, "sm_replay_status() failed");
 *sp = rp.status;
 return 0;
}

	// abort a replay
static err_t abort_replay(tSMChannelId chan, int nowait)
{
 struct sm_replay_abort_parms rp;
 int e;
 memset(&rp, 0, sizeof(rp));
 rp.channel = chan;
#ifdef PROSODY_TiNG
 rp.nowait = nowait;
#endif
 e = sm_replay_abort(&rp);
 if (e) return prosody_error(e, "sm_replay_abort() failed");
 return 0;
}

	// tell main thread that we have started if synchronisation
	// is being used
static err_t checksync(ACTIVE_JOB *ajp)
{
#ifdef PROSODY_TiNG
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
#endif
 return 0;
}

	// we have capacity for more data on a channel - give it
	// some data if there is any to give it
static err_t handle_capacity(ACTIVE_JOB *ajp, int *had_eof)
{
 SM_TS_DATA_PARMS dp;
 char buff[2048];
 size_t nc = ajp->mf->fp ? fread(buff, 1, ajp->mf->xfersize, ajp->mf->fp) : ajp->mf->xfersize;
 err_t err;
 int e;
 if (ajp->mf->fp && ferror(ajp->mf->fp)) {
	return error(error_errno(errno, "fread()failed"),
		"Cannot read replay data file");
 }
 memset(&dp, 0, sizeof(dp));
 dp.channel = ajp->mf->chan;
 dp.data = buff;
 dp.length = nc;
 if (!nc) {	// EOF
	if (ajp->mf->once != ONCE_NOT) {
			// only playing file once - finish at EOF
			// then we may restart or quit
		e = sm_put_last_replay_data(&dp);
		if (e) return prosody_error(e, "sm_put_last_replay_data() failed");
	} 
	if (ajp->mf->once != ONCE_ONLY) {
			// playing repeatedly - start again at the beginning
			// this may play continuously or by restarting
		if (*had_eof) {
				// already had EOF - got no data after rewinding
			return error(0, "File is empty");
		}
		if (fseek(ajp->mf->fp, 0, SEEK_SET)) {
			err = error_errno(errno, "fseek() failed");
			return error(err, "Cannot rewind file");
		}
		*had_eof = 1;
		return 0;
	}
 } else {
	e = sm_put_replay_data(&dp);
	if (e) return prosody_error(e, "sm_put_replay_data() failed");
 }
 if (ajp->mf->acstate == ACSTATE_IDLE) {
	setacstate(ajp->mf, ACSTATE_BUSY);
 }
 err = checksync(ajp);
 if (err) return error(err, "checksync() failed");
 return 0;
}

	// the handler for when a channel's event is signalled
	// note that this is run by the worker thread
static int handle_replay(ACTIVE_JOB *ajp)
{
 err_t err = 0;
 int had_eof = 0;
 for (;;) {
	enum kSMReplayStatus status;
	err = replay_status(ajp, &status);
	if (err) break;
	switch (status) {
	case kSMReplayStatusUnderrun:
		setacstate(ajp->mf, ACSTATE_UNDERRUN);
		return 0;
	case kSMReplayStatusComplete:
#ifdef TEST_EVENT
		test_event(ajp->event, "after completion");
#endif
		if (ajp->mf->once == ONCE_REPEAT) {
			 struct sm_replay_parms rp;
			 int e;
			 memset(&rp, 0, sizeof(rp));
			 rp.channel = ajp->mf->chan;
			 rp.type = 8;
			 e = sm_replay_start(&rp);
			 if (e) {
				error_log(stderr, prosody_error(e, "sm_replay_start() failed"));
			 	return 1;
			 }
			 return 0;
		}
		return 1;
	case kSMReplayStatusCompleteData:
		return 0;
	case kSMReplayStatusHasCapacity:
		err = handle_capacity(ajp, &had_eof);
			/* optimisation: no need to re-check status
			 *
			 * Prosody version 2 (TiNG) guarantees that the
			 * event triggered by available space remains
			 * set until the space is used. This means that
			 * when we write some data, we do not need to
			 * worry about whether we have filled all the
			 * available space. Therefore we return. If there
			 * is more space avilable, we will simply find the
			 * event still set and get back here to handle it.
			 *
			 * In version 1, however, the event is only set by
			 * space *becoming* available, if we do not make
			 * full use of the space by filling it with data,
			 * we will never get another event and the replay
			 * will stall.
			 */
#ifdef PROSODY_TiNG
		if (!err) return 0;	// only Prosody version 2 (TiNG)
#endif
		break;
	case kSMReplayStatusNoCapacity:
		return 0;
	}
	if (err) break;
 }
	// some sort of error - abort and report
 error_log(stderr, error(err, "handle_replay() failed"));
 abort_replay(ajp->mf->chan,0);
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

	// open a file for reading
static err_t open_file(ACTIVE_JOB *ajp, char *fname)
{
 err_t err = 0;
 if (!fname) {
 	ajp->mf->fp = 0;
 } else {
	ajp->mf->fp = fopen(fname, "rb");
	if (!ajp->mf->fp) {
		char *n;
		err = error_errno(errno, "fopen() failed");
		n = malloc(strlen(fname) + sizeof("File: ''"));
		if (n) sprintf(n, "File: '%s'", fname);
		err = error(err, n);
		err = error(err, "Cannot open file");
		free(n);
	}
 }
 return err;
}

	// allocate a Prosody channel
static err_t alloc_chan(ACTIVE_JOB *ajp, tSMModuleId module)
{
 SM_CHANNEL_ALLOC_PLACED_PARMS ap;
 int e;
 memset(&ap, 0, sizeof(ap));
 ap.type = kSMChannelTypeOutput;
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
#ifdef PROSODY_TiNG
 sw.type = xts.type;
#endif
 e = sm_switch_channel_output(&sw);
 if (e) return prosody_error(e, "sm_switch_channel_output() failed");
 return 0;
}

	// allocate an event
static err_t alloc_event(ACTIVE_JOB *ajp)
{
 int e = smd_ev_create(&ajp->event, ajp->mf->chan,
	kSMEventTypeWriteData, kSMChannelSpecificEvent);
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
 sep.event_type = kSMEventTypeWriteData;
 sep.issue_events = kSMChannelSpecificEvent;
 e = sm_channel_set_event(&sep);
 if (e) return prosody_error(e, "sm_channel_set_event() failed");
 return 0;
}

	// start playing
static err_t start_replay(ACTIVE_JOB *ajp, CHANPAR chanpar)
{
 struct sm_replay_parms rp;
 err_t err;
 int e;
 err = sleeprand(ajp->mf->rp, chanpar.delay);
 if (err) return error(err, "sleeprand() failed");
#ifdef PROSODY_TiNG
 if (chanpar.threshold) {
	SM_CHANNEL_SET_OUTPUT_THRESHOLD_PARMS sotp;
	memset(&sotp, 0, sizeof(sotp));
	sotp.channel = ajp->mf->chan;
	sotp.minimum_bits = chanpar.threshold;
	e = sm_channel_set_output_threshold(&sotp);
	if (e) return prosody_error(e, "sm_channel_set_output_threshold() failed");
 }
 if (chanpar.out_sample_rate) {
 	SM_CHANNEL_SET_OUTPUT_RATE_PARMS orp;
 	memset(&orp, 0, sizeof(orp));
 	orp.channel = ajp->mf->chan;
 	orp.sample_rate = chanpar.out_sample_rate;
 	e = sm_channel_set_output_rate(&orp);
	if (e) return prosody_error(e, "sm_channel_set_output_rate() failed");
 }
#endif
 memset(&rp, 0, sizeof(rp));
 rp.channel = ajp->mf->chan;
 rp.background = chanpar.bgchan;
 rp.volume = chanpar.volume;
 rp.speed = chanpar.speed;
 rp.agc = chanpar.agc;
 rp.type = chanpar.format;
#ifdef PROSODY_TiNG
 rp.sampling_rate = chanpar.rate;
#endif
 rp.data_length = chanpar.max_octets;
#ifdef TEST_EVENT
 test_event(ajp->event, "before start");
#endif
 e = sm_replay_start(&rp);
 if (e) return prosody_error(e, "sm_replay_start() failed");
#ifdef TEST_EVENT
 test_event(ajp->event, "after start");
 Sleep(1000);
 test_event(ajp->event, "after start + 1S");
#endif
#ifdef PROSODY_TiNG
 if (ajp->mf->sync) {
	SM_SYNC_ADD_PARMS sa;
	memset(&sa, 0, sizeof(sa));
	sa.channel = ajp->mf->chan;
	sa.synchroniser = ajp->mf->sync->synchroniser;
	e = sm_sync_add(&sa);
	if (e) return prosody_error(e, "sm_sync_add() failed");
 }
#else
 if (handle_replay(ajp)) return error(0, "initial handle_replay() failed");
#endif
 return 0;
}

	// adjust a replay which has already started
static err_t adj_replay(tSMChannelId chan, CHANPAR chanpar)
{
 struct sm_replay_adjust_parms rp;
 int e;
 memset(&rp, 0, sizeof(rp));
 rp.channel = chan;
 rp.background = chanpar.bgchan;
 rp.volume = chanpar.volume;
 rp.speed = chanpar.speed;
 rp.agc = chanpar.agc;
 e = sm_replay_adjust(&rp);
 if (e) return prosody_error(e, "sm_replay_adjust() failed");
 return 0;
}

	// start a new replay
static err_t replay(ACTIVE_JOB **ajpp, EVENTSET *evs, CHANPAR chanpar, struct threadsync *ts, struct randnum *rp)
{
 ACTIVE_JOB *ajp;
 err_t err = alloc_active(&ajp);
 if (!err) {
	*ajpp = ajp;
	initacstate(ajp->mf);
#ifdef PROSODY_TiNG
	ajp->mf->sync = ts;
#endif
	ajp->handler = handle_replay;
	ajp->cleanup_fn = cleanup_replay;
	ajp->mf->once = chanpar.once;
	if (chanpar.threshold <= 0) ajp->mf->xfersize = 2048;
	else ajp->mf->xfersize = chanpar.threshold / 8;
	ajp->mf->rp = rp;
	err = open_file(ajp, chanpar.fname);
	if (!err) {
		MODDESC mod;
		err = cardlist_getmod(&mod, chanpar.cards, chanpar.curcard, chanpar.curmod);
		if (!err) err = alloc_chan(ajp, mod.id);
		if (!err) {
#ifdef PROSODY_TiNG
			err = assign_timeslot(ajp, chanpar.timeslot);
#endif
			if (!err) {
				err = alloc_event(ajp);
				if (!err) {
					err = associate_event(ajp);
					if (!err) {
						err = start_replay(ajp, chanpar);
						if (!err) {
							err = eventset_insert(evs, ajp);
							if (!err) {
								ajp->mf->evs = evs;
								return 0;
							}
						}
					}
					smd_ev_free(ajp->event);
				}
			}
			sm_channel_release(ajp->mf->chan);
		}
		if (ajp->mf->fp) fclose(ajp->mf->fp);
	}
	free_active(ajp);
	*ajpp = 0;
 }
 return error(err, "Cannot play file");
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
	} else if (c == '-' && !ndig) isneg = !isneg;
	else {
		ungetc(c, stdin);
		return isneg ? v : -v;
	}
 }
}

	// read a timeslot from stdin
static err_t readts(MVIP *tsp)
{
 MVIP ts;
 int c;
 ts.stream = readint();
 c = getchar();
 if (c != ':') return error(0, "no ':' in timeslot");
 ts.timeslot = readint();
 c = getchar();
#ifdef PROSODY_TiNG
 ts.type = kSMTimeslotTypeALaw;
 if (c == ':') {
	c = getchar();
	switch (c) {
	case 'a': ts.type = kSMTimeslotTypeALaw; break;
	case 'u': ts.type = kSMTimeslotTypeMuLaw; break;
	case 'r': ts.type = kSMTimeslotTypeData; break;
	default: return error(0, "Expected 'a', 'u', or 'r' in timeslot");
	}
 }
#endif
 *tsp = ts;
 return 0;
}

#ifdef PROSODY_TiNG
	// allow synchronised replays to start now
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
#endif

	// setup the background channel
	// This simple sets up a dummy channel which uses the selected
	// timeslot - it doesn't actually play anything down it. To
	// test background replay, use some other test program to
	// generate a signal on this timeslot
static err_t setup_background(CHANPAR *chanparp, MVIP bg)
{
 SM_CHANNEL_ALLOC_PLACED_PARMS ap;
 SM_SWITCH_CHANNEL_PARMS sw;
 MODDESC mod;
 err_t err;
 int e;
 chanparp->bgchan = kSMNullChannelId;
 if (!bg.stream) return 0;
 memset(&ap, 0, sizeof(ap));
 ap.type = bg.timeslot >= 32 ? kSMChannelTypeInput : kSMChannelTypeOutput;
 err = cardlist_getmod(&mod, chanparp->cards, chanparp->curcard, chanparp->curmod);
 if (err) return err;
 ap.module = mod.id;
 e = sm_channel_alloc_placed(&ap);
 if (e) return prosody_error(e, "sm_channel_alloc_placed() failed");
 chanparp->bgchan = ap.channel;
 memset(&sw, 0, sizeof(sw));
 sw.channel = ap.channel;
 sw.st = bg.stream;
 sw.ts = bg.timeslot;
#ifdef PROSODY_TiNG
 sw.type = bg.type;
#endif
 e = sm_switch_channel_output(&sw);
 if (e) {
	err = prosody_error(e, "sm_switch_channel_output() failed");
	e = sm_channel_release(chanparp->bgchan);
	chanparp->bgchan = kSMNullChannelId;
	if (e) {
		err_t nerr = prosody_error(e, "sm_channel_release() failed");
		return error_join(err, nerr);
	}
 }
 return 0;
}

static void *remote_trace_thread(void *arg)
{
 remote_trace(arg);
 return 0;
}

static err_t start_remote_trace(char *port)
{
 pthread_t tid;
 int e = pthread_create(&tid, 0, remote_trace_thread, port);
 if (e) {
 	err_t err = error_errno(e, "pthread_create() failed");
 	err = error(err, "cannot start remote trace thread");
 	return err;
 }
 return 0;
}

/*
 * The main play loop. Since we want to display channel status and adjust
 * channels, the cleanup function above cannot free the resources (in case
 * we were accessing the channel in this thread while it was freed in the
 * worker thread). Therefore we need to check for finished channels and
 * clear up. There is no real penalty in deferring this cleanup since
 * if we are sitting waiting for a command there's nothing useful we
 * could do with the newly freed resources.
 */
static err_t locplay(CHANPAR chanpar, int use_sync, unsigned numchan, char *remport)
{
 ACTIVE_JOB *ajlist[8192];
 struct threadsync *ts = 0;
 unsigned freeac = 0;		// free entry after all used ones in ajlist
 struct randnum ran;
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
 if (remport) {
	err = start_remote_trace(remport);
	if (err) return err;
 }
 randmt(&ran.rand, 1);
 e = pthread_mutex_init(&ran.mx, 0);
 if (e) return error_errno(e, "pthread_mutex_init() failed");
 err = setup_background(&chanpar, chanpar.background);
 if (err) return error(err, "cannot setup background channel");
#ifdef PROSODY_TiNG
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
#endif
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
	int c;
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
			Sleep(1000);
		}
		c = '\n';
	} else c = getchar();
	switch (c) {
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
		printf("starting replay on %d:%d\n",
			chanpar.timeslot.stream, chanpar.timeslot.timeslot);
if (u == 255) TiNGtrace = 8;
		err = replay(&ajlist[u], evsetp, chanpar, ts, &ran);
if (u == 255) TiNGtrace = 0;
		if (err) error_log(stderr, err);
		else {
			if (++chanpar.timeslot.timeslot + 0u == chanpar.maxts) {
				chanpar.timeslot.stream++;
				chanpar.timeslot.timeslot = 0;
			}
			if (chanpar.per_mod && ++chanpar.thismod >= chanpar.per_mod) {
				chanpar.thismod = 0;
				cardlist_nextmod(chanpar.cards, &chanpar.curcard, &chanpar.curmod);
			}
		}
		break;
	case '?':	// help
		printf("Commands:\n"
			"\t+\tstart replay with current agc, speed, and volume\n"
			"\t#...\tcomment\n"
			"\tTN\tset TiNGtrace to N\n"
			"\taN\tset agc (0=off, 1=on)\n"
#ifdef PROSODY_TiNG
			"\tg\tgo - start synced channels\n"
#endif
			"\tkN\tkill channel N\n"
			"\tKN\tkill channel N without waiting\n"
			"\toN\tset once (0=repeat, 1=once, 2=once+restart)\n"
			"\tsN\tset speed\n"
			"\ttN\tset timeslot\n"
			"\tuN\tupdate channel N to current agc, speed and volume\n"
			"\tvN\tset volume\n"
			"\txN\tset max octets\n"
			"\tw\twait for current replays to finish\n"
			"\tQ\tabort all current replays\n"
			"\tS\tshutdown\n"
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
#ifdef PROSODY_TiNG
 	case 'g':	// go
 		err = runsync(ts);
		ts = 0;
 		if (err) return error(err, "runsync() failed");
		break;
#endif
	case 'k':	// kill
	case 'K':
		u = readint();
		if (u >= freeac || !ajlist[u]) {
			printf("Channel %d not in use\n", u);
			break;
		}
		ajlist[u]->mf->once = ONCE_ONLY;
		err = abort_replay(ajlist[u]->mf->chan,c=='K');
		if (err) error_log(stderr, err);
		break;
	case 'o':	// once
		chanpar.once = readint();
		break;
	case 's':
		chanpar.speed = readint();
		break;
#ifdef PROSODY_TiNG
	case 'T':	// Trace
		TiNGtrace = readint();
		break;
#endif
	case 't':	// timeslot
		err = readts(&chanpar.timeslot);
		if (err) error_log(stderr, err);
		break;
	case 'u':	// update
		u = readint();
		if (u >= freeac || !ajlist[u]) {
			printf("Channel %d not in use\n", u);
			break;
		}
		err = adj_replay(ajlist[u]->mf->chan, chanpar);
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
		for(u=0; u < freeac; u++) {
			if (ajlist[u] && ajlist[u]->mf->acstate!=ACSTATE_DONE) {
				ajlist[u]->mf->once = ONCE_ONLY;
				err = abort_replay(ajlist[u]->mf->chan, 1);
				if (err) error_log(stderr, err);
			}
		}
		break;
	case 'q':	// quit

#ifndef MULTIPLE_EVENTSETS
		stopworker(tid, &evset);
#else
		for (u=0; u < nextevset; u++) {
			stopworker(tid[u], &evset[u]);
		}
#endif
		return 0;
	case 'S':	// shutdown
			// first ensure no other threads are using the channels
		for (u=0; u < freeac; u++) {
			if (ajlist[u]) eventset_delete(ajlist[u]->mf->evs,ajlist[u]);
		}
#ifndef MULTIPLE_EVENTSETS
		stopworker(tid, &evset);
#else
		for (u=0; u < nextevset; u++) {
			stopworker(tid[u], &evset[u]);
		}
#endif
			// then free the channels and events
		for (u=0; u < freeac; u++) {
			if (ajlist[u]) free_resources(ajlist[u]);
		}
		return 0;
	case EOF:
		return 0;
	}
 }
}

#ifndef PROSODY_TiNG
	// dummy declarations which do nothing except on TiNG
#define kSMTimeslotTypeALaw 0
#define kSMTimeslotTypeMuLaw 0
#define kSMTimeslotTypeData 0
int TiNGtrace;
#endif

#include "gen/locplay.args.i"

int main(int argc, char **argv)
{
 CARDLIST cards;
 CHANPAR chanpar;
 err_t err;
 ARGS_DECL
 (void) argc;
#ifdef TiNGTYPE_WINNT
 _setmaxstdio(2048);
#endif
 if (ARGS_CALL || (*argv && argv[1])) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE " [file]\nFormats:", progname);
	fmtlist_dump(stderr);
	return 1;
 }
 //assp_trace(65535);
 //setlog(9);
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
 chanpar.agc = arg.agc;
 chanpar.delay = arg.randdelay;
 chanpar.once = ONCE_NOT;
 if (arg.once) {
 	chanpar.once = ONCE_ONLY;
	if (arg.restart) {
		fprintf(stderr, "Error: both 'once' and 'restart' requested\n");
		return 1;
	}
 }
 if (arg.restart) chanpar.once = ONCE_REPEAT;
 chanpar.speed = arg.speed;
 chanpar.volume = arg.volume;
 chanpar.cards = &cards;
 chanpar.background = arg.background;
 chanpar.timeslot = arg.timeslot;
 chanpar.fname = *argv;
 chanpar.max_octets = arg.maxoctets;
 chanpar.threshold = arg.threshold;
 chanpar.per_mod = arg.per_mod;
 chanpar.thismod = 0;
 chanpar.maxts = arg.maxts;
 chanpar.flood = arg.flood;
 chanpar.out_sample_rate = arg.out_sample_rate;
 err = locplay(chanpar, arg.sync, arg.numchan, arg.remote_trace_port);
 if (err) {
	error_log(stderr, err);
	return 1;
 }
 cardlist_dtor(&cards);
 return 0;
}
