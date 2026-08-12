/* rtp2tdm.c - demonstration of RTP to TDM gateway */
#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef TiNGTYPE_LINUX
#include <poll.h>
#include <unistd.h>
#define USE_POLL
#endif

#ifdef TiNGTYPE_QNX
#include <sys/poll.h>
#include <unistd.h>
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

#define APP_ACTIVE_VMPRX 1

struct appactive {
	tSMVMPrxId vmprx;
	tSMTDMtxId tdmtx;
	enum {
		AVSTATE_IDLE,
		AVSTATE_BUSY,
		AVSTATE_DONE,
	} avstate;
	RTPCLERK_SOCKET clerk_sock;
	unsigned use_clerk;
	char local_addr[256];
};

	// we need this because the automatic argument parser
	// refers to a type 'MVIP' when it produces code to
	// handle timeslots
typedef struct {
	int stream;
	int timeslot;
	int type;
} MVIP;
#define MVIP_STREAM_NONE 0xff

	// This conveniently gathers together the configurable
	// parameters for a channel
typedef struct {
	RTPCLERK_SOCKET clerk_sock;
	unsigned use_clerk;
	char local_addr[256];
	unsigned curcard;
	unsigned curmod;
	CARDLIST *cards;
	unsigned per_mod;
	unsigned thismod;
	unsigned short source_port;
	unsigned maxts;
	unsigned use_ipv6;
		//for tdm
	MVIP timeslot;
	RTPRX_PAR rtprx_par;
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
	// channel's event handler finished its job.
	//
	// Note that we must not delete the resources since the main
	// thread might be in the middle of adjusting a parameter
static void cleanup_job(ACTIVE_JOB *ajp)
{
 ajp->mf->avstate = AVSTATE_DONE;
}

static void free_active(ACTIVE_JOB *ajp)
{
 free(ajp->mf);
 free(ajp);
}

	// free any resources associated with the active channel
static void free_resources(ACTIVE_JOB *ajp)
{
 sm_vmprx_destroy(ajp->mf->vmprx);
 sm_tdmtx_destroy(ajp->mf->tdmtx);
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
	statusp.vmprx = ajp->mf->vmprx;
	e = sm_vmprx_status(&statusp);
	if (e) err = prosody_error(e, "sm_vmprx_status() failed");
	else if (statusp.status == kSMVMPrxStatusStopped) {
		printf("Status: STOPPED\n");
		return 1;
	} else if (statusp.status == kSMVMPrxStatusGotPorts) {
		memset(&portp, 0, sizeof(portp));
		portp.vmprx = ajp->mf->vmprx;
		portp.nowait = 1;
		e = sm_vmprx_get_ports(&portp);
		if (e) err =  prosody_error(e, "sm_vmprx_get_ports() failed");
		if (ajp->mf->use_clerk) {
			int rc = rtpclerk_cli_post_port(ajp->mf->clerk_sock, ajp->mf->local_addr, portp.RTP_port);
			if (rc) {
				stop_vmprx(ajp->mf->vmprx);
				return 0;
			}
		} else {
			printf("RTP port = " FORMAT_IN_ADDR ":%u, RTCP port = %u\n", ARGS_IN_ADDR(portp.address), portp.RTP_port, portp.RTCP_port);
		}
	} else if (statusp.status == kSMVMPrxStatusGotPortsIPv6) {
		memset(&portp, 0, sizeof(portp));
		portp.vmprx = ajp->mf->vmprx;
		portp.nowait = 1;
		e = sm_vmprx_get_ports(&portp);
		if (e) err =  prosody_error(e, "sm_vmprx_get_ports() failed");
		if (ajp->mf->use_clerk) {
			int rc = rtpclerk_cli_post_port(ajp->mf->clerk_sock, ajp->mf->local_addr, portp.RTP_port);
			if (rc) {
				stop_vmprx(ajp->mf->vmprx);
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
		printf("Codec specific payload type - %d\n",statusp.u.codec_specific.payload_type);
		e = sm_vmprx_status_discard_codec_specific(ajp->mf->vmprx);
		if (e) err = prosody_error(e, "sm_vmprx_status_discard_codec_specific() failed");
	}
	ajp->mf->avstate = AVSTATE_BUSY;
	if (err) {
		error_log(stderr, error(err, "handle_vmprx() failed"));
		stop_vmprx(ajp->mf->vmprx);
		return 1;
	}
	return 0;	
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

static err_t create_vmprx(ACTIVE_JOB *ajp, tSMModuleId module, unsigned ipv6)
{
 SM_VMPRX_CREATE_PARMS vcp;
 int e;
 memset(&vcp, 0, sizeof(vcp));
 vcp.module = module;
 if (ipv6) vcp.type = kSMVMPrxTypeIPv6;
 e = sm_vmprx_create(&vcp);
 if (e) return prosody_error(e, "sm_vmprx_create() failed");
 ajp->mf->vmprx = vcp.vmprx;
 return 0;
}

	//create a tdm object
static err_t create_tdmtx(ACTIVE_JOB *ajp, VMPPAR vmppar, tSMModuleId module)
{
 SM_TDMTX_CREATE_PARMS tdmcp;
 int e;
 memset(&tdmcp, 0, sizeof(tdmcp));
 tdmcp.module = module;
 tdmcp.stream = vmppar.timeslot.stream;
 tdmcp.timeslot = vmppar.timeslot.timeslot;
 tdmcp.type = vmppar.timeslot.type;
 e = sm_tdmtx_create(&tdmcp);
 if (e) return prosody_error(e, "sm_tdmtx_create() failed");
 ajp->mf->tdmtx = tdmcp.tdmtx;
 return 0;
}

static err_t connect_rtp_tdm(ACTIVE_JOB *ajp)
{
 SM_VMPRX_DATAFEED_PARMS vdgp;
 SM_TDMTX_DATAFEED_CONNECT_PARMS tdcp;
 int e;
 memset(&vdgp, 0, sizeof(vdgp));
 vdgp.vmprx = ajp->mf->vmprx;
 e = sm_vmprx_get_datafeed(&vdgp);
 if (e) return prosody_error(e, "sm_vmprx_get_datafeed() failed");
 memset(&tdcp, 0, sizeof(tdcp));
 tdcp.tdmtx = ajp->mf->tdmtx;
 tdcp.data_source = vdgp.datafeed;
 e = sm_tdmtx_datafeed_connect(&tdcp);
 if (e) return prosody_error(e, "sm_tdmtx_datafeed_connect() failed");
 return 0;
}

static err_t get_event(ACTIVE_JOB *ajp)
{
 SM_VMPRX_EVENT_PARMS evp;
 int e;
 memset(&evp, 0, sizeof(evp));
 evp.vmprx = ajp->mf->vmprx;
 e = sm_vmprx_get_event(&evp);
 if (e) return prosody_error(e, "sm_vmprx_get_event() failed");
 ajp->event = evp.event;
 return 0;
}

static err_t config_vmprx(ACTIVE_JOB *ajp, VMPPAR vmppar)
{
 tSMVMPrxId vmprx = ajp->mf->vmprx; 
 return rtprx_config_vmprx(vmprx, &vmppar.rtprx_par);
}

	// start a new rtp2tdm
static err_t recrtp(ACTIVE_JOB **ajpp, EVENTSET *evs, VMPPAR vmppar)
{
 ACTIVE_JOB *ajp_v;
 err_t err = alloc_active(&ajp_v);
 if (!err) {
	MODDESC mod;
	ajpp[0] = ajp_v;
	ajp_v->mf->avstate = AVSTATE_IDLE;
	ajp_v->handler = handle_vmprx;
	ajp_v->cleanup_fn = cleanup_job;
	ajp_v->mf->use_clerk = vmppar.use_clerk;
	ajp_v->mf->clerk_sock = vmppar.clerk_sock;
	strcpy(ajp_v->mf->local_addr, vmppar.local_addr);
	err = cardlist_getmod(&mod, vmppar.cards, vmppar.curcard, vmppar.curmod);
	if (!err) { 
		err = create_vmprx(ajp_v, mod.id, vmppar.use_ipv6);
		if (!err) {
			err = create_tdmtx(ajp_v, vmppar, mod.id);
			if (!err) {
				err = connect_rtp_tdm(ajp_v);
				if (!err) {
					err = config_vmprx(ajp_v, vmppar);
					if(!err) {
						err = get_event(ajp_v);
						if (!err) {
							err = eventset_insert(evs, ajp_v);
							if (!err) return 0;
						}
					}
				}
			}
		}
	}
 }
 free_active(ajp_v);
 ajp_v = ajpp[0] = 0;
 return err;
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

/*
 * The main gateway loop. Since we want to display channel status,
 * the cleanup function above cannot free the resources (in case
 * we were accessing the channel in this thread while it was freed in the
 * worker thread). Therefore we need to check for finished channels and
 * clear up. There is no real penalty in deferring this cleanup since
 * if we are sitting waiting for a command there's nothing useful we
 * could do with the newly freed resources.
 */
static err_t rtp2tdm(VMPPAR vmppar, unsigned numchan)
{
 ACTIVE_JOB *ajlist[4096];
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
#ifdef USE_POLL
 err = startworker(&tid, &evset);
 if (err) return err;
#endif
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
		else {
			switch(ajlist[u]->mf->avstate) {
			case AVSTATE_IDLE: putchar('v'); break;
			case AVSTATE_BUSY: putchar('V'); break;
			case AVSTATE_DONE:
				putchar('-');
				free_resources(ajlist[u]);
				ajlist[u] = 0;
				break;
			}
		}	
	}
	while (freeac > 0 && !ajlist[freeac-1]) freeac--;
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
		err = recrtp(&ajlist[u], evsetp, vmppar);
		if (err) error_log(stderr, err);
		else {
			if (++vmppar.timeslot.timeslot + 0u == vmppar.maxts) {
				vmppar.timeslot.stream++;
				vmppar.timeslot.timeslot = 0;
			}
			if (vmppar.per_mod && ++vmppar.thismod >= vmppar.per_mod) {
				vmppar.thismod = 0;
				cardlist_nextmod(vmppar.cards, &vmppar.curcard, &vmppar.curmod);
			}
		}
		vmppar.source_port += 2;
		freeac++;
		break;
	case '?':	// help
		printf("Commands:\n"
			"\t+\tstart gateway\n"
			"\t#...\tcomment\n"
			"\tkN\tkill channel N\n"
			"\tKN\tkill channel N\n"
			"\tTN\tset TiNGtrace to N\n"
			"\tw\twait\n"
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
	case 'k':	// kill
	case 'K':	//   with discard
		u = readint();
		if (u >= freeac || !ajlist[u]) {
			printf("Channel %d not in use\n", u);
			break;
		}
		if (ajlist[u]) {
			err = stop_vmprx(ajlist[u]->mf->vmprx);
		}
		if (err) error_log(stderr, err);
		break;
	case 'T':	// Trace
		TiNGtrace = readint();
		break;
	case 'w':
		waiting = 1;
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

#include "gen/rtp2tdm.args.i"

int main(int argc, char **argv)
{
 CARDLIST cards;
 err_t err;
 VMPPAR vmppar;
 ARGS_DECL
 (void) argc;
 memset(&vmppar, 0, sizeof(vmppar));
 if (ARGS_CALL || (*argv && argv[1])) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\nFormats:", progname);
	return 1;
 }
 if (!arg.codec || rtprx_init_par(&vmppar.rtprx_par, arg.codec)) {
	fprintf(stderr, "RTP codec required\nUsage: %s" ARGS_USAGE "\nFormats:", progname);
	fmtlist_dump(stderr);
	rtprx_dump_codecs(stderr);
	return -1;
 }
 cardlist(&cards);
 vmppar.curcard = 0;
 vmppar.curmod = 0;
 if (arg.serialnumber) {
	err = cardlist_addcard(&cards, arg.serialnumber);
	if (err) {
		error_log(stderr, err);
		return 1;
	}
 }
 if (!cards.ncard) {
	fprintf(stderr, "No cards specifed\n");
	return 1;
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
 if (!arg.numchan) arg.numchan = 1;
 vmppar.cards = &cards;
 vmppar.per_mod = arg.per_mod;
 vmppar.thismod = 0;
 vmppar.maxts = arg.maxts;
 vmppar.timeslot = arg.timeslot;
 vmppar.source_port = arg.source_port;
 vmppar.use_ipv6 = arg.use_ipv6;
 vmppar.rtprx_par.regen_tones = arg.regen_tones;
 vmppar.rtprx_par.enforce_tone_spacing = arg.enforce_tone_spacing;
 vmppar.rtprx_par.frame_len = arg.frame_len;
 vmppar.rtprx_par.detect_tones = arg.detect_tones;
 vmppar.rtprx_par.tone_payload_mapping = arg.rfc2833tones;
 vmppar.rtprx_par.init_ms = arg.jitter_initial_ms;
 vmppar.rtprx_par.max_ms = arg.jitter_max_ms;
 vmppar.rtprx_par.ad_target_delay = arg.ad_target_delay;
 vmppar.rtprx_par.ad_freq_upper_tolerance = arg.ad_freq_upper_tolerance;
 vmppar.rtprx_par.ad_freq_lower_tolerance = arg.ad_freq_lower_tolerance;
 vmppar.rtprx_par.payloadmapping = arg.payloadmapping;
 vmppar.rtprx_par.plc = arg.plc;
 vmppar.rtprx_par.bitrate = arg.bitrate;
 vmppar.rtprx_par.variant = arg.variant;
 vmppar.rtprx_par.handle_cn = !!arg.comfort_noise;
 vmppar.rtprx_par.comfort_noise_payload_mapping = arg.comfort_noise_payload_mapping;
 err = rtp2tdm(vmppar, arg.numchan);
 if (err) {
	error_log(stderr, err);
 }
 cardlist_dtor(&cards);
 return err ? 1 : 0;
}
