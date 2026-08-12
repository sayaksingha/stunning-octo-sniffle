/* smdc_ifp.h - structures for IFP */

#ifndef INCLUDED_SMDC_IFP_H
#define INCLUDED_SMDC_IFP_H

#ifdef TiNGTYPE_WINNT
#ifdef TiNG_USE_DLL
#ifndef ACUAPI
#define ACUAPI __declspec(dllimport)
#endif
#endif
#endif

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
