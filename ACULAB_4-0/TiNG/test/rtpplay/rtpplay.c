/* rtpplay.c - demonstration of replay over RTP with an eventset */
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef TiNGTYPE_LINUX
#include <poll.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define USE_POLL
#endif

#ifdef TiNGTYPE_QNX
#include <sys/poll.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define USE_POLL
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

#include "../../libutil/generic_io.h"
#include "../../libutil/random.h"
#include "../Testlib/error.h"
#include "../Testlib/fmtcode.h"
#include "../Testlib/errcode_sm.h"
#include "../Testlib/eventset.h"
#include "../Testlib/perthread_trace.h"
#include "../Testlib/rtpclerk_cli.h"
#include "../Testlib/rtptx_codec.h"
#include "smbesp.h"
#include "smdrvr.h"
#include "smrtp.h"

#include "../Testlib/cardlist.h"
#include "../../apilib/smsync.h"	// unpublished API

#define arlen(x) (sizeof(x)/sizeof(*(x)))

#define APP_ACTIVE_CHANNEL 0
#define APP_ACTIVE_VMPTX 1

static tSMVMPTxToneSetId G_ToneSets[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0};

int g_count = 0;

struct appactive {
	unsigned type;
	union {
		struct {
			tSMChannelId chan;
			FILE *fp;
			int once;	// play only once and then stop
			enum {
				ACSTATE_IDLE,
				ACSTATE_BUSY,
				ACSTATE_UNDERRUN,
				ACSTATE_DONE,
			} acstate;
			struct threadsync {
				tSMSynchroniser synchroniser;
				pthread_mutex_t mx;
				pthread_cond_t synced;
				unsigned waitcount;
			} *sync;
			struct randnum *rp;
			unsigned xfersize;
		} c;
		struct {
			tSMVMPtxId vmptx;
			tSMModuleId mod;
			enum AVSTATE {
				AVSTATE_IDLE,
				AVSTATE_BUSY,
				AVSTATE_SSRC,
				AVSTATE_TONE,
				AVSTATE_TONEDONE,
				AVSTATE_STOP,
				AVSTATE_DONE,
			} avstate;
		} vt;
	} u; 
};
	// This conveniently gathers together the configurable
	// parameters for a channel
typedef struct {
		// parameters which can be changed during replay
	int agc;
	enum {
		ONCE_NOT,		// play file repeatedly
		ONCE_ONLY,		// play file exactly once
		ONCE_REPEAT,		// play once, then restart
	} once;
	int speed;
	int volume;
		// not changeable
	unsigned delay;
	enum kSMDataFormat format;
	tSM_UT32 rate;
	char *fname;
	unsigned curcard;
	unsigned curmod;
	CARDLIST *cards;
	//FIXME: background TS?
	//FIXME: main TS?
	unsigned max_octets;
	int threshold;
	unsigned per_mod;
	unsigned thismod;
	unsigned maxts;
	tSMChannelId bgchan;
	unsigned out_sample_rate;
} CHANPAR;

typedef struct vmppar {
	char *src_ip;
	char *dest_ip;
	RTPCLERK_SOCKET clerk_sock;
	unsigned use_clerk;
	unsigned short dest_port;
	unsigned short source_port;
	unsigned elim_tones;
	RTPTX_PAR rtptx_par;
	int sse_pt;
	int sse_spacing;
	int sse_repeats;
	char* srtp_key;
} VMPPAR;

	// A thread-safe, high quality random number generator
	// This is used to for the random startup delay option
struct randnum {
	RANDMT rand;
	pthread_mutex_t mx;
};


#define MAX_CSRCS 16

struct vmpcsrcs {
	tSMModuleId mod;
	tSMVMPTxCSRCListId csrcs;
	struct vmpcsrcs *next;
};

static struct vmpcsrcs *csrc_head = 0;

static err_t prosody_error(int rc, char *text);
static err_t get_csrc_list(tSMModuleId mod, tSMVMPTxCSRCListId *list)
{
	struct sm_vmptx_create_csrc_list_parms clp;
	struct vmpcsrcs **c = &csrc_head;
	int err;

	while(*c) {
		if ((*c)->mod == mod) {
			*list = (*c)->csrcs;
			return 0;
		}
		c = &((*c)->next);
	}

	*c = malloc(sizeof(struct vmpcsrcs));
	memset(*c, 0, sizeof(struct vmpcsrcs));
	memset(&clp, 0, sizeof(clp));
	clp.module = mod;
	err = sm_vmptx_create_csrc_list(&clp);
	if (err) {
		free(*c);
		*c = 0;
		return prosody_error(err, "sm_vmptx_create_csrc_list");
	}
	(*c)->mod = mod;
	(*c)->csrcs = clp.csrc_list;
	*list = (*c)->csrcs;
	return 0;
}

static int readint(void);
static int iseol(void);
static err_t set_csrcs_lists(void)
{
	struct vmpcsrcs **c = &csrc_head;
	struct sm_vmptx_csrc_list_set_parms csrcp;
	int e;
	int csrc_array[MAX_CSRCS];
	memset(&csrcp,0,sizeof(csrcp));
	csrcp.csrc = csrc_array;
	while (!iseol() && csrcp.num_csrc < MAX_CSRCS) {
		 csrc_array[csrcp.num_csrc++] = readint();
	}

	while(*c) {
		csrcp.csrc_list = (*c)->csrcs;
		e = sm_vmptx_csrc_list_set(&csrcp);
		if (e) return prosody_error(e, "sm_vmptx_csrc_list_set() failed");
		c = &((*c)->next);
	}
 return 0;
}

static void destroy_csrc_lists(void)
{
	struct vmpcsrcs **c = &csrc_head;
	struct sm_vmptx_destroy_csrc_list_parms csrcp;
	int e;
	memset(&csrcp,0,sizeof(csrcp));
	while(*c) {
		csrcp.csrc_list = (*c)->csrcs;
		e = sm_vmptx_destroy_csrc_list(&csrcp);
		if (e) {
			prosody_error(e, "sm_vmptx_destroy_csrc_list() failed");
			return;
		}
		c = &((*c)->next);
	}

}


	// Convert a Prosody error code and an explanation into an err_t
static err_t prosody_error(int rc, char *text)
{
 const char *name = errcode_sm(rc);
 char buf[80];
 if (!name) {
 	sprintf(buf, "Prosody error %d\n", rc);
 	name = buf;
 }
 return error(error(0, name), text);
}

static int sksp(void)
{
 for (;;) {
	int c = getchar();
	if (c != ' ' && c != '\t') return c;
 }
}

static int iseol(void)
{
 int c = sksp();
 ungetc(c, stdin);
 return c == '\n';
}


	// read an integer from stdin
static int readint(void)
{
 int isneg = 0, ndig = 0;
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

	// wait a random amount of time
static err_t sleeprand(struct randnum *rp, unsigned delay)
{
 int e = pthread_mutex_lock(&rp->mx);
 double d;
 if (e) return error_errno(e, "pthread_mutex_lock() failed");
 d = randmt_real1(&rp->rand) * delay;
 e = pthread_mutex_unlock(&rp->mx);
 if (e) return error_errno(e, "pthread_mutex_unlock() failed");
 Sleep(d);
 return 0;
}

	// the cleanup handler for when the channel gets deleted from 
	// the worker's list of active channels.
	//
	// This is run by the worker either because a work order
	// deleted the channel from the active list or because the
	// channel's event handler finished playing
	//
	// Note that was must not delete the resources since the main
	// thread might be in the middle of adjusting a parameter
static void cleanup_replay(ACTIVE_JOB *ajp)
{
 ajp->mf->u.c.acstate = ACSTATE_DONE;
}

static void cleanup_vmptx(ACTIVE_JOB *ajp)
{
 ajp->mf->u.vt.avstate = AVSTATE_DONE;
}

static void free_active(ACTIVE_JOB *ajp)
{
 free(ajp->mf);
 free(ajp);
}

static void destroy_tonesets(void)
{
 unsigned  i;
 for (i=0; i < arlen(G_ToneSets); i++) {
	if (G_ToneSets[i]) {
		struct sm_vmptx_destroy_toneset_parms tonep;
		tonep.tone_set_id = G_ToneSets[i];
		sm_vmptx_destroy_toneset(&tonep);
	}
 }
}

	// free any resources associated with the active channel
static void free_resources(ACTIVE_JOB *ajp)
{
 if (ajp->mf->type == APP_ACTIVE_CHANNEL) {
	smd_ev_free(ajp->event);
	sm_channel_release(ajp->mf->u.c.chan);
	if (ajp->mf->u.c.fp) fclose(ajp->mf->u.c.fp);
 } else if (ajp->mf->type == APP_ACTIVE_VMPTX) {
	sm_vmptx_destroy(ajp->mf->u.vt.vmptx);	
 }
 free_active(ajp);
}

	// check the current replay status
static err_t replay_status(ACTIVE_JOB *ajp, enum kSMReplayStatus *sp)
{
 struct sm_replay_status_parms rp;
 int e;
 memset(&rp, 0, sizeof(rp));
 rp.channel = ajp->mf->u.c.chan;
 e = sm_replay_status(&rp);
 if (e) return prosody_error(e, "sm_replay_status() failed");
 *sp = rp.status;
 return 0;
}

static err_t stop_vmptx(tSMVMPtxId vmptx)
{
 SM_VMPTX_STOP_PARMS vsp;
 int e;
 memset(&vsp, 0, sizeof(vsp));
 vsp.vmptx = vmptx;
 e = sm_vmptx_stop(&vsp);
 if (e) return prosody_error(e, "sm_vmptx_stop() failed");
 return 0;
}

static err_t set_csrcs_to_list(tSMVMPtxId vmptx, int delay, int packet_count, tSMVMPTxCSRCListId list, int exclude, int ssrc)
{
 struct sm_vmptx_set_csrc_parms csrcp;
 int e;
 memset(&csrcp,0,sizeof(csrcp));
 csrcp.vmptx = vmptx;
 csrcp.repeat_delay = delay;
 csrcp.packet_count = packet_count;
 csrcp.type = kSMVMPTxCSRCTypeList;
 csrcp.u.csrc_list.csrc_list_id = list;
 csrcp.u.csrc_list.exclude_ssrc = exclude;
 csrcp.u.csrc_list.ssrc = ssrc;
 e = sm_vmptx_set_csrc(&csrcp);
 if (e) return prosody_error(e, "sm_vmptx_set_csrc() failed");
 return 0;
}

static err_t set_csrcs(tSMVMPtxId vmptx, int delay, int packet_count)
{
 struct sm_vmptx_set_csrc_parms csrcp;
 int e;
 int csrc_array[MAX_CSRCS];
 memset(&csrcp,0,sizeof(csrcp));
 csrcp.vmptx = vmptx;
 csrcp.repeat_delay = delay;
 csrcp.packet_count = packet_count;
 csrcp.type = kSMVMPTxCSRCTypeArray;
 csrcp.u.csrc_array.csrc = csrc_array;
 while (!iseol() && csrcp.u.csrc_array.num_csrc < MAX_CSRCS) {
	 csrc_array[csrcp.u.csrc_array.num_csrc++] = readint();
 }
 e = sm_vmptx_set_csrc(&csrcp);
 if (e) return prosody_error(e, "sm_vmptx_set_csrc() failed");
 return 0;
}

static err_t generate_tones(tSMVMPtxId vmptx)
{
 static int tones[] = {10,9,8,7,6,5,4,3,2,1};
 struct sm_vmptx_generate_tones_parms tonep;
 int e;
 memset(&tonep, 0, sizeof(tonep));
 tonep.vmptx = vmptx;
 tonep.duration = 100;
 tonep.interval = 50;
 tonep.num = 10;
 tonep.volume = -3;
 tonep.tones = tones;
 e = sm_vmptx_generate_tones(&tonep);
 if (e) return prosody_error(e, "sm_vmptx_generate_tones() failed");
 return 0;
}

static err_t generate_tones_abort(tSMVMPtxId vmptx)
{
 struct sm_vmptx_generate_tones_abort_parms tonep;
 int e;
 memset(&tonep, 0, sizeof(tonep));
 tonep.vmptx = vmptx;
 e = sm_vmptx_generate_tones_abort(&tonep);
 if (e) return prosody_error(e, "sm_vmptx_generate_tones_abort() failed");
 return 0;
}
	// abort a replay
static err_t abort_replay(tSMChannelId chan)
{
 struct sm_replay_abort_parms rp;
 int e;
 memset(&rp, 0, sizeof(rp));
 rp.channel = chan;
 e = sm_replay_abort(&rp);
 if (e) return prosody_error(e, "sm_replay_abort() failed");
 return 0;
}

	// tell main thread that we have started if synchronisation
	// is being used
static err_t checksync(ACTIVE_JOB *ajp)
{
 if (ajp->mf->u.c.sync) {
	int e = pthread_mutex_lock(&ajp->mf->u.c.sync->mx);
	if (e) return error_errno(e, "pthread_mutex_lock() failed");
	if (!--ajp->mf->u.c.sync->waitcount) {
		e = pthread_cond_broadcast(&ajp->mf->u.c.sync->synced);
		if (e) return error_errno(e, "pthread_cond_broadcast() failed");
	}
	e = pthread_mutex_unlock(&ajp->mf->u.c.sync->mx);
	if (e) return error_errno(e, "pthread_mutex_unlock() failed");
	ajp->mf->u.c.sync = 0;
 }
 return 0;
}

	// we have capacity for more data on a channel - give it
	// some data if there is any to give it
static err_t handle_capacity(ACTIVE_JOB *ajp, int *had_eof)
{
 SM_TS_DATA_PARMS dp;
 char buff[2048];
 size_t nc = ajp->mf->u.c.fp ? fread(buff, 1, sizeof(buff), ajp->mf->u.c.fp) : sizeof(buff);
 err_t err;
 int e;
 if (ajp->mf->u.c.fp && ferror(ajp->mf->u.c.fp)) {
	return error(error_errno(errno, "fread()failed"),
		"Cannot read replay data file");
 }
 memset(&dp, 0, sizeof(dp));
 dp.channel = ajp->mf->u.c.chan;
 dp.data = buff;
 dp.length = nc;
 if (!nc) {	// EOF
	if (ajp->mf->u.c.once != ONCE_NOT) {
			// only playing file once - finish at EOF
			// then we may restart or quit
		e = sm_put_last_replay_data(&dp);
		if (e) return prosody_error(e, "sm_put_last_replay_data() failed");
	} 
	if (ajp->mf->u.c.once != ONCE_ONLY) {
			// playing repeatedly - start again at the beginning
			// this may play continuously or by restarting
		if (*had_eof) {
				// already had EOF - got no data after rewinding
			return error(0, "File is empty");
		}
		if (fseek(ajp->mf->u.c.fp, 0, SEEK_SET)) {
			err = error_errno(errno, "fseek() failed");
			return error(err, "Cannot rewind file");
		}
		*had_eof = 1;
		return 0;
	}
 } else {
	e = sm_put_replay_data(&dp);
	if (e) return prosody_error(e, "sm_put_replay_data() failed");
 }
 if (ajp->mf->u.c.acstate == ACSTATE_IDLE) ajp->mf->u.c.acstate = ACSTATE_BUSY;
 err = checksync(ajp);
 if (err) return error(err, "checksync() failed");
 return 0;
}

static int handle_vmptx(ACTIVE_JOB *ajp)
{
 SM_VMPTX_STATUS_PARMS statusp;
 enum AVSTATE ns = AVSTATE_IDLE;
 int e;
 memset(&statusp,0,sizeof(statusp));
 statusp.vmptx = ajp->mf->u.vt.vmptx;
 e = sm_vmptx_status(&statusp);
 if (e) {
	error_log(stderr, prosody_error(e, "sm_vmptx_status() failed"));
	stop_vmptx(ajp->mf->u.vt.vmptx);
	return 0;
 }
 switch (statusp.status) {
 case kSMVMPtxStatusRunning: ns = AVSTATE_BUSY; break;
 case kSMVMPtxStatusStopped:
	ajp->mf->u.vt.avstate = AVSTATE_STOP;
	return 1;
 case kSMVMPtxStatusToneOngoing: ns = AVSTATE_TONE; break;
 case kSMVMPtxStatusToneCompleted: ns = AVSTATE_TONEDONE; break;
 case kSMVMPtxStatusSSRC:
	printf("SSRC:%u\n", statusp.u.ssrc.ssrc);
	fflush(stdout);
	ns = AVSTATE_SSRC;
	break;
 }
 if (ns > ajp->mf->u.vt.avstate) ajp->mf->u.vt.avstate = ns;
 return 0;
}

	// the handler for when a channel's event is signalled
	// note that this is run by the worker thread
static int handle_replay(ACTIVE_JOB *ajp)
{
 err_t err = 0;
 int had_eof = 0;
 for (;;) {
	enum kSMReplayStatus status;
	err = replay_status(ajp, &status);
	if (err) break;
	switch (status) {
	case kSMReplayStatusUnderrun:
		ajp->mf->u.c.acstate = ACSTATE_UNDERRUN;
		return 0;
	case kSMReplayStatusComplete:
		if (ajp->mf->u.c.once == ONCE_REPEAT) {
			 struct sm_replay_parms rp;
			 int e;
			 memset(&rp, 0, sizeof(rp));
			 rp.channel = ajp->mf->u.c.chan;
			 rp.type = 8;
			 e = sm_replay_start(&rp);
			 if (e) {
				error_log(stderr, prosody_error(e, "sm_replay_start() failed"));
			 	return 1;
			 }
			 return 0;
		}
		return 1;
	case kSMReplayStatusCompleteData:
		return 0;
	case kSMReplayStatusHasCapacity:
		err = handle_capacity(ajp, &had_eof);
			/* optimisation: no need to re-check status
			 *
			 * Prosody version 2 (TiNG) guarantees that the
			 * event triggered by available space remains
			 * set until the space is used. This means that
			 * when we write some data, we do not need to
			 * worry about whether we have filled all the
			 * available space. Therefore we return. If there
			 * is more space avilable, we will simply find the
			 * event still set and get back here to handle it.
			 *
			 * In version 1, however, the event is only set by
			 * space *becoming* available, if we do not make
			 * full use of the space by filling it with data,
			 * we will never get another event and the replay
			 * will stall.
			 */
		if (!err) return 0;	// only Prosody version 2 (TiNG)
		break;
	case kSMReplayStatusNoCapacity:
		return 0;
	}
	if (err) break;
 }
	// some sort of error - abort and report
 error_log(stderr, error(err, "handle_replay() failed"));
 abort_replay(ajp->mf->u.c.chan);
 return 1;
}

/* Here are a bunch of little utility functions which really just
 * encapsulate one action each.
 */

	// allocate an ACTIVE_JOB object
static err_t alloc_active(ACTIVE_JOB **ajpp)
{
 *ajpp = malloc(sizeof(**ajpp));
 if (!*ajpp) {
	err_t err = error_errno(errno, "malloc() failed");
	return error(err, "cannot make channel object");
 }
 (*ajpp)->mf = malloc(sizeof(*(*ajpp)->mf));
 if (!(*ajpp)->mf) {
	err_t err = error_errno(errno, "malloc() failed");
	free(*ajpp);
	return error(err, "cannot make private fields for channel object");
 }
 return 0;
}

	// open a file for reading
static err_t open_file(ACTIVE_JOB *ajp, char *fmain, int par1, int par2)
{
 err_t err = 0;
 if (!fmain) {
 	ajp->mf->u.c.fp = 0;
 } else {
	char *fname = malloc(strlen(fmain) + sizeof("0123456789")*2);
	if (!fname) return error_errno(errno, "malloc() failed");
	sprintf(fname, fmain, par1, par2);
	ajp->mf->u.c.fp = fopen(fname, "rb");
	if (!ajp->mf->u.c.fp) {
		char *n;
		err = error_errno(errno, "fopen() failed");
		n = malloc(strlen(fname) + sizeof("File: ''"));
		if (n) sprintf(n, "File: '%s'", fname);
		err = error(err, n);
		err = error(err, "Cannot open file");
		free(n);
	}
	free(fname);
 }
 return err;
}

static err_t create_vmptx(ACTIVE_JOB *ajp, tSMModuleId module)
{
 SM_VMPTX_CREATE_PARMS vcp;
 int e;
 memset(&vcp, 0, sizeof(vcp));
 vcp.module = module;
 e = sm_vmptx_create(&vcp);
 if (e) return prosody_error(e, "sm_vmptx_create() failed");
 ajp->mf->u.vt.vmptx = vcp.vmptx;
 ajp->mf->u.vt.mod = module;
 ajp->mf->type = APP_ACTIVE_VMPTX;
 return 0;
}

	// allocate a Prosody channel
static err_t alloc_chan(ACTIVE_JOB *ajp, tSMModuleId module)
{
 SM_CHANNEL_ALLOC_PLACED_PARMS ap;
 int e;
 memset(&ap, 0, sizeof(ap));
 ap.type = kSMChannelTypeOutput;
 ap.module = module;
 e = sm_channel_alloc_placed(&ap);
 if (e) return prosody_error(e, "sm_channel_alloc_placed() failed");
 ajp->mf->u.c.chan = ap.channel;
 ajp->mf->type = APP_ACTIVE_CHANNEL;
 return 0;
}

static err_t get_datafeed(ACTIVE_JOB *ajp, tSMDatafeedId *df)
{
 SM_CHANNEL_DATAFEED_PARMS dfp;
 int e;
 memset(&dfp, 0, sizeof(dfp));
 dfp.channel = ajp->mf->u.c.chan;
 e = sm_channel_get_datafeed(&dfp);
 if (e) return prosody_error(e, "sm_channel_get_datafeed() failed");
 *df = dfp.datafeed;
 return 0;
}

static err_t connect_datafeed(ACTIVE_JOB *ajp, tSMDatafeedId df)
{
 SM_VMPTX_DATAFEED_CONNECT_PARMS dfp;
 int e;
 memset(&dfp, 0, sizeof(dfp));
 dfp.vmptx = ajp->mf->u.vt.vmptx;
 dfp.data_source = df;
 e = sm_vmptx_datafeed_connect(&dfp);
 if (e) return prosody_error(e, "sm_vmptx_datafeed_connect() failed");
 return 0;
}

static err_t get_event(ACTIVE_JOB *ajp)
{
 SM_VMPTX_EVENT_PARMS evp;
 int e;
 memset(&evp, 0, sizeof(evp));
 evp.vmptx = ajp->mf->u.vt.vmptx;
 e = sm_vmptx_get_event(&evp);
 if (e) return prosody_error(e, "sm_vmptx_get_event() failed");
 ajp->event = evp.event;
 return 0;
}

	// allocate an event
static err_t alloc_event(ACTIVE_JOB *ajp)
{
 int e = smd_ev_create(&ajp->event, ajp->mf->u.c.chan,
	kSMEventTypeWriteData, kSMChannelSpecificEvent);
 if (e) return prosody_error(e, "smd_ev_create() failed");
 return 0;
}

	// associate event with channel
static err_t associate_event(ACTIVE_JOB *ajp)
{
 SM_CHANNEL_SET_EVENT_PARMS sep;
 int e;
 memset(&sep, 0, sizeof(sep));
 sep.event = ajp->event;
 sep.channel = ajp->mf->u.c.chan;
 sep.event_type = kSMEventTypeWriteData;
 sep.issue_events = kSMChannelSpecificEvent;
 e = sm_channel_set_event(&sep);
 if (e) return prosody_error(e, "sm_channel_set_event() failed");
 return 0;
}


static err_t config_vmptx(ACTIVE_JOB *ajp, VMPPAR vmppar, tSMModuleId module, int modix)
{
 tSMVMPtxId vmptx = ajp->mf->u.vt.vmptx; 
 err_t err;
 int e;
 
 struct addrinfo hints1;
 struct addrinfo *result1;
 struct addrinfo hints2;
 struct addrinfo *result2;

 int s;
 memset(&hints1, 0, sizeof(struct addrinfo));
 hints1.ai_family = AF_UNSPEC;	/* Allow IPv4 or IPv6 */
 hints1.ai_socktype = SOCK_DGRAM;
 hints1.ai_protocol = IPPROTO_UDP;
 hints1.ai_flags = AI_NUMERICHOST;

 s = getaddrinfo(vmppar.dest_ip, NULL, &hints1, &result1);
 if (s != 0) {
	err = error(error(NULL, gai_strerror(s)), "getaddrinfo dest failed");
	return err;
 }

 if (vmppar.src_ip != 0) {
 	memset(&hints2, 0, sizeof(struct addrinfo));
 	hints2.ai_family = AF_UNSPEC;  /* Allow IPv4 or IPv6 */
 	hints2.ai_socktype = SOCK_DGRAM;
 	hints2.ai_protocol = IPPROTO_UDP;
 	hints2.ai_flags = AI_NUMERICHOST;

 	s = getaddrinfo(vmppar.src_ip, NULL, &hints2, &result2);
 	if (s != 0) {
		err = error(error(NULL, gai_strerror(s)), "getaddrinfo src failed");
		return err;
 	}
 }

 if (result1->ai_family == AF_INET) {
	SM_VMPTX_CONFIG_PARMS configp;
	memset(&configp, 0, sizeof(configp));
	configp.vmptx = vmptx;
	configp.destination_rtp.sin_addr.s_addr = ((struct sockaddr_in*)result1->ai_addr)->sin_addr.s_addr;
	configp.destination_rtp.sin_port = htons(vmppar.dest_port);
	configp.destination_rtcp.sin_addr.s_addr = ((struct sockaddr_in*)result1->ai_addr)->sin_addr.s_addr;
	configp.destination_rtcp.sin_port = htons(vmppar.dest_port+1);
	configp.source_rtp.sin_port = htons(vmppar.source_port);
	configp.source_rtcp.sin_port = htons(vmppar.source_port+1);
    if (vmppar.src_ip != 0) {
		configp.source_rtp.sin_addr.s_addr = ((struct sockaddr_in*)result2->ai_addr)->sin_addr.s_addr;
		configp.source_rtcp.sin_addr.s_addr = ((struct sockaddr_in*)result2->ai_addr)->sin_addr.s_addr;
	}
	e = sm_vmptx_config(&configp);
	if (e) return prosody_error(e, "sm_vmptx_config() failed");
 } else {
	SM_VMPTX_CONFIG_IPV6_PARMS configp;
	memset(&configp, 0, sizeof(configp));
	configp.vmptx = vmptx;
	configp.destination_rtp.sin6_addr = ((struct sockaddr_in6*)result1->ai_addr)->sin6_addr;
	configp.destination_rtp.sin6_port = htons(vmppar.dest_port);
	configp.destination_rtp.sin6_flowinfo = htonl(0 << 20);
	configp.destination_rtcp.sin6_addr = ((struct sockaddr_in6*)result1->ai_addr)->sin6_addr;
	configp.destination_rtcp.sin6_port = htons(vmppar.dest_port+1);
	configp.source_rtp.sin6_port = htons(vmppar.source_port);
	configp.source_rtcp.sin6_port = htons(vmppar.source_port+1);
    if (vmppar.src_ip != 0) {
		configp.source_rtp.sin6_addr = ((struct sockaddr_in6*)result2->ai_addr)->sin6_addr;
		configp.source_rtcp.sin6_addr = ((struct sockaddr_in6*)result2->ai_addr)->sin6_addr;
    }
	e = sm_vmptx_config_ipv6(&configp);
	if (e) return prosody_error(e, "sm_vmptx_config() failed");
 }
 freeaddrinfo(result1);
 if (vmppar.src_ip != 0) {
	freeaddrinfo(result2);
 }

	//and now the codec
 err = rtptx_config_vmptx(vmptx, &vmppar.rtptx_par);
 if (err) return err;
 if (vmppar.elim_tones) {
	struct sm_vmptx_tone_parms tparms;
	SM_VMPTX_CODEC_RFC2833_PARMS tcp;
	if(G_ToneSets[modix] == 0 ) {
		struct sm_vmptx_create_toneset_parms tsparms;
		tsparms.module = module;
		tsparms.toneset = kSMVMPTxDefaultToneSet;
		e = sm_vmptx_create_toneset(&tsparms);
 		if (e) return prosody_error(e, "sm_vmptx_config_toneset() failed");
		G_ToneSets[modix] = tsparms.tone_set_id;
	}
	//turn on tones;
	memset(&tcp, 0, sizeof(tcp));
	tcp.vmptx = vmptx;
	tcp.payload_type = vmppar.elim_tones;
	e = sm_vmptx_config_codec_rfc2833(&tcp);
	if (e) return prosody_error(e,"sm_vmptx_config_code_rfc2833() failed");
	memset(&tparms, 0, sizeof(tparms));
	tparms.vmptx = vmptx;
	tparms.convert_tones = 1;
	tparms.tone_set_id = G_ToneSets[modix];
	tparms.elim_tones = 1;
	e = sm_vmptx_config_tones(&tparms);
 	if (e) return prosody_error(e, "sm_vmprx_config_toneset() failed");
 }
 if (vmppar.srtp_key) {
	SM_VMPTX_CONFIG_ENCRYPTION_AES_CM_PARMS ep;
	SM_VMPTX_CONFIG_AUTHENTICATION_HMAC_SHA1_PARMS ap;
	memset(&ap, 0, sizeof(ap));
	ap.vmptx = vmptx;
	ap.key = vmppar.srtp_key;
	ap.keylen = 30;
	ap.taglen = 80;
	e = sm_vmptx_config_authentication_hmac_sha1(&ap);
	if (e) return prosody_error(e, "sm_vmptx_config_authentication_hmac_sha1() failed");

	memset(&ep, 0, sizeof(ep));
	ep.vmptx = vmptx;
	ep.key = vmppar.srtp_key;
	ep.keylen = 30;
	e = sm_vmptx_config_encryption_aes_cm(&ep);
	if (e) return prosody_error(e, "sm_vmptx_config_encryption_aes_cm() failed");
 }
 return 0;
}
	// start playing
static err_t start_replay(ACTIVE_JOB *ajp, CHANPAR chanpar)
{
 struct sm_replay_parms rp;
 err_t err;
 int e;
 err = sleeprand(ajp->mf->u.c.rp, chanpar.delay);
 if (err) return error(err, "sleeprand() failed");
 if (chanpar.threshold) {
	SM_CHANNEL_SET_OUTPUT_THRESHOLD_PARMS sotp;
	memset(&sotp, 0, sizeof(sotp));
	sotp.channel = ajp->mf->u.c.chan;
	sotp.minimum_bits = chanpar.threshold;
	e = sm_channel_set_output_threshold(&sotp);
	if (e) return prosody_error(e, "sm_channel_set_output_threshold() failed");
 }
 if (chanpar.out_sample_rate) {
 	SM_CHANNEL_SET_OUTPUT_RATE_PARMS orp;
 	memset(&orp, 0, sizeof(orp));
 	orp.channel = ajp->mf->u.c.chan;
 	orp.sample_rate = chanpar.out_sample_rate;
 	e = sm_channel_set_output_rate(&orp);
	if (e) return prosody_error(e, "sm_channel_set_output_rate() failed");
 }
 memset(&rp, 0, sizeof(rp));
 rp.channel = ajp->mf->u.c.chan;
 rp.background = chanpar.bgchan;
 rp.volume = chanpar.volume;
 rp.speed = chanpar.speed;
 rp.agc = chanpar.agc;
 rp.type = chanpar.format;
 rp.sampling_rate = chanpar.rate;
 rp.data_length = chanpar.max_octets;
 e = sm_replay_start(&rp);
 if (e) return prosody_error(e, "sm_replay_start() failed");
 if (ajp->mf->u.c.sync) {
	SM_SYNC_ADD_PARMS sa;
	memset(&sa, 0, sizeof(sa));
	sa.channel = ajp->mf->u.c.chan;
	sa.synchroniser = ajp->mf->u.c.sync->synchroniser;
	e = sm_sync_add(&sa);
	if (e) return prosody_error(e, "sm_sync_add() failed");
 }
 return 0;
}

static err_t config_vmptx_rate(ACTIVE_JOB *ajp, unsigned sample_rate)
{
 if (sample_rate) {
 	SM_VMPTX_SAMPLE_RATE_PARMS srp;
 	int e;
 	memset(&srp, 0, sizeof(srp));
 	srp.vmptx = ajp->mf->u.vt.vmptx;
 	srp.sample_rate = sample_rate;
 	e = sm_vmptx_config_sample_rate(&srp);
	if (e) return prosody_error(e, "sm_vmptx_config_sample_rate() failed");
 }
 return 0;
}

static err_t config_vmptx_sse(ACTIVE_JOB *ajp, VMPPAR *vmppar)
{
 SM_VMPTX_CODEC_SSE_PARMS srp;
 int e;
 if (vmppar->sse_pt) {
	 memset(&srp, 0, sizeof(srp));
	 srp.vmptx = ajp->mf->u.vt.vmptx;
	 srp.payload_type = vmppar->sse_pt;
	 srp.retransmissions = vmppar->sse_repeats;
	 srp.delay = vmppar->sse_spacing;
	 e = sm_vmptx_config_codec_sse(&srp);
	 if (e) return prosody_error(e, "sm_vmptx_config_codec_sse() failed");
 }
 return 0;
}

static err_t send_sse(tSMVMPtxId vmptx, char *buf, int length)
{
 SM_VMPTX_SEND_SSE_PARMS vsp;
 int e;
 memset(&vsp, 0, sizeof(vsp));
 vsp.vmptx = vmptx;
 vsp.payload = buf;
 vsp.payload_length = length;
 e = sm_vmptx_send_sse(&vsp);
 if (e) return prosody_error(e, "sm_vmptx_send_sse() failed");
 return 0;
}

	// adjust a replay which has already started
static err_t adj_replay(tSMChannelId chan, CHANPAR chanpar)
{
 struct sm_replay_adjust_parms rp;
 int e;
 memset(&rp, 0, sizeof(rp));
 rp.channel = chan;
 rp.background = chanpar.bgchan;
 rp.volume = chanpar.volume;
 rp.speed = chanpar.speed;
 rp.agc = chanpar.agc;
 e = sm_replay_adjust(&rp);
 if (e) return prosody_error(e, "sm_replay_adjust() failed");
 return 0;
}

	// start a new replay and VMP[tx]
static err_t playrtp(ACTIVE_JOB **ajpp, EVENTSET *evs, CHANPAR chanpar, VMPPAR vmppar, struct threadsync *ts, struct randnum *rp)
{
 ACTIVE_JOB *ajp_c, *ajp_v;
 tSMDatafeedId datafeed;
 err_t err = alloc_active(&ajp_c);
 	//watch out for this return
 if (err) return error(err, "Cannot play file");
 err = alloc_active(&ajp_v);
 if (!err) {
	ajpp[0] = ajp_c;
	ajpp[1] = ajp_v;
	ajp_c->mf->u.c.acstate = ACSTATE_IDLE;
	ajp_v->mf->u.vt.avstate = AVSTATE_IDLE;
	ajp_c->mf->u.c.sync = ts;
	ajp_c->handler = handle_replay;
	ajp_v->handler = handle_vmptx;
	ajp_c->cleanup_fn = cleanup_replay;
	ajp_v->cleanup_fn = cleanup_vmptx;
	ajp_c->mf->u.c.once = chanpar.once;
	if (chanpar.threshold <= 0) ajp_c->mf->u.c.xfersize = 2048;
	else ajp_c->mf->u.c.xfersize = chanpar.threshold / 8;
	ajp_c->mf->u.c.rp = rp;
	err = open_file(ajp_c, chanpar.fname, g_count, 0);
	g_count++;
	if (!err) {
		MODDESC mod;
		err = cardlist_getmod(&mod, chanpar.cards, chanpar.curcard, chanpar.curmod);
		if (!err) err = alloc_chan(ajp_c, mod.id);
			if (!err) {
				err = create_vmptx(ajp_v, mod.id);
				if (!err) {
					err = config_vmptx_rate(ajp_v, chanpar.out_sample_rate);
					vmppar.rtptx_par.sample_rate = chanpar.out_sample_rate;

					if (!err) err = config_vmptx(ajp_v, vmppar, mod.id, chanpar.curmod);
					if (!err) err = config_vmptx_sse(ajp_v, &vmppar);
					if (!err) err = get_datafeed(ajp_c, &datafeed);
					if (!err) err = connect_datafeed(ajp_v, datafeed);
					if (!err) err = alloc_event(ajp_c);
					if (!err) {
						err = associate_event(ajp_c);
						if (!err) err = start_replay(ajp_c, chanpar);
						if (!err) err = eventset_insert(evs, ajp_c);
						if (!err) err = get_event(ajp_v);
						if (!err) err = eventset_insert(evs, ajp_v);
						if (!err) return 0;
						smd_ev_free(ajp_c->event);
					}
					//FIXME: Breaks all the rules
					sm_vmptx_destroy(ajp_v->mf->u.vt.vmptx);	
				}
				sm_channel_release(ajp_c->mf->u.c.chan);
			}
		if (ajp_c->mf->u.c.fp) fclose(ajp_c->mf->u.c.fp);
	}
	free_active(ajp_v);
	ajpp[0] = 0;
	ajpp[1] = 0;
 }
 free_active(ajp_c);
 return error(err, "Cannot play file");
}

	// the worker thread is this function
static void *worker_thread(void *p)
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

	// start the worker thread
static err_t startworker(pthread_t *tidp, EVENTSET *evs)
{
 err_t err = eventset(evs);
 int e;
 if (err) return err;
 e = pthread_create(tidp, 0, worker_thread, evs);
 if (e) {
 	err = error_errno(e, "pthread_create() failed");
 	err = error(err, "cannot start worker thread");
 }
 return err;
}

static err_t stopworker(pthread_t tid, EVENTSET *evs)
{
 void *sts;
 err_t err = eventset_stop(evs);
 int e;
 if (err) return err;
 e = pthread_join(tid, &sts);
 if (e) {
 	err = error_errno(e, "pthread_join() failed");
 	return error(err, "cannot join with worker thread");
 }
 return 0;
}

	// allow synchronised replays to start now
static err_t runsync(struct threadsync *ts)
{
 SM_SYNC_DELETE_PARMS sp;
 int e;
 if (!ts) {
	printf("Not awiting sync\n");
	return 0;
 }
 memset(&sp, 0, sizeof(sp));
 e = pthread_mutex_lock(&ts->mx);
 if (e) return error_errno(e, "pthread_mutex_lock() failed");
 while (ts->waitcount) {
		e = pthread_cond_wait(&ts->synced, &ts->mx);
		if (e) return error_errno(e, "pthread_cond_wait() failed");
 }
 e = pthread_mutex_unlock(&ts->mx);
 if (e) return error_errno(e, "pthread_mutex_unlock() failed");
 e = pthread_mutex_destroy(&ts->mx);
 if (e) return error_errno(e, "pthread_mutex_destoy() failed");
 e = pthread_cond_destroy(&ts->synced);
 if (e) return error_errno(e, "pthread_cond_destoy() failed");
 sp.synchroniser = ts->synchroniser;
 e = sm_sync_delete(&sp);
 if (e) return prosody_error(e, "sm_sync_delete() failed");
 free(ts);
 return 0;
}
#if 0
	// setup the background channel
	// This simple sets up a dummy channel which uses the selected
	// timeslot - it doesn't actually play anything down it. To
	// test background replay, use some other test program to
	// generate a signal on this timeslot
static err_t setup_background(CHANPAR *chanparp, MVIP bg)
{
 SM_CHANNEL_ALLOC_PLACED_PARMS ap;
 SM_SWITCH_CHANNEL_PARMS sw;
 MODDESC mod;
 err_t err;
 int e;
 chanparp->bgchan = kSMNullChannelId;
 if (!bg.stream) return 0;
 memset(&ap, 0, sizeof(ap));
 ap.type = bg.timeslot >= 32 ? kSMChannelTypeInput : kSMChannelTypeOutput;
 err = cardlist_getmod(&mod, chanparp->cards, chanparp->curcard, chanparp->curmod);
 if (err) return err;
 ap.module = mod.id;
 e = sm_channel_alloc_placed(&ap);
 if (e) return prosody_error(e, "sm_channel_alloc_placed() failed");
 chanparp->bgchan = ap.channel;
 memset(&sw, 0, sizeof(sw));
 sw.channel = ap.channel;
 sw.st = bg.stream;
 sw.ts = bg.timeslot;
#ifdef PROSODY_TiNG
 sw.type = bg.type;
#endif
 e = sm_switch_channel_output(&sw);
 if (e) {
	err = prosody_error(e, "sm_switch_channel_output() failed");
	e = sm_channel_release(chanparp->bgchan);
	chanparp->bgchan = kSMNullChannelId;
	if (e) {
		err_t nerr = prosody_error(e, "sm_channel_release() failed");
		return error_join(err, nerr);
	}
 }
 return 0;
}
#endif
/*
 * The main play loop. Since we want to display channel status and adjust
 * channels, the cleanup function above cannot free the resources (in case
 * we were accessing the channel in this thread while it was freed in the
 * worker thread). Therefore we need to check for finished channels and
 * clear up. There is no real penalty in deferring this cleanup since
 * if we are sitting waiting for a command there's nothing useful we
 * could do with the newly freed resources.
 */
static err_t rtpplay(CHANPAR chanpar, VMPPAR vmppar, int use_sync, unsigned numchan)
{
 ACTIVE_JOB *ajlist[4096];
 struct threadsync *ts = 0;
 unsigned freeac = 0;		// free entry after all used ones in ajlist
 struct randnum ran;
 int waiting = 0;
#ifdef USE_POLL
 EVENTSET evset;
 pthread_t tid;
#endif
#ifdef TiNGTYPE_WINNT
 EVENTSET evset[(arlen(ajlist) + 59)/60];
 pthread_t tid[arlen(evset)];
 unsigned nextevset = 0;
#endif
 unsigned u;
 err_t err;
 int e;
#ifdef USE_POLL
 err = startworker(&tid, &evset);
 if (err) return err;
#endif
 randmt(&ran.rand, 1);
 e = pthread_mutex_init(&ran.mx, 0);
 if (e) return error_errno(e, "pthread_mutex_init() failed");
#if 0
 err = setup_background(&chanpar, chanpar.background);
 if (err) return error(err, "cannot setup background channel");
#endif
 if (use_sync) {
 		/*
 		 * This synchroniser uses an unpublished API. It is
 		 * unpublished because the correct use of the
 		 * synchroniser requires detailed knowlege of how
 		 * the operations are implemented to ensure that the
 		 * channel gets up to the right point in processing
 		 * before being frozen awaiting permission to start.
 		 * The information required to do this correctly may
 		 * change arbitrarily from release to release and may
 		 * not work at all in some releases. Therefore it is
 		 * for Aculab use only, and even then only for certain
 		 * specific tests.
 		 */
	SM_SYNC_CREATE_PARMS sp;
	MODDESC mod;
	ts = malloc(sizeof(*ts));
	if (!ts) {
		err = error_errno(errno, "malloc() failed");
		return error(err, "cannot make synchroniser object");
	}
	ts->waitcount = numchan;
	memset(&sp, 0, sizeof(sp));
	err = cardlist_getmod(&mod, chanpar.cards, chanpar.curcard, chanpar.curmod);
	if (err) return err;
	sp.module = mod.id;
	e = sm_sync_create(&sp);
	if (e) return prosody_error(e, "sm_sync_create() failed");
	ts->synchroniser = sp.synchroniser;
	e = pthread_mutex_init(&ts->mx, 0);
	if (e) return error_errno(e, "pthread_mutex_init() failed");
	e = pthread_cond_init(&ts->synced, 0);
	if (e) return error_errno(e, "pthread_cond_init() failed");
 }
 for (;;) {	// the main loop waiting for commands
		/* if we wanted to have a continuous status update
		 * (such as every second), a good way to do it would
		 * be to use a separate thread to read the input and
		 * pass it to a manager thread which uses a wait
		 * with timeout to read the input and prints the new
		 * status on a timeout. Since it's easy enough to get
		 * a status by hitting CR, and printing status would
		 * interfere with measuring CPU usage, we don't
		 * bother.
		 */
	int c;
	putchar('\r');
	for (u=0; u < freeac; u++) {
		if (!ajlist[u]) putchar(' ');
		else if (ajlist[u]->mf->type == APP_ACTIVE_CHANNEL) {
			switch (ajlist[u]->mf->u.c.acstate) {
			case ACSTATE_IDLE: putchar('.'); break;
			case ACSTATE_BUSY:
				putchar('*');
				ajlist[u]->mf->u.c.acstate = ACSTATE_IDLE;
				break;
			case ACSTATE_UNDERRUN:
				putchar('u');
				ajlist[u]->mf->u.c.acstate = ACSTATE_IDLE;
				break;
			case ACSTATE_DONE:
				putchar('-');
				free_resources(ajlist[u]);
				ajlist[u] = 0;
				break;
			}
		} else if (ajlist[u]->mf->type == APP_ACTIVE_VMPTX) {
			switch(ajlist[u]->mf->u.vt.avstate) {
			case AVSTATE_IDLE: putchar('v'); break;
			case AVSTATE_BUSY:
				putchar('V');
				ajlist[u]->mf->u.vt.avstate = ACSTATE_IDLE;
				break;
			case AVSTATE_TONE:
				putchar('t');
				ajlist[u]->mf->u.vt.avstate = ACSTATE_IDLE;
				break;
			case AVSTATE_TONEDONE:
				putchar('T');
				ajlist[u]->mf->u.vt.avstate = ACSTATE_IDLE;
				break;
			case AVSTATE_SSRC:
				putchar('s');
				ajlist[u]->mf->u.vt.avstate = ACSTATE_IDLE;
				break;
			case AVSTATE_STOP:
				putchar('S');
				ajlist[u]->mf->u.vt.avstate = ACSTATE_IDLE;
				break;
			case AVSTATE_DONE:
				putchar('-');
				free_resources(ajlist[u]);
				ajlist[u] = 0;
				break;
			}
		}	
	}
	while (freeac > 0 && !ajlist[freeac-1]) freeac--;
	if (ts) printf("sync awaiting 'g'");
	putchar(')');
	putchar('>');
	if (numchan) {
			/* we've been asked to pre-load a certain number
			 * of channels - while we have more to create,
			 * pretend that we got a '+' command.
			 */
		c = '+';
		numchan--;
	} else if (waiting) {
		static int twiddle;
		if (!freeac) {
			putchar('\n');
			waiting = 0;
		} else {
			putchar('W');
			putchar("-\\|/"[twiddle++]);
			twiddle &= 3;
			fflush(stdout);
			Sleep(1000);
		}
		c = '\n';
	} else c = getchar();
	switch (c) {
#ifdef USE_POLL
#define evsetp (&evset)
#endif
#ifdef TiNGTYPE_WINNT
		EVENTSET *evsetp;
		unsigned evsetno;
#endif
	case '\n': break;
	case '+':	// start new channel
	{
		for (u=0; u < freeac && ajlist[u]; u++);
		if (u == arlen(ajlist)) {
			printf("cannot start: too many channels\n");
			break;
		}
		if (u == freeac) freeac++;
#ifdef TiNGTYPE_WINNT
		evsetno = u / 30; //30 pairs = 60
		evsetp = &evset[evsetno];
		if (evsetno >= nextevset) {
			err = startworker(&tid[evsetno], evsetp);
			nextevset++;
		}
#endif
		if (vmppar.use_clerk) {
			char *ipaddr, *msg = 0;
			short port_num;
			int rc;
			printf("awaiting port number\n");
			rc = rtpclerk_cli_request_port(vmppar.clerk_sock, &msg);
			if (rc) return error_errno(rc, "failed to get port");
			ipaddr = malloc(256);
			if (!ipaddr) {
				return error_errno(-1, "malloc failed for ipaddr space\n");
			}
			rc = rtpclerk_cli_msg2port(msg, ipaddr, &port_num);
			if (rc) {
				free(ipaddr);
				free(msg);
				return error_errno(rc, "failed to decode port");
			}
			free(vmppar.dest_ip);
			vmppar.dest_ip = ipaddr;
			free(msg);
			vmppar.dest_port = port_num;
		}
		printf("starting replay\n");
	
		err = playrtp(&ajlist[u], evsetp, chanpar, vmppar, ts, &ran);
		if (err) error_log(stderr, err);
		else {
			if (chanpar.per_mod && ++chanpar.thismod >= chanpar.per_mod) {
				chanpar.thismod = 0;
				cardlist_nextmod(chanpar.cards, &chanpar.curcard, &chanpar.curmod);
			}
		}
		//ensure we don't re-use the same port and move freeac one more for the VMP[tx]
		if (!vmppar.use_clerk) {
			vmppar.dest_port += 2;
		}
		vmppar.source_port += 2;
		freeac++;
		break;
	}
	case '?':	// help
		printf("Commands:\n"
			"\t+\tstart replay with current agc, speed, and volume\n"
			"\t#...\tcomment\n"
			"\tTN\tset TiNGtrace to N\n"
			"\tAN N|W bitrate\tset AMR-[N|W]B CMR request to bitrate on VMPtx N\n"
			"\taN\tset agc (0=off, 1=on)\n"
			"\tBN N|W bitrate\tset AMR-[N|W]B bitrate on VMPtx N\n"
#ifdef PROSODY_TiNG
			"\tg\tgo - start synced channels\n"
#endif
			"\tkN\tkill channel N\n"
			"\toN\tset once (0=repeat, 1=once, 2=once+restart)\n"
			"\tsN\tset speed\n"
			"\tuN\tupdate channel N to current agc, speed and volume\n"
			"\tvN\tset volume\n"
			"\txN\tset max octets\n"
			"\tdN\tplay a stream of digits\n"
			"\tDN\tabort playing stream of digits\n"
			"\tcN d n [C C ...]\tset vmptx csrcs with repeat delay d, packet count n\n"
			"\tC [C C ...]\tset csrc lists\n"
			"\tlN d n [ex ssrc]\tset vmptx csrc to list with repeat delay d, packet count n\n"
			"\tq\tquit\n"
			"\t?\tshow command list\n"
			);
		break;
	case '#':
		for (;;) {
			c = getchar();
			if (c == EOF) return 0;
			putchar(c);
			if (c == '\n') break;
		}
		break;
	case 'A':
		{
			int t, b;
			u = readint();
			if (iseol()) {
				printf("Expected N|W after 'A'\n");
				break;
			}
			t = getchar();
			if (t != 'N' && t != 'W') {
				printf("Expected N|W after 'A'\n");
				break;
			}
			if (iseol()) {
				printf("Expected bitrate after 'A'\n");
				break;
			}
			b = readint();
			if (ajlist[u]->mf->type == APP_ACTIVE_VMPTX) {
				if (t == 'N') {
					SM_VMPTX_CONFIG_CODEC_AMRNB_CMR_PARMS amrp;
					memset(&amrp, 0, sizeof(amrp));
					amrp.vmptx = ajlist[u]->mf->u.vt.vmptx;
					amrp.bitrate = b;
					e = sm_vmptx_config_codec_amrnb_cmr(&amrp);
					if (e) {
						err = prosody_error(e, "sm_vmptx_config_codec_amrnb_cmr");
						if (err) error_log(stderr, err);
					}
				} else { // t == 'W'
					SM_VMPTX_CONFIG_CODEC_AMRWB_CMR_PARMS amrp;
					memset(&amrp, 0, sizeof(amrp));
					amrp.vmptx = ajlist[u]->mf->u.vt.vmptx;
					amrp.bitrate = b;
					e = sm_vmptx_config_codec_amrwb_cmr(&amrp);
					if (e) {
						err = prosody_error(e, "sm_vmptx_config_codec_amrwb_cmr");
						if (err) error_log(stderr, err);
					}
				}
			}
		}
		break;
	case 'a':	// agc
		chanpar.agc = readint();
		break;
	case 'B':
		{
			int t, b;
			u = readint();
			if (iseol()) {
				printf("Expected N|W after 'B'\n");
				break;
			}
			t = getchar();
			if (t != 'N' && t != 'W') {
				printf("Expected N|W after 'B'\n");
				break;
			}
			if (iseol()) {
				printf("Expected bitrate after 'B'\n");
				break;
			}
			b = readint();
			if (ajlist[u]->mf->type == APP_ACTIVE_VMPTX) {
				if (t == 'N') {
					SM_VMPTX_CONFIG_CODEC_AMRNB_MODE_PARMS amrp;
					memset(&amrp, 0, sizeof(amrp));
					amrp.vmptx = ajlist[u]->mf->u.vt.vmptx;
					amrp.bitrate = b;
					e = sm_vmptx_config_codec_amrnb_mode(&amrp);
					if (e) {
						err = prosody_error(e, "sm_vmptx_config_codec_amrnb_mode");
						if (err) error_log(stderr, err);
					}
				} else { // t == 'W'
					SM_VMPTX_CONFIG_CODEC_AMRWB_MODE_PARMS amrp;
					memset(&amrp, 0, sizeof(amrp));
					amrp.vmptx = ajlist[u]->mf->u.vt.vmptx;
					amrp.bitrate = b;
					e = sm_vmptx_config_codec_amrwb_mode(&amrp);
					if (e) {
						err = prosody_error(e, "sm_vmptx_config_codec_amrwb_mode");
						if (err) error_log(stderr, err);
					}
				}
			}
		}
		break;
	case 'c':
		{
			int delay;
			u = readint();
			if (iseol()) {
				printf("Expected repeat delay after 'c'\n");
				break;
			}
			delay = readint();
			if (iseol()) {
				printf("Expected packet count after 'c'\n");
				break;
			}
			if (ajlist[u]->mf->type == APP_ACTIVE_VMPTX) {
				err = set_csrcs(ajlist[u]->mf->u.vt.vmptx, delay, readint());
				if (err) error_log(stderr, err);
			}
		}
		break;
	case 'C':
		err = set_csrcs_lists();
		if (err) error_log(stderr, err);
		break;
	case 'l':
		{
			int delay, repeat, exclude = 0, ssrc = 0;
			u = readint();
			if (iseol()) {
				printf("Expected repeat delay after 'c'\n");
				break;
			}
			delay = readint();
			if (iseol()) {
				printf("Expected packet count after 'c'\n");
				break;
			}
			repeat = readint();
			if (!iseol()) {
				exclude = readint();
				if (iseol()) {
					printf("Expected SSRC after exclude flag\n");
					break;
				}
				ssrc = readint();
			}
			if (ajlist[u]->mf->type == APP_ACTIVE_VMPTX) {
				tSMVMPTxCSRCListId list;
				err = get_csrc_list(ajlist[u]->mf->u.vt.mod, &list);
				if (!err) err = set_csrcs_to_list(ajlist[u]->mf->u.vt.vmptx, delay, repeat, list, exclude, ssrc);
				if (err) error_log(stderr, err);
			}
		}
		break;
	case 'd':
		u = readint();
		if (ajlist[u]->mf->type == APP_ACTIVE_VMPTX) {
			err = generate_tones(ajlist[u]->mf->u.vt.vmptx);
			if (err) error_log(stderr, err);
		}
		break;
	case 'D':
		u = readint();
		if (ajlist[u]->mf->type == APP_ACTIVE_VMPTX) {
			err = generate_tones_abort(ajlist[u]->mf->u.vt.vmptx);
			if (err) error_log(stderr, err);
		}
		break;
#ifdef PROSODY_TiNG
 	case 'g':	// go
 		err = runsync(ts);
		ts = 0;
 		if (err) return error(err, "runsync() failed");
		break;
#endif
	case 'k':	// kill
		u = readint();
		if (u >= freeac || !ajlist[u]) {
			printf("Channel %d not in use\n", u);
			break;
		}
		if (ajlist[u]->mf->type == APP_ACTIVE_CHANNEL) {
			err = abort_replay(ajlist[u]->mf->u.c.chan);
			if (err) error_log(stderr, err);
		} else if (ajlist[u]->mf->type == APP_ACTIVE_VMPTX) {
			err = stop_vmptx(ajlist[u]->mf->u.vt.vmptx);
			if (err) error_log(stderr, err);
		}
		break;
	case 'm':
		u = readint();
		if (ajlist[u]->mf->type == APP_ACTIVE_VMPTX) {
			char buf[2048];
			int len = 0;
			int ch = sksp();
			while (len < 2048) {
				if (ch == '\n') break;
				buf[len++] = ch;
				ch = getchar();
			}
			printf("sending %s\n", buf);
			err = send_sse(ajlist[u]->mf->u.vt.vmptx, buf, len);
			if (err) error_log(stderr, err);
		}
		break;
	case 'o':	// once
		chanpar.once = readint();
		break;
	case 's':
		chanpar.speed = readint();
		break;
#ifdef PROSODY_TiNG
	case 'T':	// Trace
		TiNGtrace = readint();
		break;
#endif
	case 'u':	// update
		u = readint();
		if (u >= freeac || !ajlist[u]) {
			printf("Channel %d not in use\n", u);
			break;
		}
		err = adj_replay(ajlist[u]->mf->u.c.chan, chanpar);
		if (err) error_log(stderr, err);
		break;
	case 'v':	// volume
		chanpar.volume = readint();
		break;
	case 'x':	// max octets
		chanpar.max_octets = readint();
		break;
	case 'w':
		waiting = 1;
		break;
	case 'q':	// quit
		destroy_tonesets();
		destroy_csrc_lists();
#ifdef USE_POLL
		stopworker(tid, &evset);
#endif
#ifdef TiNGTYPE_WINNT
		for (u=0; u < nextevset; u++) {
			stopworker(tid[u], &evset[u]);
		}
#endif
		return 0;
	case EOF:
		return 0;
	}
 }
}


static const unsigned char b64dectbl[] = {
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  62, 255, 255, 255,  63,
 52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255, 255, 128, 255, 255,
255,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
 15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,
255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
 41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
};

static int base64decode(const unsigned char *in, size_t inlen, void *out, size_t outlen)
{
	unsigned int word;
	size_t pos;
	int pad = 0;
	unsigned char *outp = (unsigned char*)out;
	if (inlen % 4 || (inlen >> 2) * 3 > outlen) return -1; // outlen could be less if padding present!
	for(pos = 0; pos < inlen; ) {
		unsigned char code = b64dectbl[in[pos++]];
		if (code == 255 || code == 128) return -1; // not valid
		word = code << 18;
		code = b64dectbl[in[pos++]];
		if (code == 255 || code == 128) return -1; // not valid
		word |= code << 12;
		code = b64dectbl[in[pos++]];
		if (code == 255) return -1; // not valid
		if (code == 128) { // padding
			if (pos + 1 != inlen || in[pos] != '=') return -1;
			pad = 2;
			code = 0;
		}
		word |= code << 6;
		code = b64dectbl[in[pos++]];
		if (code == 255) return -1; // not valid
		if (code == 128) { // padding
			if (pos != inlen) return -1;
			if (!pad) pad = 1;
			code = 0;
		} else if (pad) {
			return -1; // expected more pad
		}
		word |= code;

		if (pad == 0) {
			*outp++ = (word & 0x00ff0000)>>16;
			*outp++ = (word & 0x0000ff00)>>8;
			*outp++ = (word & 0x000000ff);
		} else if (pad == 1) {
			*outp++ = (word & 0x00ff0000)>>16;
			*outp++ = (word & 0x0000ff00)>>8;
		} else { // pad == 2
			*outp++ = (word & 0x00ff0000)>>16;
		}
	}
	return (outp - (unsigned char*)out);
}

static int base64encode(const void* data_buf, size_t dataLength, char* result, size_t resultSize)
{
   const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
   const unsigned char *data = (const unsigned char *)data_buf;
   size_t resultIndex = 0;
   size_t x;
   unsigned int n = 0;
   size_t blocks = dataLength / 3;
   size_t padCount = dataLength % 3;
   unsigned char n0, n1, n2, n3;

   if (padCount) blocks++;
   if (resultSize < (blocks << 2)) return -1; // result not big enough

   /* increment over the length of the string, three characters at a time */
   for (x = 0; x < dataLength; x += 3) 
   {
      /* these three 8-bit (ASCII) characters become one 24-bit number */
      n = data[x] << 16;
 
      if((x+1) < dataLength)
         n += data[x+1] << 8;
 
      if((x+2) < dataLength)
         n += data[x+2];
 
      /* this 24-bit number gets separated into four 6-bit numbers */
      n0 = (unsigned char)(n >> 18) & 63;
      n1 = (unsigned char)(n >> 12) & 63;
      n2 = (unsigned char)(n >> 6) & 63;
      n3 = (unsigned char)n & 63;
 
      /*
       * if we have one byte available, then its encoding is spread
       * out over two characters
       */
      result[resultIndex++] = base64chars[n0];
      result[resultIndex++] = base64chars[n1];
 
      /*
       * if we have only two bytes available, then their encoding is
       * spread out over three chars
       */
      if((x+1) < dataLength)
      {
         result[resultIndex++] = base64chars[n2];
      }
 
      /*
       * if we have all three bytes available, then their encoding is spread
       * out over four characters
       */
      if((x+2) < dataLength)
      {
         result[resultIndex++] = base64chars[n3];
      }
   }  
 
   /*
    * create and add padding that is required if we did not have a multiple of 3
    * number of characters available
    */
   if (padCount > 0) 
   { 
      for (; padCount < 3; padCount++) 
      { 
         result[resultIndex++] = '=';
      } 
   }
   result[resultIndex] = 0;
   return 0;   /* indicate success */
}

static char* get_key(const char* src_key)
{
	char* ret;
	if (src_key && strlen(src_key) != 40) return NULL;
	ret = malloc(30);
	if (src_key) {
		if (base64decode(src_key, 40, ret, 30) != 30) {
			free(ret);
			return NULL;
		}
		fprintf(stderr, "Using SRTP key:%s\n", src_key);
	} else {
		char b64[41];
		int i;
		// fill with random data
		srand(time(NULL));
		for(i=0;i<30;i++) {
			ret[i] = rand();
		}
		// print value as base64 encoded string for passing to transmitter
		base64encode(ret, 30, b64, 40);
		b64[40] = 0;
		printf("SRTP key:%s\n", b64);
		fflush(stdout);
	}
	return ret;
}


#ifdef TiNGTYPE_WINNT
static int WSAInit()
{
 WSADATA wsaData;
 WORD wVersionRequested = MAKEWORD( 2, 2 );
 int err = WSAStartup( wVersionRequested, &wsaData );
 if (err) fprintf(stderr, "WSAStartup failed: [%d]\n",err);
 return err;
}
#endif

#include "gen/rtpplay.args.i"


static void usage(void)
{
 fprintf(stderr, "Usage: %s" ARGS_USAGE " [file]\nFormats:", progname);
 fmtlist_dump(stderr);
 rtptx_dump_codecs(stderr);
 rtptx_dump_codec_parms(stderr);
}

int main(int argc, char **argv)
{
 CARDLIST cards;
 CHANPAR chanpar;
 VMPPAR vmppar;
 err_t err;
 ARGS_DECL
 (void) argc;

 memset(&vmppar, 0, sizeof(vmppar));
 memset(&chanpar, 0, sizeof(chanpar));
 if (ARGS_CALL || (*argv && argv[1])) {
	usage();
	return 1;
 }
 if (!arg.format) {
 	fprintf(stderr, "-F format option required\n");
	usage();
 	return 1;
 }
 if (!arg.codec) {
 	fprintf(stderr, "-c codec option required\n");
	usage();
 	return 1;
 }
 if (rtptx_init_par(&vmppar.rtptx_par, arg.codec)) {
	usage();
	return 1;
 }
 cardlist(&cards);
 chanpar.curcard = 0;
 chanpar.curmod = 0;
 if (arg.identifier) {
	err = cardlist_addcard(&cards, arg.identifier);
	if (err) {
		error_log(stderr, err);
		return 1;
	}
 }
 if (!cards.ncard) {
	fprintf(stderr, "No cards specifed\n");
	return 1;
 }
 if (fmtcode(&chanpar.format, &chanpar.rate, arg.format)) {
	fprintf(stderr, "Unknown format: '%s'\n", arg.format);
	return 1;
 }
 if (!arg.numchan) arg.numchan = 1;
 chanpar.agc = arg.agc;
 chanpar.delay = arg.randdelay;
 chanpar.once = ONCE_NOT;
 if (arg.once) {
 	chanpar.once = ONCE_ONLY;
	if (arg.restart) {
		fprintf(stderr, "Error: both 'once' and 'restart' requested\n");
		return 1;
	}
 }
 if (arg.restart) chanpar.once = ONCE_REPEAT;
 chanpar.speed = arg.speed;
 chanpar.volume = arg.volume;
 chanpar.cards = &cards;
 chanpar.bgchan = kSMNullChannelId;
 chanpar.fname = *argv;
 chanpar.max_octets = arg.maxoctets;
 chanpar.threshold = arg.threshold;
 chanpar.per_mod = arg.per_mod;
 chanpar.thismod = 0;
 chanpar.out_sample_rate = arg.out_sample_rate;
 vmppar.dest_ip = 0;
 vmppar.src_ip = 0;
 vmppar.source_port = arg.source_port;
 if (arg.pticks == 0) vmppar.rtptx_par.pticks = 80;
 else vmppar.rtptx_par.pticks = arg.pticks;
 if (arg.frame_len == 0) vmppar.rtptx_par.frame_len = 20 * 8;
 else vmppar.rtptx_par.frame_len = arg.frame_len;
 vmppar.rtptx_par.vadmode = arg.vad;
 vmppar.rtptx_par.high_pass_filter = arg.high_pass_filter;
 vmppar.rtptx_par.bitrate = arg.bitrate;
 vmppar.rtptx_par.variant = arg.variant;
 vmppar.rtptx_par.payloadmapping = arg.payloadmapping;
 vmppar.rtptx_par.comfort_noise_payload_mapping = arg.comfort_noise_payload_mapping;
 vmppar.elim_tones = arg.rfc2833tones;
 vmppar.sse_pt = arg.sse_payload_type;
 vmppar.sse_repeats = arg.sse_repeat_count;
 vmppar.sse_spacing = arg.sse_repeat_spacing;
 if (arg.clerk_port && arg.clerk_addr) {
#ifdef TiNGTYPE_WINNT
	WSAInit();
#endif
 	vmppar.clerk_sock = rtpclerk_cli_connect(arg.clerk_addr, arg.clerk_port);
	if (vmppar.clerk_sock < 0) return -1;
	vmppar.use_clerk = 1;
 } else if (!arg.dest_ip) {
	fprintf(stderr, "Error: destination IP or RTP clerk required\n");
	return 1;
 } else {
 		//only use the command line options if we're not using the clerk
	vmppar.dest_ip = strdup(arg.dest_ip);
	vmppar.dest_port = arg.dest_port;
 }
 if (arg.src_ip) {
 	vmppar.src_ip = strdup(arg.src_ip);
 }
 if (arg.use_srtp) {
	vmppar.srtp_key = get_key(arg.srtp_key);
	if (arg.srtp_key && !vmppar.srtp_key) {
		fprintf(stderr, "SRTP key must be 30 octets encoded in 40 base64 characters\n");
		return -1;
	}
 }

 err = rtpplay(chanpar, vmppar, arg.sync, arg.numchan);
 if (err) {
	error_log(stderr, err);
	return 1;
 }
 cardlist_dtor(&cards);
 free(vmppar.srtp_key);
 return 0;
}
