/* incall.c - test/demo of answering incoming calls */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../test/Testlib/v6.h"

static struct chan {
	struct chan *next;
	pthread_mutex_t mx;
	int stop;
	pthread_t thread;
	ACU_INT handle;
	TEST_PORT_TYPE port;
	unsigned ts;
	char id[32];
} *chans;

static int showdetails(unsigned port, unsigned ts, ACU_INT handle)
{
 struct detail_xparms xp;
 int err;
 INIT_ACU_CL_STRUCT(&xp);
 xp.handle = handle;
 xp.timeout = 0;
 err = call_details(&xp);
 if (err) {
	fprintf(stderr, "%d.%d: call_details returned %s\n", port, ts, error_2_string(err));
	return 1;
 }
 if (!xp.valid) {
	printf("details not valid\n");
	return 0;
 }
 printf("%d.%d: @%d.%d", port, ts, xp.stream, xp.ts);
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
 for (;;) {
	int stop = 0;
	int err;
	struct in_xparms inx;
	INIT_ACU_CL_STRUCT(&inx);
	inx.net = cp->port;
	inx.ts = cp->ts;
	err = call_openin(&inx);
	cp->handle = inx.handle;
	if (err) {
		fprintf(stderr, "%s: call_openin returned %s\n", cp->id, error_2_string(err));
		return (void *) 1;
	}
	for (;;) {
		struct state_xparms stx;
		err = pthread_mutex_lock(&cp->mx);
		if (err) {
			fprintf(stderr, "pthread_mutex_lock failed: %s\n", strerror(err));
			return (void *) 1;
		}
		stop = cp->stop;
		err = pthread_mutex_unlock(&cp->mx);
		if (err) {
			fprintf(stderr, "pthread_mutex_unlock failed: %s\n", strerror(err));
			return (void *) 1;
		}
		if (stop) break;

		INIT_ACU_CL_STRUCT(&stx);
		stx.handle = cp->handle;
		stx.timeout = 1000;
		err = call_event(&stx);
		if (err) {
			fprintf(stderr, "%s: call_state returned %s\n", cp->id, error_2_string(err));
			return (void *) 1;
		}
		if (!stx.handle) continue;
		switch (stx.state) {
			struct cause_xparms cxp;
		case EV_WAIT_FOR_INCOMING:
			fprintf(stderr, "%s: EV_WAIT_FOR_INCOMING\n", cp->id);
			break;
		case EV_INCOMING_CALL_DET:
			fprintf(stderr, "%s: EV_INCOMING_CALL_DET\n", cp->id);
			if (showdetails(cp->port, cp->ts, stx.handle)) return (void *) 1;
			err = call_accept(stx.handle);
			if (err) {
				fprintf(stderr, "%s: call_accept returned %s\n", cp->id, error_2_string(err));
				return (void *) 1;
			}
			break;
		case EV_CALL_CONNECTED:
			fprintf(stderr, "%s: EV_CALL_CONNECTED\n", cp->id);
			break;
		case EV_WAIT_FOR_OUTGOING:
			fprintf(stderr, "%s: EV_WAIT_FOR_OUTGOING\n", cp->id);
			break;
		case EV_OUTGOING_RINGING:
			fprintf(stderr, "%s: EV_OUTGOING_RINGING\n", cp->id);
			break;
		case EV_REMOTE_DISCONNECT:
			fprintf(stderr, "%s: EV_REMOTE_DISCONNECT\n", cp->id);
			goto endcall;
		case EV_WAIT_FOR_ACCEPT:
			fprintf(stderr, "%s: EV_WAIT_FOR_ACCEPT\n", cp->id);
			break;
		case EV_IDLE:
			INIT_ACU_CL_STRUCT(&cxp);
			cxp.handle = cp->handle;
			err=call_getcause(&cxp);
			if (err) {
				fprintf(stderr, "%s: call_getcause returned %s\n", cp->id, error_2_string(err));
			}
			fprintf(stderr, "%s: EV_IDLE %02x (%d=", cp->id, cxp.raw, cxp.cause);
			switch (cxp.cause) {
			case LC_NORMAL: fprintf(stderr, "NORMAL"); break;
			case LC_NUMBER_BUSY: fprintf(stderr, "NUMBER_BUSY"); break;
			case LC_NO_ANSWER: fprintf(stderr, "NO_ANSWER"); break;
			case LC_NUMBER_UNOBTAINABLE: fprintf(stderr, "NUMBER_UNOBTAINABLE"); break;
			case LC_NUMBER_CHANGED: fprintf(stderr, "NUMBER_CHANGED"); break;
			case LC_OUT_OF_ORDER: fprintf(stderr, "OUT_OF_ORDER"); break;
			case LC_INCOMING_CALLS_BARRED: fprintf(stderr, "INCOMING_CALLS_BARRED"); break;
			case LC_CALL_REJECTED: fprintf(stderr, "CALL_REJECTED"); break;
			case LC_CALL_FAILED: fprintf(stderr, "CALL_FAILED"); break;
			case LC_CHANNEL_BUSY: fprintf(stderr, "CHANNEL_BUSY"); break;
			case LC_NO_CHANNELS: fprintf(stderr, "NO_CHANNELS"); break;
			case LC_CONGESTION: fprintf(stderr, "CONGESTION"); break;
			default: fprintf(stderr, "?"); break;
			}
			fprintf(stderr, ")\n");
			goto endcall;
		case EV_HOLD:
			fprintf(stderr, "%s: EV_HOLD\n", cp->id);
			break;
		case EV_HOLD_REJECT:
			fprintf(stderr, "%s: EV_HOLD_REJECT\n", cp->id);
			break;
		case EV_TRANSFER_REJECT:
			fprintf(stderr, "%s: EV_TRANSFER_REJECT\n", cp->id);
			break;
		case EV_RECONNECT_REJECT:
			fprintf(stderr, "%s: EV_RECONNECT_REJECT\n", cp->id);
			break;
		case EV_PROGRESS:
			fprintf(stderr, "%s: EV_PROGRESS\n", cp->id);
			break;
		case EV_OUTGOING_PROCEEDING:
			fprintf(stderr, "%s: EV_OUTGOING_PROCEEDING\n", cp->id);
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
		fprintf(stderr, "%s: call_release returned %s\n", cp->id, error_2_string(err));
		return (void *) 1;
	}
	if (stop) break;
 }
 return 0;
}

static int incall_ts(TEST_PORT_TYPE port, unsigned portno, unsigned ts)
{
 struct chan *cp = malloc(sizeof(*cp));
 int err;
 if (!cp) {
	perror("malloc");
	fprintf(stderr, "Cannot allocate channel struct\n");
	return 1;
 }
 cp->stop = 0;
 cp->port = port;
 cp->ts = ts;
 sprintf(cp->id, "%d.%d", portno, ts);
 err = pthread_mutex_init(&cp->mx, 0);
 if (err) {
	fprintf(stderr, "pthread_mutex_init failed: %s\n", strerror(err));
	return 1;
 }
 err = pthread_create(&cp->thread, 0, chanfunc, cp);
 if (err) {
	fprintf(stderr, "pthread_create failed: %s\n", strerror(err));
	return 1;
 }
 cp->next = chans;
 chans = cp;
 return 0;
}

static unsigned char *numlist(char *s, unsigned limit)
{
 unsigned char *set = malloc(limit);
 if (!set) {
	perror("malloc() failed");
	fprintf(stderr, "Cannot allocate %d bytes for list\n", limit);
	return 0;
 }
 if (!s) {
	memset(set, 1, limit);
	return set;
 }
 memset(set, 0, limit);
 for (;;) {
	char *ns = s;
	unsigned minval = strtol(s, &ns, 0);
	unsigned maxval;
	if (*ns == '-') {
		if (*++ns == '*') ns++, maxval = limit-1;
		else maxval = strtol(ns, &ns, 0);
	} else maxval = minval;
	if (maxval >= limit) maxval = limit-1;
	while (minval <= maxval) set[minval++] = 1;
	if (!*ns) return set;
	if (*ns != ',') {
		fprintf(stderr, "Junk after number: '%s' in '%s'\n", ns, s);
		fprintf(stderr, "Expected range list: N,N-N,N-* etc\n");
		free(set);
		return 0;
	}
	s = ns;
 }
}
static int incall_port(TEST_PORT_TYPE port, unsigned portno, unsigned char *tslist)
{
	// find our sigsys mask
	ACU_ERR AERR;
	PORT_INFO_PARMS pinfo;
    unsigned nts=0;
    unsigned ts;
    unsigned mask;
    int err = 0;
	INIT_ACU_CL_STRUCT(&pinfo);
	pinfo.port_id=port;
    AERR = call_port_info(&pinfo);
	if(AERR)
	{
		printf("call_port_info returned %d\n",AERR);
		return 0;
	}
	for (ts=0, mask = pinfo.valid_vector; mask; ts++, mask >>=1) {
	if (tslist[ts]) 
	{
		if (mask & 1) 
		{
			err = incall_ts(port, portno, ts);
			if (err) break;
			nts++;
		} else {
			printf("%d.%d: invalid timeslot - ignored\n", portno, ts);
		}
	}
 }
 printf("Started %d timeslots on port %d: %s\n", nts, portno, pinfo.sig_sys);
 return err;
}

static int stopthreads(void)
{
 while (chans) {
	struct cause_xparms xp;
	struct chan *nx;
	void *sts;
	int err;
	INIT_ACU_CL_STRUCT(&xp);
	err = pthread_mutex_lock(&chans->mx);
	if (err) {
		fprintf(stderr, "pthread_mutex_lock failed: %s\n", strerror(err));
		return 1;
	}
	chans->stop = 1;
	err = pthread_mutex_unlock(&chans->mx);
	if (err) {
		fprintf(stderr, "pthread_mutex_unlock failed: %s\n", strerror(err));
		return 1;
	}
	xp.handle = chans->handle;
	xp.cause = LC_NORMAL;
	err=call_disconnect(&xp);
	if (err) {
		fprintf(stderr, "%s: call_disconnect returned %s\n", chans->id, error_2_string(err));
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
 return 0;
}

static int handle_kbd(void)
{
 for (;;) {
	int c = getchar();
	switch (c) {
	case '\n': break;
	case 'q':
	case EOF:
		return 0;
	}
 }
}

static int incall(ACU_CARD_ID card, char *ports, char *timeslots)
{
 ACU_PORT_ID portids[8];
 CARD_INFO_PARMS ciparms;
 unsigned numport;
 unsigned char *pset;
 unsigned char *tslist;
 unsigned port;
 int err = 0;
 ACU_ERR AERR;
 INIT_ACU_CL_STRUCT(&ciparms);
 ciparms.card_id=card;
 AERR = call_get_card_info(&ciparms);
 if (AERR)
 {
	 fprintf(stderr,"problem calling call_get_card_info %d\n",AERR);
	 return 1;
 }
 numport = ciparms.ports;
 pset = numlist(ports, numport);

 if (!pset) {
	fprintf(stderr, "Cannot set up port list\n");
	return 1;
 }
 tslist = numlist(timeslots, 64 /* max bits in validvector */);
 if (!tslist) {
	free(pset);
	fprintf(stderr, "Cannot set up timeslot list\n");
	return 1;
 }
 for (port=0; port < numport; port++) {
	if (pset[port]) {
		// open the port
		OPEN_PORT_PARMS opparms;
		INIT_ACU_CL_STRUCT(&opparms);
		opparms.card_id=card;
		opparms.port_ix=port;
		AERR = call_open_port(&opparms);
		if (AERR)
		{
			fprintf(stderr,"problem calling call_open_port %d\n",AERR);
			return 1;
		}
		portids[port] = opparms.port_id;
		err = incall_port(opparms.port_id, port, tslist);
		if (err) break;
	}
 }
 if (!err) handle_kbd();
 printf("exitting\n");
 if (stopthreads()) err = 1;
 // close our ports
 for (port=0; port < numport; port++) 
 {
	if (pset[port]) 
	{
		// close the port
		CLOSE_PORT_PARMS cpparms;
		INIT_ACU_CL_STRUCT(&cpparms);
		cpparms.port_id=portids[port];
		AERR = call_close_port(&cpparms);
		if (AERR)
		{
			fprintf(stderr,"problem calling call_close_port %d\n",AERR);
			return 1;
		}
	}
 }
 free(tslist);
 free(pset);
 return err;
}

static int run_incall(char *serialnumber, char *ports, char *timeslots)
{
 ACU_CARD_ID card_id;
 int iErr;
 {
	ACU_OPEN_CARD_PARMS ocparms;
	ACU_OPEN_CALL_PARMS oclparms;
	ACU_ERR AERR;
	INIT_ACU_STRUCT(&ocparms);
	strcpy(ocparms.serial_no, serialnumber);
	AERR = acu_open_card(&ocparms);
	if (AERR)
	{
		printf("failed to open card %d\n",AERR);
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
	card_id = ocparms.card_id;
 }
	// call function
	iErr =  incall(card_id, ports, timeslots);
	// close things
 {
	// call
	ACU_CLOSE_CALL_PARMS parms;
	INIT_ACU_STRUCT(&parms);
	parms.card_id = card_id;
	acu_close_call(&parms);	
 }
 {
	//card
	ACU_CLOSE_CARD_PARMS ccparms;
	INIT_ACU_STRUCT(&ccparms);
	ccparms.card_id = card_id;
	acu_close_card(&ccparms);
 }
	return iErr;
}

#include "gen/incall.args.i"

int main(int argc, char **argv)
{
 ARGS_DECL
 (void) argc;	
 if (ARGS_CALL || *argv) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE " file\n", progname);
	return 1;
 }
 if (!arg.serialnumber) {
	fprintf(stderr, "%s: serial-number required\n", progname);
	return 1;
 }
 return run_incall(arg.serialnumber, arg.portlist, arg.timeslotlist);
}
