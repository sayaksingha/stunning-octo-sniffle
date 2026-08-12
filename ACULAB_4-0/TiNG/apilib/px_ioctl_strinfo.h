/* px_ioctl_strinfo.h - ioctl code for read/write strinfo */

#ifndef INCLUDED_PX_IOCTL_STRINFO_H
#define INCLUDED_PX_IOCTL_STRINFO_H

#include "px_conn.h"
#include "../TiNG.h"

LOCALDEC int px_rdstrinfo(PX_MOD *pxmod, struct ting_strinfo *p);

#endif
