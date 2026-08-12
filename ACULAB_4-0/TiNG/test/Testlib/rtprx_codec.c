#include "rtprx_codec.h"
#include "../Testlib/errcode_sm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define arlen(x) (sizeof(x)/sizeof(*(x)))

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

static err_t config_codec_alaw(tSMVMPrxId vmprx, RTPRX_PAR *rtprx_par)
{
 SM_VMPRX_CODEC_ALAW_PARMS cp;
 int e;
 memset(&cp, 0, sizeof(cp));
 cp.vmprx = vmprx;
 cp.payload_type = rtprx_par->payloadmapping;
 cp.plc_mode = rtprx_par->plc ? kSMPLCModeEnabled : kSMPLCModeDisabled;
 e = sm_vmprx_config_codec_alaw(&cp);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_alaw() failed");
 return 0;
}

static err_t config_codec_l8(tSMVMPrxId vmprx, RTPRX_PAR *rtprx_par)
{
 SM_VMPRX_CODEC_L8_PARMS cp;
 int e;
 memset(&cp, 0, sizeof(cp));
 cp.vmprx = vmprx;
 cp.payload_type = rtprx_par->payloadmapping;
 cp.plc_mode = rtprx_par->plc ? kSMPLCModeEnabled : kSMPLCModeDisabled;
 e = sm_vmprx_config_codec_l8(&cp);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_l8() failed");
 return 0;
}

static err_t config_codec_l16(tSMVMPrxId vmprx, RTPRX_PAR *rtprx_par)
{
 SM_VMPRX_CODEC_L16_PARMS cp;
 int e;
 memset(&cp, 0, sizeof(cp));
 cp.vmprx = vmprx;
 cp.payload_type = rtprx_par->payloadmapping;
 cp.plc_mode = rtprx_par->plc ? kSMPLCModeEnabled : kSMPLCModeDisabled;
 e = sm_vmprx_config_codec_l16(&cp);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_l16() failed");
 return 0;
}

static err_t config_codec_g723_1(tSMVMPrxId vmprx, RTPRX_PAR *rtprx_par)
{
 SM_VMPRX_CODEC_G723_1_PARMS cp;
 int e;
 memset(&cp, 0, sizeof(cp));
 cp.vmprx = vmprx;
 cp.payload_type = rtprx_par->payloadmapping;
 cp.post_filter = rtprx_par->variant;
 e = sm_vmprx_config_codec_g723_1(&cp);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_g723_1() failed");
 return 0;
}

static err_t config_codec_g722(tSMVMPrxId vmprx, RTPRX_PAR *rtprx_par)
{
 SM_VMPRX_CODEC_G722_PARMS cp;
 int e;
 memset(&cp, 0, sizeof(cp));
 cp.vmprx = vmprx;
 cp.payload_type = rtprx_par->payloadmapping;
 cp.plc_mode = rtprx_par->plc ? kSMPLCModeEnabled : kSMPLCModeDisabled;
 e = sm_vmprx_config_codec_g722(&cp);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_g722() failed");
 return 0;
}

static err_t config_codec_g722_1(tSMVMPrxId vmprx, RTPRX_PAR *rtprx_par)
{
 SM_VMPRX_CODEC_G722_1_PARMS cp;
 int e;
 if (rtprx_par->bitrate != 24000 && rtprx_par->bitrate != 32000) {
 	return error(0, "G.722.1: can only handle bitrates of 24000 or 32000");
 }
 memset(&cp, 0, sizeof(cp));
 cp.vmprx = vmprx;
 cp.payload_type = rtprx_par->payloadmapping;
 cp.bitrate = rtprx_par->bitrate;
 e = sm_vmprx_config_codec_g722_1(&cp);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_g722_1() failed");
 return 0;
}

static err_t config_codec_silk(tSMVMPrxId vmprx, RTPRX_PAR *rtprx_par)
{
 SM_VMPRX_CODEC_SILK_PARMS cp;
 int e;
 memset(&cp, 0, sizeof(cp));
 cp.vmprx = vmprx;
 cp.payload_type = rtprx_par->payloadmapping;
 cp.plc_mode = rtprx_par->plc;
 e = sm_vmprx_config_codec_silk(&cp);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_silk() failed");
 return 0;
}

static err_t config_codec_opus(tSMVMPrxId vmprx, RTPRX_PAR *rtprx_par)
{
 SM_VMPRX_CODEC_OPUS_PARMS cp;
 int e;
 memset(&cp, 0, sizeof(cp));
 cp.vmprx = vmprx;
 cp.payload_type = rtprx_par->payloadmapping;
 cp.decode_fec = rtprx_par->variant;
 e = sm_vmprx_config_codec_opus(&cp);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_opus() failed");
 return 0;
}

static err_t config_codec_g726(tSMVMPrxId vmprx, RTPRX_PAR *rtprx_par)
{
 SM_VMPRX_CODEC_G726_PARMS cp;
 int e;
 memset(&cp, 0, sizeof(cp));
 cp.vmprx = vmprx;
 cp.payload_type = rtprx_par->payloadmapping;
 cp.bits = rtprx_par->bitrate / 8000;
 cp.variant = rtprx_par->variant;
 if (cp.bits < 2 || cp.bits > 5) 
	 return error(0, "G.726 requires the bitrate to be set 16000 to 40000");
 cp.plc_mode = rtprx_par->plc ? kSMPLCModeEnabled : kSMPLCModeDisabled;
 e = sm_vmprx_config_codec_g726(&cp);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_g726() failed");
 return 0;
}

static err_t config_codec_g728(tSMVMPrxId vmprx, RTPRX_PAR *rtprx_par)
{
 SM_VMPRX_CODEC_G728_PARMS cp;
 int e;
 if (rtprx_par->bitrate != 9600 && rtprx_par->bitrate != 12800 && rtprx_par->bitrate != 16000) {
	 return error(0, "G.728 requires bitrate 9600, 12800 or 16000");
 }
 memset(&cp, 0, sizeof(cp));
 cp.vmprx = vmprx;
 cp.payload_type = rtprx_par->payloadmapping;
 cp.rate = rtprx_par->bitrate;
 cp.post_filter = rtprx_par->variant;
 e = sm_vmprx_config_codec_g728(&cp);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_g728() failed");
 return 0;
}

static err_t config_codec_g729(tSMVMPrxId vmprx, RTPRX_PAR *rtprx_par)
{
 SM_VMPRX_CODEC_G729I_PARMS cp;
 int e;
 memset(&cp, 0, sizeof(cp));
 cp.vmprx = vmprx;
 cp.payload_type = rtprx_par->payloadmapping;
 cp.g729_mode = kSMG729IModeG729;
 e = sm_vmprx_config_codec_g729i(&cp);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_g729() failed");
 return 0;
}

static err_t config_codec_g729ab(tSMVMPrxId vmprx, RTPRX_PAR *rtprx_par)
{
 SM_VMPRX_CODEC_G729AB_PARMS cp;
 int e;
 memset(&cp, 0, sizeof(cp));
 cp.vmprx = vmprx;
 cp.payload_type = rtprx_par->payloadmapping;
 e = sm_vmprx_config_codec_g729ab(&cp);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_g729ab() failed");
 return 0;
}

static err_t config_codec_g729d(tSMVMPrxId vmprx, RTPRX_PAR *rtprx_par)
{
 SM_VMPRX_CODEC_G729I_PARMS cp;
 int e;
 memset(&cp, 0, sizeof(cp));
 cp.vmprx = vmprx;
 cp.payload_type = rtprx_par->payloadmapping;
 cp.g729_mode = kSMG729IModeG729D;
 e = sm_vmprx_config_codec_g729i(&cp);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_g729d() failed");
 return 0;
}

static err_t config_codec_melpe(tSMVMPrxId vmprx, RTPRX_PAR *rtprx_par)
{
 SM_VMPRX_CODEC_MELPE_PARMS cp;
 int e;
 memset(&cp, 0, sizeof(cp));
 cp.vmprx = vmprx;
 cp.payload_type = rtprx_par->payloadmapping;
 e = sm_vmprx_config_codec_melpe(&cp);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_melpe() failed");
 return 0;
}

static err_t config_codec_g729e(tSMVMPrxId vmprx, RTPRX_PAR *rtprx_par)
{
 SM_VMPRX_CODEC_G729I_PARMS cp;
 int e;
 memset(&cp, 0, sizeof(cp));
 cp.vmprx = vmprx;
 cp.payload_type = rtprx_par->payloadmapping;
 cp.g729_mode = kSMG729IModeG729E;
 e = sm_vmprx_config_codec_g729i(&cp);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_g729e() failed");
 return 0;
}

static err_t config_codec_gsmefr(tSMVMPrxId vmprx, RTPRX_PAR *rtprx_par)
{
 SM_VMPRX_CODEC_GSMEFR_PARMS codec_gsmefr;
 int e;
 memset(&codec_gsmefr, 0, sizeof(codec_gsmefr));
 codec_gsmefr.vmprx = vmprx;
 codec_gsmefr.payload_type = rtprx_par->payloadmapping;
 e = sm_vmprx_config_codec_gsmefr(&codec_gsmefr);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_gsmefr() failed");
 return 0;
}

static err_t config_codec_amrnb(tSMVMPrxId vmprx, RTPRX_PAR *rtprx_par)
{
 SM_VMPRX_CODEC_AMRNB_PARMS codec_amrp;
 int e;
 memset(&codec_amrp, 0, sizeof(codec_amrp));
 codec_amrp.vmprx = vmprx;
 codec_amrp.payload_type = rtprx_par->payloadmapping;
 codec_amrp.aligned = rtprx_par->variant == 1;
 codec_amrp.gsmefr = rtprx_par->variant == 2;
 codec_amrp.report_cmr = 1;
 e = sm_vmprx_config_codec_amrnb(&codec_amrp);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_amrnb() failed");
 return 0;
}

static err_t codec_status_amrnb(tSMVMPrxId vmprx)
{
 SM_VMPRX_STATUS_CODEC_AMRNB_PARMS codec_amrp;
 int e;
 memset(&codec_amrp, 0, sizeof(codec_amrp));
 codec_amrp.vmprx = vmprx;
 e = sm_vmprx_status_codec_amrnb(&codec_amrp);
 if (e) return prosody_error(e, "sm_vmprx_status_codec_amrnb() failed");
 if (codec_amrp.status == kSMVMPrxAMRNBStatusNewCMR) {
	 printf("New AMR-NB bitrate request %d\n", codec_amrp.u.cmr.bitrate);
 }
 return 0;
}

static err_t config_codec_amrwb(tSMVMPrxId vmprx, RTPRX_PAR *rtprx_par)
{
 SM_VMPRX_CODEC_AMRWB_PARMS codec_amrp;
 int e;
 memset(&codec_amrp, 0, sizeof(codec_amrp));
 codec_amrp.vmprx = vmprx;
 codec_amrp.payload_type = rtprx_par->payloadmapping;
 codec_amrp.aligned = rtprx_par->variant;
 codec_amrp.report_cmr = 1;
 e = sm_vmprx_config_codec_amrwb(&codec_amrp);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_amrwb() failed");
 return 0;
}

static err_t codec_status_amrwb(tSMVMPrxId vmprx)
{
 SM_VMPRX_STATUS_CODEC_AMRWB_PARMS codec_amrp;
 int e;
 memset(&codec_amrp, 0, sizeof(codec_amrp));
 codec_amrp.vmprx = vmprx;
 e = sm_vmprx_status_codec_amrwb(&codec_amrp);
 if (e) return prosody_error(e, "sm_vmprx_status_codec_amrwb() failed");
 if (codec_amrp.status == kSMVMPrxAMRWBStatusNewCMR) {
	 printf("New AMR-WB bitrate request %d\n", codec_amrp.u.cmr.bitrate);
 }
 return 0;
}

static err_t config_codec_speex(tSMVMPrxId vmprx, RTPRX_PAR *rtprx_par)
{
 SM_VMPRX_CODEC_SPEEX_PARMS codec_speexp;
 SM_VMPRX_CODEC_SPEEX_MODE_PARMS codec_speexp_mode;
 int e;
 memset(&codec_speexp, 0, sizeof(codec_speexp));
 codec_speexp.vmprx = vmprx;
 codec_speexp.payload_type = rtprx_par->payloadmapping;
 e = sm_vmprx_config_codec_speex(&codec_speexp);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_speex() failed");
 memset(&codec_speexp_mode, 0, sizeof(codec_speexp_mode));
 codec_speexp_mode.vmprx = vmprx;
 codec_speexp_mode.enh = rtprx_par->variant;
 e = sm_vmprx_config_codec_speex_mode(&codec_speexp_mode);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_speex_mode() failed");
 return 0;
}

static err_t config_codec_gsmfr(tSMVMPrxId vmprx, RTPRX_PAR *rtprx_par)
{
 SM_VMPRX_CODEC_GSMFR_PARMS cp;
 int e;
 memset(&cp, 0, sizeof(cp));
 cp.vmprx = vmprx;
 cp.payload_type = rtprx_par->payloadmapping;
 cp.variant = rtprx_par->variant;
 e = sm_vmprx_config_codec_gsmfr(&cp);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_gsmfr() failed");
 return 0;
}

static err_t config_codec_gsmhr(tSMVMPrxId vmprx, RTPRX_PAR *rtprx_par)
{
 SM_VMPRX_CODEC_GSMHR_PARMS cp;
 int e;
 memset(&cp, 0, sizeof(cp));
 cp.vmprx = vmprx;
 cp.payload_type = rtprx_par->payloadmapping;
 e = sm_vmprx_config_codec_gsmhr(&cp);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_gsmhr() failed");
 return 0;
}

static err_t config_codec_ilbc(tSMVMPrxId vmprx, RTPRX_PAR *rtprx_par)
{
 SM_VMPRX_CODEC_ILBC_PARMS cp;
 int e;
 if (rtprx_par->frame_len != 160 && rtprx_par->frame_len != 240) {
 	return error(0, "iLBC: can only handle frames which are 160 or 240 samples long");
 }
 memset(&cp, 0, sizeof(cp));
 cp.vmprx = vmprx;
 cp.payload_type = rtprx_par->payloadmapping;
 cp.frame_len = rtprx_par->frame_len / 8;
 cp.enhancer = rtprx_par->variant;
 e = sm_vmprx_config_codec_ilbc(&cp);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_ilbc() failed");
 return 0;
}

static err_t config_codec_isac(tSMVMPrxId vmprx, RTPRX_PAR *rtprx_par)
{
 SM_VMPRX_CODEC_ISAC_PARMS cp;
 int e;
 memset(&cp, 0, sizeof(cp));
 cp.vmprx = vmprx;
 cp.payload_type = rtprx_par->payloadmapping;
 e = sm_vmprx_config_codec_isac(&cp);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_isac() failed");
 return 0;
}

static err_t config_codec_smv(tSMVMPrxId vmprx, RTPRX_PAR *rtprx_par)
{
 SM_VMPRX_CODEC_SMV_PARMS cp;
 int e;
 memset(&cp, 0, sizeof(cp));
 cp.vmprx = vmprx;
 cp.payload_type = rtprx_par->payloadmapping;
 e = sm_vmprx_config_codec_smv(&cp);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_smv() failed");
 return 0;
}

static err_t config_codec_evrc(tSMVMPrxId vmprx, RTPRX_PAR *rtprx_par)
{
 SM_VMPRX_CODEC_EVRC_PARMS cp;
 int e;
 memset(&cp, 0, sizeof(cp));
 cp.vmprx = vmprx;
 cp.payload_type = rtprx_par->payloadmapping;

// 	kSMEVRCRTPModeHeaderless 1
//	kSMEVRCRTPModeRFC2658    2
//	kSMEVRCRTPModeRFC3558    3
 switch(rtprx_par->variant)
 {
	case 1:
		cp.rtp_mode = kSMEVRCRTPModeHeaderless;
		break;
	case 2:
		cp.rtp_mode = kSMEVRCRTPModeRFC2658;
		break;
	case 3:
		cp.rtp_mode = kSMEVRCRTPModeRFC3558;
		break;
	default:
		return error(0, "EVRC: variant should be 1, 2, 3 for kSMEVRCRTPModeHeaderless, kSMEVRCRTPModeRFC2658 or kSMEVRCRTPModeRFC3558");

 }

 cp.post_filter = 1;
 e = sm_vmprx_config_codec_evrc(&cp);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_evrc() failed");
 return 0;
}

static err_t config_codec_ulaw(tSMVMPrxId vmprx, RTPRX_PAR *rtprx_par)
{
 SM_VMPRX_CODEC_MULAW_PARMS cp;
 int e;
 memset(&cp, 0, sizeof(cp));
 cp.vmprx = vmprx;
 cp.payload_type = rtprx_par->payloadmapping;
 cp.plc_mode = rtprx_par->plc ? kSMPLCModeEnabled : kSMPLCModeDisabled;
 e = sm_vmprx_config_codec_mulaw(&cp);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_mulaw() failed");
 return 0;
}

static err_t config_codec_rfc4040(tSMVMPrxId vmprx, RTPRX_PAR *rtprx_par)
{
 SM_VMPRX_CODEC_RFC4040_PARMS cp;
 int e;
 memset(&cp, 0, sizeof(cp));
 cp.vmprx = vmprx;
 cp.payload_type = rtprx_par->payloadmapping;
 e = sm_vmprx_config_codec_rfc4040(&cp);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_rfc4040() failed");
 return 0;
}

static err_t config_codec_tetra(tSMVMPrxId vmprx, RTPRX_PAR *rtprx_par)
{
 SM_VMPRX_CODEC_TETRA_PARMS cp;
 int e;
 memset(&cp, 0, sizeof(cp));
 cp.vmprx = vmprx;
 cp.payload_type = rtprx_par->payloadmapping;
 e = sm_vmprx_config_codec_tetra(&cp);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_tetra() failed");
 return 0;
}

static err_t config_codec_rttext(tSMVMPrxId vmprx, RTPRX_PAR *rtprx_par)
{
 SM_VMPRX_CODEC_RTTEXT_PARMS cp;
 int e;
 memset(&cp, 0, sizeof(cp));
 cp.vmprx = vmprx;
 cp.payload_type = rtprx_par->payloadmapping;
 cp.redundant_payload_type = rtprx_par->variant;
 cp.out_of_order_delay = rtprx_par->bitrate;
 e = sm_vmprx_config_codec_rttext(&cp);
 if (e) return prosody_error(e, "sm_vmprx_config_codec_rttext() failed");
 return 0;
}

static struct {
	char *name;
	char *parms;
	err_t (*fn)(tSMVMPrxId vmptx, RTPRX_PAR *rtprx_par);
	err_t (*codec_specific_status)(tSMVMPrxId vmprx);
} codec[] = {
	{ "a", "plc", config_codec_alaw, NULL, },
	{ "alaw", "plc", config_codec_alaw, NULL, },
	{ "G711Alaw", "plc", config_codec_alaw, NULL, },
	{ "G711Mulaw", "plc", config_codec_ulaw, NULL, },
	{ "G722", "plc", config_codec_g722, NULL, },
	{ "G722.1", "bitrate [24000, 32000]", config_codec_g722_1, NULL, },
	{ "G723.1", "variant [post filter]", config_codec_g723_1, NULL, },
	{ "G726", "plc bitrate [16000 to 40000] variant [aal2]", config_codec_g726, NULL, },
	{ "G728", "bitrate [9600, 12800 or 16000] variant [post filter]", config_codec_g728, NULL, },
	{ "G729", "", config_codec_g729, NULL, },
	{ "G729AB", "", config_codec_g729ab, NULL, },
	{ "G729D", "", config_codec_g729d, NULL, },
	{ "G729E", "", config_codec_g729e, NULL, },
	{ "MELPe", "", config_codec_melpe, NULL, },
	{ "AMR-NB", "variant [0 non-aligned, 1 aligned, 2 gsmefr]", config_codec_amrnb, codec_status_amrnb, },
	{ "AMR-WB", "variant [aligned]", config_codec_amrwb, codec_status_amrwb, },
	{ "GSM-EFR", "", config_codec_gsmefr, NULL, },
	{ "gsmfr", "variant [msgsm mode]", config_codec_gsmfr, NULL, },
	{ "gsmhr", "", config_codec_gsmhr, NULL, },
	{ "ilbc", "variant [enhancer] frame_len [160, 240]", config_codec_ilbc, NULL, },
	{ "isac", "", config_codec_isac, NULL, },
	{ "opus", "variant [decode-fec]", config_codec_opus, NULL, },
	{ "smv", "", config_codec_smv, NULL, },
	{ "evrc", "variant [1 (Headerless), 2 (RFC2658), 3 (RFC3558)]", config_codec_evrc, NULL, },
	{ "4040", "", config_codec_rfc4040, NULL, },
	{ "RFC4040", "", config_codec_rfc4040, NULL, },
	{ "u", "plc", config_codec_ulaw, NULL, },
	{ "ulaw", "plc", config_codec_ulaw, NULL, },
	{ "L8", "plc", config_codec_l8, NULL, },
	{ "L16", "plc", config_codec_l16, NULL, },
	{ "TETRA", "", config_codec_tetra, NULL, },
	{ "SPEEX", "variant [enhancer] ", config_codec_speex, NULL, },
	{ "SILK", "plc", config_codec_silk, NULL, },
	{ "rtt", "variant [redundant pt] bitrate [out of order delay]", config_codec_rttext, NULL, },
};

err_t rtprx_config_vmprx(tSMVMPrxId vmprx, RTPRX_PAR* rtprx_par)
{
 err_t err;
 int e;
 if(rtprx_par->init_ms && rtprx_par->max_ms) {
	struct sm_vmprx_jitter_parms jitterp;
	memset(&jitterp,0,sizeof(jitterp));
	jitterp.vmprx = vmprx;
	jitterp.max_delay_ms = rtprx_par->max_ms;
	jitterp.initial_delay_ms = rtprx_par->init_ms;
	e = sm_vmprx_config_jitter(&jitterp);
	if (e) return prosody_error(e,"sm_vmprx_config_jitter() failed");
 }
 if (rtprx_par->ad_target_delay || rtprx_par->ad_freq_upper_tolerance || rtprx_par->ad_freq_lower_tolerance) {
	struct sm_vmprx_config_jitter_mode_parms jittermp;
	memset(&jittermp,0,sizeof(jittermp));
	jittermp.vmprx = vmprx;
	jittermp.mode = kSMVMPrxJitterBufferModeAdaptive;
	jittermp.u.adaptive.target_delay = rtprx_par->ad_target_delay;
	jittermp.u.adaptive.freq_upper_tolerance = rtprx_par->ad_freq_upper_tolerance;
	jittermp.u.adaptive.freq_lower_tolerance = rtprx_par->ad_freq_lower_tolerance;
	e = sm_vmprx_config_jitter_mode(&jittermp);
	if (e) return prosody_error(e,"sm_vmprx_config_jitter_mode() failed");
 }
 err = rtprx_par->config(vmprx, rtprx_par);
 if (err) return err;
 if(rtprx_par->regen_tones || rtprx_par->detect_tones) {
	SM_VMPRX_CODEC_RFC2833_PARMS rfc2833p;
	struct sm_vmprx_tone_parms tonep;
	memset(&tonep,0,sizeof(tonep));
 	memset(&rfc2833p, 0, sizeof(rfc2833p));
 	rfc2833p.vmprx = vmprx;
 	rfc2833p.payload_type = rtprx_par->tone_payload_mapping;
 	e = sm_vmprx_config_codec_rfc2833(&rfc2833p);
 	if (e) return prosody_error(e, "sm_vmprx_config_codec_rfc2833() failed");
	// turn it on
	tonep.vmprx = vmprx;
	tonep.regen_tones = rtprx_par->regen_tones;
	tonep.detect_tones = rtprx_par->detect_tones;
	tonep.enforce_tone_spacing = rtprx_par->enforce_tone_spacing;
	e =  sm_vmprx_config_tones(&tonep);
	if (e) return prosody_error(e,"sm_vmprx_config_tones() failed");
 }
 if (rtprx_par->handle_cn) {
	SM_VMPRX_CODEC_COMFORT_NOISE_PARMS cnp;
 	memset(&cnp, 0, sizeof(cnp));
 	cnp.vmprx = vmprx;
 	cnp.payload_type = rtprx_par->comfort_noise_payload_mapping;
 	e = sm_vmprx_config_codec_comfort_noise(&cnp);
 	if (e) return prosody_error(e, "sm_vmprx_config_codec_comfort_noise() failed");
 }
 if (rtprx_par->report_jb_resync) {
	SM_VMPRX_CONFIG_JITTER_RESYNC_NOTIFY_PARMS jnp;
	memset(&jnp, 0, sizeof(jnp));
	jnp.vmprx = vmprx;
	jnp.enable = 1;
	e = sm_vmprx_config_jitter_resync_notify(&jnp);
	if (e) return prosody_error(e, "sm_vmprx_config_jitter_resync_notify() failed");
	else printf("jbresync enabled\n");
 }
 return 0;
}

void rtprx_dump_codecs(FILE* f)
{
 const unsigned colwid = 16;
 unsigned col = 0;
 unsigned i;
 fprintf(f, "Codecs:");
 for (i=0; i < arlen(codec); i++) {
	unsigned spc;
	if (!(i & 3)) {
		putc('\n', f);
		col = 0;
	}
	spc = colwid - (col-8) % colwid;
	col += spc;
	while (spc--) putc(' ', f);
	fputs(codec[i].name, f);
	col += strlen(codec[i].name);
 }
 putc('\n', f);
}

void rtprx_dump_codec_parms(FILE* f)
{
 unsigned i;
 fprintf(f, "Codec parameters:\n");
 for (i=0; i < arlen(codec); i++) {
	fprintf(f, "        %-16s %s\n", codec[i].name, codec[i].parms);
 }
 putc('\n', f);
}

int rtprx_init_par(RTPRX_PAR* rtprx_par, const char* name)
{
 unsigned u;
 memset(rtprx_par,0,sizeof(RTPRX_PAR));
 for (u=0; u < arlen(codec) ; u++) {
	if (!strcmp(name, codec[u].name)) {
		rtprx_par->config = codec[u].fn;
		rtprx_par->codec_specific_status = codec[u].codec_specific_status;
		return 0;
	}
 }
 return 1;
}
