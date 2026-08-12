/* smdc_rlp.h - structures for V.110 RLP */

#ifndef INCLUDED_SMDC_RLP_H
#define INCLUDED_SMDC_RLP_H

#ifdef TiNGTYPE_WINNT
#ifdef TiNG_USE_DLL
#ifndef ACUAPI
#define ACUAPI __declspec(dllimport)
#endif
#endif
#endif

#define kSMDCProtocolV110RLPrx 4
#define kSMDCProtocolV110RLPtx 5

#include "../libutil/header_prolog.h"
typedef struct smdc_v110rlp_config_parms {
	unsigned long speed;
} SMDC_V110RLP_CONFIG_PARMS;
#include "../libutil/header_epilog.h"

#endif
