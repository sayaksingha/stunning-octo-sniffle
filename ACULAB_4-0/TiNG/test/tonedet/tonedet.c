/* tonedet.c - test/demo of detecting tones */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>

#include "smbesp.h"
#include "smdrvr.h"

#include "../Testlib/cardlist.h"
#include "../Testlib/error.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

#ifdef TiNGTYPE_LINUX
#include <poll.h>
#include <unistd.h>
typedef struct pollfd WAITABLE;
#define USE_POLL
#endif

#ifdef TiNGTYPE_QNX
#include <sys/poll.h>
#include <unistd.h>
typedef struct pollfd WAITABLE;
#define USE_POLL
#endif


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

typedef enum {TONE_TYPE_START, TONE_TYPE_END, TONE_TYPE_LEN} TONE_TYPE;


typedef struct {
	//detection specific stuff
	pthread_t thread;
	int stop;
	unsigned toneset;
	unsigned map;
	unsigned cptone;
	unsigned ansam;
	unsigned tonemode;
	TONE_TYPE tonetype;
	unsigned beep;
	unsigned mindur;
	double minfreq;
	double maxfreq;
	//end 
	CARDLIST *cards;
	tSMModuleId module;
	MVIP timeslot;
} CHANPAR;

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

static int fn_sm_switch_channel_input(tSMChannelId chan, CHANPAR *cp)
{
 struct sm_switch_channel_parms sw;
 int err;
 memset(&sw, 0, sizeof(sw));
 sw.channel = chan;
 sw.st = cp->timeslot.stream;
 sw.ts = cp->timeslot.timeslot;
 sw.type = cp->timeslot.type;
 err = sm_switch_channel_input(&sw);
 if (err) fprintf(stderr, "sm_switch_channel_input returned %d\n", err);
 return err;
}

static int releasechan(tSMChannelId chan)
{
 int err = sm_channel_release(chan);
 if (err) fprintf(stderr, "sm_channel_release returned %d\n", err);
 return err;
}

static void do_addfreq(FILE *f, tSMModuleId curmod)
{
 struct sm_input_freq_coeffs_parms fp;
 int err;
 memset(&fp, 0, sizeof(fp));
 fp.lower_limit = getfloat(f);
 fp.upper_limit = getfloat(f);
 fp.module = curmod;
 err = sm_add_input_freq_coeffs(&fp);
 if (err) {
	fprintf(stderr, "sm_add_input_freq_coeffs returned %d\n", err);
 }
 printf("id = %d\n", fp.id);
}

static void do_addtone(FILE *f, tSMModuleId curmod)
{
 struct sm_input_tone_set_parms fp;
 int err;
 int c;
 memset(&fp, 0, sizeof(fp));
 fp.band1_first_freq_coeffs_id = getnum(f);
 if ((c = getc(f)) != '+') {
 	printf("Expected '+', got '%c'\n", c);
 	ungetc(c, f);
 	return;
 }
 fp.band1_freq_count = getnum(f);
 fp.band2_first_freq_coeffs_id = getnum(f);
 if ((c = getc(f)) != '+') {
 	printf("Expected '+', got '%c'\n", c);
 	ungetc(c, f);
 	return;
 }
 fp.band2_freq_count = getnum(f);
 fp.req_third_peak = getfloat(f);
 fp.req_signal_to_noise_ratio = getfloat(f);
 fp.req_minimum_power = getfloat(f);
 fp.req_twist_for_dual_tone = getfloat(f);
 fp.module = curmod;
 err = sm_add_input_tone_set(&fp);
 if (err) {
	fprintf(stderr, "sm_add_input_tone_set returned %d\n", err);
 }
 printf("id = %d\n", fp.id);
}

static void do_adjtone(FILE *f, tSMModuleId curmod)
{
 struct sm_adjust_tone_set_parms fp;
 int err;
 int c;
 memset(&fp, 0, sizeof(fp));
 fp.tone_set_id = getnum(f);
 c = sksp(f);
 if (c >= '0' && c <= '9') {
	ungetc(c, f);
	fp.parameter_id = getnum(f);
	c = sksp(f);
 } else {
	static struct {
		char *name;
		int isint;
		enum kAdjustToneSet pcode;
	} params[] = {
		{ "3rdPeak", 0, kAdjustToneSetFPParamId3rdPeak, },
		{ "SNRatio", 0, kAdjustToneSetFPParamIdSNRatio, },
		{ "MinPower", 0, kAdjustToneSetFPParamIdMinPower, },
		{ "Twist", 0, kAdjustToneSetFPParamIdTwist, },
		{ "MinOnTime", 1, kAdjustToneSetIntParamIdMinOnTime, },
		{ "MinOffTime", 1, kAdjustToneSetIntParamIdMinOffTime, },
		{ "StartFreq", 0, kAdjustToneSetFPParamIdStartFreq, },
		{ "StopFreq", 0, kAdjustToneSetFPParamIdStopFreq, },
	};
	char pname[15];
	unsigned i;
	ungetc(c, f);
	for (i=0;;) {
		c = getc(f);
		if (c == ' ' || c == '\n') break;
		pname[i++] = c;
		if (i >= arlen(pname)) {
			pname[--i] = 0;
			printf("Parameter name too long: '%s'\n", pname);
			return;
		}
	}
	pname[i] = 0;
	ungetc(c, f);
	for (i=0; strcmp(params[i].name, pname); ) {
		if (++i >= arlen(params)) {
			printf("Unknown parameter name: '%s'\n", pname);
			printf("Parameters are:\n");
			for (i=0; i < arlen(params); i++) {
				printf("\t%s\n", params[i].name);
			}
			return;
		}
	}
	fp.parameter_id = params[i].pcode;
	c = params[i].isint ? 'i' : 'f';
 }
 if (c == 'i') {
	fp.parameter_value.int_value = getnum(f);
 } else if (c == 'f') {
	fp.parameter_value.fp_value = getfloat(f);
 } else {
	printf("Expected 'i' or 'f', got '%c'\n", c);
	ungetc(c, f);
	return;
 }
 fp.module = curmod;
 err = sm_adjust_input_tone_set(&fp);
 if (err) {
	fprintf(stderr, "sm_adjust_input_tone_set returned %d\n", err);
 }
}

static void do_resetcp(FILE *f, tSMModuleId curmod)
{
 struct sm_reset_input_cptones_parms fp;
 int err;
 memset(&fp, 0, sizeof(fp));
 fp.tone_set_id = getnum(f);
 fp.module = curmod;
 err = sm_reset_input_cptones(&fp);
 if (err) {
	fprintf(stderr, "sm_reset_input_cptones returned %d\n", err);
 }
}

static void do_addcptone(FILE *f, tSMModuleId curmod)
{
 struct sm_input_cptone_parms fp;
 int err;
 memset(&fp, 0, sizeof(fp));
 fp.module = curmod;
 fp.id = getnum(f);
 for (;;) {
	int c = sksp(f);
	ungetc(c, f);
	if (c == '\n') break;
	if (fp.state_count == arlen(fp.states)) {
		printf("Error: too many tones in cadence\n");
		fp.state_count--;
	}
	fp.states[fp.state_count].freq_id = getnum(f);
	fp.states[fp.state_count].minimum_cadence = getnum(f);
	fp.states[fp.state_count].maximum_cadence = getnum(f);
	fp.state_count++;
 }
 err = sm_add_input_cptone(&fp);
 if (err) {
	fprintf(stderr, "sm_add_input_cptone returned %d\n", err);
 }
}

static void do_selmod(MODDESC *modp, FILE *f, CARDLIST *cards)
{
 int c = sksp(f);
 if (c == '?') {
	unsigned u = 0;
	unsigned cno;
	for (cno=0; cno < cards->ncard; cno++) {
		MODLIST *mp = &cards->card[cno].modlist;
		unsigned m;
		for (m=0; m < mp->nmod; m++) {
			printf("m%d = %s", u, mp->mods[m].logname);
			if (mp->mods[m].id == modp->id) printf("  <--");
			printf("\n");
			u++;
		}
	}
 } else {
	unsigned m;
	err_t e = 0;
	if (c != '+') {
		ungetc(c, f);
		m = getnum(f);
	} else {
		char buff[256];
		unsigned n;
		e = cardlist_nummod(&m, cards);
		if (!e) for (n=0; ; n++) {
			if (n >= arlen(buff) - 1) {
				fprintf(stderr, "Card/module specification too long\n");
				return;
			}
			c = getc(f);
			if (c == '\n') {
				buff[n] = 0;
				e = cardlist_addcard(cards, buff);
				ungetc(c, f);
				break;
			}
			buff[n] = c;
		}
	}
	if (!e) e = cardlist_nthmod(modp, cards, m);
	if (e) error_log(stderr, e);
 }
}

static int detectloop(tSMChannelId chan, CHANPAR *cp, tSMEventId ev)
{
 struct timeb lastout;
 ftime(&lastout);
 while (!cp->stop) {
	int err;
#ifdef TiNGTYPE_WINNT
	DWORD res = WaitForSingleObject(ev, 500);
	if (res != WAIT_OBJECT_0) {
		if (res != WAIT_TIMEOUT) {
			fprintf(stderr, "WaitForSingleObject returned %08lx\n", res);
			return 1;
		}
#ifdef TEST_REPEAT
		return 2;
#endif
	} else
#endif
#ifdef USE_POLL
	struct pollfd pfd;
	int res;
	pfd.fd = ev.fd;
	pfd.events = ev.mode;
	res = poll(&pfd, 1, 500);
	if (res != 1) {
		if (res) {
			perror("poll() error");
			return 1;
		}
#ifdef TEST_REPEAT
		return 2;
#endif
	} else
#endif
		{
		for (;;) {
			SM_RECOGNISED_PARMS rp;
			unsigned long dtim;
			struct timeb now;
			char buff[80];
			const char *type;
			memset(&rp, 0, sizeof(rp));
			rp.channel = chan;
			err = sm_get_recognised(&rp);
			if (err) {
				fprintf(stderr, "sm_get_recognised returned %d\n", err);
				return 1;
			}
			ftime(&now);
			dtim = now.millitm - lastout.millitm;
			dtim += (now.time - lastout.time) * 1000;
			switch (rp.type) {
			case kSMRecognisedNothing: type = 0; break;
			case kSMRecognisedDigit:
				sprintf(buff, "Digit: %c", rp.param0);
				type = buff;
				break;
			case kSMRecognisedTone: type = "Tone"; break;
			case kSMRecognisedCPTone: type = "CPTone"; break;
			case kSMRecognisedGruntEnd: type = "GruntEnd"; break;
			case kSMRecognisedOverrun: type = "Overrun"; break;
			case kSMRecognisedANS: type = "ANS"; break;
			case kSMRecognisedBeep: type = "Beep"; break;
			default:
				fprintf(stderr, "sm_get_recognised produced event %d\n", rp.type);
				return 1;
			}
			if (!type) break;
			printf("+%ld mS\n", dtim);
			lastout = now;
			printf("\t%s: %d %d\n", type, rp.param0, rp.param1);
		}
	}
	fflush(stdout);
 }
 return 0;
}

static int start_listen_for(CHANPAR *cp, tSMChannelId chan)
{
 struct sm_listen_for_parms lp;
 memset(&lp, 0, sizeof(lp));
 lp.channel = chan;
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
 return sm_listen_for(&lp);
}

static int detectchan(CHANPAR *cp)
{
 SM_CHANNEL_ALLOC_PLACED_PARMS ap;
 int err;
 // detect on timeslot xp.stream:xp.ts
 memset(&ap, 0, sizeof(ap));
 ap.type = kSMChannelTypeInput;
 ap.module = cp->module;
 err = sm_channel_alloc_placed(&ap);
 if (err) {
 	fprintf(stderr, "sm_channel_alloc_placed returned %d\n", err);
 } else {
	int e2;
	if (!fn_sm_switch_channel_input(ap.channel, cp)) {
#ifdef TEST_REPEAT
again:
#endif
		SM_CHANNEL_SET_EVENT_PARMS ep;
		memset(&ep, 0, sizeof(ep));
		err = smd_ev_create(&ep.event, ap.channel, kSMEventTypeRecog, kSMChannelSpecificEvent);
		if (err) fprintf(stderr, "smd_ev_create returned %d\n", err);
		else {
			ep.channel = ap.channel;
			ep.event_type=kSMEventTypeRecog;
			ep.issue_events=kSMChannelSpecificEvent;
			err = sm_channel_set_event(&ep);
			if (err) fprintf(stderr, "sm_channel_set_event returned %d\n", err);
			else {
				if (cp->ansam) {
					struct sm_ans_listen_for_parms lp;
					memset(&lp, 0, sizeof(lp));
					lp.channel = ap.channel;
					lp.detection_mode = kSMANSModeDetect;
					err = sm_ans_listen_for(&lp);
					if (err) fprintf(stderr, "sm_ans_listen_for returned %d\n", err);
				} else if (cp->beep) {
					struct sm_beep_listen_for_parms lp;
					memset(&lp, 0, sizeof(lp));
					lp.channel = ap.channel;
					lp.min_duration = cp->mindur;
					lp.upper_limit = cp->maxfreq;
					lp.lower_limit = cp->minfreq;
					err = sm_beep_listen_for(&lp);
					if (err) fprintf(stderr, "sm_beep_listen_for returned %d\n", err);
				} else {
					err = start_listen_for(cp, ap.channel);
					if (err) fprintf(stderr, "sm_listen_for returned %d\n", err);
				}
			}
			if (!err) {
				err = detectloop(ap.channel, cp, ep.event);
#ifdef TEST_REPEAT
				if (err == 2) goto again;
#endif
			}
			smd_ev_free(ep.event);
		}
 	}
 	e2 = releasechan(ap.channel);
	err = err || e2;
 }
 return err;
}

static void *chanfunc(void *vp)
{
 CHANPAR *cp = vp;
 if (detectchan(cp)) return (void *) 1;
 return 0;
}

//	l a
//	l c
//	l {1,40,64}[EL] [m] [sN]
static int rd_listen(CHANPAR *cp, FILE *f)
{
 int c = sksp(f);
 cp->toneset = 0;
 cp->map = 0;
 cp->cptone = 0;
 cp->ansam = 0;
 cp->tonemode = 0;
 cp->tonetype = TONE_TYPE_START;
 cp->beep = 0;
 if (c == 'c') {
 	cp->cptone = 1;
 } else if (c == 'a') {
	cp->ansam = 1;
 } else if (c == 'b') {
	cp->beep = 1;
	cp->mindur = getnum(f);
	cp->minfreq = getfloat(f);
	cp->maxfreq = getfloat(f);
 } else {
	ungetc(c, f);
	cp->tonemode = getnum(f);
	c = sksp(f);
	if (c == 'E') {
		cp->tonetype = TONE_TYPE_END;
		c = sksp(f);
	} else if (c == 'L') {
		cp->tonetype = TONE_TYPE_LEN;
		c = sksp(f);
	}
	if (c == 'm') {
		cp->map = 1;
		c = sksp(f);
	}
	if (c == 's') {
		cp->toneset = getnum(f);
	} else {
		ungetc(c, f);
	}
 }
 return 0;
}

static void cmdloop(FILE *f, CHANPAR *chan)
{
 MODDESC mod;
 int running = 0;
 if (cardlist_nthmod(&mod, chan->cards, 0)) {
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
	"f FREQ FREQ			add a new tone band\n"
	"t id1+n id2+n 3rd snr minpwr twist	add new tone set\n"
	"j tsid what val		adjust tone set param\n"
	"r tsid				reset cptones\n"
	"c ID [ fid max min ]*		add cptone\n"
	"m MODULENO			select module\n"
	"m? 				list open modules\n"
	"m +type:serial[pos]		make module available for selection\n"
	"l c				listen for cptone\n"
	"l a				listen for ANS/ANSam\n"
	"l {1,40,64}[EL] [m] [sN]	listen for tone\n"
	"l b mindur minfreq maxfreq    listen for beep tone\n"
	"q				quit\n"
			);
		break;
	case EOF:
	case 'q':
		if (running) {
			void *sts;
			chan->stop = 1;
			c = pthread_join(chan->thread, &sts);
			if (c) {
				fprintf(stderr, "pthread_join failed: %s\n", strerror(c));
			}
		}
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
	case 'j': do_adjtone(f, mod.id); break;
	case 'r': do_resetcp(f, mod.id); break;
	case 'c': do_addcptone(f, mod.id); break;
	case 'm': do_selmod(&mod, f, chan->cards);
		break;
	case 'l':
		if (running) {
			void *sts;
			chan->stop = 1;
			c = pthread_join(chan->thread, &sts);
			if (c) {
				fprintf(stderr, "pthread_join failed: %s\n", strerror(c));
			}
		}
		if (!rd_listen(chan, f)) {
			chan->stop = 0;
			chan->module=mod.id;
			c = pthread_create(&chan->thread, 0, chanfunc, chan);
			if (c) {
				fprintf(stderr, "pthread_create failed: %s\n", strerror(c));
			} else running = 1;
		}
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

static int tonedet(CHANPAR chan)
{
 cmdloop(stdin, &chan);
 return 0;
}

#include "gen/tonedet.args.i"

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
 if (arg.serialnumber) {
	err = cardlist_addcard(&cards, arg.serialnumber);
	if (err) {
		error_log(stderr, err);
		return 1;
	}
 }
 chanpar.cards = &cards;
 chanpar.timeslot = arg.timeslot; 
 iErr = tonedet(chanpar);
 cardlist_dtor(&cards);
 return iErr;
}
