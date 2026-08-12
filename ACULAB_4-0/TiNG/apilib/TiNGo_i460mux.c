/* TiNGo_i460mux.c - TiNG i460mux objects */

#include "TiNGo_i460mux.h"
#include "TiNGcommon.h"
#include <stdlib.h>
#include <string.h>

LOCALDEF void i460mux_dtor(struct tSMI460muxId_struct *vtp)
{
 vtp->groove.dtor(&vtp->groove);
 vtp->mod->card->vtbl->datafeed_dtor(vtp->dfin[0]);
 vtp->mod->card->vtbl->datafeed_dtor(vtp->dfin[1]);
 vtp->mod->card->vtbl->datafeed_dtor(vtp->dfin[2]);
 vtp->mod->card->vtbl->datafeed_dtor(vtp->dfin[3]);
 vtp->mod->card->vtbl->datafeed_dtor(vtp->dfin[4]);
 vtp->mod->card->vtbl->datafeed_dtor(vtp->dfin[5]);
 vtp->mod->card->vtbl->datafeed_dtor(vtp->dfin[6]);
 vtp->mod->card->vtbl->datafeed_dtor(vtp->dfin[7]);
 vtp->mod->card->vtbl->datafeed_dtor(vtp->dataf);
 mutx_dtor(&vtp->pm.mx);
 memset(vtp, 0xa5, sizeof(*vtp));		// for safety
 free(vtp);
}

LOCALDEF struct tSMI460muxId_struct *init_i460mux(struct module *mod)
{
 struct tSMI460muxId_struct *vtp = alloc(0,sizeof(*vtp));
 if (!vtp) return 0;
 memset(vtp, 0, sizeof(*vtp));
 vtp->mod = mod;
 if (!SMGroove_module(&vtp->groove, mod)) {
 	vtp->dfin[0] = NULL_DATAFEED;
 	vtp->dfin[1] = NULL_DATAFEED;
 	vtp->dfin[2] = NULL_DATAFEED;
 	vtp->dfin[3] = NULL_DATAFEED;
 	vtp->dfin[4] = NULL_DATAFEED;
 	vtp->dfin[5] = NULL_DATAFEED;
 	vtp->dfin[6] = NULL_DATAFEED;
 	vtp->dfin[7] = NULL_DATAFEED;
 	vtp->dataf = mod->card->vtbl->datafeed();
 	if(nonnull_datafeed(vtp->dataf)) {
 		mutx_init(&vtp->pm.mx);
 		make_object(vtp);
 		return vtp;
 	}
	vtp->groove.dtor(&vtp->groove);
 }
 free(vtp);
 return 0;
}

