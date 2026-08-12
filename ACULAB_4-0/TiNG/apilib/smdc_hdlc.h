/* smdc_hdlc.h - structures for HDLC */

#ifndef INCLUDED_SMDC_HDLC_H
#define INCLUDED_SMDC_HDLC_H

#ifdef TiNGTYPE_WINNT
#ifdef TiNG_USE_DLL
#ifndef ACUAPI
#define ACUAPI __declspec(dllimport)
#endif
#endif
#endif

#include "smtypes.h"

#define kSMDCConfigEncodingHDLC 3

#include "../libutil/header_prolog.h"
typedef struct smdc_hdlc_format_parms {
	tSM_INT crcsize;	// size of CRC (bits)
	tSM_INT crc;		// CRC polynomial: bit N is x^N term
	tSM_INT rx_min_initial_flags; // number of consecutive flags before any frames
	tSM_INT rx_min_ones; // number of ones required before reporting idle (zero to disable)
} SMDC_HDLC_FORMAT_PARMS;
#include "../libutil/header_epilog.h"

#endif
