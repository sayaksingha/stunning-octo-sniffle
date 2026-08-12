/* prosi460muxapi.i - Prosody I.460 Mux trace code */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

ACUAPI int ACUTiNG_WINAPI sm_i460mux_create(struct sm_i460mux_create_parms *createp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_i460mux_create(module=" TiNG_PRINTF_MODULE ")", timestamp_now(), createp->module);
 sts = real_sm_i460mux_create(createp);
 if (TiNGtrace > 1) olog(" == %d i460mux=%p\n", sts, createp->i460mux);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_i460mux_datafeed_connect(struct sm_i460mux_datafeed_connect_parms *datafeedp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_i460mux_datafeed_connect(data_source=%p, i460mux=%p)", timestamp_now(), datafeedp->data_source, datafeedp->i460mux);
 sts = real_sm_i460mux_datafeed_connect(datafeedp);
 if (TiNGtrace > 1) olog(" == %d input_id=%d\n", sts, datafeedp->input_id);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_i460mux_datafeed_disconnect(struct sm_i460mux_datafeed_disconnect_parms *datafeedp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_i460mux_datafeed_disconnect(i460mux=%p, input_id=%d)", timestamp_now(), datafeedp->i460mux, datafeedp->input_id);
 sts = real_sm_i460mux_datafeed_disconnect(datafeedp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_i460mux_destroy(tSMI460muxId mux)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_i460mux_destroy(mux=%p)", timestamp_now(), mux);
 sts = real_sm_i460mux_destroy(mux);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_i460mux_get_datafeed(struct sm_i460mux_datafeed_parms *datafeedp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_i460mux_get_datafeed(i460mux=%p)", timestamp_now(), datafeedp->i460mux);
 sts = real_sm_i460mux_get_datafeed(datafeedp);
 if (TiNGtrace > 1) olog(" == %d datafeed=%p\n", sts, datafeedp->datafeed);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_i460mux_get_event(struct sm_i460mux_event_parms *eventp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_i460mux_get_event(i460mux=%p)", timestamp_now(), eventp->i460mux);
 sts = real_sm_i460mux_get_event(eventp);
 if (TiNGtrace > 1) olog(" == %d event=" FORMAT_EVENT "\n", sts, ARGS_EVENT(eventp->event));
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_i460mux_status(struct sm_i460mux_status_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_i460mux_status(i460mux=%p)", timestamp_now(), statusp->i460mux);
 sts = real_sm_i460mux_status(statusp);
 if (TiNGtrace > 1) olog(" == %d status=%d\n", sts, statusp->status);
 return sts;
}
