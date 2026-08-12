/* smdc_ttyrx.h - structures for TTY rx (ITU V.18 Annex A) protocol */

#ifndef INCLUDED_SMDC_TTYRX_H
#define INCLUDED_SMDC_TTYRX_H

#ifdef TiNGTYPE_WINNT
#ifdef TiNG_USE_DLL
#ifndef ACUAPI
#define ACUAPI __declspec(dllimport)
#endif
#endif
#endif

#define kSMDCProtocolTTYrx 0xcb08

#if 0
#include "../libutil/header_prolog.h"
typedef struct smdc_ttyrx_config_parms {
} SMDC_TTYRX_CONFIG_PARMS;
#include "../libutil/header_epilog.h"
#endif

#endif
