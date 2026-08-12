/* TiNGo_fmprx.c - TiNG fmprx objects */

#include "TiNGo_fmprx.h"
#include "TiNGcommon.h"
#include <stdlib.h>
#include <string.h>

LOCALDEF void fmprx_dtor(struct tSMFMPrxId_struct *vrp)
{
 vrp->groove.dtor(&vrp->groove);
 vrp->mod->card->vtbl->datafeed_dtor(vrp->dataf);
 mutx_dtor(&vrp->pm.mx);
 memset(vrp, 0xa5, sizeof(*vrp));		// for safety
 free(vrp);
}

LOCALDEF struct tSMFMPrxId_struct *init_fmprx(struct module* mod)
{
 struct tSMFMPrxId_struct *vrp = alloc(0, sizeof(*vrp));
 if (!vrp) return 0;
 memset(vrp, 0, sizeof(*vrp));
 vrp->mod = mod;
 if(!SMGroove_module(&vrp->groove, mod)) {
	vrp->dataf = mod->card->vtbl->datafeed();
	if (nonnull_datafeed(vrp->dataf)) {
		mutx_init(&vrp->pm.mx);
		make_object(vrp);
		return vrp;
	}
	vrp->groove.dtor(&vrp->groove);
 }
 free(vrp);
 return 0;
}
