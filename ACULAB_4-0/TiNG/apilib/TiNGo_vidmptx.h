/* TiNGo_vidmptx.h - TiNG VidMPtx objects */

#ifndef INCLUDED_TINGO_VIDMPTX_H
#define INCLUDED_TINGO_VIDMPTX_H

#include "paranoia.h"
#include "TiNGo_mod.h"
#include "smgroove.h"

struct tSMVidMPtxId_struct {
 PARAMUTX pm;
 struct module *mod;
 tSMGroove groove;
 enum {VIDMPTX_STATE_IDLE, VIDMPTX_STATE_RUNNING, VIDMPTX_STATE_STOPPING} state;
 uint32_t authsubtask;
 uint32_t encsubtask;
 uint32_t maincodecsubtask;
 DATAFEED dfin; // for ref count
};

LOCALDEC void vidmptx_dtor(struct tSMVidMPtxId_struct *vtp);
LOCALDEC struct tSMVidMPtxId_struct *init_vidmptx(struct module *mod);

#endif
