/* trace_gen_show.c - helper functions for generic API tracing */

#include "trace_gen_show.h"
#include "trace.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

LOCALDEF void show_rev_out(struct sm_card_rev_parms *pp)
{
 unsigned u;
 for (u=0; u < arlen(pp->rev); u++) olog(", r[%d]=0x%x", u, pp->rev[u]);
 olog("\n");
}
