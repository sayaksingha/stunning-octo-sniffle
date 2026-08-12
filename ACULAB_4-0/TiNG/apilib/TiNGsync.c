#include "smsync.h"

#include "paranoia.h"
#include "rawchan.h"
#include "TiNGcommon.h"
#include "trace.h"
#include "../hsif/typecode.h"
#include <stdlib.h>

struct tSMSynchroniser_struct {
	PARAMUTX pm;
	RAWCHAN rawc;
	struct module *mp;
};

STATIC int real_sm_sync_create(struct sm_sync_create_parms *pp)
{
 struct module *mp = modid2lockedmodule(pp->module);
 int sts;
 if (!mp) return err(ERR_SM_NO_SUCH_MODULE);
 pp->synchroniser = alloc(0, sizeof(*pp->synchroniser));
 if (!pp->synchroniser) {
	sts = err(ERR_SM_NO_RESOURCES);
 } else {
	mutx_init(&pp->synchroniser->pm.mx);
	pp->synchroniser->mp = mp;
	sts = open_channel(&pp->synchroniser->rawc, mp);
	if (!sts) sts = starttask(&pp->synchroniser->rawc.cx, TASKTC_SYNC, 0);
 }
 unlock_module(mp);
 return sts;
}

STATIC int real_sm_sync_add(struct sm_sync_add_parms *pp)
{
 tSMChannelId chan = pp->channel;
 enum handles hand;
 int sts = 0;
 lock_object(pp->synchroniser);
 if (pp->synchroniser->mp != chan->mod) sts = err(ERR_SM_BAD_PARAMETER);
 else for (hand = HANDLE_WRITE; hand < HANDLE_NUM; hand++) {
	RAWCHAN *rc = &chan->chan[hand];
	if (cardconn_valid(&rc->cx)) {
		sts = setfriend(&pp->synchroniser->rawc.cx, rc, 0);
		if (sts) break;
	}
 }
 unlock_object(pp->synchroniser);
 return sts;
}

STATIC int real_sm_sync_delete(struct sm_sync_delete_parms *pp)
{
 tSMSynchroniser sp = pp->synchroniser;
 int sts = 0;
 lock_object(sp);
 if (ERR(cx_close(&sp->rawc.cx))) sts = err(ERR_SM_DEVERR);
 unlock_object(sp);
 mutx_dtor(&sp->pm.mx);
 free(sp);
 return sts;
}

#include "sync_trace.i"
