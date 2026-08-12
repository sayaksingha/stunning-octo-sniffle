/* TiNGsmdc_speech.c - Data communications functions applicable to speech */

#include "TiNGsmdc_speech.h"
#include "TiNGcommon.h"

STATIC int speech_lcontrol(struct smdc_line_control_parms *pp)
{
 switch (pp->cmd) {
 case kSMDCLineCtlCmdInitiatorConnect:
 case kSMDCLineCtlCmdResponderConnect:
	return 0;	// always connected
 default:
	return err(ERR_SM_BAD_PARAMETER);
 }
}

STATIC int speech_lstatus(struct smdc_line_status_parms *pp)
{
 (void) pp;
 return 0;
}

STATIC int speech_tstatus(struct smdc_tx_status_parms *pp, U32 *msg)
{
 (void) pp;
 (void) msg;
 return 0;
}

STATIC int speech_tcontrol(struct smdc_tx_control_parms *pp)
{
 (void) pp;
 return 1;
}

STATIC int speech_rstatus(struct smdc_rx_status_parms *pp, U32 *msg)
{
 (void) pp;
 (void) msg;
 return 0;
}

STATIC int speech_rcontrol(struct smdc_rx_control_parms *pp)
{
 (void) pp;
 return err(ERR_SM_BAD_PARAMETER);
}

LOCALDEF struct protable protable_speech = {
	0,
	0,
	0,
	speech_lcontrol,
	speech_lstatus,
	speech_lcontrol,
	speech_lstatus,
	speech_tcontrol,
	speech_tstatus,
	speech_rcontrol,
	speech_rstatus,
	{ 0, 0, },
	0,
};
