/* smbesp.h - Prosody declarations specific to Basic & Enhanced speech */
/* these are externally visible declarations */

#ifndef INCLUDED_PROSODY_SMBESP_H
#define INCLUDED_PROSODY_SMBESP_H

#ifdef TiNGTYPE_WINNT
#ifdef TiNG_USE_DLL
#ifndef ACUAPI
#define ACUAPI __declspec(dllimport)
#endif
#endif
#endif

#include "smdrvr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define kSMMaxReplayDataBufferSize 2048
#define kSMMaxRecordDataBufferSize 2048

#define kSMConfAdjInputVolumeMute -128

#include "../libutil/header_prolog.h"
#include "../pubdoc/gen/prospapi.h"
#include "../libutil/header_epilog.h"

	// replay and record compatibility codes
#define kSMDataFormat8KHzPCM ((enum kSMDataFormat) 1)
#define kSMDataFormat8KHzALawPCM ((enum kSMDataFormat) 8)
#define kSMDataFormat8KHzULawPCM ((enum kSMDataFormat) 7)
#define kSMDataFormat8KHzOKIADPCM ((enum kSMDataFormat) 2)
#define kSMDataFormat8KHzACUBLKPCM ((enum kSMDataFormat) 3)
#define kSMDataFormat6KHzPCM ((enum kSMDataFormat) 4)
#define kSMDataFormat6KHzALawPCM ((enum kSMDataFormat) 18)
#define kSMDataFormat6KHzULawPCM ((enum kSMDataFormat) 19)
#define kSMDataFormat6KHzOKIADPCM ((enum kSMDataFormat) 5)
#define kSMDataFormat6KHzACUBLKPCM ((enum kSMDataFormat) 6)
#define kSMDataFormatCELP8KBPS ((enum kSMDataFormat) 9)
#define kSMDataFormat8KHz16bitMono ((enum kSMDataFormat) 14)
#define kSMDataFormat8KHz8bitMono ((enum kSMDataFormat) 15)
#define kSMDataFormat8KHzSigned8bitMono ((enum kSMDataFormat) 20)

	// compatibility with names in old code
#define kSMBESPCatSigParamI_max_valid_speech_cnt kSMBESPCatSigParamI_min_valid_speech_cnt
#define kSMBESPCatSigParamI_max_off_count kSMBESPCatSigParamI_min_off_count
#define kSMInputCondAdjustNonLinear kSMInputCondAdjustNonLinearWithMuting
#ifdef __cplusplus
}
#endif

#endif
