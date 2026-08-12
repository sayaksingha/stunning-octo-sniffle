/* px_ioctl_stats.h - statistics */

#ifndef INCLUDED_PX_IOCTL_STATS_H
#define INCLUDED_PX_IOCTL_STATS_H

#include "px_conn.h"
#include "../TiNG.h"

LOCALDEC int px_rdstats(PX_MOD *pxmod, struct ting_statistics *p);

#endif
