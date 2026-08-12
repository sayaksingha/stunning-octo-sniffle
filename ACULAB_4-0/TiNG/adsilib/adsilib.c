#include <stdio.h>
#include <string.h>

#include "adsilib.h"
#include "smbesp.h"
#include "smdrvr.h"
#include "smdc.h"
#include "smdc_async.h"
#include "smdc_fsk.h"

static int add_output_freq(int *id, tSMModuleId module, int freq, int ampl)
{
 SM_OUTPUT_FREQ_PARMS ofp;
 int e;
 memset(&ofp, 0, sizeof(ofp));
 ofp.module = module;
 ofp.freq = freq;
 ofp.amplitude = ampl;
 e = sm_add_output_freq(&ofp);
 *id = ofp.id;
 return e;
}

static int add_output_tone(int *id, tSMModuleId module, int id1, int id2)
{
 SM_OUTPUT_TONE_PARMS otp;
 int e;
 memset(&otp, 0, sizeof(otp));
 otp.module = module;
 otp.component1_id = id1;
 otp.component2_id = id2;		// silence
 e = sm_add_output_tone(&otp);
 *id = otp.id;
 return e;
}

int adsi_init(ADSI_MODULE *mp, tSMModuleId module)
{
 int id_2130;
 int id_2750;
 int id_440;
 int e;
 mp->module = module;
 	// add CAS
 e = add_output_freq(&id_2130, mp->module, 2130, -15);
 if (e) return e;
 e = add_output_freq(&id_2750, mp->module, 2750, -15);
 if (e) return e;
 e = add_output_tone(&mp->tone_id_cas, mp->module, id_2130, id_2750);
 if (e) return e;
 	// add SAS
 e = add_output_freq(&id_440, mp->module, 440, -16);
 if (e) return e;
 e = add_output_tone(&mp->tone_id_440_Hz, mp->module, id_440, 0);
 if (e) return e;
 return 0;
}

int sm_play_sas_cas(struct sm_play_sas_cas_parms *pp, ADSI_MODULE *mp)
{
 SM_PLAY_CPTONE_PARMS cpp;
 memset(&cpp, 0, sizeof(cpp));
 cpp.channel = pp->channel;
 switch (pp->sas_tone_type) {
 case kSASToneTypeSingleBurst:
 	// cpp.duration = 300 + 160;
 	cpp.tone_count = 1;
 	cpp.cadences[0].tone_id = mp->tone_id_440_Hz;
 	cpp.cadences[0].on_cadence = 300;
 	cpp.cadences[0].off_cadence = 160;
 	break;
 case kSASToneTypeDoubleBurst:
 	// cpp.duration = 100 + 100 + 100 + 160;
 	cpp.tone_count = 2;
 	cpp.cadences[0].tone_id = mp->tone_id_440_Hz;
 	cpp.cadences[0].on_cadence = 100;
 	cpp.cadences[0].off_cadence = 100;
 	cpp.cadences[1].tone_id = mp->tone_id_440_Hz;
 	cpp.cadences[1].on_cadence = 100;
 	cpp.cadences[1].off_cadence = 160;
 	break;
 case kSASToneTypeTripleBurst:
 	// cpp.duration = 100 + 100 + 100 + 160;
 	cpp.tone_count = 3;
 	cpp.cadences[0].tone_id = mp->tone_id_440_Hz;
 	cpp.cadences[0].on_cadence = 100;
 	cpp.cadences[0].off_cadence = 100;
 	cpp.cadences[1].tone_id = mp->tone_id_440_Hz;
 	cpp.cadences[1].on_cadence = 100;
 	cpp.cadences[1].off_cadence = 100;
 	cpp.cadences[2].tone_id = mp->tone_id_440_Hz;
 	cpp.cadences[2].on_cadence = 100;
 	cpp.cadences[2].off_cadence = 160;
 	break;
 }
 	// append CAS to tone list
 cpp.cadences[cpp.tone_count].tone_id = mp->tone_id_cas;
 cpp.cadences[cpp.tone_count].on_cadence = 82;
 cpp.cadences[cpp.tone_count].off_cadence = 160;
 cpp.tone_count++;
 	// start the tone playing
 cpp.type = kSMPlayCPToneTypeOneShot;
 return sm_play_cptone(&cpp);
}

static int adsi_fsk_config(tSMChannelId chan)
{
 SMDC_CHANNEL_CONFIG_PARMS ccp;
 SMDC_ASYNC_FORMAT_PARMS asyp;
 SMDC_FSK_CONFIG_PARMS fskp;
 memset(&ccp, 0, sizeof(ccp));
 ccp.channel = chan;
 	// use FSK protocol
 ccp.protocol = kSMDCProtocolFSKtx;
 memset(&fskp, 0, sizeof(fskp));
 fskp.speed = 1200;
 fskp.mark_frequency = 1200;
 fskp.space_frequency = 2200;
 fskp.set_power = 1;
 fskp.user_power = -13;
 ccp.config_length = sizeof(fskp);
 ccp.config_data = &fskp;
 	// use ASYNC encoding
 memset(&asyp, 0, sizeof(asyp));
 asyp.stopbits = 0;
 asyp.databits = 8;
 ccp.encoding = kSMDCConfigEncodingAsync;
 ccp.encoding_config_length = sizeof(asyp);
 ccp.encoding_config_data = &asyp;
 return smdc_channel_config(&ccp);
}

static int adsi_onhook_prefix(tSMChannelId chan)
{
 static tSM_UT32 prefix[] = {
	0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa,
	0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xaaaaaaaa, 0xfffffaaa,
	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
 };	// 0101...01 followed by mark bits
 SMDC_LINE_CONTROL_PARMS slp;
 memset(&slp, 0, sizeof(slp));
 slp.channel = chan;
 slp.cmd = kSMDCLineCtlCmdSetPrefixSuffix;
 slp.prefix_data = prefix;
 slp.prefix_bitlength = 300 + 180;
 slp.suffix_data = 0;
 slp.suffix_bitlength = 0;
 return smdc_line_control(&slp);
}

static int adsi_offhook_prefix(tSMChannelId chan)
{
 static tSM_UT32 prefix[] = {
	0xffffffff, 0xffffffff, 0xffffffff,
 };	// all mark bits
 static tSM_UT32 suffix[1] = { 0xffffffff };	// all mark bits
 SMDC_LINE_CONTROL_PARMS slp;
 memset(&slp, 0, sizeof(slp));
 slp.channel = chan;
 slp.cmd = kSMDCLineCtlCmdSetPrefixSuffix;
 slp.prefix_data = prefix;
 slp.prefix_bitlength = 80;
 slp.suffix_data = suffix;
 slp.suffix_bitlength = 0;
 return smdc_line_control(&slp);
}

static int finish_cmd(tSMChannelId chan)
{
 SMDC_TX_CONTROL_PARMS tcp;
 memset(&tcp, 0, sizeof(tcp));
 tcp.channel = chan;
 tcp.cmd = kSMDCTxCtlFinish;
 return smdc_tx_control(&tcp);
}

static int check_txstatus(SMDC_LINE_STATUS_PARMS *lsp, SMDC_TX_STATUS_PARMS *tsp, tSMChannelId chan)
{
 int e;
 memset(lsp, 0, sizeof(*lsp));
 memset(tsp, 0, sizeof(*tsp));
 lsp->channel = chan;
 tsp->channel = chan;
 e = smdc_line_status(lsp);
 if (e) return e;
 if (lsp->link_status != kSMDCLinkStatusNotConnected) {
	e = smdc_tx_status(tsp);
	if (e) return e;
 }
 return 0;
}

	// handy predefined messages for message indicator
char adsi_sdmf_indicator_on[] = {6, 3, 'B', 'B', 'B',	// message
	0xff & -(6 + 3 + 'B' + 'B' + 'B') };		// checksum
char adsi_sdmf_indicator_off[] = {6, 3, 'o', 'o', 'o',	// message
	0xff & -(6 + 3 + 'o' + 'o' + 'o') };		// checksum
char adsi_mdmf_indicator_on[] = {0x82, 0x03, 0x0b, 0x01, 0xff,	// message
	0xff & -(0x82 + 0x03 + 0x0b + 0x01 + 0xff) };		// checksum
char adsi_mdmf_indicator_off[] = {0x82, 0x03, 0x0b, 0x01, 0x00,	// message
	0xff & -(0x82 + 0x03 + 0x0b + 0x00 + 0xff) };		// checksum

static int adsi_tx_data(tSMChannelId chan, char *data, unsigned datalen)
{
 int e;
 for (;;) {
	SMDC_LINE_STATUS_PARMS lsp;
	SMDC_TX_STATUS_PARMS tsp;
	SMDC_DATA_PARMS dp;
	memset(&dp, 0, sizeof(dp));
	dp.channel = chan;
	dp.data = data;
	dp.max_length = datalen;
	e = smdc_tx_data(&dp);
	if (e) return e;
 	data += dp.done_length;
 	datalen -= dp.done_length;
	if (!datalen) break;
 	e = check_txstatus(&lsp, &tsp, chan);
 	if (e) return e;
 }
 e = finish_cmd(chan);
 if (e) return e;
 return 0;
}

int sm_play_vmwi_fsk(struct sm_play_vmwi_fsk_parms *pp)
{
 static struct {
	char *data;
	unsigned len;
 } msg[4] = {
	{ adsi_sdmf_indicator_on, sizeof(adsi_sdmf_indicator_on), },
	{ adsi_sdmf_indicator_off, sizeof(adsi_sdmf_indicator_off), },
	{ adsi_mdmf_indicator_on, sizeof(adsi_mdmf_indicator_on), },
	{ adsi_mdmf_indicator_off, sizeof(adsi_mdmf_indicator_off), },
 };
 int e = adsi_fsk_config(pp->channel);
 if (e) return e;
 if (pp->do_on_hook) e = adsi_onhook_prefix(pp->channel);
 else e = adsi_offhook_prefix(pp->channel);
 if (e) return e;
 e = !pp->is_indicator_on_message;	// e=0 for ON, 1 for OFF
 e += 2 * !pp->is_sdmf_message;		// add two if mdmf
 return adsi_tx_data(pp->channel, msg[e].data, msg[e].len);
}

int adsi_status(enum adsi_txstatus *sts, tSMChannelId chan)
{
 SMDC_LINE_STATUS_PARMS lsp;
 SMDC_TX_STATUS_PARMS tsp;
 int e = check_txstatus(&lsp, &tsp, chan);
 if (e) return e;
 if (lsp.link_status == kSMDCLinkStatusNotConnected) {
 	*sts = kADSIStatusComplete;
 } else {
 	*sts = kADSIStatusOngoing;
 }
 return 0;
}

static int adsi_checksum(char *msg, char *endmsg)
{
 char sum = 0;
 while (msg < endmsg) {
 	sum += *msg++;
 }
 return -sum;
}

int sm_play_cnd_fsk(struct sm_play_cnd_fsk_parms *pp)
{
 unsigned dlen = strlen(pp->digit_string);
 int e = adsi_fsk_config(pp->channel);
 char buff[512];
 char *ep;	// end pointer for buff
 if (e) return e;
 	// set up suitable prefix
 if (pp->do_on_hook) e = adsi_onhook_prefix(pp->channel);
 else e = adsi_offhook_prefix(pp->channel);
 if (e) return e;
 	// build message
 ep = buff;
 if (pp->is_sdmf_message) {
	*ep++ = 4;		// message type
	*ep++ = 0;		// placeholder for length
	sprintf(ep, "%02d%02d%02d%02d", pp->month, pp->day, pp->hour, pp->minute);
	ep += 8;
	switch (pp->cnd_type) {
	case kCNDTypeAnonymous:
		*ep++ = 'P';
		break;
	case kCNDTypeOutOfArea:
		*ep++ = 'O';
		break;
	case kCNDTypeCallingDN:
		strcpy(ep, pp->digit_string);
		ep += dlen;
		break;
	}
 } else {
	*ep++ = 0x80;		// message type
	*ep++ = 0;		// placeholder for length
	*ep++ = 0x01;	// code for timestamp
	*ep++ = 0x08;	// length of timestamp
	sprintf(ep, "%02d%02d%02d%02d", pp->month, pp->day, pp->hour, pp->minute);
	ep += 8;
	switch (pp->cnd_type) {
	case kCNDTypeAnonymous:
		*ep++ = 4;	// code for anon/outofarea
		*ep++ = 1;	// length of anon/outofarea
		*ep++ = 'P';
		break;
	case kCNDTypeOutOfArea:
		*ep++ = 4;	// code for anon/outofarea
		*ep++ = 1;	// length of anon/outofarea
		*ep++ = 'O';
		break;
	case kCNDTypeCallingDN:
		*ep++ = 2;	// code for number
		*ep++ = dlen;
		strcpy(ep, pp->digit_string);
		ep += dlen;
		dlen = strlen(pp->name_string);
		if (dlen) {
			*ep++ = 7;	// type for name
			*ep++ = dlen;	// length of name
			strcpy(ep, pp->name_string);
			ep += dlen;
		}
		break;
	}
 }
 buff[1] = ep - buff - 2;				// fill in length
 *ep = adsi_checksum(buff, ep);				// append checksum
 ep++;
 return adsi_tx_data(pp->channel, buff, ep - buff);	// and send it
}
