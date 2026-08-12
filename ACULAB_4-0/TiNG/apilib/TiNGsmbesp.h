
#ifndef INCLUDED_TINGSMBESP_H
#define INCLUDED_TINGSMBESP_H

#include "smbesp.h"

LOCALDEC int real_sm_replay_adjust(struct sm_replay_adjust_parms *pp);
LOCALDEC int real_sm_replay_start(struct sm_replay_parms *pp);
LOCALDEC int real_sm_put_replay_data(struct sm_ts_data_parms *pp);
LOCALDEC int real_sm_put_last_replay_data(struct sm_ts_data_parms *pp);
	// must already hold channel and anychan locks
LOCALDEC int unsafe_sm_replay_status(struct sm_replay_status_parms *pp);
LOCALDEC int real_sm_replay_status(struct sm_replay_status_parms *pp);
LOCALDEC int real_sm_replay_abort(struct sm_replay_abort_parms *pp);
LOCALDEC int real_sm_record_start(struct sm_record_parms *pp);
LOCALDEC int real_sm_get_recorded_data(struct sm_ts_data_parms *pp);
	// must already hold channel and anychan locks
LOCALDEC int unsafe_sm_record_status(struct sm_record_status_parms *pp);
LOCALDEC int real_sm_record_status(struct sm_record_status_parms *pp);
LOCALDEC int real_sm_record_abort(struct sm_record_abort_parms *pp);
LOCALDEC int real_sm_record_agc_adjust(struct sm_record_agc_adjust_parms *pp);
LOCALDEC int real_sm_record_agc_adjust_settings(struct sm_record_agc_adjust_settings_parms *pp);
LOCALDEC int real_sm_condition_input(struct sm_condition_input_parms *pp);
LOCALDEC int real_sm_condition_adjust(struct sm_condition_adjust_parms *pp);
LOCALDEC int real_sm_condition_adjust_span(struct sm_condition_adjust_span_parms *pp);
LOCALDEC int real_sm_condition_reinit(tSMChannelId channel);
LOCALDEC int real_sm_catsig_listen_for(struct sm_catsig_listen_for_parms *pp);
LOCALDEC int real_sm_adjust_catsig_module_params(struct sm_adjust_catsig_module_parms *pp);
LOCALDEC int real_sm_channel_set_input_threshold(struct sm_channel_set_input_threshold_parms *pp);
LOCALDEC int real_sm_channel_set_output_threshold(struct sm_channel_set_output_threshold_parms *pp);

#endif
