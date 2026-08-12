/* smdc_raw.h - structures for raw protocol */

#ifndef INCLUDED_SMDC_RAW_H
#define INCLUDED_SMDC_RAW_H

#ifdef TiNGTYPE_WINNT
#ifdef TiNG_USE_DLL
#ifndef ACUAPI
#define ACUAPI __declspec(dllimport)
#endif
#endif
#endif

#define kSMDCProtocolRawRx 0xcb01
#define kSMDCProtocolRawTx 0xcb02

#include "../libutil/header_prolog.h"
typedef struct smdc_raw_config_parms {
	unsigned i460_mask;
	unsigned xorval;
} SMDC_RAW_CONFIG_PARMS;
#include "../libutil/header_epilog.h"

#endif
