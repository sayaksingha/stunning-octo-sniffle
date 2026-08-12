/* px_ioctl_confinfo.c - ioctl code for read/write confinfo */

#include "px_ioctl_confinfo.h"
#include "TiNGcommon.h"
#include "trace.h"

LOCALDEF int px_rdconfinfo(PX_CHAN *pxchan, struct ting_confinfo *p)
{
 (void) pxchan;
 (void) p;
 if (TiNGtrace) olog("rdconfinfo: not implemented\n");
 return 1;
}
