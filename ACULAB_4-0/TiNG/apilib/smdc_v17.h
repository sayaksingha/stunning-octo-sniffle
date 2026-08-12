/* smdc_v17.h - structures for V17 */

#ifndef INCLUDED_SMDC_V17_H
#define INCLUDED_SMDC_V17_H

#ifdef TiNGTYPE_WINNT
#ifdef TiNG_USE_DLL
#ifndef ACUAPI
#define ACUAPI __declspec(dllimport)
#endif
#endif
#endif

#define kSMDCProtocolV17rx 14
#define kSMDCProtocolV17tx 15

#include "../libutil/header_prolog.h"
typedef struct smdc_v17_config_parms {
	tSM_INT speed;
} SMDC_V17_CONFIG_PARMS;
#include "../libutil/header_epilog.h"

#endif
