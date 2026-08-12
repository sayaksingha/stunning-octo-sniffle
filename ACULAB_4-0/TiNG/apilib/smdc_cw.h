/* smdc_cw.h - structures for CW */

#ifndef INCLUDED_SMDC_CW_H
#define INCLUDED_SMDC_CW_H

#ifdef TiNGTYPE_WINNT
#ifdef TiNG_USE_DLL
#ifndef ACUAPI
#define ACUAPI __declspec(dllimport)
#endif
#endif
#endif

#define kSMDCProtocolCWrx 16
#define kSMDCProtocolCWtx 17

#include "../libutil/header_prolog.h"
typedef struct smdc_cw_config_parms {
	unsigned long speed;
	float space_frequency;
	unsigned long rx_carrier_off_mS;
	unsigned long set_power;
	float user_power;
} SMDC_CW_CONFIG_PARMS;
#include "../libutil/header_epilog.h"

#endif
