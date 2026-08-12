/* px_ioctl_stats.c - statistics */

#include "px_ioctl_stats.h"
#include "TiNGcommon.h"
#include "trace.h"
#include <string.h>

/*
 * http://docbox/~ch/TOPDK/ioctl-STATS.html#ioctl-STATS
 */
LOCALDEF int px_rdstats(PX_MOD *pxmod, struct ting_statistics *p)
{
 (void) pxmod;
 (void) p;
 if (TiNGtrace) olog("rdstats: not implemented\n");
 return 1;
}
