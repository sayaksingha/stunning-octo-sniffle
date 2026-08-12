/* smdc_fsk.h - structures for FSK */

#ifndef INCLUDED_SMDC_FSK_H
#define INCLUDED_SMDC_FSK_H

#ifdef TiNGTYPE_WINNT
#ifdef TiNG_USE_DLL
#ifndef ACUAPI
#define ACUAPI __declspec(dllimport)
#endif
#endif
#endif

#define kSMDCProtocolFSKrx 8
#define kSMDCProtocolFSKtx 9

#include "../libutil/header_prolog.h"
typedef struct smdc_fsk_config_parms {
	unsigned long speed;
	float mark_frequency;
	float space_frequency;
	unsigned long rx_carrier_on_mS;
	unsigned long rx_carrier_off_mS;
	unsigned long set_power;
	float user_power;
	unsigned long get_metric;
	unsigned long thresh_metric;
	unsigned long reverse_chan_filter;
} SMDC_FSK_CONFIG_PARMS;
#include "../libutil/header_epilog.h"

#endif
