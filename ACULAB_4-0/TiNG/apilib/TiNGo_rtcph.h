/* TiNGo_rtcphand.h - TiNG RTCPHand objects */

#ifndef INCLUDED_TINGO_RTCPHAND_H
#define INCLUDED_TINGO_RTCPHAND_H

#include "TiNGo_datafeed.h"
#include "TiNGo_mod.h"
#include "paranoia.h"
#include "smgroove.h"

#ifdef TiNGTYPE_LINUX
#include "netinet/in.h"
#endif

#ifdef TiNGTYPE_QNX
#include "netinet/in.h"
#endif


#ifdef TiNGTYPE_WINNT
#include "../libutil/WINNT/wind.h"
#endif

struct tSMRTCPHandId_struct {
	PARAMUTX pm;
	struct module *mod;
	tSMGroove groove;
};

LOCALDEC void rtcphand_dtor(struct tSMRTCPHandId_struct *vrp);
LOCALDEC struct tSMRTCPHandId_struct *init_rtcphand(struct module *mod);

#endif
