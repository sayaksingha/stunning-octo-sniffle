#ifndef INCLUDED_RTPRX_H
#define INCLUDED_RTPRX_H

#include "../Testlib/error.h"
#include "smrtp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rtprx_par {
	unsigned init_ms;
	unsigned max_ms;
	unsigned ad_target_delay;
	float ad_freq_upper_tolerance;
	float ad_freq_lower_tolerance;
	unsigned payloadmapping;
	unsigned plc;
	unsigned regen_tones;
	unsigned detect_tones;
	unsigned enforce_tone_spacing;
	unsigned tone_payload_mapping;
	unsigned handle_cn;
	unsigned comfort_noise_payload_mapping;
	unsigned bitrate;
	unsigned variant;
	unsigned frame_len;
	unsigned report_jb_resync;
	err_t (*config)(tSMVMPrxId vmprx, struct rtprx_par *rtprx_par);
	err_t (*codec_specific_status)(tSMVMPrxId vmprx);
} RTPRX_PAR;

err_t rtprx_config_vmprx(tSMVMPrxId vmprx, RTPRX_PAR* rtprx_par);
void rtprx_dump_codec_parms(FILE* f);
void rtprx_dump_codecs(FILE* f);
int rtprx_init_par(RTPRX_PAR* rtprx_par, const char* name);

#ifdef __cplusplus
}
#endif

#endif
