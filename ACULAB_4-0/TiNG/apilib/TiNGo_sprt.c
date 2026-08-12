#include "TiNGo_sprt.h"
#include "TiNGcommon.h"
#include <stdlib.h>
#include <string.h>

LOCALDEF void sprt_dtor(struct tSMSPRTId_struct *vtp)
{
 vtp->groove.unbind_df(&vtp->groove, vtp->dataf);
 vtp->groove.dtor(&vtp->groove);
 vtp->groove.dtor(&vtp->groove_packetiser);
 vtp->mod->card->vtbl->datafeed_dtor(vtp->dataf);
 vtp->mod->card->vtbl->datafeed_dtor(vtp->dfin);
 mutx_dtor(&vtp->pm.mx);
 memset(vtp, 0xa5, sizeof(*vtp));		// for safety
 free(vtp);
}

LOCALDEF struct tSMSPRTId_struct *init_sprt(struct module *mod)
{
 struct tSMSPRTId_struct *vtp = alloc(0,sizeof(*vtp));
 if (!vtp) return 0;
 memset(vtp, 0, sizeof(*vtp));
 vtp->mod = mod;
 vtp->dataf = mod->card->vtbl->datafeed();
 if (nonnull_datafeed(vtp->dataf)) {
	if (!SMGroove_module(&vtp->groove, mod)) {
		if (!SMGroove_module(&vtp->groove_packetiser, mod)) {
			mutx_init(&vtp->pm.mx);
			vtp->dfin = NULL_DATAFEED;
			make_object(vtp);
			return vtp;
		}
		vtp->groove.dtor(&vtp->groove);
	}
 }
 mod->card->vtbl->datafeed_dtor(vtp->dataf);
 free(vtp);
 return 0;
}

