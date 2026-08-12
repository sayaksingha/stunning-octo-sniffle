/* smdc_none.h - structures for no encoding on protocol */

#ifndef INCLUDED_SMDC_NONE_H
#define INCLUDED_SMDC_NONE_H

#ifdef TiNGTYPE_WINNT
#ifdef TiNG_USE_DLL
#ifndef ACUAPI
#define ACUAPI __declspec(dllimport)
#endif
#endif
#endif

#define kSMDCConfigEncodingNone 0

#if 0
#include "../libutil/header_prolog.h"
typedef struct smdc_none_format_parms {
} SMDC_NONE_FORMAT_PARMS;
#include "../libutil/header_epilog.h"
#endif

#endif
