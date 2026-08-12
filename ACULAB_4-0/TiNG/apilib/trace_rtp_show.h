/* trace_rtp_show.h - helper functions for RTP API tracing */

#ifndef INCLUDED_TRACE_RTP_SHOW_H
#define INCLUDED_TRACE_RTP_SHOW_H

#include "smrtp.h"

LOCALDEC void show_sdes_in(SM_RTCPHAND_CONFIG_SDES_PARMS *pp);
LOCALDEC void show_rtcp_data_in(SM_RTCPHAND_GET_DATA_PARMS *pp);
LOCALDEC void show_key_rah_in(SM_VMPRX_CONFIG_AUTHENTICATION_HMAC_SHA1_PARMS *pp);
LOCALDEC void show_key_reac_in(SM_VMPRX_CONFIG_ENCRYPTION_AES_CM_PARMS *pp);
LOCALDEC void show_key_rea8_in(SM_VMPRX_CONFIG_ENCRYPTION_AES_F8_PARMS *pp);
LOCALDEC void show_key_tah_in(SM_VMPTX_CONFIG_AUTHENTICATION_HMAC_SHA1_PARMS *pp);
LOCALDEC void show_key_teac_in(SM_VMPTX_CONFIG_ENCRYPTION_AES_CM_PARMS *pp);
LOCALDEC void show_key_tea8_in(SM_VMPTX_CONFIG_ENCRYPTION_AES_F8_PARMS *pp);
LOCALDEC void show_gen_tones_in(SM_VMPTX_GENERATE_TONES_PARMS *pp);
LOCALDEC void show_csrc_in(SM_VMPTX_SET_CSRC_PARMS *pp);
LOCALDEC void show_csrc_list_in(SM_VMPTX_CSRC_LIST_SET_PARMS *pp);
LOCALDEC void show_sse_payload_tx_in(SM_VMPTX_SEND_SSE_PARMS *pp);
LOCALDEC void show_sse_payload_rx_in(SM_VMPRX_STATUS_CODEC_SSE_PARMS *pp);
LOCALDEC void show_sse_payload_rx_out(SM_VMPRX_STATUS_CODEC_SSE_PARMS *pp);
LOCALDEC void show_fwd_types_in(SM_VMPRX_CONFIG_FORWARDING_PARMS *pp);
LOCALDEC void show_vmprx_stun_transaction_id_in(SM_VMPRX_ICE_STUN_SEND_BIND_REQUEST_PARMS *pp);
LOCALDEC void show_vmprx_stun_result_transaction_id_out(SM_VMPRX_STATUS_PARMS *pp);
LOCALDEC void show_vidmprx_stun_transaction_id_in(SM_VIDMPRX_ICE_STUN_SEND_BIND_REQUEST_PARMS *pp);
LOCALDEC void show_vidmprx_stun_result_transaction_id_out(SM_VIDMPRX_STATUS_PARMS *pp);

#endif
