/* prospapi.i - Prosody speech processing trace code */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

ACUAPI int ACUTiNG_WINAPI sm_add_input_cptone(struct sm_input_cptone_parms *cptonep)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_add_input_cptone(module=" TiNG_PRINTF_MODULE ", state_count=%d, id=%d", timestamp_now(), cptonep->module, cptonep->state_count, cptonep->id);
	show_input_cptone_in(cptonep);
 }
 sts = real_sm_add_input_cptone(cptonep);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_add_input_freq_coeffs(struct sm_input_freq_coeffs_parms *freqcoeffp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_add_input_freq_coeffs(module=" TiNG_PRINTF_MODULE ", upper_limit=%g, lower_limit=%g)", timestamp_now(), freqcoeffp->module, freqcoeffp->upper_limit, freqcoeffp->lower_limit);
 sts = real_sm_add_input_freq_coeffs(freqcoeffp);
 if (TiNGtrace > 1) olog(" == %d id=%d\n", sts, freqcoeffp->id);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_add_input_tone_set(struct sm_input_tone_set_parms *tonesetp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_add_input_tone_set(module=" TiNG_PRINTF_MODULE ", band1_first_freq_coeffs_id=%d, band1_freq_count=%d, band2_first_freq_coeffs_id=%d, band2_freq_count=%d, req_third_peak=%g, req_signal_to_noise_ratio=%g, req_minimum_power=%g, req_twist_for_dual_tone=%g)", timestamp_now(), tonesetp->module, tonesetp->band1_first_freq_coeffs_id, tonesetp->band1_freq_count, tonesetp->band2_first_freq_coeffs_id, tonesetp->band2_freq_count, tonesetp->req_third_peak, tonesetp->req_signal_to_noise_ratio, tonesetp->req_minimum_power, tonesetp->req_twist_for_dual_tone);
 sts = real_sm_add_input_tone_set(tonesetp);
 if (TiNGtrace > 1) olog(" == %d id=%d\n", sts, tonesetp->id);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_add_output_freq(struct sm_output_freq_parms *freqp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_add_output_freq(module=" TiNG_PRINTF_MODULE ", freq=%g, amplitude=%g)", timestamp_now(), freqp->module, freqp->freq, freqp->amplitude);
 sts = real_sm_add_output_freq(freqp);
 if (TiNGtrace > 1) olog(" == %d id=%d\n", sts, freqp->id);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_add_output_tone(struct sm_output_tone_parms *tonep)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_add_output_tone(module=" TiNG_PRINTF_MODULE ", component1_id=%d, component2_id=%d)", timestamp_now(), tonep->module, tonep->component1_id, tonep->component2_id);
 sts = real_sm_add_output_tone(tonep);
 if (TiNGtrace > 1) olog(" == %d id=%d\n", sts, tonep->id);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_adjust_catsig_module_params(struct sm_adjust_catsig_module_parms *catsigp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_adjust_catsig_module_params(module=" TiNG_PRINTF_MODULE ", catsig_alg_id=%d, parameter_id=%d, fp_value=%g, int_value=%d)", timestamp_now(), catsigp->module, catsigp->catsig_alg_id, catsigp->parameter_id, catsigp->parameter_value.fp_value, catsigp->parameter_value.int_value);
 sts = real_sm_adjust_catsig_module_params(catsigp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_adjust_input_tone_set(struct sm_adjust_tone_set_parms *tonesetp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_adjust_input_tone_set(module=" TiNG_PRINTF_MODULE ", tone_set_id=%d, parameter_id=%d, fp_value=%g, int_value=%d)", timestamp_now(), tonesetp->module, tonesetp->tone_set_id, tonesetp->parameter_id, tonesetp->parameter_value.fp_value, tonesetp->parameter_value.int_value);
 sts = real_sm_adjust_input_tone_set(tonesetp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_ans_listen_for(struct sm_ans_listen_for_parms *listenp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_ans_listen_for(channel=%p, detection_mode=%d)", timestamp_now(), listenp->channel, listenp->detection_mode);
 sts = real_sm_ans_listen_for(listenp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_beep_listen_for(struct sm_beep_listen_for_parms *listenp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_beep_listen_for(channel=%p, min_duration=%d, upper_limit=%g, lower_limit=%g)", timestamp_now(), listenp->channel, listenp->min_duration, listenp->upper_limit, listenp->lower_limit);
 sts = real_sm_beep_listen_for(listenp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_catsig_listen_for(struct sm_catsig_listen_for_parms *listenp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_catsig_listen_for(channel=%p, catsig_alg_id=%d, catsig_hint_param=%d, catsig_timeout=%d, catsig_config_length=%d, abort_catsig_alg=%d", timestamp_now(), listenp->channel, listenp->catsig_alg_id, listenp->catsig_hint_param, listenp->catsig_timeout, listenp->catsig_config_length, listenp->abort_catsig_alg);
	show_pluginconfigdata_in(listenp);
 }
 sts = real_sm_catsig_listen_for(listenp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_catsig_signalling_hint(struct sm_catsig_signalling_hint_parms *hintp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_catsig_signalling_hint(channel=%p, catsig_hint_param=%d)", timestamp_now(), hintp->channel, hintp->catsig_hint_param);
 sts = real_sm_catsig_signalling_hint(hintp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_channel_set_input_threshold(struct sm_channel_set_input_threshold_parms *thp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_channel_set_input_threshold(channel=%p, minimum_bits=%d)", timestamp_now(), thp->channel, thp->minimum_bits);
 sts = real_sm_channel_set_input_threshold(thp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_channel_set_output_threshold(struct sm_channel_set_output_threshold_parms *thp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_channel_set_output_threshold(channel=%p, minimum_bits=%d)", timestamp_now(), thp->channel, thp->minimum_bits);
 sts = real_sm_channel_set_output_threshold(thp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_condition_adjust(struct sm_condition_adjust_parms *condp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_condition_adjust(channel=%p, adjust_type=%d, adjust_value=%d)", timestamp_now(), condp->channel, condp->adjust_type, condp->adjust_value);
 sts = real_sm_condition_adjust(condp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_condition_adjust_span(struct sm_condition_adjust_span_parms *condp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_condition_adjust_span(channel=%p, span=%d)", timestamp_now(), condp->channel, condp->span);
 sts = real_sm_condition_adjust_span(condp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_condition_input(struct sm_condition_input_parms *condp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_condition_input(channel=%p, reference=%p, reference_type=%d, conditioning_type=%d, conditioning_param=%d, alt_data_dest=%p, alt_dest_type=%d, ectest_type=%d, ectest_gain=%g, ectest_delay=%d, ectest_flen=%u", timestamp_now(), condp->channel, condp->reference, condp->reference_type, condp->conditioning_type, condp->conditioning_param, condp->alt_data_dest, condp->alt_dest_type, condp->ectest_type, condp->ectest_gain, condp->ectest_delay, condp->ectest_flen);
	show_ectest_filt_in(condp);
 }
 sts = real_sm_condition_input(condp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_condition_reinit(tSMChannelId channel)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_condition_reinit(channel=%p)", timestamp_now(), channel);
 sts = real_sm_condition_reinit(channel);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_abort(tSMChannelId channel)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_conf_prim_abort(channel=%p)", timestamp_now(), channel);
 sts = real_sm_conf_prim_abort(channel);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_add(struct sm_conf_prim_add_parms *confp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_conf_prim_add(channel=%p, participant=%p, factor=%g)", timestamp_now(), confp->channel, confp->participant, confp->factor);
 sts = real_sm_conf_prim_add(confp);
 if (TiNGtrace > 1) olog(" == %d id=%d\n", sts, confp->id);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_adj_input(struct sm_conf_prim_adj_input_parms *confp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_conf_prim_adj_input(channel=%p, volume=%d, agc=%d)", timestamp_now(), confp->channel, confp->volume, confp->agc);
 sts = real_sm_conf_prim_adj_input(confp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_adj_input_settings(struct sm_conf_prim_adj_input_settings_parms *confp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_conf_prim_adj_input_settings(channel=%p, max_level_decay=%g, target_level=%g)", timestamp_now(), confp->channel, confp->max_level_decay, confp->target_level);
 sts = real_sm_conf_prim_adj_input_settings(confp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_adj_output(struct sm_conf_prim_adj_output_parms *confp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_conf_prim_adj_output(channel=%p, volume=%d, agc=%d)", timestamp_now(), confp->channel, confp->volume, confp->agc);
 sts = real_sm_conf_prim_adj_output(confp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_adj_tracking(struct sm_conf_prim_adj_tracking_parms *trackp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_conf_prim_adj_tracking(channel=%p, min_noise_level=%g, speech_thresh=%g)", timestamp_now(), trackp->channel, trackp->min_noise_level, trackp->speech_thresh);
 sts = real_sm_conf_prim_adj_tracking(trackp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_attach(struct sm_conf_prim_attach_parms *confp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_conf_prim_attach(channel=%p, conf_type=%d)", timestamp_now(), confp->channel, confp->conf_type);
 sts = real_sm_conf_prim_attach(confp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_clone(struct sm_conf_prim_clone_parms *clonep)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_conf_prim_clone(channel=%p, model=%p)", timestamp_now(), clonep->channel, clonep->model);
 sts = real_sm_conf_prim_clone(clonep);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_config_activity_reporting(struct sm_conf_prim_config_activity_reporting_parms *activityp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_conf_prim_config_activity_reporting(channel=%p, delay=%u, sensitivity=%u)", timestamp_now(), activityp->channel, activityp->delay, activityp->sensitivity);
 sts = real_sm_conf_prim_config_activity_reporting(activityp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_detach(struct sm_conf_prim_detach_parms *confp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_conf_prim_detach(channel=%p, conf_type=%d)", timestamp_now(), confp->channel, confp->conf_type);
 sts = real_sm_conf_prim_detach(confp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_info(struct sm_conf_prim_info_parms *confp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_conf_prim_info(channel=%p)", timestamp_now(), confp->channel);
 sts = real_sm_conf_prim_info(confp);
 if (TiNGtrace > 1) olog(" == %d participant_count=%d, speakers='%s'\n", sts, confp->participant_count, confp->speakers);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_leave(struct sm_conf_prim_leave_parms *confp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_conf_prim_leave(channel=%p, id=%d)", timestamp_now(), confp->channel, confp->id);
 sts = real_sm_conf_prim_leave(confp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_start(struct sm_conf_prim_start_parms *confp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_conf_prim_start(channel=%p, volume=%d, agc=%d, conf_type=%d)", timestamp_now(), confp->channel, confp->volume, confp->agc, confp->conf_type);
 sts = real_sm_conf_prim_start(confp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_status(struct sm_conf_prim_status_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_conf_prim_status(channel=%p)", timestamp_now(), statusp->channel);
 sts = real_sm_conf_prim_status(statusp);
 if (TiNGtrace > 1) {
	olog(" == %d status=%d", sts, statusp->status);
	show_conf_active_ids_out(statusp);
 }
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_stop(struct sm_conf_prim_stop_parms *stopp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_conf_prim_stop(channel=%p, no_wait=%u)", timestamp_now(), stopp->channel, stopp->no_wait);
 sts = real_sm_conf_prim_stop(stopp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_discard_recognised(tSMChannelId channel)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_discard_recognised(channel=%p)", timestamp_now(), channel);
 sts = real_sm_discard_recognised(channel);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_get_recognised(struct sm_recognised_parms *recogp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_get_recognised(channel=%p)", timestamp_now(), recogp->channel);
 sts = real_sm_get_recognised(recogp);
 if (TiNGtrace > 1) olog(" == %d channel=%p, type=%d, param0=%d, param1=%d\n", sts, recogp->channel, recogp->type, recogp->param0, recogp->param1);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_get_recorded_data(struct sm_ts_data_parms *datap)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_get_recorded_data(channel=%p", timestamp_now(), datap->channel);
	show_playrec_data_in(datap);
 }
 sts = real_sm_get_recorded_data(datap);
 if (TiNGtrace > 1) olog(" == %d length=%d\n", sts, datap->length);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_listen_for(struct sm_listen_for_parms *listenp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_listen_for(channel=%p, tone_detection_mode=%d, active_tone_set_id=%d, map_tones_to_digits=%d, enable_cptone_recognition=%d, enable_grunt_detection=%d, grunt_latency=%d, min_noise_level=%g, grunt_threshold=%g, grunt_holdoff=%u)", timestamp_now(), listenp->channel, listenp->tone_detection_mode, listenp->active_tone_set_id, listenp->map_tones_to_digits, listenp->enable_cptone_recognition, listenp->enable_grunt_detection, listenp->grunt_latency, listenp->min_noise_level, listenp->grunt_threshold, listenp->grunt_holdoff);
 sts = real_sm_listen_for(listenp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_onhook_listen_for(struct sm_onhook_listen_for_parms *listenp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_onhook_listen_for(channel=%p, enable=%d, pre_pulse_max_power=%g, pulse_min_power=%g, post_pulse_floor=%g, latitude=%g, count_ratio=%g, total_ratio=%g, max_duration=%d)", timestamp_now(), listenp->channel, listenp->enable, listenp->pre_pulse_max_power, listenp->pulse_min_power, listenp->post_pulse_floor, listenp->latitude, listenp->count_ratio, listenp->total_ratio, listenp->max_duration);
 sts = real_sm_onhook_listen_for(listenp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_play_cptone(struct sm_play_cptone_parms *cptonep)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_play_cptone(channel=%p, duration=%u, wait_for_completion=%d, type=%d, tone_count=%d", timestamp_now(), cptonep->channel, cptonep->duration, cptonep->wait_for_completion, cptonep->type, cptonep->tone_count);
	show_play_cptone_in(cptonep);
 }
 sts = real_sm_play_cptone(cptonep);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_play_cptone_abort(tSMChannelId channel)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_play_cptone_abort(channel=%p)", timestamp_now(), channel);
 sts = real_sm_play_cptone_abort(channel);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_play_cptone_status(struct sm_play_cptone_status_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_play_cptone_status(channel=%p)", timestamp_now(), statusp->channel);
 sts = real_sm_play_cptone_status(statusp);
 if (TiNGtrace > 1) olog(" == %d channel=%p, status=%d\n", sts, statusp->channel, statusp->status);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_play_digits(struct sm_play_digits_parms *digitsp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_play_digits(channel=%p, wait_for_completion=%d, type=%d, qualifier=%d, digit_string='%s', inter_digit_delay=%d, digit_duration=%d)", timestamp_now(), digitsp->channel, digitsp->wait_for_completion, digitsp->digits.type, digitsp->digits.qualifier, digitsp->digits.digit_string, digitsp->digits.inter_digit_delay, digitsp->digits.digit_duration);
 sts = real_sm_play_digits(digitsp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_play_digits_status(struct sm_play_digits_status_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_play_digits_status(channel=%p)", timestamp_now(), statusp->channel);
 sts = real_sm_play_digits_status(statusp);
 if (TiNGtrace > 1) olog(" == %d channel=%p, status=%d\n", sts, statusp->channel, statusp->status);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_play_tone(struct sm_play_tone_parms *tonep)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_play_tone(channel=%p, duration=%u, wait_for_completion=%d, tone_id=%d)", timestamp_now(), tonep->channel, tonep->duration, tonep->wait_for_completion, tonep->tone_id);
 sts = real_sm_play_tone(tonep);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_play_tone_abort(tSMChannelId channel)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_play_tone_abort(channel=%p)", timestamp_now(), channel);
 sts = real_sm_play_tone_abort(channel);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_play_tone_list(struct sm_play_tone_list_parms *tonep)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_play_tone_list(channel=%p, operation=%d, duration=%u, frequency1=%g, amplitude1=%g, frequency2=%g, amplitude2=%g, tone_count=%d)", timestamp_now(), tonep->channel, tonep->tones->operation, tonep->tones->duration, tonep->tones->frequency1, tonep->tones->amplitude1, tonep->tones->frequency2, tonep->tones->amplitude2, tonep->tone_count);
 sts = real_sm_play_tone_list(tonep);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_play_tone_list_abort(tSMChannelId channel)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_play_tone_list_abort(channel=%p)", timestamp_now(), channel);
 sts = real_sm_play_tone_list_abort(channel);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_play_tone_list_phase_reverse(struct sm_play_tone_list_phase_reverse_parms *pp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_play_tone_list_phase_reverse(channel=%p, period=%u)", timestamp_now(), pp->channel, pp->period);
 sts = real_sm_play_tone_list_phase_reverse(pp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_play_tone_list_status(struct sm_play_tone_list_status_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_play_tone_list_status(channel=%p)", timestamp_now(), statusp->channel);
 sts = real_sm_play_tone_list_status(statusp);
 if (TiNGtrace > 1) olog(" == %d channel=%p, status=%d\n", sts, statusp->channel, statusp->status);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_play_tone_status(struct sm_play_tone_status_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_play_tone_status(channel=%p)", timestamp_now(), statusp->channel);
 sts = real_sm_play_tone_status(statusp);
 if (TiNGtrace > 1) olog(" == %d channel=%p, status=%d\n", sts, statusp->channel, statusp->status);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_put_audio_data(struct sm_audio_data_parms *datap)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_put_audio_data(channel=%p, max_length=%d", timestamp_now(), datap->channel, datap->max_length);
	show_audio_data_in(datap);
 }
 sts = real_sm_put_audio_data(datap);
 if (TiNGtrace > 1) olog(" == %d done_length=%d\n", sts, datap->done_length);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_put_last_replay_data(struct sm_ts_data_parms *datap)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_put_last_replay_data(channel=%p, length=%d", timestamp_now(), datap->channel, datap->length);
	show_playrec_data_in(datap);
 }
 sts = real_sm_put_last_replay_data(datap);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_put_replay_data(struct sm_ts_data_parms *datap)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_put_replay_data(channel=%p, length=%d", timestamp_now(), datap->channel, datap->length);
	show_playrec_data_in(datap);
 }
 sts = real_sm_put_replay_data(datap);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_record_abort(struct sm_record_abort_parms *abortp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_record_abort(channel=%p, discard=%d)", timestamp_now(), abortp->channel, abortp->discard);
 sts = real_sm_record_abort(abortp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_record_agc_adjust(struct sm_record_agc_adjust_parms *recadjp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_record_agc_adjust(channel=%p, gain=%d)", timestamp_now(), recadjp->channel, recadjp->gain);
 sts = real_sm_record_agc_adjust(recadjp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_record_agc_adjust_settings(struct sm_record_agc_adjust_settings_parms *recadjp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_record_agc_adjust_settings(channel=%p, max_level_decay=%g, target_level=%g)", timestamp_now(), recadjp->channel, recadjp->max_level_decay, recadjp->target_level);
 sts = real_sm_record_agc_adjust_settings(recadjp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_record_start(struct sm_record_parms *recordp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_record_start(channel=%p, alt_data_source=%p, type=%d, silence_elimination=%u, tone_elimination_mode=%d, tone_elimination_set_id=%u, max_octets=%u, max_elapsed_time=%u, max_silence=%u, agc=%d, volume=%d, alt_data_source_type=%d, sampling_rate=%u, min_noise_level=%g, grunt_threshold=%g, grunt_holdoff=%u, max_initial_silence=%u)", timestamp_now(), recordp->channel, recordp->alt_data_source, recordp->type, recordp->silence_elimination, recordp->tone_elimination_mode, recordp->tone_elimination_set_id, recordp->max_octets, recordp->max_elapsed_time, recordp->max_silence, recordp->agc, recordp->volume, recordp->alt_data_source_type, recordp->sampling_rate, recordp->min_noise_level, recordp->grunt_threshold, recordp->grunt_holdoff, recordp->max_initial_silence);
 sts = real_sm_record_start(recordp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_record_status(struct sm_record_status_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_record_status(channel=%p)", timestamp_now(), statusp->channel);
 sts = real_sm_record_status(statusp);
 if (TiNGtrace > 1) olog(" == %d channel=%p, status=%d, recog_type=%d, param0=%d, param1=%d, termination_reason=%d, termination_octets=%u, sample_rate=%u\n", sts, statusp->channel, statusp->status, statusp->recog_type, statusp->param0, statusp->param1, statusp->termination_reason, statusp->termination_octets, statusp->sample_rate);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_replay_abort(struct sm_replay_abort_parms *abortp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_replay_abort(channel=%p, nowait=%u)", timestamp_now(), abortp->channel, abortp->nowait);
 sts = real_sm_replay_abort(abortp);
 if (TiNGtrace > 1) olog(" == %d offset=%u\n", sts, abortp->offset);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_replay_adjust(struct sm_replay_adjust_parms *adjustp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_replay_adjust(channel=%p, background=%p, volume=%d, agc=%d, speed=%d)", timestamp_now(), adjustp->channel, adjustp->background, adjustp->volume, adjustp->agc, adjustp->speed);
 sts = real_sm_replay_adjust(adjustp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_replay_start(struct sm_replay_parms *replayp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_replay_start(channel=%p, background=%p, volume=%d, agc=%d, speed=%d, type=%d, data_length=%u, sampling_rate=%u)", timestamp_now(), replayp->channel, replayp->background, replayp->volume, replayp->agc, replayp->speed, replayp->type, replayp->data_length, replayp->sampling_rate);
 sts = real_sm_replay_start(replayp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_replay_status(struct sm_replay_status_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_replay_status(channel=%p)", timestamp_now(), statusp->channel);
 sts = real_sm_replay_status(statusp);
 if (TiNGtrace > 1) olog(" == %d channel=%p, status=%d, offset=%u\n", sts, statusp->channel, statusp->status, statusp->offset);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_reset_input_cptones(struct sm_reset_input_cptones_parms *resetp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_reset_input_cptones(module=" TiNG_PRINTF_MODULE ", tone_set_id=%d)", timestamp_now(), resetp->module, resetp->tone_set_id);
 sts = real_sm_reset_input_cptones(resetp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_add_input_vocab(struct sm_input_vocab_parms *vocabp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_add_input_vocab(module=" TiNG_PRINTF_MODULE ", filename='%s')", timestamp_now(), vocabp->module, vocabp->filename);
 sts = real_sm_add_input_vocab(vocabp);
 if (TiNGtrace > 1) olog(" == %d item_id=%u\n", sts, vocabp->item_id);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_asr_listen_for(struct sm_asr_listen_for_parms *listenp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_asr_listen_for(channel=%p, vocab_item_count=%d, asr_mode=%d, sidetone=%p", timestamp_now(), listenp->channel, listenp->vocab_item_count, listenp->asr_mode, listenp->sidetone);
	show_vocab_item_list_vocab_recog_list_vocab_params_in(listenp);
 }
 sts = real_sm_asr_listen_for(listenp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_reset_input_vocabs(struct sm_reset_input_vocabs_parms *resetp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_reset_input_vocabs(module=" TiNG_PRINTF_MODULE ")", timestamp_now(), resetp->module);
 sts = real_sm_reset_input_vocabs(resetp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_set_sidetone_channel(struct sm_set_sidetone_channel_parms *sidetp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_set_sidetone_channel(channel=%p, output=%p)", timestamp_now(), sidetp->channel, sidetp->output);
 sts = real_sm_set_sidetone_channel(sidetp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}
