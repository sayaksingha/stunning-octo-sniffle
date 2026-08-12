/* sync_trace.i - tracing for unpublished APIs - synchroniser */

ACUAPI int ACUTiNG_WINAPI sm_sync_create(struct sm_sync_create_parms *pp)
{
 int sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog("sm_sync_create(m=%d)", pp->module);
 sts = real_sm_sync_create(pp);
 if (TiNGtrace > 1) olog(" == %d, synchroniser=%p\n", sts, pp->synchroniser);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_sync_add(struct sm_sync_add_parms *pp)
{
 int sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog("sm_sync_add(synchroniser=%p, channel=%p)",
 	pp->synchroniser, pp->channel);
 sts = real_sm_sync_add(pp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_sync_delete(struct sm_sync_delete_parms *pp)
{
 int sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog("sm_sync_delete(synchroniser=%p)", pp->synchroniser);
 sts = real_sm_sync_delete(pp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}
