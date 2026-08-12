/* tdm2rtp.c - demonstration of a TDM to RTP gateway with an eventset */
#include <errno.h>
#include <math.h>
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
#include "../Testlib/error.h"
#include "../Testlib/fmtcode.h"
#include "../Testlib/errcode_sm.h"
#include "../Testlib/eventset.h"
#include "../Testlib/rtpclerk_cli.h"
#include "../Testlib/rtptx_codec.h"
#include "../Testlib/taistamp.h"
#include "../Testlib/cardlist.h"
#include "smbesp.h"
#include "smdrvr.h"
#include "smrtp.h"
#include "../../apilib/smsync.h"	// unpublished API

#define arlen(x) (sizeof(x)/sizeof(*(x)))

static tSMVMPTxToneSetId G_VMPToneSet=0;

struct appactive {
	tSMVMPtxId vmptx;
	tSMTDMrxId tdmrx;
	enum {
		AVSTATE_IDLE,
		AVSTATE_BUSY,
		AVSTATE_DONE,
	} avstate;
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
	unsigned curcard;
	unsigned curmod;
	CARDLIST *cards;
	unsigned per_mod;
	unsigned thismod;
	RTPCLERK_SOCKET clerk_sock;
	unsigned use_clerk;
	unsigned short source_port;
	unsigned short dest_port;
	char dest_ip[256];
	unsigned elim_tones;
	unsigned maxts;
		//for tdm
	MVIP timeslot;
	RTPTX_PAR rtptx_par;
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
	// channel's event handler finished the job.
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
 sm_vmptx_destroy(ajp->mf->vmptx);
 sm_tdmrx_destroy(ajp->mf->tdmrx);
 free_active(ajp);
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

static int handle_vmptx(ACTIVE_JOB *ajp)
{
	SM_VMPTX_STATUS_PARMS statusp;
	int e;
	err_t err = 0;
	memset(&statusp,0,sizeof(statusp));
	statusp.vmptx = ajp->mf->vmptx;
	e = sm_vmptx_status(&statusp);
	if (e) err = prosody_error(e, "sm_vmptx_status() failed");
	else if (statusp.status == kSMVMPtxStatusStopped) {
		return 1;
	}
	ajp->mf->avstate = AVSTATE_BUSY;
	if (err) {
		error_log(stderr, error(err, "handle_vmptx() failed"));
		stop_vmptx(ajp->mf->vmptx);
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

static err_t create_vmptx(ACTIVE_JOB *ajp, tSMModuleId module)
{
 SM_VMPTX_CREATE_PARMS vcp;
 int e;
 memset(&vcp, 0, sizeof(vcp));
 vcp.module = module;
 e = sm_vmptx_create(&vcp);
 if (e) return prosody_error(e, "sm_vmptx_create() failed");
 ajp->mf->vmptx = vcp.vmptx;
 return 0;
}

	//create a tdm object
static err_t create_tdmrx(ACTIVE_JOB *ajp, VMPPAR vmppar, tSMModuleId module)
{
 SM_TDMRX_CREATE_PARMS tdmcp;
 int e;
 memset(&tdmcp, 0, sizeof(tdmcp));
 tdmcp.module = module;
 tdmcp.stream = vmppar.timeslot.stream;
 tdmcp.timeslot = vmppar.timeslot.timeslot;
 tdmcp.type = vmppar.timeslot.type;
 e = sm_tdmrx_create(&tdmcp);
 if (e) return prosody_error(e, "sm_tdmrx_create() failed");
 ajp->mf->tdmrx = tdmcp.tdmrx;
 return 0;
}

static err_t connect_tdm_rtp(ACTIVE_JOB *ajp)
{
 SM_TDMRX_DATAFEED_PARMS tdgp;
 SM_VMPTX_DATAFEED_CONNECT_PARMS vdcp;
 int e;
 memset(&tdgp, 0, sizeof(tdgp));
 tdgp.tdmrx = ajp->mf->tdmrx;
 e = sm_tdmrx_get_datafeed(&tdgp);
 if (e) return prosody_error(e, "sm_tdmrx_get_datafeed() failed");
 memset(&vdcp, 0, sizeof(vdcp));
 vdcp.vmptx = ajp->mf->vmptx;
 vdcp.data_source = tdgp.datafeed;
 e = sm_vmptx_datafeed_connect(&vdcp);
 if (e) return prosody_error(e, "sm_vmptx_datafeed_connect() failed");
 return 0;
}

static err_t get_event(ACTIVE_JOB *ajp)
{
 SM_VMPTX_EVENT_PARMS evp;
 int e;
 memset(&evp, 0, sizeof(evp));
 evp.vmptx = ajp->mf->vmptx;
 e = sm_vmptx_get_event(&evp);
 if (e) return prosody_error(e, "sm_vmptx_get_event() failed");
 ajp->event = evp.event;
 return 0;
}

static err_t config_vmptx(ACTIVE_JOB *ajp, VMPPAR vmppar, tSMModuleId module)
{
 SM_VMPTX_CONFIG_PARMS configp;
 tSMVMPtxId vmptx = ajp->mf->vmptx; 
 int e;
 err_t err;
 struct addrinfo hints;
 struct addrinfo *result;
 int s;
 char *ipaddr, *msg = 0;
 short port_num;
 memset(&configp, 0, sizeof(configp));
 configp.vmptx = vmptx;
 if (vmppar.use_clerk) {
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
	printf("got addr %s, port %d\n", ipaddr, port_num);
 } else {
	ipaddr = vmppar.dest_ip;
	port_num = vmppar.dest_port;
 }
 memset(&hints, 0, sizeof(struct addrinfo));
 hints.ai_family = AF_UNSPEC;	/* Allow IPv4 or IPv6 */
 hints.ai_socktype = SOCK_DGRAM;
 hints.ai_protocol = IPPROTO_UDP;
 hints.ai_flags = AI_NUMERICHOST;

 s = getaddrinfo(vmppar.dest_ip, NULL, &hints, &result);
 if (s != 0) {
	err = error(error(NULL, gai_strerror(s)), "getaddrinfo failed");
	return err;
 }

 if (result->ai_family == AF_INET) {
	SM_VMPTX_CONFIG_PARMS configp;
	memset(&configp, 0, sizeof(configp));
	configp.vmptx = vmptx;
	configp.destination_rtp.sin_addr.s_addr = ((struct sockaddr_in*)result->ai_addr)->sin_addr.s_addr;
	configp.destination_rtp.sin_port = htons(port_num);
	configp.destination_rtcp.sin_addr.s_addr = ((struct sockaddr_in*)result->ai_addr)->sin_addr.s_addr;
	configp.destination_rtcp.sin_port = htons(port_num+1);
	configp.source_rtp.sin_port = htons(vmppar.source_port);
	e = sm_vmptx_config(&configp);
	if (e) return prosody_error(e, "sm_vmptx_config() failed");
 } else {
	SM_VMPTX_CONFIG_IPV6_PARMS configp;
	memset(&configp, 0, sizeof(configp));
	configp.vmptx = vmptx;
	configp.destination_rtp.sin6_addr = ((struct sockaddr_in6*)result->ai_addr)->sin6_addr;
	configp.destination_rtp.sin6_port = htons(port_num);
	configp.destination_rtcp.sin6_addr = ((struct sockaddr_in6*)result->ai_addr)->sin6_addr;
	configp.destination_rtcp.sin6_port = htons(port_num+1);
	configp.source_rtp.sin6_port = htons(vmppar.source_port);
	e = sm_vmptx_config_ipv6(&configp);
	if (e) return prosody_error(e, "sm_vmptx_config() failed");
 }
 freeaddrinfo(result);
 if (vmppar.use_clerk) {
	free(ipaddr);
	free(msg);
 }

 err = rtptx_config_vmptx(vmptx, &vmppar.rtptx_par);
 if (err) return err;
 if(vmppar.elim_tones) {
	struct sm_vmptx_tone_parms tparms;
	SM_VMPTX_CODEC_RFC2833_PARMS codecp;
	if(!G_VMPToneSet) {
		struct sm_vmptx_create_toneset_parms tsparms;
		tsparms.module = module;
		tsparms.toneset = kSMVMPTxDefaultToneSet;
		e = sm_vmptx_create_toneset(&tsparms);
 		if (e) return prosody_error(e, "sm_vmptx_config_toneset() failed");
		G_VMPToneSet = tsparms.tone_set_id;
	}
	//turn on tones;
	memset(&codecp,0,sizeof(codecp));
	codecp.vmptx = vmptx;
	codecp.payload_type = vmppar.elim_tones;
	e = sm_vmptx_config_codec_rfc2833(&codecp);
	if (e) return prosody_error(e,"sm_vmptx_config_code_rfc2833() failed");
	memset(&tparms, 0, sizeof(tparms));
	tparms.vmptx = vmptx;
	tparms.convert_tones = 1;
	tparms.tone_set_id = G_VMPToneSet;
	tparms.elim_tones = 1;
	e = sm_vmptx_config_tones(&tparms);
 	if (e) return prosody_error(e, "sm_vmprx_config_toneset() failed");
 }
 return 0;
}

	// start a new tdm2rtp
static err_t playrtp(ACTIVE_JOB **ajpp, EVENTSET *evs, VMPPAR vmppar)
{
 ACTIVE_JOB *ajp_v;
 err_t err = alloc_active(&ajp_v);
 if (!err) {
	MODDESC mod;
	ajpp[0] = ajp_v;
	ajp_v->mf->avstate = AVSTATE_IDLE;
	ajp_v->handler = handle_vmptx;
	ajp_v->cleanup_fn = cleanup_job;
	err = cardlist_getmod(&mod, vmppar.cards, vmppar.curcard, vmppar.curmod);
	if (!err) { 
		err = create_vmptx(ajp_v, mod.id);
		if (!err) {
			vmppar.rtptx_par.sample_rate = 8000;
			err = create_tdmrx(ajp_v, vmppar, mod.id);
			if (!err) {
				err = connect_tdm_rtp(ajp_v);
				if (!err) {
					err = config_vmptx(ajp_v, vmppar, mod.id);
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
static err_t tdm2rtp(VMPPAR vmppar, unsigned numchan)
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
		err = playrtp(&ajlist[u], evsetp, vmppar);
		if (err) error_log(stderr, err);
		else {
			if (++vmppar.timeslot.timeslot + 0u == vmppar.maxts) {
				vmppar.timeslot.stream++;
				vmppar.timeslot.timeslot = 0;
			}
			if (vmppar.per_mod && ++vmppar.thismod == vmppar.per_mod) {
				cardlist_nextmod(vmppar.cards, &vmppar.curcard, &vmppar.curmod);
				vmppar.thismod = 0;
			}
		}
		vmppar.source_port += 2;
		freeac++;
		break;
	case '?':	// help
		printf("Commands:\n"
			"\t+\tstart tdm2rtp to current destination\n"
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
			err = stop_vmptx(ajlist[u]->mf->vmptx);
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

#include "gen/tdm2rtp.args.i"

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
	fmtlist_dump(stderr);
	rtptx_dump_codecs(stderr);
	return 1;
 }
 if (!arg.codec || rtptx_init_par(&vmppar.rtptx_par, arg.codec)) {
	fprintf(stderr, "RTP codec required\nUsage: %s" ARGS_USAGE "\nFormats:", progname);
	fmtlist_dump(stderr);
	rtptx_dump_codecs(stderr);
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
 if (!arg.numchan) arg.numchan = 1;
 vmppar.cards = &cards;
 vmppar.per_mod = arg.per_mod;
 vmppar.thismod = 0;
 vmppar.maxts = arg.maxts;
 vmppar.timeslot = arg.timeslot;
 vmppar.source_port = arg.source_port;
 if (arg.clerk_port && arg.clerk_addr) {
#ifdef TiNGTYPE_WINNT
	WSAInit();
#endif
 	vmppar.clerk_sock = rtpclerk_cli_connect(arg.clerk_addr, arg.clerk_port);
	if (vmppar.clerk_sock < 0) return -1;
	vmppar.use_clerk = 1;
 } else {
 	//only use the command line options if we're not using the clerk
	strcpy(vmppar.dest_ip, arg.dest_ip);
	vmppar.dest_port = arg.dest_port;
 }
 vmppar.elim_tones = arg.rfc2833tones;
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
 err = tdm2rtp(vmppar, arg.numchan);
 if (err) {
	error_log(stderr, err);
 }
 if(G_VMPToneSet) {
	struct sm_vmptx_destroy_toneset_parms tsparms;
	int e;
	tsparms.tone_set_id = G_VMPToneSet;
	e = sm_vmptx_destroy_toneset(&tsparms);
	if (e) error_log(stderr, prosody_error(e, "sm_vmptx_destroy_toneset() failed"));
 }
 cardlist_dtor(&cards);
 return err ? 1 : 0;
}
