/* px_ioctl_dest.c - ioctl code for read/write destination */

#include "px_ioctl_dest.h"
#include "../hsif/kernel.h"
#include "../hsif/kernel_x.h"
#include "TiNGcommon.h"
#include "trace.h"
#include <stdio.h>
#include <string.h>

LOCALDEF int px_rddest(PX_CARDCONN_VTBL_DATA *pvp, struct ting_destination *p)
{
 memset(p, 0, sizeof(*p));
 p->ioctl_length = sizeof(*p);
 p->dest = pvp->dest_type;
 switch (pvp->dest_type) {
 	unsigned u;
 case TiNG_DEST_CARD:
	p->modmask = 0;
	for (u=0; u < pvp->id->nmod; u++) {
		p->modmask |= 1 << pvp->id->mod[u].physmod;
	}
	break;
 case TiNG_DEST_MODULE:
	p->modmask = pvp->u.mod->modmask;
	break;
 case TiNG_DEST_CHANNEL:
	p->modmask = pvp->u.chan->pxmod->modmask;
	p->taskid  = TASK_GENID_L(pvp->u.chan->chancx->taskgen, pvp->u.chan->chancx->taskid); //uses full id here
		//uses full id and gen here
	p->magic  = TASK_GENID_L(pvp->u.chan->chancx->taskgen, pvp->u.chan->chancx->taskid);
		//uses the full id here
	p->channo = pvp->u.chan->chancx->taskid;
	break;
 default:
 	die("Assert failed: bad px dest\n");
 }
 return 0;
}

LOCALDEF int px_wrdest(PX_CARDCONN_VTBL_DATA *pvp, struct ting_destination *p)
{
 switch (p->dest) {
	PX_CHAN *chan;
	char *remaddr;
 	unsigned port;
 	unsigned u;
 case TiNG_DEST_MODULE:
	if (pvp->dest_type != TiNG_DEST_CARD) {
		if (TiNGtrace) olog("px_wrdest: bad dest: %x -> mod\n", pvp->dest_type);
		return 1;
	}
	for (u=0; ; u++) {
		if (u >= pvp->id->nmod) {
			if (TiNGtrace) olog("no such module: %u\n", u);
			return 1;
		}
		if (1 << pvp->id->mod[u].physmod & p->modmask) {
			port = pvp->id->mod[u].firstport + 1;
			remaddr = pvp->id->mod[u].addrname;
			break;
		}
	}
	if (strlen(remaddr) == 0) {
		remaddr = pvp->id->remaddr;
	} else if (strchr(remaddr, '%') == NULL) {
		// module specific address does not have a scope, so append scope from card address if it has one
		char* scope = strchr(pvp->id->remaddr, '%');
		if (scope) strcat(remaddr, scope);
	}
	pvp->u.mod = allocateProsodyXModule(pvp->id, p->modmask, remaddr, port);
	if (!pvp->u.mod) return 1;
	break;
 case TiNG_DEST_CHANNEL:
	if (pvp->dest_type != TiNG_DEST_MODULE) {
		if (TiNGtrace) olog("px_wrdest: bad dest: %x -> chan\n", pvp->dest_type);
		return 1;
	}
	chan = allocateProsodyXChannel(pvp->u.mod);
	if (!chan) return 1;
	freeProsodyXModule(pvp->u.mod);
	pvp->u.chan = chan;
	break;
 default:
	if (TiNGtrace) olog("px_wrdest: bad dest: %x -> %x\n", pvp->dest_type, p->dest);
	return 1;
 }
 pvp->dest_type = p->dest;
 return px_rddest(pvp, p);
}
