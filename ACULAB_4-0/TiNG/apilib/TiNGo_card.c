/* TiNGo_card.c - card objects */

#include "TiNGo_card.h"
#include "trace.h"
#include "TiNGcommon.h"
#include <stdlib.h>
#include <string.h>

#define arlen(x) (sizeof(x)/sizeof(*(x)))

STATIC void card_dtor(struct card *card)
{
 card->vtbl->card_dtor(card);
 free(card->cardname);
 mutx_dtor(&card->pm.mx);
 memset(card, 0xa5, sizeof(*card));		// for safety
 free(card);
}

struct card *aculab_TiNG_card_init(const struct card_vtbl *vtbl, char *cardname)
{
 struct ting_cardinfo cardinfo;
 struct ting_destination dst;
 struct card *newcard;
 unsigned physmod;
 CARDCONN cx;
 ERRCODE e;
 if (TiNGtrace > 1) olog("Init card: %s\n", cardname);
 newcard = alloc(0, sizeof(*newcard));
 if (!newcard) return 0;
 newcard->dtor = card_dtor;
 newcard->cardname = strdup(cardname);
 if (!newcard->cardname) {
	free(newcard);
	return 0;
 }
 if (ERR(e=vtbl->cardconn_ctor(&cx, newcard))) {
	printerr(e);
	free(newcard->cardname);
	free(newcard);
	return 0;
 }
 cardinfo.ioctl_length = sizeof(cardinfo);
 if (ERR(e=cx_ioctl(&cx, IOCTL_TiNG_RDCARDINFO,
		&cardinfo, sizeof(cardinfo)))) {
	printerr(e);
	free(newcard->cardname);
	printerr(cx_close(&cx));
	free(newcard);
	return 0;
 }
 if (TiNGtrace > 1) olog("card %s: '%s' '%s'\n",
  	cardname, cardinfo.serialno, cardinfo.verinfo);
 newcard->vtbl = vtbl;
 if (vtbl->card_ctor(newcard, &cardinfo)) {
	if (TiNGtrace > 1) olog("card %s failed to attach\n", cardname);
	free(newcard->cardname);
	printerr(cx_close(&cx));
	free(newcard);
	return 0;
 }
 newcard->swdrvr = cardinfo.switchno;
 dst.ioctl_length = sizeof(dst);
 if (ERR(e=cx_ioctl(&cx, IOCTL_TiNG_RDDEST, &dst, sizeof(dst)))) {
	printerr(e);
	free(newcard->cardname);
	printerr(cx_close(&cx));
	free(newcard);
	return 0;
 }
#if TiNG_TRACE > 1
 if (trace_vtbl == lvtbl && trace_vtbl->io_isinvalid(trace_hand)) {
	struct ting_destination tdst;
	tdst.ioctl_length = sizeof(tdst);
	tdst.dest = TiNG_DEST_TRACEIN;
	if (ERR(e=lvtbl->io_ioctlfile(cx, IOCTL_TiNG_WRDEST, &tdst, sizeof(tdst)))) {
		printerr(e);
		free(newcard->cardname);
		printerr(cx_close(&cx));
		free(newcard);
		return 0;
	}
 	trace_hand = dev;
 } else
#endif
	// watch out for that 'else' in the #if
 if (ERR(e=cx_close(&cx))) {
	printerr(e);
	free(newcard->cardname);
	free(newcard);
	return 0;
 }
 mutx_init(&newcard->pm.mx);
 newcard->modmask = dst.modmask;
 newcard->nmod = 0;
 for (physmod=0; ; physmod++) {
	unsigned long m = 1 << physmod;
	if (!dst.modmask) {
		make_object(newcard);
		return newcard;
 	}
 	if (dst.modmask & m) {
		dst.modmask &= ~m;
		newcard->nmod++;
	}
 }
 card_dtor(newcard);
 return 0;
}
