#ifndef TINGOVMPTONESHINCLUDED
#define TINGOVMPTONESHINCLUDED

#include "refcnt.h"
#include "smgroove.h"

struct tSMVMPTxToneSetId_struct {
	REFCNT ref;
	tSMGroove groove;
};

LOCALDEC void vmptone_dtor(struct tSMVMPTxToneSetId_struct *vtp);
LOCALDEC struct tSMVMPTxToneSetId_struct *init_vmptone(struct module *mod);

#endif
