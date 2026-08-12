/* detect.c - test/demo of answering incoming calls and detecting tones */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "smbesp.h"
#include "smdrvr.h"
#include "../../libutil/generic_io.h"
#include "../Testlib/cardlist.h"
#include "../Testlib/v6.h"

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
#define sleep(s) Sleep((s)*1000)
#endif

#define TiNG_PROSODYS_PORTNO_ENVVARNAME "ACULAB_PROSODYS_PORT"


typedef struct {
	int stream;
	int timeslot;
	int type;
} MVIP;
#define MVIP_STREAM_NONE 0xff

static char *endstr(char *s)
{
 while (*s) s++;
 return s;
}

typedef enum {TONE_TYPE_START, TONE_TYPE_END, TONE_TYPE_LEN} TONE_TYPE;

static int switch_call_from_ts(unsigned port, unsigned slot, MVIP xts)
{
 struct output_parms op;
 int i = call_port_2_swdrvr(port);
 int err;
 if (i < 0) {
	fprintf(stderr, "%d.%d: call_port_2_swdrvr returned %d\n", port, slot, i);
	return 1;
 }
 INIT_ACU_CL_STRUCT(&op);
 op.ist = call_port_2_stream(port);
 if (op.ost < 0) {
	fprintf(stderr, "%d.%d: call_port_2_stream returned %d\n", port, slot, op.ost);
	return 1;
 }
 op.its = slot;
 op.mode = CONNECT_MODE;
 op.ost = xts.stream;
 op.ots = xts.timeslot;
 printf("%d.%d <- %d.%d\n", op.ist, op.its, op.ost, op.ots);
 err = sw_set_output(i, &op);
 if (err) fprintf(stderr, "%d.%d: sw_set_output returned %d\n", port, slot, err);
 return err;
}

static int break_call_from_ts(unsigned port, unsigned slot, MVIP xts)
{
 struct output_parms op;
 int i = call_port_2_swdrvr(port);
 int err;
 if (i < 0) {
	fprintf(stderr, "%d.%d: call_port_2_swdrvr returned %d\n", port, slot, i);
	return 1;
 }
 INIT_ACU_CL_STRUCT(&op);
 op.ost = xts.stream;
 op.ots = xts.timeslot;
 op.mode = DISABLE_MODE;
 err = sw_set_output(i, &op);
 if (err) fprintf(stderr, "%d.%d: sw_set_output returned %d\n", port, slot, err);
 return err;
}

static struct l1 {
	struct l1 *next;
	pthread_t thread;
	int stop;
	ACU_INT net;
} *l1s;

static void *checkl1(void *vp)
{
 struct l1 *l1 = vp;
 struct l1_xstats prevxs;
 INIT_ACU_CL_STRUCT(&prevxs);
 
 while (!l1->stop) {
	struct l1_xstats xs;
	char buff[256];
	int err;
	INIT_ACU_CL_STRUCT(&xs);
	xs.net = l1->net;
 	err = call_l1_stats(&xs);
 	if (err) {
		fprintf(stderr, "%d: call_l1_stats returned %s\n", l1->net, error_2_string(err));
		return (void *) 1;
	}
	buff[0] = 0;
	if (xs.getset.linestat != prevxs.getset.linestat)
		sprintf(endstr(buff), " linestat=0x%04x", xs.getset.linestat);
	if (xs.getset.bipvios != prevxs.getset.bipvios)
		sprintf(endstr(buff), " bipvios=0x%04x", xs.getset.bipvios);
	if (xs.getset.faserrs != prevxs.getset.faserrs)
		sprintf(endstr(buff), " faserrs=0x%04x", xs.getset.faserrs);
	if (xs.getset.sliperrs != prevxs.getset.sliperrs)
		sprintf(endstr(buff), " sliperrs=0x%04x", xs.getset.sliperrs);
	if (xs.get.majorrev != prevxs.get.majorrev
		|| xs.get.minorrev != prevxs.get.minorrev)
		sprintf(endstr(buff), " v%d.%d", xs.get.majorrev, xs.get.minorrev);
	if (xs.get.nos != prevxs.get.nos)
		sprintf(endstr(buff), " %cnos", "+-"[!xs.get.nos]);
	if (xs.get.ais != prevxs.get.ais)
		sprintf(endstr(buff), " %cais", "+-"[!xs.get.ais]);
	if (xs.get.los != prevxs.get.los)
		sprintf(endstr(buff), " %clos", "+-"[!xs.get.los]);
	if (xs.get.rra != prevxs.get.rra)
		sprintf(endstr(buff), " %crra", "+-"[!xs.get.rra]);
	if (xs.get.tra != prevxs.get.tra)
		sprintf(endstr(buff), " %ctra", "+-"[!xs.get.tra]);
	if (xs.get.rma != prevxs.get.rma)
		sprintf(endstr(buff), " %crma", "+-"[!xs.get.rma]);
	if (xs.get.tma != prevxs.get.tma)
		sprintf(endstr(buff), " %ctma", "+-"[!xs.get.tma]);
	if (xs.get.usr != prevxs.get.usr)
		sprintf(endstr(buff), " usr=%02x", xs.get.usr);
	if (memcmp(xs.get.buildstr, prevxs.get.buildstr, sizeof(xs.get.buildstr)))
		sprintf(endstr(buff), " buildstr='%s'", xs.get.buildstr);
	if (memcmp(xs.get.manstr, prevxs.get.manstr, sizeof(xs.get.manstr)))
		sprintf(endstr(buff), " manstr='%s'", xs.get.manstr);
	if (memcmp(xs.get.sigstr, prevxs.get.sigstr, sizeof(xs.get.sigstr)))
		sprintf(endstr(buff), " sigstr='%s'", xs.get.sigstr);
	if (buff[0]) printf("%d:%s\n", l1->net, buff);
	prevxs = xs;
	sleep(1);
 }
 return 0;
}

static struct chan {
	struct chan *next;
	pthread_t thread;
	int stop;
	ACU_INT handle;
	unsigned port, slot;
	MVIP xts;
	unsigned toneset;
	unsigned grunt;
	unsigned map;
	unsigned cptone;
	unsigned tonemode;
	TONE_TYPE tonetype;
	tSMModuleId module;
	int catsig_alg;
} *chans;

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
 if (err) fprintf(stderr, "%d.%d: sm_switch_channel_input returned %d\n", cp->port, cp->slot, err);
 return err;
}

static int handle_recog(struct chan *cp, tSMChannelId chan)
{
 for (;;) {
	SM_RECOGNISED_PARMS rp;
	const char *type;
	int err;
	memset(&rp, 0, sizeof(rp));
	rp.channel = chan;
	err = sm_get_recognised(&rp);
	if (err) {
		fprintf(stderr, "%d.%d: sm_get_recognised returned %d\n", cp->port, cp->slot, err);
		return 1;
	}
	switch (rp.type) {
	case kSMRecognisedNothing: type = 0; break;
	case kSMRecognisedTrainingDigit: type = "TrainingDigit"; break;
	case kSMRecognisedDigit:
		type = 0;
		printf("Digit: %d (%c) %d\n", rp.param0, rp.param0, rp.param1);
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
		fprintf(stderr, "%d.%d: sm_get_recognised produced event %d\n", cp->port, cp->slot, rp.type);
		return 1;
	}
	if (!type) break;
	printf("%s: %d %d\n", type, rp.param0, rp.param1);
 }
 return 0;
}

static int detectloop(tSMChannelId chan, struct chan *cp, struct state_xparms *cxp, tSMEventId ev)
{
 for (;;) {
	int err;
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
	} else {
		if (pfd.revents & POLLHUP) {
			fprintf(stderr, "%d.%d: POLLHUP\n", cp->port, cp->slot);
			return 1;
		}
		if (handle_recog(cp, chan)) return 1;
	}
#endif
#ifdef TiNGTYPE_WINNT
	DWORD res = WaitForSingleObject(ev, 500);
	if (res != WAIT_OBJECT_0) {
		if (res != WAIT_TIMEOUT) {
			fprintf(stderr, "%d.%d: WaitForSingleObject returned %08lx\n", cp->port, cp->slot, res);
			return 1;
		}
	} else {
		if (handle_recog(cp, chan)) return 1;
	}
#endif
	cxp->timeout = 0;
	cxp->handle = cp->handle;
	err = call_event(cxp);
	if (err) {
		fprintf(stderr, "%d.%d: call_state returned %s\n", cp->port, cp->slot, error_2_string(err));
		return 1;
	}
	if (cxp->handle)
	{
		if (cxp->state != EV_CALL_CONNECTED) {
			return 0;
		}
	}
 }
}

static int detectchan(struct chan *cp, struct state_xparms *cxp)
{
 struct sm_channel_alloc_placed_parms ap;
 struct detail_xparms xp;
 int err;
 INIT_ACU_CL_STRUCT(&xp);
 xp.handle = cp->handle;
 xp.timeout = 0;
 err = call_details(&xp);
 if (err) {
	fprintf(stderr, "%d.%d: call_details returned %s\n", cp->port, cp->slot, error_2_string(err));
	return 1;
 }
 if (!xp.valid) {
	printf("%d.%d: details not valid\n", cp->port, cp->slot);
	return 1;
 }
 // detect on timeslot xp.stream:xp.ts
 memset(&ap, 0, sizeof(ap));
 ap.type = kSMChannelTypeInput;
 ap.module = cp->module;
 err = sm_channel_alloc_placed(&ap);
 if (err) {
 	fprintf(stderr, "%d.%d: sm_channel_alloc_placed returned %d\n", cp->port, cp->slot, err);
 } else {
	int e2;
	if (!fn_sm_switch_channel_input(ap.channel, cp)
		&& !switch_call_from_ts(cp->port, cp->slot, cp->xts)) {
		struct sm_listen_for_parms lp;
		memset(&lp, 0, sizeof(lp));
		lp.channel = ap.channel;
		if (cp->grunt != ~0U) {
			lp.enable_grunt_detection = 1;
			lp.grunt_latency = cp->grunt;
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
		err = sm_listen_for(&lp);
		if (err) fprintf(stderr, "%d.%d: sm_listen_for returned %d\n", cp->port, cp->slot, err);
		if (!err && cp->catsig_alg) {
			struct sm_catsig_listen_for_parms clp;
			memset(&clp, 0, sizeof(clp));
			clp.channel = ap.channel;
			clp.catsig_alg_id = cp->catsig_alg;
			err = sm_catsig_listen_for(&clp);
			if (err) fprintf(stderr, "sm_catsig_listen_for returned %d\n", err);
		}
		if (!err) {
			SM_CHANNEL_SET_EVENT_PARMS ep;
			memset(&ep, 0, sizeof(ep));
			err = smd_ev_create(&ep.event, ap.channel, kSMEventTypeRecog, kSMChannelSpecificEvent);
			if (err) fprintf(stderr, "%d.%d: smd_ev_create returned %d\n", cp->port, cp->slot, err);
			else {
				ep.channel = ap.channel;
				ep.event_type=kSMEventTypeRecog;
				ep.issue_events=kSMChannelSpecificEvent;
				err = sm_channel_set_event(&ep);
				if (err) fprintf(stderr, "%d.%d: sm_channel_set_event returned %d\n", cp->port, cp->slot, err);
				else err = detectloop(ap.channel, cp, cxp, ep.event);
				smd_ev_free(ep.event);
			}
		}
		break_call_from_ts(cp->port, cp->slot, cp->xts);
 	}
 	e2 = sm_channel_release(ap.channel);
	if (e2) fprintf(stderr, "%d.%d: sm_channel_release returned %d\n", cp->port, cp->slot, e2);
	err = err || e2;
 }
 return err;
}

static int showdetails(struct chan *cp)
{
 struct detail_xparms xp;
 int err;
 INIT_ACU_CL_STRUCT(&xp);
 xp.handle = cp->handle;
 xp.timeout = 0;
 err = call_details(&xp);
 if (err) {
	fprintf(stderr, "%d.%d: call_details returned %s\n", cp->port, cp->slot, error_2_string(err));
	return 1;
 }
 printf("%d.%d: ", cp->port, cp->slot);
 if (!xp.valid) {
	printf("details not valid\n");
	return 0;
 }
 printf("@%d.%d", xp.stream, xp.ts);
 switch (xp.calltype) {
 case OUTGOING: printf(" OUT"); break;
 case INCOMING: printf(" IN"); break;
 default: printf(" ty=%d", xp.calltype);
 }
 printf(" sc=%d '%s' -> '%s' as '%s'", xp.sending_complete,
 	xp.originating_addr, xp.connected_addr, xp.destination_addr);
 printf("\n");
 return 0;
}

static void *chanfunc(void *vp)
{
 struct chan *cp = vp;
 while (!cp->stop) {
	int err;
	{
	 struct in_xparms inx;
	 INIT_ACU_CL_STRUCT(&inx);

	 inx.net = cp->port;
	 inx.ts = cp->slot;
	 err = call_openin(&inx);
	 cp->handle = inx.handle;
	}
	if (err) {
		fprintf(stderr, "%d.%d: call_openin returned %s\n", cp->port, cp->slot, error_2_string(err));
		return (void *) 1;
	}
	for (;;) {
		struct state_xparms stx;
		INIT_ACU_CL_STRUCT(&stx);
		stx.handle = cp->handle;
		stx.timeout = 1000;
		err = call_event(&stx);
		if (err) {
			fprintf(stderr, "%d.%d: call_state returned %s\n", cp->port, cp->slot, error_2_string(err));
			return (void *) 1;
		}
	if (!stx.handle) continue;
	recheck:
		switch (stx.state) {
		case EV_WAIT_FOR_INCOMING:
			fprintf(stderr, "%d.%d: EV_WAIT_FOR_INCOMING\n", cp->port, cp->slot);
			break;
		case EV_INCOMING_CALL_DET:
			fprintf(stderr, "%d.%d: EV_INCOMING_CALL_DET\n", cp->port, cp->slot);
			if (showdetails(cp)) return (void *) 1;
			err = call_accept(stx.handle);
			if (err) {
				fprintf(stderr, "%d.%d: call_accept returned %s\n", cp->port, cp->slot, error_2_string(err));
				return (void *) 1;
			}
			break;
		case EV_CALL_CONNECTED:
			fprintf(stderr, "%d.%d: EV_CALL_CONNECTED\n", cp->port, cp->slot);
			if (detectchan(cp, &stx)) return (void *) 1;
			goto recheck;
		case EV_WAIT_FOR_OUTGOING:
			fprintf(stderr, "%d.%d: EV_WAIT_FOR_OUTGOING\n", cp->port, cp->slot);
			break;
		case EV_OUTGOING_RINGING:
			fprintf(stderr, "%d.%d: EV_OUTGOING_RINGING\n", cp->port, cp->slot);
			break;
		case EV_REMOTE_DISCONNECT:
			fprintf(stderr, "%d.%d: EV_REMOTE_DISCONNECT\n", cp->port, cp->slot);
			break;
		case EV_WAIT_FOR_ACCEPT:
			fprintf(stderr, "%d.%d: EV_WAIT_FOR_ACCEPT\n", cp->port, cp->slot);
			break;
		case EV_IDLE:
			fprintf(stderr, "%d.%d: EV_IDLE\n", cp->port, cp->slot);
			// clear connection
			goto endcall;
		case EV_HOLD:
			fprintf(stderr, "%d.%d: EV_HOLD\n", cp->port, cp->slot);
			break;
		case EV_HOLD_REJECT:
			fprintf(stderr, "%d.%d: EV_HOLD_REJECT\n", cp->port, cp->slot);
			break;
		case EV_TRANSFER_REJECT:
			fprintf(stderr, "%d.%d: EV_TRANSFER_REJECT\n", cp->port, cp->slot);
			break;
		case EV_RECONNECT_REJECT:
			fprintf(stderr, "%d.%d: EV_RECONNECT_REJECT\n", cp->port, cp->slot);
			break;
		case EV_PROGRESS:
			fprintf(stderr, "%d.%d: EV_PROGRESS\n", cp->port, cp->slot);
			break;
		case EV_OUTGOING_PROCEEDING:
			fprintf(stderr, "%d.%d: EV_OUTGOING_PROCEEDING\n", cp->port, cp->slot);
			break;
		}
	}
endcall:
	{
	 struct cause_xparms xp;
	 INIT_ACU_CL_STRUCT(&xp);
	 
	 xp.handle = cp->handle;
	 xp.cause = LC_NORMAL;
	 err=call_release(&xp);
 	}
	if (err) {
		fprintf(stderr, "%d.%d: call_release returned %s\n", cp->port, cp->slot, error_2_string(err));
		return (void *) 1;
	}
 }
 return 0;
}

static int detect(ACU_CARD_ID card, int numports, CARDLIST *cardsp, MVIP ts, unsigned toneset, unsigned grunt, unsigned map, unsigned cptone, unsigned tonemode, TONE_TYPE tonetype, unsigned per_mod, unsigned catsig_alg)
{
	ACU_PORT_ID *ports = 0;
	int i;
 unsigned curcardpos = 0;
 unsigned curmodpos = 0;
 unsigned thismod = 0;
 unsigned port;
 int err;
 printf("ansplay..... numports is %d\n",numports);
 for (port=0; port < (unsigned) numports ; port++) {
	 // oen the port
	 OPEN_PORT_PARMS parms;
	 PORT_INFO_PARMS pinfo;
     ACU_ERR AERR;
	 struct l1 *l1p = malloc(sizeof(*l1p));
	 unsigned nts=0;
	 unsigned mask;
	 unsigned slot;
	 ACU_PORT_ID *np = realloc(ports, (port + 1) * sizeof(*ports));
	 if (!np) {
		perror("realloc() failed");
		return 1;
	 }
	 ports = np;
	 INIT_ACU_CL_STRUCT(&parms);
	 parms.port_ix=port;
	 parms.card_id=card;
	 AERR = call_open_port(&parms);
	 if (AERR)
	 {
		 printf("failed to open port %d,%d\n",port,AERR);
		 return AERR;
	 }
	 ports[port]=parms.port_id;
	 if (!l1p) {
		perror("malloc");
		fprintf(stderr, "Cannot allocate l1 struct\n");
		return 1;
	}
	l1p->stop = 0;
	l1p->net = ports[port];
	err = pthread_create(&l1p->thread, 0, checkl1, l1p);
	if (err) {
		fprintf(stderr, "pthread_create failed: %s\n", strerror(err));
		return 1;
	}
	l1p->next = l1s;
	l1s = l1p;
	// find our sigsys mask
	INIT_ACU_CL_STRUCT(&pinfo);
	pinfo.port_id=ports[port];
	AERR = call_port_info(&pinfo);
	if(AERR)
	{
		printf("call_port_info returned %d\n",AERR);
		return 0;
	}
	if (pinfo.port_type & ACU_PORT_CAP_IP)
	{
		printf("Skipping IP telephony port %d\n", port);
		continue;
	}
	printf("port info is %ld\n", (long) pinfo.valid_vector);
	for (slot=0, mask = pinfo.valid_vector; mask; slot++, mask >>=1) {
		if (mask & 1) {
			struct chan *cp = malloc(sizeof(*cp));
			MODDESC mod;
			err_t e;
			if (!cp) {
				perror("malloc");
				fprintf(stderr, "Cannot allocate channel struct\n");
				return 1;
			}
			cp->stop = 0;
			e = cardlist_getmod(&mod, cardsp, curcardpos, curmodpos);
			if (e) {
				error_log(stderr, e);
				return 1;
			}
			cp->module = mod.id;
			cp->port = ports[port];
			cp->xts = ts;
			cp->slot = slot;
			cp->toneset = toneset;
			cp->grunt = grunt;
			cp->map = map;
			cp->cptone = cptone;
			cp->tonemode = tonemode;
			cp->tonetype = tonetype;
			cp->catsig_alg = catsig_alg;
			err = pthread_create(&cp->thread, 0, chanfunc, cp);
			if (err) {
				fprintf(stderr, "pthread_create failed: %s\n", strerror(err));
				return 1;
			}
			nts++;
			cp->next = chans;
			chans = cp;
			if (++ts.timeslot == 32) {
				ts.timeslot = 0;
				ts.stream ^= 1;
			}
			if (++thismod >= per_mod) {
				thismod = 0;
				cardlist_nextmod(cardsp, &curcardpos, &curmodpos);
			}
		}
	}
	printf("Started %d timeslots on port %d: %s\n", nts, port, pinfo.sig_sys);
 }
 for (;;) {
 	int c = getchar();
 	switch (c) {
		struct l1 *l1p;
		struct chan *cp;
 	case '\n': break;
 	case 'q':
 	case EOF:
		printf("exitting\n");
		for (l1p = l1s; l1p; l1p=l1p->next) {
			l1p->stop = 1;
		}
		for (cp = chans; cp; cp=cp->next) {
			cp->stop = 1;
		}
		while (l1s) {
			struct l1 *nx;
			void *sts;
			err = pthread_join(l1s->thread, &sts);
			if (err) {
				fprintf(stderr, "pthread_join failed: %s\n", strerror(err));
				return 1;
			}
			nx = l1s->next;
			free(l1s);
			l1s = nx;
		}
		while (chans) {
			struct cause_xparms xp;
			struct chan *nx;
			void *sts;
			INIT_ACU_CL_STRUCT(&xp)
			chans->stop = 1;
			xp.handle = chans->handle;
			xp.cause = LC_NORMAL;
			err=call_disconnect(&xp);
			if (err) {
				fprintf(stderr, "%d.%d: call_disconnect returned %s\n", chans->port, chans->slot, error_2_string(err));
				return 1;
			}
			err = pthread_join(chans->thread, &sts);
			if (err) {
				fprintf(stderr, "pthread_join failed: %s\n", strerror(err));
				return 1;
			}
			nx = chans->next;
			free(chans);
			chans = nx;
		 }
		// kill our ports
		
		for (i=0;i<numports;i++)
		{
			CLOSE_PORT_PARMS clpparms;
			INIT_ACU_CL_STRUCT(&clpparms);
			clpparms.port_id=ports[i];
			call_close_port(&clpparms);
		}
		return 0;
 	}
 }
}

#include "gen/detect.args.i"

int main(int argc, char **argv)
{
 ACU_ERR AERR;
 ACU_OPEN_CARD_PARMS ocparms;
 ACU_OPEN_SWITCH_PARMS osparms;
 ACU_OPEN_CALL_PARMS oclparms;
 ACU_CARD_INFO_PARMS aciparms;
 CARD_INFO_PARMS cdiparms;
 CARDLIST cards;
 char cardspec[256];
 int e;
 TONE_TYPE tonetype = TONE_TYPE_START;
 ARGS_DECL
 arg.grunt = ~0;

 (void) argc;
 if (ARGS_CALL || *argv) {
usage:
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
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
 if (arg.grunt == ~0U && !arg.cptone && !arg.tonemode && !arg.catsig_alg) {
	printf("Warning: no detection selected\n");
 }

// open our card
 INIT_ACU_STRUCT(&ocparms);
 if (!arg.serialnumber) {
	fprintf(stderr, "No cards specifed\n");
	return 1;
 }
 strcpy(ocparms.serial_no,arg.serialnumber);
 AERR = acu_open_card(&ocparms);
 if (AERR)
 {
 	printf("failed to open card %d\n",AERR);
 	return AERR;
 }
// open switch
 INIT_ACU_STRUCT(&osparms);
 osparms.card_id = ocparms.card_id;
 AERR = acu_open_switch(&osparms);
 if (AERR)
 {
 	printf("failed to open switch %d\n",AERR);
 	return AERR;
 }
// open call
 INIT_ACU_STRUCT(&oclparms);
 oclparms.card_id = ocparms.card_id;
 AERR = acu_open_call(&oclparms);
 if (AERR)
 {
 	printf("failed to open call %d\n",AERR);
 	return AERR;
 }
 // ok lets see what we haveon this card
 INIT_ACU_CL_STRUCT(&cdiparms);
 cdiparms.card_id = ocparms.card_id;
 AERR = call_get_card_info(&cdiparms);
 if (AERR)
 {
 	printf("failed to get call card info, %d\n",AERR);
 	return AERR;
 }
 // create cardspec based on card type
 INIT_ACU_STRUCT(&aciparms);
 aciparms.card_id = ocparms.card_id;
 AERR = acu_get_card_info(&aciparms);
 if (AERR)
 {
 	printf("failed to get card info, %d\n",AERR);
 	return AERR;
 }
 switch(aciparms.card_type){
 case ACU_PROSODY_X_CARD:
	 if (strlen(aciparms.card_key) == 0)
		sprintf(cardspec, "x:%s",aciparms.ip_address);
	 else
		sprintf(cardspec, "x:%s/%s",aciparms.ip_address,aciparms.card_key);
	 break;
 case ACU_PROSODY_S_V3_CARD:
	 if (strlen(aciparms.card_key) == 0)
		sprintf(cardspec, "s3:%s:%d",aciparms.ip_address,aciparms.ip_port);
	 else
		sprintf(cardspec, "s3:%s:%d/%s",aciparms.ip_address,aciparms.ip_port,aciparms.card_key);
	 break;
 default:
	 printf("Unsupported card type %d\n", aciparms.card_type);
	 return 1;
 }

 cardlist(&cards);
 if (arg.serialnumber) {
	err_t err = cardlist_addcard(&cards, cardspec);
	if (err) {
		error_log(stderr, err);
		return 1;
	}
 }
 if (!cards.ncard) {
	fprintf(stderr, "No cards specifed\n");
	return 1;
 }
	
 e = detect(ocparms.card_id, cdiparms.ports, &cards, arg.timeslot, arg.toneset, arg.grunt, arg.map, arg.cptone, arg.tonemode, tonetype, arg.per_mod, arg.catsig_alg);
// close things
 {
	// call
 	ACU_CLOSE_CALL_PARMS parms;
 	INIT_ACU_STRUCT(&parms);
 	parms.card_id = ocparms.card_id;
 	acu_close_call(&parms);	
 }
 {
	// switch
 	ACU_CLOSE_SWITCH_PARMS parms;
 	INIT_ACU_STRUCT(&parms);
 	parms.card_id = ocparms.card_id;
 	acu_close_switch(&parms);
 }
 {
 	ACU_CLOSE_CARD_PARMS ccparms;
 	INIT_ACU_STRUCT(&ccparms);
 	ccparms.card_id = ocparms.card_id;
 	acu_close_card(&ccparms);
 }
 cardlist_dtor(&cards);
 return e;
}
#if 0
 TONE_TYPE tonetype = TONE_TYPE_START;
 CARDLIST cards;
 int e;
 ARGS_DECL
 (void) argc;
 arg.grunt = ~0;
 if (ARGS_CALL || *argv) {
usage:
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
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
 if (arg.grunt == ~0U && !arg.cptone && !arg.tonemode && !arg.catsig_alg) {
	printf("Warning: no detection selected\n");
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
 {
	// open our card
	ACU_ERR AERR;
	ACU_OPEN_CARD_PARMS ocparms;
	ACU_OPEN_SWITCH_PARMS osparms;
	ACU_OPEN_CALL_PARMS oclparms;
	CARD_INFO_PARMS cdiparms;

	INIT_ACU_STRUCT(&ocparms);
	strcpy(ocparms.serial_no,arg.serialnumber);
	AERR = acu_open_card(&ocparms);
	if (AERR)
	{
		printf("failed to open card %d\n",AERR);
		return AERR;
	}
	// open switch
	INIT_ACU_STRUCT(&osparms);
	osparms.card_id = ocparms.card_id;
	AERR = acu_open_switch(&osparms);
	if (AERR)
	{
		printf("failed to open switch %d\n",AERR);
		return AERR;
	}
	// open call
	INIT_ACU_STRUCT(&oclparms);
	oclparms.card_id = ocparms.card_id;
	AERR = acu_open_call(&oclparms);
	if (AERR)
	{
		printf("failed to open call %d\n",AERR);
		return AERR;
	}
	// ok lets see what we haveon this card
	INIT_ACU_CL_STRUCT(&cdiparms);
	cdiparms.card_id = ocparms.card_id;
	AERR = call_get_card_info(&cdiparms);
	if (AERR)
	{
		printf("failed to get card info, %d\n",AERR);
		return AERR;
	}
	e = detect(ocparms.card_id, cdiparms.ports, &cards, arg.timeslot, arg.toneset, arg.grunt, arg.map, arg.cptone, arg.tonemode, tonetype, arg.per_mod, arg.catsig_alg);
	// close things
	{
		// call
		ACU_CLOSE_CALL_PARMS parms;
		INIT_ACU_STRUCT(&parms);
		parms.card_id = ocparms.card_id;
		acu_close_call(&parms);	
	}
	{
		// switch
		ACU_CLOSE_SWITCH_PARMS parms;
		INIT_ACU_STRUCT(&parms);
		parms.card_id = ocparms.card_id;
		acu_close_switch(&parms);
	}
 }
 return e;
}
#endif
