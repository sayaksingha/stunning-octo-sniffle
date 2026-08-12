/* TiNGsmdc_v34.h - Data communications functions for V.34 */

#ifndef INCLUDED_TINGSMDC_V34_H
#define INCLUDED_TINGSMDC_V34_H

#include "TiNGsmdc.h"

LOCALDEC struct protable protable_v34rx;
LOCALDEC struct protable protable_v34tx;
LOCALDEC int real_smdc_rx_config_v34(struct smdc_rx_config_v34_parms *pp);
LOCALDEC int real_smdc_tx_config_v34(struct smdc_tx_config_v34_parms *pp);
LOCALDEC int real_smdc_line_control_v34(struct smdc_line_control_v34_parms *pp);
LOCALDEC int real_smdc_line_status_v34(struct smdc_line_status_v34_parms *pp);

#endif
