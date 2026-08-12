/* locplay.c - play a message to a local timeslot */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/timeb.h>

#include "../Testlib/cardopen.h"
#include "../Testlib/fmtcode.h"
#include "../Testlib/modopen.h"
#include "smbesp.h"
#include "smdrvr.h"
#include "smhlib.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

typedef struct {
	int stream;
	int timeslot;
	int type;
} MVIP;
#define MVIP_STREAM_NONE 0xff

struct chan {
	pthread_t thread;
	pthread_mutex_t adjmx;
	pthread_cond_t cond;
	enum status {
		STS_STARTING,
		STS_RUNNING,
		STS_STOPPING,
		STS_STOPPED
	} status;
	struct adjustable {
		unsigned agc;
		int volume;
		int speed;
	} cur, wanted;
	enum kSMDataFormat format;
	tSM_UT32 rate;
	struct sm_file_replay_parms rp;
	char *file;
	MVIP xts;
	tSMModuleId module;
};

	// handy utilities
static int reperrno(const char *op, int errnum)
{
 if (errnum) fprintf(stderr, "%s failed: %s\n", op, strerror(errnum));
 return errnum;
}

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

static int lockadj(struct chan *cp)
{
 return reperrno("mutex lock", pthread_mutex_lock(&cp->adjmx));
}

static int unlockadj(struct chan *cp)
{
 return reperrno("mutex unlock", pthread_mutex_unlock(&cp->adjmx));
}

static void *chanfunc(void *vp)
{
 struct chan *cp = vp;
 int err = 0;
 SM_CHANNEL_ALLOC_PLACED_PARMS ap;
 FILE *f;
	// play cp->file to timeslot xp.stream:xp.ts
 f = fopen(cp->file, "rb");
 if (!f) {
	perror("Error opening file");
	fprintf(stderr, "%d.%d: File: %s\n", cp->xts.stream, cp->xts.timeslot, cp->file);
	return (void *) 1;
 }
 memset(&ap, 0, sizeof(ap));
 ap.type = kSMChannelTypeOutput;
 ap.module = cp->module;
 err = sm_channel_alloc_placed(&ap);
 if (err) {
	fprintf(stderr, "%d.%d: sm_channel_alloc_placed returned %d\n", cp->xts.stream, cp->xts.timeslot, err);
 } else {
	int e2;
	if (!fn_sm_switch_channel_output(ap.channel, cp)) {
		memset(&cp->rp, 0, sizeof(cp->rp));
		cp->rp.replay_parms.channel = ap.channel;
		cp->rp.replay_parms.type = cp->format;
		cp->rp.replay_parms.sampling_rate = cp->rate;
		cp->rp.fd = f;
		if (lockadj(cp)) err = 1;
		else {
			cp->cur = cp->wanted;
			cp->rp.replay_parms.agc = cp->cur.agc;
			cp->rp.replay_parms.volume = cp->cur.volume;
			cp->rp.replay_parms.speed = cp->cur.speed;
			err = sm_replay_file_start(&cp->rp);
			if (err) {
				fprintf(stderr, "%d.%d: sm_file_replay_start returned %d\n", cp->xts.stream, cp->xts.timeslot, err);
			}
			cp->status = STS_RUNNING;
			pthread_cond_broadcast(&cp->cond);
			if (unlockadj(cp)) err = 1;
		}
		if (!err) {
			struct timeb start;
			ftime(&start);
			err = sm_replay_file_progress(&cp->rp);
			if (err == ERR_SM_PENDING) {
				err = sm_replay_file_complete(&cp->rp);
			}
			cp->status = STS_STOPPING;
			if (err) {
				fprintf(stderr, "%d.%d: sm_file_replay_complete returned %d\n", cp->xts.stream, cp->xts.timeslot, err);
			} else {
				struct timeb now;
				double dtim;
				ftime(&now);
				dtim = now.time - start.time
					+ (now.millitm - start.millitm)
					/ 1000.0;
				if (!dtim) dtim = 0.001;
				printf("%g S\n", dtim);
			}
		}
	}
	e2 = sm_channel_release(ap.channel);
	if (e2) fprintf(stderr, "%d.%d: sm_channel_release returned %d\n", cp->xts.stream, cp->xts.timeslot, e2);
	err = err || e2;
 }
 fclose(f);
 printf("done\n");
 return (void *) err;
}

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

static int playone(tSMModuleId module, int volume, int speed, unsigned agc, MVIP ts, enum kSMDataFormat format, unsigned rate, char *file)
{
 struct chan chan;
 int err;
 if (reperrno("mutex init", pthread_mutex_init(&chan.adjmx, 0))) return 1;
 if (reperrno("condvar init", pthread_cond_init(&chan.cond, 0))) return 1;
 chan.wanted.volume = volume;
 chan.wanted.speed = speed;
 chan.wanted.agc = agc;
 chan.xts = ts;
 chan.module = module;
 chan.format = format;
 chan.rate = rate;
 chan.file = file;
 chan.status = STS_STARTING;
 err = pthread_create(&chan.thread, 0, chanfunc, &chan);
 if (err) {
	fprintf(stderr, "pthread_create failed: %s\n", strerror(err));
	return 1;
 }
 for (;;) {
 	int c = getchar();
 	switch (c) {
 	case '\n': break;
	case 'a':
		if (lockadj(&chan)) return 1;
		chan.wanted.agc = readint();
		if (unlockadj(&chan)) return 1;
		break;
	case 's':
		if (lockadj(&chan)) return 1;
		chan.wanted.speed = readint();
		if (unlockadj(&chan)) return 1;
		break;
	case 'v':
		if (lockadj(&chan)) return 1;
		chan.wanted.volume = readint();
		if (unlockadj(&chan)) return 1;
		break;
 	case 'q':
		if (lockadj(&chan)) return 1;
		while (chan.status == STS_STARTING)
			pthread_cond_wait(&chan.cond, &chan.adjmx);
		if (chan.status == STS_RUNNING) {
			sm_replay_file_stop(&chan.rp, 1);
			chan.status = STS_STOPPING;
		}
		if (unlockadj(&chan)) return 1;
		// fall in ...
 	case EOF: {
		void *sts;
		printf("exitting\n");
		err = pthread_join(chan.thread, &sts);
		if (err) {
			fprintf(stderr, "pthread_join failed: %s\n", strerror(err));
			return 1;
		}
	}
		return 0;
 	}
 }
}

#include "gen/hplayone.args.i"

int main(int argc, char **argv)
{
 enum kSMDataFormat format;
 tSMModuleId mod;
 tSMCardId card;
 tSM_UT32 rate;
 int iErr;
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
 iErr = playone(mod, arg.volume, arg.speed, arg.agc, arg.timeslot, format, rate, *argv);
 if (arg.serialnumber) {
	modclose(mod);
	cardclose(card);
 }
 return iErr;
}
