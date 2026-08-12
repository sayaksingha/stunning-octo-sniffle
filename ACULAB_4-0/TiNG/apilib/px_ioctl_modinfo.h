/* px_ioctl_modinfo.h - ioctl code for read/write modinfo */

#ifndef INCLUDED_PX_IOCTL_MODINFO_H
#define INCLUDED_PX_IOCTL_MODINFO_H

#include "px_conn.h"
#include "../TiNG.h"

LOCALDEC int px_rdmodinfo(PX_MOD *pxmod, struct ting_modinfo *p);

#endif
