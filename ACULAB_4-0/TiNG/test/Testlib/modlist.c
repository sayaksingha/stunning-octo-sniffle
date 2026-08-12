#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "modlist.h"
#include "errcode_sm.h"

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

err_t modlist(MODLIST *lp)
{
 lp->nmod = 0;
 lp->mods = 0;
 return 0;
}

static err_t modlist_addone(MODLIST *lp, tSMModuleId mid, char *logname)
{
 MODDESC *mp = realloc(lp->mods, (lp->nmod + 1) * sizeof(*mp));
 if (!mp) {
	return error(error_errno(errno, "realloc() failed"),
		"cannot extend module list");
 }
 lp->mods = mp;
 lp->mods[lp->nmod].id = mid;
 if (! (lp->mods[lp->nmod].logname = strdup(logname))) {
		// note that we haven't yet incremented nmod, so the modlist
		// appears to be unchanged
	return error(error_errno(errno, "strdup() failed"),
		"cannot copy module name");
 }
 lp->nmod++;
 return 0;
}

err_t modlist_addmods(MODLIST *lp, tSMCardId cid, char **specp, char *cname)
{
 char *spec = *specp;
 if (*spec == '[') {
	for (;;) {
		SM_OPEN_MODULE_PARMS mp;
		char logname[128];
		err_t err;
		int e;
		spec++;
		if (*spec == ']') break;
		if (!*spec) return error(0, "module spec [...] has no ']'");
		if ((unsigned) (*spec - '0') > 9) {
			return error(0, "module spec in [...] does not start with digit']'");
		}
		memset(&mp, 0, sizeof(mp));
		mp.card_id = cid;
		mp.module_ix = strtol(spec, &spec, 0);
		sprintf(logname, "%s[%u]", cname, (unsigned) mp.module_ix);
		e = sm_open_module(&mp);
		if (e) return prosody_error(e, "sm_open_module() failed");
		err = modlist_addone(lp, mp.module_id, logname);
		if (err) return err;
		if (*spec == ']') break;
		if (*spec != ',') return error(0, "not ',' or ']' after module");
	}
	*specp = spec + 1;
 } else {	// add all of them
	SM_CARD_INFO_PARMS pp;
	unsigned m;
	int e;
	memset(&pp, 0, sizeof(pp));
	pp.card = cid;
	e = sm_get_card_info(&pp);
	if (e) return prosody_error(e, "sm_get_card_info() failed");
	for (m=0; pp.module_count; m++) {
		SM_OPEN_MODULE_PARMS mp;
		char logname[128];
		memset(&mp, 0, sizeof(mp));
		mp.card_id = cid;
		mp.module_ix = m;
		sprintf(logname, "%s[%u]", cname, (unsigned) mp.module_ix);
		e = sm_open_module(&mp);
		if (!e) {
			err_t err = modlist_addone(lp, mp.module_id, logname);
			if (err) return err;
			pp.module_count--;	// one fewer to find
		} else if (e != ERR_SM_NO_SUCH_MODULE) {
			return prosody_error(e, "sm_open_module() failed");
		}
	}
 }
 return 0;
}

void modlist_dtor(MODLIST *lp)
{
 unsigned ii = lp->nmod;
 for (; ii; ) {
	SM_SHUTDOWN_MODULE_PARMS pp;
	SM_MODULE_STATUS_PARMS sp;
	SM_MODULE_EVENT_PARMS ep;
	int r;
	ii--;
	memset(&ep,0,sizeof(ep));
	ep.module_id = lp->mods[ii].id;
	r = sm_module_get_event(&ep);
 	if (r) {
 		fprintf(stderr, "Ignoring error %d from sm_module_get_event()\n",
 			r);
	}
	memset(&sp,0,sizeof(sp));
	sp.module_id = lp->mods[ii].id;
	r = sm_module_status(&sp);
 	if (r) {
 		fprintf(stderr, "Ignoring error %d from sm_module_status()\n",
 			r);
	}
	memset(&pp, 0, sizeof(pp));
	pp.module_id = lp->mods[ii].id;
	r = sm_shutdown_module(&pp);
 	if (r) {
 		fprintf(stderr, "Ignoring error %d from sm_shutdown_module()\n",
 			r);
	}
	for(;;) {
		smd_ev_wait(ep.event);
		memset(&sp,0,sizeof(sp));
		sp.module_id = lp->mods[ii].id;
		r = sm_module_status(&sp);
 		if (r) {
 			fprintf(stderr, "Error %d from sm_module_status()\n",
 				r);
			break;
		}
		if (sp.status==kSMModuleStatusShutdown) break;
	}

 }
 for (; lp->nmod; ) {
	SM_CLOSE_MODULE_PARMS pp;
	int r;
	lp->nmod--;
	memset(&pp, 0, sizeof(pp));
	pp.module_id = lp->mods[lp->nmod].id;
	r = sm_close_module(&pp);
 	if (r) {
 		fprintf(stderr, "Ignoring error %d from sm_close_module()\n",
 			r);
	}
	free(lp->mods[lp->nmod].logname);
 }
 free(lp->mods);
}
