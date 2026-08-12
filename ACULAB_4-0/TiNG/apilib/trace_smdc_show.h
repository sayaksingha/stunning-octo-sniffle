/* trace_smdc_show.h - helper functions for data communications API tracing */

#ifndef INCLUDED_TRACE_SMDC_SHOW_H
#define INCLUDED_TRACE_SMDC_SHOW_H

#include "smdc.h"

LOCALDEC void show_rxtx_data_in(struct smdc_data_parms *pp);
LOCALDEC void show_enc_in(struct smdc_encoding_parms *pp);
LOCALDEC void show_config_enc_in(struct smdc_channel_config_parms *pp);
LOCALDEC void show_prefdat_sufdat_in(struct smdc_line_control_parms *pp);

#endif
