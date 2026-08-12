/* prosdtls.h - Prosody DTLS processing API declarations */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

#ifndef INCLUDED_PUBDOC_PROSDTLS_H
#define INCLUDED_PUBDOC_PROSDTLS_H

#ifdef ACULAB_TiNG_VERSION_NUM
#if ACULAB_TiNG_VERSION_NUM != 1504
#error ACULAB_TiNG_VERSION_NUM does not match 1504
#endif
#else
#define ACULAB_TiNG_VERSION_NUM 1504
#define ACULAB_TiNG_VERSION_CHANGED 
#define ACULAB_TiNG_VERSION_STRING "m1504."
#endif

enum kSMDTLSrxStatus {
	kSMDTLSrxStatusRunning,
	kSMDTLSrxStatusStopped,
	kSMDTLSrxStatusHasPacket,
	kSMDTLSrxStatusOverrun,
};

enum kSMDTLStxStatus {
	kSMDTLStxStatusRunning,
	kSMDTLStxStatusStopped,
	kSMDTLStxStatusHasCapacity,
};



typedef struct sm_dtlsrx_create_parms {
	tSMDTLSrxId dtlsrx;
	tSMModuleId module;
} SM_DTLSRX_CREATE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_dtlsrx_create sm_dtlsrx_create_iPsDTLSrMt_
#define SM_DTLSRX_CREATE_NAME "sm_dtlsrx_create_iPsDTLSrMt_"
#endif
typedef int SM_DTLSRX_CREATE_FN(struct sm_dtlsrx_create_parms *dtlsrxp);

ACUAPI int ACUTiNG_WINAPI sm_dtlsrx_create(struct sm_dtlsrx_create_parms *dtlsrxp);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_dtlsrx_destroy sm_dtlsrx_destroy_iDTLSr
#define SM_DTLSRX_DESTROY_NAME "sm_dtlsrx_destroy_iDTLSr"
#endif
typedef int SM_DTLSRX_DESTROY_FN(tSMDTLSrxId dtlsrx);

ACUAPI int ACUTiNG_WINAPI sm_dtlsrx_destroy(tSMDTLSrxId dtlsrx);


typedef struct sm_dtlsrx_event_parms {
	tSMDTLSrxId dtlsrx;
	tSMEventId event;
} SM_DTLSRX_EVENT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_dtlsrx_get_event sm_dtlsrx_get_event_iPsDTLSrEt_
#define SM_DTLSRX_GET_EVENT_NAME "sm_dtlsrx_get_event_iPsDTLSrEt_"
#endif
typedef int SM_DTLSRX_GET_EVENT_FN(struct sm_dtlsrx_event_parms *eventp);

ACUAPI int ACUTiNG_WINAPI sm_dtlsrx_get_event(struct sm_dtlsrx_event_parms *eventp);


typedef struct sm_dtlsrx_get_packet_parms {
	tSMDTLSrxId dtlsrx;
	char *data;
	tSM_INT max_length;
	tSM_INT packet_length;
} SM_DTLSRX_GET_PACKET_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_dtlsrx_get_packet sm_dtlsrx_get_packet_iPsDTLSrPcii_
#define SM_DTLSRX_GET_PACKET_NAME "sm_dtlsrx_get_packet_iPsDTLSrPcii_"
#endif
typedef int SM_DTLSRX_GET_PACKET_FN(struct sm_dtlsrx_get_packet_parms *readp);

ACUAPI int ACUTiNG_WINAPI sm_dtlsrx_get_packet(struct sm_dtlsrx_get_packet_parms *readp);


typedef struct sm_dtlsrx_status_parms {
	tSMDTLSrxId dtlsrx;
	enum kSMDTLSrxStatus status;
} SM_DTLSRX_STATUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_dtlsrx_status sm_dtlsrx_status_iPsDTLSre1_4__
#define SM_DTLSRX_STATUS_NAME "sm_dtlsrx_status_iPsDTLSre1_4__"
#endif
typedef int SM_DTLSRX_STATUS_FN(struct sm_dtlsrx_status_parms *statusp);

ACUAPI int ACUTiNG_WINAPI sm_dtlsrx_status(struct sm_dtlsrx_status_parms *statusp);


typedef struct sm_dtlsrx_stop_parms {
	tSMDTLSrxId dtlsrx;
} SM_DTLSRX_STOP_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_dtlsrx_stop sm_dtlsrx_stop_iPsDTLSr_
#define SM_DTLSRX_STOP_NAME "sm_dtlsrx_stop_iPsDTLSr_"
#endif
typedef int SM_DTLSRX_STOP_FN(struct sm_dtlsrx_stop_parms *stopp);

ACUAPI int ACUTiNG_WINAPI sm_dtlsrx_stop(struct sm_dtlsrx_stop_parms *stopp);


typedef struct sm_dtlstx_config_parms {
	tSMDTLStxId dtlstx;
	SOCKADDR_IN destination;
	SOCKADDR_IN source;
} SM_DTLSTX_CONFIG_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_dtlstx_config sm_dtlstx_config_iPsDTLStSAdSAd_
#define SM_DTLSTX_CONFIG_NAME "sm_dtlstx_config_iPsDTLStSAdSAd_"
#endif
typedef int SM_DTLSTX_CONFIG_FN(struct sm_dtlstx_config_parms *configp);

ACUAPI int ACUTiNG_WINAPI sm_dtlstx_config(struct sm_dtlstx_config_parms *configp);


typedef struct sm_dtlstx_config_ipv6_parms {
	tSMDTLStxId dtlstx;
	SOCKADDR_IN6 destination;
	SOCKADDR_IN6 source;
} SM_DTLSTX_CONFIG_IPV6_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_dtlstx_config_ipv6 sm_dtlstx_config_ipv6_iPsDTLStSA6dSA6d_
#define SM_DTLSTX_CONFIG_IPV6_NAME "sm_dtlstx_config_ipv6_iPsDTLStSA6dSA6d_"
#endif
typedef int SM_DTLSTX_CONFIG_IPV6_FN(struct sm_dtlstx_config_ipv6_parms *configp);

ACUAPI int ACUTiNG_WINAPI sm_dtlstx_config_ipv6(struct sm_dtlstx_config_ipv6_parms *configp);


typedef struct sm_dtlstx_config_notify_parms {
	tSMDTLStxId dtlstx;
	tSM_UT32 packet_length;
	tSM_INT no_capacity_event;
} SM_DTLSTX_CONFIG_NOTIFY_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_dtlstx_config_notify sm_dtlstx_config_notify_iPsDTLStUii_
#define SM_DTLSTX_CONFIG_NOTIFY_NAME "sm_dtlstx_config_notify_iPsDTLStUii_"
#endif
typedef int SM_DTLSTX_CONFIG_NOTIFY_FN(struct sm_dtlstx_config_notify_parms *configp);

ACUAPI int ACUTiNG_WINAPI sm_dtlstx_config_notify(struct sm_dtlstx_config_notify_parms *configp);


typedef struct sm_dtlstx_create_parms {
	tSMDTLStxId dtlstx;
	tSMModuleId module;
} SM_DTLSTX_CREATE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_dtlstx_create sm_dtlstx_create_iPsDTLStMt_
#define SM_DTLSTX_CREATE_NAME "sm_dtlstx_create_iPsDTLStMt_"
#endif
typedef int SM_DTLSTX_CREATE_FN(struct sm_dtlstx_create_parms *dtlstxp);

ACUAPI int ACUTiNG_WINAPI sm_dtlstx_create(struct sm_dtlstx_create_parms *dtlstxp);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_dtlstx_destroy sm_dtlstx_destroy_iDTLSt
#define SM_DTLSTX_DESTROY_NAME "sm_dtlstx_destroy_iDTLSt"
#endif
typedef int SM_DTLSTX_DESTROY_FN(tSMDTLStxId dtlstx);

ACUAPI int ACUTiNG_WINAPI sm_dtlstx_destroy(tSMDTLStxId dtlstx);


typedef struct sm_dtlstx_event_parms {
	tSMDTLStxId dtlstx;
	tSMEventId event;
} SM_DTLSTX_EVENT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_dtlstx_get_event sm_dtlstx_get_event_iPsDTLStEt_
#define SM_DTLSTX_GET_EVENT_NAME "sm_dtlstx_get_event_iPsDTLStEt_"
#endif
typedef int SM_DTLSTX_GET_EVENT_FN(struct sm_dtlstx_event_parms *eventp);

ACUAPI int ACUTiNG_WINAPI sm_dtlstx_get_event(struct sm_dtlstx_event_parms *eventp);


typedef struct sm_dtlstx_send_packet_parms {
	tSMDTLStxId dtlstx;
	char *data;
	tSM_INT packet_length;
} SM_DTLSTX_SEND_PACKET_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_dtlstx_send_packet sm_dtlstx_send_packet_iPsDTLStPci_
#define SM_DTLSTX_SEND_PACKET_NAME "sm_dtlstx_send_packet_iPsDTLStPci_"
#endif
typedef int SM_DTLSTX_SEND_PACKET_FN(struct sm_dtlstx_send_packet_parms *packetp);

ACUAPI int ACUTiNG_WINAPI sm_dtlstx_send_packet(struct sm_dtlstx_send_packet_parms *packetp);


typedef struct sm_dtlstx_status_parms {
	tSMDTLStxId dtlstx;
	enum kSMDTLStxStatus status;
} SM_DTLSTX_STATUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_dtlstx_status sm_dtlstx_status_iPsDTLSte1_3__
#define SM_DTLSTX_STATUS_NAME "sm_dtlstx_status_iPsDTLSte1_3__"
#endif
typedef int SM_DTLSTX_STATUS_FN(struct sm_dtlstx_status_parms *statusp);

ACUAPI int ACUTiNG_WINAPI sm_dtlstx_status(struct sm_dtlstx_status_parms *statusp);


typedef struct sm_dtlstx_stop_parms {
	tSMDTLStxId dtlstx;
} SM_DTLSTX_STOP_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_dtlstx_stop sm_dtlstx_stop_iPsDTLSt_
#define SM_DTLSTX_STOP_NAME "sm_dtlstx_stop_iPsDTLSt_"
#endif
typedef int SM_DTLSTX_STOP_FN(struct sm_dtlstx_stop_parms *stopp);

ACUAPI int ACUTiNG_WINAPI sm_dtlstx_stop(struct sm_dtlstx_stop_parms *stopp);


typedef struct sm_vmprx_config_dtls_parms {
	tSMVMPrxId vmprx;
	tSMDTLSrxId dtlsrx;
	tSM_INT use_RTCP_port;
} SM_VMPRX_CONFIG_DTLS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_dtls sm_vmprx_config_dtls_iPsVrDTLSri_
#define SM_VMPRX_CONFIG_DTLS_NAME "sm_vmprx_config_dtls_iPsVrDTLSri_"
#endif
typedef int SM_VMPRX_CONFIG_DTLS_FN(struct sm_vmprx_config_dtls_parms *configp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_dtls(struct sm_vmprx_config_dtls_parms *configp);



#endif
