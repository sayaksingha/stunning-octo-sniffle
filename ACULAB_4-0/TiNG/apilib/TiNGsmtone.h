#ifndef INCLUDED_TINGSMTONE_H
#define INCLUDED_TINGSMTONE_H

#include "smbesp.h"

LOCALDEC int real_sm_add_output_freq(struct sm_output_freq_parms *pp);
LOCALDEC int real_sm_add_output_tone(struct sm_output_tone_parms *pp);
LOCALDEC int real_sm_play_tone(struct sm_play_tone_parms *pp);
	// must already hold channel and anychan mutex
LOCALDEC int unsafe_sm_play_tone_status(struct sm_play_tone_status_parms *pp);
LOCALDEC int real_sm_play_tone_status(struct sm_play_tone_status_parms *pp);
LOCALDEC int real_sm_play_tone_abort(tSMChannelId cp);
LOCALDEC int real_sm_play_cptone_abort(tSMChannelId cp);
LOCALDEC int real_sm_play_cptone(struct sm_play_cptone_parms *pp);
	// must already hold channel and anychan mutex
LOCALDEC int unsafe_sm_play_cptone_status(struct sm_play_cptone_status_parms *pp);
LOCALDEC int real_sm_play_cptone_status(struct sm_play_cptone_status_parms *pp);
LOCALDEC int real_sm_play_digits(struct sm_play_digits_parms *pp);
	// must already hold channel and anychan mutex
LOCALDEC int unsafe_sm_play_digits_status(struct sm_play_digits_status_parms *pp);
LOCALDEC int real_sm_play_digits_status(struct sm_play_digits_status_parms *pp);

LOCALDEC int real_sm_play_tone_list(struct sm_play_tone_list_parms *pp);
LOCALDEC int real_sm_play_tone_list_status(struct sm_play_tone_list_status_parms *pp);
LOCALDEC int real_sm_play_tone_list_abort(tSMChannelId cp);
LOCALDEC int real_sm_play_tone_list_phase_reverse(struct sm_play_tone_list_phase_reverse_parms *pp);

#endif
