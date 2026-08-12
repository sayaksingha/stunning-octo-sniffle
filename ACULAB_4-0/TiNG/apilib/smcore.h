/* smcore.h - Prosody generic API declarations */

#ifndef INCLUDED_SMCORE_H
#define INCLUDED_SMCORE_H

#include "smtypes.h"

#ifdef TiNGTYPE_LINUX
#include "netinet/in.h"
#endif

#ifdef TiNGTYPE_QNX
#include "netinet/in.h"
#endif


#ifdef TiNGTYPE_WINNT
#include "../libutil/WINNT/wind.h"
#include <ws2tcpip.h>

// these are defined in the Windows headers
#define DEFINED_SOCKADDR_IN
#define DEFINED_SOCKADDR_IN6
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define ERR_SM_NO_LICENSE ERR_SM_NO_LICENCE /* alternative spelling */

/*
 * Following event types are defined.
 */
enum {
	kSMEventTypeWriteData=1,
	kSMEventTypeReadData=2,
	kSMEventTypeRecog=3,
};

/*
 * Following may be set in issue_events.
 */
enum {
	kSMChannelNoEvent=0,
	kSMChannelSpecificEvent=1,
	kSMAnyChannelEvent=2,
	kSMChannelSpecificEventNonIdle=3,
};

#define PROSODY_TiNG

#ifndef DEFINED_SOCKADDR_IN
typedef struct sockaddr_in SOCKADDR_IN;
#define DEFINED_SOCKADDR_IN
#endif

#ifndef DEFINED_SOCKADDR_IN6
typedef struct sockaddr_in6 SOCKADDR_IN6;
#define DEFINED_SOCKADDR_IN6
#endif

#include "../libutil/header_prolog.h"
#include "../pubdoc/gen/prosgen.h"
#include "../libutil/header_epilog.h"

#ifdef __cplusplus
}
#endif

#endif
