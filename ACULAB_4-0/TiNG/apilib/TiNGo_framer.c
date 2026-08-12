/* TiNGo_framer.c - TiNG framer objects */

#include "TiNGo_framer.h"
#include "TiNGcommon.h"
#include <stdlib.h>
#include <string.h>

LOCALDEF void framer_dtor(struct tSMFramerId_struct *vtp)
{
 vtp->groove.dtor(&vtp->groove);
 vtp->mod->card->vtbl->datafeed_dtor(vtp->dfin[0]);
 vtp->mod->card->vtbl->datafeed_dtor(vtp->dfin[1]);
 vtp->mod->card->vtbl->datafeed_dtor(vtp->dataf[0]);
 vtp->mod->card->vtbl->datafeed_dtor(vtp->dataf[1]);
 mutx_dtor(&vtp->pm.mx);
 memset(vtp, 0xa5, sizeof(*vtp));		// for safety
 free(vtp);
}

LOCALDEF struct tSMFramerId_struct *init_framer(struct module *mod)
{
 struct tSMFramerId_struct *vtp = alloc(0,sizeof(*vtp));
 if (!vtp) return 0;
 memset(vtp, 0, sizeof(*vtp));
 vtp->mod = mod;
 if (!SMGroove_module(&vtp->groove, mod)) {
 	vtp->dfin[0] = NULL_DATAFEED;
 	vtp->dfin[1] = NULL_DATAFEED;
 	vtp->dataf[0] = mod->card->vtbl->datafeed();
 	if(nonnull_datafeed(vtp->dataf[0])) {
 		vtp->dataf[1] = mod->card->vtbl->datafeed();
 		if(nonnull_datafeed(vtp->dataf[1])) {
 			mutx_init(&vtp->pm.mx);
 			make_object(vtp);
 			return vtp;
		}
	 	mod->card->vtbl->datafeed_dtor(vtp->dataf[0]);
 	}
	vtp->groove.dtor(&vtp->groove);
 }
 free(vtp);
 return 0;
}

