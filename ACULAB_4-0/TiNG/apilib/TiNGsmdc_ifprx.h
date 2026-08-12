#ifndef INCLUDED_TINGSMDC_IFPRX_H
#define INCLUDED_TINGSMDC_IFPRX_H

#include "smdc.h"

LOCALDEC int real_smdc_rx_config_t38(struct smdc_rx_config_t38_parms *pp);
LOCALDEC int real_smdc_rx_status_t38(struct smdc_rx_status_t38_parms *statusp);

#endif
