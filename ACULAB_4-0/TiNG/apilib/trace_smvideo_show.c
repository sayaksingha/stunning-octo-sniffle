/* trace_smvideo_show.c - helper functions for video API tracing */

#include "trace_smvideo_show.h"
#include "trace.h"

LOCALDEF void show_vidrx_data_in(struct sm_avfrec_get_data_parms *pp)
{
 olog(", data=%p)", pp->data);
}

LOCALDEF void show_vidtx_data_in(struct sm_avfplay_put_data_parms *pp)
{
 olog(", data=%p)", pp->data);
}
