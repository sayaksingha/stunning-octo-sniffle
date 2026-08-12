#include "TiNGo_sntp.h"
#include "TiNGcommon.h"
#include <stdlib.h>
#include <string.h>

LOCALDEF void sntp_dtor(struct tSMSNTPId_struct *vtp)
{
 vtp->groove.dtor(&vtp->groove);
 mutx_dtor(&vtp->pm.mx);
 memset(vtp, 0xa5, sizeof(*vtp));		// for safety
 free(vtp);
}

LOCALDEF struct tSMSNTPId_struct *init_sntp(struct module *mod)
{
 struct tSMSNTPId_struct *vtp = alloc(0,sizeof(*vtp));
 if (!vtp) return 0;
 memset(vtp, 0, sizeof(*vtp));
 vtp->mod = mod;
 if (!SMGroove_module(&vtp->groove, mod)) {
	mutx_init(&vtp->pm.mx);
	make_object(vtp);
	return vtp;
 }
 free(vtp);
 return 0;
}

