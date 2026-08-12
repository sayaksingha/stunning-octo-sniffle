/* px_ioctl_mstate.h - ioctl code for read/write mstate */

#ifndef INCLUDED_PX_IOCTL_MSTATE_H
#define INCLUDED_PX_IOCTL_MSTATE_H

#include "px_conn.h"
#include "../TiNG.h"

LOCALDEC int px_rdmstate(PX_MOD *pxmod, struct ting_mstate *p);
LOCALDEC int px_wrmstate(PX_MOD *pxmod, struct ting_mstate *p);

#endif
