#include <pthread.h>
#include <stdio.h>
#include <smbesp.h>
#include <smdrvr.h>
#include <smclib.h>
#include "../Testlib/cardlist.h"

#ifdef TiNGTYPE_LINUX
#include <poll.h>
#include <string.h>
#include <unistd.h>
typedef struct pollfd WAITABLE;
#define USE_PIPE
#endif

#ifdef TiNGTYPE_QNX
#include <sys/poll.h>
#include <string.h>
#include <unistd.h>
typedef struct pollfd WAITABLE;
#define USE_PIPE
#endif


#ifdef TiNGTYPE_WINNT
#include <io.h>
#include "../../libutil/WINNT/lasterr.h"
typedef HANDLE WAITABLE;
#endif


#define arlen(x) (sizeof(x)/sizeof(*(x)))

typedef struct {
	int stream;
	int timeslot;
	int type;
} MVIP;
#define MVIP_STREAM_NONE 0xff

typedef struct {
	unsigned id;
#ifdef USE_PIPE
	int cmdpipe[2];
#endif
#ifdef TiNGTYPE_WINNT
	HANDLE cmdevent[2];
#endif
	pthread_t tid;
	pthread_mutex_t mx;
	unsigned quit;
	MODDESC mod;
	tSMConference hconf;
	struct confin {
		tSMChannelId chan;
		MVIP ts;
		int agc;
		int volume;
		int detecting;
		tSMEventId event;
	} in[64];
	struct confout {
		tSMChannelId chan;
		MVIP ts;
		int agc;
		int volume;
	} out[64];	
} CONFDAT;

typedef struct {
	CONFDAT clist[84];
	CARDLIST cardlist;
} CONFLIST;

static void confinit(CONFDAT *cp)
{
 struct confout *co;
 struct confin *ci;
 for (ci=cp->in; ci < cp->in+arlen(cp->in); ci++) {
	ci->chan = 0;
	ci->detecting = 0;
 }
 for (co=cp->out; co < cp->out+arlen(cp->out); co++) {
	co->chan = 0;
 }
 cp->quit = 0;
 cp->hconf = 0;
 cp->mod.id = 0;
 cp->mod.logname = "(no module)";
}

static int signal_thread(CONFDAT *cp, int channo)
{
#ifdef USE_PIPE
 char c = 0;
 int e = write(cp->cmdpipe[1], &c, 1);
 (void) channo;
 if (e < 0) perror("write to cmdpipe failed");
 return e <= 0;
#endif
#ifdef TiNGTYPE_WINNT
 if (!SetEvent(cp->cmdevent[channo >= 32])) {
	lasterr("SetEvent failed");
	return 1;
 }
 return 0;
#endif
}

static void showconf(CONFDAT *cp)
{
#if 0
 SM_CONFERENCE_INFO_PARMS cip;
 tSMChannelId actchan[129];
#endif
 unsigned i;
#if 0
 int r;
 memset(&cip, 0, sizeof(cip));
 cip.conf = cp->hconf;
 cip.activeChannelList = actchan;
 r = sm_conference_info(&cip);
 if (r) printf("%d: sm_conference_info returned %d\n", cp->id, r);
#endif
 for (i=0; i < arlen(cp->in); i++) {
	struct confin *ci = &cp->in[i];
	if (ci->chan) {
		char vol[10];
		char ts[20];
#if 0
		unsigned j;
#endif
		switch (ci->ts.type) {
		case kSMTimeslotTypeALaw: sprintf(vol, "a"); break;
		case kSMTimeslotTypeMuLaw: sprintf(vol, "u"); break;
		case kSMTimeslotTypeData: sprintf(vol, "r"); break;
		default: sprintf(vol, "%d", ci->ts.type); break;
		}
		sprintf(ts, "%d:%d:%s", ci->ts.stream, ci->ts.timeslot, vol);
		sprintf(vol, "v%d", ci->volume);
		printf("i%d A%c %4s %5s det%c",
			i,
			ci->agc ? '+' : '-',
			vol,
			ts,
			ci->detecting ? '+' : '-');
#if 0
		for (j=0; j < cip.activeChannelCount; j++) {
			if (actchan[j] == ci->chan) printf(" active");
		}
#endif
		printf("\n");
	}
 }
 for (i=0; i < arlen(cp->out); i++) {
	struct confout *co = &cp->out[i];
	if (co->chan) {
		char vol[10];
		char ts[20];
#if 0
		unsigned j;
#endif
		switch (co->ts.type) {
		case kSMTimeslotTypeALaw: sprintf(vol, "a"); break;
		case kSMTimeslotTypeMuLaw: sprintf(vol, "u"); break;
		case kSMTimeslotTypeData: sprintf(vol, "r"); break;
		default: sprintf(vol, "%d", co->ts.type); break;
		}
		sprintf(ts, "%d:%d:%s", co->ts.stream, co->ts.timeslot, vol);
		sprintf(vol, "v%d", co->volume);
		printf("o%d A%c %4s %5s",
			i,
			co->agc ? '+' : '-',
			vol,
			ts);
#if 0
		for (j=0; j < cip.activeChannelCount; j++) {
			if (actchan[j] == ci->chan) printf(" active");
		}
#endif
		printf("\n");
	}
 }
}

static void do_showall(CONFLIST *clp)
{
 unsigned u;
 for (u=0; u < arlen(clp->clist); u++) {
	if (clp->clist[u].hconf) {
		printf("Conference %d: mod:%s\n", u, clp->clist[u].mod.logname);
		showconf(&clp->clist[u]);
		printf("\n");
	}
 }
}

typedef struct {
	FILE *f;
	int ungot;
} ECHOFILE;

static int hgetc(ECHOFILE *f)
{
 int c;
 if (f->ungot) {
 	f->ungot = 0;
 	return getc(f->f);
 }
 c = getc(f->f);
 // putchar(c);
 return c;
}

static void hungetc(int c, ECHOFILE *f)
{
 ungetc(c, f->f);
 f->ungot = 1;
}

static int sksp(ECHOFILE *f)
{
 for (;;) {
	int c = hgetc(f);
	if (c != ' ' && c != '\t') return c;
 }
}

static int getnum(ECHOFILE *f)
{
 int c = sksp(f);
 int base = 10;
 int num = 0;
 int neg = 0;
 if (c == '-') c=hgetc(f), neg = 1;
 if (c == '0') {
 	c = hgetc(f);
 	if (c == 'x') {
 		base = 16;
		c = hgetc(f);
	} else base = 8;
 }
 for (;;) {		// decimal
	int d;
	if (c >= '0' && c <= '9') d = c - '0';
	else if (c >= 'a' && c <= 'f') d = c - 'a' + 10;
	else break;
	if (d >= base) break;
	num = num*base + d;
	c = hgetc(f);
 }
 hungetc(c, f);
 return neg ? -num : num;
}

static int getinchan(unsigned *channo, CONFDAT *cp, ECHOFILE *f)
{
 int i = sksp(f);
 if (i != 'i') {
 	printf("Expected 'i', got '%c'\n", i);
 	return 1;
 }
 i = getnum(f);
 if (i < 0) {
	printf("Input channel number is negative (%d)\n", i);
	return 1;
 }
 if ((unsigned) i > arlen(cp->in)) {
	printf("Input channel number too big (%d)\n", i);
	return 1;
 }
 if (!cp->in[i].chan) {
	printf("Input channel not opened (%d)\n", i);
	return 1;
 }
 *channo = i;
 return 0;
}

static int getoutchan(unsigned *channo, CONFDAT *cp, ECHOFILE *f)
{
 int i = sksp(f);
 if (i != 'o') {
 	printf("Expected 'o', got '%c'\n", i);
 	return 1;
 }
 i = getnum(f);
 if (i < 0) {
	printf("Output channel number is negative (%d)\n", i);
	return 1;
 }
 if ((unsigned) i > arlen(cp->out)) {
	printf("Output channel number too big (%d)\n", i);
	return 1;
 }
 if (!cp->out[i].chan) {
	printf("Output channel not opened (%d)\n", i);
	return 1;
 }
 *channo = i;
 return 0;
}

static void do_new(CONFDAT *cp, ECHOFILE *f)
{
 SM_SWITCH_CHANNEL_PARMS sp;
 int io = sksp(f);
 unsigned i;
 int c;
 memset(&sp, 0, sizeof(sp));
 if (io == 'i') io = 1;
 else if (io == 'o') io = 0;
 else {
 	printf("expected 'i' or 'o', got '%c'\n", io);
 	hungetc(io, f);
 	return;
 }
 sp.st = getnum(f);
 c = sksp(f);
 if (c != ':') {
 	printf("expected ':' after stream, got '%c'\n", c);
 	hungetc(c, f);
 	return;
 }
 sp.ts = getnum(f);
 c = hgetc(f);
 if (c == ':') {
 	c = hgetc(f);
 	switch (c) {
	case 'a': sp.type = kSMTimeslotTypeALaw; break;
	case 'u': sp.type = kSMTimeslotTypeMuLaw; break;
	case 'r': sp.type = kSMTimeslotTypeData; break;
	default:
		printf("expected 'a', 'u', or 'r' after ':', got '%c'\n", c);
		hungetc(c, f);
		return;
	}
	c = sksp(f);
 }
 hungetc(c, f);
 if (c != '\n') {
	printf("got '%c' after command\n", c);
	return;
 }
 if (io) for (i=0; i < arlen(cp->in); i++) {
 	struct confin *const ci = &cp->in[i];
 	if (!ci->chan) {
		SM_CHANNEL_ALLOC_PLACED_PARMS pp;
		int r;
		memset(&pp, 0, sizeof(pp));
		pp.type = kSMChannelTypeInput;
		pp.module = cp->mod.id;
		r = sm_channel_alloc_placed(&pp);
		if (r) {
			printf("sm_channel_alloc_placed returned %d\n", r);
			return;
		}
		sp.channel = pp.channel;
		r = sm_switch_channel_input(&sp);
		if (r) {
			printf("sm_switch_channel_input returned %d\n", r);
			r = sm_channel_release(pp.channel);
			if (r) printf("sm_release_channel returned %d\n", r);
			return;
		}
		ci->ts.stream = sp.st;
		ci->ts.timeslot = sp.ts;
		ci->ts.type = sp.type;
		ci->chan = pp.channel;
		ci->volume = 0;
		ci->agc = 0;
		ci->detecting = 0;
		return;
 	}
 }
 for (i=0; i < arlen(cp->out); i++) {
 	struct confout *const co = &cp->out[i];
 	if (!co->chan) {
		SM_CHANNEL_ALLOC_PLACED_PARMS pp;
		int r;
		memset(&pp, 0, sizeof(pp));
		pp.type = kSMChannelTypeOutput;
		pp.module = cp->mod.id;
		r = sm_channel_alloc_placed(&pp);
		if (r) {
			printf("sm_channel_alloc_placed returned %d\n", r);
			return;
		}
		sp.channel = pp.channel;
		r = sm_switch_channel_output(&sp);
		if (r) {
			printf("sm_switch_channel_output returned %d\n", r);
			r = sm_channel_release(pp.channel);
			if (r) printf("sm_release_channel returned %d\n", r);
			return;
		}
		co->ts.stream = sp.st;
		co->ts.timeslot = sp.ts;
		co->ts.type = sp.type;
		co->chan = pp.channel;
		co->volume = 0;
		co->agc = 0;
		return;
 	}
 }
 printf("%d: Too many channels to start another\n", cp->id);
}

static CONFDAT *do_create(MODDESC *modp, CONFLIST *clp, ECHOFILE *f)
{
 unsigned u = getnum(f);
 CONFDAT *cp;
 if (u >= arlen(clp->clist)) {
	printf("conference number too big: must be under %ld\n",
		(long) arlen(clp->clist));
	return 0;
 }
 cp = &clp->clist[u];
 if (!cp->hconf) {
	cp->hconf = sm_conference_create();
	if (!cp->hconf) {
		printf("%d: sm_conference_create failed\n", cp->id);
		cp = 0;
	}
 }
 cp->mod = *modp;
 return cp;
}

static void do_selmod(MODDESC *modp, ECHOFILE *f, CARDLIST *cards)
{
 err_t e;
 int c = sksp(f);
 if (c != '+') {
	hungetc(c, f);
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
		c = hgetc(f);
		if (c == '\n') {
			buff[n] = 0;
			e = cardlist_addcard(cards, buff);
			hungetc(c, f);
			if (e) error_log(stderr, e);
			return;
		}
		buff[n] = c;
	}
 }
}

static void clear_inp(struct confin *ci)
{
 if (ci->detecting) {
	SM_CHANNEL_SET_EVENT_PARMS ep;
	int r;
	ci->detecting = 0;
	memset(&ep, 0, sizeof(ep));
	ep.channel = ci->chan;
	ep.event_type = kSMEventTypeRecog;
	ep.issue_events = kSMChannelNoEvent;
	r = sm_channel_set_event(&ep);
	if (r) printf("sm_channel_set_event returned %d\n", r);
	smd_ev_free(ci->event);
 }
}

static void do_delete(CONFDAT *cp, ECHOFILE *f)
{
 (void) f;
 if (!cp->hconf) {
	printf("No conference to delete\n");
 } else {
	unsigned i;
	sm_conference_delete(cp->hconf);
	cp->hconf = 0;
	for (i=0; i < arlen(cp->in); i++) {
		struct confin *const ci = &cp->in[i];
		if (ci->chan) {
			int r;
			clear_inp(ci);
			r = sm_channel_release(ci->chan);
			if (r) printf("sm_release_channel returned %d\n", r);
			ci->chan = 0;
		}
	}
	for (i=0; i < arlen(cp->out); i++) {
		struct confout *const co = &cp->out[i];
		if (co->chan) {
			int r = sm_channel_release(co->chan);
			if (r) printf("sm_release_channel returned %d\n", r);
			co->chan = 0;
		}
	}
 }
}

static void do_add(CONFDAT *cp, ECHOFILE *f)
{
 SM_CONFERENCE_ADD_PARTY_PARMS pp;
 unsigned channo;
 int r;
 if (!cp->hconf) {
	printf("No conference\n");
	return;
 }
 memset(&pp, 0, sizeof(pp));
 if (getinchan(&channo, cp, f)) return;
 pp.channelIn = cp->in[channo].chan;
 for (;;) {
	int c = sksp(f);
	if (c == 'a') {
		pp.in_agc = getnum(f);
		cp->in[channo].agc = pp.in_agc;
	} else if (c == 'v') {
		pp.in_volume = getnum(f);
		cp->in[channo].volume = pp.in_volume;
	} else {
		hungetc(c, f);
		break;
	}
 }
 if (getoutchan(&channo, cp, f)) return;
 pp.channelOut = cp->out[channo].chan;
 for (;;) {
	int c = sksp(f);
	if (c == 'a') {
		pp.out_agc = getnum(f);
	} else if (c == 'v') {
		pp.out_volume = getnum(f);
	} else {
		hungetc(c, f);
		break;
	}
 }
 pp.conf = cp->hconf;
 r = sm_conference_add_party(&pp);
 if (r) printf("%d: sm_conference_add_party returned %d\n", cp->id, r);
}

static void do_remove(CONFDAT *cp, ECHOFILE *f)
{
 SM_CONFERENCE_REMOVE_PARTY_PARMS pp;
 unsigned channo;
 int r;
 if (!cp->hconf) {
	printf("No conference\n");
	return;
 }
 memset(&pp, 0, sizeof(pp));
 if (getinchan(&channo, cp, f)) return;
 pp.channelIn = cp->in[channo].chan;
 if (getoutchan(&channo, cp, f)) return;
 pp.channelOut = cp->out[channo].chan;
 pp.conf = cp->hconf;
 r = sm_conference_remove_party(&pp);
 if (r) printf("%d: sm_conference_remove_party returned %d\n", cp->id, r);
}

/* detection parameters:
 *	mD	tone detection mode D mS
 *	sS	active tone set S
 *	a	map tones to digits
 *	c	cptone
 *	gN	grunt detection N mS
 */
static void do_detect(unsigned channo, CONFDAT *cp, ECHOFILE *f)
{
 SM_LISTEN_FOR_PARMS lp;
 memset(&lp, 0, sizeof(lp));
 for (;;) {
 	int c = sksp(f);
 	switch (c) {
 	case 'm':
 		c = getnum(f);
 		switch (c) {
 		case 0:
 		case 1:
 		case 32:
			lp.tone_detection_mode = kSMToneDetectionNoMinDuration;
			break;
 		case 40:
			lp.tone_detection_mode = kSMToneDetectionMinDuration40;
			break;
 		case 64:
			lp.tone_detection_mode = kSMToneDetectionMinDuration64;
			break;
 		default:
 			printf("invalid tone detection min duration: %d\n", c);
 			return;
 		}
 		break;
 	case 's':
 		lp.active_tone_set_id = getnum(f);
 		break;
 	case 'a':
 		lp.map_tones_to_digits = 1;
 		break;
 	case 'c':
 		lp.enable_cptone_recognition = 1;
 		break;
 	case 'g':
 		lp.enable_grunt_detection = 1;
 		c = getnum(f);
 		if (!c) c = 1;
 		lp.grunt_latency = c;
 		break;
 	case '\n':
		hungetc(c, f);
 		clear_inp(&cp->in[channo]);
 		lp.channel = cp->in[channo].chan;
 		c = sm_listen_for(&lp);
 		if (c) {
 			printf("%d: sm_listen_for returned %d\n", cp->id, c);
 			return;
 		} else {
 			SM_CHANNEL_SET_EVENT_PARMS ep;
 			memset(&ep, 0, sizeof(ep));
 			c = smd_ev_create(&ep.event, cp->in[channo].chan,
 				kSMEventTypeRecog, kSMChannelSpecificEvent);
			if (c) {
				printf("%d: smd_ev_create returned %d\n", cp->id, c);
				// cancel listen?
				return;
			}
			ep.channel = cp->in[channo].chan;
			ep.event_type = kSMEventTypeRecog;
			ep.issue_events = kSMChannelSpecificEvent;
			c = sm_channel_set_event(&ep);
			if (c) {
				printf("%d: sm_channel_set_event returned %d\n", cp->id, c);
				smd_ev_free(ep.event);
			} else {
				cp->in[channo].event = ep.event;
				cp->in[channo].detecting = 1;
			}
 		}
 		signal_thread(cp, channo);
 		return;
 	default:
		printf("got unknown detection command '%c'\n", c);
		printf("Valid commands are:\n"
			"\tmD\ttone detection mode D mS\n"
			"\tsS\tactove tone set S\n"
			"\ta\tmap tones to digits\n"
			"\tc\tenable cptone detection\n"
			"\tgN\tenable grunt detection, N mS\n"
			);
		return;
 	}
 }
}

/*
 * Commands:
 *	+c			create conference
 *	-c			delete conference
 *	+[io]N:N		new input/output on timeslot N:N
 *	a iN aA vV oN aA vV	add party to conference
 *	r iN oN			remove party from conference
 *	iN d ...		start detection
 */
static int runcmd(MODDESC *modp, CONFLIST *allconf, CONFDAT **cur, ECHOFILE *f, int c)
{
 switch (c) {
	unsigned channo;
 case '?':
	printf("Commands:\n"
		"		show current status\n"
		"cNmN			select conference (create if necessary)\n"
		"d			delete conference\n"
		"+[io]N:N		new input/output on timeslot N:N\n"
		"a iN aA vV oN aA vV	add party to conference\n"
		"r iN oN			remove party from conference\n"
		"iN d ...		start detection\n"
		"m S N			open module N on card S\n"
		"s			show all conferences\n"
		"#anything		comment\n"
		);
	break;
#if 0
 case '.': showconfinternals(); break;
#endif
 case EOF:
 case 'q': return c;
 case '\n':
	hungetc(c, f);
	if (!*cur) {
 		printf("No conference selected\n");
 		break;
	}
	showconf(*cur);
	break;
 case 'c':
	*cur = do_create(modp, allconf, f);
	break;
 case 'd':
	if (!*cur) {
 		printf("No conference selected\n");
 		break;
	}
	do_delete(*cur, f);
 	*cur = 0;
 	break;
 case '+':
	if (!*cur) {
 		printf("No conference selected\n");
 		break;
	}
	do_new(*cur, f);
 	break;
 case 'a':
	if (!*cur) {
 		printf("No conference selected\n");
 		break;
	}
	do_add(*cur, f);
	break;
 case 'r':
	if (!*cur) {
 		printf("No conference selected\n");
 		break;
	}
	do_remove(*cur, f);
	break;
 case 'm':
 	do_selmod(modp, f, &allconf->cardlist);
 	break;
 case 's':
 	do_showall(allconf);
 	if (*cur) printf("Current conference: %ld\n",
		(long) (*cur - allconf->clist));
 	else printf("No current conference\n");
 	break;
 case 'i':
	if (!*cur) {
 		printf("No conference selected\n");
 		break;
	}
	hungetc(c, f);
	if (!getinchan(&channo, *cur, f)) {
		c = sksp(f);
		switch (c) {
		case 'd':
			do_detect(channo, *cur, f);
			break;
		default:
			hungetc(c, f);
			break;
		}
	}
	break;
 default:
	fprintf(stderr, "Bad command: %c\n", c);
	break;
 case '#':
	for (;;) {
		c = hgetc(f);
		if (c == EOF) return c;
		putchar(c);
		if (c == '\n') {
			fflush(stdout);
			return c;
		}
	}
	break;
 }
 for (;;) {
	c = hgetc(f);
	if (c == EOF || c == '\n') return c;
 }
}

static int mainloop(MODDESC *modp, CONFLIST *allconf, ECHOFILE *f)
{
 CONFDAT *cur = 0;
 for (;;) {
	CONFDAT *locked = cur;
	int c = sksp(f);
	if (locked) pthread_mutex_lock(&locked->mx);
	c = runcmd(modp, allconf, &cur, f, c);
	if (locked) pthread_mutex_unlock(&locked->mx);
	if (c != '\n') return c;
 }
}

static int handle_detect(CONFDAT *cp, unsigned i)
{
 if (cp->in[i].detecting) {
	for (;;) {
		SM_RECOGNISED_PARMS rp;
		const char *type;
		char buff[80];
		int err;
		memset(&rp, 0, sizeof(rp));
		rp.channel = cp->in[i].chan;
		err = sm_get_recognised(&rp);
		if (err) {
			fprintf(stderr, "sm_get_recognised returned %d\n", err);
			return 1;
		}
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
		case kSMRecognisedOverrun: type = "Overrun"; break;
		default:
			printf("sm_get_recognised produced event %d\n", rp.type);
			return 0;
		}
		if (!type) break;
		printf("i%d: %s: %d %d\n", i, type, rp.param0, rp.param1);
	}
 }
 return 0;
}

#ifdef USE_PIPE
static int detect(CONFDAT *cp)
{
 for (;;) {
	struct pollfd waitp[arlen(cp->in)+1];
	unsigned u;
	int i;
	for (u=0; u < arlen(cp->in); u++) {
		if (cp->in[u].detecting) {
			waitp[u].fd = cp->in[u].event.fd;
			waitp[u].events = cp->in[u].event.mode;
		} else {
			waitp[u].fd = -1;
			waitp[u].events = 0;
		}
	}
	waitp[u].fd = cp->cmdpipe[0];
	waitp[u].events = POLLIN;
	pthread_mutex_unlock(&cp->mx);
	i = poll(waitp, u+1, -1);
	if (cp->quit) return 0;
	pthread_mutex_lock(&cp->mx);
	if (i < 0) {
		perror("poll() failed");
		continue;	// fd closed while waiting
	}
	for (u=0; i && u < arlen(cp->in); u++) {
		if (waitp[u].revents) {
			if (handle_detect(cp, u)) return 1;
			i--;
		}
	}
	if (i && waitp[u].revents) {
		if (waitp[u].revents & POLLIN) {
			char c;
			i = read(cp->cmdpipe[0], &c, 1);
			if (i < 0) {
				perror("read from cmdpipe failed");
				return 1;
			}
		} else {
			printf("cmdpipe poll status = 0x%x\n",
				waitp[u].revents);
			return 1;
		}
	}
 }
}
#endif

#ifdef TiNGTYPE_WINNT
static int detecthalf(CONFDAT *cp, unsigned base, unsigned limit)
{
 int sts = 0;
 if (limit > arlen(cp->in)) limit = arlen(cp->in);
 for (; !cp->quit;) {
	HANDLE waitp[64];
	unsigned nw = 0;
	int indx[64];
	unsigned u;
	for (u=base; u < limit; u++) {
		if (cp->in[u].detecting) {
	 		waitp[nw] = cp->in[u].event;
	 		indx[nw] = u;
	 		nw++;
		}
	}
	waitp[nw] = cp->cmdevent[!!base];
	indx[nw] = nw;
	pthread_mutex_unlock(&cp->mx);
	u = WaitForMultipleObjects(nw+1, waitp, 0, INFINITE);
	pthread_mutex_lock(&cp->mx);
	if (u == WAIT_FAILED) {
		lasterr("WaitForMultipleObjects failed");
		sts = 1;
		break;
	}
	if (u >= WAIT_OBJECT_0 && u < WAIT_OBJECT_0+nw) {
		if (handle_detect(cp, indx[u-WAIT_OBJECT_0])) {
			sts = 1;
			break;
		}
	} else if (u == WAIT_OBJECT_0 + nw) {
		// just waking us up to re-make list
	} else {
		fprintf(stderr, "WaitForMultipleObjects returned 0x%x\n", u);
		sts = 1;
		break;
	}
 }
 return sts;
}

struct detectfn {
	CONFDAT *cp;
	unsigned base;
	unsigned limit;
};

static void *detecthalf_thread(void *p)
{
 struct detectfn *fnp = (struct detectfn *) p;
 int sts;
 pthread_mutex_lock(&fnp->cp->mx);
 sts = detecthalf(fnp->cp, fnp->base, fnp->limit);
 pthread_mutex_unlock(&fnp->cp->mx);
 return sts;
}

static int detect(CONFDAT *cp)
{
 struct detectfn fns;
 pthread_t id;
 void *sts;
 int e;
 int i;
 fns.cp = cp;
 fns.base = 32;
 fns.limit = 64;
 e = pthread_create(&id, 0, detecthalf_thread, &fns);
 if (e) {
 	fprintf(stderr, "pthread_create returned %d: %s\n", e, strerror(e));
 	return 1;
 }
 i = detecthalf(cp, 0, 32);
 pthread_mutex_unlock(&cp->mx);
 e = pthread_join(id, &sts);
 pthread_mutex_lock(&cp->mx);
 if (e) {
 	fprintf(stderr, "pthread_join returned %d: %s\n", e, strerror(e));
 	return 1;
 }
 return i || sts;
}
#endif

static void *detect_thread(void *p)
{
 CONFDAT *cp = (CONFDAT *) p;
 int sts;
 pthread_mutex_lock(&cp->mx);
 sts = detect(cp);
 pthread_mutex_unlock(&cp->mx);
 return (void *) sts;
}

static int procfiles(CONFLIST *allconf, char *inifile)
{
 MODDESC curmod;
 ECHOFILE f;
 if (cardlist_nthmod(&curmod, &allconf->cardlist, 0)) {
	curmod.id = 0;
	curmod.logname = "(no module)";
 }
 f.ungot = 0;
 if (inifile) {
 	f.f = fopen(inifile, "r");
 	if (!f.f) {
 		perror("Cannot open file");
 		fprintf(stderr, "File: %s\n", inifile);
 		return 1;
	}
	switch (mainloop(&curmod, allconf, &f)) {
	case EOF: break;
	case 'q':
		fprintf(stderr, "quitting inifile\n");
		return 0;
	default:
		fprintf(stderr, "error while processing initfile: %s\n",
			inifile);
		return 1;
	}
 }
 f.f = stdin;
 f.ungot = 0;
 switch (mainloop(&curmod, allconf, &f)) {
 case EOF: break;
 case 'q':
	fprintf(stderr, "quitting\n");
	break;
 default:
	fprintf(stderr, "error while processing stdin\n");
	return 1;
 }
 return 0;
}

static int confprep(CONFDAT *cp)
{
 int e;
 confinit(cp);
#ifdef USE_PIPE
 e = pipe(cp->cmdpipe);
 if (e) {
	perror("pipe() failed");
	return 1;
 }
#endif
#ifdef TiNGTYPE_WINNT
 if (! (cp->cmdevent[0] = CreateEvent(0, 0, 0, 0)) ) {
	lasterr("CreateEvent failed");
	return 1;
 }
 if (! (cp->cmdevent[1] = CreateEvent(0, 0, 0, 0)) ) {
	lasterr("CreateEvent failed");
	CloseHandle(cp->cmdevent[1]);
	return 1;
 }
#endif
 e = pthread_create(&cp->tid, 0, detect_thread, cp);
 if (e) {
 	fprintf(stderr, "pthread_create returned %d: %s\n", e, strerror(e));
 	return 1;
 }
 return 0;
}

static int confstop(CONFDAT *cp)
{
 void *sts;
 int c;
 pthread_mutex_lock(&cp->mx);
 cp->quit = 1;
 pthread_mutex_unlock(&cp->mx);
#ifdef USE_PIPE
 close(cp->cmdpipe[1]);
#endif
#ifdef TiNGTYPE_WINNT
 signal_thread(cp, 0);
 signal_thread(cp, 32);
 CloseHandle(cp->cmdevent[0]);
 CloseHandle(cp->cmdevent[1]);
#endif
 c = pthread_join(cp->tid, &sts);
 if (c) {
 	fprintf(stderr, "pthread_join returned %d: %s\n", c, strerror(c));
 	return 1;
 }
 return (int) sts;
}

static int conftest(char *inifile)
{
 CONFLIST cdat;
 unsigned u;
 int c;
 cardlist(&cdat.cardlist);
 for (u=0; u < arlen(cdat.clist); u++) {
	if (confprep(&cdat.clist[u])) {
		printf("Error initialising conference %d\n", u);
		break;
	}
 }
 c = procfiles(&cdat, inifile);
 for (; u--;) {
	if (confstop(&cdat.clist[u])) {
		printf("Error stoppong conference %d\n", u);
		c = 1;
	}
 }
 return c;
}

#include "gen/hconftest.args.i"

int main(int argc, char **argv)
{
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || *argv) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
	return 1;
 }
 return conftest(arg.inifile);
}
