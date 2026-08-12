/* TiNGo_cardmap.c - TiNG card mapping object */

#include "TiNGo_cardmap.h"
#include "TiNGcommon.h"
#include <stdlib.h>

LOCALDEF int cardmap_ctor(struct cardmap *cm)
{
 mutx_init(&cm->pm.mx);
 cm->ncards = 0;
 cm->cards = 0;
 make_object(cm);
 return 0;
}

#if 0
	// not used
LOCALDEF void cardmap_dtor(struct cardmap *cm)
{
 free(cm->cards);
 mutx_dtor(&cm->pm.mx);
}
#endif

LOCALDEF struct card *cardid2lockedcard(struct cardmap *cm, tSMCardId card_id)
{
 struct card *cardp = 0;
 unsigned u;
 lock_object(cm);
 for (u=0; u < cm->ncards; u++) {
	if (card_id == cm->cards[u].id) {
		cardp = cm->cards[u].cardp;
		lock_object(cardp);
		break;
	}
 }
 unlock_object(cm);
 return cardp;
}

int aculab_TiNG_cardmap_add(struct cardmap *cm, tSMCardId cardid, struct card *cardp)
{
 int sts = 0;
 unsigned u;
 lock_object(cm);
 for (u=0; ; u++) {
	if (u >= cm->ncards) {
		struct allcard *acp = alloc(cm->cards, sizeof(*acp) * (u + 1));
		if (acp) {
			cm->cards = acp;
			cm->cards[cm->ncards].id = cardid;
			cm->cards[cm->ncards].cardp = cardp;
			cm->cards[cm->ncards].refs = 1;
			cm->ncards++;
		} else {
			sts = err(ERR_SM_OS_RESOURCE_PROBLEM);
		}
		break;
	}
	if (cardid == cm->cards[u].id) {
		if (cm->cards[u].cardp == cardp) cm->cards[u].refs++;
		else sts = err(ERR_SM_BAD_PARAMETER);
		break;
	}
 }
 unlock_object(cm);
 return sts;
}

LOCALDEF int cardmap_del(struct cardmap *cm, tSMCardId cardid)
{
 struct card *cardp = 0;
 int sts = 0;
 unsigned u;
 lock_object(cm);
 for (u=0; ; u++) {
	if (u >= cm->ncards) {
		sts = err(ERR_SM_BAD_PARAMETER);
		break;
	}
	if (cardid == cm->cards[u].id) {
		if (!--cm->cards[u].refs) {
			cardp = cm->cards[u].cardp;
			if (--cm->ncards) cm->cards[u] = cm->cards[cm->ncards];
			else {
				free(cm->cards);
				cm->cards = 0;
			}
		}
		break;
	}
 }
 unlock_object(cm);
 if (cardp) cardp->dtor(cardp);
 return sts;
}

LOCALDEF int cardmap_findid(struct cardmap *cm, tSMCardId *idp, struct card *cardp)
{
 int sts = 0;
 unsigned u;
 lock_object(cm);
 for (u=0; ; u++) {
	if (u >= cm->ncards) {
		sts = err(ERR_SM_BAD_PARAMETER);
		break;
	}
	if (cm->cards[u].cardp == cardp) {
		*idp = cm->cards[u].id;
		break;
	}
 }
 unlock_object(cm);
 return sts;
}
