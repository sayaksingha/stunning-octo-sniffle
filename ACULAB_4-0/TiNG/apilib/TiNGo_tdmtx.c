/* TiNGo_tdmtx.c - TiNG tdmtx objects */

#include "TiNGo_tdmtx.h"
#include "TiNGcommon.h"
#include <stdlib.h>
#include <string.h>

LOCALDEF void tdmtx_dtor(struct tSMTDMtxId_struct *ttp)
{
 if (nonnull_datafeed(ttp->dataf)) {
	ttp->mod->card->vtbl->tdm_assign(ttp->dataf, -1, -1, -1, 0);
	ttp->mod->card->vtbl->datafeed_dtor(ttp->dataf);
 }
 mutx_dtor(&ttp->pm.mx);
 memset(ttp, 0xa5, sizeof(*ttp));		// for safety
 free(ttp);
}

LOCALDEF struct tSMTDMtxId_struct *init_tdmtx(struct module *mod)
{
 struct tSMTDMtxId_struct *ttp = alloc(0, sizeof(*ttp));
 if (!ttp) return 0;
 memset(ttp, 0, sizeof(*ttp));
 ttp->mod = mod;
 ttp->dataf = NULL_DATAFEED;
 mutx_init(&ttp->pm.mx);
 make_object(ttp);
 return ttp;
}
