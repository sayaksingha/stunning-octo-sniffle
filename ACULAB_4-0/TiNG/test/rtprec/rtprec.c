/* rtprec.c - demonstration of record over RTP with an eventset */

#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
#include "../Testlib/error.h"
#include "../Testlib/fmtcode.h"
#include "../Testlib/errcode_sm.h"
#include "../Testlib/eventset.h"
#include "../Testlib/taistamp.h"
#include "../Testlib/cardlist.h"
#include "../Testlib/rtpclerk_cli.h"
#include "../Testlib/rtprx_codec.h"
#include "smbesp.h"
#include "smdrvr.h"
#include "smrtp.h"
#include "../../apilib/smsync.h"	// unpublished API

#define arlen(x) (sizeof(x)/sizeof(*(x)))

#define APP_ACTIVE_CHANNEL 0
#define APP_ACTIVE_VMPRX 1

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
			int underrun_flag;
			char id[10];
		} c;
		struct {
			tSMVMPrxId vmprx;
			enum {
				AVSTATE_IDLE,
				AVSTATE_BUSY,
				AVSTATE_DONE,
				AVSTATE_PORTS,
			} avstate;
			unsigned char tone;
			RTPCLERK_SOCKET clerk_sock;
			unsigned use_clerk;
			char local_addr[256];
			int sse_pt;
			int main_pt;
			err_t (*codec_specific_status)(tSMVMPrxId vmprx);
		} vr;
	} u; 
};

	// This conveniently gathers together the configurable
	// parameters for a channel
typedef struct {
	char *fname;
	unsigned curcard;
	unsigned curmod;
	CARDLIST *cards;
	enum kSMDataFormat format;
	tSM_UT32 rate;
	unsigned sil_elim;
	unsigned tone_elim;
	unsigned tone_elim_set;
	unsigned max_octets;
	unsigned max_silence;
	unsigned max_elapsed;
	unsigned thresh;
	int agc;
	int volume;
	int underrun_flag;
	unsigned per_mod;
	unsigned thismod;
} CHANPAR;

typedef struct vmppar {
	RTPCLERK_SOCKET clerk_sock;
	unsigned use_clerk;
	char *local_addr;
	unsigned short source_port;
	RTPRX_PAR rtprx_par;
	unsigned sample_rate;
	unsigned sse_pt;
	int ipv6;
	char* srtp_key;
} VMPPAR;

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

	// the cleanup handler for when the channel gets deleted from 
	// the worker's list of active channels.
	//
	// This is run by the worker either because a work order
	// deleted the channel from the active list or because the
	// channel's event handler finished recording
	//
	// Note that we must not delete the resources since the main
	// thread might be in the middle of adjusting a parameter
static void cleanup_record(ACTIVE_JOB *ajp)
{
 ajp->mf->u.c.acstate = ACSTATE_DONE;
}

static void cleanup_vmprx(ACTIVE_JOB *ajp)
{
 ajp->mf->u.vr.avstate = AVSTATE_DONE;
}

static void free_active(ACTIVE_JOB *ajp)
{
 free(ajp->mf);
 free(ajp);
}

	// free any resources associated with the active channel
static void free_resources(ACTIVE_JOB *ajp)
{
 if(ajp->mf->type == APP_ACTIVE_CHANNEL) {
	smd_ev_free(ajp->event);
	sm_channel_release(ajp->mf->u.c.chan);
	if (ajp->mf->u.c.fp) fclose(ajp->mf->u.c.fp);
 } else if (ajp->mf->type == APP_ACTIVE_VMPRX) {
	sm_vmprx_destroy(ajp->mf->u.vr.vmprx);
 }
 free(ajp);
}

static err_t stop_vmprx(tSMVMPrxId vmprx)
{
 SM_VMPRX_STOP_PARMS vsp;
 int e;
 memset(&vsp, 0, sizeof(vsp));
 vsp.vmprx = vmprx;
 e = sm_vmprx_stop(&vsp);
 if (e) return prosody_error(e, "sm_vmprx_stop() failed");
 return 0;
}

	// abort a record
static err_t abort_record(tSMChannelId chan, int discard)
{
 struct sm_record_abort_parms rp;
 int e;
 memset(&rp, 0, sizeof(rp));
 rp.channel = chan;
 rp.discard = discard;
 e = sm_record_abort(&rp);
 if (e) return prosody_error(e, "sm_record_abort() failed");
 return 0;
}

	// adjust AGC gain in a record
static err_t agc_adj_record(tSMChannelId chan, float gain)
{
 struct sm_record_agc_adjust_parms rp;
 int e;
 memset(&rp, 0, sizeof(rp));
 rp.channel = chan;
 rp.gain = gain;
 e = sm_record_agc_adjust(&rp);
 if (e) return prosody_error(e, "sm_record_agc_adjust() failed");
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

	// we have data on a channel - fetch it
static err_t handle_data(ACTIVE_JOB *ajp)
{
 SM_TS_DATA_PARMS dp;
 char buff[2048];
 int e;
 memset(&dp, 0, sizeof(dp));
 dp.channel = ajp->mf->u.c.chan;
 dp.data = buff;
 e = sm_get_recorded_data(&dp);
 if (e) return prosody_error(e, "sm_get_recorded_data() failed");
 if (ajp->mf->u.c.fp) {
	size_t nc = fwrite(buff, 1, dp.length, ajp->mf->u.c.fp);
	if (ferror(ajp->mf->u.c.fp)) {
		return error(error_errno(errno, "fwrite()failed"),
			"Cannot write recorded data file");
 	}
 	if (nc < (unsigned) dp.length) {
		return error(error(0, "fwrite() performed partial write"),
			"Cannot write recorded data file");
 	}
 }
 if (ajp->mf->u.c.acstate == ACSTATE_IDLE) ajp->mf->u.c.acstate = ACSTATE_BUSY;
 return 0;
}

#define FORMAT_IN_ADDR "%d.%d.%d.%d"
#define ARGS_IN_ADDR(x) \
	((ntohl((x).s_addr) >> 24) & 0xff), \
	((ntohl((x).s_addr) >> 16) & 0xff), \
	((ntohl((x).s_addr) >> 8) & 0xff), \
	(ntohl((x).s_addr) & 0xff)

#define FORMAT_IN6_ADDR "%x:%x:%x:%x:%x:%x:%x:%x"
#define ARGS_IN6_ADDR(x) \
	((((x).s6_addr[0])<<8)+((x).s6_addr[1])), \
	((((x).s6_addr[2])<<8)+((x).s6_addr[3])), \
	((((x).s6_addr[4])<<8)+((x).s6_addr[5])), \
	((((x).s6_addr[6])<<8)+((x).s6_addr[7])), \
	((((x).s6_addr[8])<<8)+((x).s6_addr[9])), \
	((((x).s6_addr[10])<<8)+((x).s6_addr[11])), \
	((((x).s6_addr[12])<<8)+((x).s6_addr[13])), \
	((((x).s6_addr[14])<<8)+((x).s6_addr[15]))

static int handle_vmprx(ACTIVE_JOB *ajp)
{
	SM_VMPRX_STATUS_PARMS statusp;
	SM_VMPRX_PORT_PARMS portp;
	int e;
	err_t err = 0;
	memset(&statusp,0,sizeof(statusp));
	statusp.vmprx = ajp->mf->u.vr.vmprx;
	e = sm_vmprx_status(&statusp);
	if (e) err = prosody_error(e, "sm_vmprx_status() failed");
	else if (statusp.status == kSMVMPrxStatusStopped) {
		printf("Status: STOPPED\n");
		return 1;
	} else if (statusp.status == kSMVMPrxStatusGotPorts) {
		memset(&portp, 0, sizeof(portp));
		portp.vmprx = ajp->mf->u.vr.vmprx;
		portp.nowait = 1;
		e = sm_vmprx_get_ports(&portp);
		if (e) err =  prosody_error(e, "sm_vmprx_get_ports() failed");
		ajp->mf->u.vr.avstate = AVSTATE_PORTS;
		if (ajp->mf->u.vr.use_clerk) {
			int rc = rtpclerk_cli_post_port(ajp->mf->u.vr.clerk_sock, ajp->mf->u.vr.local_addr, portp.RTP_port);
			if (rc) {
				stop_vmprx(ajp->mf->u.vr.vmprx);
				return 0;
			}
		} else {
			printf("RTP port = " FORMAT_IN_ADDR ":%u, RTCP port = %u\n", ARGS_IN_ADDR(portp.address), portp.RTP_port, portp.RTCP_port);
			fflush(stdout);
		}
	} else if (statusp.status == kSMVMPrxStatusGotPortsIPv6) {
		memset(&portp, 0, sizeof(portp));
		portp.vmprx = ajp->mf->u.vr.vmprx;
		portp.nowait = 1;
		e = sm_vmprx_get_ports(&portp);
		if (e) err =  prosody_error(e, "sm_vmprx_get_ports() failed");
		ajp->mf->u.vr.avstate = AVSTATE_PORTS;
		if (ajp->mf->u.vr.use_clerk) {
			int rc = rtpclerk_cli_post_port(ajp->mf->u.vr.clerk_sock, ajp->mf->u.vr.local_addr, portp.RTP_port);
			if (rc) {
				stop_vmprx(ajp->mf->u.vr.vmprx);
				return 0;
			}
		} else {
			printf("RTP port = [" FORMAT_IN6_ADDR "]:%u, RTCP port = %u\n", ARGS_IN6_ADDR(portp.ipv6_address), portp.RTP_port, portp.RTCP_port);
		}
	} else if(statusp.status == kSMVMPrxStatusDetectTone) {
		printf("Tone - %d %f\n",statusp.u.tone.id, statusp.u.tone.volume);
	} else if(statusp.status == kSMVMPrxStatusEndTone) {
		printf("Tone end - %d %f %d\n",statusp.u.tone.id, statusp.u.tone.volume, statusp.u.tone.duration);
	} else if(statusp.status == kSMVMPrxStatusNewSSRC) {
		printf("SSRC - %d from " FORMAT_IN_ADDR ":%u\n",statusp.u.ssrc.ssrc, ARGS_IN_ADDR(statusp.u.ssrc.address), statusp.u.ssrc.port);
	} else if(statusp.status == kSMVMPrxStatusNewSSRCIPv6) {
		printf("SSRC - %d from [" FORMAT_IN6_ADDR "]:%u\n",statusp.u.ssrc_ipv6.ssrc, ARGS_IN6_ADDR(statusp.u.ssrc_ipv6.address), statusp.u.ssrc_ipv6.port);
	} else if(statusp.status == kSMVMPrxStatusUnhandledPayload) {
		printf("Unhandled payload type - %d\n",statusp.u.payload.type);
	} else if (statusp.status == kSMVMPrxStatusCodecSpecific) {
		if (statusp.u.codec_specific.payload_type == ajp->mf->u.vr.sse_pt) {
			SM_VMPRX_STATUS_CODEC_SSE_PARMS ssep;
			char buf[1024];
			memset(&ssep, 0, sizeof(ssep));
			ssep.vmprx = ajp->mf->u.vr.vmprx;
			ssep.payload_type = ajp->mf->u.vr.sse_pt;
			ssep.payload = buf;
			ssep.max_length = 1023; // one to add zero terminator;
			e = sm_vmprx_status_codec_sse(&ssep);
			if (e) err = prosody_error(e, "sm_vmprx_status_codec_sse() failed");
			else {
				buf[ssep.length] = 0;
				printf("SSE payload \"%s\"\n",buf);
			}
		} else if (statusp.u.codec_specific.payload_type == ajp->mf->u.vr.main_pt) {
			if (ajp->mf->u.vr.codec_specific_status) {
				ajp->mf->u.vr.codec_specific_status(ajp->mf->u.vr.vmprx);
			}
		} else {
			printf("Codec specific payload type - %d\n",statusp.u.codec_specific.payload_type);
			e = sm_vmprx_status_discard_codec_specific(ajp->mf->u.vr.vmprx);
			if (e) err = prosody_error(e, "sm_vmprx_status_discard_codec_specific() failed");
		}
	} else if(statusp.status == kSMVMPrxStatusJBResync) {
		time_t t = time(NULL);
		printf("Jitter buffer resync - %lu (%s)\n",statusp.u.jbresync.timestamp, ctime(&t) );
	} else {
		ajp->mf->u.vr.avstate = AVSTATE_BUSY;
	}
	if (err) {
		error_log(stderr, error(err, "handle_record() failed"));
		stop_vmprx(ajp->mf->u.vr.vmprx);
		return 1;
	}
	return 0;	
}

	// the handler for when a channel's event is signalled
	// note that this is run by the worker thread
static int handle_record(ACTIVE_JOB *ajp)
{
 struct sm_record_status_parms rp;
 err_t err = 0;
 int e;
 memset(&rp, 0, sizeof(rp));
 rp.channel = ajp->mf->u.c.chan;
 e = sm_record_status(&rp);
 if (e) {
	err = prosody_error(e, "sm_record_status() failed");
 } else switch (rp.status) {
 case kSMRecordStatusOverrun:
	if (ajp->mf->u.c.underrun_flag) {
		char now[32];
		taistamp(now);
		printf("%s %s underrun\n", now, ajp->mf->u.c.id);
	}
	ajp->mf->u.c.acstate = ACSTATE_UNDERRUN;
	return 0;
 case kSMRecordStatusComplete:
	if (rp.termination_reason == kSMRecordHowTerminatedError) {
		fprintf(stderr, "Record terminated by error\n");
	}
	return 1;
 case kSMRecordStatusCompleteData:
 case kSMRecordStatusData:
	err = handle_data(ajp);
		/* optimisation: no need to re-check status
		 *
		 * Prosody version 2 (TiNG) guarantees that the
		 * event triggered by available data remains
		 * set until the data is read. This means that
		 * when we read some data, we do not need to
		 * worry about whether we have read all the
		 * available data. Therefore we return. If there
		 * is more data available, we will simply find the
		 * event still set and get back here to handle it.
		 *
		 * In version 1, however, the event is only set by
		 * data *becoming* available, if we do not read
		 * all of the data we will never get another
		 * event and the record will stall.
		 */
	if (!err) return 0;	// only Prosody version 2 (TiNG)
	break;
 case kSMRecordStatusNoData:
	err = checksync(ajp);
	if (err) {
		err = error(err, "checksync() failed");
		break;
	}
	return 0;
 case kSMRecordStatusRecognition:
	printf("recognised: type=%d, p0=%d, p1=%d\n",
		rp.recog_type, rp.param0, rp.param1);
	return 0;
 }
	// some sort of error - abort and report
 error_log(stderr, error(err, "handle_record() failed"));
 abort_record(ajp->mf->u.c.chan, 1);
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

	// open a file for writing
static err_t open_file(ACTIVE_JOB *ajp, char *fmain, int st, int ts)
{
 if (!fmain) {
	ajp->mf->u.c.fp = 0;
 } else {
	char *fname = malloc(strlen(fmain) + sizeof("0123456789")*2);
	err_t err = 0;
	if (!fname) return error_errno(errno, "malloc() failed");
	sprintf(fname, fmain, st, ts);
	ajp->mf->u.c.fp = fopen(fname, "wb");
	if (!ajp->mf->u.c.fp) {
		char *n = malloc(strlen(fname) + sizeof("File: ''"));
		if (n) sprintf(n, "File: '%s'", fname);
		err = error_errno(errno, "fopen() failed");
		err = error(err, n);
		err = error(err, "Cannot open file");
		free(n);
	}
	free(fname);
	return err;
 }
 return 0;
}

static err_t create_vmprx(ACTIVE_JOB *ajp, tSMModuleId module, int ipv6)
{
 SM_VMPRX_CREATE_PARMS vcp;
 int e;
 memset(&vcp, 0, sizeof(vcp));
 vcp.module = module;
 if (ipv6) vcp.type = kSMVMPrxTypeIPv6;
 e = sm_vmprx_create(&vcp);
 if (e) return prosody_error(e, "sm_vmprx_create() failed");
 ajp->mf->u.vr.vmprx = vcp.vmprx;
 ajp->mf->type = APP_ACTIVE_VMPRX;
 return 0;
}

	// allocate a Prosody channel
static err_t alloc_chan(ACTIVE_JOB *ajp, tSMModuleId module)
{
 SM_CHANNEL_ALLOC_PLACED_PARMS ap;
 int e;
 memset(&ap, 0, sizeof(ap));
 ap.type = kSMChannelTypeInput;
 ap.module = module;
 e = sm_channel_alloc_placed(&ap);
 if (e) return prosody_error(e, "sm_channel_alloc_placed() failed");
 ajp->mf->u.c.chan = ap.channel;
 ajp->mf->type = APP_ACTIVE_CHANNEL;
 return 0;
}

	// set the desired data transfer threshold
static err_t set_threshold(ACTIVE_JOB *ajp, unsigned thresh)
{
 if (thresh) {		// value zero leaves default setting
	SM_CHANNEL_SET_INPUT_THRESHOLD_PARMS tp;
	int e;
	memset(&tp, 0, sizeof(tp));
	tp.channel = ajp->mf->u.c.chan;
	tp.minimum_bits = thresh * 8;
	e = sm_channel_set_input_threshold(&tp);
	if (e) return prosody_error(e, "sm_channel_set_input_threshold() failed");
 }
 return 0;
}

static err_t get_datafeed(ACTIVE_JOB *ajp, tSMDatafeedId *df)
{
 SM_VMPRX_DATAFEED_PARMS dfp;
 int e;
 memset(&dfp, 0, sizeof(dfp));
 dfp.vmprx = ajp->mf->u.vr.vmprx;
 e = sm_vmprx_get_datafeed(&dfp);
 if (e) return prosody_error(e, "sm_vmprx_get_datafeed() failed");
 *df = dfp.datafeed;
 return 0;
}

static err_t config_sample_rate(ACTIVE_JOB *ajp, unsigned sample_rate)
{
 if (sample_rate) {
 	SM_VMPRX_SAMPLE_RATE_PARMS srp;
 	int e;
 	memset(&srp, 0, sizeof(srp));
 	srp.vmprx = ajp->mf->u.vr.vmprx;
 	srp.sample_rate = sample_rate;
 	e = sm_vmprx_config_sample_rate(&srp);
	if (e) return prosody_error(e, "sm_vmprx_config_sample_rate() failed");
 }
 return 0;
}

static err_t connect_datafeed(ACTIVE_JOB *ajp, tSMDatafeedId df)
{
 SM_CHANNEL_DATAFEED_CONNECT_PARMS dfp;
 int e;
 memset(&dfp, 0, sizeof(dfp));
 dfp.channel = ajp->mf->u.c.chan;
 dfp.data_source = df;
 e = sm_channel_datafeed_connect(&dfp);
 if (e) return prosody_error(e, "sm_channel_datafeed_connect() failed");
 return 0;
}

static err_t get_event(ACTIVE_JOB *ajp)
{
 SM_VMPRX_EVENT_PARMS evp;
 int e;
 memset(&evp, 0, sizeof(evp));
 evp.vmprx = ajp->mf->u.vr.vmprx;
 e = sm_vmprx_get_event(&evp);
 if (e) return prosody_error(e, "sm_vmprx_get_event() failed");
 ajp->event = evp.event;
 return 0;
}

	// allocate an event
static err_t alloc_event(ACTIVE_JOB *ajp)
{
 int e = smd_ev_create(&ajp->event, ajp->mf->u.c.chan,
	kSMEventTypeReadData, kSMChannelSpecificEvent);
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
 sep.event_type = kSMEventTypeReadData;
 sep.issue_events = kSMChannelSpecificEvent;
 e = sm_channel_set_event(&sep);
 if (e) return prosody_error(e, "sm_channel_set_event() failed");
 return 0;
}

	// start recording
static err_t start_record(ACTIVE_JOB *ajp, CHANPAR chanpar)
{
 struct sm_record_parms rp;
 int e;
 memset(&rp, 0, sizeof(rp));
 rp.channel = ajp->mf->u.c.chan;
 rp.volume = chanpar.volume;
 rp.agc = chanpar.agc;
 rp.type = chanpar.format;
 rp.sampling_rate = chanpar.rate;
 rp.silence_elimination = chanpar.sil_elim;
 rp.tone_elimination_mode = chanpar.tone_elim;
 rp.tone_elimination_set_id = chanpar.tone_elim_set;
 rp.max_silence = chanpar.max_silence;
 rp.max_elapsed_time = chanpar.max_elapsed;
 rp.max_octets = chanpar.max_octets;
 e = sm_record_start(&rp);
 if (e) return prosody_error(e, "sm_record_start() failed");
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

static err_t vmprx_config_sse(ACTIVE_JOB *ajp_v, unsigned pt)
{
 if (pt & 0x7f) {
 	SM_VMPRX_CODEC_SSE_PARMS srp;
 	int e;
 	memset(&srp, 0, sizeof(srp));
 	srp.vmprx = ajp_v->mf->u.vr.vmprx;
 	srp.payload_type = pt & 0x7f;
 	e = sm_vmprx_config_codec_sse(&srp);
	if (e) return prosody_error(e, "sm_vmprx_config_codec_sse() failed");
	ajp_v->mf->u.vr.sse_pt = pt & 0x7f;
 }
 return 0;
}
static err_t config_payload_rep(tSMVMPrxId vmprx, int delay);
static err_t config_vmprx(tSMVMPrxId vmprx, RTPRX_PAR* rtprx_par, char* srtp_key)
{
 err_t err = rtprx_config_vmprx(vmprx, rtprx_par);
 if (err) return err;
 if (srtp_key) {
	int e;
	SM_VMPRX_CONFIG_ENCRYPTION_AES_CM_PARMS ep;
	SM_VMPRX_CONFIG_AUTHENTICATION_HMAC_SHA1_PARMS ap;
	memset(&ap, 0, sizeof(ap));
	ap.vmprx = vmprx;
	ap.key = srtp_key;
	ap.keylen = 30;
	ap.taglen = 80;
	e = sm_vmprx_config_authentication_hmac_sha1(&ap);
	if (e) return prosody_error(e, "sm_vmprx_config_authentication_hmac_sha1() failed");

	memset(&ep, 0, sizeof(ep));
	ep.vmprx = vmprx;
	ep.key = srtp_key;
	ep.keylen = 30;
	e = sm_vmprx_config_encryption_aes_cm(&ep);
	if (e) return prosody_error(e, "sm_vmprx_config_encryption_aes_cm() failed");

	config_payload_rep(vmprx, 100);
 }
 return 0;
}

	// start a new record
static err_t recrtp(ACTIVE_JOB **ajpp, EVENTSET *evs, CHANPAR chanpar, VMPPAR vmppar,  struct threadsync *ts)
{
 ACTIVE_JOB *ajp_c, *ajp_v;
 tSMDatafeedId datafeed = kSMNullDatafeedId;
 err_t err = alloc_active(&ajp_c);
 if (err) return error(err, "Cannot read file"); 
 err = alloc_active(&ajp_v);
 if (!err) {
	printf("starting record\n");
	ajpp[0] = ajp_c;
	ajpp[1] = ajp_v;
	ajp_c->mf->u.c.acstate = ACSTATE_IDLE;
	ajp_v->mf->u.vr.avstate = AVSTATE_IDLE;
	ajp_v->mf->u.vr.use_clerk = vmppar.use_clerk;
	ajp_v->mf->u.vr.clerk_sock = vmppar.clerk_sock;
	strcpy(ajp_v->mf->u.vr.local_addr, vmppar.local_addr);
	ajp_c->mf->u.c.sync = ts;
	ajp_c->mf->u.c.underrun_flag = chanpar.underrun_flag;
	ajp_c->handler = handle_record;
	ajp_v->handler = handle_vmprx;
	ajp_c->cleanup_fn = cleanup_record;
	ajp_v->cleanup_fn = cleanup_vmprx;
	ajp_v->mf->u.vr.codec_specific_status = vmppar.rtprx_par.codec_specific_status;
	ajp_v->mf->u.vr.main_pt = vmppar.rtprx_par.payloadmapping;
	ajp_v->mf->u.vr.sse_pt = -1;
	err = open_file(ajp_c, chanpar.fname, vmppar.source_port, 0);
	if (!err) {
		MODDESC mod;
		err = cardlist_getmod(&mod, chanpar.cards, chanpar.curcard, chanpar.curmod);
		if (!err) err = alloc_chan(ajp_c, mod.id);
		if (!err) {
			err = create_vmprx(ajp_v, mod.id, vmppar.ipv6);
			if (!err) {
				err = get_datafeed(ajp_v, &datafeed);
				if (!err) err = connect_datafeed(ajp_c, datafeed);
				if (!err) err = set_threshold(ajp_c, chanpar.thresh);
				if (!err) err = alloc_event(ajp_c);
				if (!err) {
					err = associate_event(ajp_c);
					if (!err) err = start_record(ajp_c, chanpar);
					if (!err) err = eventset_insert(evs, ajp_c);
					if (!err) err = config_sample_rate(ajp_v, vmppar.sample_rate);
					if (!err) err = config_vmprx(ajp_v->mf->u.vr.vmprx, &vmppar.rtprx_par, vmppar.srtp_key);
					if (!err) err = vmprx_config_sse(ajp_v, vmppar.sse_pt);
					if (!err) err = get_event(ajp_v);
					if (!err) err = eventset_insert(evs, ajp_v);
					if (!err) return 0;
					smd_ev_free(ajp_c->event);
				}
					// shouldn't we free the VMP here?
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
 return error(err, "Cannot record file");
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
 int isneg = 0, ndig=0;
 int v = 0;
 for (;;) {
	int c = getchar();
	if (c >= '0' && c <= '9') {
		v = v * 10 - c + '0';
		ndig++;
	} else if (c == '-' && !ndig) {
		isneg = !isneg;
	} else if (c == '+' && !ndig) {
		// unary +
	} else {
		ungetc(c, stdin);
		return isneg ? v : -v;
	}
 }
}

	// read a float from stdin
static float readfloat(void)
{
 int isneg = 0, ndig=0;
 float v = 0;
 int c;
 for (;;) {
	c = getchar();
	if (c >= '0' && c <= '9') {
		v = v * 10 + c - '0';
		ndig++;
	} else if (c == '-' && !ndig) {
		isneg = !isneg;
	} else if (c == '.') {
		float p = 0.1;
		for (;;) {
			c = getchar();
			if (c >= '0' && c <= '9') {
				v += (c - '0') * p;
				p *= 0.1;
			} else if (c == 'e') {
				v *= pow(10.0, readint());
				c = getchar();
				break;
			} else break;
		}
		break;
	} else break;
 }
 ungetc(c, stdin);
 return isneg ? -v : v;
}

	// allow synchronised records to start now
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

static err_t config_payload_rep(tSMVMPrxId vmprx, int delay)
{
 struct sm_vmprx_unhandled_payload_reporting_parms upp;
 int ee;
 memset(&upp, 0, sizeof(upp));
 upp.vmprx = vmprx;
 upp.delay = delay;
 ee = sm_vmprx_config_unhandled_payload_reporting(&upp);
 if (ee) return prosody_error(ee, "sm_vmprx_config_unhandled_payload_reporting() failed");
 return 0;
}


static err_t config_forward(tSMVMPrxId vmprx)
{
 struct sm_vmprx_config_forwarding_parms pp;
 int ee;
 struct addrinfo hints;
 struct addrinfo *result;
 int s;
 int dport, sport;
 char addr_buf[256];
 memset(&pp, 0, sizeof(pp));
 pp.vmprx = vmprx;
 if (iseol()) {
	 printf("Missing mode\n");
	 return 0;
 }
 pp.mode = readint();
 if (pp.mode != 0) {
	if (iseol()) {
		printf("Missing dest\n");
		return 0;
	}
	ee = 0;
	for(;;) {
		char c = getchar();
		if (c == ' ' || c == '\t') {
			break;
		}
		if (ee < 255) {
			addr_buf[ee++] = c;
		}
		if (c == '\n') {
			printf("Missing dest port\n");
			return 0;
		}
	}
	addr_buf[ee] = 0;

	
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;	/* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_flags = AI_NUMERICHOST;

	s = getaddrinfo(addr_buf, NULL, &hints, &result);
	if (s != 0) {
		err_t err = error(error(NULL, gai_strerror(s)), "getaddrinfo failed");
		return err;
	}

	if (iseol()) {
		printf("Missing dest port\n");
		return 0;
	}
	dport = readint();
	if (iseol()) {
		printf("Missing src port\n");
		return 0;
	}
	sport = readint();

	if (result->ai_family == AF_INET) {
		pp.dest_type = kSMVMPrxFwdDestTypeIPv4;
		pp.u.ipv4.destination.sin_addr.s_addr = ((struct sockaddr_in*)result->ai_addr)->sin_addr.s_addr;
		pp.u.ipv4.destination.sin_port = htons(dport);
		pp.u.ipv4.source.sin_port = htons(sport);
	} else {
		pp.dest_type = kSMVMPrxFwdDestTypeIPv6;
		pp.u.ipv6.destination.sin6_addr = ((struct sockaddr_in6*)result->ai_addr)->sin6_addr;
		pp.u.ipv6.destination.sin6_port = htons(dport);
		pp.u.ipv6.source.sin6_port = htons(sport);
	}
	freeaddrinfo(result);

	ee = 0;
	if (!iseol()) for(;;) {
		pp.types[ee].incoming_pt = readint();
		if (iseol()) {
			printf("Missing outgoing pt\n");
			return 0;
		}
		pp.types[ee].outgoing_pt = readint();
		if (iseol()) {
			printf("Missing transcoding\n");
			return 0;
		}
		pp.types[ee].transcoding = readint();
		ee++;
		if (iseol()) {
			pp.num_types = ee;
			break;
		}
	}
 }
 ee = sm_vmprx_config_forwarding(&pp);
 if (ee) return prosody_error(ee, "sm_vmprx_config_forwarding() failed");
 return 0;
}

/*
 * The main record loop. Since we want to display channel status,
 * the cleanup function above cannot free the resources (in case
 * we were accessing the channel in this thread while it was freed in the
 * worker thread). Therefore we need to check for finished channels and
 * clear up. There is no real penalty in deferring this cleanup since
 * if we are sitting waiting for a command there's nothing useful we
 * could do with the newly freed resources.
 */
static err_t rtprec(CHANPAR chanpar, VMPPAR vmppar, int use_sync, unsigned numchan)
{
 ACTIVE_JOB *ajlist[4096];
 struct threadsync *ts = 0;
 unsigned freeac = 0;		// free entry after all used ones in ajlist
 int waiting = 0;
#ifdef USE_POLL
 EVENTSET evset;
 pthread_t tid;
#endif
#ifdef TiNGTYPE_WINNT
 EVENTSET evset[(arlen(ajlist) + 59)/60];
 unsigned nextevset = 0;
#endif
 unsigned u;
 err_t err;
 int e;
#ifdef USE_POLL
 err = startworker(&tid, &evset);
 if (err) return err;
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
	int c = '\n';
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
		} else if (ajlist[u]->mf->type == APP_ACTIVE_VMPRX) {
			switch(ajlist[u]->mf->u.vr.avstate) {
			case AVSTATE_IDLE: putchar('v'); break;
			case AVSTATE_BUSY: putchar('V'); break;
					   break;
			case AVSTATE_PORTS:
				putchar('P');
				ajlist[u]->mf->u.vr.avstate = AVSTATE_BUSY;
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
#ifdef TiNGTYPE_LINUX
			sleep(1);
#endif
#ifdef TiNGTYPE_QNX
			sleep(1);
#endif
#ifdef TiNGTYPE_WINNT
			Sleep(1000);
#endif
		}
		c = '\n';
	} else c = getchar();
	switch (c) {
		float f;
#ifdef USE_POLL
#define evsetp (&evset)
#endif
#ifdef TiNGTYPE_WINNT
		EVENTSET *evsetp;
		unsigned evsetno;
#endif
	case '\n': break;
	case '+':	// start new channel
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
			pthread_t tid;
			err = startworker(&tid, evsetp);
			nextevset++;
		}
#endif
		err = recrtp(&ajlist[u], evsetp, chanpar, vmppar, ts);
		if (err) error_log(stderr, err);
		else {
			if (chanpar.per_mod && ++chanpar.thismod == chanpar.per_mod) {
				cardlist_nextmod(chanpar.cards, &chanpar.curcard, &chanpar.curmod);
				chanpar.thismod = 0;
			}
		}
		vmppar.source_port += 2;
		freeac++;
		break;
	case '?':	// help
		printf("Commands:\n"
			"\t+\tstart record with current agc, and volume\n"
			"\t#...\tcomment\n"
			"\tTN\tset TiNGtrace to N\n"
			"\taN\tset agc (0=off, 1=on)\n"
			"\tfN m ip dprt sprt [inpn outpn trans]*\tpayload forwarding\n"
			"\tGN=V\tset agc gain to V for channel N\n"
			"\tg\tgo - start synced channels\n"
			"\tkN\tkill channel N\n"
			"\tKN\tkill channel N, discarding uncollected data\n"
			"\tsN\tset max silence\n"
			"\tvN\tset volume\n"
			"\txN\tset max octets\n"
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
	case 'a':	// agc
		chanpar.agc = readint();
		break;
	case 'f':	// forward payload
		u = readint();
		if (u >= freeac || !ajlist[u] || ajlist[u]->mf->type != APP_ACTIVE_VMPRX) {
			printf("VMP %d not in use\n", u);
			break;
		}
		err = config_forward(ajlist[u]->mf->u.vr.vmprx);
 		if (err) return error(err, "config_forward() failed");
		break;
 	case 'g':	// go
 		err = runsync(ts);
		ts = 0;
 		if (err) return error(err, "runsync() failed");
		break;
	case 'G':	// AGC gain
		u = readint();
		if (u >= freeac || !ajlist[u]) {
			printf("Channel %d not in use\n", u);
			break;
		}
		c = getchar();
		if (c != '=') {
			f = 0;
			ungetc(c, stdin);
		} else {
			f = readfloat();
		}
		err = agc_adj_record(ajlist[u]->mf->u.c.chan, f);
		if (err) error_log(stderr, err);
		break;
	case 'k':	// kill
	case 'K':	//   with discard
		u = readint();
		if (u >= freeac || !ajlist[u]) {
			printf("Channel %d not in use\n", u);
			break;
		}
		if(ajlist[u]->mf->type == APP_ACTIVE_CHANNEL) {
			err = abort_record(ajlist[u]->mf->u.c.chan, c == 'K');
		} else if (ajlist[u]->mf->type == APP_ACTIVE_VMPRX) {
			err = stop_vmprx(ajlist[u]->mf->u.vr.vmprx);
		}
		if (err) error_log(stderr, err);
		break;
	case 'l':	// max elapsed time
		chanpar.max_elapsed = readint();
		break;
	case 's':	// max silence
		chanpar.max_silence = readint();
		break;
	case 'T':	// Trace
		TiNGtrace = readint();
		break;
	case 'u':	// unhandled payload
		u = readint();
		if (u >= freeac || !ajlist[u] || ajlist[u]->mf->type != APP_ACTIVE_VMPRX) {
			printf("VMP %d not in use\n", u);
			break;
		}
		if (!iseol()) {
			err = config_payload_rep(ajlist[u]->mf->u.vr.vmprx, readint());
		} else {
			printf("Expected delay value\n");
			break;
		}
		if (err) error_log(stderr, err);
		break;
	case 'v':	// volume
		chanpar.volume = readint();
		break;
	case 'w':
		waiting = 1;
		break;
	case 'x':	// max octets
		chanpar.max_octets = readint();
		break;
	case 'q':	// quit
	case EOF:
		return 0;
	}
 }
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
	}
	return ret;
}


#include "gen/rtprec.args.i"

static void usage(void)
{
 fprintf(stderr, "Usage: %s" ARGS_USAGE " [file]\nFormats:", progname);
 fmtlist_dump(stderr);
 rtprx_dump_codecs(stderr);
 rtprx_dump_codec_parms(stderr);
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
 	return 1;
 }
 if (!arg.codec) {
 	fprintf(stderr, "-c codec option required\n");
 	return 1;
 }
 if (rtprx_init_par(&vmppar.rtprx_par, arg.codec)) {
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
 chanpar.sil_elim = arg.sil_elim;
 chanpar.tone_elim = arg.tone_elim_mode;
 chanpar.tone_elim_set = arg.tone_set;
 chanpar.agc = arg.agc;
 chanpar.volume = arg.volume;
 chanpar.cards = &cards;
 chanpar.fname = *argv;
 chanpar.max_silence = arg.max_silence;
 chanpar.max_elapsed = arg.max_elapsed_time;
 chanpar.max_octets = arg.maxoctets;
 chanpar.thresh = arg.threshold;
 chanpar.underrun_flag = arg.underrun_flag;
 chanpar.per_mod = arg.per_mod;
 chanpar.thismod = 0;
 vmppar.rtprx_par.regen_tones = arg.regen_tones;
 vmppar.rtprx_par.enforce_tone_spacing = arg.enforce_tone_spacing;
 vmppar.rtprx_par.frame_len = arg.frame_len;
 vmppar.rtprx_par.detect_tones = arg.detect_tones;
 vmppar.rtprx_par.tone_payload_mapping = arg.rfc2833tones;
 vmppar.rtprx_par.init_ms = arg.initial_ms;
 vmppar.rtprx_par.max_ms = arg.max_ms;
 vmppar.rtprx_par.ad_target_delay = arg.ad_target_delay;
 vmppar.rtprx_par.ad_freq_upper_tolerance = arg.ad_freq_upper_tolerance;
 vmppar.rtprx_par.ad_freq_lower_tolerance = arg.ad_freq_lower_tolerance;
 vmppar.rtprx_par.payloadmapping = arg.payloadmapping;
 vmppar.rtprx_par.plc = arg.plc;
 vmppar.rtprx_par.bitrate = arg.bitrate;
 vmppar.rtprx_par.variant = arg.variant;
 vmppar.rtprx_par.handle_cn = !!arg.comfort_noise;
 vmppar.rtprx_par.report_jb_resync = !!arg.report_jb_resync;
 vmppar.rtprx_par.comfort_noise_payload_mapping = arg.comfort_noise_payload_mapping;
 vmppar.local_addr = malloc(256);
 vmppar.sample_rate = arg.sample_rate;
 vmppar.sse_pt = arg.sse_payload_type;
 vmppar.ipv6 = arg.vmp_is_ipv6;
 if (!vmppar.local_addr) {
 	fprintf(stderr, "failed to malloc local ip address\n");
	return -1;
 }
 if (arg.clerk_port && arg.clerk_addr) {
	if (arg.local_ip) {
		strcpy(vmppar.local_addr, arg.local_ip);
	} else {
		fprintf(stderr, "Local IP address is required when using rtpclerk\n");
		fprintf(stderr, "Usage: %s" ARGS_USAGE " [file]\nFormats:", progname);
		fmtlist_dump(stderr);
		return 1;
	}
#ifdef TiNGTYPE_WINNT
 	WSAInit();
#endif
 	vmppar.clerk_sock = rtpclerk_cli_connect(arg.clerk_addr, arg.clerk_port);
	if (vmppar.clerk_sock < 0) {
		fprintf(stderr, "Unable to connect to rtpclerk\n");	
		return -1;
	}
	vmppar.use_clerk = 1;
 } else {
 	vmppar.use_clerk = 0;
 }
 if (arg.use_srtp) {
	vmppar.srtp_key = get_key(arg.srtp_key);
	if (arg.srtp_key && !vmppar.srtp_key) {
		fprintf(stderr, "SRTP key must be 30 octets encoded in 40 base64 characters\n");
		return -1;
	}
 }

 err = rtprec(chanpar, vmppar, arg.sync, arg.numchan);
 if (err) {
	error_log(stderr, err);
 }
 return err ? 1 : 0;
}
