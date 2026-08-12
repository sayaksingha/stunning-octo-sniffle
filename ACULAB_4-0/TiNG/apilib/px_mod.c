/* px_mod.c - Prosody X modules */

#include "px_mod.h"
#include "TiNGcommon.h"
#include "trace.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

STATIC void free_pxmod(void *p)
{
 PX_MOD *pxmod = p;
 PX_CARDID *cid = pxmod->cid;
 refcnt_dec(&cid->ref);
 asspgroup_dtor(&pxmod->modgroup);
 free(pxmod);
}

LOCALDEF PX_MOD *allocateProsodyXModule(PX_CARDID *cid, int modmask, char* remaddr, int port)
{
 PX_MOD *pxmod = alloc(0, sizeof(*pxmod));
 if (!pxmod) return 0;
 pxmod->modmask = modmask;
 if (asspgroup_ctor(&pxmod->modgroup, remaddr, cid->remaddr, cid->asspmonport, port, cid->key)) {
	free(pxmod);
	return 0;
 }
 pxmod->cid = cid;
 refcnt_inc(&cid->ref);
 refcnt(&pxmod->ref, free_pxmod, pxmod);
 return pxmod;
}

LOCALDEF void freeProsodyXModule(PX_MOD *pxmod)
{
 refcnt_dec(&pxmod->ref);
}

LOCALDEF void shutdownProsodyXModule(PX_MOD *pxmod)
{
	asspgroup_shutdown(&pxmod->modgroup);
}
