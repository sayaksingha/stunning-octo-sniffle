/* px_ioctl_modinfo.c - ioctl code for read/write modinfo */

#include "px_ioctl_modinfo.h"
#include "TiNGcommon.h"
#include "trace.h"
#include <string.h>

LOCALDEC int px_rdmodinfo(PX_MOD *pxmod, struct ting_modinfo *p)
{
 PX_CARDID *cid = pxmod->cid;
 unsigned u;
 if (!cid) {
	if (TiNGtrace) olog("rdmodinfo: no card id\n");
	return 1;
 }

 if (cid->card_type == TiNG_CARDTYPE_X) {
	unsigned long mask = p->modmask;
	for (u=0; mask; u++) mask >>= 1;
	p->modslot = u - 1;
	if (p->modslot >= 4) {
		p->modfirststream = 72 + 2 * p->modslot;
	} else {
		p->modfirststream = 64 + 2 * p->modslot;
	}
	// Here it would be nice if for ProsodyX 1U Enterprise we could say 1 stream rather than 2
	// If we ever update cardinfod to do that, then this will be picked up here.
	if (cid->ntdmcards == 1) {
		p->modlaststream = p->modfirststream+(cid->tdmcards[0].laststream - cid->tdmcards[0].firststream);
	} else {
		p->modlaststream = 1+p->modfirststream;
	}
 } else if (cid->ntdmcards > 0) {
	p->modfirststream = cid->tdmcards[0].firststream;
	p->modlaststream = cid->tdmcards[cid->ntdmcards-1].laststream;
	p->modslot = 0;
 } else if (cid->ntdmcards < 0) {
    // no tdmlist response with older PS - return backwards compatible behaviour for capx
	p->modfirststream = 64;
	p->modlaststream = 64;
	p->modslot = 0;
 } else {
	// PS with no TDM
	p->modfirststream = 0;
	p->modlaststream = 0;
	p->modslot = 0;
 }
 return 0;
}

