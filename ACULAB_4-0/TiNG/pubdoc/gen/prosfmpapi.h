/* prosfmpapi.h - Prosody FMP processing API declarations */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

#ifndef INCLUDED_PUBDOC_PROSFMPAPI_H
#define INCLUDED_PUBDOC_PROSFMPAPI_H

#ifdef ACULAB_TiNG_VERSION_NUM
#if ACULAB_TiNG_VERSION_NUM != 1504
#error ACULAB_TiNG_VERSION_NUM does not match 1504
#endif
#else
#define ACULAB_TiNG_VERSION_NUM 1504
#define ACULAB_TiNG_VERSION_CHANGED 
#define ACULAB_TiNG_VERSION_STRING "m1504."
#endif

enum kSMFMPrxType {
	kSMFMPrxTypeIPv4,
	kSMFMPrxTypeIPv6,
};

enum kSMFMPrxStatus {
	kSMFMPrxStatusRunning,
	kSMFMPrxStatusStopped,
	kSMFMPrxStatusGotPorts,
	kSMFMPrxStatusSource,
	kSMFMPrxStatusGotPortsIPv6,
	kSMFMPrxStatusSourceIPv6,
};

enum kSMFMPRecoveryType {
	kSMFMPRecoveryTypeRED,
	kSMFMPRecoveryTypeFEC,
	kSMFMPRecoveryTypeRTP,
};

enum kSMFMPtxStatus {
	kSMFMPtxStatusRunning,
	kSMFMPtxStatusStopped,
};



typedef struct sm_fmprx_create_parms {
	tSMFMPrxId fmprx;
	tSMModuleId module;
	enum kSMFMPrxType type;
	struct in_addr address;
	struct in6_addr ipv6_address;
} SM_FMPRX_CREATE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_fmprx_create sm_fmprx_create_iPsFMrMte1_2_SIdSI6d_
#define SM_FMPRX_CREATE_NAME "sm_fmprx_create_iPsFMrMte1_2_SIdSI6d_"
#endif
typedef int SM_FMPRX_CREATE_FN(struct sm_fmprx_create_parms *fmprxp);

ACUAPI int ACUTiNG_WINAPI sm_fmprx_create(struct sm_fmprx_create_parms *fmprxp);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_fmprx_destroy sm_fmprx_destroy_iFMr
#define SM_FMPRX_DESTROY_NAME "sm_fmprx_destroy_iFMr"
#endif
typedef int SM_FMPRX_DESTROY_FN(tSMFMPrxId fmprx);

ACUAPI int ACUTiNG_WINAPI sm_fmprx_destroy(tSMFMPrxId fmprx);


typedef struct sm_fmprx_datafeed_parms {
	tSMFMPrxId fmprx;
	tSMDatafeedId datafeed;
} SM_FMPRX_DATAFEED_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_fmprx_get_datafeed sm_fmprx_get_datafeed_iPsFMrDt_
#define SM_FMPRX_GET_DATAFEED_NAME "sm_fmprx_get_datafeed_iPsFMrDt_"
#endif
typedef int SM_FMPRX_GET_DATAFEED_FN(struct sm_fmprx_datafeed_parms *datafeedp);

ACUAPI int ACUTiNG_WINAPI sm_fmprx_get_datafeed(struct sm_fmprx_datafeed_parms *datafeedp);


typedef struct sm_fmprx_event_parms {
	tSMFMPrxId fmprx;
	tSMEventId event;
} SM_FMPRX_EVENT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_fmprx_get_event sm_fmprx_get_event_iPsFMrEt_
#define SM_FMPRX_GET_EVENT_NAME "sm_fmprx_get_event_iPsFMrEt_"
#endif
typedef int SM_FMPRX_GET_EVENT_FN(struct sm_fmprx_event_parms *eventp);

ACUAPI int ACUTiNG_WINAPI sm_fmprx_get_event(struct sm_fmprx_event_parms *eventp);


typedef struct sm_fmprx_status_parms {
	tSMFMPrxId fmprx;
	enum kSMFMPrxStatus status;
	union {
		struct {
			int T38_port;
			struct in_addr address;
		} port;
		struct {
			struct in_addr address;
			int port;
		} source;
		struct {
			int T38_port;
			struct in6_addr address;
		} port_ipv6;
		struct {
			struct in6_addr address;
			int port;
		} source_ipv6;
	} u;
} SM_FMPRX_STATUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_fmprx_status sm_fmprx_status_iPsFMre1_6_usiSId_sSIdi_siSI6d_sSI6di___
#define SM_FMPRX_STATUS_NAME "sm_fmprx_status_iPsFMre1_6_usiSId_sSIdi_siSI6d_sSI6di___"
#endif
typedef int SM_FMPRX_STATUS_FN(struct sm_fmprx_status_parms *statusp);

ACUAPI int ACUTiNG_WINAPI sm_fmprx_status(struct sm_fmprx_status_parms *statusp);


typedef struct sm_fmprx_stop_parms {
	tSMFMPrxId fmprx;
} SM_FMPRX_STOP_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_fmprx_stop sm_fmprx_stop_iPsFMr_
#define SM_FMPRX_STOP_NAME "sm_fmprx_stop_iPsFMr_"
#endif
typedef int SM_FMPRX_STOP_FN(struct sm_fmprx_stop_parms *stopp);

ACUAPI int ACUTiNG_WINAPI sm_fmprx_stop(struct sm_fmprx_stop_parms *stopp);


typedef struct sm_fmptx_config_parms {
	tSMFMPtxId fmptx;
	SOCKADDR_IN destination;
	SOCKADDR_IN source;
	enum kSMFMPRecoveryType Recovery;
	tSM_UT32 RecoveryLevel;
} SM_FMPTX_CONFIG_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_fmptx_config sm_fmptx_config_iPsFMtSAdSAde1_3_Ui_
#define SM_FMPTX_CONFIG_NAME "sm_fmptx_config_iPsFMtSAdSAde1_3_Ui_"
#endif
typedef int SM_FMPTX_CONFIG_FN(struct sm_fmptx_config_parms *configp);

ACUAPI int ACUTiNG_WINAPI sm_fmptx_config(struct sm_fmptx_config_parms *configp);


typedef struct sm_fmptx_config_ipv6_parms {
	tSMFMPtxId fmptx;
	SOCKADDR_IN6 destination;
	SOCKADDR_IN6 source;
	enum kSMFMPRecoveryType Recovery;
	tSM_UT32 RecoveryLevel;
} SM_FMPTX_CONFIG_IPV6_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_fmptx_config_ipv6 sm_fmptx_config_ipv6_iPsFMtSA6dSA6de1_3_Ui_
#define SM_FMPTX_CONFIG_IPV6_NAME "sm_fmptx_config_ipv6_iPsFMtSA6dSA6de1_3_Ui_"
#endif
typedef int SM_FMPTX_CONFIG_IPV6_FN(struct sm_fmptx_config_ipv6_parms *configp);

ACUAPI int ACUTiNG_WINAPI sm_fmptx_config_ipv6(struct sm_fmptx_config_ipv6_parms *configp);


typedef struct sm_fmptx_create_parms {
	tSMFMPtxId fmptx;
	tSMModuleId module;
} SM_FMPTX_CREATE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_fmptx_create sm_fmptx_create_iPsFMtMt_
#define SM_FMPTX_CREATE_NAME "sm_fmptx_create_iPsFMtMt_"
#endif
typedef int SM_FMPTX_CREATE_FN(struct sm_fmptx_create_parms *fmptxp);

ACUAPI int ACUTiNG_WINAPI sm_fmptx_create(struct sm_fmptx_create_parms *fmptxp);


typedef struct sm_fmptx_datafeed_connect_parms {
	tSMDatafeedId data_source;
	tSMFMPtxId data_dest;
} SM_FMPTX_DATAFEED_CONNECT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_fmptx_datafeed_connect sm_fmptx_datafeed_connect_iPsDtFMt_
#define SM_FMPTX_DATAFEED_CONNECT_NAME "sm_fmptx_datafeed_connect_iPsDtFMt_"
#endif
typedef int SM_FMPTX_DATAFEED_CONNECT_FN(struct sm_fmptx_datafeed_connect_parms *datafeedp);

ACUAPI int ACUTiNG_WINAPI sm_fmptx_datafeed_connect(struct sm_fmptx_datafeed_connect_parms *datafeedp);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_fmptx_destroy sm_fmptx_destroy_iFMt
#define SM_FMPTX_DESTROY_NAME "sm_fmptx_destroy_iFMt"
#endif
typedef int SM_FMPTX_DESTROY_FN(tSMFMPtxId fmptx);

ACUAPI int ACUTiNG_WINAPI sm_fmptx_destroy(tSMFMPtxId fmptx);


typedef struct sm_fmptx_event_parms {
	tSMFMPtxId fmptx;
	tSMEventId event;
} SM_FMPTX_EVENT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_fmptx_get_event sm_fmptx_get_event_iPsFMtEt_
#define SM_FMPTX_GET_EVENT_NAME "sm_fmptx_get_event_iPsFMtEt_"
#endif
typedef int SM_FMPTX_GET_EVENT_FN(struct sm_fmptx_event_parms *eventp);

ACUAPI int ACUTiNG_WINAPI sm_fmptx_get_event(struct sm_fmptx_event_parms *eventp);


typedef struct sm_fmptx_status_parms {
	tSMFMPtxId fmptx;
	enum kSMFMPtxStatus status;
} SM_FMPTX_STATUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_fmptx_status sm_fmptx_status_iPsFMte1_2__
#define SM_FMPTX_STATUS_NAME "sm_fmptx_status_iPsFMte1_2__"
#endif
typedef int SM_FMPTX_STATUS_FN(struct sm_fmptx_status_parms *statusp);

ACUAPI int ACUTiNG_WINAPI sm_fmptx_status(struct sm_fmptx_status_parms *statusp);


typedef struct sm_fmptx_stop_parms {
	tSMFMPtxId fmptx;
} SM_FMPTX_STOP_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_fmptx_stop sm_fmptx_stop_iPsFMt_
#define SM_FMPTX_STOP_NAME "sm_fmptx_stop_iPsFMt_"
#endif
typedef int SM_FMPTX_STOP_FN(struct sm_fmptx_stop_parms *stopp);

ACUAPI int ACUTiNG_WINAPI sm_fmptx_stop(struct sm_fmptx_stop_parms *stopp);



#endif
