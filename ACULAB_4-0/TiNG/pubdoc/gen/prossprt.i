/* prossprt.i - Prosody SPRT end point trace code */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

ACUAPI int ACUTiNG_WINAPI sm_sprt_config(struct sm_sprt_config_parms *configp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_sprt_config(sprt=%p, dest=" FORMAT_SOCKADDR_IN ", src=" FORMAT_SOCKADDR_IN ", payload_type=%d)", timestamp_now(), configp->sprt, ARGS_SOCKADDR_IN(configp->dest), ARGS_SOCKADDR_IN(configp->src), configp->payload_type);
 sts = real_sm_sprt_config(configp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_sprt_config_data_channel(struct sm_sprt_config_data_channel_parms *configp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_sprt_config_data_channel(sprt=%p, data_channel=%d, max_payload_length=%d, max_latency=%d)", timestamp_now(), configp->sprt, configp->data_channel, configp->max_payload_length, configp->max_latency);
 sts = real_sm_sprt_config_data_channel(configp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_sprt_config_data_format(struct sm_sprt_config_data_format_parms *configp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_sprt_config_data_format(sprt=%p, direction=%d, format=%d)", timestamp_now(), configp->sprt, configp->direction, configp->format);
 sts = real_sm_sprt_config_data_format(configp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_sprt_config_ipv6(struct sm_sprt_config_ipv6_parms *configp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_sprt_config_ipv6(sprt=%p, dest=" FORMAT_SOCKADDR_IN6 ", src=" FORMAT_SOCKADDR_IN6 ", payload_type=%d)", timestamp_now(), configp->sprt, ARGS_SOCKADDR_IN6(configp->dest), ARGS_SOCKADDR_IN6(configp->src), configp->payload_type);
 sts = real_sm_sprt_config_ipv6(configp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_sprt_config_reliable_channel(struct sm_sprt_config_reliable_channel_parms *configp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_sprt_config_reliable_channel(sprt=%p, channel=%d, window_size=%d, timer_TA01=%d, timer_TA02=%d, timer_TR03=%d)", timestamp_now(), configp->sprt, configp->channel, configp->window_size, configp->timer_TA01, configp->timer_TA02, configp->timer_TR03);
 sts = real_sm_sprt_config_reliable_channel(configp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_sprt_create(struct sm_sprt_create_parms *createp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_sprt_create(module=" TiNG_PRINTF_MODULE ")", timestamp_now(), createp->module);
 sts = real_sm_sprt_create(createp);
 if (TiNGtrace > 1) olog(" == %d sprt=%p\n", sts, createp->sprt);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_sprt_datafeed_connect(struct sm_sprt_datafeed_connect_parms *datafeedp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_sprt_datafeed_connect(data_source=%p, sprt=%p)", timestamp_now(), datafeedp->data_source, datafeedp->sprt);
 sts = real_sm_sprt_datafeed_connect(datafeedp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_sprt_destroy(tSMSPRTId sprt)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_sprt_destroy(sprt=%p)", timestamp_now(), sprt);
 sts = real_sm_sprt_destroy(sprt);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_sprt_get_datafeed(struct sm_sprt_get_datafeed_parms *datafeedp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_sprt_get_datafeed(sprt=%p)", timestamp_now(), datafeedp->sprt);
 sts = real_sm_sprt_get_datafeed(datafeedp);
 if (TiNGtrace > 1) olog(" == %d datafeed=%p\n", sts, datafeedp->datafeed);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_sprt_get_event(struct sm_sprt_get_event_parms *eventp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_sprt_get_event(sprt=%p)", timestamp_now(), eventp->sprt);
 sts = real_sm_sprt_get_event(eventp);
 if (TiNGtrace > 1) olog(" == %d event=" FORMAT_EVENT "\n", sts, ARGS_EVENT(eventp->event));
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_sprt_read_message(struct sm_sprt_read_message_parms *datafeedp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_sprt_read_message(sprt=%p, max_length=%d", timestamp_now(), datafeedp->sprt, datafeedp->max_length);
	show_sprt_message_rx_in(datafeedp);
 }
 sts = real_sm_sprt_read_message(datafeedp);
 if (TiNGtrace > 1) {
	olog(" == %d length=%d", sts, datafeedp->length);
	show_sprt_message_rx_out(datafeedp);
 }
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_sprt_send_message(struct sm_sprt_send_message_parms *datafeedp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_sprt_send_message(sprt=%p, length=%d", timestamp_now(), datafeedp->sprt, datafeedp->length);
	show_sprt_message_tx_in(datafeedp);
 }
 sts = real_sm_sprt_send_message(datafeedp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_sprt_status(struct sm_sprt_status_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_sprt_status(sprt=%p)", timestamp_now(), statusp->sprt);
 sts = real_sm_sprt_status(statusp);
 if (TiNGtrace > 1) olog(" == %d status=%d\n", sts, statusp->status);
 return sts;
}
