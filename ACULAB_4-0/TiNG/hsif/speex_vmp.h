/* speex_vmp.h - messages passed between host and sharc for Speex codec */

#ifndef INCLUDED_SPEEX_VMP_H
#define INCLUDED_SPEEX_VMP_H

#define MSG_SPEEX_VMP_NARROWBAND        0
#define MSG_SPEEX_VMP_WIDEWBAND         1
#define MSG_SPEEX_VMP_ULTRAWIDEWBAND    2

#define MSG_CONFIG_SPEEXENC             0xcfb
#define MSG_CONFIG_SPEEXENC_CTRL        0xcfc

#define MSG_CONFIG_SPEEXENC_COMPLEXITY  0
#define MSG_CONFIG_SPEEXENC_QUALITY     1
#define MSG_CONFIG_SPEEXENC_BITRATE		2

#define MSG_CONFIG_SPEEXDEC             0xcfd
#define MSG_CONFIG_SPEEXDEC_CTRL        0xcfe

#define MSG_CONFIG_SPEEXDEC_ENHANCED    0

#endif
