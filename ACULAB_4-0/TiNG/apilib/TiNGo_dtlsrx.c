/* TiNGo_dtlsrx.c - TiNG dtlsrx objects */

#include "TiNGo_dtlsrx.h"
#include "TiNGcommon.h"
#include <stdlib.h>
#include <string.h>

LOCALDEF void dtlsrx_dtor(struct tSMDTLSrxId_struct *vrp)
{
 vrp->groove.dtor(&vrp->groove);
 mutx_dtor(&vrp->pm.mx);
 memset(vrp, 0xa5, sizeof(*vrp));		// for safety
 free(vrp);
}

LOCALDEF struct tSMDTLSrxId_struct *init_dtlsrx(struct module* mod)
{
 struct tSMDTLSrxId_struct *vrp = alloc(0, sizeof(*vrp));
 if (!vrp) return 0;
 memset(vrp, 0, sizeof(*vrp));
 vrp->mod = mod;
 if(!SMGroove_module(&vrp->groove, mod)) {
	mutx_init(&vrp->pm.mx);
	make_object(vrp);
	return vrp;
 }
 free(vrp);
 return 0;
}
