/* highapi_playrec.i - Prosody high level FILE play/record trace code */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

ACUAPI int ACUTiNG_WINAPI sm_replay_file_start(struct sm_file_replay_parms *file_parms)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_replay_file_start(fd=%p, offset=%u, replay_parms=%p)", timestamp_now(), file_parms->fd, file_parms->offset, file_parms->replay_parms);
 sts = real_sm_replay_file_start(file_parms);
 if (TiNGtrace > 1) olog(" == %d status=%d, private_length=%u, completing=%d, data_length_in_buffer=%d, buffer='%s'\n", sts, file_parms->status, file_parms->private_length, file_parms->completing, file_parms->data_length_in_buffer, file_parms->buffer);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_replay_file_progress_istatus(SM_FILE_REPLAY_PARMS *file_parms, int initial_status)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_replay_file_progress_istatus(file_parms=%p, initial_status=%d)", timestamp_now(), file_parms, initial_status);
 sts = real_sm_replay_file_progress_istatus(file_parms, initial_status);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_replay_file_progress(SM_FILE_REPLAY_PARMS *file_parms)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_replay_file_progress(file_parms=%p)", timestamp_now(), file_parms);
 sts = real_sm_replay_file_progress(file_parms);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_replay_file_complete(SM_FILE_REPLAY_PARMS *file_parms)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_replay_file_complete(file_parms=%p)", timestamp_now(), file_parms);
 sts = real_sm_replay_file_complete(file_parms);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_replay_file_stop(SM_FILE_REPLAY_PARMS *file_parms, int nowait)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_replay_file_stop(file_parms=%p, nowait=%d)", timestamp_now(), file_parms, nowait);
 sts = real_sm_replay_file_stop(file_parms, nowait);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_record_file_start(struct sm_file_record_parms *file_parms)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_record_file_start(fd=%p, record_parms=%p)", timestamp_now(), file_parms->fd, file_parms->record_parms);
 sts = real_sm_record_file_start(file_parms);
 if (TiNGtrace > 1) olog(" == %d status=%d, private_length=%u, completing=%d, buffer='%s'\n", sts, file_parms->status, file_parms->private_length, file_parms->completing, file_parms->buffer);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_record_file_progress_istatus(SM_FILE_RECORD_PARMS *file_parms, int initial_status)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_record_file_progress_istatus(file_parms=%p, initial_status=%d)", timestamp_now(), file_parms, initial_status);
 sts = real_sm_record_file_progress_istatus(file_parms, initial_status);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_record_file_progress_ostatus(SM_FILE_RECORD_PARMS *file_parms, SM_RECORD_STATUS_PARMS *status_parms)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_record_file_progress_ostatus(file_parms=%p, status_parms=%p)", timestamp_now(), file_parms, status_parms);
 sts = real_sm_record_file_progress_ostatus(file_parms, status_parms);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_record_file_progress(SM_FILE_RECORD_PARMS *file_parms)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_record_file_progress(file_parms=%p)", timestamp_now(), file_parms);
 sts = real_sm_record_file_progress(file_parms);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_record_file_complete(SM_FILE_RECORD_PARMS *file_parms)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_record_file_complete(file_parms=%p)", timestamp_now(), file_parms);
 sts = real_sm_record_file_complete(file_parms);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}
#ifdef PROSODY_TiNG

ACUAPI int ACUTiNG_WINAPI sm_record_file_complete_tstatus(SM_FILE_RECORD_PARMS *file_parms, SM_RECORD_STATUS_PARMS *status_parms)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_record_file_complete_tstatus(file_parms=%p, status_parms=%p)", timestamp_now(), file_parms, status_parms);
 sts = real_sm_record_file_complete_tstatus(file_parms, status_parms);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}
#endif

ACUAPI int ACUTiNG_WINAPI sm_record_file_stop(SM_FILE_RECORD_PARMS *file_parms, int nowait)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_record_file_stop(file_parms=%p, nowait=%d)", timestamp_now(), file_parms, nowait);
 sts = real_sm_record_file_stop(file_parms, nowait);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}
