/* trace_smvideo_show.h - helper functions for video API tracing */

#ifndef INCLUDED_TRACE_SMVIDEO_SHOW_H
#define INCLUDED_TRACE_SMVIDEO_SHOW_H

#include "smvideo.h"

LOCALDEC void show_vidrx_data_in(struct sm_avfrec_get_data_parms *pp);
LOCALDEC void show_vidtx_data_in(struct sm_avfplay_put_data_parms *pp);

#endif
