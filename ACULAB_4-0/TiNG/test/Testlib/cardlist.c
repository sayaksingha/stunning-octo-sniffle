#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "cardlist.h"
#include "errcode_sm.h"


#ifndef NO_PROSODY_X
#include "prosody_x.h"
#endif

	// Convert a Prosody error code and an explanation into an err_t
static err_t prosody_error(int rc, char *text)
{
 const char *name = errcode_sm(rc);
 char buf[80];
 if (!name) {
 	sprintf(buf, "Prosody error %d\n", rc);
 	name = buf;
 }
 return error(error(0, name), text);
}

err_t cardlist(CARDLIST *lp)
{
 lp->ncard = 0;
 lp->card = 0;
 return 0;
}

static tSMCardId nextcard = 0xacacacac;

static err_t addcard(CARDLIST *lp, tSMCardId cid, char **specp, char *cname)
{
 struct cardspec *sp = realloc(lp->card, (lp->ncard + 1) * sizeof(*sp));
 err_t err;
 if (!sp) {
	return error(error_errno(errno, "realloc() failed"),
		"cannot extend card list");
 }
 lp->card = sp;
 lp->card[lp->ncard].cid = cid;
 err = modlist(&lp->card[lp->ncard].modlist);
 if (err) return err;
 return modlist_addmods(&lp->card[lp->ncard++].modlist, cid, specp, cname);
}


#ifndef NO_PROSODY_S_V3
static err_t addcard_s_v3(CARDLIST *lp, char *spec, char **specp)
{
 SM_OPEN_PROSODY_S_V3_PARMS pp;
 unsigned len = *specp - spec;
 char *t = malloc(len + sizeof("s3:"));
 err_t e;
 int r;
 if (!t) {
	return error(error_errno(errno, "malloc() failed"),
		"cannot allocate space for card name");
 }
 t[0] = 's';
 t[1] = '3';
 t[2] = ':';
 strncpy(t + 3, spec, len);
 t[len + 3] = '\0';
 memset(&pp, 0, sizeof(pp));
 pp.ps_locator = t+3;
 pp.card_id = nextcard++;
 r = sm_open_prosody_s_v3(&pp);
 if (r) {
	free(t);
	return prosody_error(r, "sm_open_prosody_s_v3() failed");
 }
 e = addcard(lp, pp.card_id, specp, t);
 free(t);
	// *specp has been advanced past any [...] module spec
 if (e) return e;
 return 0;
}
#endif

#ifndef NO_PROSODY_X
static err_t addcard_x(CARDLIST *lp, char *spec, char **specp)
{
 SM_OPEN_PROSODY_X_PARMS pp;
 unsigned len = *specp - spec;
 char *t = malloc(len + sizeof("x:"));
 err_t e;
 int r;
 if (!t) {
	return error(error_errno(errno, "malloc() failed"),
		"cannot allocate space for card name");
 }
 t[0] = 'x';
 t[1] = ':';
 strncpy(t + 2, spec, len);
 t[len + 2] = '\0';
 memset(&pp, 0, sizeof(pp));
 pp.px_locator = t+2;
 pp.card_id = nextcard++;
 r = sm_open_prosody_x(&pp);
 if (r) {
	free(t);
	return prosody_error(r, "sm_open_prosody_x() failed");
 }
 e = addcard(lp, pp.card_id, specp, t);
 free(t);
	// *specp has been advanced past any [...] module spec
 if (e) return e;
 return 0;
}
#endif

err_t cardlist_addcard(CARDLIST *lp, char *spec)
{
 char *colp = 0;
 char *ep;
 for (ep=spec; *spec; ep++) {
 		// spec -> start of serial number
 	if (*ep == ',' || *ep == '[' || !*ep) {
		int type = 'p';
 			// ep -> one char after end of serial number
 		if (colp && spec[1] == ':') {
 			type = spec[0];
 			spec += 2;
		}
#ifndef NO_PROSODY_S_V3
		if (colp && spec[0] == 's' && spec[1] == '3' && spec[2] == ':') {
			// Prosody S v3
			type = 'S';
			spec +=3;
		}
#endif
		if (0) {
#ifndef NO_PROSODY_S_V3
		} else if (type == 'S') {
			err_t e = addcard_s_v3(lp, spec, &ep);
				// ep has been advanced past any [...] module spec
			if (e) return e;
#endif
#ifndef NO_PROSODY_X
		} else if (type == 'x') {
			err_t e = addcard_x(lp, spec, &ep);
				// ep has been advanced past any [...] module spec
			if (e) return e;
#endif
		} else {
			return error(0, "Unknown Prosody type");
		}
		if (!*ep) return 0;
		spec = ep + 1;	// skip separator
		colp = 0;
	} else if (*ep == ':' && !colp) {
		colp = ep;
		if (*(ep + 1) == '[') { // IPv6 address
			ep++; // skip on to stop it being treated as modspec
		}
	}
 }
 return 0;
}

err_t cardlist_nthmod(MODDESC *mdp, CARDLIST *lp, unsigned mod)
{
 unsigned card = 0;
 if (!lp->ncard) return error(0, "insufficient modules defined");
 while (mod >= lp->card[card].modlist.nmod) {
 	mod -= lp->card[card].modlist.nmod;
 	card++;
	if (card >= lp->ncard) {
		return error(0, "insufficient modules defined");
	}
 }
 *mdp = lp->card[card].modlist.mods[mod];
 return 0;
}

err_t cardlist_nummod(unsigned *nmodp, CARDLIST *lp)
{
 unsigned card;
 *nmodp = 0;
 for (card=0; card < lp->ncard; card++) {
	*nmodp += lp->card[card].modlist.nmod;
 }
 return 0;
}

err_t cardlist_getmod(MODDESC *mdp, CARDLIST *lp, unsigned card, unsigned mod)
{
 if (card >= lp->ncard) {
	return error(0, "insufficient cards defined");
 }
 if (mod >= lp->card[card].modlist.nmod) {
	return error(0, "Internal error: no such module on card");
 }
 *mdp = lp->card[card].modlist.mods[mod];
 return 0;
}

void cardlist_nextmod(CARDLIST *lp, unsigned *cardp, unsigned *modp)
{
 if (++*modp == lp->card[*cardp].modlist.nmod) {
	*modp = 0;
	++*cardp;
 }
}

void cardlist_dtor(CARDLIST *lp)
{
 for (; lp->ncard; ) {
 	SM_CLOSE_PROSODY_PARMS pp;
 	int e;
	memset(&pp, 0, sizeof(pp));
 	lp->ncard--;
 	modlist_dtor(&lp->card[lp->ncard].modlist);
 	pp.card_id = lp->card[lp->ncard].cid;
 	e = sm_close_prosody(&pp);
 	if (e) {
 		fprintf(stderr, "Ignoring error %d from sm_close_prosody()\n",
 			e);
	}
 }
 free(lp->card);
}
