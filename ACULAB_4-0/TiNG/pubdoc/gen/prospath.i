/* prospath.i - Prosody signal path processing trace code */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

ACUAPI int ACUTiNG_WINAPI sm_path_agc(struct sm_path_agc_parms *agcp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_path_agc(path=%p, agc=%d, volume=%d)", timestamp_now(), agcp->path, agcp->agc, agcp->volume);
 sts = real_sm_path_agc(agcp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_path_agc_adjust_settings(struct sm_path_agc_adjust_settings_parms *agcadjp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_path_agc_adjust_settings(path=%p, max_level_decay=%g, target_level=%g)", timestamp_now(), agcadjp->path, agcadjp->max_level_decay, agcadjp->target_level);
 sts = real_sm_path_agc_adjust_settings(agcadjp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_path_avf_amrnb_dec(struct sm_path_avf_amrnb_dec_parms *amrnbdecparms)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_path_avf_amrnb_dec(path=%p, enable=%d)", timestamp_now(), amrnbdecparms->path, amrnbdecparms->enable);
 sts = real_sm_path_avf_amrnb_dec(amrnbdecparms);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_path_avf_amrnb_enc(struct sm_path_avf_amrnb_enc_parms *amrnbencparms)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_path_avf_amrnb_enc(path=%p, enable=%d, payload_type=%d, bitrate=%d, vad_enable=%d)", timestamp_now(), amrnbencparms->path, amrnbencparms->enable, amrnbencparms->payload_type, amrnbencparms->bitrate, amrnbencparms->vad_enable);
 sts = real_sm_path_avf_amrnb_enc(amrnbencparms);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_path_create(struct sm_path_create_parms *createp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_path_create(module=" TiNG_PRINTF_MODULE ")", timestamp_now(), createp->module);
 sts = real_sm_path_create(createp);
 if (TiNGtrace > 1) olog(" == %d path=%p\n", sts, createp->path);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_path_datafeed_connect(struct sm_path_datafeed_connect_parms *datafeedp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_path_datafeed_connect(path=%p, data_source=%p)", timestamp_now(), datafeedp->path, datafeedp->data_source);
 sts = real_sm_path_datafeed_connect(datafeedp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_path_delay(struct sm_path_delay_parms *delayparms)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_path_delay(path=%p, enable=%d, delay=%d)", timestamp_now(), delayparms->path, delayparms->enable, delayparms->delay);
 sts = real_sm_path_delay(delayparms);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_path_destroy(tSMPathId path)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_path_destroy(path=%p)", timestamp_now(), path);
 sts = real_sm_path_destroy(path);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_path_echocancel(struct sm_path_echocancel_parms *cancelp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_path_echocancel(path=%p, enable=%d, reference=%p, non_linear=%d, mode=%d, use_agc=%d, fix_agc=%d, span=%d, delay=%d, tone_action=%d)", timestamp_now(), cancelp->path, cancelp->enable, cancelp->reference, cancelp->non_linear, cancelp->mode, cancelp->use_agc, cancelp->fix_agc, cancelp->span, cancelp->delay, cancelp->tone_action);
 sts = real_sm_path_echocancel(cancelp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_path_emphasis(struct sm_path_emphasis_parms *emphparms)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_path_emphasis(path=%p, enable=%d, mode=%d, gain=%d)", timestamp_now(), emphparms->path, emphparms->enable, emphparms->mode, emphparms->gain);
 sts = real_sm_path_emphasis(emphparms);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_path_get_datafeed(struct sm_path_datafeed_parms *datafeedp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_path_get_datafeed(path=%p)", timestamp_now(), datafeedp->path);
 sts = real_sm_path_get_datafeed(datafeedp);
 if (TiNGtrace > 1) olog(" == %d datafeed=%p\n", sts, datafeedp->datafeed);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_path_get_event(struct sm_path_event_parms *eventp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_path_get_event(path=%p)", timestamp_now(), eventp->path);
 sts = real_sm_path_get_event(eventp);
 if (TiNGtrace > 1) olog(" == %d event=" FORMAT_EVENT "\n", sts, ARGS_EVENT(eventp->event));
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_path_mix(struct sm_path_mix_parms *mixp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_path_mix(path=%p, enable=%d, mixin=%p, volume=%d)", timestamp_now(), mixp->path, mixp->enable, mixp->mixin, mixp->volume);
 sts = real_sm_path_mix(mixp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_path_pitchshift(struct sm_path_pitchshift_parms *pitchshiftp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_path_pitchshift(path=%p, shift=%g)", timestamp_now(), pitchshiftp->path, pitchshiftp->shift);
 sts = real_sm_path_pitchshift(pitchshiftp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_path_resample(struct sm_path_resample_parms *resamplep)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_path_resample(path=%p, uprate=%d, downrate=%d)", timestamp_now(), resamplep->path, resamplep->uprate, resamplep->downrate);
 sts = real_sm_path_resample(resamplep);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_path_status(struct sm_path_status_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_path_status(path=%p)", timestamp_now(), statusp->path);
 sts = real_sm_path_status(statusp);
 if (TiNGtrace > 1) olog(" == %d status=%d, id=%d, duration=%u\n", sts, statusp->status, statusp->u.tone.id, statusp->u.tone.duration);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_path_stop(struct sm_path_stop_parms *stopp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_path_stop(path=%p)", timestamp_now(), stopp->path);
 sts = real_sm_path_stop(stopp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_path_tonedetect(struct sm_path_tonedetect_parms *tdparms)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_path_tonedetect(path=%p, elim=%d, min_duration=%d, tone_set_id=%d)", timestamp_now(), tdparms->path, tdparms->elim, tdparms->min_duration, tdparms->tone_set_id);
 sts = real_sm_path_tonedetect(tdparms);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_path_ttyeliminate(struct sm_path_ttyeliminate_parms *ttyeliminatep)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_path_ttyeliminate(path=%p, enable=%d)", timestamp_now(), ttyeliminatep->path, ttyeliminatep->enable);
 sts = real_sm_path_ttyeliminate(ttyeliminatep);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}
