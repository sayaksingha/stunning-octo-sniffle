/* TiNGo_vidplay.c - TiNG vidplay objects */

#include "TiNGo_vidplay.h"
#include "TiNGcommon.h"
#include <stdlib.h>
#include <string.h>

LOCALDEF void avfplay_dtor(struct tSMAVFplayId_struct *avfplay)
{
 avfplay->groove.dtor(&avfplay->groove);
 avfplay->mod->card->vtbl->datafeed_dtor(avfplay->dataf);
 mutx_dtor(&avfplay->pm.mx);
 memset(avfplay, 0xa5, sizeof(*avfplay));		// for safety
 free(avfplay);
}

LOCALDEF struct tSMAVFplayId_struct *init_avfplay(struct module *mod)
{
 struct tSMAVFplayId_struct *avfplay = alloc(0, sizeof(*avfplay));
 if (!avfplay) return 0;
 memset(avfplay, 0, sizeof(*avfplay));
 avfplay->mod = mod;
 avfplay->dataf = mod->card->vtbl->datafeed();
 avfplay->state = ST_REPLAY_IDLE;
 if (!nonnull_datafeed(avfplay->dataf)) {
 	free(avfplay);
 	return 0;
 }
 if (SMGroove_module(&avfplay->groove, mod)) {
 	mod->card->vtbl->datafeed_dtor(avfplay->dataf);
 	free(avfplay);
 	return 0;
 }
 mutx_init(&avfplay->pm.mx);
 make_object(avfplay);
 return avfplay;
}
