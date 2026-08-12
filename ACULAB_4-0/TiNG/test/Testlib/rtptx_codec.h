#ifndef INCLUDED_RTPTX_H
#define INCLUDED_RTPTX_H

#include "../Testlib/error.h"
#include "smrtp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rtptx_par {
	unsigned comfort_noise_payload_mapping;
	unsigned bitrate;
	unsigned vadmode;
	unsigned variant;
	unsigned pticks;
	unsigned frame_len;
	unsigned sample_rate; // used to convey -oR setting to codec for opus/SiLK
	unsigned high_pass_filter;
	unsigned payloadmapping;
	err_t (*config)(tSMVMPtxId vmptx, struct rtptx_par *rtptx_par);
} RTPTX_PAR;

err_t rtptx_config_vmptx(tSMVMPtxId vmptx, RTPTX_PAR* rtptx_par);
void rtptx_dump_codec_parms(FILE* f);
void rtptx_dump_codecs(FILE* f);
int rtptx_init_par(RTPTX_PAR* rtptx_par, const char* name);

#ifdef __cplusplus
}
#endif

#endif
