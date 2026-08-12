/* TiNGsmdc.h - general data communications */

#ifndef INCLUDED_TINGSMDC_H
#define INCLUDED_TINGSMDC_H

#include "smdc.h"
#include "smdc_encode.h"
#include "../cmndev.h"

struct protable {
		// the value visible through the API
	int protocol;
		// configure this protocol on the channel
	int (*config)(struct smdc_channel_config_parms *pp);
		// set encoding for this protocol on the channel
	int (*setenc)(tSMChannelId chan, ENCTBL *enctbl, unsigned cflen, void *cfdata);
		// called to handle all line control commands on tx
	int (*ltcontrol)(struct smdc_line_control_parms *pp);
		// called to supplement the standard report on tx
	int (*ltstatus)(struct smdc_line_status_parms *pp);
		// called to handle all line control commands on rx
	int (*lrcontrol)(struct smdc_line_control_parms *pp);
		// called to supplement the standard report on rx
	int (*lrstatus)(struct smdc_line_status_parms *pp);
		// called to handle unknown commands
	int (*tcontrol)(struct smdc_tx_control_parms *pp);
		// called to supplement the standard report
	int (*tstatus)(struct smdc_tx_status_parms *pp, U32 *msg);
		// called to handle unknown commands
	int (*rcontrol)(struct smdc_rx_control_parms *pp);
		// called to supplement the standard report
	int (*rstatus)(struct smdc_rx_status_parms *pp, U32 *msg);
		// called after standard processing of each message
		// returns:
		//	0 = handled message
		//	>0 = no handler
		//	<0 = error
		// as usual, [0] is out/tx, [1] is in/rx
	int (*msghand[2])(tSMChannelId chan, U32 *msg);
		// called instead of standard stop action
	int (*stop)(struct smdc_stop_parms *pp);
};

LOCALDEC int real_smdc_channel_config(struct smdc_channel_config_parms *pp);
LOCALDEC int real_smdc_tx_encoding(struct smdc_encoding_parms *pp);
LOCALDEC int real_smdc_rx_encoding(struct smdc_encoding_parms *pp);
LOCALDEC int real_smdc_line_control(struct smdc_line_control_parms *pp);
LOCALDEC int real_smdc_line_status(struct smdc_line_status_parms *pp);
LOCALDEC int real_smdc_tx_control(struct smdc_tx_control_parms *pp);
LOCALDEC int real_smdc_tx_status(struct smdc_tx_status_parms *pp);
LOCALDEC int real_smdc_rx_control(struct smdc_rx_control_parms *pp);
LOCALDEC int real_smdc_rx_status(struct smdc_rx_status_parms *pp);
LOCALDEC int real_smdc_stop(struct smdc_stop_parms *pp);
LOCALDEC int real_smdc_tx_stop(struct smdc_stop_parms *pp);
LOCALDEC int real_smdc_rx_stop(struct smdc_stop_parms *pp);
LOCALDEC int real_smdc_tx_data(struct smdc_data_parms *pp);
LOCALDEC int real_smdc_rx_data(struct smdc_data_parms *pp);

LOCALDEC int setup_coll_disp(tSMChannelId cp, ENCTBL *enctbl, int istx, int isrx);
#endif
