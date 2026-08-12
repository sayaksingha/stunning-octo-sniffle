/* smdc_v27.h - structures for V27 */

#ifndef INCLUDED_SMDC_V27_H
#define INCLUDED_SMDC_V27_H

#ifdef TiNGTYPE_WINNT
#ifdef TiNG_USE_DLL
#ifndef ACUAPI
#define ACUAPI __declspec(dllimport)
#endif
#endif
#endif

#define kSMDCProtocolV27rx 12
#define kSMDCProtocolV27tx 13

#include "../libutil/header_prolog.h"
typedef struct smdc_v27_config_parms {
	tSM_INT speed;
} SMDC_V27_CONFIG_PARMS;
#include "../libutil/header_epilog.h"

#endif
