/* watchl1.c - watch layer 1 on port */
#include "acu_type.h"
#include "cl_lib.h"

#include <stdio.h>
#include <string.h>
#include "../../libutil/vseprintf.h"

#include "../Testlib/watchl1.h"

#ifdef TiNGTYPE_LINUX
#include <unistd.h>
#endif

#ifdef TiNGTYPE_QNX
#include <unistd.h>
#endif


int watchl1(LAYER1 *l1)
{
	 // last status which was reported - so we know when it changes
 struct l1_xstats prevxs;
 INIT_ACU_CL_STRUCT(&prevxs);
 while (!l1->stop) {
	struct l1_xstats xs;
	char buff[256];
	char *const ep = buff + sizeof(buff);
	char *sp = buff;
	int err;
	INIT_ACU_CL_STRUCT(&xs);
 	xs.net = l1->net;
 	err = call_l1_stats(&xs);
 	if (err) {
		fprintf(stderr, "%s: call_l1_stats returned %s\n", l1->dbgname, error_2_string(err));
		return 1;
	}
		// now tediously check each status value
	if (xs.getset.linestat != prevxs.getset.linestat)
		sp = seprintf(sp, ep, " linestat=0x%04x", xs.getset.linestat);
	if (xs.getset.bipvios != prevxs.getset.bipvios)
		sp = seprintf(sp, ep, " bipvios=0x%04x", xs.getset.bipvios);
	if (xs.getset.faserrs != prevxs.getset.faserrs)
		sp = seprintf(sp, ep, " faserrs=0x%04x", xs.getset.faserrs);
	if (xs.getset.sliperrs != prevxs.getset.sliperrs)
		sp = seprintf(sp, ep, " sliperrs=0x%04x", xs.getset.sliperrs);
	if (xs.get.majorrev != prevxs.get.majorrev
		|| xs.get.minorrev != prevxs.get.minorrev)
		sp = seprintf(sp, ep, " v%d.%d", xs.get.majorrev, xs.get.minorrev);
	if (xs.get.nos != prevxs.get.nos)
		sp = seprintf(sp, ep, " %cnos", "+-"[!xs.get.nos]);
	if (xs.get.ais != prevxs.get.ais)
		sp = seprintf(sp, ep, " %cais", "+-"[!xs.get.ais]);
	if (xs.get.los != prevxs.get.los)
		sp = seprintf(sp, ep, " %clos", "+-"[!xs.get.los]);
	if (xs.get.rra != prevxs.get.rra)
		sp = seprintf(sp, ep, " %crra", "+-"[!xs.get.rra]);
	if (xs.get.tra != prevxs.get.tra)
		sp = seprintf(sp, ep, " %ctra", "+-"[!xs.get.tra]);
	if (xs.get.rma != prevxs.get.rma)
		sp = seprintf(sp, ep, " %crma", "+-"[!xs.get.rma]);
	if (xs.get.tma != prevxs.get.tma)
		sp = seprintf(sp, ep, " %ctma", "+-"[!xs.get.tma]);
	if (xs.get.usr != prevxs.get.usr)
		sp = seprintf(sp, ep, " usr=%02x", xs.get.usr);
	if (memcmp(xs.get.buildstr, prevxs.get.buildstr, sizeof(xs.get.buildstr)))
		sp = seprintf(sp, ep, " buildstr='%s'", xs.get.buildstr);
	if (memcmp(xs.get.manstr, prevxs.get.manstr, sizeof(xs.get.manstr)))
		sp = seprintf(sp, ep, " manstr='%s'", xs.get.manstr);
	if (memcmp(xs.get.sigstr, prevxs.get.sigstr, sizeof(xs.get.sigstr)))
		sp = seprintf(sp, ep, " sigstr='%s'", xs.get.sigstr);
		// if anything changed, report this
	if (sp > buff) printf("%s: %s\n", l1->dbgname, buff);
	prevxs = xs;
		// wait a while - ideally this should use an API call
		// which waits until a status change occurs, but there's
		// no such call in the older versions of the API
#ifdef TiNGTYPE_WINNT
	Sleep(1000);
#else
	sleep(1);
#endif
 }
 return 0;
}
