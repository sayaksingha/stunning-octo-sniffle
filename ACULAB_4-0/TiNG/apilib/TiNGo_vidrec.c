/* TiNGo_vidrec.c - TiNG vidrec objects */

#include "TiNGo_vidrec.h"
#include "TiNGcommon.h"
#include <stdlib.h>
#include <string.h>

LOCALDEF void avfrec_dtor(struct tSMAVFrecId_struct *avfrec)
{
 avfrec->groove.dtor(&avfrec->groove);
 avfrec->mod->card->vtbl->datafeed_dtor(avfrec->dataf);
 mutx_dtor(&avfrec->pm.mx);
 memset(avfrec, 0xa5, sizeof(*avfrec));		// for safety
 free(avfrec);
}

LOCALDEF struct tSMAVFrecId_struct *init_avfrec(struct module *mod)
{
 struct tSMAVFrecId_struct *avfrec = alloc(0, sizeof(*avfrec));
 if (!avfrec) return 0;
 memset(avfrec, 0, sizeof(*avfrec));
 avfrec->mod = mod;
 avfrec->dataf = NULL_DATAFEED;
 if (SMGroove_module(&avfrec->groove, mod)) {
 	mod->card->vtbl->datafeed_dtor(avfrec->dataf);
 	free(avfrec);
 	return 0;
 }
 mutx_init(&avfrec->pm.mx);
 make_object(avfrec);
 return avfrec;
}
