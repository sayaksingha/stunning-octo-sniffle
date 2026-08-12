/* pros_x.i - Prosody X trace code */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

ACUAPI int ACUTiNG_WINAPI sm_open_prosody_s_v3(struct sm_open_prosody_s_v3_parms *openp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_open_prosody_s_v3(ps_locator='%s', card_id=%d)", timestamp_now(), openp->ps_locator, openp->card_id);
 sts = real_sm_open_prosody_s_v3(openp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_open_prosody_x(struct sm_open_prosody_x_parms *openp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_open_prosody_x(px_locator='%s', card_id=%d)", timestamp_now(), openp->px_locator, openp->card_id);
 sts = real_sm_open_prosody_x(openp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}
