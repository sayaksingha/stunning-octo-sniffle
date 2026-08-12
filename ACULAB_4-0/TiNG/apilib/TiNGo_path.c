/* TiNGo_path.c - TiNG path objects */

#include "TiNGo_path.h"
#include "TiNGcommon.h"
#include <stdlib.h>
#include <string.h>

LOCALDEF void path_dtor(struct tSMPathId_struct *path)
{
 path->groove.unbind_df(&path->groove, path->dataf);
 path->groove.dtor(&path->groove);
 path->mod->card->vtbl->datafeed_dtor(path->dataf);
 path->mod->card->vtbl->datafeed_dtor(path->dfin);
 path->mod->card->vtbl->datafeed_dtor(path->ec.refdf);
 path->mod->card->vtbl->datafeed_dtor(path->mix.bgdf);
 mutx_dtor(&path->pm.mx);
 memset(path, 0xa5, sizeof(*path));		// for safety
 free(path);
}

LOCALDEF struct tSMPathId_struct *init_path(struct module *mod)
{
 struct tSMPathId_struct *path = alloc(0, sizeof(*path));
 if (!path) return 0;
 memset(path, 0, sizeof(*path));
 path->mod = mod;
 path->dataf = mod->card->vtbl->datafeed();
 if (!nonnull_datafeed(path->dataf)) {
 	free(path);
 	return 0;
 }
 if (SMGroove_module(&path->groove, mod)) {
 	mod->card->vtbl->datafeed_dtor(path->dataf);
 	free(path);
 	return 0;
 }
 path->dfin = NULL_DATAFEED;
 path->ec.running = 0;
 path->ec.refdf = NULL_DATAFEED;
 path->agc.running = 0;
 path->mix.running = 0;
 path->mix.bgdf = NULL_DATAFEED;
 path->pitchshift.running = 0;
 path->resample.running = 0;
 path->codec.running = kPathCodecNone;
 path->emphasis.running = 0;
 path->delay.running = 0;
 path->ttyeliminate.running = 0;
 mutx_init(&path->pm.mx);
 make_object(path);
 return path;
}
