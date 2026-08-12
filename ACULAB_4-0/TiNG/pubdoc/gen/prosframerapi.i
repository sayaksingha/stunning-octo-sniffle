/* prosframerapi.i - Prosody Framer trace code */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

ACUAPI int ACUTiNG_WINAPI sm_framer_create(struct sm_framer_create_parms *createp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_framer_create(module=" TiNG_PRINTF_MODULE ")", timestamp_now(), createp->module);
 sts = real_sm_framer_create(createp);
 if (TiNGtrace > 1) olog(" == %d framer=%p\n", sts, createp->framer);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_framer_datafeed_connect(struct sm_framer_datafeed_connect_parms *datafeedp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_framer_datafeed_connect(data_source=%p, framer=%p, direction=%d)", timestamp_now(), datafeedp->data_source, datafeedp->framer, datafeedp->direction);
 sts = real_sm_framer_datafeed_connect(datafeedp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_framer_datafeed_disconnect(struct sm_framer_datafeed_disconnect_parms *datafeedp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_framer_datafeed_disconnect(framer=%p, direction=%d)", timestamp_now(), datafeedp->framer, datafeedp->direction);
 sts = real_sm_framer_datafeed_disconnect(datafeedp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_framer_destroy(tSMFramerId framer)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_framer_destroy(framer=%p)", timestamp_now(), framer);
 sts = real_sm_framer_destroy(framer);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_framer_get_datafeed(struct sm_framer_datafeed_parms *datafeedp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_framer_get_datafeed(framer=%p, direction=%d)", timestamp_now(), datafeedp->framer, datafeedp->direction);
 sts = real_sm_framer_get_datafeed(datafeedp);
 if (TiNGtrace > 1) olog(" == %d datafeed=%p\n", sts, datafeedp->datafeed);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_framer_get_event(struct sm_framer_event_parms *eventp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_framer_get_event(framer=%p)", timestamp_now(), eventp->framer);
 sts = real_sm_framer_get_event(eventp);
 if (TiNGtrace > 1) olog(" == %d event=" FORMAT_EVENT "\n", sts, ARGS_EVENT(eventp->event));
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_framer_status(struct sm_framer_status_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_framer_status(framer=%p)", timestamp_now(), statusp->framer);
 sts = real_sm_framer_status(statusp);
 if (TiNGtrace > 1) olog(" == %d status=%d, new_state=%d\n", sts, statusp->status, statusp->new_state);
 return sts;
}
