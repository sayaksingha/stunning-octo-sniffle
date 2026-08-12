/* px_ioctl_cardinfo.c - ioctl code for read/write cardinfo */

#include "px_ioctl_cardinfo.h"
#include "TiNGcommon.h"
#include "trace.h"
#include <string.h>

LOCALDEF int px_rdcardinfo(PX_CARDCONN_VTBL_DATA *pvp, struct ting_cardinfo *p)
{
 PX_CARDID *cid;
 if (pvp->dest_type != TiNG_DEST_CHANNEL
	&& pvp->dest_type != TiNG_DEST_MODULE
	&& pvp->dest_type != TiNG_DEST_CARD) {
	if (TiNGtrace) olog("rdcardinfo: bad dest: %d\n", pvp->dest_type);
 	return 1;
 }
 cid = pvp->id;
 if (!cid) {
	if (TiNGtrace) olog("rdcardinfo: no card id\n");
 	return 1;
 }
 p->cardtype = cid->card_type;
 p->ioaddress = 0;
 p->interrupt = 0;
 p->memaddress = 0;
 p->switchno = -1;
	// need to fill in serialno and verinfo
 strncpy(p->serialno, cid->serialno, sizeof(p->serialno) - 1);
 p->serialno[sizeof(p->serialno)-1] = 0;
 strncpy(p->verinfo, cid->verinfo, sizeof(p->verinfo) - 1);
 p->verinfo[sizeof(p->verinfo)-1] = 0;
 return 0;
}
