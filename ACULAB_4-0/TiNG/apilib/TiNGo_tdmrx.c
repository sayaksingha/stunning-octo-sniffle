/* TiNGo_tdmrx.c - TiNG tdmrx objects */

#include "TiNGo_tdmrx.h"
#include "TiNGcommon.h"
#include "TiNGo_datafeed.h"
#include <stdlib.h>
#include <string.h>

LOCALDEF void tdmrx_dtor(struct tSMTDMrxId_struct *trp)
{
 trp->mod->card->vtbl->datafeed_dtor(trp->dataf);
 mutx_dtor(&trp->pm.mx);
 memset(trp, 0xa5, sizeof(*trp));		// for safety
 free(trp);
}

LOCALDEF struct tSMTDMrxId_struct *init_tdmrx(struct module *mod)
{
 struct tSMTDMrxId_struct *trp = alloc(0, sizeof(*trp));
 if (!trp) return 0;
 memset(trp, 0, sizeof(*trp));
 trp->mod = mod;
 mutx_init(&trp->pm.mx);
 trp->dataf = mod->card->vtbl->datafeed();
 make_object(trp);
 return trp;
}
