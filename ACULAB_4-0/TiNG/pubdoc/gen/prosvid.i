/* prosvid.i - Prosody Audio Video Format (AVF) trace code */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

ACUAPI int ACUTiNG_WINAPI sm_avfplay_create(struct sm_avfplay_create_parms *avfcp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_avfplay_create(module=" TiNG_PRINTF_MODULE ")", timestamp_now(), avfcp->module);
 sts = real_sm_avfplay_create(avfcp);
 if (TiNGtrace > 1) olog(" == %d avfplay=%p\n", sts, avfcp->avfplay);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_avfplay_destroy(tSMAVFplayId avfplay)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_avfplay_destroy(avfplay=%p)", timestamp_now(), avfplay);
 sts = real_sm_avfplay_destroy(avfplay);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_avfplay_get_datafeed(struct sm_avfplay_datafeed_parms *datafeedp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_avfplay_get_datafeed(avfplay=%p)", timestamp_now(), datafeedp->avfplay);
 sts = real_sm_avfplay_get_datafeed(datafeedp);
 if (TiNGtrace > 1) olog(" == %d datafeed=%p\n", sts, datafeedp->datafeed);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_avfplay_get_event(struct sm_avfplay_event_parms *eventp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_avfplay_get_event(avfplay=%p)", timestamp_now(), eventp->avfplay);
 sts = real_sm_avfplay_get_event(eventp);
 if (TiNGtrace > 1) olog(" == %d event=" FORMAT_EVENT "\n", sts, ARGS_EVENT(eventp->event));
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_avfplay_put_data(struct sm_avfplay_put_data_parms *datap)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_avfplay_put_data(avfplay=%p, max_length=%d", timestamp_now(), datap->avfplay, datap->max_length);
	show_vidtx_data_in(datap);
 }
 sts = real_sm_avfplay_put_data(datap);
 if (TiNGtrace > 1) olog(" == %d done_length=%d\n", sts, datap->done_length);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_avfplay_start(struct sm_avfplay_start_parms *avfsp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_avfplay_start(avfplay=%p, dest_type=%d, sampling_rate=%d, src_type=%d, hbuf_size=%d, test_mode=%d)", timestamp_now(), avfsp->avfplay, avfsp->dest_type, avfsp->u_dest_type.audio.sampling_rate, avfsp->u_dest_type.audio.src_type, avfsp->hbuf_size, avfsp->test_mode);
 sts = real_sm_avfplay_start(avfsp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_avfplay_status(struct sm_avfplay_status_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_avfplay_status(avfplay=%p)", timestamp_now(), statusp->avfplay);
 sts = real_sm_avfplay_status(statusp);
 if (TiNGtrace > 1) olog(" == %d status=%d\n", sts, statusp->status);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_avfplay_stop(struct sm_avfplay_stop_parms *avfsp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_avfplay_stop(avfplay=%p, immediate=%d)", timestamp_now(), avfsp->avfplay, avfsp->immediate);
 sts = real_sm_avfplay_stop(avfsp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_avfplay_sync(struct sm_avfplay_sync_parms *avfsp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_avfplay_sync(avfplay=%p, sync_avfplay=%p)", timestamp_now(), avfsp->avfplay, avfsp->sync_avfplay);
 sts = real_sm_avfplay_sync(avfsp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_avfrec_create(struct sm_avfrec_create_parms *vcp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_avfrec_create(module=" TiNG_PRINTF_MODULE ")", timestamp_now(), vcp->module);
 sts = real_sm_avfrec_create(vcp);
 if (TiNGtrace > 1) olog(" == %d avfrec=%p\n", sts, vcp->avfrec);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_avfrec_datafeed_connect(struct sm_avfrec_datafeed_connect_parms *datafeedp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_avfrec_datafeed_connect(avfrec=%p, data_source=%p)", timestamp_now(), datafeedp->avfrec, datafeedp->data_source);
 sts = real_sm_avfrec_datafeed_connect(datafeedp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_avfrec_destroy(tSMAVFrecId avfrec)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_avfrec_destroy(avfrec=%p)", timestamp_now(), avfrec);
 sts = real_sm_avfrec_destroy(avfrec);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_avfrec_get_data(struct sm_avfrec_get_data_parms *datap)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_avfrec_get_data(avfrec=%p, max_length=%d", timestamp_now(), datap->avfrec, datap->max_length);
	show_vidrx_data_in(datap);
 }
 sts = real_sm_avfrec_get_data(datap);
 if (TiNGtrace > 1) olog(" == %d done_length=%d\n", sts, datap->done_length);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_avfrec_get_event(struct sm_avfrec_event_parms *eventp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_avfrec_get_event(avfrec=%p)", timestamp_now(), eventp->avfrec);
 sts = real_sm_avfrec_get_event(eventp);
 if (TiNGtrace > 1) olog(" == %d event=" FORMAT_EVENT "\n", sts, ARGS_EVENT(eventp->event));
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_avfrec_start(struct sm_avfrec_start_parms *avfsp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_avfrec_start(avfrec=%p, src_type=%d, payload_type=%d, dest_type=%d, bitrate=%d, vad_enable=%d, hbuf_size=%d, test_mode=%d)", timestamp_now(), avfsp->avfrec, avfsp->src_type, avfsp->u_src_type.audio.payload_type, avfsp->u_src_type.audio.dest_type, avfsp->u_src_type.audio.u_dest_type.amrnb.bitrate, avfsp->u_src_type.audio.u_dest_type.amrnb.vad_enable, avfsp->hbuf_size, avfsp->test_mode);
 sts = real_sm_avfrec_start(avfsp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_avfrec_status(struct sm_avfrec_status_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_avfrec_status(avfrec=%p)", timestamp_now(), statusp->avfrec);
 sts = real_sm_avfrec_status(statusp);
 if (TiNGtrace > 1) olog(" == %d status=%d\n", sts, statusp->status);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_avfrec_stop(struct sm_avfrec_stop_parms *avfsp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_avfrec_stop(avfrec=%p, immediate=%d)", timestamp_now(), avfsp->avfrec, avfsp->immediate);
 sts = real_sm_avfrec_stop(avfsp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_avfrec_sync(struct sm_avfrec_sync_parms *avfsp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_avfrec_sync(avfrec=%p, sync_avfrec=%p)", timestamp_now(), avfsp->avfrec, avfsp->sync_avfrec);
 sts = real_sm_avfrec_sync(avfsp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}
