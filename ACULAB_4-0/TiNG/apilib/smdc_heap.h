/* smdc_heap.h - structures for fictional Heap protocol */

#ifndef INCLUDED_SMDC_HEAP_H
#define INCLUDED_SMDC_HEAP_H

#ifdef TiNGTYPE_WINNT
#ifdef TiNG_USE_DLL
#ifndef ACUAPI
#define ACUAPI __declspec(dllimport)
#endif
#endif
#endif

#define kSMDCProtocolHeap 0xcb04

#if 0
#include "../libutil/header_prolog.h"
typedef struct smdc_heap_config_parms {
} SMDC_HEAP_CONFIG_PARMS;
#include "../libutil/header_epilog.h"
#endif

#endif
