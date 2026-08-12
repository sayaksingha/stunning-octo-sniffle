/* px_ioctl_intslist.c - ioctl code for read/write intslist */

#include "px_ioctl_intslist.h"
#include "assp_conn.h"
#include "px_util.h"
#include "../hsif/kernel.h"
#include "../hsif/typecode.h"
#include "TiNGcommon.h"
#include "trace.h"
#include "TiNGo_datafeed.h"
#include <string.h>

LOCALDEF int px_rdintslist(PX_CHAN *pxchan, struct ting_tslist *p)
{
 (void) pxchan;
 (void) p;
 if (TiNGtrace) olog("rdintslist: not implemented\n");
 return 1;
}

LOCALDEF int px_wrintslist(PX_CHAN *pxchan, struct ting_tslist *p)
{
 (void) pxchan;
 (void) p;
 if (TiNGtrace) olog("wrintslist: not implemented\n");
 return 1;
}
