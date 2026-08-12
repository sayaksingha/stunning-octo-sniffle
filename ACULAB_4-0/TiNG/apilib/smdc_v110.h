/* smdc_v110.h - structures for V.110 */

#ifndef INCLUDED_SMDC_V110_H
#define INCLUDED_SMDC_V110_H

#ifdef TiNGTYPE_WINNT
#ifdef TiNG_USE_DLL
#ifndef ACUAPI
#define ACUAPI __declspec(dllimport)
#endif
#endif
#endif

#define kSMDCProtocolV110 	3

#define kSMDCFormatAsync 0
#define kSMDCFormatSync 1

#include "../libutil/header_prolog.h"
typedef struct smdc_v110_config_parms {
	unsigned long speed;
	int format;
} SMDC_V110_CONFIG_PARMS;
#include "../libutil/header_epilog.h"

#define kSMDCV110Aux_S1 0x80000000
#define kSMDCV110Aux_X2 0x40000000
#define kSMDCV110Aux_S3 0x20000000
#define kSMDCV110Aux_S4 0x10000000
#define kSMDCV110Aux_E1 0x08000000
#define kSMDCV110Aux_E2 0x04000000
#define kSMDCV110Aux_E3 0x02000000
#define kSMDCV110Aux_E4 0x01000000
#define kSMDCV110Aux_E5 0x00800000
#define kSMDCV110Aux_E6 0x00400000
#define kSMDCV110Aux_E7 0x00200000
#define kSMDCV110Aux_S6 0x00100000
#define kSMDCV110Aux_X7 0x00080000
#define kSMDCV110Aux_S8 0x00040000
#define kSMDCV110Aux_S9 0x00020000

#endif
