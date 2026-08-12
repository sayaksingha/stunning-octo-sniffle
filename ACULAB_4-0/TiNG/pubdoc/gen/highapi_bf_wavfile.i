/* highapi_bf_wavfile.i - Prosody WAV BFILE trace code */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

ACUAPI int ACUTiNG_WINAPI sm_replay_wav_bf_start(char *filename, SM_BFILE_REPLAY_PARMS *file_parms)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_replay_wav_bf_start(filename='%s', file_parms=%p)", timestamp_now(), filename, file_parms);
 sts = real_sm_replay_wav_bf_start(filename, file_parms);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_replay_wav_bf_close(SM_BFILE_REPLAY_PARMS *file_parms)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_replay_wav_bf_close(file_parms=%p)", timestamp_now(), file_parms);
 sts = real_sm_replay_wav_bf_close(file_parms);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_record_wav_bf_start(char *filename, SM_BFILE_RECORD_PARMS *file_parms)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_record_wav_bf_start(filename='%s', file_parms=%p)", timestamp_now(), filename, file_parms);
 sts = real_sm_record_wav_bf_start(filename, file_parms);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_record_wav_bf_close(SM_BFILE_RECORD_PARMS *file_parms)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_record_wav_bf_close(file_parms=%p)", timestamp_now(), file_parms);
 sts = real_sm_record_wav_bf_close(file_parms);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_record_wav_bf_trim_close(SM_BFILE_RECORD_PARMS *file_parms, tSM_UT32 trimLength)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_record_wav_bf_trim_close(file_parms=%p, trimLength=%u)", timestamp_now(), file_parms, trimLength);
 sts = real_sm_record_wav_bf_trim_close(file_parms, trimLength);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_replay_wav_bf_get_type(char *filename, int *replay_type)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_replay_wav_bf_get_type(filename='%s', replay_type=%p)", timestamp_now(), filename, replay_type);
 sts = real_sm_replay_wav_bf_get_type(filename, replay_type);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}
