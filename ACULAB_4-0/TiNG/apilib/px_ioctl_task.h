/* px_ioctl_task.h - ioctl code for read/write task */

#ifndef INCLUDED_PX_IOCTL_TASK_H
#define INCLUDED_PX_IOCTL_TASK_H

#include "px_conn.h"
#include "../TiNG.h"

LOCALDEC int px_rdtask(PX_CHAN *pxchan, struct ting_task *p);
LOCALDEC int px_wrtask(PX_CHAN *pxchan, struct ting_task *p);

#endif
