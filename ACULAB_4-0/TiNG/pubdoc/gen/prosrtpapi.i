/* prosrtpapi.i - Prosody RTP processing trace code */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

ACUAPI int ACUTiNG_WINAPI sm_rtcphand_config_bandwidth(struct sm_rtcphand_config_bandwidth_parms *rvp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_rtcphand_config_bandwidth(rtcphand=%p, rtcp_bw=%d, rtcprx_bw=%d, rtcptx_bw=%d)", timestamp_now(), rvp->rtcphand, rvp->rtcp_bw, rvp->rtcprx_bw, rvp->rtcptx_bw);
 sts = real_sm_rtcphand_config_bandwidth(rvp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_rtcphand_config_options(struct sm_rtcphand_config_options_parms *rvp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_rtcphand_config_options(rtcphand=%p, rtcptx_suppress_xr=%d)", timestamp_now(), rvp->rtcphand, rvp->rtcptx_suppress_xr);
 sts = real_sm_rtcphand_config_options(rvp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_rtcphand_config_reports(struct sm_rtcphand_config_reports_parms *rvp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_rtcphand_config_reports(rtcphand=%p, reports=%d)", timestamp_now(), rvp->rtcphand, rvp->reports);
 sts = real_sm_rtcphand_config_reports(rvp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_rtcphand_config_sdes(struct sm_rtcphand_config_sdes_parms *rvp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_rtcphand_config_sdes(rtcphand=%p, itemnum=%d, stringlen=%d", timestamp_now(), rvp->rtcphand, rvp->itemnum, rvp->stringlen);
	show_sdes_in(rvp);
 }
 sts = real_sm_rtcphand_config_sdes(rvp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_rtcphand_create(struct sm_rtcphand_create_parms *rtcphandp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_rtcphand_create(module=" TiNG_PRINTF_MODULE ")", timestamp_now(), rtcphandp->module);
 sts = real_sm_rtcphand_create(rtcphandp);
 if (TiNGtrace > 1) olog(" == %d rtcphand=%p\n", sts, rtcphandp->rtcphand);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_rtcphand_destroy(tSMRTCPHandId rtcphand)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_rtcphand_destroy(rtcphand=%p)", timestamp_now(), rtcphand);
 sts = real_sm_rtcphand_destroy(rtcphand);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_rtcphand_get_data(struct sm_rtcphand_get_data_parms *datap)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_rtcphand_get_data(rtcphand=%p, max_length=%d", timestamp_now(), datap->rtcphand, datap->max_length);
	show_rtcp_data_in(datap);
 }
 sts = real_sm_rtcphand_get_data(datap);
 if (TiNGtrace > 1) olog(" == %d done_length=%d\n", sts, datap->done_length);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_rtcphand_get_event(struct sm_rtcphand_event_parms *eventp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_rtcphand_get_event(rtcphand=%p)", timestamp_now(), eventp->rtcphand);
 sts = real_sm_rtcphand_get_event(eventp);
 if (TiNGtrace > 1) olog(" == %d event=" FORMAT_EVENT "\n", sts, ARGS_EVENT(eventp->event));
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_rtcphand_request_statistics(struct sm_rtcphand_request_statistics_parms *rrsp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_rtcphand_request_statistics(rtcphand=%p, statcode=%d)", timestamp_now(), rrsp->rtcphand, rrsp->statcode);
 sts = real_sm_rtcphand_request_statistics(rrsp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_rtcphand_status(struct sm_rtcphand_status_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_rtcphand_status(rtcphand=%p)", timestamp_now(), statusp->rtcphand);
 sts = real_sm_rtcphand_status(statusp);
 if (TiNGtrace > 1) olog(" == %d status=%d\n", sts, statusp->status);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_authentication_hmac_sha1(struct sm_vmprx_config_authentication_hmac_sha1_parms *pp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_vmprx_config_authentication_hmac_sha1(vmprx=%p, keylen=%d, taglen=%d", timestamp_now(), pp->vmprx, pp->keylen, pp->taglen);
	show_key_rah_in(pp);
 }
 sts = real_sm_vmprx_config_authentication_hmac_sha1(pp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_authentication_null(struct sm_vmprx_config_authentication_null_parms *pp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_authentication_null(vmprx=%p)", timestamp_now(), pp->vmprx);
 sts = real_sm_vmprx_config_authentication_null(pp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_alaw(struct sm_vmprx_codec_alaw_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_alaw(vmprx=%p, payload_type=%d, plc_mode=%d)", timestamp_now(), codecp->vmprx, codecp->payload_type, codecp->plc_mode);
 sts = real_sm_vmprx_config_codec_alaw(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_amrnb(struct sm_vmprx_codec_amrnb_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_amrnb(vmprx=%p, payload_type=%d, aligned=%d, gsmefr=%d, plc_mode=%d, report_cmr=%d)", timestamp_now(), codecp->vmprx, codecp->payload_type, codecp->aligned, codecp->gsmefr, codecp->plc_mode, codecp->report_cmr);
 sts = real_sm_vmprx_config_codec_amrnb(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_amrwb(struct sm_vmprx_codec_amrwb_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_amrwb(vmprx=%p, payload_type=%d, aligned=%d, plc_mode=%d, report_cmr=%d)", timestamp_now(), codecp->vmprx, codecp->payload_type, codecp->aligned, codecp->plc_mode, codecp->report_cmr);
 sts = real_sm_vmprx_config_codec_amrwb(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_comfort_noise(struct sm_vmprx_codec_comfort_noise_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_comfort_noise(vmprx=%p, payload_type=%d, plc_mode=%d)", timestamp_now(), codecp->vmprx, codecp->payload_type, codecp->plc_mode);
 sts = real_sm_vmprx_config_codec_comfort_noise(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_evrc(struct sm_vmprx_codec_evrc_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_evrc(vmprx=%p, payload_type=%d, post_filter=%d, rtp_mode=%d, plc_mode=%d)", timestamp_now(), codecp->vmprx, codecp->payload_type, codecp->post_filter, codecp->rtp_mode, codecp->plc_mode);
 sts = real_sm_vmprx_config_codec_evrc(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_g722(struct sm_vmprx_codec_g722_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_g722(vmprx=%p, payload_type=%d, plc_mode=%d)", timestamp_now(), codecp->vmprx, codecp->payload_type, codecp->plc_mode);
 sts = real_sm_vmprx_config_codec_g722(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_g722_1(struct sm_vmprx_codec_g722_1_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_g722_1(vmprx=%p, payload_type=%d, bitrate=%d, plc_mode=%d)", timestamp_now(), codecp->vmprx, codecp->payload_type, codecp->bitrate, codecp->plc_mode);
 sts = real_sm_vmprx_config_codec_g722_1(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_g723_1(struct sm_vmprx_codec_g723_1_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_g723_1(vmprx=%p, payload_type=%d, post_filter=%d, plc_mode=%d)", timestamp_now(), codecp->vmprx, codecp->payload_type, codecp->post_filter, codecp->plc_mode);
 sts = real_sm_vmprx_config_codec_g723_1(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_g726(struct sm_vmprx_codec_g726_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_g726(vmprx=%p, payload_type=%d, bits=%d, plc_mode=%d, variant=%d)", timestamp_now(), codecp->vmprx, codecp->payload_type, codecp->bits, codecp->plc_mode, codecp->variant);
 sts = real_sm_vmprx_config_codec_g726(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_g728(struct sm_vmprx_codec_g728_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_g728(vmprx=%p, payload_type=%d, rate=%d, post_filter=%d, plc_mode=%d)", timestamp_now(), codecp->vmprx, codecp->payload_type, codecp->rate, codecp->post_filter, codecp->plc_mode);
 sts = real_sm_vmprx_config_codec_g728(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_g729ab(struct sm_vmprx_codec_g729ab_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_g729ab(vmprx=%p, payload_type=%d, plc_mode=%d)", timestamp_now(), codecp->vmprx, codecp->payload_type, codecp->plc_mode);
 sts = real_sm_vmprx_config_codec_g729ab(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_g729i(struct sm_vmprx_codec_g729i_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_g729i(vmprx=%p, payload_type=%d, g729_mode=%d)", timestamp_now(), codecp->vmprx, codecp->payload_type, codecp->g729_mode);
 sts = real_sm_vmprx_config_codec_g729i(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_gsmefr(struct sm_vmprx_codec_gsmefr_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_gsmefr(vmprx=%p, payload_type=%d, plc_mode=%d)", timestamp_now(), codecp->vmprx, codecp->payload_type, codecp->plc_mode);
 sts = real_sm_vmprx_config_codec_gsmefr(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_gsmfr(struct sm_vmprx_codec_gsmfr_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_gsmfr(vmprx=%p, payload_type=%d, variant=%d, plc_mode=%d)", timestamp_now(), codecp->vmprx, codecp->payload_type, codecp->variant, codecp->plc_mode);
 sts = real_sm_vmprx_config_codec_gsmfr(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_gsmhr(struct sm_vmprx_codec_gsmhr_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_gsmhr(vmprx=%p, payload_type=%d, plc_mode=%d)", timestamp_now(), codecp->vmprx, codecp->payload_type, codecp->plc_mode);
 sts = real_sm_vmprx_config_codec_gsmhr(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_ilbc(struct sm_vmprx_codec_ilbc_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_ilbc(vmprx=%p, payload_type=%d, frame_len=%d, enhancer=%d, plc_mode=%d)", timestamp_now(), codecp->vmprx, codecp->payload_type, codecp->frame_len, codecp->enhancer, codecp->plc_mode);
 sts = real_sm_vmprx_config_codec_ilbc(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_isac(struct sm_vmprx_codec_isac_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_isac(vmprx=%p, payload_type=%d)", timestamp_now(), codecp->vmprx, codecp->payload_type);
 sts = real_sm_vmprx_config_codec_isac(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_l16(struct sm_vmprx_codec_l16_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_l16(vmprx=%p, payload_type=%d, plc_mode=%d)", timestamp_now(), codecp->vmprx, codecp->payload_type, codecp->plc_mode);
 sts = real_sm_vmprx_config_codec_l16(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_l8(struct sm_vmprx_codec_l8_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_l8(vmprx=%p, payload_type=%d, plc_mode=%d)", timestamp_now(), codecp->vmprx, codecp->payload_type, codecp->plc_mode);
 sts = real_sm_vmprx_config_codec_l8(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_melpe(struct sm_vmprx_codec_melpe_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_melpe(vmprx=%p, payload_type=%d)", timestamp_now(), codecp->vmprx, codecp->payload_type);
 sts = real_sm_vmprx_config_codec_melpe(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_mulaw(struct sm_vmprx_codec_mulaw_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_mulaw(vmprx=%p, payload_type=%d, plc_mode=%d)", timestamp_now(), codecp->vmprx, codecp->payload_type, codecp->plc_mode);
 sts = real_sm_vmprx_config_codec_mulaw(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_opus(struct sm_vmprx_codec_opus_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_opus(vmprx=%p, decode_fec=%d, payload_type=%d)", timestamp_now(), codecp->vmprx, codecp->decode_fec, codecp->payload_type);
 sts = real_sm_vmprx_config_codec_opus(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_rfc2833(struct sm_vmprx_codec_rfc2833_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_rfc2833(vmprx=%p, payload_type=%d, plc_mode=%d)", timestamp_now(), codecp->vmprx, codecp->payload_type, codecp->plc_mode);
 sts = real_sm_vmprx_config_codec_rfc2833(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_rfc4040(struct sm_vmprx_codec_rfc4040_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_rfc4040(vmprx=%p, payload_type=%d)", timestamp_now(), codecp->vmprx, codecp->payload_type);
 sts = real_sm_vmprx_config_codec_rfc4040(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_rttext(struct sm_vmprx_codec_rttext_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_rttext(vmprx=%p, payload_type=%d, redundant_payload_type=%d, out_of_order_delay=%d)", timestamp_now(), codecp->vmprx, codecp->payload_type, codecp->redundant_payload_type, codecp->out_of_order_delay);
 sts = real_sm_vmprx_config_codec_rttext(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_silk(struct sm_vmprx_codec_silk_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_silk(vmprx=%p, payload_type=%d, plc_mode=%d)", timestamp_now(), codecp->vmprx, codecp->payload_type, codecp->plc_mode);
 sts = real_sm_vmprx_config_codec_silk(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_smv(struct sm_vmprx_codec_smv_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_smv(vmprx=%p, payload_type=%d, plc_mode=%d)", timestamp_now(), codecp->vmprx, codecp->payload_type, codecp->plc_mode);
 sts = real_sm_vmprx_config_codec_smv(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_speex(struct sm_vmprx_codec_speex_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_speex(vmprx=%p, payload_type=%d, nb_channels=%u, sampling_rate=%u, bandwidth=%d, plc_mode=%d)", timestamp_now(), codecp->vmprx, codecp->payload_type, codecp->nb_channels, codecp->sampling_rate, codecp->bandwidth, codecp->plc_mode);
 sts = real_sm_vmprx_config_codec_speex(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_speex_mode(struct sm_vmprx_codec_speex_mode_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_speex_mode(vmprx=%p, enh=%u)", timestamp_now(), codecp->vmprx, codecp->enh);
 sts = real_sm_vmprx_config_codec_speex_mode(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_sse(struct sm_vmprx_codec_sse_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_sse(vmprx=%p, payload_type=%d)", timestamp_now(), codecp->vmprx, codecp->payload_type);
 sts = real_sm_vmprx_config_codec_sse(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_tetra(struct sm_vmprx_codec_tetra_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec_tetra(vmprx=%p, payload_type=%d)", timestamp_now(), codecp->vmprx, codecp->payload_type);
 sts = real_sm_vmprx_config_codec_tetra(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_dataloss(struct sm_vmprx_dataloss_parms *datalossp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_dataloss(vmprx=%p, loss_threshold=%d)", timestamp_now(), datalossp->vmprx, datalossp->loss_threshold);
 sts = real_sm_vmprx_config_dataloss(datalossp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_encryption_aes_cm(struct sm_vmprx_config_encryption_aes_cm_parms *pp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_vmprx_config_encryption_aes_cm(vmprx=%p, keylen=%d", timestamp_now(), pp->vmprx, pp->keylen);
	show_key_reac_in(pp);
 }
 sts = real_sm_vmprx_config_encryption_aes_cm(pp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_encryption_aes_f8(struct sm_vmprx_config_encryption_aes_f8_parms *pp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_vmprx_config_encryption_aes_f8(vmprx=%p, keylen=%d", timestamp_now(), pp->vmprx, pp->keylen);
	show_key_rea8_in(pp);
 }
 sts = real_sm_vmprx_config_encryption_aes_f8(pp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_encryption_null(struct sm_vmprx_config_encryption_null_parms *pp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_encryption_null(vmprx=%p)", timestamp_now(), pp->vmprx);
 sts = real_sm_vmprx_config_encryption_null(pp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_forwarding(struct sm_vmprx_config_forwarding_parms *configp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_vmprx_config_forwarding(vmprx=%p, mode=%d, dest_type=%d, destination=" FORMAT_SOCKADDR_IN ", source=" FORMAT_SOCKADDR_IN ", TOS=%d, destination=" FORMAT_SOCKADDR_IN6 ", source=" FORMAT_SOCKADDR_IN6 ", num_types=%d", timestamp_now(), configp->vmprx, configp->mode, configp->dest_type, ARGS_SOCKADDR_IN(configp->u.ipv4.destination), ARGS_SOCKADDR_IN(configp->u.ipv4.source), configp->u.ipv4.TOS, ARGS_SOCKADDR_IN6(configp->u.ipv6.destination), ARGS_SOCKADDR_IN6(configp->u.ipv6.source), configp->num_types);
	show_fwd_types_in(configp);
 }
 sts = real_sm_vmprx_config_forwarding(configp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_jitter(struct sm_vmprx_jitter_parms *jitterp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_jitter(vmprx=%p, max_delay_ms=%u, initial_delay_ms=%u)", timestamp_now(), jitterp->vmprx, jitterp->max_delay_ms, jitterp->initial_delay_ms);
 sts = real_sm_vmprx_config_jitter(jitterp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_jitter_mode(struct sm_vmprx_config_jitter_mode_parms *jitterp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_jitter_mode(vmprx=%p, mode=%d, target_delay=%u, freq_upper_tolerance=%g, freq_lower_tolerance=%g)", timestamp_now(), jitterp->vmprx, jitterp->mode, jitterp->u.adaptive.target_delay, jitterp->u.adaptive.freq_upper_tolerance, jitterp->u.adaptive.freq_lower_tolerance);
 sts = real_sm_vmprx_config_jitter_mode(jitterp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_jitter_resync_notify(struct sm_vmprx_config_jitter_resync_notify_parms *notifyp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_jitter_resync_notify(vmprx=%p, enable=%d)", timestamp_now(), notifyp->vmprx, notifyp->enable);
 sts = real_sm_vmprx_config_jitter_resync_notify(notifyp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_profile_specific(struct sm_vmprx_config_profile_specific_parms *psp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_profile_specific(vmprx=%p, enable=%d, plugin='%s', paramlen=%d, paramval='%s')", timestamp_now(), psp->vmprx, psp->enable, psp->plugin, psp->paramlen, psp->paramval);
 sts = real_sm_vmprx_config_profile_specific(psp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_sample_rate(struct sm_vmprx_sample_rate_parms *sample_ratep)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_sample_rate(vmprx=%p, sample_rate=%u)", timestamp_now(), sample_ratep->vmprx, sample_ratep->sample_rate);
 sts = real_sm_vmprx_config_sample_rate(sample_ratep);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_sprt(struct sm_vmprx_config_sprt_parms *configp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_sprt(vmprx=%p, payload_type=%d, sprt=%p)", timestamp_now(), configp->vmprx, configp->payload_type, configp->sprt);
 sts = real_sm_vmprx_config_sprt(configp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_tones(struct sm_vmprx_tone_parms *tonep)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_tones(vmprx=%p, regen_tones=%d, detect_tones=%d, enforce_tone_spacing=%d)", timestamp_now(), tonep->vmprx, tonep->regen_tones, tonep->detect_tones, tonep->enforce_tone_spacing);
 sts = real_sm_vmprx_config_tones(tonep);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_unhandled_payload_reporting(struct sm_vmprx_unhandled_payload_reporting_parms *pp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_unhandled_payload_reporting(vmprx=%p, delay=%d)", timestamp_now(), pp->vmprx, pp->delay);
 sts = real_sm_vmprx_config_unhandled_payload_reporting(pp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_create(struct sm_vmprx_create_parms *vmprxp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_create(module=" TiNG_PRINTF_MODULE ", type=%d, address=" FORMAT_IN_ADDR ", ipv6_address=" FORMAT_IN6_ADDR ", mux_rtcp=%d)", timestamp_now(), vmprxp->module, vmprxp->type, ARGS_IN_ADDR(vmprxp->address), ARGS_IN6_ADDR(vmprxp->ipv6_address), vmprxp->mux_rtcp);
 sts = real_sm_vmprx_create(vmprxp);
 if (TiNGtrace > 1) olog(" == %d vmprx=%p\n", sts, vmprxp->vmprx);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_destroy(tSMVMPrxId vmprx)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_destroy(vmprx=%p)", timestamp_now(), vmprx);
 sts = real_sm_vmprx_destroy(vmprx);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_get_datafeed(struct sm_vmprx_datafeed_parms *datafeedp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_get_datafeed(vmprx=%p)", timestamp_now(), datafeedp->vmprx);
 sts = real_sm_vmprx_get_datafeed(datafeedp);
 if (TiNGtrace > 1) olog(" == %d datafeed=%p\n", sts, datafeedp->datafeed);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_get_event(struct sm_vmprx_event_parms *eventp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_get_event(vmprx=%p)", timestamp_now(), eventp->vmprx);
 sts = real_sm_vmprx_get_event(eventp);
 if (TiNGtrace > 1) olog(" == %d event=" FORMAT_EVENT "\n", sts, ARGS_EVENT(eventp->event));
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_get_ports(struct sm_vmprx_port_parms *portp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_get_ports(vmprx=%p, nowait=%u)", timestamp_now(), portp->vmprx, portp->nowait);
 sts = real_sm_vmprx_get_ports(portp);
 if (TiNGtrace > 1) olog(" == %d RTP_port=%d, RTCP_port=%d, address=" FORMAT_IN_ADDR ", ipv6_address=" FORMAT_IN6_ADDR "\n", sts, portp->RTP_port, portp->RTCP_port, ARGS_IN_ADDR(portp->address), ARGS_IN6_ADDR(portp->ipv6_address));
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_ice_stun_change_role(struct sm_vmprx_ice_stun_change_role_parms *rolep)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_ice_stun_change_role(vmprx=%p, local_role=%d)", timestamp_now(), rolep->vmprx, rolep->local_role);
 sts = real_sm_vmprx_ice_stun_change_role(rolep);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_ice_stun_config(struct sm_vmprx_ice_stun_config_parms *configp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_ice_stun_config(vmprx=%p, local_role=%d, local_ice_ufrag='%s', local_ice_ufrag_length=%u, local_ice_pwd='%s', local_ice_pwd_length=%u, role_tiebreaker=%" PRIu64 ")", timestamp_now(), configp->vmprx, configp->local_role, configp->local_ice_ufrag, configp->local_ice_ufrag_length, configp->local_ice_pwd, configp->local_ice_pwd_length, configp->role_tiebreaker);
 sts = real_sm_vmprx_ice_stun_config(configp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_ice_stun_remote_credentials(struct sm_vmprx_ice_stun_remote_credentials_parms *credentialsp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_ice_stun_remote_credentials(vmprx=%p, remote_ice_ufrag='%s', remote_ice_ufrag_length=%u, remote_ice_pwd='%s', remote_ice_pwd_length=%u)", timestamp_now(), credentialsp->vmprx, credentialsp->remote_ice_ufrag, credentialsp->remote_ice_ufrag_length, credentialsp->remote_ice_pwd, credentialsp->remote_ice_pwd_length);
 sts = real_sm_vmprx_ice_stun_remote_credentials(credentialsp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_ice_stun_send_bind_request(struct sm_vmprx_ice_stun_send_bind_request_parms *requestp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_vmprx_ice_stun_send_bind_request(vmprx=%p, use_RTCP_port=%d, ipv4=" FORMAT_SOCKADDR_IN ", ipv6=" FORMAT_SOCKADDR_IN6 ", priority=%u, use_candidate=%d", timestamp_now(), requestp->vmprx, requestp->use_RTCP_port, ARGS_SOCKADDR_IN(requestp->destination.ipv4), ARGS_SOCKADDR_IN6(requestp->destination.ipv6), requestp->priority, requestp->use_candidate);
	show_vmprx_stun_transaction_id_in(requestp);
 }
 sts = real_sm_vmprx_ice_stun_send_bind_request(requestp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_set_rtcphand(struct sm_vmprx_set_rtcphand_parms *rvp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_set_rtcphand(vmprx=%p, rtcphand=%p)", timestamp_now(), rvp->vmprx, rvp->rtcphand);
 sts = real_sm_vmprx_set_rtcphand(rvp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_status(struct sm_vmprx_status_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_status(vmprx=%p)", timestamp_now(), statusp->vmprx);
 sts = real_sm_vmprx_status(statusp);
 if (TiNGtrace > 1) {
	olog(" == %d status=%d, id=%d, volume=%g, duration=%u, RTP_Port=%d, RTCP_Port=%d, address=" FORMAT_IN_ADDR ", no_data=%d, address=" FORMAT_IN_ADDR ", port=%d, ssrc=%d, type=%d, payload_type=%d, RTP_Port=%d, RTCP_Port=%d, address=" FORMAT_IN6_ADDR ", address=" FORMAT_IN6_ADDR ", port=%d, ssrc=%d, recv_port=%d, address_type=%d, ipv4=" FORMAT_IN_ADDR ", ipv6=" FORMAT_IN6_ADDR ", port=%d, priority=%u, use_candidate=%d, remote_is_controlling=%d, recv_port=%d, result=%d, error_code=%d, address_type=%d, ipv4=" FORMAT_IN_ADDR ", ipv6=" FORMAT_IN6_ADDR ", port=%d, local_was_controlling=%d, timestamp=%u, localtimeref=%" PRIu64 ", notification_code=%d", sts, statusp->status, statusp->u.tone.id, statusp->u.tone.volume, statusp->u.tone.duration, statusp->u.ports.RTP_Port, statusp->u.ports.RTCP_Port, ARGS_IN_ADDR(statusp->u.ports.address), statusp->u.run.no_data, ARGS_IN_ADDR(statusp->u.ssrc.address), statusp->u.ssrc.port, statusp->u.ssrc.ssrc, statusp->u.payload.type, statusp->u.codec_specific.payload_type, statusp->u.ports_ipv6.RTP_Port, statusp->u.ports_ipv6.RTCP_Port, ARGS_IN6_ADDR(statusp->u.ports_ipv6.address), ARGS_IN6_ADDR(statusp->u.ssrc_ipv6.address), statusp->u.ssrc_ipv6.port, statusp->u.ssrc_ipv6.ssrc, statusp->u.bind_request.recv_port, statusp->u.bind_request.address_type, ARGS_IN_ADDR(statusp->u.bind_request.address.ipv4), ARGS_IN6_ADDR(statusp->u.bind_request.address.ipv6), statusp->u.bind_request.port, statusp->u.bind_request.priority, statusp->u.bind_request.use_candidate, statusp->u.bind_request.remote_is_controlling, statusp->u.bind_response.recv_port, statusp->u.bind_response.result, statusp->u.bind_response.error_code, statusp->u.bind_response.address_type, ARGS_IN_ADDR(statusp->u.bind_response.address.ipv4), ARGS_IN6_ADDR(statusp->u.bind_response.address.ipv6), statusp->u.bind_response.port, statusp->u.bind_response.local_was_controlling, statusp->u.jbresync.timestamp, statusp->u.jbresync.localtimeref, statusp->u.profile_specific.notification_code);
	show_vmprx_stun_result_transaction_id_out(statusp);
 }
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_status_codec_amrnb(struct sm_vmprx_status_codec_amrnb_parms *amrnbp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_status_codec_amrnb(vmprx=%p, payload_type=%d)", timestamp_now(), amrnbp->vmprx, amrnbp->payload_type);
 sts = real_sm_vmprx_status_codec_amrnb(amrnbp);
 if (TiNGtrace > 1) olog(" == %d status=%d, bitrate=%d\n", sts, amrnbp->status, amrnbp->u.cmr.bitrate);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_status_codec_amrwb(struct sm_vmprx_status_codec_amrwb_parms *amrwbp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_status_codec_amrwb(vmprx=%p, payload_type=%d)", timestamp_now(), amrwbp->vmprx, amrwbp->payload_type);
 sts = real_sm_vmprx_status_codec_amrwb(amrwbp);
 if (TiNGtrace > 1) olog(" == %d status=%d, bitrate=%d\n", sts, amrwbp->status, amrwbp->u.cmr.bitrate);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_status_codec_sse(struct sm_vmprx_status_codec_sse_parms *ssep)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_vmprx_status_codec_sse(vmprx=%p, payload_type=%d, max_length=%d", timestamp_now(), ssep->vmprx, ssep->payload_type, ssep->max_length);
	show_sse_payload_rx_in(ssep);
 }
 sts = real_sm_vmprx_status_codec_sse(ssep);
 if (TiNGtrace > 1) {
	olog(" == %d status=%d, length=%d", sts, ssep->status, ssep->length);
	show_sse_payload_rx_out(ssep);
 }
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_status_discard_codec_specific(tSMVMPrxId vmprx)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_status_discard_codec_specific(vmprx=%p)", timestamp_now(), vmprx);
 sts = real_sm_vmprx_status_discard_codec_specific(vmprx);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_status_profile_specific(struct sm_vmprx_status_profile_specific_parms *psp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_status_profile_specific(vmprx=%p, notification_code=%d, payload='%s', max_length=%d)", timestamp_now(), psp->vmprx, psp->notification_code, psp->payload, psp->max_length);
 sts = real_sm_vmprx_status_profile_specific(psp);
 if (TiNGtrace > 1) olog(" == %d payload='%s', length=%d\n", sts, psp->payload, psp->length);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_stop(struct sm_vmprx_stop_parms *stopp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_stop(vmprx=%p)", timestamp_now(), stopp->vmprx);
 sts = real_sm_vmprx_stop(stopp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config(struct sm_vmptx_config_parms *configp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config(vmptx=%p, destination_rtp=" FORMAT_SOCKADDR_IN ", source_rtp=" FORMAT_SOCKADDR_IN ", TOS_RTP=%d, destination_rtcp=" FORMAT_SOCKADDR_IN ", source_rtcp=" FORMAT_SOCKADDR_IN ", TOS_RTCP=%d)", timestamp_now(), configp->vmptx, ARGS_SOCKADDR_IN(configp->destination_rtp), ARGS_SOCKADDR_IN(configp->source_rtp), configp->TOS_RTP, ARGS_SOCKADDR_IN(configp->destination_rtcp), ARGS_SOCKADDR_IN(configp->source_rtcp), configp->TOS_RTCP);
 sts = real_sm_vmptx_config(configp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_authentication_hmac_sha1(struct sm_vmptx_config_authentication_hmac_sha1_parms *pp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_vmptx_config_authentication_hmac_sha1(vmptx=%p, keylen=%d, taglen=%d", timestamp_now(), pp->vmptx, pp->keylen, pp->taglen);
	show_key_tah_in(pp);
 }
 sts = real_sm_vmptx_config_authentication_hmac_sha1(pp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_authentication_null(struct sm_vmptx_config_authentication_null_parms *pp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_authentication_null(vmptx=%p)", timestamp_now(), pp->vmptx);
 sts = real_sm_vmptx_config_authentication_null(pp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_alaw(struct sm_vmptx_codec_alaw_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_alaw(vmptx=%p, payload_type=%d, VADMode=%d, ptime=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type, codecp->VADMode, codecp->ptime);
 sts = real_sm_vmptx_config_codec_alaw(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_amrnb(struct sm_vmptx_codec_amrnb_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_amrnb(vmptx=%p, payload_type=%d, aligned=%d, gsmefr=%d, VADMode=%d, frames_per_packet=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type, codecp->aligned, codecp->gsmefr, codecp->VADMode, codecp->frames_per_packet);
 sts = real_sm_vmptx_config_codec_amrnb(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_amrnb_cmr(struct sm_vmptx_config_codec_amrnb_cmr_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_amrnb_cmr(vmptx=%p, bitrate=%d)", timestamp_now(), codecp->vmptx, codecp->bitrate);
 sts = real_sm_vmptx_config_codec_amrnb_cmr(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_amrnb_mode(struct sm_vmptx_config_codec_amrnb_mode_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_amrnb_mode(vmptx=%p, bitrate=%d)", timestamp_now(), codecp->vmptx, codecp->bitrate);
 sts = real_sm_vmptx_config_codec_amrnb_mode(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_amrwb(struct sm_vmptx_codec_amrwb_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_amrwb(vmptx=%p, payload_type=%d, aligned=%d, VADMode=%d, frames_per_packet=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type, codecp->aligned, codecp->VADMode, codecp->frames_per_packet);
 sts = real_sm_vmptx_config_codec_amrwb(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_amrwb_cmr(struct sm_vmptx_config_codec_amrwb_cmr_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_amrwb_cmr(vmptx=%p, bitrate=%d)", timestamp_now(), codecp->vmptx, codecp->bitrate);
 sts = real_sm_vmptx_config_codec_amrwb_cmr(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_amrwb_mode(struct sm_vmptx_config_codec_amrwb_mode_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_amrwb_mode(vmptx=%p, bitrate=%d)", timestamp_now(), codecp->vmptx, codecp->bitrate);
 sts = real_sm_vmptx_config_codec_amrwb_mode(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_comfort_noise(struct sm_vmptx_codec_comfort_noise_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_comfort_noise(vmptx=%p, payload_type=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type);
 sts = real_sm_vmptx_config_codec_comfort_noise(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_evrc(struct sm_vmptx_codec_evrc_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_evrc(vmptx=%p, payload_type=%d, high_pass_filter=%d, noise_suppression_filter=%d, rtp_mode=%d, max_rate=%d, min_rate=%d, VADMode=%d, frames_per_packet=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type, codecp->high_pass_filter, codecp->noise_suppression_filter, codecp->rtp_mode, codecp->max_rate, codecp->min_rate, codecp->VADMode, codecp->frames_per_packet);
 sts = real_sm_vmptx_config_codec_evrc(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_g722(struct sm_vmptx_codec_g722_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_g722(vmptx=%p, payload_type=%d, packetlen=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type, codecp->packetlen);
 sts = real_sm_vmptx_config_codec_g722(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_g722_1(struct sm_vmptx_codec_g722_1_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_g722_1(vmptx=%p, payload_type=%d, bitrate=%d, frames_per_packet=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type, codecp->bitrate, codecp->frames_per_packet);
 sts = real_sm_vmptx_config_codec_g722_1(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_g723_1(struct sm_vmptx_codec_g723_1_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_g723_1(vmptx=%p, payload_type=%d, high_pass_filter=%d, rate=%d, silence_compression=%d, frames_per_packet=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type, codecp->high_pass_filter, codecp->rate, codecp->silence_compression, codecp->frames_per_packet);
 sts = real_sm_vmptx_config_codec_g723_1(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_g726(struct sm_vmptx_codec_g726_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_g726(vmptx=%p, payload_type=%d, bits=%d, VADMode=%d, packetlen=%d, variant=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type, codecp->bits, codecp->VADMode, codecp->packetlen, codecp->variant);
 sts = real_sm_vmptx_config_codec_g726(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_g728(struct sm_vmptx_codec_g728_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_g728(vmptx=%p, payload_type=%d, rate=%d, VADMode=%d, ptime=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type, codecp->rate, codecp->VADMode, codecp->ptime);
 sts = real_sm_vmptx_config_codec_g728(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_g729ab(struct sm_vmptx_codec_g729ab_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_g729ab(vmptx=%p, payload_type=%d, VADMode=%d, ptime=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type, codecp->VADMode, codecp->ptime);
 sts = real_sm_vmptx_config_codec_g729ab(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_g729i(struct sm_vmptx_codec_g729i_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_g729i(vmptx=%p, payload_type=%d, g729_mode=%d, VADMode=%d, ptime=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type, codecp->g729_mode, codecp->VADMode, codecp->ptime);
 sts = real_sm_vmptx_config_codec_g729i(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_gsmefr(struct sm_vmptx_codec_gsmefr_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_gsmefr(vmptx=%p, payload_type=%d, VADMode=%d, frames_per_packet=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type, codecp->VADMode, codecp->frames_per_packet);
 sts = real_sm_vmptx_config_codec_gsmefr(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_gsmfr(struct sm_vmptx_codec_gsmfr_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_gsmfr(vmptx=%p, payload_type=%d, variant=%d, VADMode=%d, frames_per_packet=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type, codecp->variant, codecp->VADMode, codecp->frames_per_packet);
 sts = real_sm_vmptx_config_codec_gsmfr(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_gsmhr(struct sm_vmptx_codec_gsmhr_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_gsmhr(vmptx=%p, payload_type=%d, VADMode=%d, frames_per_packet=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type, codecp->VADMode, codecp->frames_per_packet);
 sts = real_sm_vmptx_config_codec_gsmhr(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_ilbc(struct sm_vmptx_codec_ilbc_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_ilbc(vmptx=%p, payload_type=%d, frame_len=%d, VADMode=%d, frames_per_packet=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type, codecp->frame_len, codecp->VADMode, codecp->frames_per_packet);
 sts = real_sm_vmptx_config_codec_ilbc(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_isac(struct sm_vmptx_codec_isac_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_isac(vmptx=%p, payload_type=%d, partner=%p, channel_associated_mode=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type, codecp->partner, codecp->channel_associated_mode);
 sts = real_sm_vmptx_config_codec_isac(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_l16(struct sm_vmptx_codec_l16_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_l16(vmptx=%p, payload_type=%d, VADMode=%d, ptime=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type, codecp->VADMode, codecp->ptime);
 sts = real_sm_vmptx_config_codec_l16(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_l8(struct sm_vmptx_codec_l8_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_l8(vmptx=%p, payload_type=%d, VADMode=%d, ptime=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type, codecp->VADMode, codecp->ptime);
 sts = real_sm_vmptx_config_codec_l8(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_melpe(struct sm_vmptx_codec_melpe_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_melpe(vmptx=%p, payload_type=%d, ptime=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type, codecp->ptime);
 sts = real_sm_vmptx_config_codec_melpe(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_mulaw(struct sm_vmptx_codec_mulaw_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_mulaw(vmptx=%p, payload_type=%d, VADMode=%d, ptime=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type, codecp->VADMode, codecp->ptime);
 sts = real_sm_vmptx_config_codec_mulaw(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_opus(struct sm_vmptx_codec_opus_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_opus(vmptx=%p, payload_type=%d, sample_rate=%d, bit_rate=%d, ptime=%d, packet_loss=%d, complexity=%d, use_fec=%d, use_dtx=%d, rate_control=%d, signal_type_hint=%d, application_type=%d, disable_prediction=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type, codecp->sample_rate, codecp->bit_rate, codecp->ptime, codecp->packet_loss, codecp->complexity, codecp->use_fec, codecp->use_dtx, codecp->rate_control, codecp->signal_type_hint, codecp->application_type, codecp->disable_prediction);
 sts = real_sm_vmptx_config_codec_opus(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_opus_mode(struct sm_vmptx_config_codec_opus_mode_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_opus_mode(vmptx=%p, sample_rate=%d, bit_rate=%d, ptime=%d, packet_loss=%d, complexity=%d, use_fec=%d, use_dtx=%d, rate_control=%d, signal_type_hint=%d, application_type=%d, disable_prediction=%d)", timestamp_now(), codecp->vmptx, codecp->sample_rate, codecp->bit_rate, codecp->ptime, codecp->packet_loss, codecp->complexity, codecp->use_fec, codecp->use_dtx, codecp->rate_control, codecp->signal_type_hint, codecp->application_type, codecp->disable_prediction);
 sts = real_sm_vmptx_config_codec_opus_mode(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_rfc2833(struct sm_vmptx_codec_rfc2833_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_rfc2833(vmptx=%p, payload_type=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type);
 sts = real_sm_vmptx_config_codec_rfc2833(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_rfc4040(struct sm_vmptx_codec_rfc4040_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_rfc4040(vmptx=%p, payload_type=%d, packetlen=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type, codecp->packetlen);
 sts = real_sm_vmptx_config_codec_rfc4040(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_rttext(struct sm_vmptx_codec_rttext_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_rttext(vmptx=%p, payload_type=%d, redundant_payload_type=%d, redundancy_level=%d, buffer_time=%d, idle_time=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type, codecp->redundant_payload_type, codecp->redundancy_level, codecp->buffer_time, codecp->idle_time);
 sts = real_sm_vmptx_config_codec_rttext(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_silk(struct sm_vmptx_codec_silk_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_silk(vmptx=%p, payload_type=%d, sample_rate=%d, internal_rate=%d, bit_rate=%d, frames_per_packet=%d, packet_loss=%d, complexity=%d, use_fec=%d, use_dtx=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type, codecp->sample_rate, codecp->internal_rate, codecp->bit_rate, codecp->frames_per_packet, codecp->packet_loss, codecp->complexity, codecp->use_fec, codecp->use_dtx);
 sts = real_sm_vmptx_config_codec_silk(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_silk_mode(struct sm_vmptx_config_codec_silk_mode_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_silk_mode(vmptx=%p, sample_rate=%d, internal_rate=%d, bit_rate=%d, frames_per_packet=%d, packet_loss=%d, complexity=%d, use_fec=%d, use_dtx=%d)", timestamp_now(), codecp->vmptx, codecp->sample_rate, codecp->internal_rate, codecp->bit_rate, codecp->frames_per_packet, codecp->packet_loss, codecp->complexity, codecp->use_fec, codecp->use_dtx);
 sts = real_sm_vmptx_config_codec_silk_mode(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_smv(struct sm_vmptx_codec_smv_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_smv(vmptx=%p, payload_type=%d, VADMode=%d, frames_per_packet=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type, codecp->VADMode, codecp->frames_per_packet);
 sts = real_sm_vmptx_config_codec_smv(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_speex(struct sm_vmptx_codec_speex_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_speex(vmptx=%p, payload_type=%d, bandwidth=%d, VADMode=%d, frames_per_packet=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type, codecp->bandwidth, codecp->VADMode, codecp->frames_per_packet);
 sts = real_sm_vmptx_config_codec_speex(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_speex_mode(struct sm_vmptx_codec_speex_mode_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_speex_mode(vmptx=%p, complexity=%u, quality=%u, bitrate=%u, denoiser=%u)", timestamp_now(), codecp->vmptx, codecp->complexity, codecp->quality, codecp->bitrate, codecp->denoiser);
 sts = real_sm_vmptx_config_codec_speex_mode(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_sse(struct sm_vmptx_codec_sse_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_sse(vmptx=%p, payload_type=%d, retransmissions=%d, delay=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type, codecp->retransmissions, codecp->delay);
 sts = real_sm_vmptx_config_codec_sse(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_tetra(struct sm_vmptx_codec_tetra_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec_tetra(vmptx=%p, payload_type=%d)", timestamp_now(), codecp->vmptx, codecp->payload_type);
 sts = real_sm_vmptx_config_codec_tetra(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_encryption_aes_cm(struct sm_vmptx_config_encryption_aes_cm_parms *pp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_vmptx_config_encryption_aes_cm(vmptx=%p, keylen=%d", timestamp_now(), pp->vmptx, pp->keylen);
	show_key_teac_in(pp);
 }
 sts = real_sm_vmptx_config_encryption_aes_cm(pp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_encryption_aes_f8(struct sm_vmptx_config_encryption_aes_f8_parms *pp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_vmptx_config_encryption_aes_f8(vmptx=%p, keylen=%d", timestamp_now(), pp->vmptx, pp->keylen);
	show_key_tea8_in(pp);
 }
 sts = real_sm_vmptx_config_encryption_aes_f8(pp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_encryption_null(struct sm_vmptx_config_encryption_null_parms *pp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_encryption_null(vmptx=%p)", timestamp_now(), pp->vmptx);
 sts = real_sm_vmptx_config_encryption_null(pp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_ipv6(struct sm_vmptx_config_ipv6_parms *configp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_ipv6(vmptx=%p, destination_rtp=" FORMAT_SOCKADDR_IN6 ", source_rtp=" FORMAT_SOCKADDR_IN6 ", destination_rtcp=" FORMAT_SOCKADDR_IN6 ", source_rtcp=" FORMAT_SOCKADDR_IN6 ")", timestamp_now(), configp->vmptx, ARGS_SOCKADDR_IN6(configp->destination_rtp), ARGS_SOCKADDR_IN6(configp->source_rtp), ARGS_SOCKADDR_IN6(configp->destination_rtcp), ARGS_SOCKADDR_IN6(configp->source_rtcp));
 sts = real_sm_vmptx_config_ipv6(configp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_isac_rate(struct sm_vmptx_isac_rate_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_isac_rate(vmptx=%p, rate=%d, ptime=%d)", timestamp_now(), codecp->vmptx, codecp->rate, codecp->ptime);
 sts = real_sm_vmptx_config_isac_rate(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_profile_specific(struct sm_vmptx_config_profile_specific_parms *psp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_profile_specific(vmptx=%p, enable=%d, plugin='%s', paramlen=%d, paramval='%s')", timestamp_now(), psp->vmptx, psp->enable, psp->plugin, psp->paramlen, psp->paramval);
 sts = real_sm_vmptx_config_profile_specific(psp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_sample_rate(struct sm_vmptx_sample_rate_parms *sample_ratep)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_sample_rate(vmptx=%p, sample_rate=%u)", timestamp_now(), sample_ratep->vmptx, sample_ratep->sample_rate);
 sts = real_sm_vmptx_config_sample_rate(sample_ratep);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_tag(struct sm_vmptx_tag_parms *tagp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_tag(vmptx=%p, payload_type=%d, tag_type=%d, min_time=%u, max_time=%u)", timestamp_now(), tagp->vmptx, tagp->payload_type, tagp->tag_type, tagp->min_time, tagp->max_time);
 sts = real_sm_vmptx_config_tag(tagp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_tones(struct sm_vmptx_tone_parms *tonep)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_tones(vmptx=%p, convert_tones=%d, elim_tones=%d, tone_set_id=%p, min_duration=%d)", timestamp_now(), tonep->vmptx, tonep->convert_tones, tonep->elim_tones, tonep->tone_set_id, tonep->min_duration);
 sts = real_sm_vmptx_config_tones(tonep);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_create(struct sm_vmptx_create_parms *vmptxp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_create(module=" TiNG_PRINTF_MODULE ")", timestamp_now(), vmptxp->module);
 sts = real_sm_vmptx_create(vmptxp);
 if (TiNGtrace > 1) olog(" == %d vmptx=%p\n", sts, vmptxp->vmptx);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_create_csrc_list(struct sm_vmptx_create_csrc_list_parms *csrcp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_create_csrc_list(module=" TiNG_PRINTF_MODULE ")", timestamp_now(), csrcp->module);
 sts = real_sm_vmptx_create_csrc_list(csrcp);
 if (TiNGtrace > 1) olog(" == %d csrc_list=%p\n", sts, csrcp->csrc_list);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_create_toneset(struct sm_vmptx_create_toneset_parms *tonesetp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_create_toneset(toneset=%p, module=" TiNG_PRINTF_MODULE ")", timestamp_now(), tonesetp->toneset, tonesetp->module);
 sts = real_sm_vmptx_create_toneset(tonesetp);
 if (TiNGtrace > 1) olog(" == %d tone_set_id=%p\n", sts, tonesetp->tone_set_id);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_csrc_list_set(struct sm_vmptx_csrc_list_set_parms *csrcp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_vmptx_csrc_list_set(csrc_list=%p, num_csrc=%d", timestamp_now(), csrcp->csrc_list, csrcp->num_csrc);
	show_csrc_list_in(csrcp);
 }
 sts = real_sm_vmptx_csrc_list_set(csrcp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_datafeed_connect(struct sm_vmptx_datafeed_connect_parms *datafeedp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_datafeed_connect(data_source=%p, vmptx=%p)", timestamp_now(), datafeedp->data_source, datafeedp->vmptx);
 sts = real_sm_vmptx_datafeed_connect(datafeedp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_destroy(tSMVMPtxId vmptx)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_destroy(vmptx=%p)", timestamp_now(), vmptx);
 sts = real_sm_vmptx_destroy(vmptx);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_destroy_csrc_list(struct sm_vmptx_destroy_csrc_list_parms *csrcp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_destroy_csrc_list(csrc_list=%p)", timestamp_now(), csrcp->csrc_list);
 sts = real_sm_vmptx_destroy_csrc_list(csrcp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_destroy_toneset(struct sm_vmptx_destroy_toneset_parms *tonep)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_destroy_toneset(tone_set_id=%p)", timestamp_now(), tonep->tone_set_id);
 sts = real_sm_vmptx_destroy_toneset(tonep);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_generate_jitter(struct sm_vmptx_generate_jitter_parms *jitterp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_generate_jitter(vmptx=%p, external_jitter_threshold=%d, inter_jitter_gap=%d, added_jitter=%d, max_packet_age=%d)", timestamp_now(), jitterp->vmptx, jitterp->external_jitter_threshold, jitterp->inter_jitter_gap, jitterp->added_jitter, jitterp->max_packet_age);
 sts = real_sm_vmptx_generate_jitter(jitterp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_generate_tones(struct sm_vmptx_generate_tones_parms *tonep)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_vmptx_generate_tones(vmptx=%p, num=%d, duration=%d, interval=%d, volume=%d", timestamp_now(), tonep->vmptx, tonep->num, tonep->duration, tonep->interval, tonep->volume);
	show_gen_tones_in(tonep);
 }
 sts = real_sm_vmptx_generate_tones(tonep);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_generate_tones_abort(struct sm_vmptx_generate_tones_abort_parms *tonep)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_generate_tones_abort(vmptx=%p)", timestamp_now(), tonep->vmptx);
 sts = real_sm_vmptx_generate_tones_abort(tonep);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_get_event(struct sm_vmptx_event_parms *eventp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_get_event(vmptx=%p)", timestamp_now(), eventp->vmptx);
 sts = real_sm_vmptx_get_event(eventp);
 if (TiNGtrace > 1) olog(" == %d event=" FORMAT_EVENT "\n", sts, ARGS_EVENT(eventp->event));
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_propagate_rtcp_sr_ntp(struct sm_vmptx_propagate_rtcp_sr_ntp_parms *propp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_propagate_rtcp_sr_ntp(vmptx=%p, localtimeref=%" PRIu64 ", ntp_timestamp=%" PRIu64 ")", timestamp_now(), propp->vmptx, propp->localtimeref, propp->ntp_timestamp);
 sts = real_sm_vmptx_propagate_rtcp_sr_ntp(propp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_send_sse(struct sm_vmptx_send_sse_parms *sendssep)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_vmptx_send_sse(vmptx=%p, payload_length=%d", timestamp_now(), sendssep->vmptx, sendssep->payload_length);
	show_sse_payload_tx_in(sendssep);
 }
 sts = real_sm_vmptx_send_sse(sendssep);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_set_csrc(struct sm_vmptx_set_csrc_parms *csrcp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_vmptx_set_csrc(vmptx=%p, repeat_delay=%u, packet_count=%u, type=%d, num_csrc=%d, csrc_list_id=%p, exclude_ssrc=%d, ssrc=%d", timestamp_now(), csrcp->vmptx, csrcp->repeat_delay, csrcp->packet_count, csrcp->type, csrcp->u.csrc_array.num_csrc, csrcp->u.csrc_list.csrc_list_id, csrcp->u.csrc_list.exclude_ssrc, csrcp->u.csrc_list.ssrc);
	show_csrc_in(csrcp);
 }
 sts = real_sm_vmptx_set_csrc(csrcp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_set_profile_specific(struct sm_vmptx_set_profile_specific_parms *psp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_set_profile_specific(vmptx=%p, paramlen=%d, paramval='%s')", timestamp_now(), psp->vmptx, psp->paramlen, psp->paramval);
 sts = real_sm_vmptx_set_profile_specific(psp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_set_rtcphand(struct sm_vmptx_set_rtcphand_parms *rvp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_set_rtcphand(vmptx=%p, rtcphand=%p)", timestamp_now(), rvp->vmptx, rvp->rtcphand);
 sts = real_sm_vmptx_set_rtcphand(rvp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_status(struct sm_vmptx_status_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_status(vmptx=%p)", timestamp_now(), statusp->vmptx);
 sts = real_sm_vmptx_status(statusp);
 if (TiNGtrace > 1) olog(" == %d status=%d, ssrc=%d\n", sts, statusp->status, statusp->u.ssrc.ssrc);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_stop(struct sm_vmptx_stop_parms *stopp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_stop(vmptx=%p)", timestamp_now(), stopp->vmptx);
 sts = real_sm_vmptx_stop(stopp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_config_codec_rfc3984(struct sm_vidmprx_codec_rfc3984_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vidmprx_config_codec_rfc3984(vidmprx=%p, payload_type=%d, pic_size=%d)", timestamp_now(), codecp->vidmprx, codecp->payload_type, codecp->pic_size);
 sts = real_sm_vidmprx_config_codec_rfc3984(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_config_codec_rfc4629(struct sm_vidmprx_codec_rfc4629_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vidmprx_config_codec_rfc4629(vidmprx=%p, payload_type=%d, h263_profile=%d, h263_level=%d)", timestamp_now(), codecp->vidmprx, codecp->payload_type, codecp->h263_profile, codecp->h263_level);
 sts = real_sm_vidmprx_config_codec_rfc4629(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_config_dataloss(struct sm_vidmprx_dataloss_parms *datalossp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vidmprx_config_dataloss(vidmprx=%p, loss_threshold=%d)", timestamp_now(), datalossp->vidmprx, datalossp->loss_threshold);
 sts = real_sm_vidmprx_config_dataloss(datalossp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_config_jitter(struct sm_vidmprx_jitter_parms *jitterp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vidmprx_config_jitter(vidmprx=%p, max_delay_ms=%u, initial_delay_ms=%u)", timestamp_now(), jitterp->vidmprx, jitterp->max_delay_ms, jitterp->initial_delay_ms);
 sts = real_sm_vidmprx_config_jitter(jitterp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_config_unhandled_payload_reporting(struct sm_vidmprx_unhandled_payload_reporting_parms *pp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vidmprx_config_unhandled_payload_reporting(vidmprx=%p, delay=%d)", timestamp_now(), pp->vidmprx, pp->delay);
 sts = real_sm_vidmprx_config_unhandled_payload_reporting(pp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_create(struct sm_vidmprx_create_parms *vidmprxp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vidmprx_create(module=" TiNG_PRINTF_MODULE ", type=%d, address=" FORMAT_IN_ADDR ", ipv6_address=" FORMAT_IN6_ADDR ")", timestamp_now(), vidmprxp->module, vidmprxp->type, ARGS_IN_ADDR(vidmprxp->address), ARGS_IN6_ADDR(vidmprxp->ipv6_address));
 sts = real_sm_vidmprx_create(vidmprxp);
 if (TiNGtrace > 1) olog(" == %d vidmprx=%p\n", sts, vidmprxp->vidmprx);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_destroy(tSMVidMPrxId vidmprx)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vidmprx_destroy(vidmprx=%p)", timestamp_now(), vidmprx);
 sts = real_sm_vidmprx_destroy(vidmprx);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_get_datafeed(struct sm_vidmprx_datafeed_parms *datafeedp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vidmprx_get_datafeed(vidmprx=%p)", timestamp_now(), datafeedp->vidmprx);
 sts = real_sm_vidmprx_get_datafeed(datafeedp);
 if (TiNGtrace > 1) olog(" == %d datafeed=%p\n", sts, datafeedp->datafeed);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_get_event(struct sm_vidmprx_event_parms *eventp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vidmprx_get_event(vidmprx=%p)", timestamp_now(), eventp->vidmprx);
 sts = real_sm_vidmprx_get_event(eventp);
 if (TiNGtrace > 1) olog(" == %d event=" FORMAT_EVENT "\n", sts, ARGS_EVENT(eventp->event));
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_get_ports(struct sm_vidmprx_port_parms *portp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vidmprx_get_ports(vidmprx=%p, nowait=%u)", timestamp_now(), portp->vidmprx, portp->nowait);
 sts = real_sm_vidmprx_get_ports(portp);
 if (TiNGtrace > 1) olog(" == %d RTP_port=%d, RTCP_port=%d, address=" FORMAT_IN_ADDR ", ipv6_address=" FORMAT_IN6_ADDR "\n", sts, portp->RTP_port, portp->RTCP_port, ARGS_IN_ADDR(portp->address), ARGS_IN6_ADDR(portp->ipv6_address));
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_ice_stun_change_role(struct sm_vidmprx_ice_stun_change_role_parms *rolep)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vidmprx_ice_stun_change_role(vidmprx=%p, local_role=%d)", timestamp_now(), rolep->vidmprx, rolep->local_role);
 sts = real_sm_vidmprx_ice_stun_change_role(rolep);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_ice_stun_config(struct sm_vidmprx_ice_stun_config_parms *configp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vidmprx_ice_stun_config(vidmprx=%p, local_role=%d, local_ice_ufrag='%s', local_ice_ufrag_length=%u, local_ice_pwd='%s', local_ice_pwd_length=%u, role_tiebreaker=%" PRIu64 ")", timestamp_now(), configp->vidmprx, configp->local_role, configp->local_ice_ufrag, configp->local_ice_ufrag_length, configp->local_ice_pwd, configp->local_ice_pwd_length, configp->role_tiebreaker);
 sts = real_sm_vidmprx_ice_stun_config(configp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_ice_stun_remote_credentials(struct sm_vidmprx_ice_stun_remote_credentials_parms *credentialsp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vidmprx_ice_stun_remote_credentials(vidmprx=%p, remote_ice_ufrag='%s', remote_ice_ufrag_length=%u, remote_ice_pwd='%s', remote_ice_pwd_length=%u)", timestamp_now(), credentialsp->vidmprx, credentialsp->remote_ice_ufrag, credentialsp->remote_ice_ufrag_length, credentialsp->remote_ice_pwd, credentialsp->remote_ice_pwd_length);
 sts = real_sm_vidmprx_ice_stun_remote_credentials(credentialsp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_ice_stun_send_bind_request(struct sm_vidmprx_ice_stun_send_bind_request_parms *requestp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "sm_vidmprx_ice_stun_send_bind_request(vidmprx=%p, use_RTCP_port=%d, ipv4=" FORMAT_SOCKADDR_IN ", ipv6=" FORMAT_SOCKADDR_IN6 ", priority=%u, use_candidate=%d", timestamp_now(), requestp->vidmprx, requestp->use_RTCP_port, ARGS_SOCKADDR_IN(requestp->destination.ipv4), ARGS_SOCKADDR_IN6(requestp->destination.ipv6), requestp->priority, requestp->use_candidate);
	show_vidmprx_stun_transaction_id_in(requestp);
 }
 sts = real_sm_vidmprx_ice_stun_send_bind_request(requestp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_status(struct sm_vidmprx_status_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vidmprx_status(vidmprx=%p)", timestamp_now(), statusp->vidmprx);
 sts = real_sm_vidmprx_status(statusp);
 if (TiNGtrace > 1) {
	olog(" == %d status=%d, RTP_Port=%d, RTCP_Port=%d, address=" FORMAT_IN_ADDR ", no_data=%d, address=" FORMAT_IN_ADDR ", port=%d, ssrc=%d, type=%d, num_errors=%d, RTP_Port=%d, RTCP_Port=%d, address=" FORMAT_IN6_ADDR ", address=" FORMAT_IN6_ADDR ", port=%d, ssrc=%d, recv_port=%d, address_type=%d, ipv4=" FORMAT_IN_ADDR ", ipv6=" FORMAT_IN6_ADDR ", port=%d, priority=%u, use_candidate=%d, remote_is_controlling=%d, recv_port=%d, result=%d, error_code=%d, address_type=%d, ipv4=" FORMAT_IN_ADDR ", ipv6=" FORMAT_IN6_ADDR ", port=%d, local_was_controlling=%d", sts, statusp->status, statusp->u.ports.RTP_Port, statusp->u.ports.RTCP_Port, ARGS_IN_ADDR(statusp->u.ports.address), statusp->u.run.no_data, ARGS_IN_ADDR(statusp->u.ssrc.address), statusp->u.ssrc.port, statusp->u.ssrc.ssrc, statusp->u.payload.type, statusp->u.internal_error.num_errors, statusp->u.ports_ipv6.RTP_Port, statusp->u.ports_ipv6.RTCP_Port, ARGS_IN6_ADDR(statusp->u.ports_ipv6.address), ARGS_IN6_ADDR(statusp->u.ssrc_ipv6.address), statusp->u.ssrc_ipv6.port, statusp->u.ssrc_ipv6.ssrc, statusp->u.bind_request.recv_port, statusp->u.bind_request.address_type, ARGS_IN_ADDR(statusp->u.bind_request.address.ipv4), ARGS_IN6_ADDR(statusp->u.bind_request.address.ipv6), statusp->u.bind_request.port, statusp->u.bind_request.priority, statusp->u.bind_request.use_candidate, statusp->u.bind_request.remote_is_controlling, statusp->u.bind_response.recv_port, statusp->u.bind_response.result, statusp->u.bind_response.error_code, statusp->u.bind_response.address_type, ARGS_IN_ADDR(statusp->u.bind_response.address.ipv4), ARGS_IN6_ADDR(statusp->u.bind_response.address.ipv6), statusp->u.bind_response.port, statusp->u.bind_response.local_was_controlling);
	show_vidmprx_stun_result_transaction_id_out(statusp);
 }
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_stop(struct sm_vidmprx_stop_parms *stopp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vidmprx_stop(vidmprx=%p)", timestamp_now(), stopp->vidmprx);
 sts = real_sm_vidmprx_stop(stopp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_sync_media(struct sm_vidmprx_sync_media_parms *syncmediap)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vidmprx_sync_media(vidmprx=%p, vmprx=%p)", timestamp_now(), syncmediap->vidmprx, syncmediap->vmprx);
 sts = real_sm_vidmprx_sync_media(syncmediap);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vidmptx_config(struct sm_vidmptx_config_parms *configp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vidmptx_config(vidmptx=%p, destination_rtp=" FORMAT_SOCKADDR_IN ", source_rtp=" FORMAT_SOCKADDR_IN ", TOS_RTP=%d, destination_rtcp=" FORMAT_SOCKADDR_IN ", source_rtcp=" FORMAT_SOCKADDR_IN ", TOS_RTCP=%d)", timestamp_now(), configp->vidmptx, ARGS_SOCKADDR_IN(configp->destination_rtp), ARGS_SOCKADDR_IN(configp->source_rtp), configp->TOS_RTP, ARGS_SOCKADDR_IN(configp->destination_rtcp), ARGS_SOCKADDR_IN(configp->source_rtcp), configp->TOS_RTCP);
 sts = real_sm_vidmptx_config(configp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vidmptx_config_codec_rfc3984(struct sm_vidmptx_codec_rfc3984_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vidmptx_config_codec_rfc3984(vidmptx=%p, payload_type=%d, deaggregate=%d)", timestamp_now(), codecp->vidmptx, codecp->payload_type, codecp->deaggregate);
 sts = real_sm_vidmptx_config_codec_rfc3984(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vidmptx_config_codec_rfc4629(struct sm_vidmptx_codec_rfc4629_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vidmptx_config_codec_rfc4629(vidmptx=%p, payload_type=%d, rem_ext_pic_hdr=%d)", timestamp_now(), codecp->vidmptx, codecp->payload_type, codecp->rem_ext_pic_hdr);
 sts = real_sm_vidmptx_config_codec_rfc4629(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vidmptx_config_ipv6(struct sm_vidmptx_config_ipv6_parms *configp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vidmptx_config_ipv6(vidmptx=%p, destination_rtp=" FORMAT_SOCKADDR_IN6 ", source_rtp=" FORMAT_SOCKADDR_IN6 ", destination_rtcp=" FORMAT_SOCKADDR_IN6 ", source_rtcp=" FORMAT_SOCKADDR_IN6 ")", timestamp_now(), configp->vidmptx, ARGS_SOCKADDR_IN6(configp->destination_rtp), ARGS_SOCKADDR_IN6(configp->source_rtp), ARGS_SOCKADDR_IN6(configp->destination_rtcp), ARGS_SOCKADDR_IN6(configp->source_rtcp));
 sts = real_sm_vidmptx_config_ipv6(configp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vidmptx_create(struct sm_vidmptx_create_parms *vidmptxp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vidmptx_create(module=" TiNG_PRINTF_MODULE ")", timestamp_now(), vidmptxp->module);
 sts = real_sm_vidmptx_create(vidmptxp);
 if (TiNGtrace > 1) olog(" == %d vidmptx=%p\n", sts, vidmptxp->vidmptx);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vidmptx_datafeed_connect(struct sm_vidmptx_datafeed_connect_parms *datafeedp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vidmptx_datafeed_connect(data_source=%p, vidmptx=%p)", timestamp_now(), datafeedp->data_source, datafeedp->vidmptx);
 sts = real_sm_vidmptx_datafeed_connect(datafeedp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vidmptx_destroy(tSMVidMPtxId vidmptx)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vidmptx_destroy(vidmptx=%p)", timestamp_now(), vidmptx);
 sts = real_sm_vidmptx_destroy(vidmptx);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vidmptx_get_event(struct sm_vidmptx_event_parms *eventp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vidmptx_get_event(vidmptx=%p)", timestamp_now(), eventp->vidmptx);
 sts = real_sm_vidmptx_get_event(eventp);
 if (TiNGtrace > 1) olog(" == %d event=" FORMAT_EVENT "\n", sts, ARGS_EVENT(eventp->event));
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vidmptx_status(struct sm_vidmptx_status_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vidmptx_status(vidmptx=%p)", timestamp_now(), statusp->vidmptx);
 sts = real_sm_vidmptx_status(statusp);
 if (TiNGtrace > 1) olog(" == %d status=%d, ssrc=%d, num_errors=%d\n", sts, statusp->status, statusp->u.ssrc.ssrc, statusp->u.internal_error.num_errors);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vidmptx_stop(struct sm_vidmptx_stop_parms *stopp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vidmptx_stop(vidmptx=%p)", timestamp_now(), stopp->vidmptx);
 sts = real_sm_vidmptx_stop(stopp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec(struct sm_vmprx_codec_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmprx_config_codec(vmprx=%p, codec=%d, payload_type=%d, plc_mode=%d)", timestamp_now(), codecp->vmprx, codecp->codec, codecp->payload_type, codecp->plc_mode);
 sts = real_sm_vmprx_config_codec(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec(struct sm_vmptx_codec_parms *codecp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_vmptx_config_codec(vmptx=%p, codec=%d, payload_type=%d, VADMode=%d, ptime=%d)", timestamp_now(), codecp->vmptx, codecp->codec, codecp->payload_type, codecp->VADMode, codecp->ptime);
 sts = real_sm_vmptx_config_codec(codecp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}
