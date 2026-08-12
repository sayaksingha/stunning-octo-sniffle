/* ansrec.c - test/demo of answering incoming calls and recording a message */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/timeb.h>

#include "../../libutil/generic_io.h"
#include "bfopen.h"
#include "../Testlib/cardlist.h"
#include "../Testlib/checkrec.h"
#include "../Testlib/fmtcode.h"
#include "../Testlib/cardlist.h"
#include "../Testlib/v6.h"
#include "smbesp.h"
#include "smdrvr.h"
#include "../Testlib/watchl1.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

#include <pthread.h>

#define TiNG_PROSODYS_PORTNO_ENVVARNAME "ACULAB_PROSODYS_PORT"


typedef struct {
	int stream;
	int timeslot;
	int type;
} MVIP;
#define MVIP_STREAM_NONE 0xff

static struct l1 {
	struct l1 *next;
	pthread_t thread;
	LAYER1 l1port;
} *l1s;

static void *checkl1(void *vp)
{
 struct l1 *l1 = vp;
 return (void *) watchl1(&l1->l1port);
}

static struct chan {
	struct chan *next;
	pthread_t thread;
	int stop;
	tSMModuleId module;
	unsigned sil_elim;
	unsigned max_silence;
	enum kSMDataFormat format;
	tSM_UT32 rate;
	char *file;
	TEST_CALL_TYPE handle;
	unsigned port, slot;
	MVIP xts;
	char dbgname[32];
} *chans;

static int alloc_chan(tSMChannelId *chanp, struct chan *cp)
{
 struct sm_channel_alloc_placed_parms ap;
 int err;
 memset(&ap, 0, sizeof(ap));
 ap.type = kSMChannelTypeInput;
 ap.module = cp->module;
 err = sm_channel_alloc_placed(&ap);
 if (err) {
 	fprintf(stderr, "%s: sm_channel_alloc_placed returned %d\n", cp->dbgname, err);
 	return 1;
 }
 *chanp = ap.channel;
 return 0;
}

static int switch_chan(tSMChannelId chan, struct chan *cp)
{
 struct sm_switch_channel_parms sw;
 struct output_parms op;
 int swdrvr;
 int err;
 memset(&sw, 0, sizeof(sw));
 sw.channel = chan;
 sw.st = cp->xts.stream;
 sw.ts = cp->xts.timeslot;
 sw.type = cp->xts.type;
 err = sm_switch_channel_input(&sw);
 if (err) {
	fprintf(stderr, "%s: sm_switch_channel_input returned %d\n",
		cp->dbgname, err);
	return 1;
 }
 swdrvr = call_port_2_swdrvr(cp->port);
 if (swdrvr < 0) {
	fprintf(stderr, "%s: call_port_2_swdrvr returned %d\n",
		cp->dbgname, swdrvr);
	return 1;
 }
 INIT_ACU_SW_STRUCT(&op)
 op.ist = call_port_2_stream(cp->port);
 if (op.ist < 0) {
	fprintf(stderr, "%s: call_port_2_stream returned %d\n",
		cp->dbgname, op.ist);
	return 1;
 }
 op.its = cp->slot;
 op.mode = CONNECT_MODE;
 op.ost = cp->xts.stream;
 op.ots = cp->xts.timeslot;
 printf("%d.%d <- %d.%d\n", op.ist, op.its, op.ost, op.ots);
 err = sw_set_output(swdrvr, &op);
 if (err) {
	fprintf(stderr, "%s: sw_set_output returned %d\n",
		cp->dbgname, err);
	return 1;
 }
 return 0;
}

static int get_record_event(tSMEventId *evp, tSMChannelId chan, struct chan *cp)
{
 SM_CHANNEL_SET_EVENT_PARMS sep;
 int err = smd_ev_create(evp, chan,
 		kSMEventTypeReadData, kSMChannelSpecificEvent);
 if (err) {
	fprintf(stderr, "%s: smd_ev_create returned %d\n",
		cp->dbgname, err);
	return 1;
 }
 memset(&sep, 0, sizeof(sep));
 sep.event = *evp;
 sep.channel = chan;
 sep.event_type = kSMEventTypeReadData;
 sep.issue_events = kSMChannelSpecificEvent;
 err = sm_channel_set_event(&sep);
 if (err) {
	fprintf(stderr, "%s: sm_channel_set_event returned %d\n",
		cp->dbgname, err);
	smd_ev_free(*evp);
	return 1;
 }
 return 0;
}

static int start_record(tSMChannelId chan, struct chan *cp)
{
 struct sm_record_parms rp;
 int err;
 memset(&rp, 0, sizeof(rp));
 rp.channel = chan;
 rp.type = cp->format;
 rp.sampling_rate = cp->rate;
 rp.silence_elimination = cp->sil_elim;
 rp.max_silence = cp->max_silence;
 err = sm_record_start(&rp);
 if (err) {
	fprintf(stderr, "%s: sm_record_start returned %d\n", cp->dbgname, err);
	return 1;
 }
 return 0;
}

static int free_ev(tSMEventId ev, struct chan *cp)
{
 int err = smd_ev_free(ev);
 if (err) {
	fprintf(stderr, "%s: smd_ev_free returned %d\n", cp->dbgname, err);
	return 1;
 }
 return 0;
}

static int unswitch_chan(struct chan *cp)
{
 struct output_parms op;
 int swdrvr = call_port_2_swdrvr(cp->port);
 int err;
 if (swdrvr < 0) {
	fprintf(stderr, "%s: call_port_2_swdrvr returned %d\n",
		cp->dbgname, swdrvr);
	return 1;
 }
 INIT_ACU_SW_STRUCT(&op);
 op.ost = cp->xts.stream;
 op.ots = cp->xts.timeslot;
 op.mode = DISABLE_MODE;
 err = sw_set_output(swdrvr, &op);
 if (err) {
 	fprintf(stderr, "%s: sw_set_output returned %d\n", cp->dbgname, err);
 	return 1;
 }
 return 0;
}

static int unalloc_chan(tSMChannelId chan, struct chan *cp)
{
 int err = sm_channel_release(chan);
 if (err) {
	fprintf(stderr, "%s: sm_channel_release returned %d\n",
		cp->dbgname, err);
	return 1;
 }
 return 0;
}

static int do_record(RECORDING *rec, tSMEventId ev, struct chan *cp, struct state_xparms *cxp)
{
 unsigned prevbytes = 0;
 struct timeb start;
 unsigned twid = 0;
 int running = 1;
 ftime(&start);
 for (;;) {
	/* Note that for Prosody version 2 (TiNG) we could start by
	 * waiting on the event as it'll be set continuously while the
	 * channel has data or status to report. Version 1, however, will not
	 * set the event until the channel changes status, so it is safer
	 * to check it first.
 	 */
	int err = checkrec(rec);
	if (err) return 1;
	if (rec->done) {
		printf("done\n");
		return 0;
	}
	if (running) {
		struct timeb now;
		unsigned nbytes = rec->bytes - prevbytes;
		double dtim;
		ftime(&now);
		dtim = now.time - start.time
			+ (now.millitm - start.millitm)
			/ 1000.0;
		if (!dtim) dtim = 0.001;
		printf("\r%c %d  %d +/- %d bps  ",
			"|/-\\"[twid++ & 3], rec->bytes,
			(int) ((8 * rec->bytes + 4 * nbytes) / dtim),
			(int) (4 * nbytes / dtim));
		prevbytes = rec->bytes;
		fflush(stdout);
		cxp->timeout = 0;
		cxp->handle = cp->handle;
		err = call_event(cxp);
		if (err) {
			fprintf(stderr, "%s: call_state returned %s\n", cp->dbgname, error_2_string(err));
			return 1;
		}
		if (cxp->handle)
		{
			if (cxp->state != EV_CALL_CONNECTED) {
				SM_RECORD_ABORT_PARMS rp;
				memset(&rp, 0, sizeof(rp));
				rp.channel = rec->chan;
				err = sm_record_abort(&rp);
				if (err) {
					fprintf(stderr, "%s: sm_record_abort returned %d\n", cp->dbgname, err);
					return 1;
				}
				printf("abandoning\n");
				running = 0;
			}
		}
	} else {
		printf("\r%c %d  ", ".*"[twid++ & 1], rec->bytes);
		fflush(stdout);
	}
	err = smd_ev_wait(ev);
	if (err) {
		fprintf(stderr, "%s: smd_ev_wait returned %d\n", cp->dbgname, err);
		return 1;
	}
 }
}

static int recmsg(struct chan *cp, struct state_xparms *cxp)
{
 struct detail_xparms xp;
 RECORDING rec;
 int err;
 rec.done = 0;
 rec.bytes = 0;
 INIT_ACU_CL_STRUCT(&xp)
 xp.handle = cp->handle;
 xp.timeout = 0;
 err = call_details(&xp);
 if (err) {
	fprintf(stderr, "%s: call_details returned %s\n", cp->dbgname, error_2_string(err));
	return 1;
 }
 if (!xp.valid) {
	printf("%s: details not valid\n", cp->dbgname);
	return 1;
 }
 rec.dbgname = cp->dbgname;
 err = bfile(&rec.bf);
 if (err) {
	fprintf(stderr, "%s: bfile() failed: %d: %s\n",
		rec.dbgname, err, strerror(err));
	return 1;
 }
 err = bfopen(rec.bf, cp->file, "wbct");
 if (err) {
	fprintf(stderr, "%s: bfopen(%s) failed: %d: %s\n",
		rec.dbgname, cp->file, err, strerror(err));
	bfile_dtor(rec.bf);
	return 1;
 }
 err = alloc_chan(&rec.chan, cp);
 if (!err) {
 	err = switch_chan(rec.chan, cp);
 	if (!err) {
		tSMEventId ev;
		err = get_record_event(&ev, rec.chan, cp);
		if (!err) {
			err = start_record(rec.chan, cp);
			if (!err) err = do_record(&rec, ev, cp, cxp);
			err |= free_ev(ev, cp);
		}
		err |= unswitch_chan(cp);
 	}
	err |= unalloc_chan(rec.chan, cp);
 }
 err |= bfile_dtor(rec.bf);
 return err;
}

static int showdetails(struct chan *cp)
{
 struct detail_xparms xp;
 int err;
 INIT_ACU_CL_STRUCT(&xp)

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
	 INIT_ACU_CL_STRUCT(&inx)
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
		INIT_ACU_CL_STRUCT(&stx)
		stx.handle = cp->handle;
		stx.timeout = 1000;
		err = call_event(&stx);
		if (err) {
			fprintf(stderr, "%d.%d: call_state returned %s\n", cp->port, cp->slot, error_2_string(err));
			return (void *) 1;
		}
		if(!stx.handle) continue;
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
			if (recmsg(cp, &stx)) return (void *) 1;
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
	 memset(&xp, 0, sizeof(xp));
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

static int ansrec(ACU_CARD_ID card, unsigned numports, CARDLIST *cardsp, MVIP ts, unsigned max_silence, unsigned sil_elim, enum kSMDataFormat format, unsigned rate, unsigned per_mod, char *file)
{
	ACU_PORT_ID *ports = 0;
	unsigned port;
	unsigned u;
	int err;
 unsigned curcardpos = 0;
 unsigned curmodpos = 0;
 unsigned thismod = 0;
 if (!per_mod) per_mod = 64;
 printf("ansrec..... numports is %d\n", numports);
 for (port=0; port < numports ; port++) {
	struct l1 *l1p = malloc(sizeof(*l1p));
	unsigned nts=0;
	unsigned mask;
	unsigned slot;
	int portid;
	 ACU_PORT_ID *np = realloc(ports, (port + 1) * sizeof(*ports));
	 // open the port	
	 OPEN_PORT_PARMS parms;
	 PORT_INFO_PARMS pinfo;
	 ACU_ERR AERR;
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
	 portid = ports[port]=parms.port_id;
	if (!l1p) {
		perror("malloc");
		fprintf(stderr, "Cannot allocate l1 struct\n");
		return 1;
	}
	sprintf(l1p->l1port.dbgname, "Port %d", port);
	l1p->l1port.stop = 0;
	l1p->l1port.net = portid;	
	//l1p->l1port.net = ports[port];
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
			sprintf(cp->dbgname, "%d.%d", port, slot);
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
			cp->sil_elim = sil_elim;
			cp->max_silence = max_silence;
			cp->file = file;
			cp->format = format;
			cp->rate = rate;
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
			l1p->l1port.stop = 1;
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
		
		for (u=0;u<numports;u++)
		{
			CLOSE_PORT_PARMS clpparms;
			INIT_ACU_CL_STRUCT(&clpparms);
			clpparms.port_id=ports[u];
			call_close_port(&clpparms);
		}
		return 0;
 	}
 }
}

#include "gen/ansrec.args.i"


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
 enum kSMDataFormat format;
 tSM_UT32 rate;
 int e;
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
	
 e = ansrec(ocparms.card_id, cdiparms.ports, &cards, arg.timeslot, arg.max_silence, arg.sil_elim, format, rate, arg.per_mod, *argv);
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
