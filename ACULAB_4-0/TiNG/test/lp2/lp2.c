/* locplay.c - play a message to a local timeslot */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/timeb.h>

#include "../Testlib/cardopen.h"
#include "../Testlib/error.h"
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

struct chan {
	struct chan *next;
	pthread_t thread;
	SM_CHANNEL_ALLOC_PLACED_PARMS ap;
	int stop;
	int volume;
	int speed;
	tSMModuleId module;
	unsigned agc;
	enum kSMDataFormat format;
	tSM_UT32 rate;
	char *file;
	MVIP xts;
};


static int fn_sm_switch_channel_output(tSMChannelId chan, struct chan *cp)
{
 struct sm_switch_channel_parms sw;
 int err;
 memset(&sw, 0, sizeof(sw));
 sw.channel = chan;
 sw.st = cp->xts.stream;
 sw.ts = cp->xts.timeslot;
 sw.type = cp->xts.type;
 err = sm_switch_channel_output(&sw);
 if (err) fprintf(stderr, "%d.%d: sm_switch_channel_output returned %d\n", cp->xts.stream, cp->xts.timeslot, err);
 return err;
}

static void *chanfunc(void *vp)
{
 struct chan *cp = vp;
 int err = 0;
 for (; !err && !cp->stop;) 
    {
    FILE *f;
    // play cp->file to timeslot xp.stream:xp.ts
    f = fopen(cp->file, "rb");
    if (!f) {
	perror("Error opening file");
	fprintf(stderr, "%d.%d: File: %s\n", cp->xts.stream, cp->xts.timeslot, cp->file);
	return (void *) 1;
	}
    if (!fn_sm_switch_channel_output(cp->ap.channel, cp)) 
	{
	struct sm_replay_parms rp;
	memset(&rp, 0, sizeof(rp));
	rp.channel = cp->ap.channel;
	rp.type = cp->format;
	rp.sampling_rate = cp->rate;
	rp.agc = cp->agc;
	rp.volume = cp->volume;
	rp.speed = cp->speed;
	err = sm_replay_start(&rp);
	if (err) {
		fprintf(stderr, "%d.%d: sm_replay_start returned %d\n", cp->xts.stream, cp->xts.timeslot, err);
		break;
	} else {
		unsigned long totnc = 0;
		struct timeb start;
		int twid = 0;
		ftime(&start);
		for (;;) {
			struct sm_ts_data_parms dp;
			struct timeb now;
			char buff[16384];
			double dtim;
			unsigned nc = fread(buff, 1, sizeof(buff), f);
			if (!nc) {
				if (ferror(f)) {
					perror("fread failed");
					err = 1;
				}
				break;
			}
			memset(&dp, 0, sizeof(dp));
			dp.channel = cp->ap.channel;
			dp.data = buff;
			dp.length = nc;
			err = sm_put_replay_data(&dp);
			if (err) {
				fprintf(stderr, "%d.%d: sm_put_replay_data returned %d\n", cp->xts.stream, cp->xts.timeslot, err);
				break;
			}
			totnc += nc;
			ftime(&now);
			dtim = now.time - start.time
				+ (now.millitm - start.millitm)
				/ 1000.0;
			if (!dtim) dtim = 0.001;
			printf("\r%d bps %c    ",
				(int) (8 * totnc / dtim),
				"|/-\\"[twid]);
			twid = (twid+1) & 3;
			fflush(stdout);
			if (cp->stop) {
				printf("abandoning\n");
				break;
			}
		}
	    }
	}
	fclose(f);
	{
	struct sm_replay_abort_parms ab;

	memset( (void *)&ab, 0, sizeof(ab) );
	ab.channel = cp->ap.channel;
	sm_replay_abort(&ab);
	}
    }
    return (void *) err;
}

static int locplay(unsigned numchan, tSMModuleId module, int volume, int speed, unsigned agc, MVIP ts, enum kSMDataFormat format, unsigned rate, char *file)
{
 struct chan *chans = 0;
 unsigned i;
 int err;
 for (i=0; i < numchan; i++) {
	struct chan *cp = malloc(sizeof(*cp));
	if (!cp) {
		perror("malloc");
		fprintf(stderr, "Cannot allocate channel struct\n");
		return 1;
	}
	memset(&cp->ap, 0, sizeof(cp->ap));
	cp->ap.type = kSMChannelTypeOutput;
	cp->ap.module = cp->module;
	err = sm_channel_alloc_placed(&cp->ap);
	if (err) {
		fprintf(stderr, "sm_channel_alloc_placed returned %d\n", err);
		break;
	}
	cp->module = module;
	cp->stop = 0;
	cp->volume = volume;
	cp->speed = speed;
	cp->agc = agc;
	cp->xts = ts;
	cp->file = file;
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
			err = sm_channel_release(chans->ap.channel);
			if (err) fprintf(stderr, "%d.%d: sm_channel_release returned %d\n", chans->xts.stream, chans->xts.timeslot, err);
			nx = chans->next;
			free(chans);
			chans = nx;
		}
		return 0;
 	}
 }
}

#include "gen/lp2.args.i"


int main(int argc, char **argv)
{
 enum kSMDataFormat format;
 tSMModuleId mod;
 tSMCardId card;
 tSM_UT32 rate;
 int sts;
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
 sts = locplay(arg.numchan, mod, arg.volume, arg.speed, arg.agc, arg.timeslot, format, rate, *argv);
 if (arg.serialnumber) {
	modclose(mod);
	cardclose(card);
 }
 return sts;
}
