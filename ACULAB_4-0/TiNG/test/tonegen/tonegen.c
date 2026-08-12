/* tonegen.c - test/demo of generating tones */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "smbesp.h"
#include "smdrvr.h"

#include "../Testlib/cardlist.h"
#include "../Testlib/error.h"
#include "../Testlib/eventset.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

#ifdef TiNGTYPE_WINNT
#include "../../libutil/WINNT/lasterr.h"
typedef HANDLE WAITABLE;
#endif

typedef struct {
	int stream;
	int timeslot;
	int type;
} MVIP;
#define MVIP_STREAM_NONE 0xff

typedef struct {
	CARDLIST *cards;
	MVIP timeslot;
} CHANPAR;

struct appactive {
	tSMChannelId chan;
};

static tSMEventId anyev;
static pthread_t anychan_tid;
static int have_anyev;
static EVENTSET evset;
static int have_evset;
static pthread_t evset_tid;

	// the worker thread for the per-channel eventset is this function
static void *perchan_worker_thread(void *p)
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

	// start the per-channel eventset worker thread
static err_t perchan_startworker(pthread_t *tidp, EVENTSET *evs)
{
 err_t err = eventset(evs);
 int e;
 if (err) return err;
 e = pthread_create(tidp, 0, perchan_worker_thread, evs);
 if (e) {
 	err = error_errno(e, "pthread_create() failed");
 	err = error(err, "cannot start worker thread");
 }
 return err;
}

static int releasechan(tSMChannelId chan)
{
 int err = sm_channel_release(chan);
 if (err) fprintf(stderr, "sm_channel_release returned %d\n", err);
 return err;
}

static int check_tone_status(void)
{
 SM_PLAY_TONE_STATUS_PARMS pts;
 int e;
 memset(&pts, 0, sizeof(pts));
 e = sm_play_tone_status(&pts);
 if (e == ERR_SM_NO_SUCH_CHANNEL) return 0;
 if (e) {
 	fprintf(stderr, "sm_play_tone_status() returned %d\n", e);
 	return 0;
 }
 if (pts.channel && pts.status == kSMPlayToneStatusComplete) {
	releasechan(pts.channel);
 }
 return 0;
}

static int check_cptone_status(void)
{
 SM_PLAY_CPTONE_STATUS_PARMS pts;
 int e;
 memset(&pts, 0, sizeof(pts));
 e = sm_play_cptone_status(&pts);
 if (e == ERR_SM_NO_SUCH_CHANNEL) return 0;
 if (e) {
 	fprintf(stderr, "sm_play_cptone_status() returned %d\n", e);
 	return 0;
 }
 if (pts.channel && pts.status == kSMPlayCPToneStatusComplete) {
	releasechan(pts.channel);
 }
 return 0;
}

static int check_digits_status(void)
{
 SM_PLAY_DIGITS_STATUS_PARMS pts;
 int e;
 memset(&pts, 0, sizeof(pts));
 e = sm_play_digits_status(&pts);
 if (e == ERR_SM_NO_SUCH_CHANNEL) return 0;
 if (e) {
 	fprintf(stderr, "sm_play_digits_status() returned %d\n", e);
 	return 0;
 }
 if (pts.channel && pts.status == kSMPlayDigitsStatusComplete) {
	releasechan(pts.channel);
 }
 return 0;
}

	// the worker thread for anychannel is this function
static void *anychan_worker_thread(void *p)
{
 (void) p;
#ifdef TiNGTYPE_WINNT
 if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS)) {
	error_log(stderr, error_last("SetPriorityClass"));
	return (void *) 1;
 }
#endif
 for (;;) {
	int e = smd_ev_wait(anyev);
	if (e) {
		fprintf(stderr, "smd_ev_wait() returned %d\n", e);
		have_anyev = 0;
		return (void *) 1;
	}
	if (check_tone_status()
		|| check_cptone_status()
		|| check_digits_status()) {
		have_anyev = 0;
		return (void *) 1;
	}
 }
 return 0;
}

	// start the anychannel worker thread
static err_t anychan_startworker(pthread_t *tidp)
{
 int e = pthread_create(tidp, 0, anychan_worker_thread, 0);
 if (e) {
 	err_t err = error_errno(e, "pthread_create() failed");
 	err = error(err, "cannot start worker thread");
 	return err;
 }
 return 0;
}

static void cleanup_active(ACTIVE_JOB *ap)
{
 int err = smd_ev_free(ap->event);
 if (err) {
	fprintf(stderr, "smd_ev_free() returned %d\n", err);
 }
 releasechan(ap->mf->chan);
 free(ap->mf);
 free(ap);
}

static void waitins(tSMChannelId chan, int (*check_fn)(ACTIVE_JOB *), int waitfor)
{
 SM_CHANNEL_SET_EVENT_PARMS ep;
 int err = 0;
 if (waitfor == 'w')  {
	releasechan(chan);
	return;
 }
 memset(&ep, 0, sizeof(ep));
 ep.channel = chan;
 ep.event_type = kSMEventTypeWriteData;
 ep.issue_events = waitfor == 'a' ? kSMAnyChannelEvent : kSMChannelSpecificEvent;
 if (waitfor == 'a' && have_anyev) {
	ep.event = anyev;
 } else {
 	err = smd_ev_create(&ep.event, chan, kSMEventTypeWriteData, ep.issue_events);
 }
 if (err) {
	fprintf(stderr, "smd_ev_create() returned %d\n", err);
	return;
 }
 if (waitfor == 'a' && !have_anyev) {
	err_t e;
	anyev = ep.event;
	e = anychan_startworker(&anychan_tid);
	if (e) {
		error_log(stderr, e);
		return;
	}
	have_anyev = 1;
 }
 err = sm_channel_set_event(&ep);
 if (err) {
	fprintf(stderr, "sm_channel_set_event() returned %d\n", err);
	return;
 }
 if (waitfor == 'c') {
	ACTIVE_JOB *ap = malloc(sizeof(*ap));
	err_t e;
	if (!ap) {
		perror("malloc() failed");
		fprintf(stderr, "Cannot allocate ACTIVE_JOB\n");
		return;
	}
	ap->mf = malloc(sizeof(*ap->mf));
	if (!ap->mf) {
		perror("malloc() failed");
		fprintf(stderr, "Cannot allocate ACTIVE_JOB appactive\n");
		free(ap);
		return;
	}
	ap->event = ep.event;
	ap->handler = check_fn;
	ap->cleanup_fn = cleanup_active;
	ap->mf->chan = chan;
	if (!have_evset) {
		e = eventset(&evset);
		if (e) {
			error_log(stderr, e);
			return;
		}
		e = perchan_startworker(&evset_tid, &evset);
		if (e) {
			error_log(stderr, e);
			return;
		}
		have_evset = 1;
	}
	e = eventset_insert(&evset, ap);
	if (e) {
		error_log(stderr, e);
	}
 }
}

static int sksp(FILE *f)
{
 for (;;) {
	int c = getc(f);
	if (c != ' ' && c != '\t') return c;
 }
}

static int getnum(FILE *f)
{
 int c = sksp(f);
 int base = 10;
 int num = 0;
 int neg = 0;
 if (c == '-') c=getc(f), neg = 1;
 if (c == '0') {
 	c = getc(f);
 	if (c == 'x') {
 		base = 16;
		c = getc(f);
	} else base = 8;
 }
 for (;;) {
	int d;
	if (c >= '0' && c <= '9') d = c - '0';
	else if (c >= 'a' && c <= 'f') d = c - 'a' + 10;
	else break;
	if (d >= base) break;
	num = num*base + d;
	c = getc(f);
 }
 ungetc(c, f);
 return neg ? -num : num;
}

static double getfloat(FILE *f)
{
 int c = sksp(f);
 double num = 0.0;
 int neg = 0;
 if (c == '-') c=getc(f), neg = 1;
 for (;;) {
	if (c < '0' || c > '9') break;
	num = num*10.0 + c - '0';
	c = getc(f);
 }
 if (c == '.') {
	double frac = 1.0;
	for (;;) {
		c = getc(f);
		if (c < '0' || c > '9') break;
		frac *= 1.0/10.0;
		num += (c - '0') * frac;
	}
 }
 if (c == 'e' || c == 'E') {
 	int exp = getnum(f);
 	for (; exp > 0; exp--) num *= 10.0;
 	for (; exp < 0; exp++) num *= 1.0/10.0;
 } else ungetc(c, f);
 return neg ? -num : num;
}

static int getwait(FILE *f)
{
 int c = sksp(f);
 if (c == 'W') {
 	c = sksp(f);
 	switch (c) {
 	case 'c':
 	case 'a':
 	case 'w': return c;
	}
	printf("Bad wait type: %c", c);
	printf("Valid types are:\n"
		"\tw - wait\n"
		"\ta - don't wait - use 'any channel' mode\n"
		"\tc - don't wait - use 'per channel' mode\n"
		);
	ungetc(c, f);
	return 0;
 } else {
 	ungetc(c, f);
 	return 'w';
 }
}

static int fn_sm_switch_channel_output(tSMChannelId chan, MVIP *ts)
{
 struct sm_switch_channel_parms sw;
 int err;
 memset(&sw, 0, sizeof(sw));
 sw.channel = chan;
 sw.st = ts->stream;
 sw.ts = ts->timeslot;
 sw.type = ts->type;
 err = sm_switch_channel_output(&sw);
 if (err) fprintf(stderr, "sm_switch_channel_output returned %d\n", err);
 return err;
}

static tSMChannelId allocchan(MVIP *ts, tSMModuleId module)
{
 struct sm_channel_alloc_placed_parms ap;
 int err;
 memset(&ap, 0, sizeof(ap));
 ap.type = kSMChannelTypeOutput;
 ap.module = module;
 err = sm_channel_alloc_placed(&ap);
 if (err) {
 	fprintf(stderr, "sm_channel_alloc_placed returned %d\n", err);
 	return 0;
 }
 if (!fn_sm_switch_channel_output(ap.channel, ts)) return ap.channel;
 releasechan(ap.channel);
 return 0;
}

static void do_addfreq(FILE *f, tSMModuleId mod)
{
 struct sm_output_freq_parms fp;
 int err;
 memset(&fp, 0, sizeof(fp));
 fp.freq = getfloat(f);
 fp.amplitude = getfloat(f);
 fp.module = mod;
 err = sm_add_output_freq(&fp);
 if (err) {
	fprintf(stderr, "sm_add_output_freq returned %d\n", err);
 }
 printf("id = %d\n", fp.id);
}

static void do_addtone(FILE *f, tSMModuleId mod)
{
 struct sm_output_tone_parms fp;
 int err;
 memset(&fp, 0, sizeof(fp));
 fp.component1_id = getnum(f);
 fp.component2_id = getnum(f);
 fp.module = mod;
 err = sm_add_output_tone(&fp);
 if (err) {
	fprintf(stderr, "sm_add_output_tone returned %d\n", err);
 }
 printf("id = %d\n", fp.id);
}

static int check_tone(ACTIVE_JOB *ap)
{
 SM_PLAY_TONE_STATUS_PARMS ds;
 int err;
 memset(&ds, 0, sizeof(ds));
 ds.channel = ap->mf->chan;
 err = sm_play_tone_status(&ds);
 if (err) {
 	fprintf(stderr, "sm_play_tone_status returned %d\n", err);
 }
 if (err || ds.status == kSMPlayToneStatusComplete) {
	return 1;
 }
 return 0;
}

static void do_playtone(FILE *f, MVIP *ts, tSMModuleId module)
{
 struct sm_play_tone_parms fp;
 int waitfor;
 int err;
 memset(&fp, 0, sizeof(fp));
 fp.duration = getnum(f);
 fp.tone_id = getnum(f);
 waitfor = getwait(f);
 if (!waitfor) return;
 fp.wait_for_completion = waitfor == 'w';
 fp.channel = allocchan(ts, module);
 if (!fp.channel) return;
 err = sm_play_tone(&fp);
 if (err) {
	fprintf(stderr, "sm_play_tone returned %d\n", err);
 }
 waitins(fp.channel, check_tone, waitfor);
}

static int check_cptone(ACTIVE_JOB *ap)
{
 SM_PLAY_CPTONE_STATUS_PARMS ds;
 int err;
 memset(&ds, 0, sizeof(ds));
 ds.channel = ap->mf->chan;
 err = sm_play_cptone_status(&ds);
 if (err) {
 	fprintf(stderr, "sm_play_cptone_status returned %d\n", err);
 }
 if (err || ds.status == kSMPlayCPToneStatusComplete) {
	return 1;
 }
 return 0;
}

static void do_playcptone(FILE *f, MVIP *ts, tSMModuleId module)
{
 struct sm_play_cptone_parms fp;
 int waitfor;
 int err;
 int c;
 memset(&fp, 0, sizeof(fp));
 fp.duration = getnum(f);
 switch (c = sksp(f)) {
 case 'o': fp.type = kSMPlayCPToneTypeOneShot; break;
 case 'r': fp.type = kSMPlayCPToneTypeRepeat; break;
 case 'c': fp.type = kSMPlayCPToneTypeContinuous; break;
 default:
 	printf("expected 'o', 'r', or 'c', got '%c'\n", c);
 	ungetc(c, f);
 	return;
 }
 for (;;) {
	c = sksp(f);
	if (c != '=') {
		ungetc(c, f);
		break;
	}
	if (fp.tone_count == arlen(fp.cadences)) {
		printf("Error: too many tones in cadence\n");
		fp.tone_count--;
	}
	fp.cadences[fp.tone_count].tone_id = getnum(f);
	fp.cadences[fp.tone_count].on_cadence = getnum(f);
	fp.cadences[fp.tone_count].off_cadence = getnum(f);
	fp.tone_count++;
 }
 waitfor = getwait(f);
 if (!waitfor) return;
 fp.wait_for_completion = waitfor == 'w';
 fp.channel = allocchan(ts, module);
 if (!fp.channel) return;
 err = sm_play_cptone(&fp);
 if (err) {
	fprintf(stderr, "sm_play_cptone returned %d\n", err);
 }
 waitins(fp.channel, check_cptone, waitfor);
}

static int check_digits(ACTIVE_JOB *ap)
{
 SM_PLAY_DIGITS_STATUS_PARMS ds;
 int err;
 memset(&ds, 0, sizeof(ds));
 ds.channel = ap->mf->chan;
 err = sm_play_digits_status(&ds);
 if (err) {
 	fprintf(stderr, "sm_play_digits_status returned %d\n", err);
 	return 1;
 }
 if (err || ds.status == kSMPlayDigitsStatusComplete) {
	return 1;
 }
 return 0;
}

static void do_playdigits(MVIP *ts, tSMModuleId module, unsigned dur, unsigned idd, char *digits, int waitfor)
{
 struct sm_play_digits_parms fp;
 int err;
 memset(&fp, 0, sizeof(fp));
 fp.digits.type = kSMDTMFDigits;
 fp.digits.digit_duration = dur;
 fp.digits.inter_digit_delay = idd;
 memcpy(fp.digits.digit_string, digits, strlen(digits));
 fp.wait_for_completion = waitfor == 'w';
 fp.channel = allocchan(ts, module);
 if (!fp.channel) return;
 err = sm_play_digits(&fp);
 if (err) {
	fprintf(stderr, "sm_play_digits returned %d\n", err);
 }
 waitins(fp.channel, check_digits, waitfor);
}

static void cmd_playdigits(FILE *f, MVIP *ts, tSMModuleId module, int repeat)
{
 unsigned dur = getnum(f);
 unsigned idd = getnum(f);
 char digits[kSMMaxDigits + 1];
 int waitfor;
 unsigned i;
 int c;
 if ( (c = sksp(f)) != '"') {
	printf("Expected '\"', got '%c'\n", c);
	ungetc(c, f);
	return;
 }
 for (i=0;; i++) {
 	c = sksp(f);
 	if (c == '"') break;
 	if (c == EOF || c == '\n') {
 		printf("String has no closing quote\n");
 		ungetc(c, f);
 		return;
	}
	if (i >= arlen(digits) - 1) {
		printf("Digit string too long\n");
		i--;
	}
	digits[i] = c;
 }
 digits[i] = '\0';
 waitfor = getwait(f);
 if (repeat) {
 	for (;;) do_playdigits(ts, module, dur, idd, digits, waitfor);
 }
 do_playdigits(ts, module, dur, idd, digits, waitfor);
}

static int check_tonelist(ACTIVE_JOB *ap)
{
 SM_PLAY_TONE_LIST_STATUS_PARMS ds;
 int err;
 memset(&ds, 0, sizeof(ds));
 ds.channel = ap->mf->chan;
 err = sm_play_tone_list_status(&ds);
 if (err) {
 	fprintf(stderr, "sm_play_tone_list_status returned %d\n", err);
 }
 if (err || ds.status == kSMPlayToneListStatusComplete) {
	return 1;
 }
 return 0;
}

static void do_playtonelist(FILE *f, MVIP *ts, tSMModuleId module)
{
 struct sm_play_tone_list_parms fp;
 struct sm_play_tone_item tones[64];
 int c, err;
 memset(&fp, 0, sizeof(fp));
 fp.channel = allocchan(ts, module);
 if (!fp.channel) return;
 fp.tones = tones;
 for (;;) {
	c = sksp(f);
	if (c == '.')
		tones[fp.tone_count].operation = kSMToneOperationStop;
	else if (c == '+')
		tones[fp.tone_count].operation = kSMToneOperationSum;
	else if (c == '*')
		tones[fp.tone_count].operation = kSMToneOperationModulate;
	else {
		ungetc(c, f);
		break;
	}
	if (fp.tone_count == arlen(tones)) {
		printf("Error: too many tones in list\n");
		fp.tone_count--;
	}
	tones[fp.tone_count].duration = getnum(f);
	tones[fp.tone_count].frequency1 = getnum(f);
	tones[fp.tone_count].amplitude1 = getnum(f);
	tones[fp.tone_count].frequency2 = getnum(f);
	tones[fp.tone_count].amplitude2 = getnum(f);
	fp.tone_count++;
 }
 err = sm_play_tone_list(&fp);
 if (err) {
	fprintf(stderr, "sm_play_tone_list returned %d\n", err);
 }
 waitins(fp.channel, check_tonelist, 'c');
}

static void do_playtonelist_phase(FILE *f, MVIP *ts, tSMModuleId module)
{
 struct sm_play_tone_list_parms fp;
 struct sm_play_tone_item tones[64];
 struct sm_play_tone_list_phase_reverse_parms pp;
 int c, err, phase;
 memset(&fp, 0, sizeof(fp));
 fp.channel = allocchan(ts, module);
 if (!fp.channel) return;
 fp.tones = tones;
 phase = getnum(f);
 for (;;) {
	c = sksp(f);
	if (c == '.')
		tones[fp.tone_count].operation = kSMToneOperationStop;
	else if (c == '+')
		tones[fp.tone_count].operation = kSMToneOperationSum;
	else if (c == '*')
		tones[fp.tone_count].operation = kSMToneOperationModulate;
	else {
		ungetc(c, f);
		break;
	}
	if (fp.tone_count == arlen(tones)) {
		printf("Error: too many tones in list\n");
		fp.tone_count--;
	}
	tones[fp.tone_count].duration = getnum(f);
	tones[fp.tone_count].frequency1 = getnum(f);
	tones[fp.tone_count].amplitude1 = getnum(f);
	tones[fp.tone_count].frequency2 = getnum(f);
	tones[fp.tone_count].amplitude2 = getnum(f);
	fp.tone_count++;
 }
 err = sm_play_tone_list(&fp);
 if (err) {
	fprintf(stderr, "sm_play_tone_list returned %d\n", err);
 }
 memset(&pp, 0, sizeof(pp));
 pp.channel = fp.channel;
 pp.period = phase;
 err = sm_play_tone_list_phase_reverse(&pp);
 if (err) {
	fprintf(stderr, "sm_play_tone_list_phase_reverse returned %d\n", err);
 }
 waitins(fp.channel, check_tonelist, 'c');
}

static void do_selmod(MODDESC *modp, FILE *f, CARDLIST *cards)
{
 int c = sksp(f);
 if (c != '+') {
	err_t e;
	ungetc(c, f);
	e = cardlist_nthmod(modp, cards, getnum(f));
	if (e) error_log(stderr, e);
 } else {
	char buff[256];
	unsigned n;
	for (n=0; ; n++) {
		if (n >= arlen(buff) - 1) {
			fprintf(stderr, "Card/module specification too long\n");
			return;
		}
		c = getc(f);
		if (c == '\n') {
			err_t e;
			buff[n] = 0;
			e = cardlist_addcard(cards, buff);
			ungetc(c, f);
			if (e) error_log(stderr, e);
			return;
		}
		buff[n] = c;
	}
 }
}

/*
 * Commands:
 *	f freq ampl			register a new tone
 *	t tone1 tone2			register a new tone pair
 *	p dur code			start playing a tone pair
 *	c dur o/r/c (id on off)*	start playing tone sequence
 *	d dur idd "digits"		start playing digit sequence
 */
static void cmdloop(FILE *f, CHANPAR *chanpar)
{
 MODDESC mod;
 if (cardlist_nthmod(&mod, chanpar->cards, 0)) {
	mod.id = 0;
	mod.logname = "(no module)";
 }
 for (;;) {
	int junk;
	int c;
	printf("m%s ", mod.logname);
	c = sksp(f);
	switch (c) {
	case '?':
		printf("Commands:\n"
	"' text				display text\n"
	"f FREQ AMPL			register a new tone\n"
	"t TONE1 TONE2			register a new tone pair\n"
	"p DUR CODE			start playing a tone pair\n"
	"c DUR o/r/c (=ID ON OFF)*	start playing tone sequence\n"
	"d DUR IDD \"DIGITS\"		start playing digit sequence\n"
	"l (./+/* DUR FREQ1 AMPL1 FREQ2 AMPL2)+	start playing tone list\n"
	"L phase (./+/* DUR FREQ1 AMPL1 FREQ2 AMPL2)+	start playing tone list\n"
	"m MODULENO			select module\n"
	"m +type:serial[pos]		make module available for selection\n"
	"q				quit\n"
	"\tThe 'p', 'c', and 'd' commands also take an optional wait spec:\n"
	"\tWw	(default) use wait_for_completion\n"
	"\tWa	use anychannel status check (in background) until done \n"
	"\tWc	use per-channel status check (in background) until done \n"
			);
		break;
	case 'q':
		if (have_anyev) {
				/* this is not a good idea because another
				 * thread is waiting on this event - just
				 * checking what would happen
				 */
			int e = smd_ev_free(anyev);
			if (e) {
				fprintf(stderr, "smd_ev_free() returned %d\n", e);
			}
		}
		// fall in ...
	case EOF:
		return;
	case '\'':
		for (;;) {
			c = getc(f);
			if (c == EOF) return;
			putchar(c);
			if (c == '\n') break;
		}
		fflush(stdout);
		// fall in ...
	case '\n': ungetc(c, f); break;
	case 'f': do_addfreq(f, mod.id); break;
	case 't': do_addtone(f, mod.id); break;
	case 'p': do_playtone(f, &chanpar->timeslot, mod.id); break;
	case 'c': do_playcptone(f, &chanpar->timeslot, mod.id); break;
	case 'l': do_playtonelist(f, &chanpar->timeslot, mod.id); break;
	case 'L': do_playtonelist_phase(f, &chanpar->timeslot, mod.id); break;
	case 'd': cmd_playdigits(f, &chanpar->timeslot, mod.id, 0); break;
	case 'D': cmd_playdigits(f, &chanpar->timeslot, mod.id, 1); break;
	case 'm': do_selmod(&mod, f, chanpar->cards);
		break;
	default:
		fprintf(stderr, "Bad command: %c (%d)\n", c, c);
		break;
	case '#':
		break;
	}
	for (junk=0;;) {
		c = getc(f);
		if (c == EOF) return;
		if (c == '\n') break;
		if (!junk) fprintf(stderr, "Ignoring junk: ");
		fprintf(stderr, "%c", c);
		junk = 1;
	}
	if (junk) fprintf(stderr, "\n");
 }
}

static int tonegen(CHANPAR chanpar)
{
 cmdloop(stdin, &chanpar);
 return 0;
}

#include "gen/tonegen.args.i"

int main(int argc, char **argv)
{
 int iErr;
 err_t err;
 CARDLIST cards;
 CHANPAR chanpar;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || *argv) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
	return 1;
 }
 cardlist(&cards);
 chanpar.cards = &cards;
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
 chanpar.timeslot = arg.timeslot; 
 iErr = tonegen(chanpar);
 cardlist_dtor(&cards);
 return iErr;
}
