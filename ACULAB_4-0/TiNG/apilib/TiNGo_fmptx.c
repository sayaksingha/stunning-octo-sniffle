/* TiNGo_fmptx.c - TiNG fmptx objects */

#include "TiNGo_fmptx.h"
#include "TiNGcommon.h"
#include <stdlib.h>
#include <string.h>

LOCALDEF void fmptx_dtor(struct tSMFMPtxId_struct *vtp)
{
 vtp->groove.dtor(&vtp->groove);
 vtp->mod->card->vtbl->datafeed_dtor(vtp->dfin);
 mutx_dtor(&vtp->pm.mx);
 memset(vtp, 0xa5, sizeof(*vtp));		// for safety
 free(vtp);
}

LOCALDEF struct tSMFMPtxId_struct *init_fmptx(struct module *mod)
{
 struct tSMFMPtxId_struct *vtp = alloc(0,sizeof(*vtp));
 if (!vtp) return 0;
 memset(vtp, 0, sizeof(*vtp));
 vtp->mod = mod;
 if (SMGroove_module(&vtp->groove, mod)) {
 	free(vtp);
        return 0;
 }
 vtp->dfin = NULL_DATAFEED;
 mutx_init(&vtp->pm.mx);
 make_object(vtp);
 return vtp;
}

