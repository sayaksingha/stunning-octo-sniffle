#include "rtptx_codec.h"
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

static err_t config_codec_alaw(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 SM_VMPTX_CODEC_ALAW_PARMS cp;
 int e;
 if (rtptx_par->pticks < 8 || rtptx_par->pticks % 8) {
 	return error(0, "Alaw: can only handle positive multiples of 8 ticks per packet");
 }
 memset(&cp, 0, sizeof(cp));
 cp.vmptx = vmptx;
 cp.payload_type = rtptx_par->payloadmapping;
 cp.ptime = rtptx_par->pticks / 8;
 cp.VADMode = rtptx_par->vadmode;
 e = sm_vmptx_config_codec_alaw(&cp);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_alaw() failed");
 return 0;
}

static err_t config_codec_l8(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 SM_VMPTX_CODEC_L8_PARMS cp;
 int e;
 if (rtptx_par->pticks < 8 || rtptx_par->pticks % 8) {
 	return error(0, "L8: can only handle positive multiples of 8 ticks per packet");
 }
 memset(&cp, 0, sizeof(cp));
 cp.vmptx = vmptx;
 cp.payload_type = rtptx_par->payloadmapping;
 cp.ptime = rtptx_par->pticks / 8;
 cp.VADMode = rtptx_par->vadmode;
 e = sm_vmptx_config_codec_l8(&cp);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_l8() failed");
 return 0;
}

static err_t config_codec_l16(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 SM_VMPTX_CODEC_L16_PARMS cp;
 int e;
 if (rtptx_par->pticks < 8 || rtptx_par->pticks % 8) {
 	return error(0, "L16: can only handle positive multiples of 8 ticks per packet");
 }
 memset(&cp, 0, sizeof(cp));
 cp.vmptx = vmptx;
 cp.payload_type = rtptx_par->payloadmapping;
 cp.ptime = rtptx_par->pticks / 8;
 cp.VADMode = rtptx_par->vadmode;
 e = sm_vmptx_config_codec_l16(&cp);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_l16() failed");
 return 0;
}

static err_t config_codec_g723_1(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 SM_VMPTX_CODEC_G723_1_PARMS cp;
 int e;
 if (rtptx_par->pticks < 240 || rtptx_par->pticks % 240) {
 	return error(0, "G.723.1: can only handle positive multiples of 240 ticks per packet");
 }
 memset(&cp, 0, sizeof(cp));
 cp.vmptx = vmptx;
 cp.payload_type = rtptx_par->payloadmapping;
 cp.high_pass_filter = rtptx_par->high_pass_filter;
 cp.rate = rtptx_par->bitrate;
 if(cp.rate != 6300 && cp.rate != 5300)
	 return error(0, "G.723.1 requires the bitrate to be set to 6300 or 5300");
 cp.silence_compression = rtptx_par->vadmode;
 cp.frames_per_packet = rtptx_par->pticks / 240;
 e = sm_vmptx_config_codec_g723_1(&cp);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_g723_1() failed");
 return 0;
}

static err_t config_codec_g722(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 SM_VMPTX_CODEC_G722_PARMS cp;
 int e;
 if (rtptx_par->pticks < 160 || rtptx_par->pticks % 160) {
 	return error(0, "G.722: can only handle positive multiples of 160 ticks per packet");
 }
 memset(&cp, 0, sizeof(cp));
 cp.vmptx = vmptx;
 cp.payload_type = rtptx_par->payloadmapping;
 cp.packetlen = rtptx_par->pticks;
 e = sm_vmptx_config_codec_g722(&cp);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_g722() failed");
 return 0;
}

static err_t config_codec_g722_1(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 SM_VMPTX_CODEC_G722_1_PARMS cp;
 int e;
 if (rtptx_par->pticks < 320 || rtptx_par->pticks % 320) {
 	return error(0, "G.722.1: can only handle positive multiples of 320 ticks per packet");
 }
 if (rtptx_par->bitrate != 24000 && rtptx_par->bitrate != 32000) {
 	return error(0, "G.722.1: can only handle bitrates of 24000 or 32000");
 }
 memset(&cp, 0, sizeof(cp));
 cp.vmptx = vmptx;
 cp.payload_type = rtptx_par->payloadmapping;
 cp.bitrate = rtptx_par->bitrate;
 cp.frames_per_packet = rtptx_par->pticks / 320;
 e = sm_vmptx_config_codec_g722_1(&cp);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_g722_1() failed");
 return 0;
}

static err_t config_codec_silk(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 SM_VMPTX_CODEC_SILK_PARMS cp;
 int e;
 if (rtptx_par->pticks < rtptx_par->frame_len ||
     rtptx_par->pticks > 5 * rtptx_par->frame_len ||
     rtptx_par->pticks % rtptx_par->frame_len) {
 	return error(0, "SILK: can only handle between 1 and 5 whole frames per packet");
 }
 if (rtptx_par->bitrate < 5000 || rtptx_par->bitrate > 100000) {
 	return error(0, "SILK: can only handle bitrates between 5000 and 100000");
 }
 if ((rtptx_par->variant & 3) > 2) {
 	return error(0, "SILK: complexity (2 LSBs of variant parameter) must be 0, 1, or 2");
 }
 memset(&cp, 0, sizeof(cp));
 cp.vmptx = vmptx;
 cp.payload_type = rtptx_par->payloadmapping;
 cp.sample_rate = (rtptx_par->sample_rate) ? rtptx_par->sample_rate : 8000;  // get from -oR propogated value
 cp.internal_rate = cp.sample_rate < 24000 ? cp.sample_rate : 24000;    // set this as big as is allowed         
 cp.bit_rate = rtptx_par->bitrate;
 cp.frames_per_packet = rtptx_par->pticks / rtptx_par->frame_len;
 cp.packet_loss = 0;
 cp.complexity = rtptx_par->variant & 3;
 cp.use_fec = rtptx_par->variant & 4;
 cp.use_dtx = rtptx_par->vadmode;
 e = sm_vmptx_config_codec_silk(&cp);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_silk() failed");
 return 0;
}

static err_t config_codec_opus(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 SM_VMPTX_CODEC_OPUS_PARMS cp;
 int e;
 int br;
 if (rtptx_par->pticks < 48*10 ||
     rtptx_par->pticks > 60*48 ||
     (rtptx_par->pticks % (48*20) && (rtptx_par->pticks != 48*10))  ) {
 	return error(0, "opus: can only handle 10 (pticks=480), 20 (pticks = 960), 40 or 60ms frames");
 }
 br = (int) rtptx_par->bitrate;
 if ((br > 0) && (br < 500 || br > 512000)) {
 	return error(0, "opus: can only handle bitrates of 0 (auto), -1 (max), or between 500 and 512000");
 }
 if ((rtptx_par->variant & 15) > 10) {
 	return error(0, "opus: complexity (4 LSBs of variant parameter) must be in 0..10");
 }
 memset(&cp, 0, sizeof(cp));
 cp.vmptx = vmptx;
 cp.payload_type = rtptx_par->payloadmapping;
 cp.sample_rate = (rtptx_par->sample_rate) ? rtptx_par->sample_rate : 8000;   // get from -oR propogated value
 cp.bit_rate = br;
 cp.ptime = rtptx_par->pticks / 48;

 cp.packet_loss = 0;
 cp.complexity = rtptx_par->variant & 15;
 cp.use_fec = (rtptx_par->variant >> 4) & 1;
 cp.rate_control = (rtptx_par->variant >> 5) & 3;
 cp.signal_type_hint = (rtptx_par->variant >> 7) & 3;
 cp.application_type = (rtptx_par->variant >> 9) & 3;
 cp.disable_prediction = (rtptx_par->variant >> 11) & 1;
 cp.use_dtx = rtptx_par->vadmode;
 e = sm_vmptx_config_codec_opus(&cp);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_opus() failed");
 return 0;
}

static err_t config_codec_g726(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 SM_VMPTX_CODEC_G726_PARMS cp;
 int e;
 if (rtptx_par->pticks < 80 || rtptx_par->pticks % 80) {
 	return error(0, "G.726: can only handle positive multiples of 80 ticks per packet");
 }
 memset(&cp, 0, sizeof(cp));
 cp.vmptx = vmptx;
 cp.payload_type = rtptx_par->payloadmapping;
 cp.bits = rtptx_par->bitrate / 8000;
 cp.variant = rtptx_par->variant;
 if (cp.bits < 2 || cp.bits > 5) 
	 return error(0, "G.726 requires the bitrate to be set between 16000 and 40000");
 cp.VADMode = rtptx_par->vadmode;
 cp.packetlen = rtptx_par->pticks;
 e = sm_vmptx_config_codec_g726(&cp);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_g726() failed");
 return 0;
}

static err_t config_codec_g728(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 SM_VMPTX_CODEC_G728_PARMS cp;
 int e;
 if (rtptx_par->pticks < 80 || rtptx_par->pticks % 80) {
 	return error(0, "G.728: can only handle positive multiples of 80 ticks per packet");
 }
 memset(&cp, 0, sizeof(cp));
 cp.vmptx = vmptx;
 cp.payload_type = rtptx_par->payloadmapping;
 if (rtptx_par->bitrate != 9600 && rtptx_par->bitrate != 12800 && rtptx_par->bitrate != 16000) 
	 return error(0, "G.728 requires bitrate 9600, 12800 or 16000");
 cp.rate = rtptx_par->bitrate;
 cp.VADMode = rtptx_par->vadmode;
 cp.ptime = rtptx_par->pticks / 8;
 e = sm_vmptx_config_codec_g728(&cp);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_g728() failed");
 return 0;
}

static err_t config_codec_g729(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 SM_VMPTX_CODEC_G729I_PARMS cp;
 int e;
 if (rtptx_par->pticks < 80 || rtptx_par->pticks % 80) {
 	return error(0, "G.729: can only handle positive multiples of 80 ticks per packet");
 }
 memset(&cp, 0, sizeof(cp));
 cp.vmptx = vmptx;
 cp.payload_type = rtptx_par->payloadmapping;
 cp.g729_mode = kSMG729IModeG729;
 cp.VADMode = rtptx_par->vadmode;
 cp.ptime = rtptx_par->pticks / 8;
 e = sm_vmptx_config_codec_g729i(&cp);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_g729() failed");
 return 0;
}

static err_t config_codec_g729ab(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 SM_VMPTX_CODEC_G729AB_PARMS cp;
 int e;
 if (rtptx_par->pticks < 80 || rtptx_par->pticks % 80) {
 	return error(0, "G.729AB: can only handle positive multiples of 80 ticks per packet");
 }
 memset(&cp, 0, sizeof(cp));
 cp.vmptx = vmptx;
 cp.payload_type = rtptx_par->payloadmapping;
 cp.VADMode = rtptx_par->vadmode;
 cp.ptime = rtptx_par->pticks / 8;
 e = sm_vmptx_config_codec_g729ab(&cp);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_g729ab() failed");
 return 0;
}

static err_t config_codec_g729d(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 SM_VMPTX_CODEC_G729I_PARMS cp;
 int e;
 if (rtptx_par->pticks < 80 || rtptx_par->pticks % 80) {
 	return error(0, "G.729D: can only handle positive multiples of 80 ticks per packet");
 }
 memset(&cp, 0, sizeof(cp));
 cp.vmptx = vmptx;
 cp.payload_type = rtptx_par->payloadmapping;
 cp.g729_mode = kSMG729IModeG729D;
 cp.VADMode = rtptx_par->vadmode;
 cp.ptime = rtptx_par->pticks / 8;
 e = sm_vmptx_config_codec_g729i(&cp);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_g729d() failed");
 return 0;
}

static err_t config_codec_melpe(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 SM_VMPTX_CODEC_MELPE_PARMS cp;
 int e;
 if (rtptx_par->pticks < 80 || rtptx_par->pticks % 80) {
 	return error(0, "MELPe: can only handle positive multiples of 80 ticks per packet");
 }
 memset(&cp, 0, sizeof(cp));
 cp.vmptx = vmptx;
 cp.payload_type = rtptx_par->payloadmapping;
 cp.ptime = rtptx_par->pticks / 8;
 e = sm_vmptx_config_codec_melpe(&cp);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_melpe() failed");
 return 0;
}

static err_t config_codec_g729e(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 SM_VMPTX_CODEC_G729I_PARMS cp;
 int e;
 if (rtptx_par->pticks < 80 || rtptx_par->pticks % 80) {
 	return error(0, "G.729E: can only handle positive multiples of 80 ticks per packet");
 }
 memset(&cp, 0, sizeof(cp));
 cp.vmptx = vmptx;
 cp.payload_type = rtptx_par->payloadmapping;
 cp.g729_mode = kSMG729IModeG729E;
 cp.VADMode = rtptx_par->vadmode;
 cp.ptime = rtptx_par->pticks / 8;
 e = sm_vmptx_config_codec_g729i(&cp);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_g729e() failed");
 return 0;
}

static err_t config_codec_gsmefr(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 SM_VMPTX_CODEC_GSMEFR_PARMS codec_gsmefr;
 int e;
 if (rtptx_par->pticks < 160 || rtptx_par->pticks % 160) {
 	return error(0, "GSM-EFR: can only handle positive multiples of 160 ticks per packet");
 }
 memset(&codec_gsmefr, 0, sizeof(codec_gsmefr));
 codec_gsmefr.vmptx = vmptx;
 codec_gsmefr.payload_type = rtptx_par->payloadmapping;
 codec_gsmefr.VADMode = rtptx_par->vadmode;
 codec_gsmefr.frames_per_packet = rtptx_par->pticks / 160;
 e = sm_vmptx_config_codec_gsmefr(&codec_gsmefr);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_gsmefr() failed");
 return 0;
}

static err_t config_codec_amrnb(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 SM_VMPTX_CONFIG_CODEC_AMRNB_MODE_PARMS codec_amrp_mode;
 SM_VMPTX_CODEC_AMRNB_PARMS codec_amrp;
 int e;
 if (rtptx_par->pticks < 160 || rtptx_par->pticks % 160) {
 	return error(0, "AMRNB: can only handle positive multiples of 160 ticks per packet");
 }
 memset(&codec_amrp, 0, sizeof(codec_amrp));
 codec_amrp.vmptx = vmptx;
 codec_amrp.aligned = rtptx_par->variant == 1;
 codec_amrp.gsmefr = rtptx_par->variant == 2;
 codec_amrp.payload_type = rtptx_par->payloadmapping;
 codec_amrp.VADMode = rtptx_par->vadmode;
 codec_amrp.frames_per_packet = rtptx_par->pticks / 160;
 e = sm_vmptx_config_codec_amrnb(&codec_amrp);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_amrnb() failed");
 memset(&codec_amrp_mode, 0, sizeof(codec_amrp_mode));
 codec_amrp_mode.vmptx = vmptx;
 codec_amrp_mode.bitrate = rtptx_par->bitrate;
 if(codec_amrp_mode.bitrate < 4750 || codec_amrp_mode.bitrate > 12200)
	return prosody_error(e, "amrnb requires bitrate 4750, 5150, 5900, 6700, 7400, 7950, 10200 or 12200\n");
 e = sm_vmptx_config_codec_amrnb_mode(&codec_amrp_mode);
 if (e) return prosody_error(e, "sm_vmptx_codec_amrnb_mode() failed");
 return 0;
}

static err_t config_codec_amrwb(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 SM_VMPTX_CONFIG_CODEC_AMRWB_MODE_PARMS codec_amrp_mode;
 SM_VMPTX_CODEC_AMRWB_PARMS codec_amrp;
 int e;
 if (rtptx_par->pticks < 320 || rtptx_par->pticks % 320) {
 	return error(0, "AMRWB: can only handle positive multiples of 320 ticks per packet");
 }
 memset(&codec_amrp, 0, sizeof(codec_amrp));
 codec_amrp.vmptx = vmptx;
 codec_amrp.aligned = rtptx_par->variant;
 codec_amrp.payload_type = rtptx_par->payloadmapping;
 codec_amrp.VADMode = rtptx_par->vadmode;
 codec_amrp.frames_per_packet = rtptx_par->pticks / 320;
 e = sm_vmptx_config_codec_amrwb(&codec_amrp);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_amrwb() failed");
 memset(&codec_amrp_mode, 0, sizeof(codec_amrp_mode));
 codec_amrp_mode.vmptx = vmptx;
 codec_amrp_mode.bitrate = rtptx_par->bitrate;
 if(codec_amrp_mode.bitrate < 6600 || codec_amrp_mode.bitrate > 23850)
	return prosody_error(e, "amrwb requires bitrate 6600, 8850, 12650, 14250, 15850, 18250, 19850, 23050 or 23850\n");
 e = sm_vmptx_config_codec_amrwb_mode(&codec_amrp_mode);
 if (e) return prosody_error(e, "sm_vmptx_codec_amrwb_mode() failed");
 return 0;
}

static err_t config_codec_speex(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 SM_VMPTX_CODEC_SPEEX_PARMS codec_speexp;
 SM_VMPTX_CODEC_SPEEX_MODE_PARMS codec_speexp_mode;
 int e;
 if(rtptx_par->bitrate < 2150 || rtptx_par->bitrate > 24600) {
 	return prosody_error(0, "speex data rates should be between 2150 and 24600\n");
 }
 memset(&codec_speexp, 0, sizeof(codec_speexp));
 codec_speexp.vmptx = vmptx;
 codec_speexp.payload_type = rtptx_par->payloadmapping;
 codec_speexp.VADMode = rtptx_par->vadmode;
 codec_speexp.frames_per_packet = rtptx_par->pticks / 80;
 e = sm_vmptx_config_codec_speex(&codec_speexp);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_speex() failed");
 memset(&codec_speexp_mode, 0, sizeof(codec_speexp_mode));
 codec_speexp_mode.bitrate = rtptx_par->bitrate;
 codec_speexp_mode.denoiser = (rtptx_par->variant & 0x0f00) >> 8;
 codec_speexp_mode.complexity = (rtptx_par->variant & 0x00f0) >> 4;
 codec_speexp_mode.quality = rtptx_par->variant & 0x000f;
 codec_speexp_mode.vmptx = vmptx;
 e = sm_vmptx_config_codec_speex_mode(&codec_speexp_mode);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_speex_mode() failed");
 return 0;
}

static err_t config_codec_gsmfr(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 SM_VMPTX_CODEC_GSMFR_PARMS cp;
 int e;
 if (rtptx_par->pticks < 80 || rtptx_par->pticks % 80) {
 	return error(0, "GSMFR: can only handle positive multiples of 80 ticks per packet");
 }
 memset(&cp, 0, sizeof(cp));
 cp.vmptx = vmptx;
 cp.payload_type = rtptx_par->payloadmapping;
 cp.variant = rtptx_par->variant;
 cp.VADMode = rtptx_par->vadmode;
 cp.frames_per_packet = rtptx_par->pticks / 80;
 if(cp.variant && cp.frames_per_packet % 2) {
	return error(0, "GSMFR: can only handle an even number of frames per packet");
 }
 e = sm_vmptx_config_codec_gsmfr(&cp);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_gsmfr() failed");
 return 0;
}

static err_t config_codec_gsmhr(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 SM_VMPTX_CODEC_GSMHR_PARMS cp;
 int e;
 if (rtptx_par->pticks < 80 || rtptx_par->pticks % 80) {
 	return error(0, "GSMHR: can only handle positive multiples of 80 ticks per packet");
 }
 memset(&cp, 0, sizeof(cp));
 cp.vmptx = vmptx;
 cp.payload_type = rtptx_par->payloadmapping;
 cp.VADMode = rtptx_par->vadmode;
 cp.frames_per_packet = rtptx_par->pticks / 80;
 e = sm_vmptx_config_codec_gsmhr(&cp);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_gsmhr() failed");
 return 0;
}

static err_t config_codec_ilbc(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 SM_VMPTX_CODEC_ILBC_PARMS cp;
 int e;
 if (rtptx_par->frame_len != 160 && rtptx_par->frame_len != 240) {
 	return error(0, "iLBC: can only handle frames which are 160 or 240 samples long");
 }
 if (rtptx_par->pticks < rtptx_par->frame_len || rtptx_par->pticks % rtptx_par->frame_len) {
 	return error(0, "iLBC: can only handle positive multiples of whole frames per packet");
 }
 memset(&cp, 0, sizeof(cp));
 cp.vmptx = vmptx;
 cp.payload_type = rtptx_par->payloadmapping;
 cp.frame_len = rtptx_par->frame_len / 8;
 cp.VADMode = rtptx_par->vadmode;
 cp.frames_per_packet = rtptx_par->pticks / rtptx_par->frame_len;
 e = sm_vmptx_config_codec_ilbc(&cp);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_ilbc() failed");
 return 0;
}

static err_t config_codec_isac(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 SM_VMPTX_CODEC_ISAC_PARMS cp;
 SM_VMPTX_ISAC_RATE_PARMS rp;
 int e;
 memset(&cp, 0, sizeof(cp));
 memset(&rp, 0, sizeof(rp));
 cp.vmptx = vmptx;
 cp.payload_type = rtptx_par->payloadmapping;

 rp.vmptx = vmptx;
 rp.rate = rtptx_par->bitrate;
 rp.ptime = rtptx_par->pticks / 8;

 if (rp.ptime != 30 && rp.ptime != 60) {
	 return error(0, "isac: can only handle ptime of 30ms or 60ms (pticks of 240 or 480)");
 }
 if (rp.rate < 10000 || rp.rate > 32000) {
	 return error(0, "isac: rate must be between 10000 and 32000\n");
 }

 e = sm_vmptx_config_codec_isac(&cp);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_isac() failed");

 e = sm_vmptx_config_isac_rate(&rp);
 if (e) {
	return prosody_error(e, "sm_vmptx_config_isac_rate() failed");
 }
 return 0;
}

static err_t config_codec_smv(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 SM_VMPTX_CODEC_SMV_PARMS cp;
 int e;
 if (rtptx_par->pticks < 160 || rtptx_par->pticks % 160) {
 	return error(0, "SMV: can only handle positive multiples of whole frames per packet");
 }
 memset(&cp, 0, sizeof(cp));
 cp.vmptx = vmptx;
 cp.payload_type = rtptx_par->payloadmapping;
 cp.VADMode = rtptx_par->vadmode;
 cp.frames_per_packet = rtptx_par->pticks / 160;
 e = sm_vmptx_config_codec_smv(&cp);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_smv() failed");
 return 0;
}

static err_t config_codec_evrc(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 SM_VMPTX_CODEC_EVRC_PARMS cp;
 int e;
 if (rtptx_par->pticks < 160 || rtptx_par->pticks % 160) {
 	return error(0, "EVRC: can only handle multiples of whole frames (20ms) per packet");
 }
 memset(&cp, 0, sizeof(cp));
 cp.vmptx = vmptx;
 cp.VADMode = rtptx_par->vadmode;
 cp.payload_type = rtptx_par->payloadmapping;
 cp.frames_per_packet = rtptx_par->pticks / 160;

// 	kSMEVRCRTPModeHeaderless 1
//	kSMEVRCRTPModeRFC2658    2
//	kSMEVRCRTPModeRFC3558    3
 switch(rtptx_par->variant)
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

 if(cp.frames_per_packet > 1 && cp.rtp_mode != kSMEVRCRTPModeRFC3558)
	return error(0, "EVRC: multiple frames per packet is only available with mode kSMEVRCRTPModeRFC3558");

// kSMEVRCRateEighth 1
// kSMEVRCRateHalf 2 
// kSMEVRCRateFull 3 
 switch(rtptx_par->bitrate)
 {
	case 1:
		cp.min_rate = kSMEVRCRateEighth;
		break;
	case 2:
		cp.min_rate = kSMEVRCRateHalf;
		break;
	case 3:
		cp.min_rate = kSMEVRCRateFull;
		break;
	default:
		return error(0, "EVRC: bitrate (for min_rate) should be 1, 2, 3 for kSMEVRCRateEighth, kSMEVRCRateHalf or kSMEVRCRateFull");

 }

 cp.max_rate = kSMEVRCRateFull;
 cp.high_pass_filter = rtptx_par->high_pass_filter;
 cp.noise_suppression_filter = 1;
 e = sm_vmptx_config_codec_evrc(&cp);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_evrc() failed");
 return 0;
}

static err_t config_codec_ulaw(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 SM_VMPTX_CODEC_MULAW_PARMS cp;
 int e;
 memset(&cp, 0, sizeof(cp));
 if (rtptx_par->pticks < 8 || rtptx_par->pticks % 8) {
 	return error(0, "mulaw: can only handle positive multiples of 8 ticks per packet");
 }
 cp.vmptx = vmptx;
 cp.payload_type = rtptx_par->payloadmapping;
 cp.ptime = rtptx_par->pticks / 8;
 cp.VADMode = rtptx_par->vadmode;
 e = sm_vmptx_config_codec_mulaw(&cp);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_mulaw() failed");
 return 0;
}

static err_t config_codec_rfc4040(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 SM_VMPTX_CODEC_RFC4040_PARMS cp;
 int e;
 memset(&cp, 0, sizeof(cp));
 cp.vmptx = vmptx;
 cp.payload_type = rtptx_par->payloadmapping;
 cp.packetlen = rtptx_par->pticks;
 e = sm_vmptx_config_codec_rfc4040(&cp);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_rfc4040() failed");
 return 0;
}

static err_t config_codec_tetra(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 SM_VMPTX_CODEC_TETRA_PARMS cp;
 int e;
 memset(&cp, 0, sizeof(cp));
 cp.vmptx = vmptx;
 cp.payload_type = rtptx_par->payloadmapping;
 e = sm_vmptx_config_codec_tetra(&cp);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_tetra() failed");
 return 0;
}

static err_t config_codec_rttext(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 SM_VMPTX_CODEC_RTTEXT_PARMS cp;
 int e;
 memset(&cp, 0, sizeof(cp));
 cp.vmptx = vmptx;
 cp.payload_type = rtptx_par->payloadmapping;
 cp.redundant_payload_type = rtptx_par->variant;
 cp.redundancy_level = rtptx_par->frame_len;
 cp.buffer_time = rtptx_par->pticks;
 cp.idle_time = rtptx_par->bitrate;
 e = sm_vmptx_config_codec_rttext(&cp);
 if (e) return prosody_error(e, "sm_vmptx_config_codec_rttext() failed");
 return 0;
}

static struct {
	char *name;
	char *parms;
	err_t (*fn)(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par);
} codec[] = {
	{ "a", "", config_codec_alaw, },
	{ "alaw", "", config_codec_alaw, },
	{ "G711Alaw", "", config_codec_alaw, },
	{ "G711Mulaw", "", config_codec_ulaw, },
	{ "G722", "pticks", config_codec_g722, },
	{ "G722.1", "pticks bitrate [24000, 32000]", config_codec_g722_1, },
	{ "G723.1", "pticks [240] vadmode [sil comp] bitrate [6300, 5300] high_pass_filter", config_codec_g723_1, },
	{ "G726", "pticks vadmode bitrate [16000 to 40000] variant [aal2]", config_codec_g726, },
	{ "G728", "pticks vadmode bitrate [9600, 12800 or 16000]", config_codec_g728, },
	{ "G729", "pticks vadmode", config_codec_g729, },
	{ "G729AB", "pticks vadmode", config_codec_g729ab, },
	{ "G729D", "pticks vadmode", config_codec_g729d, },
	{ "G729E", "pticks vadmode", config_codec_g729e, },
	{ "MELPe", "pticks", config_codec_melpe, },
	{ "AMR-NB", "pticks vadmode bitrate [4750 to 12200] variant [0 non-aligned, 1 aligned, 2 gsmefr]", config_codec_amrnb, },
	{ "AMR-WB", "pticks vadmode bitrate [6600 to 23850] variant [aligned]", config_codec_amrwb, },
	{ "GSM-EFR", "pticks vadmode", config_codec_gsmefr, },
	{ "gsmfr", "pticks vadmode variant [msgsm mode]", config_codec_gsmfr, },
	{ "gsmhr", "pticks vadmode", config_codec_gsmhr, },
	{ "ilbc", "pticks vadmode frame_len [160, 240]", config_codec_ilbc, },
	{ "isac", "pticks [240, 480] bitrate [10000 to 32000]", config_codec_isac, },
	{ "opus", "pticks [no. 48K ticks for 10, 20, 40, or 60ms] vadmode bitrate [-1,0,500 to 512000] variant [(dp<<11)|(app<<9)|(hint<<7)|(rc<<5)|(fec<<4)|(complexity)]", config_codec_opus, },
	{ "smv", "pticks vadmode", config_codec_smv, },
	{ "evrc", "pticks [160] vadmode bitrate [1 Eighth, 2 Half, 3 Full] variant [1 Headerless, 2 RFC2658, 2 RFC3558] high_pass_filter", config_codec_evrc, },
	{ "4040", "", config_codec_rfc4040, },
	{ "RFC4040", "", config_codec_rfc4040, },
	{ "u", "", config_codec_ulaw, },
	{ "ulaw", "", config_codec_ulaw, },
	{ "L8", "", config_codec_l8, },
	{ "L16", "", config_codec_l16, },
	{ "TETRA", "", config_codec_tetra, },
	{ "SPEEX", "pticks vadmode variant [quality|(complexity<<4)|(denoiser<<8)] bitrate [2150 to 24600] ", config_codec_speex, },
	{ "SILK", "pticks [no. samples in 20, 40, 60, 80 or 100ms] vadmode bitrate [5000 to 100000] variant [(fec<<2)|(complexity)]", config_codec_silk, },
	{ "rtt", "variant [redundant pt] frame_len [redundancy level] pticks [buffer time ms] bitrate [idle time ms]", config_codec_rttext, },
};

err_t rtptx_config_vmptx(tSMVMPtxId vmptx, RTPTX_PAR *rtptx_par)
{
 err_t err;
 int e;
 err = rtptx_par->config(vmptx, rtptx_par);
 if (err) return err;
 if (rtptx_par->vadmode == kSMVMPTxVADModeComfortNoise) {
	SM_VMPTX_CODEC_COMFORT_NOISE_PARMS ncp;
 	memset(&ncp, 0, sizeof(ncp));
 	ncp.vmptx = vmptx;
 	ncp.payload_type = rtptx_par->comfort_noise_payload_mapping;
 	e = sm_vmptx_config_codec_comfort_noise(&ncp);
 	if (e) return prosody_error(e, "sm_vmptx_config_codec_comfort_noise() failed");
 }
 return 0;
}


void rtptx_dump_codecs(FILE* f)
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

void rtptx_dump_codec_parms(FILE* f)
{
 unsigned i;
 fprintf(f, "Codec parameters:\n");
 for (i=0; i < arlen(codec); i++) {
	fprintf(f, "        %-16s %s\n", codec[i].name, codec[i].parms);
 }
 putc('\n', f);
}

int rtptx_init_par(RTPTX_PAR* rtptx_par, const char* name)
{
 unsigned u;
 memset(rtptx_par,0,sizeof(RTPTX_PAR));
 for (u=0; u < arlen(codec) ; u++) {
	if (!strcmp(name, codec[u].name)) {
		rtptx_par->config = codec[u].fn;
		return 0;
	}
 }
 return 1;
}
