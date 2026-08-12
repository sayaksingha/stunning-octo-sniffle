/* px_ioctl_outts.c - ioctl code for read/write outts */

#include "px_ioctl_outts.h"
#include "smgroove.h"
#include "assp_conn.h"
#include "px_util.h"
#include "../hsif/kernel.h"
#include "../hsif/typecode.h"
#include "TiNGcommon.h"
#include "trace.h"

LOCALDEF int px_rdoutts(PX_CHAN *pxchan, struct ting_tslist *p)
{
 (void) pxchan;
 (void) p;
 if (TiNGtrace) olog("rdoutts: not implemented\n");
 return 1;
}

LOCALDEF int px_wroutts(PX_CHAN *pxchan, struct ting_tslist *p)
{
 (void) pxchan;
 (void) p;
 if (TiNGtrace) olog("wroutts: not implemented\n");
 return 1;
}
