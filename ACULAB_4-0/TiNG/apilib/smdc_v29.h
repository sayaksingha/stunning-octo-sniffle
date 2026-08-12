/* smdc_v29.h - structures for V29 */

#ifndef INCLUDED_SMDC_V29_H
#define INCLUDED_SMDC_V29_H

#ifdef TiNGTYPE_WINNT
#ifdef TiNG_USE_DLL
#ifndef ACUAPI
#define ACUAPI __declspec(dllimport)
#endif
#endif
#endif

#define kSMDCProtocolV29rx 10
#define kSMDCProtocolV29tx 11

#include "../libutil/header_prolog.h"
typedef struct smdc_v29_config_parms {
	tSM_INT speed;
} SMDC_V29_CONFIG_PARMS;
#include "../libutil/header_epilog.h"

#endif
