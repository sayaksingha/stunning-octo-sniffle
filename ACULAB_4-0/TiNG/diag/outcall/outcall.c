/* outcall.c - test/demo of dialling a call */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../test/Testlib/v6.h"

#define arlen(s) (sizeof(s)/sizeof(*(s)))

typedef struct {
	int stream;
	int timeslot;
	int type;
} MVIP;
#define MVIP_STREAM_NONE 0xff

struct chan {
	int stop;
	pthread_t thread;
	struct out_xparms *outx;
	char id[20];
};

static int showdetails(char *id, TEST_CALL_TYPE handle)
{
 struct detail_xparms xp;
 int err;
 INIT_ACU_CL_STRUCT(&xp);
 xp.handle = handle;
 xp.timeout = 0;
 err = call_details(&xp);
 if (err) {
	fprintf(stderr, "%s: call_details returned %s\n", id, error_2_string(err));
	return 1;
 }
 if (!xp.valid) {
	printf("details not valid\n");
	return 0;
 }
 printf("%s: @%d.%d", id, xp.stream, xp.ts);
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
	int err = call_openout(cp->outx);
	if (err) {
		fprintf(stderr, "%s: call_openout returned %s\n", cp->id, error_2_string(err));
		return (void *) 1;
	}
	for (;;) {
		struct state_xparms stx;
		INIT_ACU_CL_STRUCT(&stx);
		stx.handle = cp->outx->handle;
		stx.timeout = 1000;
		err = call_event(&stx);
		if (err) {
			fprintf(stderr, "%s: call_state returned %s\n", cp->id, error_2_string(err));
			return (void *) 1;
		}
		if (!stx.handle) continue;
		switch (stx.state) {
		case EV_WAIT_FOR_INCOMING:
			fprintf(stderr, "%s: EV_WAIT_FOR_INCOMING\n", cp->id);
			break;
		case EV_INCOMING_CALL_DET:
			fprintf(stderr, "%s: EV_INCOMING_CALL_DET\n", cp->id);
			if (showdetails(cp->id, stx.handle)) return (void *) 1;
			break;
		case EV_CALL_CONNECTED:
			if (showdetails(cp->id, stx.handle)) return (void *) 1;
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
			{
			 struct cause_xparms cxp;
			 INIT_ACU_CL_STRUCT(&cxp);
			 cxp.handle = stx.handle;
			 cxp.cause = LC_NORMAL;
			 err = call_disconnect(&cxp);
			if (err) {
				fprintf(stderr, "%s: call_disconnect returned %s\n", cp->id, error_2_string(err));
				return (void *) 1;
			}
			}
			break;
		case EV_WAIT_FOR_ACCEPT:
			fprintf(stderr, "%s: EV_WAIT_FOR_ACCEPT\n", cp->id);
			break;
		case EV_IDLE:
			fprintf(stderr, "%s: EV_IDLE\n", cp->id);
			cp->stop = 1;
			// clear connection
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
	 xp.handle = cp->outx->handle;
	 xp.cause = LC_NORMAL;
	 err=call_release(&xp);
 	}
	if (err) {
		fprintf(stderr, "%s: call_release returned %s\n", cp->id, error_2_string(err));
		return (void *) 1;
	}
 }
 return 0;
}

static int outcall(unsigned portno, struct out_xparms *outx)
{
 struct chan chan;
 int err;
 chan.stop = 0;
 chan.outx = outx;
 sprintf(chan.id, "%d.%d", portno, outx->ts);
 err = pthread_create(&chan.thread, 0, chanfunc, &chan);
 if (err) {
	fprintf(stderr, "pthread_create failed: %s\n", strerror(err));
	return 1;
 }
 for (;;) {
 	int c = getchar();
 	switch (c) {
 	case '\n': break;
 	case 'q':
 	case EOF: {
		struct cause_xparms xp;
		void *sts;
		printf("exitting\n");
		INIT_ACU_CL_STRUCT(&xp);
		
		xp.handle = chan.outx->handle;
		xp.cause = LC_NORMAL;
		if (!chan.stop) {
			chan.stop = 1;
			err=call_disconnect(&xp);
			if (err) {
				fprintf(stderr, "%s: call_disconnect returned %s\n", chan.id, error_2_string(err));
				return 1;
			}
		}
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

static ACU_ERR V6OpenCard(V6RESOURCES* resources, char* serial_num)
{
	ACU_ERR err;
	ACU_OPEN_CARD_PARMS parms;
	memset(resources,0,sizeof(*resources));
	INIT_ACU_STRUCT(&parms);
	strcpy(parms.serial_no,serial_num);
	err = acu_open_card(&parms);
	if (err)
	{
		fprintf(stderr, "failed to open card '%s': %d\n",
			serial_num, err);
		return err;
	}
	resources->card = parms.card_id;
	return 0;
}

	// last thing
static ACU_ERR V6CloseCard(V6RESOURCES* resources)
{
	ACU_ERR err;
	ACU_CLOSE_CARD_PARMS parms;
	INIT_ACU_STRUCT(&parms);
	parms.card_id = resources->card;
	err = acu_close_card(&parms);
	if (err)
	{
		fprintf(stderr, "failed to close card, err = %d\n",err);
		return err;
	}
	return 0;
}

static ACU_ERR V6OpenCall(V6RESOURCES* resources)
{
	ACU_ERR err;
	ACU_OPEN_CALL_PARMS parms;
	INIT_ACU_STRUCT(&parms);
	parms.card_id = resources->card;
	err = acu_open_call(&parms);
	if (err)
	{
		fprintf(stderr, "failed to open call: %d\n",err);
		return err;
	}
	return 0;
}

static ACU_ERR V6CloseCall(V6RESOURCES* resources)
{
	ACU_ERR err;
	ACU_CLOSE_CALL_PARMS parms;
	INIT_ACU_STRUCT(&parms);
	parms.card_id = resources->card;
	err = acu_close_call(&parms);
	if (err)
	{
		fprintf(stderr, "failed to close call, err = %d\n",err);
		return err;
	}
	return 0;
}

static ACU_ERR V6OpenPort(V6RESOURCES* resources, int port_number)
{
	ACU_ERR err;
	OPEN_PORT_PARMS parms;
	INIT_ACU_CL_STRUCT(&parms);
	parms.card_id = resources->card;
	parms.port_ix = port_number;
	err = call_open_port(&parms);
	if (err)
	{
		fprintf(stderr, "faied to open port %d, err = %d\n",port_number,err);
		return err;
	}
	resources->port = parms.port_id;
	return 0;
}

static ACU_ERR V6ClosePort(V6RESOURCES* resources)
{
	ACU_ERR err;
	CLOSE_PORT_PARMS parms;
	INIT_ACU_CL_STRUCT(&parms);
	parms.port_id = resources->port;
	err = call_close_port(&parms);
	if (err)
	{
		fprintf(stderr, "failed to close port, err = %d\n",err);
		return err;
	}
	return 0;
}

#include "gen/outcall.args.i"

int main(int argc, char **argv)
{
 V6RESOURCES resources;
 int iErr;
 struct out_xparms outx;
 int nettype;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || !*argv || argv[1]) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE " phone-number\n", progname);
	return 1;
 }
 if (!arg.serialnumber) {
	fprintf(stderr, "%s: serial-number required\n", progname);
	return 1;
 }
 if (V6OpenCard(&resources,arg.serialnumber)) return 1;
 if (V6OpenCall(&resources)) return 1;
 if (V6OpenPort(&resources,arg.port)) return 1;
 INIT_ACU_CL_STRUCT(&outx);
 outx.net = resources.port;
 outx.ts = arg.timeslot;
 if (arg.remdiscwait) outx.cnf |= CNF_REM_DISC;
 if (arg.charge) outx.cnf |= CNF_CALL_CHARGE;
 if (arg.timeslot_preferred) outx.cnf |= CNF_TSPREFER;
 if (arg.sending_complete) outx.cnf |= CNF_COMPLETE;
 outx.sending_complete = arg.sending_complete;
 if (strlen(*argv) >= arlen(outx.destination_addr)) {
	fprintf(stderr, "Number too long at %ld, max %ld: '%s'\n",
		(long) strlen(*argv),
		(long) (arlen(outx.destination_addr)-1),
		*argv);
	return 1;
 }
 strcpy(outx.destination_addr, *argv);
 if (arg.origin_addr) {
	if (strlen(arg.origin_addr) >= arlen(outx.originating_addr)) {
		fprintf(stderr, "Number too long at %ld, max %ld: '%s'\n",
			(long) strlen(arg.origin_addr),
			(long) (arlen(outx.originating_addr)-1),
			arg.origin_addr);
		return 1;
	}
 	strcpy(outx.originating_addr, arg.origin_addr);
 }
 nettype = call_type(outx.net);
 switch (nettype) {
 case S_1TR6:
 case S_1TR6NET:
#if 0
	outx.unique_xparms.sig_1tr6.numbering_type = NT_NATIONAL;
	outx.unique_xparms.sig_1tr6.numbering_plan = NP_ISDN;
	outx.unique_xparms.sig_1tr6.service_octet  = 0;
	outx.unique_xparms.sig_1tr6.add_info_octet = 0;
#endif
	break;
 case S_ETS300:
 case S_ETSNET:
 case S_VN3:
 case S_VN3NET:
 case S_AUSTEL:
 case S_AUSTNET:
 case S_TNA_NZ:
 case S_TNANET:
 case S_FETEX_150:
 case S_FETEXNET:
 case S_ATT:
 case S_ATTNET:
 case S_ATT_T1:
 case S_ATTNET_T1:
 case S_NI2:
 case S_NI2NET:
 case S_INS :
 case S_INSNET :    
 case S_IDAP:
 case S_IDAPNET:
 case S_QSIG:
	outx.unique_xparms.sig_q931.dest_numbering_type = NT_NATIONAL;
	outx.unique_xparms.sig_q931.dest_numbering_plan = NP_ISDN;
	outx.unique_xparms.sig_q931.service_octet  = 0;
	outx.unique_xparms.sig_q931.add_info_octet = 0;
	outx.unique_xparms.sig_q931.bearer.ie[0] = 0;
	break;
 case S_DASS:
 case S_DASSNET:
	outx.unique_xparms.sig_dass.sic1 = 0x10;
	outx.unique_xparms.sig_dass.sic2 = 0xff;
	break;
 case S_DPNSS:
	outx.unique_xparms.sig_dpnss.sic1 = 0x10;
	outx.unique_xparms.sig_dpnss.sic2 = 0xff;
	break;
 case S_CAS:
 case S_CAS_TONE:
 case S_T1CAS:
 case S_T1CAS_TONE:
	break;
 default:
 	printf("No unique_xparms set for unknown net type: %d\n", nettype);
 }
 iErr = outcall(arg.port, &outx);
 V6ClosePort(&resources);
 V6CloseCall(&resources);
 V6CloseCard(&resources);
 return iErr;
}
