/* TiNGo_vmpcsrc.c - TiNG vmpcsrc objects */

#include "TiNGo_vmpcsrc.h"
#include "TiNGcommon.h"
#include <stdlib.h>
#include <string.h>
#include "px_prostype.h"

LOCALDEF void csrc_list_dtor(struct tSMVMPTxCSRCListId_struct *vcp)
{
 vcp->groove.dtor(&vcp->groove);
 mutx_dtor(&vcp->pm.mx);
 memset(vcp, 0xa5, sizeof(*vcp));		// for safety
 free(vcp);
}

LOCALDEF struct tSMVMPTxCSRCListId_struct *init_csrc_list(struct module *mod)
{
 struct tSMVMPTxCSRCListId_struct *vcp = alloc(0, sizeof(*vcp));
 if (!vcp) return 0;
 memset(vcp, 0, sizeof(*vcp));
 if (SMGroove_module(&vcp->groove, mod)) {
        free(vcp);
        return 0;
 }
 mutx_init(&vcp->pm.mx);
 make_object(vcp);
 return vcp;
}
