#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "smdrvr.h"
#include "smdc.h"
#include "smdc_cpu.h"

#include "../Testlib/cardopen.h"
#include "../Testlib/errcode_sm.h"
#include "../Testlib/modopen.h"
#include "../../libutil/generic_io.h"

#define arlen(s) (sizeof(s)/sizeof(*(s)))

	// convenient encapsulations of Prosody API functions

static int chan_config_cpu(tSMChannelId chan)
{
 SMDC_CHANNEL_CONFIG_PARMS sc;
 int r;
 memset(&sc, 0, sizeof(sc));
 sc.channel = chan;
 sc.protocol = kSMDCProtocolCpu;
 sc.config_length = 0;
 sc.config_data = 0;
 r = smdc_channel_config(&sc);
 if (r) return printerr_sm("smdc_channel_config", r);
 return 0;
}

static int chan_rx_control(tSMChannelId chan, int cmd, int mincol, int minidl)
{
 int r;
 SMDC_RX_CONTROL_PARMS dp;
 memset(&dp, 0, sizeof(dp));
 dp.channel = chan;
 dp.cmd = cmd;
 dp.min_to_collect = mincol;
 dp.min_idle = minidl;
 r = smdc_rx_control(&dp);
 if (r) return printerr_sm("smdc_rx_control", r);
 return 0;
}

static int chan_rx_data(int *rxlen, tSMChannelId chan, void *buf, int len)
{
 for (;;) {
	SMDC_RX_STATUS_PARMS sp;
	SMDC_DATA_PARMS dp;
	int r;
	memset(&dp, 0, sizeof(dp));
	dp.channel = chan;
	dp.data = buf;
	dp.max_length = len;
	r = smdc_rx_data(&dp);
	if (r) return printerr_sm("smdc_rx_data", r);
	if ((*rxlen =  dp.done_length)) break;
	memset(&sp, 0, sizeof(sp));
	sp.channel = chan;
	r = smdc_rx_status(&sp);
	if (r) return printerr_sm("smdc_rx_status", r);
 }
 return 0;
}

static int chan_stop_cpu(tSMChannelId chan)
{
 SMDC_STOP_PARMS stopp;
 int r;
 memset(&stopp, 0, sizeof(stopp));
 stopp.channel = chan;
 r = smdc_stop(&stopp);
 if (r) return printerr_sm("smdc_stop", r);
 return 0;
}


static int cpumon(tSMModuleId module, unsigned long maxrx, char *pfx)
{
 SM_CHANNEL_ALLOC_PLACED_PARMS sa;
 unsigned long totrx = 0;
 FILE *rxfile;
 OPENFILE in;
 int r = 0;
 if (io_stdinfile(&in)) {
	fprintf(stderr, "Cannot get stdin\n");
	return 1;
 }
 	// initialisation needed by data transfer
 memset(&sa, 0, sizeof(sa));
 sa.type = kSMChannelTypeInput;
 sa.module = module;
 r = sm_channel_alloc_placed(&sa);
 if (r) return printerr_sm("sm_channel_alloc_placed", r);
 rxfile = fopen(pfx, "wb");
 if (!rxfile) {
	perror("Cannot create file");
	fprintf(stderr, "File: %s\n", pfx);
	return 1;
 }
 if (chan_config_cpu(sa.channel)) return 1;
 if (chan_rx_control(sa.channel, kSMDCRxCtlNotifyOnData, 1024, 500)) return 1;
 for (;;) {
	unsigned char buff[1024];
	int p0;
	if (chan_rx_data(&p0, sa.channel, buff, sizeof(buff))) break;
	if (fwrite(buff, 1, p0, rxfile) != (unsigned) p0) {
		perror("fwrite failed");
		return 1;
	}
	fflush(rxfile);
	totrx += p0;
	if (maxrx && totrx >= maxrx) break;
	p0 = io_inkey(in);
	if (p0 != INKEY_NONE) break;
 } 
 fclose(rxfile);
 chan_stop_cpu(sa.channel);
 sm_channel_release(sa.channel);
 return 0;
}

#include "gen/cpumon.args.i"

int main(int argc, char **argv)
{
 tSMModuleId mod;
 tSMCardId card;
 int iErr;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || !*argv || argv[1]) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE " datafile\n", progname);
	return 1;
 }
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
 if (arg.maxrx) arg.maxrx *= 4;
 iErr = cpumon(mod, arg.maxrx, *argv);
 if (arg.serialnumber) {
	modclose(mod);
	cardclose(card);
 }
 return iErr;
}
