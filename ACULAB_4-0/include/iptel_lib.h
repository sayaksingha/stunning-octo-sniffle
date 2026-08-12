/*****************************************************************************\
*
* IPTEL_LIB.H : V6 IP telephony API Declarations
*
* (C)2002-2005 Aculab Plc
*
* Version 6.?
*
\*****************************************************************************/

#ifndef IPTEL_LIB_H__
#define IPTEL_LIB_H__

#include "acu_type.h"
#include "cl_lib.h"
#include "res_lib.h"

#ifdef _WIN32
#pragma pack (push, 1 )
#endif

#ifdef __cplusplus
extern "C"{
#endif

/* SIP transport types */
#define ACU_SIP_TRANSPORT_UDP 0
#define ACU_SIP_TRANSPORT_TCP 1
#define ACU_SIP_TRANSPORT_TLS 2
#define ACU_SIP_TRANSPORT_ANY 255
/* For backwards compatibility */
#define ACUUDP ACU_SIP_TRANSPORT_UDP
#define ACUTCP ACU_SIP_TRANSPORT_TCP
#define ACUTLS ACU_SIP_TRANSPORT_TLS

#define MAXALIAS 512 /* maximum alias string length */
#define MAXLISTENPARAMS 5 /* maximum number of listen parameters to be set in the SIP service */
#define MAXREGADDR 512
#define MAXFIRMWARE 2048
#define MAXETHERNET 6
#define IPT_MAX_DESCR 50

/* Call events specific to IP telephony */
typedef enum tIPT_CALL_EVENT {
  EV_ADD_ALIAS_SUCCEEDED = 1,
  EV_ADD_ALIAS_FAILED = 2,
  EV_ALIAS_REMOVED = 3,
  EV_IPT_SYSTEM_NON_STANDARD = 4
} IPT_EVENT;

/* Defines to enable zero defaulting */
typedef enum tIPT_TRISTATE {
  IPT_DEFAULT = 0,
  IPT_ENABLED = 1,
  IPT_DISABLED = 2
} IPT_TRISTATE;

/* IP Telephony card events. */
typedef enum tIPT_CARD_EVENT {
  IPT_CARD_NO_EVENT = 0,
  IPT_CARD_EV_IN_SERVICE = 1,
  IPT_CARD_EV_OUT_OF_SERVICE = 2,
  IPT_CARD_EV_L1_CHANGE = 3,
  IPT_CARD_EV_SWITCH_CLOCKING = 4,
  IPT_CARD_EV_FIRMWARE_STOPPED = 5,
  IPT_CARD_EV_CODECS_CHANGED = 6,
  IPT_CARD_EV_FIRMWARE_RUNNING = 7,
  IPT_CARD_EV_MEDIA_DEFAULTS_CHANGED = 8
} IPT_CARD_EVENT;

typedef enum tIPT_VALIDITY {
  IPT_INDETERMINATE = 0,
  IPT_VALID = 1,
  IPT_INVALID = 2
} IPT_VALIDITY;

typedef enum tIPT_H323_PROTOCOL_TYPE {
  IPT_H225_RAS_PROTOCOL = 0,
  IPT_H225_PROTOCOL = 1,
  IPT_RAS_PROTOCOL = 2
} IPT_H323_PROTOCOL_TYPE;

typedef enum tIPT_H323_NSM_TYPE {
  IPT_NSM_RAS = 1,
  IPT_NSM_XRS = 2,
  IPT_CONNECTIONLESS_FACILITY = 3
} IPT_H323_NSM_TYPE;

typedef enum tIPT_SIP_FAILURE_CODE {
  IPT_SIP_NO_ERROR = 0,
  IPT_SIP_REQUEST_TIMEOUT = 1,
  IPT_SIP_TCP_CONNECT_FAILED = 2,
  IPT_SIP_SSL_ERROR = 3,
  IPT_SIP_SSL_PEER_CERT_NOT_TRUSTED = 4,
  IPT_SIP_SSL_PEER_CERT_INVALID = 5,
  IPT_SIP_BAD_RESPONSE = 6
} IPT_SIP_FAILURE_CODE;

/* IP Telephony Registration Mode Defines */
#define IPT_REG_MODE_GATEWAY	0
#define IPT_REG_MODE_TERMINAL	1

#define IPT_MAX_MAC 20

typedef struct tIPT_CODEC_INFO
{
	ACU_INT	codec_type;
	ACU_INT	channels_supported;
	ACU_INT	channels_free;
} ACU_PACK_DIRECTIVE IPT_CODEC_INFO;

typedef struct tIPT_ETHERNET_INFO {
	ACU_INT	active;
	ACU_INT	connected;
	ACU_INT	speed;
	ACU_INT	duplex;
} ACU_PACK_DIRECTIVE IPT_ETHERNET_INFO;

/* structure for defining a host machine */
typedef struct host_details
{
	ACU_CHAR address[MAXHOSTADDRESS];
	ACU_UINT port;
	ACU_INT  transport_type;
} ACU_PACK_DIRECTIVE HOST_DETAILS;

/* structure for setting and querying SIP proxy */
typedef struct sip_proxy
{
	ACU_ULONG size;
	HOST_DETAILS proxy;
	ACU_INT disable_insertion_into_routeset;
} ACU_PACK_DIRECTIVE SIP_PROXY;

/* H323 register xparms*/
typedef struct register_xparms 
{
	ACU_ULONG size;
	ACU_INT ttl;
	ACU_CHAR registration_address[MAXREGADDR];
	ACU_CHAR gatekeeper_id[MAXID];
	ACU_INT registration_mode;
}ACU_PACK_DIRECTIVE REGISTER_XPARMS;

typedef struct query_registration_xparms{
	ACU_ULONG size;	
	ACU_INT registration_status;
	ACU_CHAR registration_address[MAXREGADDR];
}QUERY_REGISTRATION_XPARMS;

/* unique SIP add alias */
typedef struct sip_add_alias
{
	HOST_DETAILS registrar;
	ACU_CHAR     admin_sip_url[MAXALIAS];
	ACU_CHAR     contact[MAXALIAS];
} ACU_PACK_DIRECTIVE SIP_ADD_ALIAS;

/* structure for adding aliases */
typedef struct add_alias_xparms
{
	ACU_ULONG size;
	ACU_INT   protocol;
	ACU_UINT  registration_handle;
	ACU_CHAR  alias[MAXALIAS];
	union
	{
		struct
		{
			SIP_ADD_ALIAS sip_add_alias;
		}sig_sip;

		struct
		{
			ACU_INT prefix;
		}sig_h323;
	}unique_xparms;
} ACU_PACK_DIRECTIVE ADD_ALIAS_XPARMS;

/* structure for removing and deleting aliases */
typedef struct remove_alias_xparms
{
	ACU_ULONG size;
	ACU_UINT  protocol;
	ACU_UINT  registration_handle;
}ACU_PACK_DIRECTIVE REMOVE_ALIAS_XPARMS;

/* structure for querying aliases */
typedef struct query_alias_xparms
{
	ACU_ULONG size;
	ACU_UINT  protocol;
	ACU_UINT  registration_handle;
	ACU_CHAR  alias[MAXALIAS];
        ACU_INT   prefix;
	ACU_UINT  state;
	ACU_UINT  error;
} ACU_PACK_DIRECTIVE QUERY_ALIAS_XPARMS;

/* structure for querying set of registered aliases */
typedef struct snapshot_registrations_xparms
{
	ACU_ULONG size;
	ACU_UINT  protocol;
	ACU_UINT  count;
	ACU_UINT  handles[MAXREGISTRATIONS];
} ACU_PACK_DIRECTIVE SNAPSHOT_REGISTRATIONS_XPARMS;

/* setting up listen details*/
typedef struct 
{
	ACU_ULONG    size;
	HOST_DETAILS listen_details;
}ACU_PACK_DIRECTIVE LISTEN_XPARMS;

typedef struct listen_list_parms
{
	ACU_ULONG					size;
	ACU_UINT					num_of_listen_params;
	HOST_DETAILS				listen_details[MAXLISTENPARAMS];
}ACU_PACK_DIRECTIVE LISTEN_LIST_XPARMS;

typedef struct
{
        ACU_ULONG       size;
        ACU_CALL_HANDLE handle;
        ACU_UINT        enable_event_notification;
        ACU_UINT        relay_disabled;
}ACU_PACK_DIRECTIVE DTMF_HANDLING_XPARMS;

typedef	struct
{
	ACU_ULONG		size;
	ACU_UINT		protocol;
	ACU_CODEC		default_codecs[MAXCODECS];
	union
	{
		struct
		{
			ACU_INT	 h245_tunneling;
			ACU_INT	 faststart;
			ACU_INT  early_h245;
		}sig_h323;
		struct
		{
			ACU_INT  zero_connection_address_hold;
			ACU_INT  disable_early_media;
		}sig_sip;
	}unique_xparms;
}ACU_PACK_DIRECTIVE	PROTOCOL_DEFAULTS_XPARMS;

typedef struct 
{
	ACU_ULONG	size;
	ACU_CARD_ID	card_id;	/* IN */
} ACU_PACK_DIRECTIVE ACU_OPEN_IPTEL_PARMS;

typedef struct 
{
	ACU_ULONG	size;
	ACU_CARD_ID	card_id; /* IN */
} ACU_PACK_DIRECTIVE ACU_CLOSE_IPTEL_PARMS;

typedef struct
{
	ACU_ULONG       size;
	ACU_CARD_ID     card_id;
	ACU_CHAR        address[MAXHOSTADDRESS];
	ACU_CHAR        netmask[MAXHOSTADDRESS];
	ACU_CHAR        gateway[MAXHOSTADDRESS];
} ACU_PACK_DIRECTIVE IPT_CARD_CONFIGURE_XPARMS;

typedef struct
{
	ACU_ULONG       size;
	ACU_CARD_ID     card_id;
	ACU_CHAR        firmware[MAXFIRMWARE];
	ACU_CHAR        options[MAXFIRMWARE];
} ACU_PACK_DIRECTIVE IPT_CARD_DOWNLOAD_XPARMS;

typedef struct
{
	ACU_ULONG         size;
	ACU_CARD_ID       card_id;
	ACU_INT           active;
	ACU_CHAR          address[MAXHOSTADDRESS];
	ACU_INT           firmware_running;
	ACU_INT           clock;
	ACU_INT           codec_count;
	IPT_CODEC_INFO    codecs[MAXCODECS];
	ACU_INT           ethernet_ports;
	IPT_ETHERNET_INFO ethernet_status[MAXETHERNET];
	ACU_CHAR          dsp_module_model[ACU_MAX_HWVER];
	ACU_CHAR          dsp_module_serial_no[MAX_RESOURCE_SERIAL];
	ACU_CHAR          firmware_version[IPT_MAX_DESCR];
	IPT_VALIDITY      switch_clocking;
} ACU_PACK_DIRECTIVE IPT_CARD_INFO_XPARMS;

typedef struct
{
	ACU_ULONG         size;
	ACU_CARD_ID       card_id;
	ACU_WAIT_OBJECT   wait_object;
} ACU_PACK_DIRECTIVE IPT_WAIT_OBJECT_PARMS;

typedef struct
{
	ACU_ULONG         size;
	ACU_CARD_ID       card_id;
	ACU_UINT          event;
} ACU_PACK_DIRECTIVE IPT_CARD_NOTIFICATION_PARMS;

typedef struct tMEDIA_DEFAULTS
{
	ACU_ULONG               size;
	ACU_CARD_ID             card_id;
	ACU_INT			tdm_encoding;
	ACU_INT			encode_gain;
	ACU_INT			decode_gain;
	ACU_INT                 reserved;
	ACU_INT			echo_cancellation;
	ACU_INT			echo_suppression;
	ACU_INT			echo_span;
	ACU_UINT		rtp_tos;
	ACU_UINT		rtcp_tos;
	ACU_UINT                def_jitter;
	ACU_UINT                max_jitter;
	ACU_UINT                max_jitter_buffer;
	ACU_UINT		dtmf_detector;
}ACU_PACK_DIRECTIVE IPT_MEDIA_DEFAULTS_XPARMS;

typedef struct
{
	ACU_ULONG       size;
	ACU_CARD_ID     card_id;
	ACU_INT         firmware;
} ACU_PACK_DIRECTIVE IPT_CARD_TRACE_MODE_PARMS;

typedef struct
{
	ACU_ULONG       size;
        ACU_CARD_ID     card_id;
        
	ACU_INT         mtu;
	ACU_INT         speed;
	ACU_CHAR        hw_addr[IPT_MAX_MAC];
	ACU_INT         oper_status;
	ACU_UINT        in_octets;
	ACU_UINT        in_ucast_pkts;
	ACU_UINT        in_nucast_pkts;
	ACU_UINT        in_discards;
	ACU_UINT        in_errors;
	ACU_UINT        in_unknown_protos;
	ACU_UINT        out_octets;
	ACU_UINT        out_ucast_pkts;
	ACU_UINT        out_nucast_pkts;
	ACU_UINT        out_discards;
	ACU_UINT        out_errors;
} ACU_PACK_DIRECTIVE IPT_CARD_IF_STATS_XPARMS;

typedef struct
{
        ACU_ULONG       size;
	ACU_CHAR        h225_address[MAXHOSTADDRESS];
	ACU_UINT        h225_port;
        ACU_CHAR        ras_address[MAXHOSTADDRESS];
	ACU_UINT        ras_port;
} ACU_PACK_DIRECTIVE H323_LISTEN_ADDRESSES_XPARMS;

typedef struct
{
        ACU_ULONG       size;
	ACU_INT         protocol;
} ACU_PACK_DIRECTIVE H323_STOP_LISTEN_XPARMS;

typedef struct
{
	ACU_ULONG	size;
	ACU_INT         message_type;
        ADDRESSED_NON_STANDARD_DATA_XPARMS data;
} ACU_PACK_DIRECTIVE H323_NON_STANDARD_XPARMS;

typedef struct
{
	ACU_ULONG	size;
	ACU_INT         message_type;
	ACU_INT         enable;
} ACU_PACK_DIRECTIVE H323_NON_STANDARD_ENABLE_XPARMS;

/* structure for implementing application failover resilience */
typedef struct tREGISTRATION_FAILOVER_XPARMS
{
	ACU_ULONG       size;
	ACU_UINT        registration_handle;
	ACU_FAILOVER_ID failover_id;
} ACU_PACK_DIRECTIVE REGISTRATION_FAILOVER_XPARMS;

ACU_ERR ACU_EXPORT ipt_set_sip_proxy(SIP_PROXY *proxy_detailsp);
ACU_ERR ACU_EXPORT ipt_query_sip_proxy(SIP_PROXY *proxy_queryp);
ACU_ERR ACU_EXPORT ipt_clear_sip_proxy(void);

ACU_ERR ACU_EXPORT ipt_set_h323_gatekeeper(REGISTER_XPARMS *regdetailsp);
ACU_ERR ACU_EXPORT ipt_query_h323_gatekeeper(QUERY_REGISTRATION_XPARMS *queryp);
ACU_ERR ACU_EXPORT ipt_clear_h323_gatekeeper(void);

ACU_ERR ACU_EXPORT ipt_add_alias(ADD_ALIAS_XPARMS *add_aliasp);
ACU_ERR ACU_EXPORT ipt_remove_alias(REMOVE_ALIAS_XPARMS *remove_aliasp);
ACU_ERR ACU_EXPORT ipt_delete_alias(REMOVE_ALIAS_XPARMS *delete_aliasp);
ACU_ERR ACU_EXPORT ipt_query_alias(QUERY_ALIAS_XPARMS *query_aliasp);
ACU_ERR ACU_EXPORT ipt_snapshot_registrations(SNAPSHOT_REGISTRATIONS_XPARMS *snapshotp);

ACU_ERR ACU_EXPORT ipt_add_listen_address(LISTEN_XPARMS *listen_detailsp);
ACU_ERR ACU_EXPORT ipt_remove_listen_address(LISTEN_XPARMS *listen_detailsp);
ACU_ERR ACU_EXPORT ipt_get_listen_list(LISTEN_LIST_XPARMS *listen_listp);

ACU_INT ACU_EXPORT ipt_translate_h225rcr(ACU_INT h225rcr);	
ACU_ERR ACU_EXPORT ipt_set_dtmf_handling(DTMF_HANDLING_XPARMS *dtmf_handlingp);

ACU_ERR ACU_EXPORT ipt_set_protocol_defaults(PROTOCOL_DEFAULTS_XPARMS *protocol_defaultsp);
ACU_ERR ACU_EXPORT ipt_get_protocol_defaults(PROTOCOL_DEFAULTS_XPARMS *protocol_defaultsp);

ACU_ERR ACU_EXPORT acu_open_ipt(ACU_OPEN_IPTEL_PARMS* openp);
ACU_ERR ACU_EXPORT acu_close_ipt(ACU_CLOSE_IPTEL_PARMS* closep);

ACU_ERR ACU_EXPORT ipt_card_configure(IPT_CARD_CONFIGURE_XPARMS *configp);
ACU_ERR ACU_EXPORT ipt_card_download(IPT_CARD_DOWNLOAD_XPARMS *downloadp);

ACU_ERR ACU_EXPORT ipt_card_info(IPT_CARD_INFO_XPARMS *infop);
ACU_ERR ACU_EXPORT ipt_card_get_media_defaults(IPT_MEDIA_DEFAULTS_XPARMS *mdp);
ACU_ERR ACU_EXPORT ipt_card_set_media_defaults(IPT_MEDIA_DEFAULTS_XPARMS *mdp);
ACU_ERR ACU_EXPORT ipt_card_get_if_stats(IPT_CARD_IF_STATS_XPARMS *statsp);

ACU_ERR ACU_EXPORT ipt_card_get_app_context_token(ACU_APP_CONTEXT_TOKEN_PARMS* token_parms);
ACU_ERR ACU_EXPORT ipt_card_set_app_context_token(ACU_APP_CONTEXT_TOKEN_PARMS* token_parms);

ACU_ERR ACU_EXPORT ipt_card_notification_get_wait_object(IPT_WAIT_OBJECT_PARMS *woparms);
ACU_ERR ACU_EXPORT ipt_get_card_notification(IPT_CARD_NOTIFICATION_PARMS *parms);
ACU_ERR ACU_EXPORT ipt_card_set_notification_queue(ACU_QUEUE_PARMS* queue_parms);

ACU_ERR ACU_EXPORT ipt_card_get_trace(V6_TRACE_BUFFER *trace);
ACU_ERR ACU_EXPORT ipt_card_set_trace_mode(IPT_CARD_TRACE_MODE_PARMS *modep);
ACU_ERR ACU_EXPORT ipt_set_h323_listen_addresses(H323_LISTEN_ADDRESSES_XPARMS* listenp);
ACU_ERR ACU_EXPORT ipt_get_h323_listen_addresses(H323_LISTEN_ADDRESSES_XPARMS* listenp);
ACU_ERR ACU_EXPORT ipt_h323_stop_listening(H323_STOP_LISTEN_XPARMS* listenp);
ACU_ERR ACU_EXPORT ipt_h323_send_non_standard(H323_NON_STANDARD_XPARMS* nsp);
ACU_ERR ACU_EXPORT ipt_h323_get_non_standard(H323_NON_STANDARD_XPARMS* nsp);
ACU_ERR ACU_EXPORT ipt_h323_enable_non_standard(H323_NON_STANDARD_ENABLE_XPARMS* enablep);

/* unsupported */
ACU_ERR ACU_DEPRECATED ACU_EXPORT ipt_get_registration_failover_id(REGISTRATION_FAILOVER_XPARMS* reg_failover_parms);
/* unsupported */
ACU_ERR ACU_DEPRECATED ACU_EXPORT ipt_reopen_registration(REGISTRATION_FAILOVER_XPARMS* reg_failover_parms);

#ifdef _WIN32
#pragma pack (pop)
#endif

#ifdef __cplusplus
}
#endif

#endif
