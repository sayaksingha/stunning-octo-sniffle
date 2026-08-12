/* TiNGo_coll.c - TiNG collector objects */

#include "TiNGo_coll.h"
#include "TiNGcommon.h"
#include <stdlib.h>
#include <string.h>

LOCALDEF void collector_dtor(struct tSMCollectorId_struct *ttp)
{
 ttp->groove.dtor(&ttp->groove);
 mutx_dtor(&ttp->pm.mx);
 memset(ttp, 0xa5, sizeof(*ttp));		// for safety
 free(ttp);
}

LOCALDEF struct tSMCollectorId_struct *init_collector(struct module *mod)
{
 struct tSMCollectorId_struct *ttp = alloc(0, sizeof(*ttp));
 if (!ttp) return 0;
 memset(ttp, 0, sizeof(*ttp));
 ttp->mod = mod;
 if (SMGroove_module(&ttp->groove, mod)) {
 	free(ttp);
 	return 0;
 }
 mutx_init(&ttp->pm.mx);
 make_object(ttp);
 return ttp;
}
