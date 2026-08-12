/* smi460.h */

#ifndef INCLUDED_SMFRAMER_H
#define INCLUDED_SMFRAMER_H

#include "smdrvr.h"
#include "px_prostype.h"

#ifdef TiNGTYPE_LINUX
#include "netinet/in.h"
#endif

#ifdef TiNGTYPE_QNX
#include "netinet/in.h"
#endif


#ifdef TiNGTYPE_WINNT
#include "../libutil/WINNT/wind.h"
#endif

#ifndef DEFINED_SOCKADDR_IN
typedef struct sockaddr_in SOCKADDR_IN;
#define DEFINED_SOCKADDR_IN
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "../libutil/header_prolog.h"
#include "../pubdoc/gen/prosframerapi.h"
#include "../libutil/header_epilog.h"

#ifdef __cplusplus
}
#endif

#endif

