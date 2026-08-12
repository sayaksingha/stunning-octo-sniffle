/* TiNGo_cardmap.h - TiNG card mapping object */

#ifndef INCLUDED_TiNGO_CARDMAP_H
#define INCLUDED_TiNGO_CARDMAP_H

#include "paranoia.h"
#include "smtypes.h"
#include "TiNGo_card.h"

struct cardmap {
	PARAMUTX pm;
	unsigned ncards;
	struct allcard {
		tSMCardId id;
		struct card *cardp;
		unsigned refs;
	} *cards;
};

/*
 * The general purpose of this class is to provide mapping from the
 * (arbitrary) tSMCardId values supplied by the user and the internal
 * card objects. Mappings are reference counted, so a tSMCardId can be
 * registered multiple times. This is essential to allow the same card
 * to be opened more than once using the same ID (which the V6 API tends
 * to make people to, as the use the ACU_CARD_ID as the tSMCardId value).
 */
LOCALDEC int cardmap_ctor(struct cardmap *cm);
LOCALDEC struct card *cardid2lockedcard(struct cardmap *cm, tSMCardId card_id);
int aculab_TiNG_cardmap_add(struct cardmap *cm, tSMCardId cardid, struct card *cardp);
LOCALDEC int cardmap_del(struct cardmap *cm, tSMCardId cardid);
	// add a new card, automatically allocating a tSMCardId
	// the allocations are guaranteed not to conflict if this is
	// the only method used to allocate them

	// convert a card to its registered ID
LOCALDEC int cardmap_findid(struct cardmap *cm, tSMCardId *idp, struct card *cardp);

	// return the total number of cards
	// cards added multiple times with cardmap_append() will be
	// counted multiple times

#if 0
	// not used
LOCALDEC void cardmap_dtor(struct cardmap *cm);
#endif

#endif
