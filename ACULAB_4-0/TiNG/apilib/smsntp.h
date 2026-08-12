/* smfmp.h */

#ifndef INCLUDED_SMSNTP_H
#define INCLUDED_SMSNTP_H

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

#ifndef DEFINED_SOCKADDR_IN6
typedef struct sockaddr_in6 SOCKADDR_IN6;
#define DEFINED_SOCKADDR_IN6
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "../libutil/header_prolog.h"
#include "../pubdoc/gen/prossntp.h"
#include "../libutil/header_epilog.h"

#ifdef __cplusplus
}
#endif

#endif

