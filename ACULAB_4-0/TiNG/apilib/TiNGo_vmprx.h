/* TiNGo_vmprx.h - TiNG VMPrx objects */

#ifndef INCLUDED_TINGO_VMPRX_H
#define INCLUDED_TINGO_VMPRX_H

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
#include <ws2tcpip.h>
#endif

struct toneinfo{
 char tone;
 int vol;
};

struct tSMVMPrxId_struct {
 PARAMUTX pm;
 struct module *mod;
 tSMGroove groove;
 DATAFEED dataf; //equivalent to tSMDatafeedId
 enum { VMPRX_STATE_IDLE, VMPRX_STATE_RUNNING, VMPRX_STATE_STOPPING, } state;
 unsigned udpports[2];
 int is_ipv6;
 union {
	struct in_addr ipv4;
	struct in6_addr ipv6;
 } address;
 unsigned tonehead;
 unsigned tonetail;
 struct toneinfo tones[10];
 int hasdata;
 uint32_t authsubtask;
 uint32_t encsubtask;
 unsigned plrsubtasks[10];
 unsigned plcsubtasks[10];
 enum { VMPRX_STUN_ROLE_NONE, VMPRX_STUN_ROLE_LITE, VMPRX_STUN_ROLE_CONTROLLED, VMPRX_STUN_ROLE_CONTROLLING, } stun_role;
 unsigned dtls_usage;
};

LOCALDEC void vmprx_dtor(struct tSMVMPrxId_struct *vrp);
LOCALDEC struct tSMVMPrxId_struct *init_vmprx(struct module *mod);

#endif
