/* prosh223api.i - Prosody H.223 Mux trace code */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

ACUAPI int ACUTiNG_WINAPI sm_h223mux_add_entry(struct sm_h223mux_add_entry_parms *entryp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_h223mux_add_entry(h223mux=%p, direction=%d, entry_number=%d, descriptor_len=%d, descriptor='%s')", timestamp_now(), entryp->h223mux, entryp->direction, entryp->entry_number, entryp->descriptor_len, entryp->descriptor);
 sts = real_sm_h223mux_add_entry(entryp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_h223mux_close_lc(struct sm_h223mux_close_lc_parms *close_lcp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_h223mux_close_lc(h223mux=%p, direction=%d, lcn=%d)", timestamp_now(), close_lcp->h223mux, close_lcp->direction, close_lcp->lcn);
 sts = real_sm_h223mux_close_lc(close_lcp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_h223mux_create(struct sm_h223mux_create_parms *createp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_h223mux_create(module=" TiNG_PRINTF_MODULE ")", timestamp_now(), createp->module);
 sts = real_sm_h223mux_create(createp);
 if (TiNGtrace > 1) olog(" == %d h223mux=%p\n", sts, createp->h223mux);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_h223mux_datafeed_connect(struct sm_h223mux_datafeed_connect_parms *datafeedp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_h223mux_datafeed_connect(datafeed=%p, port=%d, h223mux=%p)", timestamp_now(), datafeedp->datafeed, datafeedp->port, datafeedp->h223mux);
 sts = real_sm_h223mux_datafeed_connect(datafeedp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_h223mux_delete_session(struct sm_h223mux_delete_session_parms *sessionp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_h223mux_delete_session(h223mux=%p)", timestamp_now(), sessionp->h223mux);
 sts = real_sm_h223mux_delete_session(sessionp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_h223mux_destroy(tSMH223muxId mux)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_h223mux_destroy(mux=%p)", timestamp_now(), mux);
 sts = real_sm_h223mux_destroy(mux);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_h223mux_get_datafeed(struct sm_h223mux_datafeed_parms *datafeedp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_h223mux_get_datafeed(h223mux=%p, port=%d)", timestamp_now(), datafeedp->h223mux, datafeedp->port);
 sts = real_sm_h223mux_get_datafeed(datafeedp);
 if (TiNGtrace > 1) olog(" == %d datafeed=%p\n", sts, datafeedp->datafeed);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_h223mux_get_event(struct sm_h223mux_event_parms *eventp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_h223mux_get_event(h223mux=%p)", timestamp_now(), eventp->h223mux);
 sts = real_sm_h223mux_get_event(eventp);
 if (TiNGtrace > 1) olog(" == %d event=" FORMAT_EVENT "\n", sts, ARGS_EVENT(eventp->event));
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_h223mux_new_session(struct sm_h223mux_new_session_parms *sessionp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_h223mux_new_session(h223mux=%p, level=%d, bit_order=%d)", timestamp_now(), sessionp->h223mux, sessionp->level, sessionp->bit_order);
 sts = real_sm_h223mux_new_session(sessionp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_h223mux_open_lc(struct sm_h223mux_open_lc_parms *open_lcp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_h223mux_open_lc(h223mux=%p, segmentable_flag=%u, type=%d, lcn=%d, direction=%d, mark_unit=%d, hi_mark=%d, lo_mark=%d, adaptation_layer_type=%d, control_field_octets=%d, send_buffer_size=%d, lcn=%d, direction=%d, mark_unit=%d, hi_mark=%d, lo_mark=%d, adaptation_layer_type=%d, control_field_octets=%d, send_buffer_size=%d)", timestamp_now(), open_lcp->h223mux, open_lcp->segmentable_flag, open_lcp->type, open_lcp->forward_channel.lcn, open_lcp->forward_channel.direction, open_lcp->forward_channel.transmit_lc.mark_unit, open_lcp->forward_channel.transmit_lc.hi_mark, open_lcp->forward_channel.transmit_lc.lo_mark, open_lcp->forward_channel.adaptation_layer_type, open_lcp->forward_channel.u_al.al3.control_field_octets, open_lcp->forward_channel.u_al.al3.send_buffer_size, open_lcp->reverse_channel.lcn, open_lcp->reverse_channel.direction, open_lcp->reverse_channel.transmit_lc.mark_unit, open_lcp->reverse_channel.transmit_lc.hi_mark, open_lcp->reverse_channel.transmit_lc.lo_mark, open_lcp->reverse_channel.adaptation_layer_type, open_lcp->reverse_channel.u_al.al3.control_field_octets, open_lcp->reverse_channel.u_al.al3.send_buffer_size);
 sts = real_sm_h223mux_open_lc(open_lcp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_h223mux_reconfig(struct sm_h223mux_reconfig_parms *reconfigp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_h223mux_reconfig(h223mux=%p, direction=%d, choice=%d, mode_change=%d, annex_a_double_flag=%d)", timestamp_now(), reconfigp->h223mux, reconfigp->direction, reconfigp->multiplex_reconfiguration.choice, reconfigp->multiplex_reconfiguration.u.mode_change, reconfigp->multiplex_reconfiguration.u.annex_a_double_flag);
 sts = real_sm_h223mux_reconfig(reconfigp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_h223mux_set_caps(struct sm_h223mux_set_caps_parms *capsp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_h223mux_set_caps(h223mux=%p, role=%d, optional_ext=%d, transport_with_i_frames=%u, video_with_AL1=%u, video_with_AL2=%u, video_with_AL3=%u, audio_with_AL1=%u, audio_with_AL2=%u, audio_with_AL3=%u, data_with_AL1=%u, data_with_AL2=%u, data_with_AL3=%u, maximum_AL2_sdu_size=%d, maximum_AL3_sdu_size=%d, maximum_delay_jitter=%d, max_MUX_PDU_size_capability=%u, nsrp_support=%u, bit_rate_o=%d, mode_change_capability=%u, h223_AnnexA=%u, h223_AnnexA_double_flag=%u, h223_AnnexB=%u, h223_AnnexB_with_header=%u, maximum_sample_size=%d, maximum_payload_length=%d, video_with_AL1M=%u, video_with_AL2M=%u, video_with_AL3M=%u, audio_with_AL1M=%u, audio_with_AL2M=%u, audio_with_AL3M=%u, data_with_AL1M=%u, data_with_AL2M=%u, data_with_AL3M=%u, alpdu_interleaving=%u, maximum_AL1M_pdu_size=%d, maximum_AL2M_pdu_size=%d, maximum_AL3M_pdu_size=%d, rs_code_capability=%u)", timestamp_now(), capsp->h223mux, capsp->role, capsp->optional_ext, capsp->transport_with_i_frames, capsp->video_with_AL1, capsp->video_with_AL2, capsp->video_with_AL3, capsp->audio_with_AL1, capsp->audio_with_AL2, capsp->audio_with_AL3, capsp->data_with_AL1, capsp->data_with_AL2, capsp->data_with_AL3, capsp->maximum_AL2_sdu_size, capsp->maximum_AL3_sdu_size, capsp->maximum_delay_jitter, capsp->max_MUX_PDU_size_capability, capsp->nsrp_support, capsp->bit_rate_o, capsp->mobile_operation_transmit_capability_o.mode_change_capability, capsp->mobile_operation_transmit_capability_o.h223_AnnexA, capsp->mobile_operation_transmit_capability_o.h223_AnnexA_double_flag, capsp->mobile_operation_transmit_capability_o.h223_AnnexB, capsp->mobile_operation_transmit_capability_o.h223_AnnexB_with_header, capsp->mobile_multilink_frame_capability_o.maximum_sample_size, capsp->mobile_multilink_frame_capability_o.maximum_payload_length, capsp->H223_AnnexC_capability_o.video_with_AL1M, capsp->H223_AnnexC_capability_o.video_with_AL2M, capsp->H223_AnnexC_capability_o.video_with_AL3M, capsp->H223_AnnexC_capability_o.audio_with_AL1M, capsp->H223_AnnexC_capability_o.audio_with_AL2M, capsp->H223_AnnexC_capability_o.audio_with_AL3M, capsp->H223_AnnexC_capability_o.data_with_AL1M, capsp->H223_AnnexC_capability_o.data_with_AL2M, capsp->H223_AnnexC_capability_o.data_with_AL3M, capsp->H223_AnnexC_capability_o.alpdu_interleaving, capsp->H223_AnnexC_capability_o.maximum_AL1M_pdu_size, capsp->H223_AnnexC_capability_o.maximum_AL2M_pdu_size, capsp->H223_AnnexC_capability_o.maximum_AL3M_pdu_size, capsp->H223_AnnexC_capability_o.rs_code_capability);
 sts = real_sm_h223mux_set_caps(capsp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_h223mux_status(struct sm_h223mux_status_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_h223mux_status(h223mux=%p)", timestamp_now(), statusp->h223mux);
 sts = real_sm_h223mux_status(statusp);
 if (TiNGtrace > 1) olog(" == %d status=%d, level=%d, num_errors=%d\n", sts, statusp->status, statusp->u.session_transport.level, statusp->u.internal_error.num_errors);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_h223mux_stop(struct sm_h223mux_stop_parms *stopp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_h223mux_stop(h223mux=%p)", timestamp_now(), stopp->h223mux);
 sts = real_sm_h223mux_stop(stopp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}
