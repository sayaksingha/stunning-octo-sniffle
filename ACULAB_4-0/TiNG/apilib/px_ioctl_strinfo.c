/* px_ioctl_strinfo.c - ioctl code for read/write strinfo */

#include "px_ioctl_strinfo.h"
#include "TiNGcommon.h"
#include "trace.h"
#include <string.h>

LOCALDEC int px_rdstrinfo(PX_MOD *pxmod, struct ting_strinfo *p)
{
 int found = 0;
 unsigned base;
 int i;
 PX_CARDID *cid = pxmod->cid;
 if (!cid) {
	if (TiNGtrace) olog("rdstrinfo: no card id\n");
	return 1;
 }
 if (cid->card_type == TiNG_CARDTYPE_X){
	strcpy(&p->tdmcard[0],&(cid->serialno[0]));
	p->serialportix = p->modstrix;
	p->timeslots    = 128;
 } else if (cid->ntdmcards > 0) {
	base = cid->tdmcards[0].firststream;
	for (i = 0; i < cid->ntdmcards; i++) {
		if ((p->modstrix >= (cid->tdmcards[i].firststream-base))
				&& (p->modstrix <= (cid->tdmcards[i].laststream-base)) ) {
			strcpy(&p->tdmcard[0],&cid->tdmcards[i].serialno[0]);
			p->serialportix  = p->modstrix - (cid->tdmcards[i].firststream-base); // relative to card not module
			p->timeslots = cid->tdmcards[i].timeslots;
			found = 1;
			break;
		}
	}
	if (!found) {
		if (TiNGtrace) olog("rdstrinfo: bad stream ix\n");
		return 1;
	}
 } else if (cid->ntdmcards < 0) {
	strcpy(&p->tdmcard[0],&(cid->serialno[0]));
	p->serialportix = 0;
	p->timeslots    = 128;
 } else {
	if (TiNGtrace) olog("rdstrinfo: PS without TDM\n");
	return 1;
 }
 return 0;
}

