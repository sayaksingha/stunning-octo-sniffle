/* smrtp.h */

#ifndef INCLUDED_SMRTP_H
#define INCLUDED_SMRTP_H

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
#include <ws2tcpip.h>
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

extern ACUAPI tSMVMPTxToneSet *kSMVMPTxDefaultToneSet;

#include "../libutil/header_prolog.h"
#include "../pubdoc/gen/prosrtpapi.h"

typedef struct sm_vmptx_sync_start_parms {
	tSMVMPtxId vmptx;
} SM_VMPTX_SYNC_START_PARMS;

ACUAPI int ACUTiNG_WINAPI sm_vmptx_sync_start(struct sm_vmptx_sync_start_parms* parms);

#include "../libutil/header_epilog.h"

#ifdef __cplusplus
}
#endif

#endif

