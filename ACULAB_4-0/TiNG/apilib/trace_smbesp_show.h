/* trace_smbesp_show.h - helper functions for speech API tracing */

#ifndef INCLUDED_TRACE_SMBESP_SHOW_H
#define INCLUDED_TRACE_SMBESP_SHOW_H

#include "smbesp.h"

LOCALDEC void show_playrec_data_in(struct sm_ts_data_parms *pp);
LOCALDEC void show_audio_data_in(struct sm_audio_data_parms *pp);
LOCALDEC void show_play_cptone_in(struct sm_play_cptone_parms *pp);
LOCALDEC void show_ectest_filt_in(struct sm_condition_input_parms *pp);
LOCALDEC void show_input_cptone_in(struct sm_input_cptone_parms *pp);
LOCALDEC void show_vocab_item_list_vocab_recog_list_vocab_params_in(struct sm_asr_listen_for_parms *pp);
LOCALDEC void show_conf_active_ids_out(struct sm_conf_prim_status_parms *statusp);
LOCALDEF void show_pluginconfigdata_in(struct sm_catsig_listen_for_parms *pp);

#endif
