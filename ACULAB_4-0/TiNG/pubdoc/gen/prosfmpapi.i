/* prosfmpapi.i - Prosody FMP processing trace code */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

ACUAPI int ACUTiNG_WINAPI sm_fmprx_create(struct sm_fmprx_create_parms *fmprxp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_fmprx_create(module=" TiNG_PRINTF_MODULE ", type=%d, address=" FORMAT_IN_ADDR ", ipv6_address=" FORMAT_IN6_ADDR ")", timestamp_now(), fmprxp->module, fmprxp->type, ARGS_IN_ADDR(fmprxp->address), ARGS_IN6_ADDR(fmprxp->ipv6_address));
 sts = real_sm_fmprx_create(fmprxp);
 if (TiNGtrace > 1) olog(" == %d fmprx=%p\n", sts, fmprxp->fmprx);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_fmprx_destroy(tSMFMPrxId fmprx)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_fmprx_destroy(fmprx=%p)", timestamp_now(), fmprx);
 sts = real_sm_fmprx_destroy(fmprx);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_fmprx_get_datafeed(struct sm_fmprx_datafeed_parms *datafeedp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_fmprx_get_datafeed(fmprx=%p)", timestamp_now(), datafeedp->fmprx);
 sts = real_sm_fmprx_get_datafeed(datafeedp);
 if (TiNGtrace > 1) olog(" == %d datafeed=%p\n", sts, datafeedp->datafeed);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_fmprx_get_event(struct sm_fmprx_event_parms *eventp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_fmprx_get_event(fmprx=%p)", timestamp_now(), eventp->fmprx);
 sts = real_sm_fmprx_get_event(eventp);
 if (TiNGtrace > 1) olog(" == %d event=" FORMAT_EVENT "\n", sts, ARGS_EVENT(eventp->event));
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_fmprx_status(struct sm_fmprx_status_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_fmprx_status(fmprx=%p)", timestamp_now(), statusp->fmprx);
 sts = real_sm_fmprx_status(statusp);
 if (TiNGtrace > 1) olog(" == %d status=%d, T38_port=%d, address=" FORMAT_IN_ADDR ", address=" FORMAT_IN_ADDR ", port=%d, T38_port=%d, address=" FORMAT_IN6_ADDR ", address=" FORMAT_IN6_ADDR ", port=%d\n", sts, statusp->status, statusp->u.port.T38_port, ARGS_IN_ADDR(statusp->u.port.address), ARGS_IN_ADDR(statusp->u.source.address), statusp->u.source.port, statusp->u.port_ipv6.T38_port, ARGS_IN6_ADDR(statusp->u.port_ipv6.address), ARGS_IN6_ADDR(statusp->u.source_ipv6.address), statusp->u.source_ipv6.port);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_fmprx_stop(struct sm_fmprx_stop_parms *stopp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_fmprx_stop(fmprx=%p)", timestamp_now(), stopp->fmprx);
 sts = real_sm_fmprx_stop(stopp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_fmptx_config(struct sm_fmptx_config_parms *configp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_fmptx_config(fmptx=%p, destination=" FORMAT_SOCKADDR_IN ", source=" FORMAT_SOCKADDR_IN ", Recovery=%d, RecoveryLevel=%u)", timestamp_now(), configp->fmptx, ARGS_SOCKADDR_IN(configp->destination), ARGS_SOCKADDR_IN(configp->source), configp->Recovery, configp->RecoveryLevel);
 sts = real_sm_fmptx_config(configp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_fmptx_config_ipv6(struct sm_fmptx_config_ipv6_parms *configp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_fmptx_config_ipv6(fmptx=%p, destination=" FORMAT_SOCKADDR_IN6 ", source=" FORMAT_SOCKADDR_IN6 ", Recovery=%d, RecoveryLevel=%u)", timestamp_now(), configp->fmptx, ARGS_SOCKADDR_IN6(configp->destination), ARGS_SOCKADDR_IN6(configp->source), configp->Recovery, configp->RecoveryLevel);
 sts = real_sm_fmptx_config_ipv6(configp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_fmptx_create(struct sm_fmptx_create_parms *fmptxp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_fmptx_create(module=" TiNG_PRINTF_MODULE ")", timestamp_now(), fmptxp->module);
 sts = real_sm_fmptx_create(fmptxp);
 if (TiNGtrace > 1) olog(" == %d fmptx=%p\n", sts, fmptxp->fmptx);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_fmptx_datafeed_connect(struct sm_fmptx_datafeed_connect_parms *datafeedp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_fmptx_datafeed_connect(data_source=%p, data_dest=%p)", timestamp_now(), datafeedp->data_source, datafeedp->data_dest);
 sts = real_sm_fmptx_datafeed_connect(datafeedp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_fmptx_destroy(tSMFMPtxId fmptx)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_fmptx_destroy(fmptx=%p)", timestamp_now(), fmptx);
 sts = real_sm_fmptx_destroy(fmptx);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_fmptx_get_event(struct sm_fmptx_event_parms *eventp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_fmptx_get_event(fmptx=%p)", timestamp_now(), eventp->fmptx);
 sts = real_sm_fmptx_get_event(eventp);
 if (TiNGtrace > 1) olog(" == %d event=" FORMAT_EVENT "\n", sts, ARGS_EVENT(eventp->event));
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_fmptx_status(struct sm_fmptx_status_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_fmptx_status(fmptx=%p)", timestamp_now(), statusp->fmptx);
 sts = real_sm_fmptx_status(statusp);
 if (TiNGtrace > 1) olog(" == %d status=%d\n", sts, statusp->status);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_fmptx_stop(struct sm_fmptx_stop_parms *stopp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_fmptx_stop(fmptx=%p)", timestamp_now(), stopp->fmptx);
 sts = real_sm_fmptx_stop(stopp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}
