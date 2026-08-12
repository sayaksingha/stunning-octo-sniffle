/* smdc_rdbg.h - structures for remote debugging */

#ifndef INCLUDED_SMDC_RDBG_H
#define INCLUDED_SMDC_RDBG_H

#ifdef TiNGTYPE_WINNT
#ifdef TiNG_USE_DLL
#ifndef ACUAPI
#define ACUAPI __declspec(dllimport)
#endif
#endif
#endif

#define kSMDCProtocolRdbg 0xcb07

#if 0
#include "../libutil/header_prolog.h"
typedef struct smdc_rdbg_config_parms {
} SMDC_RDBG_CONFIG_PARMS;
#include "../libutil/header_epilog.h"
#endif

#endif
