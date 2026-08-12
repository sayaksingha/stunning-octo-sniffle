/* cndtest.c - test ADSI functions */

#ifdef TiNGTYPE_LINUX
#define USE_POLL
#endif

#ifdef TiNGTYPE_QNX
#define USE_POLL
#endif


#ifdef TiNGTYPE_WINNT
#define USE_WFMI
#endif

#include "../Testlib/cardopen.h"
#include "../Testlib/modopen.h"

#ifdef USE_POLL
#include <sys/poll.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef USE_WFMI
#include <windows.h>
#include <../../libutil/WINNT/lasterr.h>
#endif

#include "../../adsilib/adsilib.h"
#include "smbesp.h"
#include "smdrvr.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

	// we need this because the automatic argument parser
	// refers to a type 'MVIP' when it produces code to
	// handle timeslots
typedef struct {
	int stream;
	int timeslot;
	int type;
} MVIP;
#define MVIP_STREAM_NONE 0xff

static int parse_time(unsigned *month, unsigned *day, unsigned *hour, unsigned *minute, char *timestr)
{
 char *ep;
 if (!timestr) {
	fprintf(stderr, "No time specified\n");
	return 1;
 }
 *month = strtoul(timestr, &ep, 0);
 if (*ep != '-' && *ep != '/') {
	fprintf(stderr, "Expected '-' after month, got '%s'\n", ep);
	return 1;
 }
 *day = strtoul(ep + 1, &ep, 0);
 if (*ep != '-' && *ep != '/') {
	fprintf(stderr, "Expected '-' after day, got '%s'\n", ep);
	return 1;
 }
 *hour = strtoul(ep + 1, &ep, 0);
 if (*ep != ':') {
	fprintf(stderr, "Expected ':' after hour, got '%s'\n", ep);
	return 1;
 }
 *minute = strtoul(ep + 1, &ep, 0);
 if (*ep) {
	fprintf(stderr, "Junk after minute: '%s'\n", ep);
	return 1;
 }
 return 0;
}

static int alloc_chan(tSMChannelId *chanp, tSMModuleId module)
{
 SM_CHANNEL_ALLOC_PLACED_PARMS app;
 int e;
 memset(&app, 0, sizeof(app));
 app.module = module;
 app.type = kSMChannelTypeFullDuplex;
 e = sm_channel_alloc_placed(&app);
 if (e) fprintf(stderr, "sm_channel_alloc_placed() returned %d\n", e);
 *chanp = app.channel;
 return e;
}

static int assign_timeslot(tSMChannelId cp, MVIP xts)
{
 SM_SWITCH_CHANNEL_PARMS sw;
 int e;
 memset(&sw, 0, sizeof(sw));
 sw.channel = cp;
 sw.st = xts.stream;
 sw.ts = xts.timeslot;
 sw.type = xts.type;
 e = sm_switch_channel_output(&sw);
 if (e) fprintf(stderr, "sm_switch_channel_output() returned %d\n", e);
 return e;
}

static int setup_event(tSMEventId *evp, int eventType, tSMChannelId chan)
{
 SM_CHANNEL_SET_EVENT_PARMS sep;
 int e = smd_ev_create(evp, chan, eventType, kSMChannelSpecificEvent);
 if (e) {
 	fprintf(stderr, "smd_ev_create() returned %d\n", e);
	return 1;
 }
 memset(&sep, 0, sizeof(sep));
 sep.channel = chan;
 sep.event_type = eventType;
 sep.issue_events = kSMChannelSpecificEvent;
 sep.event = *evp;
 e = sm_channel_set_event(&sep);
 if (e) {
 	fprintf(stderr, "sm_channel_set_event() returned %d\n", e);
	smd_ev_free(*evp);
	return 1;
 }
 return 0;
}

static int setup_events(tSMEventId *outevp, tSMEventId *cogevp, tSMChannelId chan)
{
 if (setup_event(outevp, kSMEventTypeWriteData, chan)) {
 	fprintf(stderr, "cannot create write event\n");
	return 1;
 }
 if (setup_event(cogevp, kSMEventTypeRecog, chan)) {
 	fprintf(stderr, "cannot create recog event\n");
	return 1;
 }
 return 0;
}

static int dtmf_listen(tSMChannelId chan)
{
 SM_LISTEN_FOR_PARMS lfp;
 int e;
 memset(&lfp, 0, sizeof(lfp));
 lfp.channel = chan;
 lfp.tone_detection_mode = kSMToneDetectionNoMinDuration;
 lfp.map_tones_to_digits = kSMDTMFToneSetDigitMapping;
 e = sm_listen_for(&lfp);
 if (e) {
	fprintf(stderr, "sm_listen_for() returned %d\n", e);
	return 1;
 }
 return 0;
}

static int play_sas_cas(tSMChannelId chan, int sastype, ADSI_MODULE *mp)
{
 SM_PLAY_SAS_CAS_PARMS pscp;
 int e;
 pscp.channel = chan;
 switch (sastype) {
 case 1: pscp.sas_tone_type = kSASToneTypeSingleBurst; break;
 case 2: pscp.sas_tone_type = kSASToneTypeDoubleBurst; break;
 case 3: pscp.sas_tone_type = kSASToneTypeTripleBurst; break;
 default:
	fprintf(stderr, "invalid SAS type: %d, expected 1, 2, or 3\n",
		sastype);
	return 1;
 }
 e = sm_play_sas_cas(&pscp, mp);
 if (e) {
 	fprintf(stderr, "sm_play_sas_cas() returned %d\n", e);
	return 1;
 }
 return 0;
}

static int play_cnd(tSMChannelId chan, int onhook, int smdf, int anon, int outofarea, unsigned month, unsigned day, unsigned hour, unsigned minute, char *digits, char *name)
{
 SM_PLAY_CND_FSK_PARMS pvfp;
 int e;
 pvfp.channel = chan;
 pvfp.do_on_hook = onhook;
 pvfp.is_sdmf_message = smdf;
 pvfp.month = month;
 pvfp.day = day;
 pvfp.hour = hour;
 pvfp.minute = minute;
 if (anon) pvfp.cnd_type = kCNDTypeAnonymous;
 else if (outofarea) pvfp.cnd_type = kCNDTypeOutOfArea;
 else {
	pvfp.cnd_type = kCNDTypeCallingDN;
	if (digits) strcpy(pvfp.digit_string, digits);
	if (name) strcpy(pvfp.name_string, name) ;
 }
 e = sm_play_cnd_fsk(&pvfp);
 if (e) {
 	fprintf(stderr, "sm_play_cnd_fsk() returned %d\n", e);
	return 1;
 }
 return 0;
}

static int get_digit(char *cp, tSMChannelId chan)
{
 SM_RECOGNISED_PARMS rp;
 int e;
 memset(&rp, 0, sizeof(rp));
 rp.channel = chan;
 e = sm_get_recognised(&rp);
 if (e) {
 	fprintf(stderr, "sm_get_recognised() returned %d\n", e);
	return 1;
 }
 *cp = rp.param0;
 return 0;
}

static int abort_cptone(tSMChannelId chan)
{
 int e = sm_play_cptone_abort(chan);
 if (e) {
 	fprintf(stderr, "sm_play_cptone_abort() returned %d\n", e);
	return 1;
 }
 return 0;
}

static int multiwait(tSMEventId *evlist, unsigned numev)
{
#ifdef USE_POLL
 struct pollfd *pfd = malloc(numev * sizeof(*pfd));
 unsigned u;
 int i;
 if (!pfd) {
 	perror("malloc() failed");
	return 1;
 }
 for (u=0; u < numev; u++) {
 	pfd[u].fd = evlist[u].fd;
 	pfd[u].events = evlist[u].mode;
 }
 i = poll(pfd, numev, -1);
 if (i < 0) perror("poll() failed");
 else for (i=0; (unsigned)i < numev; i++) {
	if (pfd[i].revents) break;
 }
 free(pfd);
 return i;
#endif
#ifdef USE_WFMI
 DWORD n = WaitForMultipleObjects(numev, evlist, 0, INFINITE);
 if (n >= WAIT_OBJECT_0 && n <= WAIT_OBJECT_0  + numev) {
	return n - WAIT_OBJECT_0;
 }
 if (n == WAIT_FAILED) {
 	lasterr("WaitForMultipleObjects failed");
	return -1;
 }
 return -1;
#endif
}

static int check_cpstatus(enum kSMPlayCPToneStatus *stsp, tSMChannelId chan)
{
 SM_PLAY_CPTONE_STATUS_PARMS pcsp;
 int e;
 memset(&pcsp, 0, sizeof(pcsp));
 pcsp.channel = chan;
 e = sm_play_cptone_status(&pcsp);
 *stsp = pcsp.status;
 return e;
}

static int cndtest(tSMModuleId module, MVIP ts, int sastype, int onhook, int smdf, int anon, int outofarea, unsigned month, unsigned day, unsigned hour, unsigned minute, char *digits, char *name)
{
 tSMEventId evlist[2];	// 0 = play, 1 = recog
 ADSI_MODULE adsimod;
 tSMChannelId chan;
 int e = adsi_init(&adsimod, module);
 if (e) {
	fprintf(stderr, "adsi-init returned %d\n", e);
 	return 1;
 }
 if (alloc_chan(&chan, module)) return 1;
 if (assign_timeslot(chan, ts)) return 1;
 if (setup_events(&evlist[0], &evlist[1], chan)) return 1;
 if (dtmf_listen(chan)) return 1;
 if (play_sas_cas(chan, sastype, &adsimod)) return 1;
 for (;;) {	// wait for DTMF reply or end of SAS/CAS
	int w = multiwait(evlist, 2);
	if (w < 0) return 1;
	if (w == 0) {	// SAS is done
		enum kSMPlayCPToneStatus sts;
		if (check_cpstatus(&sts, chan)) return 1;
		if (sts == kSMPlayCPToneStatusComplete) break;
			// remove sas from waits
	} else if (w == 1) {	// maybe DTMF received
		char digit;
		e = get_digit(&digit, chan);
		if (e) return 1;
		printf("Got DTMF digit: %c\n", digit);
		if (abort_cptone(chan)) return 1;
	}
 }
 if (play_cnd(chan, onhook, smdf, anon, outofarea,
 	month, day, hour, minute, digits, name)) {
	return 1;
 }
 for (;;) {
	enum adsi_txstatus sts;
	e = smd_ev_wait(evlist[0]);
	if (e) {
		fprintf(stderr, "smd_ev_wait() returned %d\n", e);
		return 1;
	}
	e = adsi_status(&sts, chan);
	if (e) {
		fprintf(stderr, "adsi_status() returned %d\n", e);
		return 1;
	}
	if (sts == kADSIStatusComplete) break;
 }
 return 0;
}

#include "gen/cndtest.args.i"

int main(int argc, char **argv)
{
 unsigned month, day, hour, minute;
 tSMModuleId mod;
 tSMCardId card;
 ARGS_DECL
 int sts;
 (void) argc;
 if (ARGS_CALL || *argv) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE " \n", progname);
	return 1;
 }
 if (parse_time(&month, &day, &hour, &minute, arg.time)) {
	fprintf(stderr, "%s: Cannot parse time\n", progname);
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
 sts = cndtest(mod, arg.timeslot, arg.sastype, arg.onhook, arg.sdmf,
 	arg.anonymous, arg.out_of_area, month, day, hour, minute, arg.digits, arg.name);
 if (arg.serialnumber) {
	modclose(mod);
	cardclose(card);
 }
 return sts;
}
