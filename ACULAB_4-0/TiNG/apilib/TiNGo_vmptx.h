/* TiNGo_vmptx.h - TiNG VMPtx objects */

#ifndef INCLUDED_TINGO_VMPTX_H
#define INCLUDED_TINGO_VMPTX_H

#include "paranoia.h"
#include "TiNGo_mod.h"
#include "TiNGo_vmptone.h"
#include "smgroove.h"

struct tSMVMPtxId_struct {
 PARAMUTX pm;
 struct module *mod;
 tSMGroove groove;
 enum {VMPTX_STATE_IDLE, VMPTX_STATE_RUNNING, VMPTX_STATE_STOPPING} state;
 struct tSMVMPTxToneSetId_struct *toneset;
 uint32_t maincodecsubtask;
 uint32_t authsubtask;
 uint32_t encsubtask;
 int using_g711_vad;
 int comfort_noise_pt;
 DATAFEED dfin; // for ref count
};

LOCALDEC void vmptx_dtor(struct tSMVMPtxId_struct *vtp);
LOCALDEC struct tSMVMPtxId_struct *init_vmptx(struct module *mod);

#endif
