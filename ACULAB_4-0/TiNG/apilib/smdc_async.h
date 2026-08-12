/* smdc_async.h - structures for ASYNC */

#ifndef INCLUDED_SMDC_ASYNC_H
#define INCLUDED_SMDC_ASYNC_H

#ifdef TiNGTYPE_WINNT
#ifdef TiNG_USE_DLL
#ifndef ACUAPI
#define ACUAPI __declspec(dllimport)
#endif
#endif
#endif

#include "smtypes.h"

#define kSMDCConfigEncodingAsync 2

#include "../libutil/header_prolog.h"
typedef struct smdc_async_format_parms {
	tSM_UT32 stopbits;
	tSM_UT32 databits;
} SMDC_ASYNC_FORMAT_PARMS;
#include "../libutil/header_epilog.h"

#endif
