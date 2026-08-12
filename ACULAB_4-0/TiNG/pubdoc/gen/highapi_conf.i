/* highapi_conf.i - Prosody high level conferencing trace code */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

ACUAPI tSMConference ACUTiNG_WINAPI sm_conference_create(void)
{
 tSMConference  sts;
 int sts1 = run_once(&run_once_init);
 if (sts1) return 0;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_conference_create()", timestamp_now());
 sts = real_sm_conference_create();
 if (TiNGtrace > 1) olog(" == %p\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_conference_add_party(struct sm_conference_add_party_parms *parms)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_conference_add_party(conf=%p, channelIn=%p, channelOut=%p, out_volume=%d, out_agc=%d, in_volume=%d, in_agc=%d)", timestamp_now(), parms->conf, parms->channelIn, parms->channelOut, parms->out_volume, parms->out_agc, parms->in_volume, parms->in_agc);
 sts = real_sm_conference_add_party(parms);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_conference_remove_party(struct sm_conference_remove_party_parms *parms)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_conference_remove_party(conf=%p, channelIn=%p, channelOut=%p)", timestamp_now(), parms->conf, parms->channelIn, parms->channelOut);
 sts = real_sm_conference_remove_party(parms);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI void ACUTiNG_WINAPI sm_conference_delete(tSMConference conf)
{
  
 int sts = run_once(&run_once_init);
 if (sts) return;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_conference_delete(conf=%p)", timestamp_now(), conf);
 real_sm_conference_delete(conf);
 if (TiNGtrace > 1) olog(" == void\n");
 return ;
}

ACUAPI int ACUTiNG_WINAPI sm_conference_info(struct sm_conference_info_parms *parms)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_conference_info(conf=%p, activeChannelList=%p)", timestamp_now(), parms->conf, parms->activeChannelList);
 sts = real_sm_conference_info(parms);
 if (TiNGtrace > 1) olog(" == %d activeChannelCount=%d\n", sts, parms->activeChannelCount);
 return sts;
}
