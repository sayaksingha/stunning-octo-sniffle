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

static int chan_alloc(tSMChannelId *cp, MVIP *ts, int io, tSMModuleId module)
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

static int startchan(struct ec *ecp, MVIP *outts, MVIP *sigts, MVIP *refts, tSMModuleId module, int ec, int delay, float gain, float *filt, unsigned flen)
{
 SM_CONDITION_INPUT_PARMS ci;
 int r = chan_alloc(&ecp->sig, sigts, 1, module);
 if (r) return r;
 r = chan_alloc(&ecp->ref, refts, 1, module);
 if (r) return r;
 r = chan_alloc(&ecp->out, outts, 0, module);
 if (r) return r;
 ci.channel = ecp->sig;
 ci.reference = ecp->ref;
 ci.reference_type = kSMInputCondRefUseInput;
 ci.conditioning_type = ec ? kSMInputCondEchoCancelation : kSMInputCondNone;
 ci.conditioning_param = 0;
 ci.alt_dest_type = kSMInputCondAltDestOutput;
 ci.alt_data_dest = ecp->out;
 if (flen) {
	ci.ectest_type = 2;
	ci.ectest_filt = filt;
	ci.ectest_flen = flen;
 } else {
	ci.ectest_type = 1;
	ci.ectest_delay = delay;
	ci.ectest_gain = gain;
 }
 r = sm_condition_input(&ci);
 if (r) return printerr_sm("sm_condition_input", r);
 return 0;
}

static int read_efilt(float **efiltp, unsigned *eflenp, char *eff)
{
 unsigned curlen = 0;
 unsigned linno = 0;
 FILE *ef;
 *eflenp = 0;
 *efiltp = 0;
 if (!eff) return 0;
 ef = fopen(eff, "r");
 if (!ef) {
 	perror("fopen failed");
 } else for (;;) {
 	char buff[80], *ep;
 	if (!fgets(buff, arlen(buff), ef)) {
 		if (ferror(ef)) {
 			perror("fgets failed");
			break;
		}
		return 0;
	}
	if (buff[strlen(buff)-1] != '\n') {
		fprintf(stderr, "Line %d too long '%s'...\n", linno, buff);
		break;
	}
	linno++;
	if (curlen <= *eflenp) {
		float *np;
		curlen = (curlen + 1) * 2;
		np = realloc(*efiltp, curlen * sizeof(*np));
		if (!np) {
			perror("realloc failed");
			break;
		}
		*efiltp = np;
	}
	(*efiltp)[*eflenp] = strtod(buff, &ep);
	while (*ep == ' ' || *ep == '\t' || *ep == '\r') ep++;
	if (*ep != '\n') {
		fprintf(stderr, "Junk after number on line %d: '%s'\n",
			linno, ep);
		break;
	}
	++*eflenp;
 }
 fprintf(stderr, "File: %s\n", eff);
 free(*efiltp);
 *efiltp = 0;
 *eflenp = 0;
 return 1;
}

static int echocan(int ec, char *eff, int delay, float gain, MVIP *outtimeslot, MVIP *reftimeslot, MVIP *sigtimeslot, tSMModuleId module, unsigned numchan)
{
 struct ec *ecp = malloc(sizeof(*ecp) * numchan);
 unsigned eflen;
 unsigned chan;
 float *efilt;
 int r = 0;
 if (!ecp) {
	fprintf(stderr, "Cannot allocate memory for channels\n");
	return 1;
 }
 if (read_efilt(&efilt, &eflen, eff)) {
	fprintf(stderr, "Cannot read echo generation filter\n");
	free(ecp);
	return 1;
 }
 for (chan=0; chan < numchan; chan++) {
	int sts = startchan(&ecp[chan],
		outtimeslot, sigtimeslot, reftimeslot, module,
		ec, delay, gain, efilt, eflen);
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

#include "gen/ectest.args.i"

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
	err_t e = modopen(&card, &mod, arg.serialnumber, arg.module);
	if (e) {
		error_log(stderr, e);
		return 1;
	}
 } else {
	fprintf(stderr, "%s: no Prosody card specified\n", progname);
	return 1;
 }
 iErr = echocan(arg.echocancel, arg.echofiltfile, arg.delay, arg.gain, &arg.outtimeslot, &arg.reftimeslot, &arg.sigtimeslot, mod, arg.numchan);
 if (arg.serialnumber) {
	modclose(mod);
	cardclose(card);
 }
 return iErr;
}
