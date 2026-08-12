/* evrec.c - record a message from a local timeslot using events */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/timeb.h>

#include "../Testlib/cardlist.h"
#include "../Testlib/error.h"
#include "../Testlib/fmtcode.h"
#include "smbesp.h"
#include "smdc.h"
#include "smdrvr.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

typedef struct {
	int stream;
	int timeslot;
	int type;
} MVIP;
#define MVIP_STREAM_NONE 0xff

struct chan {
	pthread_t thread;
	tSMChannelId chan;
	int stop;
	enum kSMDataFormat format;
	tSM_UT32 rate;
	unsigned max_silence;
	char *file;
	MVIP xts;
};

static void showtime(char *where)
{
 static struct timeb prev;
 struct timeb now;
 double dtim;
 ftime(&now);
 dtim = now.time - prev.time
	+ (now.millitm - prev.millitm)
	/ 1000.0;
 if (dtim > 0.025) printf("%s took %g sec\n", where, dtim);
 prev = now;
}

	// handy utilities
static int fn_sm_switch_channel_input(tSMChannelId chan, struct chan *cp)
{
 struct sm_switch_channel_parms sw;
 int err;
 memset(&sw, 0, sizeof(sw));
 sw.channel = chan;
 sw.st = cp->xts.stream;
 sw.ts = cp->xts.timeslot;
 sw.type = cp->xts.type;
 err = sm_switch_channel_input(&sw);
 if (err) fprintf(stderr, "%d.%d: sm_switch_channel_input returned %d\n", cp->xts.stream, cp->xts.timeslot, err);
 return err;
}

static int do_rec(struct chan *cp, tSMChannelId chan, tSMEventId *ev, FILE *f)
{
 struct timeb start, prevtime;
#if 0
 SMDC_RX_CONTROL_PARMS rcp;
#endif
 unsigned long totnc = 0;
 unsigned sts = 0;
 int twid = 0;
 int err;
#if 0
 memset(&rcp, 0, sizeof(rcp));
 rcp.channel = chan;
 rcp.cmd = kSMDCRxCtlNotifyOnData;
 rcp.min_to_collect = 256;
 rcp.min_idle = 0;
 err = smdc_rx_control(&rcp);
 if (err) {
	fprintf(stderr, "smdc_rx_control returned %d\n", err);
	return 1;
 }
#endif
 ftime(&start);
 prevtime = start;
 for (;;) {
	struct sm_record_status_parms rp;
	struct sm_ts_data_parms dp;
	struct timeb now;
	char buff[2048];
	double dtim;
	unsigned nc;
	for (;;) {
		memset(&rp, 0, sizeof(rp));
		rp.channel = chan;
		showtime("before status");
		err = sm_record_status(&rp);
		showtime("status");
		if (err) {
			fprintf(stderr, "%d.%d: sm_record_status returned %d\n", cp->xts.stream, cp->xts.timeslot, err);
			return 1;
		}
		if (rp.status != kSMRecordStatusNoData) break;
		if (sts != kSMRecordStatusData) {
			printf(" -> %d\n", rp.status);
			sts = kSMRecordStatusData;
		}
		showtime("before wait");
		err = smd_ev_wait(*ev);
		showtime("wait");
		if (err) {
			fprintf(stderr, "%d.%d: smd_ev_wait returned %d\n", cp->xts.stream, cp->xts.timeslot, err);
			return 1;
		}
	}
	if (rp.status != sts) {
		printf(" -> %d\n", rp.status);
		sts = rp.status;
	}
	if (rp.status == kSMRecordStatusComplete) return 0;
	sts = rp.status;
	memset(&dp, 0, sizeof(dp));
	dp.channel = chan;
	dp.data = buff;
	showtime("before get data");
	err = sm_get_recorded_data(&dp);
	showtime("get data");
	if (err) {
		fprintf(stderr, "%d.%d: sm_put_record_data returned %d\n", cp->xts.stream, cp->xts.timeslot, err);
		return 1;
	}
	showtime("before fwrite");
	nc = fwrite(buff, 1, dp.length, f);
	showtime("fwrite");
	if (!nc) {
		if (ferror(f)) {
			perror("fwrite failed");
			return 1;
		}
	}
	totnc += nc;
	ftime(&now);
	dtim = now.time - start.time
		+ (now.millitm - start.millitm)
		/ 1000.0;
	if (!dtim) dtim = 0.001;
	printf("\r%06d\t%d  %d bps %c    ",
		nc, sts,
		(int) (8 * totnc / dtim),
		"|/-\\"[twid]);
	dtim = now.time - prevtime.time
		+ (now.millitm - prevtime.millitm)
		/ 1000.0;
	printf("%d bps     ", (int) (8 * nc / dtim));
	prevtime = now;
	twid = (twid+1) & 3;
	fflush(stdout);
 }
}

static void *chanfunc(void *vp)
{
 struct chan *cp = vp;
 int err = 0;
 FILE *f;
	// record cp->file from timeslot xp.stream:xp.ts
 f = fopen(cp->file, "wb");
 if (!f) {
	perror("Error creating file");
	fprintf(stderr, "%d.%d: File: %s\n", cp->xts.stream, cp->xts.timeslot, cp->file);
	return (void *) 1;
 }
 if (!fn_sm_switch_channel_input(cp->chan, cp)) {
	SM_CHANNEL_SET_EVENT_PARMS sep;
	struct sm_record_parms rp;
	memset(&rp, 0, sizeof(rp));
	rp.channel = cp->chan;
	rp.type = cp->format;
	rp.sampling_rate = cp->rate;
	rp.max_silence = cp->max_silence;
	err = sm_record_start(&rp);
	if (err) {
		fprintf(stderr, "%d.%d: sm_record_start returned %d\n", cp->xts.stream, cp->xts.timeslot, err);
	} else {
		memset(&sep, 0, sizeof(sep));
		err = smd_ev_create(&sep.event, cp->chan,
			kSMEventTypeReadData, kSMChannelSpecificEvent);
		if (err) {
			fprintf(stderr, "%d.%d: smd_ev_create returned %d\n", cp->xts.stream, cp->xts.timeslot, err);
		} else {
			sep.channel = cp->chan;
			sep.event_type = kSMEventTypeReadData;
			sep.issue_events = kSMChannelSpecificEvent;
			err = sm_channel_set_event(&sep);
			if (err) {
				fprintf(stderr, "%d.%d: sm_channel_set_event returned %d\n", cp->xts.stream, cp->xts.timeslot, err);
			}
		}
	}
	if (!err) err = do_rec(cp, cp->chan, &sep.event, f);
 }
 fclose(f);
 printf(cp->stop ? "abandoning\n" : "done\n");
 return (void *) err;
}

static int evrec(CARDLIST *cards, unsigned permod, unsigned nchan, MVIP ts, unsigned max_silence, enum kSMDataFormat format, unsigned rate, char *file)
{
 struct chan *cp = malloc(nchan * sizeof(*cp));
 unsigned curcard = 0;
 unsigned thismod = 0;
 unsigned curmod = 0;
 unsigned channo;
 int err;
 if (!cp) {
	perror("malloc failed");
	return 1;
 }
 for (channo=0; channo < nchan; channo++) {
	SM_CHANNEL_ALLOC_PLACED_PARMS ap;
	MODDESC mod;
	err_t e = cardlist_getmod(&mod, cards, curcard, curmod);
	if (e) {
		error_log(stderr, e);
		return 1;
	}
	memset(&ap, 0, sizeof(ap));
	ap.type = kSMChannelTypeInput;
	ap.module = mod.id;
	err = sm_channel_alloc_placed(&ap);
	if (err) {
		fprintf(stderr, "%d.%d: sm_channel_alloc_placed returned %d\n",
			ts.stream, ts.timeslot, err);
		return 1;
	}
	cp[channo].chan = ap.channel;
	cp[channo].stop = 0;
	cp[channo].xts = ts;
	if (! (++ts.timeslot & 0x1f) ) {
		ts.timeslot -= 32;
		ts.stream++;
	}
	cp[channo].format = format;
	cp[channo].rate = rate;
	cp[channo].max_silence = max_silence;
	cp[channo].file = file;
	err = pthread_create(&cp[channo].thread, 0, chanfunc, &cp[channo]);
	if (err) {
		fprintf(stderr, "pthread_create failed: %s\n", strerror(err));
		return 1;
 	}
 	if (++thismod == permod) {
 		thismod = 0;
 		cardlist_nextmod(cards, &curcard, &curmod);
	}
 }
 for (;;) {
 	int c = getchar();
 	switch (c) {
		void *sts;
 	case '\n': break;
 	case 'a':
 	case 'A':
		printf("abortting\n");
		for (channo=0; channo < nchan; channo++) {
			struct sm_record_abort_parms rp;
			memset(&rp, 0, sizeof(rp));
			rp.discard = c == 'A';
			rp.channel = cp[channo].chan;
			printf("abort: sts = %d\n", sm_record_abort(&rp));
		}
		break;
 	case 'q':
 	case EOF:
		printf("exitting\n");
		for (channo=0; channo < nchan; channo++) {
			struct sm_record_abort_parms rp;
			memset(&rp, 0, sizeof(rp));
			rp.channel = cp[channo].chan;
			cp[channo].stop = 1;
			printf("abort: sts = %d\n", sm_record_abort(&rp));
		}
		for (channo=0; channo < nchan; channo++) {
			struct chan *chan = &cp[channo];
			err = pthread_join(chan->thread, &sts);
			if (err) {
				fprintf(stderr, "pthread_join (%d) failed: %s\n", channo, strerror(err));
				return 1;
			}
			err = sm_channel_release(chan->chan);
			if (err) fprintf(stderr, "%d.%d: sm_channel_release returned %d\n", chan->xts.stream, chan->xts.timeslot, err);
		}
		return 0;
 	}
 }
}

#include "gen/evrec.args.i"

int main(int argc, char **argv)
{
 enum kSMDataFormat format;
 CARDLIST cards;
 tSM_UT32 rate;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || !*argv || argv[1]) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE " file\nFormats:", progname);
	fmtlist_dump(stderr);
	return 1;
 }
 cardlist(&cards);
 if (arg.serialnumber) {
	err_t err = cardlist_addcard(&cards, arg.serialnumber);
	if (err) {
		error_log(stderr, err);
		return 1;
	}
 }
 if (!cards.ncard) {
	fprintf(stderr, "No cards specifed\n");
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
 return evrec(&cards, arg.per_mod, arg.numchan, arg.timeslot, arg.max_silence, format, rate, *argv);
}
