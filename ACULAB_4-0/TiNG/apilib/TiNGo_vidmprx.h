/* TiNGo_vidmprx.h - TiNG VidMPrx objects */

#ifndef INCLUDED_TINGO_VIDMPRX_H
#define INCLUDED_TINGO_VIDMPRX_H

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

struct tSMVidMPrxId_struct {
 PARAMUTX pm;
 struct module *mod;
 tSMGroove groove;
 DATAFEED dataf; //equivalent to tSMDatafeedId
 enum { VIDMPRX_STATE_IDLE, VIDMPRX_STATE_RUNNING, VIDMPRX_STATE_STOPPING, } state;
 unsigned udpports[2];
 int is_ipv6;
 union {
	struct in_addr ipv4;
	struct in6_addr ipv6;
 } address;
 int hasdata;
 uint_fast32_t authsubtask;
 uint_fast32_t encsubtask;
 enum { VIDMPRX_STUN_ROLE_NONE, VIDMPRX_STUN_ROLE_LITE, VIDMPRX_STUN_ROLE_CONTROLLED, VIDMPRX_STUN_ROLE_CONTROLLING, } stun_role;
};

LOCALDEC void vidmprx_dtor(struct tSMVidMPrxId_struct *vrp);
LOCALDEC struct tSMVidMPrxId_struct *init_vidmprx(struct module *mod);

#endif
