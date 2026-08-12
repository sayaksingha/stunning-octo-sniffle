/* px_ioctl_mem.h - ioctl code for read/write mem */

#ifndef INCLUDED_PX_IOCTL_MEM_H
#define INCLUDED_PX_IOCTL_MEM_H

#include "px_conn.h"
#include "../TiNG.h"

LOCALDEC int px_rdmem(PX_MOD *pxmod, struct ting_sharcmem *p);
LOCALDEC int px_wrmem(PX_MOD *pxmod, struct ting_sharcmem *p);

#endif
