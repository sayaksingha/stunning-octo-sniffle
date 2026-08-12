/* anylocplay.c - demonstration of play on local timeslots with 'any channel' event */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>

#ifdef TiNGTYPE_LINUX
#define USE_POLL
#include <poll.h>
#include <unistd.h>
#endif

#ifdef TiNGTYPE_QNX
#define USE_POLL
#include <sys/poll.h>
#include <unistd.h>
#endif


#ifdef TiNGTYPE_WINNT
#define USE_WFMO
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
#include "smbesp.h"
#include "smdrvr.h"

#ifdef PROSODY_TiNG
#include "../Testlib/cardlist.h"
#else
#include "smosintf.h"
typedef int tSMModuleId;
#endif

#include "pthread.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

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
	enum kSMDataFormat format;
	tSM_UT32 rate;
	char *fname;
	unsigned curcard;
	unsigned curmod;
	CARDLIST *cards;
#ifdef PROSODY_TiNG
	MVIP background;
	MVIP timeslot;
#endif
	unsigned max_octets;
	unsigned per_mod;
	unsigned thismod;
} CHANPAR;

typedef struct running_channel {
	tSMChannelId chan;
	FILE *fp;
	int once;	// play only once and then stop
		/*
		 *	-	closed
		 *	*	data
		 *	u	underrun
		 *	.	idle
		 */
	char status;	// for status display
	int stop;
} RUNNING_CHANNEL;

typedef struct {
	unsigned nchan;
	RUNNING_CHANNEL **chanlist;
} ALLCHAN;

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

static err_t allchan_add(ALLCHAN *ac, RUNNING_CHANNEL *cp)
{
 int e = sm_get_channel_ix(cp->chan);
 unsigned ci;
 if (e < 0) return prosody_error(e, "sm_get_channel_ix() failed");
 ci = e;
 if (ci >= ac->nchan) {
	unsigned newmax = ci + 1;
	RUNNING_CHANNEL **newlist = realloc(ac->chanlist,
		newmax * sizeof(*newlist));
	if (!newlist) {
		return error(error_errno(errno, "realloc()failed"),
			"Cannot extend channel list");
	}
	ac->chanlist = newlist;
	while (ac->nchan < newmax) ac->chanlist[ac->nchan++] = 0;
 }
 if (ac->chanlist[ci]) {
	fprintf(stderr, "duplicate channel index\n");
	abort();
 }
 ac->chanlist[ci] = cp;
 return 0;
}

static err_t allchan_del(ALLCHAN *ac, RUNNING_CHANNEL *cp)
{
 int e = sm_get_channel_ix(cp->chan);
 unsigned ci;
 if (e < 0) return prosody_error(e, "sm_get_channel_ix() failed");
 ci = e;
 if (ci >= ac->nchan || !ac->chanlist[ci]) abort();
 if (ci + 1 == ac->nchan) {	// last - can shrink it
	for (;;) {
		if (!ci) {
			free(ac->chanlist);
			ac->nchan = 0;
			ac->chanlist = 0;
			return 0;
		}
		if (ac->chanlist[--ci]) break;
	}
	// assert: ac->chanlist[ci]
	ac->nchan = ci + 1;
	ac->chanlist = realloc(ac->chanlist, ac->nchan * sizeof(*ac->chanlist));
 } else ac->chanlist[ci] = 0;
 return 0;
}

	// we have capacity for more data on a channel - give it
	// some data if there is any to give it
static err_t handle_capacity(RUNNING_CHANNEL *rcp)
{
 SM_TS_DATA_PARMS dp;
 char buff[kSMMaxReplayDataBufferSize];
 size_t nc = rcp->fp ? fread(buff, 1, sizeof(buff), rcp->fp) : sizeof(buff);
 err_t err;
 int e;
 if (rcp->fp && ferror(rcp->fp)) {
	return error(error_errno(errno, "fread()failed"),
		"Cannot read replay data file");
 }
 memset(&dp, 0, sizeof(dp));
 dp.channel = rcp->chan;
 dp.data = buff;
 dp.length = nc;
 if (!nc) {	// EOF
	if (rcp->once != ONCE_NOT) {
			// only playing file once - finish at EOF
			// then we may restart or quit
		e = sm_put_last_replay_data(&dp);
		if (e) return prosody_error(e, "sm_put_last_replay_data() failed");
	} 
	if (rcp->once != ONCE_ONLY) {
			// playing repeatedly - start again at the beginning
			// this may play continuously or by restarting
		if (fseek(rcp->fp, 0, SEEK_SET)) {
			err = error_errno(errno, "fseek() failed");
			return error(err, "Cannot rewind file");
		}
		return 0;
	}
 } else {
	e = sm_put_replay_data(&dp);
	if (e) return prosody_error(e, "sm_put_replay_data() failed");
 }
 if (rcp->status == '.') rcp->status = '*';
 return 0;
}

	// handle one channels which have triggered the event
static err_t checkonechan(ALLCHAN *acp, tSMChannelId chan, int status)
{
 int e = sm_get_channel_ix(chan);
 RUNNING_CHANNEL *rcp;
 unsigned ci;
 static unsigned lastchan;
 if (e < 0) return prosody_error(e, "sm_get_channel_ix() failed");
 ci = e;
 if (ci >= acp->nchan) abort();
 if (0 && ci != lastchan) {
 	printf("%d", ci);
 	lastchan = ci;
 }
 rcp = acp->chanlist[ci];
#ifndef PROSODY_TiNG
 retry:
#endif
 switch (status) {
	err_t err;
 case kSMReplayStatusComplete:
	if (rcp->once == ONCE_REPEAT) {
		 struct sm_replay_parms rep;
		 memset(&rep, 0, sizeof(rep));
		 rep.channel = rcp->chan;
		 rep.type = 8;
		 e = sm_replay_start(&rep);
		 if (e) {
			return prosody_error(e, "sm_replay_start() failed");
		 }
	} else {
		rcp->status = '-';
	}
	break;
 case kSMReplayStatusCompleteData:
	break;
 case kSMReplayStatusUnderrun:
	rcp->status = 'u';
#ifdef PROSODY_TiNG
		// fall through in Prosody V1
	break;
#endif
 case kSMReplayStatusHasCapacity:
	err = handle_capacity(rcp);
	if (err) return err;
#ifndef PROSODY_TiNG
	{
	 SM_REPLAY_STATUS_PARMS rp;
	 rp.channel = chan;
	 e = sm_replay_status(&rp);
	 if (e) return prosody_error(e, "sm_replay_status() failed");
	 status = rp.status;
	}
	goto retry;
#endif
	break;
 case kSMReplayStatusNoCapacity:
	break;
 }
 return 0;
}

#if 1
	// handle all channels which have triggered the event
static err_t checkchans(ALLCHAN *acp)
{
 for (;;) {
	SM_REPLAY_STATUS_PARMS rp;
	err_t err;
	int e;
	memset(&rp, 0, sizeof(rp));
	rp.channel = 0;
	e = sm_replay_status(&rp);
	if (e == ERR_SM_NO_SUCH_CHANNEL) return 0;
	if (e) return prosody_error(e, "sm_replay_status() failed");
	if (!rp.channel) {
		static int saidnull;
		if (!saidnull) printf("! null channel\n");
		saidnull = 1;
		return 0;
	}
	err = checkonechan(acp, rp.channel, rp.status);
	if (err) return err;
 }
}
#else
static err_t checkchans(ALLCHAN *acp)
{
 for (;;) {
	SM_BESP_STATUS_LIST_PARMS lp;
	err_t err;
	int e;
	memset(&lp, 0, sizeof(lp));
	e = sm_besp_write_status_list(&lp);
	if (e == ERR_SM_NO_SUCH_CHANNEL) return 0;
	if (e) return prosody_error(e, "sm_replay_status() failed");
	if (!lp.count) {
		static int saidnull;
		if (!saidnull) printf("! zero count\n");
		saidnull = 1;
		return 0;
	}
	for (e=0; e < lp.count; e++) {
		err = checkonechan(acp, lp.channel[e], lp.status[e]);
		if (err) return err;
	}
 }
}
#endif

static err_t showsts(ALLCHAN *acp)
{
 unsigned lastp = 0;
 unsigned u;
 for (u=0; u < acp->nchan; u++) {
	RUNNING_CHANNEL *rcp = acp->chanlist[u];
	if (rcp) {
		if (u - lastp > 2) {
			printf("%d", u);
		} //else for (; lastp < u; lastp++) putchar('_');
		putchar(rcp->status);
		switch (rcp->status) {
		case '-':
			allchan_del(acp, rcp);
			sm_channel_release(rcp->chan);
			if (rcp->fp) fclose(rcp->fp);
			free(rcp);
			break;
		case '*':
		case 'u':
			rcp->status = '.';
			break;
		}
		lastp = u;
	}
 }
 return 0;
}

	// abort a replay
static err_t abort_replay(tSMChannelId chan)
{
 struct sm_replay_abort_parms rp;
 int e;
 memset(&rp, 0, sizeof(rp));
 rp.channel = chan;
 e = sm_replay_abort(&rp);
 if (e) return prosody_error(e, "sm_replay_abort() failed");
 return 0;
}

/* Here are a bunch of little utility functions which really just
 * encapsulate one action each.
 */

	// allocate an RUNNING_CHANNEL object
static err_t alloc_running(RUNNING_CHANNEL **rcpp)
{
 *rcpp = malloc(sizeof(**rcpp));
 if (!*rcpp) {
	err_t err = error_errno(errno, "malloc() failed");
	return error(err, "cannot make channel object");
 }
 return 0;
}

	// open a file for reading
static err_t open_file(RUNNING_CHANNEL *rcp, char *fname)
{
 err_t err = 0;
 if (!fname) {
 	rcp->fp = 0;
 } else {
	rcp->fp = fopen(fname, "rb");
	if (!rcp->fp) {
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
static err_t alloc_chan(RUNNING_CHANNEL *rcp, tSMModuleId module)
{
 SM_CHANNEL_ALLOC_PLACED_PARMS ap;
 int e;
 memset(&ap, 0, sizeof(ap));
 ap.type = kSMChannelTypeOutput;
 ap.module = module;
 e = sm_channel_alloc_placed(&ap);
 if (e) return prosody_error(e, "sm_channel_alloc_placed() failed");
 rcp->chan = ap.channel;
 return 0;
}

#ifdef PROSODY_TiNG
	// assign a specified timeslot
static err_t assign_timeslot(RUNNING_CHANNEL *rcp, MVIP xts)
{
 SM_SWITCH_CHANNEL_PARMS sw;
 int e;
 memset(&sw, 0, sizeof(sw));
 sw.channel = rcp->chan;
 sw.st = xts.stream;
 sw.ts = xts.timeslot;
#ifdef PROSODY_TiNG
 sw.type = xts.type;
#endif
 e = sm_switch_channel_output(&sw);
 if (e) return prosody_error(e, "sm_switch_channel_output() failed");
 return 0;
}
#endif

	// associate event with channel
static err_t associate_event(RUNNING_CHANNEL *rcp, tSMEventId ev)
{
 SM_CHANNEL_SET_EVENT_PARMS sep;
 int e;
 memset(&sep, 0, sizeof(sep));
 sep.event = ev;
 sep.channel = rcp->chan;
 sep.event_type = kSMEventTypeWriteData;
 sep.issue_events = kSMAnyChannelEvent;
 e = sm_channel_set_event(&sep);
 if (e) return prosody_error(e, "sm_channel_set_event() failed");
 return 0;
}

	// start playing
static err_t start_replay(RUNNING_CHANNEL *rcp, CHANPAR chanpar, tSMChannelId bgchan, unsigned prefix)
{
 struct sm_replay_parms rp;
 int e;
 memset(&rp, 0, sizeof(rp));
 rp.channel = rcp->chan;
 rp.background = bgchan;
 rp.volume = chanpar.volume;
 rp.speed = chanpar.speed;
 rp.agc = chanpar.agc;
 rp.type = chanpar.format;
#ifdef PROSODY_TiNG
 rp.sampling_rate = chanpar.rate;
#endif
 rp.data_length = chanpar.max_octets;
 e = sm_replay_start(&rp);
 if (e) return prosody_error(e, "sm_replay_start() failed");
 if (prefix) {
	static char buff[kSMMaxReplayDataBufferSize];
	SM_TS_DATA_PARMS dp;
	memset(&dp, 0, sizeof(dp));
	dp.channel = rcp->chan;
	dp.data = buff;
	dp.length = prefix;
	e = sm_put_replay_data(&dp);
	if (e) return prosody_error(e, "sm_put_replay_data() failed");
 }
 return 0;
}

	// adjust a replay which has already started
static err_t adj_replay(tSMChannelId chan, CHANPAR chanpar)
{
 struct sm_replay_adjust_parms rp;
 int e;
 memset(&rp, 0, sizeof(rp));
 rp.channel = chan;
 rp.volume = chanpar.volume;
 rp.speed = chanpar.speed;
 rp.agc = chanpar.agc;
 e = sm_replay_adjust(&rp);
 if (e) return prosody_error(e, "sm_replay_adjust() failed");
 return 0;
}

	// start a new replay
static err_t replay(ALLCHAN *acp, tSMEventId ev, CHANPAR chanpar, tSMChannelId bgchan, unsigned prefix)
{
 RUNNING_CHANNEL *rcp;
 err_t err = alloc_running(&rcp);
 if (!err) {
	rcp->status = '.';
	rcp->once = chanpar.once;
	err = open_file(rcp, chanpar.fname);
	if (!err) {
		MODDESC mod;
		err = cardlist_getmod(&mod, chanpar.cards, chanpar.curcard, chanpar.curmod);
		if (!err) err = alloc_chan(rcp, mod.id);
		if (!err) {
#ifdef PROSODY_TiNG
			err = assign_timeslot(rcp, chanpar.timeslot);
#endif
			if (!err) {
				err = associate_event(rcp, ev);
				if (!err) {
					err = allchan_add(acp, rcp);
					if (!err) {
						err = start_replay(rcp, chanpar, bgchan, prefix);
#ifndef PROSODY_TiNG
						if (!err) {
							err = handle_chan(rcp, kSMReplayStatusHasCapacity);
						}
#endif
						if (!err) return 0;
					}
					allchan_del(acp, rcp);
				}
			}
			sm_channel_release(rcp->chan);
		}
		if (rcp->fp) fclose(rcp->fp);
	}
	free(rcp);
 }
 return error(err, "Cannot play file");
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
#ifdef PROSODY_TiNG
 ts.type = kSMTimeslotTypeALaw;
#endif
 ts.stream = readint();
 c = getchar();
 if (c != ':') return error(0, "no ':' in timeslot");
 ts.timeslot = readint();
 c = getchar();
#ifdef PROSODY_TiNG
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
	// setup the background channel
	// This simple sets up a dummy channel which uses the selected
	// timeslot - it doesn't actually play anything down it. To
	// test background replay, use some other test program to
	// generate a signal on this timeslot
static err_t setup_background(tSMChannelId *chanp, CHANPAR *chanparp, MVIP bg)
{
 SM_CHANNEL_ALLOC_PLACED_PARMS ap;
 SM_SWITCH_CHANNEL_PARMS sw;
 MODDESC mod;
 err_t err;
 int e;
 *chanp = kSMNullChannelId;
 if (!bg.stream) return 0;
 memset(&ap, 0, sizeof(ap));
 ap.type = bg.timeslot >= 32 ? kSMChannelTypeInput : kSMChannelTypeOutput;
 err = cardlist_getmod(&mod, chanparp->cards, chanparp->curcard, chanparp->curmod);
 if (err) return err;
 ap.module = mod.id;
 e = sm_channel_alloc_placed(&ap);
 if (e) return prosody_error(e, "sm_channel_alloc_placed() failed");
 *chanp = ap.channel;
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
	e = sm_channel_release(*chanp);
	*chanp = 0;
	if (e) {
		err_t nerr = prosody_error(e, "sm_channel_release() failed");
		return error_join(err, nerr);
	}
 }
 return 0;
}
#endif

	// allocate an event
static err_t alloc_event(tSMEventId *evp)
{
 int e = smd_ev_create(evp, kSMNullChannelId, kSMEventTypeWriteData, kSMAnyChannelEvent);
 if (e) return prosody_error(e, "smd_ev_create() failed");
 return 0;
}

static err_t newchan(ALLCHAN *acp, tSMEventId anyev, CHANPAR *chanpar, tSMChannelId bgchan, unsigned prefix)
{
 err_t err;
#ifdef PROSODY_TiNG
 printf("starting replay on %d:%d\n",
	chanpar->timeslot.stream, chanpar->timeslot.timeslot);
#else
 printf("starting replay %d\n", acp->nchan);
#endif
 err = replay(acp, anyev, *chanpar, bgchan, prefix);
 if (err) return err;
#ifdef PROSODY_TiNG
 if (! (++chanpar->timeslot.timeslot & 0x1f)) {
	chanpar->timeslot.stream ^= 1;
	chanpar->timeslot.timeslot -= 32;
 }
#endif
 if (chanpar->per_mod && ++chanpar->thismod >= chanpar->per_mod) {
	chanpar->thismod = 0;
	cardlist_nextmod(chanpar->cards, &chanpar->curcard, &chanpar->curmod);
 }
 return 0;
}

struct cmd_state {
	int needsts;
	int waiting;
	int eof;
};

static err_t do_line(ALLCHAN *acp, tSMEventId anyev, CHANPAR *chanpar, tSMChannelId bgchan, struct cmd_state *cmd_state)
{
 for (;;) {
	int c = getchar();
	unsigned u;
	switch (c) {
		err_t err;
	case '\n': return 0;
	case '+':	// start new channel
		err = newchan(acp, anyev, chanpar, bgchan, 0);
		if (err) return err;
		cmd_state->needsts = 1;
		break;
	case '?':	// help
		printf("Commands:\n"
			"\t+\tstart replay with current agc, speed, and volume\n"
			"\t#...\tcomment\n"
#ifdef PROSODY_TiNG
			"\tTN\tset TiNGtrace to N\n"
#endif
			"\taN\tset agc (0=off, 1=on)\n"
			"\tkN\tkill channel N\n"
			"\toN\tset once (0=repeat, 1=once, 2=once+restart)\n"
			"\tsN\tset speed\n"
			"\ttN\tset timeslot\n"
			"\tU\tclear 'underrun' status\n"
			"\tuN\tupdate channel N to current agc, speed and volume\n"
			"\tvN\tset volume\n"
			"\txN\tset max octets\n"
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
		chanpar->agc = readint();
		break;
	case 'k':	// kill
		u = readint();
		if (u >= acp->nchan || !acp->chanlist[u]) {
			printf("Channel %d not in use\n", u);
			break;
		}
		err = abort_replay(acp->chanlist[u]->chan);
		if (err) error_log(stderr, err);
		cmd_state->needsts = 1;
		break;
	case 'o':	// once
		chanpar->once = readint();
		break;
	case 's':
		chanpar->speed = readint();
		break;
#ifdef PROSODY_TiNG
	case 'T':	// Trace
		TiNGtrace = readint();
		break;
	case 't':	// timeslot
		err = readts(&chanpar->timeslot);
		if (err) error_log(stderr, err);
		break;
#endif
	case 'U':
		for (u=0; u < acp->nchan; u++) {
			RUNNING_CHANNEL *rcp = acp->chanlist[u];
			if (rcp && rcp->status == 'u') rcp->status = '.';
		 }
		break;
	case 'u':	// update
		u = readint();
		if (u >= acp->nchan || !acp->chanlist[u]) {
			printf("Channel %d not in use\n", u);
			break;
		}
		err = adj_replay(acp->chanlist[u]->chan, *chanpar);
		if (err) error_log(stderr, err);
		break;
	case 'v':	// volume
		chanpar->volume = readint();
		break;
	case 'x':	// max octets
		chanpar->max_octets = readint();
		break;
	case 'w':
		cmd_state->waiting = 1;
		cmd_state->needsts = 1;
		break;
	case 'q':	// quit
	case EOF:
		cmd_state->eof = 1;
		return 0;
	}
 }
}

static long time_since_mS(struct timeb when)
{
 long dtim;
 struct timeb now;
 ftime(&now);
 dtim = now.time - when.time;
 dtim *= 1000;
 dtim += now.millitm - when.millitm;
 return dtim;
}

#ifdef USE_WFMO
static err_t is_stdin(int *ready)
{
 const HANDLE in = GetStdHandle(STD_INPUT_HANDLE);
 *ready = 0;
 for (;;) {
	INPUT_RECORD inp;
	DWORD nread;
	if (!PeekConsoleInput(in, &inp, 1, &nread)) {
		return error(
			error_last("PeekConsoleInput() failed"),
				"Cannot check for non-keyboard input");
	}
	if (!nread) return 0;
	if (inp.EventType == KEY_EVENT
		&& inp.Event.KeyEvent.bKeyDown
		&& inp.Event.KeyEvent.uChar.AsciiChar) {
		*ready = 1;
		return 0;
	}
	if (!ReadConsoleInput(in, &inp, 1, &nread)) {
		return error(
			error_last("ReadConsoleInput() failed"),
				"Cannot discard non-keyboard input");
	}
	if (!nread) {
		return error(0, "ReadConsoleInput returned 0 items!");
	}
 }
}
#endif

/*
 * The main play loop. Since we want to display channel status and adjust
 * channels, the cleanup function above cannot free the resources (in case
 * we were accessing the channel in this thread while it was freed in the
 * worker thread). Therefore we need to check for finished channels and
 * clear up. There is no real penalty in deferring this cleanup since
 * if we are sitting waiting for a command there's nothing useful we
 * could do with the newly freed resources.
 */
static err_t anylocplay(CHANPAR chanpar, unsigned numchan, unsigned delay, int quiet)
{
 struct cmd_state cmd_state;
 struct timeb laststs;
#ifdef PROSODY_TiNG
 tSMChannelId bgchan;
#else
 tSMChannelId bgchan = kSMNullChannelId;
#endif
 tSMEventId anyev;
 ALLCHAN ac;
 err_t err;
 RANDMT ran;
 randmt(&ran, 1);
 cmd_state.waiting = 0;
 cmd_state.needsts = 1;
 cmd_state.eof = 0;
 ac.nchan = 0;
 ac.chanlist = 0;
 err = alloc_event(&anyev);
 if (err) return error(err, "cannot setup 'any channel' event");
#ifdef PROSODY_TiNG
 err = setup_background(&bgchan, &chanpar, chanpar.background);
 if (err) return error(err, "cannot setup background channel");
#endif
//#ifdef TiNGTYPE_WINNT
// if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL)) {
//	return error(error_last("SetThreadPriority() failed"),
//		"Cannot increase priority of main thread");
// }
//#endif
 for (;;) {
	if (cmd_state.needsts || time_since_mS(laststs) >= 1000) {
		cmd_state.needsts = 1;
		putchar('\r');
		if (!quiet) {
			showsts(&ac);
			putchar(')');
			putchar('>');
		}
		ftime(&laststs);
	}
	if (numchan) {
		err = newchan(&ac, anyev, &chanpar, bgchan, randmt_real1(&ran) * delay);
		if (err) return err;
		numchan--;
		putchar('\n');
		err = checkchans(&ac);
		if (err) return error(err, "Cannot check channels");
		cmd_state.needsts = 0;
	} else {
		int stdin_ready, chan_ready;
#ifdef USE_POLL
		struct pollfd pfd[2];
		int i;
#endif
#ifdef USE_WFMO
		HANDLE h[2];
		DWORD n;
#endif
		if (cmd_state.needsts) {
			static int twiddle;
			putchar("-\\|/"[twiddle++]);
			twiddle &= 3;
			fflush(stdout);
			cmd_state.needsts = 0;
			if (cmd_state.waiting) putchar('W');
		}
		if (cmd_state.waiting) {
			if (!ac.nchan) {
				putchar('\n');
				cmd_state.waiting = 0;
			}
		}
		fflush(stdout);
#ifdef USE_POLL
		pfd[0].fd = anyev.fd;
		pfd[0].events = anyev.mode;
		pfd[1].fd = 0;
		pfd[1].events = POLLIN|POLLPRI;
		i = poll(pfd, 1 + !cmd_state.waiting, 1000);
		if (i < 0) {
			return error(error_errno(errno, "poll()failed"),
				"Cannot wait for channels");
		}
		chan_ready = pfd[0].revents;
		stdin_ready = !cmd_state.waiting && pfd[1].revents;
#endif
#ifdef USE_WFMO
		h[0] = anyev;
		h[1] = GetStdHandle(STD_INPUT_HANDLE);
		n = WaitForMultipleObjects(1 + !cmd_state.waiting, h, 0, 1000);
		stdin_ready = 0;
		chan_ready = 0;
		switch (n) {
			err_t e;
		case WAIT_OBJECT_0:
			chan_ready = 1;
			break;
		case WAIT_OBJECT_0 + 1:
			e = is_stdin(&stdin_ready);
			if (e) return e;
			break;
		case WAIT_TIMEOUT:
			break;
		default:
			return error(
				error_last("WaitForMultipleObjects() failed"),
				"Cannot wait for channels");
		}
#endif
		if (stdin_ready) {
			cmd_state.needsts = 1;
			err = do_line(&ac, anyev, &chanpar, bgchan, &cmd_state);
			if (err) return err;
			if (cmd_state.eof) {
				smd_ev_free(anyev);
				return 0;
			}
		}
		if (chan_ready) {
			err = checkchans(&ac);
			if (err) return error(err, "Cannot check channels");
		}
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

#include "gen/anylocplay.args.i"

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
 chanpar.agc = arg.agc;
 chanpar.once = ONCE_NOT;
 if (arg.once) {
 	chanpar.once = ONCE_ONLY;
	if (arg.restart) {
		fprintf(stderr, "Error: both 'once' and 'restart' requested\n");
		return 1;
	}
 }
//#ifdef TiNGTYPE_WINNT
// if (_setmaxstdio(2048) < 0) {
//	fprintf(stderr, "_setmaxstdio(2048) failed\n");
//	return 1;
// }
//#endif
 if (arg.restart) chanpar.once = ONCE_REPEAT;
 chanpar.speed = arg.speed;
 chanpar.volume = arg.volume;
 chanpar.cards = &cards;
#ifdef PROSODY_TiNG
 chanpar.background = arg.background;
 chanpar.timeslot = arg.timeslot;
#endif
 chanpar.fname = *argv;
 chanpar.max_octets = arg.maxoctets;
 chanpar.per_mod = arg.per_mod;
 chanpar.thismod = 0;
 err = anylocplay(chanpar, arg.numchan, arg.randdelay, arg.quiet);
 if (err) {
	error_log(stderr, err);
	return 1;
 }
 return 0;
}
