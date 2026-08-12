#ifndef TINGOVMPCSRCHINCLUDED
#define TINGOVMPCSRCHINCLUDED

#include "paranoia.h"
#include "TiNGo_mod.h"
#include "smgroove.h"

struct tSMVMPTxCSRCListId_struct {
	PARAMUTX pm;
	tSMGroove groove;
};

LOCALDEC void csrc_list_dtor(struct tSMVMPTxCSRCListId_struct *vtp);
LOCALDEC struct tSMVMPTxCSRCListId_struct *init_csrc_list(struct module *mod);

#endif
