#ifndef INCLUDED_TINGSMDC_IFPTX_H
#define INCLUDED_TINGSMDC_IFPTX_H

#include "smdc.h"

LOCALDEC int real_smdc_tx_config_t38(struct smdc_tx_config_t38_parms *pp);
LOCALDEC int real_smdc_tx_tone_indicator_t38(struct smdc_tx_tone_indicator_t38_parms *tonep);
LOCALDEF int real_smdc_tx_v34_line_control_t38(struct smdc_tx_v34_line_control_t38_parms *lcp);
LOCALDEF int real_smdc_tx_v34_indicator_t38(struct smdc_tx_v34_indicator_t38_parms *v34p);

#endif
