/* prosdtls.i - Prosody DTLS processing trace code */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

ACUAPI int ACUTiNG_WINAPI sm_dtlsrx_create(struct sm_dtlsrx_create_parms *dtlsrxp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_dtlsrx_create(module=" TiNG_PRINTF_MODULE ")", timestamp_now(), dtlsrxp->module);
 sts = real_sm_dtlsrx_create(dtlsrxp);
 if (TiNGtrace > 1) olog(" == %d dtlsrx=%p\n", sts, dtlsrxp->dtlsrx);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_dtlsrx_destroy(tSMDTLSrxId dtlsrx)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_dtlsrx_destroy(dtlsrx=%p)", timestamp_now(), dtlsrx);
 sts = real_sm_dtlsrx_destroy(dtlsrx);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_dtlsrx_get_event(struct sm_dtlsrx_event_parms *eventp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_dtlsrx_get_event(dtlsrx=%p)", timestamp_now(), eventp->dtlsrx);
 sts = real_sm_dtlsrx_get_event(eventp);
 if (TiNGtrace > 1) olog(" == %d event=" FORMAT_EVENT "\n", sts, ARGS_EVENT(eventp->event));
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_dtlsrx_get_packet(struct sm_dtlsrx_get_packet_parms *readp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_dtlsrx_get_packet(dtlsrx=%p, max_length=%d", timestamp_now(), readp->dtlsrx, readp->max_length);
	show_rx_packet_in(readp);
 }
 sts = real_sm_dtlsrx_get_packet(readp);
 if (TiNGtrace > 1) olog(" == %d packet_length=%d\n", sts, readp->packet_length);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_dtlsrx_status(struct sm_dtlsrx_status_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_dtlsrx_status(dtlsrx=%p)", timestamp_now(), statusp->dtlsrx);
 sts = real_sm_dtlsrx_status(statusp);
 if (TiNGtrace > 1) olog(" == %d status=%d\n", sts, statusp->status);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_dtlsrx_stop(struct sm_dtlsrx_stop_parms *stopp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_dtlsrx_stop(dtlsrx=%p)", timestamp_now(), stopp->dtlsrx);
 sts = real_sm_dtlsrx_stop(stopp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_dtlstx_config(struct sm_dtlstx_config_parms *configp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_dtlstx_config(dtlstx=%p, destination=" FORMAT_SOCKADDR_IN ", source=" FORMAT_SOCKADDR_IN ")", timestamp_now(), configp->dtlstx, ARGS_SOCKADDR_IN(configp->destination), ARGS_SOCKADDR_IN(configp->source));
 sts = real_sm_dtlstx_config(configp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_dtlstx_config_ipv6(struct sm_dtlstx_config_ipv6_parms *configp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_dtlstx_config_ipv6(dtlstx=%p, destination=" FORMAT_SOCKADDR_IN6 ", source=" FORMAT_SOCKADDR_IN6 ")", timestamp_now(), configp->dtlstx, ARGS_SOCKADDR_IN6(configp->destination), ARGS_SOCKADDR_IN6(configp->source));
 sts = real_sm_dtlstx_config_ipv6(configp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_dtlstx_config_notify(struct sm_dtlstx_config_notify_parms *configp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_dtlstx_config_notify(dtlstx=%p, packet_length=%u, no_capacity_event=%d)", timestamp_now(), configp->dtlstx, configp->packet_length, configp->no_capacity_event);
 sts = real_sm_dtlstx_config_notify(configp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_dtlstx_create(struct sm_dtlstx_create_parms *dtlstxp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_dtlstx_create(module=" TiNG_PRINTF_MODULE ")", timestamp_now(), dtlstxp->module);
 sts = real_sm_dtlstx_create(dtlstxp);
 if (TiNGtrace > 1) olog(" == %d dtlstx=%p\n", sts, dtlstxp->dtlstx);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_dtlstx_destroy(tSMDTLStxId dtlstx)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_dtlstx_destroy(dtlstx=%p)", timestamp_now(), dtlstx);
 sts = real_sm_dtlstx_destroy(dtlstx);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_dtlstx_get_event(struct sm_dtlstx_event_parms *eventp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_dtlstx_get_event(dtlstx=%p)", timestamp_now(), eventp->dtlstx);
 sts = real_sm_dtlstx_get_event(eventp);
 if (TiNGtrace > 1) olog(" == %d event=" FORMAT_EVENT "\n", sts, ARGS_EVENT(eventp->event));
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_dtlstx_send_packet(struct sm_dtlstx_send_packet_parms *packetp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_dtlstx_send_packet(dtlstx=%p, packet_length=%d", timestamp_now(), packetp->dtlstx, packetp->packet_length);
	show_tx_packet_in(packetp);
 }
 sts = real_sm_dtlstx_send_packet(packetp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_dtlstx_status(struct sm_dtlstx_status_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_dtlstx_status(dtlstx=%p)", timestamp_now(), statusp->dtlstx);
 sts = real_sm_dtlstx_status(statusp);
 if (TiNGtrace > 1) olog(" == %d status=%d\n", sts, statusp->status);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_dtlstx_stop(struct sm_dtlstx_stop_parms *stopp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_dtlstx_stop(dtlstx=%p)", timestamp_now(), stopp->dtlstx);
 sts = real_sm_dtlstx_stop(stopp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_dtls(struct sm_vmprx_config_dtls_parms *configp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_dtls(vmprx=%p, dtlsrx=%p, use_RTCP_port=%d)", timestamp_now(), configp->vmprx, configp->dtlsrx, configp->use_RTCP_port);
 sts = real_sm_vmprx_config_dtls(configp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}
