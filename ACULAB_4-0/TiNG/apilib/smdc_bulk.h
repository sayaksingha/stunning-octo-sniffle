/* smdc_bulkrx.h - structures for bulk rx protocol */

#ifndef INCLUDED_SMDC_BULKRX_H
#define INCLUDED_SMDC_BULKRX_H

#ifdef TiNGTYPE_WINNT
#ifdef TiNG_USE_DLL
#ifndef ACUAPI
#define ACUAPI __declspec(dllimport)
#endif
#endif
#endif

#define kSMDCProtocolBulkRx 0xcb05

#if 0
#include "../libutil/header_prolog.h"
typedef struct smdc_bulkrx_config_parms {
} SMDC_BULKRX_CONFIG_PARMS;
#include "../libutil/header_epilog.h"
#endif

#endif
