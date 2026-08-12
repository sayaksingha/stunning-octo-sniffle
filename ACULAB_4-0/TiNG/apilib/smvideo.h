/* smvideo.h */

#ifndef INCLUDED_SMVIDEO_H
#define INCLUDED_SMVIDEO_H

#include "smdrvr.h"

typedef struct tSMAVFrecId_struct *tSMAVFrecId;
#define kSMNullAVFrecId (tSMAVFrecId) 0

typedef struct tSMAVFplayId_struct *tSMAVFplayId;
#define kSMNullAVFplayId (tSMAVFplayId) 0

#ifdef TiNGTYPE_WINNT
#include "../libutil/WINNT/wind.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "../libutil/header_prolog.h"
#include "../pubdoc/gen/prosvid.h"
#include "../libutil/header_epilog.h"

#ifdef __cplusplus
}
#endif

#endif

