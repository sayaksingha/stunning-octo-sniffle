#ifndef INCLUDED_CARDLIST_H
#define INCLUDED_CARDLIST_H

#include "smdrvr.h"
#include "modlist.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PROSODY_TiNG
typedef int tSMCardId;
#endif

typedef struct {
	unsigned ncard;
	struct cardspec {
		tSMCardId cid;
		MODLIST modlist;
	} *card;
} CARDLIST;

#include "error.h"

err_t cardlist(CARDLIST *lp);

err_t cardlist_addcard(CARDLIST *lp, char *spec);

err_t cardlist_getmod(MODDESC *mp, CARDLIST *lp, unsigned card, unsigned mod);

err_t cardlist_nthmod(MODDESC *mp, CARDLIST *lp, unsigned mod);

err_t cardlist_nummod(unsigned *nmodp, CARDLIST *lp);

void cardlist_nextmod(CARDLIST *lp, unsigned *cardp, unsigned *modp);

void cardlist_dtor(CARDLIST *lp);

#ifdef __cplusplus
}
#endif
#endif
