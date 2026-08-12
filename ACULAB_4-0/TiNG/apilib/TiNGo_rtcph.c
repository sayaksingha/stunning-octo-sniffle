/* TiNGo_rtcphand.c - TiNG rtcphand objects */

#include "TiNGo_rtcph.h"
#include "TiNGcommon.h"
#include <stdlib.h>
#include <string.h>

LOCALDEF void rtcphand_dtor(struct tSMRTCPHandId_struct *vrp)
{
 vrp->groove.dtor(&vrp->groove);
 mutx_dtor(&vrp->pm.mx);
 memset(vrp, 0xa5, sizeof(*vrp));		// for safety
 free(vrp);
}

LOCALDEF struct tSMRTCPHandId_struct *init_rtcphand(struct module *mod)
{
 struct tSMRTCPHandId_struct *vrp = alloc(0, sizeof(*vrp));
 if (!vrp) return 0;
 memset(vrp, 0, sizeof(*vrp));
 vrp->mod = mod;
 if (!SMGroove_module(&vrp->groove, mod)) {
	mutx_init(&vrp->pm.mx);
	return vrp;
 }
 free(vrp);
 return 0;
}
