/* anylocrec.c - demonstration of 'any channel' record on local timeslots */

#include <errno.h>
#include <pthread.h>
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

#include "../../libutil/generic_io.h"
#include "../Testlib/error.h"
#include "../Testlib/fmtcode.h"
#include "../Testlib/errcode_sm.h"
#include "../Testlib/cardlist.h"
#include "smbesp.h"
#include "smdrvr.h"

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
	char *fname;
	unsigned curcard;
	unsigned curmod;
	CARDLIST *cards;
	MVIP timeslot;
	enum kSMDataFormat format;
	tSM_UT32 rate;
	unsigned max_octets;
	unsigned max_silence;
	unsigned max_elapsed;
	unsigned thresh;
	int agc;
	int volume;
	unsigned per_mod;
	unsigned thismod;
} CHANPAR;

typedef struct running_channel {
	tSMChannelId chan;
	FILE *fp;
		/*
		 *	-	closed
		 *	*	data
		 *	u	underrun
		 *	.	idle
		 */
	char status;	// for status display
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
 if (ac->chanlist[ci]) abort();
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
		if (rcp->status == '-') {
			allchan_del(acp, rcp);
			sm_channel_release(rcp->chan);
			fclose(rcp->fp);
			free(rcp);
		} else if (rcp->status == '*') rcp->status = '.';
		lastp = u;
	}
 }
 return 0;
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

	// we have data on a channel - fetch it
static err_t handle_data(RUNNING_CHANNEL *rcp)
{
 SM_TS_DATA_PARMS dp;
 char buff[2048];
 size_t nc;
 int e;
 memset(&dp, 0, sizeof(dp));
 dp.channel = rcp->chan;
 dp.data = buff;
 e = sm_get_recorded_data(&dp);
 if (e) return prosody_error(e, "sm_get_recorded_data() failed");
 nc = fwrite(buff, 1, dp.length, rcp->fp);
 if (ferror(rcp->fp)) {
	return error(error_errno(errno, "fwrite()failed"),
		"Cannot write recorded data file");
 }
 if (rcp->status == '.') rcp->status = '*';
 return 0;
}
	// handle all channels which have triggered the event
static err_t checkchans(ALLCHAN *acp)
{
 for (;;) {
	SM_RECORD_STATUS_PARMS rp;
	RUNNING_CHANNEL *rcp;
	unsigned ci;
	int e;
	memset(&rp, 0, sizeof(rp));
	rp.channel = 0;
	e = sm_record_status(&rp);
	if (e == ERR_SM_NO_SUCH_CHANNEL) return 0;
	if (e) return prosody_error(e, "sm_record_status() failed");
	e = sm_get_channel_ix(rp.channel);
	if (e < 0) return prosody_error(e, "sm_get_channel_ix() failed");
	ci = e;
	if (ci >= acp->nchan) abort();
	rcp = acp->chanlist[ci];
	switch (rp.status) {
		err_t err;
	case kSMRecordStatusOverrun:
		rcp->status = 'u';
		break;
	case kSMRecordStatusComplete:
		rcp->status = '-';
		break;
	case kSMRecordStatusCompleteData:
	case kSMRecordStatusData:
		err = handle_data(rcp);
		if (err) return err;
		break;
	case kSMRecordStatusRecognition:
	case kSMRecordStatusNoData:
		break;
	}
 }
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

	// open a file for writing
static err_t open_file(RUNNING_CHANNEL *rcp, char *fname)
{
 err_t err = 0;
 rcp->fp = fopen(fname, "wb");
 if (!rcp->fp) {
	char *n = malloc(strlen(fname) + sizeof("File: ''"));
	if (n) sprintf(n, "File: '%s'", fname);
	err = error_errno(errno, "fopen() failed");
	err = error(err, n);
	err = error(err, "Cannot open file");
	free(n);
 }
 return err;
}

	// allocate a Prosody channel
static err_t alloc_chan(RUNNING_CHANNEL *rcp, tSMModuleId module)
{
 SM_CHANNEL_ALLOC_PLACED_PARMS ap;
 int e;
 memset(&ap, 0, sizeof(ap));
 ap.type = kSMChannelTypeInput;
 ap.module = module;
 e = sm_channel_alloc_placed(&ap);
 if (e) return prosody_error(e, "sm_channel_alloc_placed() failed");
 rcp->chan = ap.channel;
 return 0;
}

	// assign a specified timeslot
static err_t assign_timeslot(RUNNING_CHANNEL *rcp, MVIP xts)
{
 SM_SWITCH_CHANNEL_PARMS sw;
 int e;
 memset(&sw, 0, sizeof(sw));
 sw.channel = rcp->chan;
 sw.st = xts.stream;
 sw.ts = xts.timeslot;
 sw.type = xts.type;
 e = sm_switch_channel_input(&sw);
 if (e) return prosody_error(e, "sm_switch_channel_input() failed");
 return 0;
}

	// set the desired data transfer threshold
static err_t set_threshold(RUNNING_CHANNEL *rcp, unsigned thresh)
{
 if (thresh) {		// value zero leaves default setting
	SM_CHANNEL_SET_INPUT_THRESHOLD_PARMS tp;
	int e;
	memset(&tp, 0, sizeof(tp));
	tp.channel = rcp->chan;
	tp.minimum_bits = thresh * 8;
	e = sm_channel_set_input_threshold(&tp);
	if (e) return prosody_error(e, "sm_channel_set_input_threshold() failed");
 }
 return 0;
}

	// associate event with channel
static err_t associate_event(RUNNING_CHANNEL *rcp, tSMEventId ev)
{
 SM_CHANNEL_SET_EVENT_PARMS sep;
 int e;
 memset(&sep, 0, sizeof(sep));
 sep.event = ev;
 sep.channel = rcp->chan;
 sep.event_type = kSMEventTypeReadData;
 sep.issue_events = kSMAnyChannelEvent;
 e = sm_channel_set_event(&sep);
 if (e) return prosody_error(e, "sm_channel_set_event() failed");
 return 0;
}

	// start recording
static err_t start_record(RUNNING_CHANNEL *rcp, CHANPAR chanpar)
{
 struct sm_record_parms rp;
 int e;
 memset(&rp, 0, sizeof(rp));
 rp.channel = rcp->chan;
 rp.volume = chanpar.volume;
 rp.agc = chanpar.agc;
 rp.type = chanpar.format;
 rp.sampling_rate = chanpar.rate;
 rp.max_silence = chanpar.max_silence;
 rp.max_elapsed_time = chanpar.max_elapsed;
 rp.max_octets = chanpar.max_octets;
 e = sm_record_start(&rp);
 if (e) return prosody_error(e, "sm_record_start() failed");
 return 0;
}

	// start a new record
static err_t record(ALLCHAN *acp, tSMEventId ev, CHANPAR chanpar)
{
 RUNNING_CHANNEL *rcp;
 err_t err = alloc_running(&rcp);
 if (!err) {
	rcp->status = '.';
	err = open_file(rcp, chanpar.fname);
	if (!err) {
		MODDESC mod;
		err = cardlist_getmod(&mod, chanpar.cards, chanpar.curcard, chanpar.curmod);
		if(!err) err = alloc_chan(rcp, mod.id);
		if (!err) {
			err = assign_timeslot(rcp, chanpar.timeslot);
			if (!err) {
				err = set_threshold(rcp, chanpar.thresh);
				if (!err) {
					err = associate_event(rcp, ev);
					if (!err) {
						err = start_record(rcp, chanpar);
						if (!err) {
							err = allchan_add(acp, rcp);
							if (!err) return 0;
						}
					}
				}
			}
			sm_channel_release(rcp->chan);
		}
		fclose(rcp->fp);
	}
	free(rcp);
 }
 return error(err, "Cannot record file");
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

	// allocate an event
static err_t alloc_event(tSMEventId *evp)
{
 int e = smd_ev_create(evp, kSMNullChannelId, kSMEventTypeReadData, kSMAnyChannelEvent);
 if (e) return prosody_error(e, "smd_ev_create() failed");
 return 0;
}

static err_t newchan(ALLCHAN *acp, tSMEventId anyev, CHANPAR *chanpar)
{
 err_t err;
 printf("starting record on %d:%d\n",
	chanpar->timeslot.stream, chanpar->timeslot.timeslot);
 err = record(acp, anyev, *chanpar);
 if (err) return err;
 if (! (++chanpar->timeslot.timeslot & 0x1f)) {
	chanpar->timeslot.stream++;
	chanpar->timeslot.timeslot -= 32;
 }
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

static err_t do_line(ALLCHAN *acp, tSMEventId anyev, CHANPAR *chanpar, struct cmd_state *cmd_state)
{
 for (;;) {
	int c = getchar();
	switch (c) {
		err_t err;
		unsigned u;
	case '\n': return 0;
	case '+':	// start new channel
		err = newchan(acp, anyev, chanpar);
		if (err) return err;
		cmd_state->needsts = 1;
		break;
	case '?':	// help
		printf("Commands:\n"
			"\t+\tstart record with current agc, and volume\n"
			"\t#...\tcomment\n"
			"\tTN\tset TiNGtrace to N\n"
			"\taN\tset agc (0=off, 1=on)\n"
			"\tkN\tkill channel N\n"
			"\tKN\tkill channel N, discarding uncollected data\n"
			"\tsN\tset max silence\n"
			"\ttN\tset timeslot\n"
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
	case 'K':	//   with discard
		u = readint();
		if (u >= acp->nchan || !acp->chanlist[u]) {
			printf("Channel %d not in use\n", u);
			break;
		}
		err = abort_record(acp->chanlist[u]->chan, c == 'K');
		if (err) error_log(stderr, err);
		cmd_state->needsts = 1;
		break;
	case 'l':	// max elapsed time
		chanpar->max_elapsed = readint();
		break;
	case 's':	// max silence
		chanpar->max_silence = readint();
		break;
	case 'T':	// Trace
		TiNGtrace = readint();
		break;
	case 't':	// timeslot
		err = readts(&chanpar->timeslot);
		if (err) error_log(stderr, err);
		break;
	case 'U':
		for (u=0; u < acp->nchan; u++) {
			RUNNING_CHANNEL *rcp = acp->chanlist[u];
			if (rcp && rcp->status == 'u') rcp->status = '.';
		 }
		break;
	case 'v':	// volume
		chanpar->volume = readint();
		break;
	case 'x':	// max octets
		chanpar->max_octets = readint();
		break;
	case 'w':
		cmd_state->waiting = 1;
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

static err_t anylocrec(CHANPAR chanpar, unsigned numchan)
{
 struct cmd_state cmd_state;
 struct timeb laststs;
 tSMEventId anyev;
 ALLCHAN ac;
 err_t err;
 cmd_state.waiting = 0;
 cmd_state.needsts = 1;
 cmd_state.eof = 0;
 ac.nchan = 0;
 ac.chanlist = 0;
 err = alloc_event(&anyev);
 if (err) return error(err, "cannot setup 'any channel' event");
 for (;;) {
	if (cmd_state.needsts || time_since_mS(laststs) >= 1000) {
		cmd_state.needsts = 1;
		putchar('\r');
		showsts(&ac);
		putchar(')');
		putchar('>');
		ftime(&laststs);
	}
	if (numchan) {
		err = newchan(&ac, anyev, &chanpar);
		if (err) return err;
		numchan--;
		putchar('\n');
		cmd_state.needsts = 1;
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
				error_last("WaitForMultipleObjects()failed"),
				"Cannot wait for channels");
		}
#endif
		if (stdin_ready) {
			cmd_state.needsts = 1;
			err = do_line(&ac, anyev, &chanpar, &cmd_state);
			if (err) return err;
			if (cmd_state.eof) return 0;
		}
		if (chan_ready) {
			err = checkchans(&ac);
			if (err) return error(err, "Cannot check channels");
		}
	}
 }
}

#include "gen/anylocrec.args.i"

int main(int argc, char **argv)
{
 CARDLIST cards;
 CHANPAR chanpar;
 err_t err;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || !*argv || argv[1]) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE " file\nFormats:", progname);
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
 chanpar.volume = arg.volume;
 chanpar.cards = &cards;
 chanpar.timeslot = arg.timeslot;
 chanpar.fname = *argv;
 chanpar.max_silence = arg.max_silence;
 chanpar.max_elapsed = arg.max_elapsed_time;
 chanpar.max_octets = arg.maxoctets;
 chanpar.thresh = arg.threshold;
 chanpar.per_mod = arg.per_mod;
 chanpar.thismod = 0;
 err = anylocrec(chanpar, arg.numchan);
 if (err) {
	error_log(stderr, err);
	return 1;
 }
 return 0;
}
