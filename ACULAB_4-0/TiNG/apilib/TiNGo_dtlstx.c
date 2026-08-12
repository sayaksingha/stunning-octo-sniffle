/* TiNGo_dtlstx.c - TiNG dtlstx objects */

#include "TiNGo_dtlstx.h"
#include "TiNGcommon.h"
#include <stdlib.h>
#include <string.h>

LOCALDEF void dtlstx_dtor(struct tSMDTLStxId_struct *vtp)
{
	vtp->groove.dtor(&vtp->groove);
	mutx_dtor(&vtp->pm.mx);
	memset(vtp, 0xa5, sizeof(*vtp));		// for safety
	free(vtp);
}

LOCALDEF struct tSMDTLStxId_struct *init_dtlstx(struct module *mod)
{
	struct tSMDTLStxId_struct *vtp = alloc(0,sizeof(*vtp));
	if (!vtp) return 0;
	memset(vtp, 0, sizeof(*vtp));
	vtp->mod = mod;
	if (SMGroove_module(&vtp->groove, mod)) {
		free(vtp);
		return 0;
	}
	vtp->min_bits = 2048 * 8;
	mutx_init(&vtp->pm.mx);
	make_object(vtp);
	return vtp;
}

