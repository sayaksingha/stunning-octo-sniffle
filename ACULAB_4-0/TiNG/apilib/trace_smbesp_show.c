/* trace_smbesp_show.c - helper functions for speech API tracing */

#include "trace_smbesp_show.h"
#include "trace.h"

LOCALDEF void show_playrec_data_in(struct sm_ts_data_parms *pp)
{
 olog(", data=%p)", pp->data);
}

LOCALDEC void show_audio_data_in(struct sm_audio_data_parms *pp)
{
 olog(", data=%p)", pp->data);
}

LOCALDEF void show_play_cptone_in(struct sm_play_cptone_parms *pp)
{
 int i;
 olog(", cadences=[");
 for (i=0; i < pp->tone_count; i++) {
	olog(" id=%d,on=%d,off=%d",
		pp->cadences[i].tone_id,
		pp->cadences[i].on_cadence,
		pp->cadences[i].off_cadence);
 }
 olog(" ])");
}

LOCALDEF void show_ectest_filt_in(struct sm_condition_input_parms *pp)
{
 olog(", ectest_filt=%p)", pp->ectest_filt);
}

LOCALDEF void show_input_cptone_in(struct sm_input_cptone_parms *pp)
{
 int i;
 olog(", %d:{", pp->state_count);
 for (i=0; i < pp->state_count; i++) {
	struct sm_cptone_state *sp = &pp->states[i];
	olog(" [%d: %d..%d]",
		sp->freq_id,
		sp->minimum_cadence,
		sp->maximum_cadence);
 }
 olog(" })");
}

LOCALDEF void show_vocab_item_list_vocab_recog_list_vocab_params_in(struct sm_asr_listen_for_parms *pp)
{
 struct sm_asr_characteristics *sp = pp->specific_parameters;
 int i;
 if (sp) {
	olog(", vfr_max_frames=%d, vfr_diff_threshold=%g, pse_max_frames=%d, pse_min_frames=%d, vit_soft_threshold=%g, vit_hard_threshold=%g", sp->vfr_max_frames, sp->vfr_diff_threshold, sp->pse_max_frames, sp->pse_min_frames, sp->vit_soft_threshold, sp->vit_hard_threshold);
 }
 olog(", vocab{");
 for (i=0; i < pp->vocab_item_count; i++) {
	olog(" %d=>%d", pp->vocab_item_ids[i], pp->vocab_recog_ids[i]);
 }
 olog(" })");
}

#define arlen(x) (sizeof(x)/sizeof(*(x)))

LOCALDEF void show_conf_active_ids_out(struct sm_conf_prim_status_parms *statusp)
{
 unsigned i;
 olog(", active_inputs={");
 for (i=0; i < arlen(statusp->u.active_inputs.input); i++) {
	olog(" {%d, %d}", statusp->u.active_inputs.input[i].id, statusp->u.active_inputs.input[i].power);
 }
 olog(" }\n");
}



STATIC void configdatastring(char *buf, unsigned buflen, unsigned char *dp, unsigned dlen)
{
 while (buflen > 1) {
 	if (!dlen--) {
 		*buf = 0;
 		return;
	}
 	sprintf(buf, "%02x", *dp++);
	buf += 2;
 	buflen -= 2;
 }
 buf[-1] = 0;
 buf[-2] = '-';
}


LOCALDEF void show_pluginconfigdata_in(SM_CATSIG_LISTEN_FOR_PARMS *pp)
{
 char ds[512];
 configdatastring(ds, sizeof(ds),(unsigned char *)pp->catsig_config_data, pp->catsig_config_length);
 olog(", catsig_config_data=%s\n", ds);
}