/* modelset.h - isolated word recognition model set */

#ifndef INCLUDED_MODELSET_H
#define INCLUDED_MODELSET_H

#include "smbesp.h"

typedef struct {
	tSMModuleId module;
	tSM_UT32 *model;
	tSM_INT *id;
	unsigned nmodel;
} MODEL_SET;

// initialise a model set
int model_set(MODEL_SET *msp, tSMModuleId module);

// add a model to a model set
int model_set_download(MODEL_SET *msp, char *fname);

// prepare the listen parameters from a model set
int model_set_listen(SM_ASR_LISTEN_FOR_PARMS *alfp, MODEL_SET *msp);

// destroy a model set
void model_set_dtor(MODEL_SET *msp);

#endif
