/* prossntp.i - Prosody SNTP Client trace code */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

ACUAPI int ACUTiNG_WINAPI sm_sntp_config(struct sm_sntp_config_parms *configp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_sntp_config(sntp=%p, server=" FORMAT_SOCKADDR_IN ", TOS=%d, no_reply_delay=%u, had_reply_delay=%u)", timestamp_now(), configp->sntp, ARGS_SOCKADDR_IN(configp->server), configp->TOS, configp->no_reply_delay, configp->had_reply_delay);
 sts = real_sm_sntp_config(configp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_sntp_config_ipv6(struct sm_sntp_config_ipv6_parms *configp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_sntp_config_ipv6(sntp=%p, server=" FORMAT_SOCKADDR_IN6 ", no_reply_delay=%u, had_reply_delay=%u)", timestamp_now(), configp->sntp, ARGS_SOCKADDR_IN6(configp->server), configp->no_reply_delay, configp->had_reply_delay);
 sts = real_sm_sntp_config_ipv6(configp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_sntp_create(struct sm_sntp_create_parms *createp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_sntp_create(module=" TiNG_PRINTF_MODULE ", type=%d, address=" FORMAT_IN_ADDR ", ipv6_address=" FORMAT_IN6_ADDR ")", timestamp_now(), createp->module, createp->type, ARGS_IN_ADDR(createp->address), ARGS_IN6_ADDR(createp->ipv6_address));
 sts = real_sm_sntp_create(createp);
 if (TiNGtrace > 1) olog(" == %d sntp=%p\n", sts, createp->sntp);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_sntp_destroy(tSMSNTPId sntp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_sntp_destroy(sntp=%p)", timestamp_now(), sntp);
 sts = real_sm_sntp_destroy(sntp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_sntp_get_event(struct sm_sntp_event_parms *eventp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_sntp_get_event(sntp=%p)", timestamp_now(), eventp->sntp);
 sts = real_sm_sntp_get_event(eventp);
 if (TiNGtrace > 1) olog(" == %d event=" FORMAT_EVENT "\n", sts, ARGS_EVENT(eventp->event));
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_sntp_status(struct sm_sntp_status_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_sntp_status(sntp=%p)", timestamp_now(), statusp->sntp);
 sts = real_sm_sntp_status(statusp);
 if (TiNGtrace > 1) olog(" == %d status=%d, has_sync=%d\n", sts, statusp->status, statusp->u.sync.has_sync);
 return sts;
}
