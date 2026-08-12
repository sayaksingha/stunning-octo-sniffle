#ifndef INLCUDED_MODLIST_H
#define INLCUDED_MODLIST_H

#include "smdrvr.h"
#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct moddesc {
	tSMModuleId id;
	char *logname;
} MODDESC;

typedef struct modlist {
	unsigned nmod;
	MODDESC *mods;
} MODLIST;

err_t modlist(MODLIST *lp);

err_t modlist_addmods(MODLIST *lp, tSMCardId cid, char **specp, char *cname);

void modlist_dtor(MODLIST *lp);

#ifdef __cplusplus
}
#endif
#endif
