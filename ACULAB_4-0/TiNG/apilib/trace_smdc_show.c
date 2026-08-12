/* trace_smdc_show.c - helper functions for data communications API tracing */

#include "trace_smdc_show.h"
#include "trace.h"
#include <stdio.h>

LOCALDEF void show_rxtx_data_in(struct smdc_data_parms *pp)
{
 olog(", data=%p)", pp->data);
}

LOCALDEF void show_enc_in(struct smdc_encoding_parms *pp)
{
 enum {maxb = 30 } ;
 char enc[3 * maxb + sizeof("... ")];
 int i;
 for (i=0; i < pp->encoding_config_length; i++) {
	char *pos = enc+i*3;
	if (i >= maxb) {
		sprintf(pos, "... ");
		break;
	}
	sprintf(pos, "%02x ", i[(unsigned char *) pp->encoding_config_data]);
 }
 if (i) enc[i*3-1] = 0;
 else enc[0] = 0;
 olog(", enc_cfg=%s)", enc);
}

LOCALDEF void show_config_enc_in(struct smdc_channel_config_parms *pp)
{
 enum {maxb = 30 } ;
 char cfg[3 * maxb + sizeof("... ")];
 char enc[3 * maxb + sizeof("... ")];
 int i;
 for (i=0; i < pp->config_length; i++) {
	char *pos = cfg+i*3;
	if (i >= maxb) {
		sprintf(pos, "... ");
		break;
	}
	sprintf(pos, "%02x ", i[(unsigned char *) pp->config_data]);
 }
 if (i) cfg[i*3-1] = 0;
 else cfg[0] = 0;
 for (i=0; i < pp->encoding_config_length; i++) {
	char *pos = enc+i*3;
	if (i >= maxb) {
		sprintf(pos, "... ");
		break;
	}
	sprintf(pos, "%02x ", i[(unsigned char *) pp->encoding_config_data]);
 }
 if (i) enc[i*3-1] = 0;
 else enc[0] = 0;
 olog(", cfg=%s, enc_cfg=%s)", cfg, enc);
}

LOCALDEF void show_prefdat_sufdat_in(struct smdc_line_control_parms *pp)
{
 unsigned u;
 olog(", pref=(");
 for (u=0; ; u++) {
	if (u * 32 >= pp->prefix_bitlength) break;
	olog(" %08x", pp->prefix_data[u]);
 }
 olog(" ), suf=(");
 for (u=0; ; u++) {
	if (u * 32 >= pp->suffix_bitlength) break;
	olog(" %08x", pp->suffix_data[u]);
 }
 olog(" ))");
}
