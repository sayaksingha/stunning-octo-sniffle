/* smdc.h - Prosody Data communications */

#ifndef INCLUDED_ACULAB_SMDC_H
#define INCLUDED_ACULAB_SMDC_H

#ifdef TiNGTYPE_WINNT
#ifdef TiNG_USE_DLL
#ifndef ACUAPI
#define ACUAPI __declspec(dllimport)
#endif
#endif
#endif

#define kSMDCProtocolNone 0

#include "smdrvr.h"

#ifndef kSMDCT38ProtocolAll
#define kSMDCT38ProtocolAll -1
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "../libutil/header_prolog.h"
#include "../pubdoc/gen/prosdc.h"
#include "../libutil/header_epilog.h"

#ifdef __cplusplus
}
#endif

#endif
