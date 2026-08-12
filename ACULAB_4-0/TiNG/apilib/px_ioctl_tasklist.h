/* px_ioctl_tasklist.h - ioctl code for read/write tasklist */

#ifndef INCLUDED_PX_IOCTL_TASKLIST_H
#define INCLUDED_PX_IOCTL_TASKLIST_H

#include "px_conn.h"
#include "../TiNG.h"

LOCALDEC int px_rdtasklist(PX_MOD *pxmod, struct ting_taskitem *p);

#endif
