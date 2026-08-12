/* modelset.c - isolated word recognition model set */

#include "modelset.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../Testlib/errcode_sm.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

int model_set(MODEL_SET *msp, tSMModuleId module)
{
 msp->module = module;
 msp->model = 0;
 msp->id = 0;
 msp->nmodel = 0;
 return 0;
}

int model_set_download(MODEL_SET *msp, char *fname)
{
 SM_INPUT_VOCAB_PARMS aivp;
 int e;
 {
  tSM_UT32 *nmp = realloc(msp->model, (msp->nmodel + 1) * sizeof(*nmp));
  if (!nmp) {
  	perror("realloc() failed");
	return 1;
  }
  msp->model = nmp;
 }
 {
  tSM_INT *nidp = realloc(msp->id, (msp->nmodel + 1) * sizeof(*nidp));
  if (!nidp) {
  	perror("realloc() failed");
	return 1;
  }
  msp->id = nidp;
 }
 msp->id[msp->nmodel] = msp->nmodel;	// default
 if (*fname == '=') {		// explicit use of default
	fname++;
 } else {
	char *ep;
	long id = strtol(fname, &ep, 0);
	if (*ep == '=') {	// non-default
		msp->id[msp->nmodel] = id;
		fname = ep + 1;
	}	// else we have implicit use of default
 }
 memset(&aivp, 0, sizeof(aivp));
 aivp.module = msp->module;
 aivp.filename = fname;
 e = sm_add_input_vocab(&aivp);
 if (e) {
	const char *name = errcode_sm(e);
	if (!name) name = "?";
	fprintf(stderr, "sm_add_input_vocab(%s) returned Prosody error %d (%s)\n", fname, e, name);
	return 1;
 }
 msp->model[msp->nmodel] = aivp.item_id;
 msp->nmodel++;
 return 0;
}

int model_set_listen(SM_ASR_LISTEN_FOR_PARMS *alfp, MODEL_SET *msp)
{
 alfp->vocab_item_count = msp->nmodel;
 alfp->vocab_item_ids = msp->model;
 alfp->vocab_recog_ids = msp->id;
 return 0;
}

void model_set_dtor(MODEL_SET *msp)
{
 free(msp->model);
 free(msp->id);
}
