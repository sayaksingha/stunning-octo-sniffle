
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <smbesp.h>
#include <smdrvr.h>
#include "../Testlib/cardopen.h"
#include "../Testlib/fmtcode.h"
#include "../Testlib/modopen.h"
#include <sys/timeb.h>

#ifdef TiNGTYPE_LINUX
#include <poll.h>
#include <string.h>
#include <unistd.h>
typedef struct pollfd WAITABLE;
#define USE_PIPES
#endif

#ifdef TiNGTYPE_QNX
#include <sys/poll.h>
#include <string.h>
#include <unistd.h>
typedef struct pollfd WAITABLE;
#define USE_PIPES
#endif


#ifdef TiNGTYPE_WINNT
#include <io.h>
#include "../../libutil/WINNT/lasterr.h"
typedef HANDLE WAITABLE;
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

#define arlen(x) (sizeof(x)/sizeof(*(x)))

typedef struct {
	int stream;
	int timeslot;
	int type;
} MVIP;
#define MVIP_STREAM_NONE 0xff

typedef struct {
#ifdef USE_PIPES
	int cmdpipe[2];
#endif
#ifdef TiNGTYPE_WINNT
	HANDLE cmdevent[2];
#endif
	pthread_mutex_t mx;
	unsigned quit;
	struct detect {
		tSMEventId event;
		int detecting;
	} detect[64];
	struct confchan {
		tSMChannelId chan;
		int duplex;		// index of its other entry if duplex
		int id;
		int agc;
		int volume;
		int side;
		MVIP ts;
		unsigned long chanmask[2];
	} conf[2][64];		// conf[0] is outputs, conf[1] is inputs
} CONFDAT;

typedef struct {
	int io;
	int channo;
	struct confchan *cp;
} CONFPOS;

static void confinit(CONFDAT *cp)
{
 unsigned io;
 for (io=0; io < arlen(cp->conf); io++) {
	struct confchan *cc;
	for (cc=cp->conf[io]; cc < cp->conf[io]+arlen(cp->conf[io]); cc++) {
		cc->chan = 0;
 	}
 }
 for (io=0; io < arlen(cp->detect); io++) {
	cp->detect[io].detecting = 0;
 }
 cp->quit = 0;
}

static int signal_thread(CONFDAT *cp, int channo)
{
#ifdef USE_PIPES
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
 unsigned io;
 for (io=0; io < arlen(cp->conf); io++) {
	unsigned i;
	printf(io ? "Inputs:\n" : "Outputs\n");
	for (i=0; i < arlen(cp->conf[io]); i++) {
		struct confchan *const cc = &cp->conf[io][i];
		if (cc->chan) {
			char vol[10];
			char ts[20];
			unsigned j;
			switch (cc->ts.type) {
			case kSMTimeslotTypeALaw: sprintf(vol, "a"); break;
			case kSMTimeslotTypeMuLaw: sprintf(vol, "u"); break;
			case kSMTimeslotTypeData: sprintf(vol, "r"); break;
			default: sprintf(vol, "%d", cc->ts.type); break;
			}
			sprintf(ts, "%d:%d:%s", cc->ts.stream, cc->ts.timeslot, vol);
			sprintf(vol, "%c%d", cc->agc ? 'A' : 'v', cc->volume);
			printf("%2d %4s %7s",
				i,
				vol,
				ts);
			for (j=0; j<arlen(cc->chanmask); j++) {
				unsigned long l = cc->chanmask[j];
				unsigned k;
				for (k=0; k<32; k++) {
					putchar(".+"[l & 1]);
					l >>= 1;
				}
			}
			if (cc->side != -1) printf(" s%d", cc->side);
			printf("\n");
		}
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

static int iseol(FILE *f)
{
 int c = sksp(f);
 ungetc(c, f);
 return c == '\n';
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
 for (;;) {		// decimal
	int d;
	if (c >= '0' && c <= '9') d = c - '0';
	else if (c >= 'a' && c <= 'f') d = c - 'a' + 10;
	else break;
	if (d >= base) break;
	num = num*base - d;
	c = getc(f);
 }
 ungetc(c, f);
 return neg ? num : -num;
}

	// read float in format: SD [ . D] [{e|E} SD]
	// S = + | - |
	// D = {0-9}*
	// examples: 3 = 3. = 3.e = 3.e0 = 3e+0 = 3.0e+0
	//	.3 = 0.3 = 00.3 = .3e = .3e-0 = 3e-1 = 3.0e-1
static float getflo(FILE *f)
{
 int c = sksp(f);
 float num = 0;
 int neg = 0;
 if (c == '+') c=getc(f);
 if (c == '-') c=getc(f), neg = 1;
 for (;;) {		// decimal
	if (c < '0' || c > '9') break;
	c -= '0';
	num = num * 10 + c;
	c = getc(f);
 }
 if (c == '.') {
 	float frac = 1;
 	for (;;) {
		c = getc(f);
		if (c < '0' || c > '9') break;
 		c -= '0';
 		frac /= 10;
 		num += c * frac;
	}
 }
 if (c == 'e' || c == 'E') {
	int expo = 0;
	int nexp = 0;
	c = getc(f);
	if (c == '+') c=getc(f);
	if (c == '-') c=getc(f), nexp = 1;
	for (;;) {
		if (c < '0' || c > '9') break;
		c -= '0';
		expo = expo * 10 - c;
		c = getc(f);
	}
	if (!nexp) expo = -expo;
	num *= pow(10.0, expo);
 }
 ungetc(c, f);
 return neg ? -num : num;
}

static int getchan(CONFPOS *pp, CONFDAT *cp, FILE *f)
{
 int channo = getnum(f);
 if (channo < 0) {
	printf("%sput channel number is negative (%d)\n", pp->io ? "In" : "Out", channo);
	return 1;
 }
 if ((unsigned) channo > arlen(cp->conf[pp->io])) {
	printf("%sput channel number too big (%d)\n", pp->io ? "In" : "Out", channo);
	return 1;
 }
 pp->cp = &cp->conf[pp->io][channo];
 if (!pp->cp->chan) {
	printf("%sput channel not opened (%d)\n", pp->io ? "In" : "Out", channo);
	return 1;
 }
 pp->channo = channo;
 return 0;
}

static int getpos(CONFPOS *pp, CONFDAT *cp, FILE *f)
{
 int c = sksp(f);
 if (c == 'i') pp->io = 1;
 else if (c == 'o') pp->io = 0;
 else {
 	printf("Expected 'i' or 'o', got '%c'\n", c);
 	return 1;
 }
 return getchan(pp, cp, f);
}

static int needio(CONFPOS *pp, int io, const char *msg)
{
 if (pp->io != io) {
	static const char *ioname[2] = { "out", "in" };
	printf("%s must be an %sput channel, not an %sput\n",
		msg, ioname[io], ioname[!io]);
	return 1;
 }
 return 0;
}

static void do_new(CONFDAT *cp, FILE *f, tSMModuleId module)
{
 SM_SWITCH_CHANNEL_PARMS sp;
 SM_CHANNEL_ALLOC_PLACED_PARMS pp;
 int io = sksp(f);
 int *duplexp = 0;
 int prevdup = -1;
 unsigned i;
 int c;
 memset(&sp, 0, sizeof(sp));
 if (io != 'b' && io != 'i' && io != 'o') {
 	printf("expected 'b', 'i' or 'o', got '%c'\n", io);
 	ungetc(io, f);
 	return;
 }
 sp.st = getnum(f);
 c = sksp(f);
 if (c != ':') {
 	printf("expected ':' after stream, got '%c'\n", c);
 	ungetc(c, f);
 	return;
 }
 sp.ts = getnum(f);
 c = getc(f);
 if (c == ':') {
 	c = getc(f);
 	switch (c) {
	case 'a': sp.type = kSMTimeslotTypeALaw; break;
	case 'u': sp.type = kSMTimeslotTypeMuLaw; break;
	case 'r': sp.type = kSMTimeslotTypeData; break;
	default:
		printf("expected 'a', 'u', or 'r' after ':', got '%c'\n", c);
		ungetc(c, f);
		return;
	}
	c = sksp(f);
 }
 ungetc(c, f);
 if (c != '\n') {
	printf("got '%c' after command\n", c);
	return;
 }
 memset(&pp, 0, sizeof(pp));
 switch (io) {
 case 'b': pp.type = kSMChannelTypeFullDuplex; break;
 case 'i': pp.type = kSMChannelTypeInput; break;
 case 'o': pp.type = kSMChannelTypeOutput; break;
 }
 pp.module = module;
 c = sm_channel_alloc_placed(&pp);
 if (c) {
	printf("sm_channel_alloc_placed returned %d\n", c);
	return;
 }
 sp.channel = pp.channel;
 if (io != 'o') {
	int r = sm_switch_channel_input(&sp);
	if (r) {
		printf("sm_switch_channel_input returned %d\n", r);
		r = sm_channel_release(pp.channel);
		if (r) printf("sm_release_channel returned %d\n", r);
		return;
	}
 }
 if (io != 'i') {
	int r = sm_switch_channel_output(&sp);
	if (r) {
		printf("sm_switch_channel_output returned %d\n", r);
		r = sm_channel_release(pp.channel);
		if (r) printf("sm_release_channel returned %d\n", r);
		return;
	}
 }
 for (c=0; c<2; c++) {
 	if (!c && io == 'i') continue;
 	if (c && io == 'o') continue;
	for (i=0; ; i++) {
		struct confchan *const cc = &cp->conf[c][i];
		if (i >= arlen(cp->conf[c])) {
			printf("Too many channels to start another\n");
			c = sm_channel_release(pp.channel);
			if (c) printf("sm_release_channel returned %d\n", c);
			return;
		}
		if (!cc->chan) {
			unsigned j;
			cc->ts.stream = sp.st;
			cc->ts.timeslot = sp.ts;
			cc->ts.type = sp.type;
			cc->chan = pp.channel;
			cc->volume = 0;
			cc->agc = 0;
			cc->side = -1;
			cc->id = -1;
			cc->duplex = prevdup;
			if (duplexp) *duplexp = i;
			duplexp = &cc->duplex;
			prevdup = i;
			printf("new %c: %d %d\n", io, c, i);
			for (j=0; j < arlen(cc->chanmask); j++) {
				cc->chanmask[j] = 0;
			}
			break;
		}
	}
 }
}

static void do_start(CONFPOS *dst, CONFDAT *cp, FILE *f)
{
 SM_CONF_PRIM_START_PARMS sp;
 SM_CONF_PRIM_ATTACH_PARMS ap;
 (void) cp;
 memset(&sp, 0, sizeof(sp));
 memset(&ap, 0, sizeof(ap));
 ap.channel = sp.channel = dst->cp->chan;
 for (;;) {
	int c = sksp(f);
	if (c == 'a' && !dst->io) {
		sp.agc = getnum(f);
	} else if (c == 'v' && !dst->io) {
		sp.volume = getnum(f);
	} else if (c == 't') {
		ap.conf_type = sp.conf_type = getnum(f);
	} else if (c == '\n') {
		ungetc(c, f);
		break;
	} else {
		printf("got '%c' after command\n", c);
		return;
	}
 }
 if (dst->io) {
	int c = sm_conf_prim_attach(&ap);
	if (c) printf("sm_conf_prim_attach() returned %d\n", c);
 } else {
	int c = sm_conf_prim_start(&sp);
	if (c) printf("sm_conf_prim_start() returned %d\n", c);
	else {
		dst->cp->agc = sp.agc;
		dst->cp->volume = sp.volume;
	}
 }
}

static void do_clone(CONFPOS *dst, CONFPOS *src, CONFDAT *cp, FILE *f)
{
 SM_CONF_PRIM_CLONE_PARMS pp;
 int r;
 (void) cp;
 (void) f;
 if (needio(dst, 0, "clone target") || needio(src, 0, "clone model")) return;
 memset(&pp, 0, sizeof(pp));
 pp.channel = dst->cp->chan;
 pp.model = src->cp->chan;
 r = sm_conf_prim_clone(&pp);
 if (r) printf("sm_conf_prim_clone() returned %d\n", r);
 else {
 	unsigned j;
 	dst->cp->agc = src->cp->agc;
 	dst->cp->volume = src->cp->volume;
	for (j=0; j < arlen(dst->cp->chanmask); j++) {
		dst->cp->chanmask[j] = src->cp->chanmask[j];
	}
 }
}

static void do_sidetone(CONFPOS *dst, CONFDAT *cp, FILE *f)
{
 SM_SET_SIDETONE_CHANNEL_PARMS pp;
 int r = sksp(f);
 memset(&pp, 0, sizeof(pp));
 if (r == '\n') {
	pp.output = kSMNullChannelId;
	dst->cp->side = -1;
	ungetc(r, f);
 } else if (r != 'o') {
 	printf("expected 'o', got '%c'\n", r);
 	return;
 } else {
	CONFPOS src;
	src.io = 0;
	if (getchan(&src, cp, f)) return;
	pp.output = src.cp->chan;
	dst->cp->side = src.channo;
 }
 pp.channel = dst->cp->chan;
 r = sm_set_sidetone_channel(&pp);
 if (r) printf("sm_set_sidetone_channel returned %d\n", r);
}
 
static void do_active(CONFPOS *dst, CONFDAT *cp, FILE *f)
{
 SM_CONF_PRIM_INFO_PARMS pp;
 int r;
 (void) f;
 memset(&pp, 0, sizeof(pp));
 pp.channel = dst->cp->chan;
 r = sm_conf_prim_info(&pp);
 if (r) printf("sm_conf_prim_info returned %d\n", r);
 else {
	unsigned i;
	printf("%2d:            ", pp.participant_count);
	for (i=0; i < arlen(cp->conf[1]); i++) {
		struct confchan *const cc = &cp->conf[1][i];
		int c = cc->id;
		if (cc->chan && c != -1) {
			c = pp.speakers[c >> 3] >> (c & 7);
		} else {
			c = 0;
		}
		putchar(".+"[c & 1]);
	 }
	 printf("\n");
 }
}
 
static void do_echocan(CONFPOS *dst, CONFDAT *cp, FILE *f)
{
 SM_CONDITION_INPUT_PARMS pp;
 int r = sksp(f);
 CONFPOS src;
 memset(&pp, 0, sizeof(pp));
 if (r == '\n') {
	pp.reference_type = kSMInputCondRefNone;
	pp.conditioning_type = kSMInputCondNone;
	dst->cp->side = -1;
	ungetc(r, f);
 } else if (ungetc(r, f), getpos(&src, cp, f)) {
 	return;
 } else {
	pp.reference = src.cp->chan;
	pp.reference_type = src.io
		? kSMInputCondRefUseInput
		: kSMInputCondRefUseOutput;
	pp.conditioning_type = kSMInputCondEchoCancelation;
	dst->cp->side = src.channo;
 }
 pp.channel = dst->cp->chan;
 r = sm_condition_input(&pp);
 if (r) printf("sm_condition_input returned %d\n", r);
}
 
/* 
 *	eN	silence elimination
 *	fNAME	filename
 *	mN	tone elimination mode
 *	sN	tone set ID
 *	tTYPE	type
 *	xN	max octets
 */
static enum {RP_OK, RP_ERR, RP_DONE } read_record_param(FILE **outf, SM_RECORD_PARMS *pp, FILE *f)
{
 int r = sksp(f);
 switch (r) {
 case '?':
	printf("Record parameters:\n"
		"\teN\tsilence elimination N\n"
		"\tfNAME\tfilename NAME\n"
		"\tmN\ttone elimination mode N\n"
		"\tsN\ttone set ID N\n"
		"\ttTYPE\trecord type TYPE\n"
		"\txN\tmax octets N\n");
	return RP_ERR;
 case 'e':
	pp->silence_elimination = getnum(f);
	break;
 case 'f': {
	char buff[80];
	unsigned i;
	r = sksp(f);
	for (i=0;;) {
		if (r == '\n' || r == ' ' || r == '\t') break;
		buff[i++] = r;
		if (i >= arlen(buff)) {
			buff[arlen(buff)-1] = 0;
			printf("record file name too long: %s...\n",
				buff);
			return RP_ERR;
		}
		r = getc(f);
	}
	ungetc(r, f);
	buff[i] = 0;
	*outf = fopen(buff, "wb");
	if (!*outf) {
		perror("Cannot create file");
		printf("File: '%s'\n", buff);
		return RP_ERR;
	}
 }
	break;
 case 'm':
	r = getnum(f);
	switch (r) {
	case 1:
	case 32:
		pp->tone_elimination_mode = kSMToneDetectionNoMinDuration;
		break;
	case 40:
		pp->tone_elimination_mode = kSMToneDetectionMinDuration40;
		break;
	case 64:
		pp->tone_elimination_mode = kSMToneDetectionMinDuration64;
		break;
	default:
		printf("Bad tone mode (%d) expected 1, 32, 40, or 64\n",
			r);
		return RP_ERR;
	}
	break;
 case 's':
	pp->tone_elimination_set_id = getnum(f);
	break;
 case 't': {
	char buff[80];
	unsigned i;
	r = sksp(f);
	if (r == '?') {
		printf("Record types:");
		fmtlist_dump(stdout);
		return RP_ERR;
	}
	for (i=0;;) {
		if (r == '\n' || r == ' ' || r == '\t') break;
		buff[i++] = r;
		if (i >= arlen(buff)) {
			buff[arlen(buff)-1] = 0;
			printf("record type name too long: %s...\n",
				buff);
			return RP_ERR;
		}
		r = getc(f);
	}
	ungetc(r, f);
	buff[i] = 0;
	if (fmtcode(&pp->type, &pp->sampling_rate, buff)) {
		printf("Unknown recording type: %s\n", buff);
		return RP_ERR;
	}
 }
	break;
 case 'x':
	pp->max_octets = getnum(f);
	break;
 case '\n':
	ungetc(r, f);
	return RP_DONE;
 default:
	printf("Unknwon record parameter code: '%c'\n", r);
	return RP_ERR;
 }
 return RP_OK;
}

static int xfer_record(FILE *outf, tSMChannelId chan, unsigned maxo)
{
 unsigned long totnc = 0;
 struct timeb start;
 int twid = 0;
 ftime(&start);
 if (!maxo) maxo = 5 * 8000;
 for (; totnc < maxo;) {
	struct sm_ts_data_parms dp;
	struct timeb now;
	char buff[16384];
	double dtim;
	unsigned nc;
	int err;
	memset(&dp, 0, sizeof(dp));
	dp.channel = chan;
	dp.data = buff;
	dp.length = arlen(buff);
	err = sm_get_recorded_data(&dp);
	ftime(&now);
	if (err) {
		fprintf(stderr, "sm_get_recorded_data returned %d\n", err);
		break;
	}
	nc = fwrite(buff, 1, dp.length, outf);
	if (nc != (unsigned) dp.length) {
		if (ferror(outf)) {
			perror("fwrite failed");
		} else {
			fprintf(stderr, "Parital write: %d of %d\n", nc, dp.length);
		}
		err = 1;
		break;
	}
	totnc += nc;
	ftime(&now);
	dtim = now.time - start.time
		+ (now.millitm - start.millitm)
		/ 1000.0;
	if (!dtim) dtim = 0.001;
	fprintf(stderr, "\r%d bps %c    ",
		(int) (8 * totnc / dtim),
		"|/-\\"[twid]);
	twid = (twid+1) & 3;
	fflush(stdout);
 }
 return 0;
}

static void do_record(CONFPOS *dst, CONFDAT *cp, FILE *f)
{
 SM_RECORD_PARMS pp;
 FILE *outf = 0;
 (void) cp;
 if (needio(dst, 0, "record target")) return;
 memset(&pp, 0, sizeof(pp));
 for (;;) {
 	switch (read_record_param(&outf, &pp, f)) {
 		int r;
 	case RP_OK: break;
 	case RP_ERR:
 		if (outf) fclose(outf);
 		return;
 	case RP_DONE:
		pp.channel = dst->cp->chan;
		r = sm_record_start(&pp);
		if (r) {
			printf("sm_record_start returned %d\n", r);
			return;
		}
		r = xfer_record(outf, dst->cp->chan, pp.max_octets);
 		if (outf) fclose(outf);
 		printf("\n");
		return;
	}
 }
}

static void do_adjust(CONFPOS *dst, CONFDAT *cp, FILE *f)
{
 int agc = 0, volume = 0;
 int c = sksp(f);
 (void) cp;
 if (c == 'a') {
	agc = getnum(f);
	c = sksp(f);
 }
 if (c == 'v') {
	volume = getnum(f);
	c = sksp(f);
 }
 ungetc(c, f);
 if (c != '\n') {
	printf("got '%c' after command\n", c);
	return;
 }
 if (dst->io) {
 	SM_CONF_PRIM_ADJ_INPUT_PARMS pp;
 	int r;
	memset(&pp, 0, sizeof(pp));
	pp.channel = dst->cp->chan;
	pp.agc = agc;
	pp.volume = volume;
	r = sm_conf_prim_adj_input(&pp);
	if (r) {
		printf("sm_conf_prim_adj_input returned %d\n", r);
		return;
	}
 } else {
 	SM_CONF_PRIM_ADJ_OUTPUT_PARMS pp;
 	int r;
	memset(&pp, 0, sizeof(pp));
	pp.channel = dst->cp->chan;
	pp.agc = agc;
	pp.volume = volume;
	r = sm_conf_prim_adj_output(&pp);
	if (r) {
		printf("sm_conf_prim_adj_output returned %d\n", r);
		return;
	}
 }
 dst->cp->agc = agc;
 dst->cp->volume = volume;
}

static void do_adjust_track(CONFPOS *dst, CONFDAT *cp, FILE *f)
{
 float noise = 0, thresh = 0;
 int c = sksp(f);
 (void) cp;
 if (c == 'n') {
	noise = getflo(f);
	c = sksp(f);
 }
 if (c == 't') {
	thresh = getflo(f);
	c = sksp(f);
 }
 ungetc(c, f);
 if (c != '\n') {
	printf("got '%c' after command\n", c);
	return;
 }
 if (dst->io) {
 	SM_CONF_PRIM_ADJ_TRACKING_PARMS pp;
 	int r;
	memset(&pp, 0, sizeof(pp));
	pp.channel = dst->cp->chan;
	pp.min_noise_level = noise;
	pp.speech_thresh = thresh;
	r = sm_conf_prim_adj_tracking(&pp);
	if (r) {
		printf("sm_conf_prim_adj_tracking returned %d\n", r);
		return;
	}
 } else {
	printf("Can't adjust tracking on output\n");
 }
}

static void do_add(CONFPOS *dst, CONFPOS *src, CONFDAT *cp, FILE *f)
{
 SM_CONF_PRIM_ADD_PARMS pp;
 int r;
 (void) f;
 (void) cp;
 if (needio(dst, 0, "add target") || needio(src, 1, "add source")) return;
 memset(&pp, 0, sizeof(pp));
 pp.channel = dst->cp->chan;
 pp.participant = src->cp->chan;
 for (;;) {
 	int c = sksp(f);
 	if (c == 'f') {
 		pp.factor = getflo(f);
	} else if (c == '\n') {
		ungetc(c, f);
		break;
	} else {
		printf("got '%c' after command\n", c);
		return;
	}

 }
 r = sm_conf_prim_add(&pp);
 if (r) printf("sm_conf_prim_add returned %d\n", r);
 else {
	src->cp->id = pp.id;
	dst->cp->chanmask[src->channo >> 5] |= 1 << (src->channo & 0x1f);
	src->cp->chanmask[dst->channo >> 5] |= 1 << (dst->channo & 0x1f);
 }
}

static void do_leave(CONFPOS *dst, CONFPOS *src, CONFDAT *cp, FILE *f)
{
 SM_CONF_PRIM_LEAVE_PARMS pp;
 int r;
 (void) cp;
 (void) f;
 if (needio(dst, 0, "leave target") || needio(src, 1, "leave source")) return;
 memset(&pp, 0, sizeof(pp));
 pp.channel = dst->cp->chan;
 pp.id = src->cp->id;
 r = sm_conf_prim_leave(&pp);
 if (r) printf("sm_conf_prim_leave returned %d\n", r);
 else {
	dst->cp->chanmask[src->channo >> 5] &= ~(1 << (src->channo & 0x1f));
	src->cp->chanmask[dst->channo >> 5] &= ~(1 << (dst->channo & 0x1f));
 }
}

static void do_abort(CONFPOS *dst, CONFDAT *cp, FILE *f)
{
 int r;
 (void) f;
 if (dst->io) {
 	SM_CONF_PRIM_DETACH_PARMS pp;
 	memset(&pp, 0, sizeof(pp));
 	pp.channel = dst->cp->chan;
	for (;;) {
		int c = sksp(f);
		if (c == 't') {
			pp.conf_type = getnum(f);
		} else if (c == '\n') {
			ungetc(c, f);
			break;
		} else {
			printf("got '%c' after command\n", c);
			return;
		}
	 }
 	r = sm_conf_prim_detach(&pp);
	if (r) printf("sm_conf_prim_detach returned %d\n", r);
 } else {
	if (!iseol(f)) {
		printf("got junk after command\n");
		return;
	}
	r = sm_conf_prim_abort(dst->cp->chan);
	if (r) printf("sm_conf_prim_abort returned %d\n", r);
	else {
		unsigned i;
		for (i=0; i < arlen(dst->cp->chanmask); i++) {
			unsigned j;
			for (j=0; j<32; j++) {
				if (dst->cp->chanmask[i] & (1 << j)) {
					cp->conf[1][i*32+j].chanmask[dst->channo >> 5] &= ~(1 << (dst->channo & 0x1f));
				}
			}
			dst->cp->chanmask[i] = 0;
		}
		dst->cp->agc = 0;
		dst->cp->volume = 0;
 	}
 }
}

static void do_close(CONFPOS *inp, CONFDAT *cp, FILE *f)
{
 (void) f;
 if (inp->cp->duplex != -1) {
	cp->conf[!inp->io][inp->cp->duplex].duplex = -1;
	printf("duplex channel: will be released when other half is closed\n");
 } else {
	int r = sm_channel_release(inp->cp->chan);
	if (r) printf("sm_channel_release returned %d\n", r);
 }
 inp->cp->chan = 0;
}

/* detection parameters:
 *	mD	tone detection mode D mS
 *	sS	active tone set S
 *	a	map tones to digits
 *	c	cptone
 *	gN	grunt detection N mS
 */
static void do_detect(CONFPOS *inp, CONFDAT *cp, FILE *f)
{
 SM_LISTEN_FOR_PARMS lp;
 if (needio(inp, 1, "detect")) return;
 memset(&lp, 0, sizeof(lp));
 for (;;) {
 	int c = sksp(f);
 	switch (c) {
 	case 'm':
 		c = getnum(f);
 		switch (c) {
 		case 0:
 			lp.tone_detection_mode = kSMToneDetectionNone;
 			break;
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
		ungetc(c, f);
 		if (cp->detect[inp->channo].detecting) {
 			SM_CHANNEL_SET_EVENT_PARMS ep;
			cp->detect[inp->channo].detecting = 0;
 			memset(&ep, 0, sizeof(ep));
			ep.channel = inp->cp->chan;
			ep.event_type = kSMEventTypeRecog;
			ep.issue_events = kSMChannelNoEvent;
			c = sm_channel_set_event(&ep);
			if (c) printf("sm_channel_set_event returned %d\n", c);
 			smd_ev_free(cp->detect[inp->channo].event);
 		}
 		lp.channel = inp->cp->chan;
 		c = sm_listen_for(&lp);
 		if (c) {
 			printf("sm_listen_for returned %d\n", c);
 			return;
 		} else {
 			SM_CHANNEL_SET_EVENT_PARMS ep;
 			memset(&ep, 0, sizeof(ep));
 			c = smd_ev_create(&ep.event, inp->cp->chan,
 				kSMEventTypeRecog, kSMChannelSpecificEvent);
			if (c) {
				printf("smd_ev_create returned %d\n", c);
				// cancel listen?
				return;
			}
			ep.channel = inp->cp->chan;
			ep.event_type = kSMEventTypeRecog;
			ep.issue_events = kSMChannelSpecificEvent;
			c = sm_channel_set_event(&ep);
			if (c) {
				printf("sm_channel_set_event returned %d\n", c);
				smd_ev_free(ep.event);
			} else {
				cp->detect[inp->channo].event = ep.event;
				cp->detect[inp->channo].detecting = 1;
			}
 		}
 		signal_thread(cp, inp->channo);
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

static void do_stop(CONFPOS *dst, CONFDAT *cp, FILE *f)
{
 int r;
 SM_CONF_PRIM_STOP_PARMS sp;
 if (needio(dst, 0, "stop")) return;
 memset(&sp,0,sizeof(sp));
 sp.channel = dst->cp->chan;
 sp.no_wait = getnum(f);
 r = sm_conf_prim_stop(&sp);
 if (r) printf("sm_conf_prim_stop returned %d\n", r);
 else if (!sp.no_wait) {
	unsigned i;
	for (i=0; i < arlen(dst->cp->chanmask); i++) {
		unsigned j;
		for (j=0; j<32; j++) {
			if (dst->cp->chanmask[i] & (1 << j)) {
				cp->conf[1][i*32+j].chanmask[dst->channo >> 5] &= ~(1 << (dst->channo & 0x1f));
			}
		}
		dst->cp->chanmask[i] = 0;
	}
	dst->cp->agc = 0;
	dst->cp->volume = 0;
 }
}

static void do_status(CONFPOS *dst, CONFDAT *cp, FILE *f)
{
 int r;
 SM_CONF_PRIM_STATUS_PARMS sp;
 (void) f;
 if (needio(dst, 0, "status")) return;
 memset(&sp,0,sizeof(sp));
 sp.channel = dst->cp->chan;
 r = sm_conf_prim_status(&sp);
 if (r) printf("sm_conf_prim_status returned %d\n", r);
 else {
	 printf("Status: ");
	 switch (sp.status) {
	 case kSMConfStatusRunning:
		 printf("Running\n");
		 break;
	 case kSMConfStatusStopped:
		 printf("Stopped\n");
		 {
			unsigned i;
			for (i=0; i < arlen(dst->cp->chanmask); i++) {
				unsigned j;
				for (j=0; j<32; j++) {
					if (dst->cp->chanmask[i] & (1 << j)) {
						cp->conf[1][i*32+j].chanmask[dst->channo >> 5] &= ~(1 << (dst->channo & 0x1f));
					}
				}
				dst->cp->chanmask[i] = 0;
			}
			dst->cp->agc = 0;
			dst->cp->volume = 0;
		 }
		 break;
	 case kSMConfStatusActiveInputs:
		 {
			unsigned i;
			printf("Active inputs = ");
			for (i=0; i < arlen(sp.u.active_inputs.input); i++) {
				if (sp.u.active_inputs.input[i].id != -1)
					printf(" {%d, %d}", sp.u.active_inputs.input[i].id, sp.u.active_inputs.input[i].power);
			}
			printf("\n");
		 }
		 break;
	 default:
		 printf("Unknown (%d)\n",sp.status);
		 break;
	 }
 }
}

static void do_aidelay(CONFPOS *dst, CONFDAT *cp, FILE *f)
{
 int r;
 SM_CONF_PRIM_CONFIG_ACTIVITY_REPORTING_PARMS sp;
 (void) cp;
 if (needio(dst, 0, "active input delay")) return;
 memset(&sp,0,sizeof(sp));
 sp.channel = dst->cp->chan;
 sp.delay = getnum(f);
 sp.sensitivity = getnum(f);
 r = sm_conf_prim_config_activity_reporting(&sp);
 if (r) printf("sm_conf_prim_config_activity_reporting returned %d\n", r);
}

/*
 * Commands:
 *	+[io]N:N	new input/output on timeslot N:N
 *	oN s aA vV tT	start output, agc A, volume V, type T
 *	oN c oN		clone output from output
 *	oN + iN		add input to output
 *	oN - iN		make input leave output
 *	[io]N aA vV	adjust input/output to agc A, volume V
 *	oN q		abort output
 *	iN t oN		set sidetone for input to be this output
 *	iN d ...	start detection
 */
static int runcmd(CONFDAT *cp, FILE *f, int c, tSMModuleId module)
{
 switch (c) {
	CONFPOS pos;
 case '?':
	printf("Commands:\n"
		"		show current status\n"
		"+[bio]N:N	new input/output on timeslot N:N\n"
		"iN s tT		attach input, type T\n"
		"oN s aA vV tT	start output, agc A, volume V, type T\n"
		"oN c oN		clone output from output\n"
		"oN + iN fF		add input to output, factor F\n"
		"oN - iN		make input leave output\n"
		"oN A		show active participants\n"
		"oN r ...		record output\n"
		"oN S no_wait	stop output\n"
		"oN u			check output status\n"
		"oN i delay sensitivity		set active input reporting\n"
		"[io]N aA vV	adjust input/output to agc A, volume V\n"
		"[io]N q		detach/abort input/output\n"
		"iN t oN		set sidetone for input to be this output\n"
		"iN T nN tT		set input tracking to min noise N, threshold T\n"
		"iN e [io]N		enables echo cancellation on input wrt output\n"
		"iN d ...		enable tone detection\n"
		"[io]N R		close channel\n"
		"#anything		comment\n"
		);
	break;
#if 0
 case '.': showconfinternals(); break;
#endif
 case EOF:
 case 'q': return c;
 case '\n': showconf(cp); ungetc(c, f); break;
 case '+': do_new(cp, f, module); break;
 case 'i':
 case 'o':
	ungetc(c, f);
	if (!getpos(&pos, cp, f)) {
		c = sksp(f);
		switch (c) {
			CONFPOS src;
		case 'A':
			do_active(&pos, cp, f);
			break;
		case 'c':
			if (!getpos(&src, cp, f) && iseol(f)) do_clone(&pos, &src, cp, f);
			break;
		case 'd':
			do_detect(&pos, cp, f);
			break;
		case 'e':
			do_echocan(&pos, cp, f);
			break;
		case 'i':
			do_aidelay(&pos, cp, f);
			break;
		case 'q':
			do_abort(&pos, cp, f);
			break;
		case 'r':
			do_record(&pos, cp, f);
			break;
		case 's':
			do_start(&pos, cp, f);
			break;
		case 'S':
			do_stop(&pos, cp, f);
			break;
		case 't':
			do_sidetone(&pos, cp, f);
			break;
		case 'u':
			do_status(&pos, cp, f);
			break;
		case 'R':
			if (iseol(f)) do_close(&pos, cp, f);
			break;
		case '+':
			if (!getpos(&src, cp, f)) do_add(&pos, &src, cp, f);
			break;
		case '-':
			if (!getpos(&src, cp, f) && iseol(f)) do_leave(&pos, &src, cp, f);
			break;
		case 'T':
			do_adjust_track(&pos, cp, f);
			break;
		case 'a':
		case 'v':
			ungetc(c, f);
			do_adjust(&pos, cp, f);
			break;
		case '\n':
			ungetc(c, f);
			fprintf(stderr, "Missing i/o command\n");
			break;
		default:
			fprintf(stderr, "Bad i/o command: %c\n", c);
			break;
		}
	}
	break;
 default:
	fprintf(stderr, "Bad command: %c\n", c);
	break;
 case '#':
	for (;;) {
		c = getc(f);
		if (c == EOF) return c;
		putchar(c);
		if (c == '\n') return c;
	}
	break;
 }
 for (;;) {
	c = getc(f);
	if (c == EOF || c == '\n') return c;
 }
}

static int mainloop(CONFDAT *cp, FILE *f, tSMModuleId module)
{
 for (;;) {
	int c = sksp(f);
	pthread_mutex_lock(&cp->mx);
	c = runcmd(cp, f, c, module);
	pthread_mutex_unlock(&cp->mx);
	if (c != '\n') return c;
 }
}

static int handle_detect(CONFDAT *cp, unsigned i)
{
 if (cp->detect[i].detecting) {
	for (;;) {
		SM_RECOGNISED_PARMS rp;
		const char *type;
		char buff[80];
		int err;
		memset(&rp, 0, sizeof(rp));
		rp.channel = cp->conf[1][i].chan;
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

#ifdef USE_PIPES
static int detect(CONFDAT *cp)
{
 for (;;) {
	struct pollfd waitp[arlen(cp->detect)+1];
	unsigned u;
	int i;
	for (u=0; u < arlen(cp->detect); u++) {
		if (cp->detect[u].detecting) {
			waitp[u].fd = cp->detect[u].event.fd;
			waitp[u].events = cp->detect[u].event.mode;
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
		return 1;
	}
	for (u=0; i && u < arlen(cp->detect); u++) {
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
 if (limit > arlen(cp->detect)) limit = arlen(cp->detect);
 pthread_mutex_lock(&cp->mx);
 for (; !cp->quit;) {
	HANDLE waitp[64];
	unsigned nw = 0;
	int indx[64];
	unsigned u;
	for (u=base; u < limit; u++) {
		if (cp->detect[u].detecting) {
	 		waitp[nw] = cp->detect[u].event;
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
 pthread_mutex_unlock(&cp->mx);
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
 return (void *) detecthalf(fnp->cp, fnp->base, fnp->limit);
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
 e = pthread_join(id, &sts);
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
#ifndef TiNGTYPE_WINNT
 // for WINNT detecthalf handles locking
 pthread_mutex_lock(&cp->mx);
#endif
 sts = detect(cp);
#ifndef TiNGTYPE_WINNT
 pthread_mutex_unlock(&cp->mx);
#endif
 return (void *) sts;
}

static int procfiles(CONFDAT *cp, char *inifile, tSMModuleId module)
{
 FILE *f;
 if (inifile) {
 	f = fopen(inifile, "r");
 	if (!f) {
 		perror("Cannot open file");
 		fprintf(stderr, "File: %s\n", inifile);
 		return 1;
	}
	switch (mainloop(cp, f, module)) {
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
 switch (mainloop(cp, stdin, module)) {
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

static int conftest(char *inifile, tSMModuleId module)
{
 pthread_t id;
 CONFDAT cdat;
 void *sts;
 int e;
 int c;
 confinit(&cdat);
#ifdef USE_PIPES
 e = pipe(cdat.cmdpipe);
 if (e) {
	perror("pipe() failed");
	return 1;
 }
#endif
#ifdef TiNGTYPE_WINNT
 if (! (cdat.cmdevent[0] = CreateEvent(0, 0, 0, 0)) ) {
	lasterr("CreateEvent failed");
	return 1;
 }
 if (! (cdat.cmdevent[1] = CreateEvent(0, 0, 0, 0)) ) {
	lasterr("CreateEvent failed");
	CloseHandle(cdat.cmdevent[1]);
	return 1;
 }
#endif
 e = pthread_create(&id, 0, detect_thread, &cdat);
 if (e) {
 	fprintf(stderr, "pthread_create returned %d: %s\n", e, strerror(e));
 	return 1;
 }
 c = procfiles(&cdat, inifile, module);
 pthread_mutex_lock(&cdat.mx);
 cdat.quit = 1;
 pthread_mutex_unlock(&cdat.mx);
#ifdef USE_PIPES
 close(cdat.cmdpipe[1]);
#endif
#ifdef TiNGTYPE_WINNT
 signal_thread(&cdat, 0);
 signal_thread(&cdat, 32);
 CloseHandle(cdat.cmdevent[0]);
 CloseHandle(cdat.cmdevent[1]);
#endif
 e = pthread_join(id, &sts);
 if (e) {
 	fprintf(stderr, "pthread_join returned %d: %s\n", e, strerror(e));
 	return 1;
 }
 return c || sts;
}

#include "gen/conftest.args.i"

int main(int argc, char **argv)
{
 tSMModuleId mod;
 tSMCardId card;
 int iErr;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || *argv) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
	return 1;
 }
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
 iErr = conftest(arg.inifile, mod);
 if (arg.serialnumber) {
	modclose(mod);
	cardclose(card);
 }
 return iErr;
}
