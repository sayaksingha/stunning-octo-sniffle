/* detraw.c - test/demo of detecting tones on local timeslots */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>

#include "smbesp.h"
#include "smdrvr.h"
#include "../Testlib/cardopen.h"
#include "../Testlib/modopen.h"
#include "modelset.h"

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
#include <windows.h>
#define pause() WaitForSingleObject(GetCurrentProcess(), INFINITE)
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

typedef struct {
	int stream;
	int timeslot;
	int type;
} MVIP;
#define MVIP_STREAM_NONE 0xff

typedef enum {TONE_TYPE_START, TONE_TYPE_END, TONE_TYPE_LEN} TONE_TYPE;

static struct chan {
	struct chan *next;
	pthread_t thread;
	int stop;
	MVIP xts;
	enum lang {LANG_NONE, LANG_EN, LANG_KR } lang;
	unsigned toneset;
	unsigned grunt;
	double grunt_min_noise_level;
	double grunt_threshold;
	unsigned map;
	unsigned cptone;
	unsigned tonemode;
	TONE_TYPE tonetype;
	int catsig_alg;
	struct sm_onhook_listen_for_parms onhook;
	MVIP refts;
	MODEL_SET *models;
	tSMModuleId module;
} *chans;

static int fn_sm_switch_channel_input(tSMChannelId chan, struct chan *cp)
{
 SM_SWITCH_CHANNEL_PARMS sw;
 int err;
 memset(&sw, 0, sizeof(sw));
 sw.channel = chan;
 sw.st = cp->xts.stream;
 sw.ts = cp->xts.timeslot;
 sw.type = cp->xts.type;
 err = sm_switch_channel_input(&sw);
 if (err) fprintf(stderr, "sm_switch_channel_input returned %d\n", err);
 return err;
}

static int detectloop(tSMChannelId chan, struct chan *cp, tSMEventId ev)
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

static int setup_asr(tSMChannelId chan, struct chan *cp)
{
 struct sm_asr_listen_for_parms alp;
 int err;
 memset(&alp, 0, sizeof(alp));
 alp.channel = chan;
 alp.asr_mode = kSMASRModeContinuous;
 if (model_set_listen(&alp, cp->models)) return 1;
 if (cp->refts.stream || cp->refts.timeslot) {
	SM_CHANNEL_ALLOC_PLACED_PARMS ap;
	SM_SWITCH_CHANNEL_PARMS scop;
	memset(&ap, 0, sizeof(ap));
	ap.type = kSMChannelTypeOutput;
	ap.module = cp->module;
	err = sm_channel_alloc_placed(&ap);
	if (err) {
		fprintf(stderr, "sm_channel_alloc_placed returned %d\n", err);
		fprintf(stderr, "Cannot allocate channel for sidetone reference\n");
		return 1;
	}
	memset(&scop, 0, sizeof(scop));
	scop.channel = ap.channel;
	scop.st = cp->refts.stream;
	scop.ts = cp->refts.timeslot;
	scop.type = cp->refts.type;
	err = sm_switch_channel_output(&scop);
	if (err) {
		fprintf(stderr, "sm_switch_channel_output returned %d\n", err);
		return 1;
	}
	alp.sidetone = ap.channel;
 }
 err = sm_asr_listen_for(&alp);
 if (err) {
	fprintf(stderr, "sm_asr_listen_for returned %d\n", err);
	return 1;
 }
 if (alp.sidetone) {
 	err = sm_channel_release(alp.sidetone);
	if (err) {
		fprintf(stderr, "sm_channel_release returned %d\n", err);
		fprintf(stderr, "Error releaseing sidetone reference channel\n");
		return 1;
	}
 }
 return 0;
}

static int setup_onhook(tSMChannelId chan, struct chan *cp)
{
 int err;
 cp->onhook.channel = chan;
 err = sm_onhook_listen_for(&cp->onhook);
 if (err) {
	fprintf(stderr, "sm_onhook_listen_for returned %d\n", err);
	return 1;
 }
 return 0;
}

static int detectchan(struct chan *cp)
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
		struct sm_listen_for_parms lp;
		memset(&lp, 0, sizeof(lp));
		lp.channel = ap.channel;
		if (cp->grunt != ~0U) {
			lp.enable_grunt_detection = 1;
			lp.grunt_latency = cp->grunt;
			lp.grunt_threshold = cp->grunt_threshold;
			lp.min_noise_level = cp->grunt_min_noise_level;
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
#ifdef TEST_REPEAT
	again:
#endif
		err = sm_listen_for(&lp);
		if (err) fprintf(stderr, "sm_listen_for returned %d\n", err);
		if (!err && cp->catsig_alg) {
			struct sm_catsig_listen_for_parms clp;
			memset(&clp, 0, sizeof(clp));
			clp.channel = ap.channel;
			clp.catsig_alg_id = cp->catsig_alg;
			err = sm_catsig_listen_for(&clp);
			if (err) fprintf(stderr, "sm_catsig_listen_for returned %d\n", err);
		}
		if (!err) err = setup_onhook(ap.channel, cp);
		if (!err && cp->models) {
			err = setup_asr(ap.channel, cp);
		}
		if (!err) {
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
				else err = detectloop(ap.channel, cp, ep.event);
#ifdef TEST_REPEAT
				if (err == 2) goto again;
#endif
				smd_ev_free(ep.event);
			}
		}
 	}
 	e2 = sm_channel_release(ap.channel);
	if (e2) fprintf(stderr, "sm_channel_release returned %d\n", e2);
	err = err || e2;
 }
 return err;
}

static void *chanfunc(void *vp)
{
 struct chan *cp = vp;
 if (detectchan(cp)) return (void *) 1;
 return 0;
}

static int setup_catsig_flo(tSMModuleId module, enum kSMBESPCatSigParam parameter_id, float f)
{
 SM_ADJUST_CATSIG_MODULE_PARMS cmp;
 int r;
 memset(&cmp, 0, sizeof(cmp));
 cmp.module = module;
 cmp.catsig_alg_id = 1;
 cmp.parameter_id = parameter_id;
 cmp.parameter_value.fp_value = f;
 r = sm_adjust_catsig_module_params (&cmp);
 if (r) fprintf(stderr, "sm_adjust_catsig_module_params () returned %d\n", r);
 return r;
}

static int setup_catsig_int(tSMModuleId module, enum kSMBESPCatSigParam parameter_id, int i)
{
 SM_ADJUST_CATSIG_MODULE_PARMS cmp;
 int r;
 memset(&cmp, 0, sizeof(cmp));
 cmp.module = module;
 cmp.catsig_alg_id = 1;
 cmp.parameter_id = parameter_id;
 cmp.parameter_value.int_value = i;
 r = sm_adjust_catsig_module_params (&cmp);
 if (r) fprintf(stderr, "sm_adjust_catsig_module_params () returned %d\n", r);
 return r;
}

static int setup_catsig_kr(tSMModuleId mod)
{
 return setup_catsig_flo(mod, kSMBESPCatSigParamF_ln_nrg_coeff, 0.95)
	|| setup_catsig_flo(mod, kSMBESPCatSigParamF_min_Lmin, 12129.6)
	|| setup_catsig_flo(mod, kSMBESPCatSigParamF_Lmin_decay, 1.001)
	|| setup_catsig_flo(mod, kSMBESPCatSigParamF_speech_thresh, 0.01)
	|| setup_catsig_flo(mod, kSMBESPCatSigParamF_debounce, 8)
	|| setup_catsig_int(mod, kSMBESPCatSigParamI_min_valid_period_count, 240)
	|| setup_catsig_int(mod, kSMBESPCatSigParamI_min_valid_count, 80)
	|| setup_catsig_int(mod, kSMBESPCatSigParamI_glitch_count, 48)
	|| setup_catsig_int(mod, kSMBESPCatSigParamI_qualify_count, 24)
	|| setup_catsig_int(mod, kSMBESPCatSigParamI_alter_duration, 80)
	|| setup_catsig_int(mod, kSMBESPCatSigParamI_max_valid_tone_cnt, 64)
	|| setup_catsig_int(mod, kSMBESPCatSigParamI_min_valid_speech_cnt, 48)
	|| setup_catsig_int(mod, kSMBESPCatSigParamI_threshold_samp_cnt, 360)
	|| setup_catsig_int(mod, kSMBESPCatSigParamI_delay_time, 112)
	|| setup_catsig_int(mod, kSMBESPCatSigParamI_period_time, 2640)
	|| setup_catsig_int(mod, kSMBESPCatSigParamI_min_off_count, 48)
	|| setup_catsig_int(mod, kSMBESPCatSigParamI_min_period_off, 680);
}

static int setup_catsig_en(tSMModuleId mod)
{
 return setup_catsig_flo(mod, kSMBESPCatSigParamF_ln_nrg_coeff, 0.8)
	|| setup_catsig_flo(mod, kSMBESPCatSigParamF_min_Lmin, 10108)
	|| setup_catsig_flo(mod, kSMBESPCatSigParamF_Lmin_decay, 1.0008)
	|| setup_catsig_flo(mod, kSMBESPCatSigParamF_speech_thresh, 0.01)
	|| setup_catsig_flo(mod, kSMBESPCatSigParamF_debounce, 8)
	|| setup_catsig_int(mod, kSMBESPCatSigParamI_min_valid_period_count, 152)
	|| setup_catsig_int(mod, kSMBESPCatSigParamI_min_valid_count, 112)
	|| setup_catsig_int(mod, kSMBESPCatSigParamI_glitch_count, 48)
	|| setup_catsig_int(mod, kSMBESPCatSigParamI_qualify_count, 24)
	|| setup_catsig_int(mod, kSMBESPCatSigParamI_alter_duration, 160)
	|| setup_catsig_int(mod, kSMBESPCatSigParamI_max_valid_tone_cnt, 48)
	|| setup_catsig_int(mod, kSMBESPCatSigParamI_min_valid_speech_cnt, 64)
	|| setup_catsig_int(mod, kSMBESPCatSigParamI_threshold_samp_cnt, 360)
	|| setup_catsig_int(mod, kSMBESPCatSigParamI_delay_time, 112)
	|| setup_catsig_int(mod, kSMBESPCatSigParamI_period_time, 1480)
	|| setup_catsig_int(mod, kSMBESPCatSigParamI_min_off_count, 16)
	|| setup_catsig_int(mod, kSMBESPCatSigParamI_min_period_off, 600);
}

static int detraw(tSMModuleId module, MVIP ts, unsigned toneset, unsigned grunt, double grunt_min_noise_level, double grunt_threshold, unsigned map, unsigned cptone, unsigned tonemode, TONE_TYPE tonetype, unsigned catsig_alg, enum lang lang, struct sm_onhook_listen_for_parms* ponhook, MVIP refts, MODEL_SET *msp)
{
 int err;
 struct chan *cp = malloc(sizeof(*cp));
 if (!cp) {
	perror("malloc");
	fprintf(stderr, "Cannot allocate channel struct\n");
	return 1;
 }
 switch (lang) {
 case LANG_NONE: break;
 case LANG_EN:
	if (setup_catsig_en(module)) {
		fprintf(stderr, "Cannot setup module catsig parameters\n");
		return 1;
	}
	break;
 case LANG_KR:
	if (setup_catsig_kr(module)) {
		fprintf(stderr, "Cannot setup module catsig parameters\n");
		return 1;
	}
	break;
 }
 cp->stop = 0;
 cp->module = module;
 cp->xts = ts;
 cp->toneset = toneset;
 cp->grunt = grunt;
 cp->grunt_min_noise_level = grunt_min_noise_level;
 cp->grunt_threshold = grunt_threshold;
 cp->map = map;
 cp->cptone = cptone;
 cp->tonemode = tonemode;
 cp->tonetype = tonetype;
 cp->catsig_alg = catsig_alg;
 cp->onhook = *ponhook;
 cp->refts = refts;
 cp->models = msp;
 err = pthread_create(&cp->thread, 0, chanfunc, cp);
 if (err) {
	fprintf(stderr, "pthread_create failed: %s\n", strerror(err));
	return 1;
 }
 cp->next = chans;
 chans = cp;
 if (++ts.timeslot == 32) {
	ts.timeslot = 0, ts.stream++;
 }
 for (;;) {
 	int c = getchar();
 	switch (c) {
 	case '?': printf("Commands:\n"
 		"w\twait forever\n"
 		"q\tquit\n");
 		break;
 	case '\n': break;
	case 'w':
		for (;;) pause();
		break;
 	case 'q':
 	case EOF:
		printf("exitting\n");
		for (cp = chans; cp; cp=cp->next) {
			cp->stop = 1;
		}
		while (chans) {
			struct chan *nx;
			void *sts;
			chans->stop = 1;
			err = pthread_join(chans->thread, &sts);
			if (err) {
				fprintf(stderr, "pthread_join failed: %s\n", strerror(err));
				return 1;
			}
			nx = chans->next;
			free(chans);
			chans = nx;
		 }
		 return 0;
 	}
 }
}

#include "gen/detraw.args.i"

int main(int argc, char **argv)
{
 int iErr;
 TONE_TYPE tonetype = TONE_TYPE_START;
 enum lang lang = LANG_NONE;
 tSMCardId card_id;
 tSMModuleId mod;
 struct sm_onhook_listen_for_parms onhook;
 MODEL_SET ms;
 MODEL_SET *msp = &ms;
 ARGS_DECL
 (void) argc;
 arg.grunt = ~0;
 if (ARGS_CALL) {
 usage:
	fprintf(stderr, "Usage: %s" ARGS_USAGE " iwr-models...\n", progname);
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
 if (arg.lang_english) lang = LANG_EN;
 if (arg.lang_korean) lang = LANG_KR;
 if (arg.grunt == ~0U && !arg.cptone && !arg.tonemode && !arg.catsig_alg && !arg.onhook
 	&& !*argv) {
	printf("Warning: no detection selected\n");
 }
 if (arg.serialnumber) {
	err_t e = modopen(&card_id, &mod, arg.serialnumber, arg.module);
	if (e) {
		error_log(stderr, e);
		return 1;
	}
 } else {
	fprintf(stderr, "%s: no Prosody card specified\n", progname);
	return 1;
 }
 if (model_set(&ms, mod)) return 1;
 if (!*argv) msp = 0;
 while (*argv) {
 	if (model_set_download(&ms, *argv)) return 1;
	argv++;
 }
 memset(&onhook, 0, sizeof(onhook));
 onhook.enable = arg.onhook;
 onhook.pre_pulse_max_power = arg.onhook_lowpower;
 onhook.pulse_min_power = arg.onhook_highpower;
 onhook.post_pulse_floor = arg.onhook_softpower;
 onhook.latitude = arg.onhook_convlat;
 onhook.count_ratio = arg.onhook_cr;
 onhook.total_ratio = arg.onhook_tr;
 onhook.max_duration = arg.onhook_dur;
 iErr = detraw(mod, arg.timeslot, arg.toneset, arg.grunt, arg.grunt_min_noise_level, arg.grunt_threshold, arg.map, arg.cptone, arg.tonemode, tonetype, arg.catsig_alg, lang, &onhook, arg.reftimeslot, msp);
 model_set_dtor(&ms);
 if (arg.serialnumber) {
 	err_t e = modclose(mod);
	if (e) error_log(stderr, e);
	e = cardclose(card_id);
	if (e) error_log(stderr, e);
 }
 return iErr;
}
