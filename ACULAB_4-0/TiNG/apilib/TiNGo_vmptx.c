/* TiNGo_vmptx.c - TiNG vmptx objects */

#include "TiNGo_vmptx.h"
#include "TiNGcommon.h"
#include <stdlib.h>
#include <string.h>

LOCALDEF void vmptx_dtor(struct tSMVMPtxId_struct *vtp)
{
 vtp->groove.dtor(&vtp->groove);
 if (vtp->toneset) vmptone_dtor(vtp->toneset);
 vtp->mod->card->vtbl->datafeed_dtor(vtp->dfin);
 mutx_dtor(&vtp->pm.mx);
 memset(vtp, 0xa5, sizeof(*vtp));		// for safety
 free(vtp);
}

LOCALDEF struct tSMVMPtxId_struct *init_vmptx(struct module *mod)
{
 struct tSMVMPtxId_struct *vtp = alloc(0, sizeof(*vtp));
 if (!vtp) return 0;
 memset(vtp, 0, sizeof(*vtp));
 vtp->mod = mod;
 if (SMGroove_module(&vtp->groove, mod)) {
 	free(vtp);
 	return 0;
 }
 vtp->dfin = NULL_DATAFEED;
 vtp->comfort_noise_pt = -1;
 mutx_init(&vtp->pm.mx);
 make_object(vtp);
 return vtp;
}

