#ifndef INCLUDED_TINGDETECT_H
#define INCLUDED_TINGDETECT_H

#include "smbesp.h"
#include "TiNGo_chan.h"
#include "../TiNG.h"

LOCALDEC int real_sm_add_input_freq_coeffs(struct sm_input_freq_coeffs_parms *pp);
LOCALDEC int real_sm_add_input_tone_set(struct sm_input_tone_set_parms *pp);
LOCALDEC int real_sm_add_input_cptone(struct sm_input_cptone_parms *pp);
LOCALDEC int real_sm_reset_input_cptones(struct sm_reset_input_cptones_parms *pp);
LOCALDEC int real_sm_listen_for(struct sm_listen_for_parms *pp);
LOCALDEC int real_sm_get_recognised(struct sm_recognised_parms *pp);
LOCALDEC int real_sm_discard_recognised(tSMChannelId chan);
LOCALDEC int real_sm_adjust_input_tone_set(struct sm_adjust_tone_set_parms *pp);
	// start a form of detection
	// must already hold channel lock
LOCALDEC int start_listen(struct sm_listen_for_parms *pp, enum handles h, int isrec);
	// decode a message from any running detection
LOCALDEC int handle_detected(int *done, struct sm_recognised_parms *pp, unsigned code, struct ting_message *msg, int isrec);
	// perform sm_get_recognised on specific channel
	// must already hold channel lock
LOCALDEC int unsafe_sm_get_recognised(struct sm_recognised_parms *pp);

LOCALDEC int notify_detpar(tSMChannelId chan, CARDCONN *cx, int toneset, int isrec, int hyst);

#define create_detpar aculab_TiNGcore_create_detpar
LOCALDEC int create_detpar(struct module *mod, struct dettoneset *sp);

LOCALDEC int real_sm_ans_listen_for(struct sm_ans_listen_for_parms *pp);
LOCALDEC int real_sm_beep_listen_for(struct sm_beep_listen_for_parms *pp);
LOCALDEC int real_sm_onhook_listen_for(struct sm_onhook_listen_for_parms *pp);

#endif
