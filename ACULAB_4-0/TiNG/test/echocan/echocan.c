#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>

#ifdef TiNGTYPE_WINNT

/* disable stupid warnings about conversions to smaller types */

#pragma warning(disable:4761)
#pragma warning(disable:4244)
#pragma warning(disable:4305)

#endif

#include "smdrvr.h"
#include "smbesp.h"

#include <malloc.h>

#include "../Testlib/cardopen.h"
#include "../Testlib/errcode_sm.h"
#include "../Testlib/modopen.h"
#include "../../libutil/generic_io.h"

#define arlen(s) (sizeof(s)/sizeof(*(s)))

typedef struct {
	int stream;
	int timeslot;
	int type;
} MVIP;
#define MVIP_STREAM_NONE 0xff

struct ec {
	tSMChannelId ref, sig, out;
};

static int chan_alloc_in(tSMChannelId *cp, MVIP *ts, int io, tSMModuleId module)
{
 SM_SWITCH_CHANNEL_PARMS swp;
 SM_CHANNEL_ALLOC_PLACED_PARMS sa;
 int r;
 memset(&sa, 0, sizeof(sa));
 sa.type = io ? kSMChannelTypeInput : kSMChannelTypeOutput;
 sa.module = module;
 r = sm_channel_alloc_placed(&sa);
 if (r) return printerr_sm("sm_channel_alloc_placed", r);
 *cp = sa.channel;
 memset(&swp, 0, sizeof(swp));
 swp.channel = *cp;
 swp.st = ts->stream;
 swp.ts = ts->timeslot;
 swp.type = ts->type;
 if (++ts->timeslot >= 32) {
	ts->timeslot = 0;
	ts->stream++;
 }
 r = (io ? sm_switch_channel_input : sm_switch_channel_output)(&swp);
 if (r) return printerr_sm("sm_switch_channel_input", r);
 return r;
}

	// initialisation

static int startchan(struct ec *ecp, MVIP *outts, MVIP *sigts, MVIP *refts, tSMModuleId module)
{
 SM_CONDITION_INPUT_PARMS ci;
 int r = chan_alloc_in(&ecp->sig, sigts, 1, module);
 if (r) return r;
 r = chan_alloc_in(&ecp->ref, refts, 1, module);
 if (r) return r;
 r = chan_alloc_in(&ecp->out, outts, 0, module);
 if (r) return r;
 memset(&ci, 0, sizeof(ci));
 ci.channel = ecp->sig;
 ci.reference = ecp->ref;
 ci.reference_type = kSMInputCondRefUseInput;
 ci.conditioning_type = kSMInputCondEchoCancelation;
 ci.conditioning_param = 0;
 ci.alt_dest_type = kSMInputCondAltDestOutput;
 ci.alt_data_dest = ecp->out;
 r = sm_condition_input(&ci);
 if (r) return printerr_sm("sm_condition_input", r);
 return 0;
}

static int echocan(MVIP *outtimeslot, MVIP *reftimeslot, MVIP *sigtimeslot, tSMModuleId module, unsigned numchan)
{
 struct ec *ecp = malloc(sizeof(*ecp) * numchan);
 unsigned chan;
 int r = 0;
 if (!ecp) {
	fprintf(stderr, "Cannot allocate memory for channels\n");
	return 1;
 }
 for (chan=0; chan < numchan; chan++) {
	int sts = startchan(&ecp[chan],
		outtimeslot, sigtimeslot, reftimeslot, module);
	if (sts) return 1;
 }
 for (;;) {
 	switch (getchar()) {
 	case EOF:
 	case 'q': goto done;
 	}
 }
 done:
 for (chan=0; chan < numchan; chan++) {
	r = sm_channel_release(ecp[chan].ref);
	if (r) printerr_sm("sm_channel_release(%d ref)", r);
	r = sm_channel_release(ecp[chan].sig);
	if (r) printerr_sm("sm_channel_release(%d sig)", r);
	r = sm_channel_release(ecp[chan].out);
	if (r) printerr_sm("sm_channel_release(%d out)", r);
 }
 free(ecp);
 return r;
}

#include "gen/echocan.args.i"

int main(int argc, char **argv)
{
 tSMModuleId mod;
 tSMCardId card;
 int iErr;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || *argv) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
	return 1;
 }
 if (!arg.numchan) arg.numchan = 1;
 if (arg.serialnumber) {
	err_t e;
	e = modopen(&card, &mod, arg.serialnumber, arg.module);
	if (e) {
		error_log(stderr, e);
		return 1;
	}
 } else {
	fprintf(stderr, "%s: no Prosody card specified\n", progname);
	return 1;
 }
 iErr = echocan(&arg.outtimeslot, &arg.reftimeslot, &arg.sigtimeslot, mod, arg.numchan);
 if (arg.serialnumber) {
	modclose(mod);
	cardclose(card);
 }
 return iErr;
}
