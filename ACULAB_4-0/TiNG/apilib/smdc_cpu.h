/* smdc_cpu.h - structures for fictional CPU protocol */

#ifndef INCLUDED_SMDC_CPU_H
#define INCLUDED_SMDC_CPU_H

#ifdef TiNGTYPE_WINNT
#ifdef TiNG_USE_DLL
#ifndef ACUAPI
#define ACUAPI __declspec(dllimport)
#endif
#endif
#endif

#define kSMDCProtocolCpu 0xcb03

#if 0
#include "../libutil/header_prolog.h"
typedef struct smdc_cpu_config_parms {
} SMDC_CPU_CONFIG_PARMS;
#include "../libutil/header_epilog.h"
#endif

#endif
