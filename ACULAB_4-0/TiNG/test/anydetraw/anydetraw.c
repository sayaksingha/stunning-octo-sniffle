/* anydetraw.c - test/demo of 'any channel' detecting tones */

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

#include "smbesp.h"
#include "smdrvr.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

#include "../Testlib/errcode_sm.h"
#include "../Testlib/error.h"
#include "../Testlib/cardopen.h"
#include "../Testlib/modopen.h"

typedef struct {
	int stream;
	int timeslot;
	int type;
} MVIP;
#define MVIP_STREAM_NONE 0xff

typedef enum {TONE_TYPE_START, TONE_TYPE_END, TONE_TYPE_LEN} TONE_TYPE;

typedef struct {
	unsigned nchan;
	struct chan {
		tSMChannelId chan;
		MVIP xts;
		unsigned toneset;
		unsigned grunt;
		unsigned map;
		unsigned cptone;
		unsigned tonemode;
		TONE_TYPE tonetype;
		int catsig_alg;
		struct timeb lastout;
	} **chanlist;

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

#ifdef TiNGTYPE_WINNT
static const char winderr[] = {
	"Windows error %ld and got error %d trying to get text for this error"
};

static err_t error_last(char *text)
{
 DWORD gle = GetLastError();
 char *bp;
 long e2 = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
 	0,
 	gle,
 	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
 	(LPTSTR) &bp,
 	0,
 	0);
 err_t err;
 if (!e2) {
	bp = malloc(sizeof(winderr) + 40);
	if (bp) sprintf(bp, winderr, gle, GetLastError());
 	err = error(0, bp);
 	free(bp);
 } else {
 	int i = strlen(bp);
 	if (i && bp[--i] == '\n') bp[i] = 0;
 	err = error(0, bp);
 	LocalFree(bp);
 }
 return error(err, text);
}
#endif


static err_t fn_sm_switch_channel_input(tSMChannelId chan, struct chan *cp)
{
 struct sm_switch_channel_parms sw;
 int err;
 memset(&sw, 0, sizeof(sw));
 sw.channel = chan;
 sw.st = cp->xts.stream;
 sw.ts = cp->xts.timeslot;
 sw.type = cp->xts.type;
 err = sm_switch_channel_input(&sw);
 if (err) return prosody_error(err, "sm_switch_channel_input() failed");
 return 0;
}

static err_t detectloop(ALLCHAN *acp)
{
 for (;;) {
	SM_RECOGNISED_PARMS rp;
	unsigned long dtim;
	struct timeb now;
	const char *type;
	struct chan *cp;
	char buff[80];
	unsigned ci;
	int err;
	memset(&rp, 0, sizeof(rp));
	rp.channel = kSMNullChannelId;
	err = sm_get_recognised(&rp);
	if (err == ERR_SM_NO_SUCH_CHANNEL) return 0;
	if (err) return prosody_error(err, "sm_get_recognised() failed");
	if (rp.channel == kSMNullChannelId) return 0;
	ftime(&now);
	err = sm_get_channel_ix(rp.channel);
	if (err < 0) return prosody_error(err, "sm_get_channel_ix() failed");
	ci = err;
	if (ci >= acp->nchan) abort();
	cp = acp->chanlist[ci];
	dtim = now.millitm - cp->lastout.millitm;
	dtim += (now.time - cp->lastout.time) * 1000;
	switch (rp.type) {
	case kSMRecognisedNothing: type = 0; break;
	case kSMRecognisedTrainingDigit: type = "TrainingDigit"; break;
	case kSMRecognisedDigit:
		sprintf(buff, "Digit: %d (%c) %d", rp.param0, rp.param0, rp.param1);
		type = buff;
		break;
	case kSMRecognisedTone: type = "Tone"; break;
	case kSMRecognisedCPTone: type = "CPTone"; break;
	case kSMRecognisedGruntStart: type = "GruntStart"; break;
	case kSMRecognisedGruntEnd: type = "GruntEnd"; break;
	case kSMRecognisedASRResult: type = "ASRResult"; break;
	case kSMRecognisedASRUncertain: type = "ASRUncertain"; break;
	case kSMRecognisedASRRejected: type = "ASRRejected"; break;
	case kSMRecognisedASRTimeout: type = "ASRTimeout"; break;
	case kSMRecognisedCatSig: type = "CatSig"; break;
	case kSMRecognisedOverrun: type = "Overrun"; break;
	default:
		fprintf(stderr, "sm_get_recognised produced event %d\n", rp.type);
		return prosody_error(0, "unknown event from sm_get_recognised");
	}
	if (type) {
		cp->lastout = now;
		printf("+%ld mS\t%s: %d %d\n", dtim, type, rp.param0, rp.param1);
	}
 }
}

static err_t detectchan(tSMModuleId mod, struct chan *cp, tSMEventId anyev)
{
 struct sm_channel_alloc_placed_parms ap;
 int err;
 // detect on timeslot xp.stream:xp.ts
 memset(&ap, 0, sizeof(ap));
 ap.module = mod;
 ap.type = kSMChannelTypeInput;
 err = sm_channel_alloc_placed(&ap);
 if (err) {
 	fprintf(stderr, "sm_channel_alloc_placed returned %d\n", err);
 } else {
	cp->chan = ap.channel;
	if (!fn_sm_switch_channel_input(ap.channel, cp)) {
		struct sm_listen_for_parms lp;
		memset(&lp, 0, sizeof(lp));
		lp.channel = ap.channel;
		if (cp->grunt != ~0U) {
			lp.enable_grunt_detection = 1;
			lp.grunt_latency = cp->grunt;
		}
		lp.active_tone_set_id = cp->toneset;
		lp.map_tones_to_digits = cp->map;
		lp.enable_cptone_recognition = cp->cptone;
		switch (cp->tonemode) {
		case 0:
			break;
		case 1:
			switch (cp->tonetype) {
			case TONE_TYPE_START:
				lp.tone_detection_mode = 
					kSMToneDetectionNoMinDuration;
				break;
			case TONE_TYPE_END:
				lp.tone_detection_mode = 
					kSMToneEndDetectionNoMinDuration;
					break;
			case TONE_TYPE_LEN:
				lp.tone_detection_mode = 
					kSMToneLenDetectionNoMinDuration;
					break;
			}
			break;
		case 40:
			switch (cp->tonetype) {
			case TONE_TYPE_START:
				lp.tone_detection_mode = 
					kSMToneDetectionMinDuration40;
				break;
			case TONE_TYPE_END:
				lp.tone_detection_mode = 
					kSMToneEndDetectionMinDuration40;
				break;
			case TONE_TYPE_LEN:
				lp.tone_detection_mode = 
					kSMToneLenDetectionMinDuration40;
				break;
			}
			break;
		case 64:
			switch (cp->tonetype) {
			case TONE_TYPE_START:
				lp.tone_detection_mode = 
					kSMToneDetectionMinDuration64;
				break;
			case TONE_TYPE_END:
				lp.tone_detection_mode = 
					kSMToneEndDetectionMinDuration64;
				break;
			case TONE_TYPE_LEN:
				lp.tone_detection_mode = 
					kSMToneLenDetectionMinDuration64;
				break;
			}
			break;
		default:
			fprintf(stderr, "Tone mode %d ignored\n", cp->tonemode);
			break;
		}
		err = sm_listen_for(&lp);
		ftime(&cp->lastout);
		if (err) fprintf(stderr, "sm_listen_for returned %d\n", err);
		if (!err && cp->catsig_alg) {
			struct sm_catsig_listen_for_parms clp;
			memset(&clp, 0, sizeof(clp));
			clp.channel = ap.channel;
			clp.catsig_alg_id = cp->catsig_alg;
			err = sm_catsig_listen_for(&clp);
			if (err) fprintf(stderr, "sm_catsig_listen_for returned %d\n", err);
		}
		if (!err) {
			SM_CHANNEL_SET_EVENT_PARMS ep;
			memset(&ep, 0, sizeof(ep));
			ep.channel = ap.channel;
			ep.event_type = kSMEventTypeRecog;
			ep.issue_events = kSMAnyChannelEvent;
			ep.event = anyev;
			err = sm_channel_set_event(&ep);
			if (err) fprintf(stderr, "sm_channel_set_event returned %d\n", err);
			else return 0;
		}
 	}
 	sm_channel_release(ap.channel);
 }
 return prosody_error(err, "detectchan() failed");
}

static err_t allchan_add(ALLCHAN *ac, struct chan *cp)
{
 int e = sm_get_channel_ix(cp->chan);
 unsigned ci;
 if (e < 0) return prosody_error(e, "sm_get_channel_ix() failed");
 ci = e;
 if (ci >= ac->nchan) {
	unsigned newmax = ci + 1;
	struct chan **newlist = realloc(ac->chanlist,
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

#if 0
static err_t allchan_del(ALLCHAN *ac, struct chan *cp)
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
#endif

struct cmd_state {
	int waiting;
	int eof;
};

static err_t do_line(unsigned *ncp, struct cmd_state *cmd_state)
{
 for (;;) {
	int c = getchar();
	switch (c) {
	case '\n': return 0;
	case '+': ++*ncp; break;
	case 'w':
		cmd_state->waiting = 1;
		break;
	case 'q':
	case EOF:
		printf("exitting\n");
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

static err_t anydetraw(tSMModuleId mod, unsigned numchan, MVIP ts, unsigned toneset, unsigned grunt, unsigned map, unsigned cptone, unsigned tonemode, TONE_TYPE tonetype, unsigned catsig_alg)
{
 struct cmd_state cmd_state;
 struct timeb laststs;
 tSMEventId anyev;
 ALLCHAN ac;
 int iErr;
 err_t err;
 ftime(&laststs);
 iErr = smd_ev_create(&anyev, kSMNullChannelId, kSMEventTypeRecog, kSMAnyChannelEvent);
 if (iErr) fprintf(stderr, "smd_ev_create returned %d\n", iErr);
 cmd_state.waiting = 0;
 cmd_state.eof = 0;
 ac.nchan = 0;
 ac.chanlist = 0;
 for (;;) {
 	if (numchan) {
 		struct chan *cp = malloc(sizeof(*cp));
		if (!cp) {
			perror("malloc");
			return prosody_error(1, "Cannot allocate channel struct");
		}
		cp->xts = ts;
		cp->toneset = toneset;
		cp->grunt = grunt;
		cp->map = map;
		cp->cptone = cptone;
		cp->tonemode = tonemode;
		cp->tonetype = tonetype;
		cp->catsig_alg = catsig_alg;
		if (++ts.timeslot == 32) {
			ts.timeslot = 0, ts.stream++;
		}
		err = detectchan(mod, cp, anyev);
		if (!err) err = allchan_add(&ac, cp);
		numchan--;
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
		if (time_since_mS(laststs) >= 1000) {
			static int twiddle;
			putchar('\r');
			putchar("-\\|/"[twiddle++]);
			twiddle &= 3;
			fflush(stdout);
			if (cmd_state.waiting) putchar('W');
			ftime(&laststs);
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
			err = do_line(&numchan, &cmd_state);
			if (err) return err;
			if (cmd_state.eof) return 0;
		}
		if (chan_ready) {
			err = detectloop(&ac);
			if (err) return error(err, "Cannot check channels");
		}
	}
 }
}

#include "gen/anydetraw.args.i"

int main(int argc, char **argv)
{
 tSMModuleId mod;
 TONE_TYPE tonetype = TONE_TYPE_START;
 err_t err;
 tSMCardId card_id;
 ARGS_DECL
 (void) argc;
 arg.grunt = ~0;
 if (ARGS_CALL || *argv) {
 usage:
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
	return 1;
 }
 if (arg.type) {
 	switch (arg.type[0]) {
 	case 'e':
 	case 'E':
 		tonetype = TONE_TYPE_END; break;
 	case 'l':
 	case 'L':
 		tonetype = TONE_TYPE_LEN; break;
 	case 's':
 	case 'S':
 		tonetype = TONE_TYPE_START; break;
 	default:
 	toneusage:
 		fprintf(stderr, "'-T' option takes one character:\n");
 		fprintf(stderr, "\tS - report tone start\n");
 		fprintf(stderr, "\tE - report tone end\n");
 		fprintf(stderr, "\tL - report tone length\n");
 		fprintf(stderr, "not '%s'\n", arg.type);
 		goto usage;
 	}
 	if (arg.type[1]) goto toneusage;
 }
 if (arg.serialnumber) {
	err_t e = modopen(&card_id, &mod, arg.serialnumber, arg.module);
	if (e) {
		error_log(stderr, e);
		return 1;
	}
 } else {
	fprintf(stderr, "Serial number is required\n Usage %s" ARGS_USAGE "\n",progname);
 }

 if (arg.grunt == ~0U && !arg.cptone && !arg.tonemode && !arg.catsig_alg) {
	printf("Warning: no detection selected\n");
 }
 err = anydetraw(mod, arg.numchan, arg.timeslot, arg.toneset, arg.grunt, arg.map, arg.cptone, arg.tonemode, tonetype, arg.catsig_alg);
 if(err) {
	error_log(stderr, err);
 }
 if (arg.serialnumber) {
 	err_t e = modclose(mod);
	if (e) error_log(stderr, e);
	e = cardclose(card_id);
	if (e) error_log(stderr, e);
 }
 return 0;
}
