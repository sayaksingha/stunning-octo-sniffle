/* echorec.c - record a message from a local timeslot with echo cancellation */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/timeb.h>

#include "../Testlib/cardopen.h"
#include "../Testlib/errcode_sm.h"
#include "../Testlib/fmtcode.h"
#include "../Testlib/modopen.h"
#include "smbesp.h"
#include "smdrvr.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

typedef struct {
	int stream;
	int timeslot;
	int type;
} MVIP;
#define MVIP_STREAM_NONE 0xff

static struct chan {
	struct chan *next;
	pthread_t thread;
	int stop;
	int volume;
	tSMModuleId module;
	unsigned agc;
	unsigned sil_elim;
	unsigned max_silence;
	enum kSMDataFormat format;
	unsigned rate;
	char *file;
	MVIP xts;
	MVIP refts;
} *chans;

static int chan_alloc_in(tSMChannelId *cp, MVIP *ts, tSMModuleId module)
{
 SM_SWITCH_CHANNEL_PARMS swp;
 SM_CHANNEL_ALLOC_PLACED_PARMS sa;
 int r;
 memset(&sa, 0, sizeof(sa));
 sa.type = kSMChannelTypeInput;
 sa.module = module;
 r = sm_channel_alloc_placed(&sa);
 if (r) return printerr_sm("sm_channel_alloc_placed", r);
 *cp = sa.channel;
 memset(&swp, 0, sizeof(swp));
 swp.channel = *cp;
 swp.st = ts->stream;
 swp.ts = ts->timeslot;
 swp.type = ts->type;
 if (++ts->timeslot >= 32) {
	ts->timeslot = 0;
	ts->stream++;
 }
 r = sm_switch_channel_input(&swp);
 if (r) return printerr_sm("sm_switch_channel_input", r);
 return r;
}

static void *chanfunc(void *vp)
{
 struct chan *cp = vp;
 int err = 0;
 for (; !err && !cp->stop;) {
	tSMChannelId mainchan, refchan;
	FILE *f;
	 // record timeslot xp.stream:xp.ts to cp->file
	if (strcmp(cp->file, "-")) {
		char *fname = malloc(strlen(cp->file)+sizeof("-1234567890-1234567890"));
		if (!fname) {
			perror("malloc failed");
			return (void *) 1;
		}
		sprintf(fname, cp->file, cp->xts.stream, cp->xts.timeslot);
		f = fopen(fname, "wb");
		if (!f) {
			perror("Error creating file");
			fprintf(stderr, "%d.%d: File: %s\n", cp->xts.stream, cp->xts.timeslot, fname);
			free(fname);
			return (void *) 1;
		}
		free(fname);
	} else {
		f = stdout;
	}
	if ((err = chan_alloc_in(&mainchan, &cp->xts, cp->module))) {
		fprintf(stderr, "%d.%d: cannot allocate main channel %d\n", cp->xts.stream, cp->xts.timeslot, err);
	} else if ((err = chan_alloc_in(&refchan, &cp->refts, cp->module))) {
		fprintf(stderr, "%d.%d: cannot allocate ref channel %d\n", cp->xts.stream, cp->xts.timeslot, err);
	} else {
		SM_CONDITION_INPUT_PARMS cip;
		int e2;
		memset(&cip, 0, sizeof(cip));
		cip.channel = mainchan;
		cip.reference = refchan;
		cip.reference_type = kSMInputCondRefUseInput;
		cip.conditioning_type = kSMInputCondEchoCancelation;
		if (1 && (err = sm_condition_input(&cip))) {
			fprintf(stderr, "%d.%d: sm_condition_input returned %d\n", cp->xts.stream, cp->xts.timeslot, err);
		} else {
			struct sm_record_parms rp;
			memset(&rp, 0, sizeof(rp));
			rp.channel = mainchan;
			rp.type = cp->format;
			rp.sampling_rate = cp->rate;
			rp.silence_elimination = cp->sil_elim;
			rp.agc = cp->agc;
			rp.volume = cp->volume;
			rp.max_silence = cp->max_silence;
			err = sm_record_start(&rp);
			if (err) fprintf(stderr, "%d.%d: sm_record_start returned %d\n", cp->xts.stream, cp->xts.timeslot, err);
			else {
				unsigned long totnc = 0;
				struct timeb start;
				int twid = 0;
				ftime(&start);
				for (;;) {
					struct sm_ts_data_parms dp;
					struct timeb now;
					char buff[16384];
					double dtim;
					unsigned nc;
					memset(&dp, 0, sizeof(dp));
					dp.channel = mainchan;
					dp.data = buff;
					dp.length = arlen(buff);
					err = sm_get_recorded_data(&dp);
					ftime(&now);
					if (err) {
						fprintf(stderr, "%d.%d: sm_get_recorded_data returned %d\n", cp->xts.stream, cp->xts.timeslot, err);
						break;
					}
					if (!dp.length) {
						SM_RECORD_STATUS_PARMS sp;
						memset(&sp, 0, sizeof(sp));
						sp.channel = mainchan;
						err = sm_record_status(&sp);
						if (err) {
							fprintf(stderr, "%d.%d: sm_record_status returned %d\n", cp->xts.stream, cp->xts.timeslot, err);
							break;
						}
					}
					nc = fwrite(buff, 1, dp.length, f);
					if (nc != (unsigned) dp.length) {
						if (ferror(f)) {
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
					if (cp->stop) {
						fprintf(stderr, "abandoning\n");
						break;
					}
				}
			}
		}
		e2 = sm_channel_release(mainchan);
		if (e2) fprintf(stderr, "%d.%d: sm_channel_release returned %d\n", cp->xts.stream, cp->xts.timeslot, e2);
		err = err || e2;
	 }
	 if (f != stdout) fclose(f);
 }
 return (void *) err;
}

static int echorec(unsigned numchan, tSMModuleId module, unsigned agc, int volume, MVIP ts, MVIP refts, unsigned max_silence, unsigned sil_elim, enum kSMDataFormat format, unsigned rate, char *file)
{
 unsigned i;
 int err;
 for (i=0; i < numchan; i++) {
	struct chan *cp = malloc(sizeof(*cp));
	if (!cp) {
		perror("malloc");
		fprintf(stderr, "Cannot allocate channel struct\n");
		return 1;
	}
	cp->module = module;
	cp->stop = 0;
	cp->volume = volume;
	cp->agc = agc;
	cp->xts = ts;
	cp->refts = refts;
	cp->file = file;
	cp->sil_elim = sil_elim;
	cp->max_silence = max_silence;
	cp->format = format;
	cp->rate = rate;
	err = pthread_create(&cp->thread, 0, chanfunc, cp);
	if (err) {
		fprintf(stderr, "pthread_create failed: %s\n", strerror(err));
		return 1;
	}
	cp->next = chans;
	chans = cp;
	if (++ts.timeslot == 32)
		ts.timeslot = 0, ts.stream++;
 }
 for (;;) {
 	int c = getchar();
 	switch (c) {
		struct chan *cp;
 	case '\n': break;
 	case 'q':
 	case EOF:
		fprintf(stderr, "exitting\n");
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

#include "gen/echorec.args.i"

int main(int argc, char **argv)
{
 enum kSMDataFormat format;
 tSMModuleId mod;
 tSMCardId card;
 int iErr;
 tSM_UT32 rate;
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
 if (fmtcode(&format, &rate, arg.format)) {
	fprintf(stderr, "Unknown format: '%s'\n", arg.format);
	return 1;
 }
 if (!arg.numchan) arg.numchan = 1;
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
 iErr = echorec(arg.numchan, mod, arg.agc, arg.volume, arg.timeslot,
	arg.reftimeslot, arg.max_silence, arg.sil_elim, format, rate, *argv);
 if (arg.serialnumber) {
	modclose(mod);
	cardclose(card);
 }
 return iErr;
}
