/* trace_rtp_show.c - helper functions for RTP API tracing */

#include "trace_rtp_show.h"
#include "trace.h"
#include <stdio.h>

STATIC void hexstring(char *buf, unsigned buflen, unsigned char *kp, unsigned klen)
{
 while (buflen > 1) {
 	if (!klen--) {
 		*buf = 0;
 		return;
	}
 	sprintf(buf, "%02x", *kp++);
	buf += 2;
 	buflen -= 2;
 }
 buf[-1] = 0;
 buf[-2] = '-';
}

LOCALDEF void show_sdes_in(SM_RTCPHAND_CONFIG_SDES_PARMS *pp)
{
 olog(", string=%.*s)", pp->stringlen, pp->stringval);
}

LOCALDEF void show_rtcp_data_in(SM_RTCPHAND_GET_DATA_PARMS *pp)
{
 olog(", data=%p)", pp->data);
}

STATIC void showkey(char *kp, unsigned klen)
{
 char ks[512];
 hexstring(ks, sizeof(ks),(unsigned char *)kp, klen);
 olog(", key=%s)", ks);
}

LOCALDEF void show_key_rah_in(SM_VMPRX_CONFIG_AUTHENTICATION_HMAC_SHA1_PARMS *pp)
{
 showkey(pp->key, pp->keylen);
}

LOCALDEF void show_key_reac_in(SM_VMPRX_CONFIG_ENCRYPTION_AES_CM_PARMS *pp)
{
 showkey(pp->key, pp->keylen);
}

LOCALDEF void show_key_rea8_in(SM_VMPRX_CONFIG_ENCRYPTION_AES_F8_PARMS *pp)
{
 showkey(pp->key, pp->keylen);
}

LOCALDEF void show_key_tah_in(SM_VMPTX_CONFIG_AUTHENTICATION_HMAC_SHA1_PARMS *pp)
{
 showkey(pp->key, pp->keylen);
}

LOCALDEF void show_key_teac_in(SM_VMPTX_CONFIG_ENCRYPTION_AES_CM_PARMS *pp)
{
 showkey(pp->key, pp->keylen);
}

LOCALDEF void show_key_tea8_in(SM_VMPTX_CONFIG_ENCRYPTION_AES_F8_PARMS *pp)
{
 showkey(pp->key, pp->keylen);
}

LOCALDEF void show_gen_tones_in(SM_VMPTX_GENERATE_TONES_PARMS *pp)
{
 tSM_INT i;
 olog(", tones={");
 for (i=0; i < pp->num; i++) {
	olog(" %d", pp->tones[i]);
 }
 olog(" }");
}

LOCALDEF void show_csrc_in(SM_VMPTX_SET_CSRC_PARMS *pp)
{
 int i;
 if (pp->type == kSMVMPTxCSRCTypeArray) {
	olog(", csrc={");
	for (i=0; i < pp->u.csrc_array.num_csrc; i++) {
		olog(" %d", pp->u.csrc_array.csrc[i]);
	}
	olog(" }");
 }
}

LOCALDEF void show_csrc_list_in(SM_VMPTX_CSRC_LIST_SET_PARMS *pp)
{
 int i;
 olog(", csrc={");
 for (i=0; i < pp->num_csrc; i++) {
	olog(" %d", pp->csrc[i]);
 }
 olog(" }");
}

LOCALDEF void show_sse_payload_tx_in(SM_VMPTX_SEND_SSE_PARMS *pp)
{
 char ks[512];
 hexstring(ks, sizeof(ks),(unsigned char *)pp->payload, pp->payload_length);
 olog(", payload=%s)", ks);
}

LOCALDEF void show_sse_payload_rx_in(SM_VMPRX_STATUS_CODEC_SSE_PARMS *pp)
{
 olog(", payload=%p)", pp->payload);
}

LOCALDEF void show_sse_payload_rx_out(SM_VMPRX_STATUS_CODEC_SSE_PARMS *pp)
{
 char ks[512];
 hexstring(ks, sizeof(ks),(unsigned char *)pp->payload, pp->length);
 olog(", payload=%s\n", ks);
}

LOCALDEF void show_fwd_types_in(SM_VMPRX_CONFIG_FORWARDING_PARMS *pp)
{
 int i;
 olog(", types={");
 for (i=0; i < pp->num_types; i++) {
	olog(" {incoming_pt=%d, outgoinging_pt=%d, transcoding=%d},", pp->types[i].incoming_pt, pp->types[i].outgoing_pt, pp->types[i].transcoding);
 }
 olog(" }");

}

LOCALDEF void show_vmprx_stun_transaction_id_in(SM_VMPRX_ICE_STUN_SEND_BIND_REQUEST_PARMS *pp)
{
 char hs[2 * (sizeof(pp->transaction_id) + 1)];
 hexstring(hs, sizeof(hs),(unsigned char *)pp->transaction_id, sizeof(pp->transaction_id));
 olog(", transaction_id=%s)", hs);
}

LOCALDEF void show_vmprx_stun_result_transaction_id_out(SM_VMPRX_STATUS_PARMS *pp)
{
 char hs[2 * (sizeof(pp->u.bind_response.transaction_id) + 1)];
 hexstring(hs, sizeof(hs),(unsigned char *)pp->u.bind_response.transaction_id, sizeof(pp->u.bind_response.transaction_id));
 olog(", transaction_id=%s\n", hs);
}

LOCALDEF void show_vidmprx_stun_transaction_id_in(SM_VIDMPRX_ICE_STUN_SEND_BIND_REQUEST_PARMS *pp)
{
 char hs[2 * (sizeof(pp->transaction_id) + 1)];
 hexstring(hs, sizeof(hs),(unsigned char *)pp->transaction_id, sizeof(pp->transaction_id));
 olog(", transaction_id=%s)", hs);
}

LOCALDEF void show_vidmprx_stun_result_transaction_id_out(SM_VIDMPRX_STATUS_PARMS *pp)
{
 char hs[2 * (sizeof(pp->u.bind_response.transaction_id) + 1)];
 hexstring(hs, sizeof(hs),(unsigned char *)pp->u.bind_response.transaction_id, sizeof(pp->u.bind_response.transaction_id));
 olog(", transaction_id=%s\n", hs);
}
