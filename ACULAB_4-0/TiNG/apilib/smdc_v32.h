/* smdc_v32.h - structures for V32 */

#ifndef INCLUDED_SMDC_V32_H
#define INCLUDED_SMDC_V32_H

#ifdef TiNGTYPE_WINNT
#ifdef TiNG_USE_DLL
#ifndef ACUAPI
#define ACUAPI __declspec(dllimport)
#endif
#endif
#endif

#define kSMDCProtocolV32rx 18
#define kSMDCProtocolV32 19

#include "../libutil/header_prolog.h"
typedef struct smdc_v32_config_parms {
	unsigned speed;
	tSM_INT isanswer;
	tSM_INT no_ans_tone;
	tSM_INT retrain_can_change_speed;
} SMDC_V32_CONFIG_PARMS;
#include "../libutil/header_epilog.h"

#endif
