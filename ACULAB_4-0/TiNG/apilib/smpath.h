/* smpath.h - API for signal path parocessing */

#ifndef INCLUDED_SMPATH_H
#define INCLUDED_SMPATH_H

#include "smdrvr.h"

typedef struct tSMPathId_struct *tSMPathId;
#define kSMNullPathId (tSMPathId) 0

#ifdef TiNGTYPE_LINUX
#endif

#ifdef TiNGTYPE_QNX
#endif


#ifdef TiNGTYPE_WINNT
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "../libutil/header_prolog.h"
#include "../pubdoc/gen/prospath.h"
#include "../libutil/header_epilog.h"

#ifdef __cplusplus
}
#endif

#endif

