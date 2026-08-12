/* prossntp.h - Prosody SNTP Client API declarations */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

#ifndef INCLUDED_PUBDOC_PROSSNTP_H
#define INCLUDED_PUBDOC_PROSSNTP_H

#ifdef ACULAB_TiNG_VERSION_NUM
#if ACULAB_TiNG_VERSION_NUM != 1504
#error ACULAB_TiNG_VERSION_NUM does not match 1504
#endif
#else
#define ACULAB_TiNG_VERSION_NUM 1504
#define ACULAB_TiNG_VERSION_CHANGED 
#define ACULAB_TiNG_VERSION_STRING "m1504."
#endif

enum kSMSNTPType {
	kSMSNTPTypeIPv4,
	kSMSNTPTypeIPv6,
};

enum kSMSNTPStatus {
	kSMSNTPStatusRunning,
	kSMSNTPStatusStopped,
	kSMSNTPStatusSync,
};



typedef struct sm_sntp_config_parms {
	tSMSNTPId sntp;
	SOCKADDR_IN server;
	int TOS;
	tSM_UT32 no_reply_delay;
	tSM_UT32 had_reply_delay;
} SM_SNTP_CONFIG_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_sntp_config sm_sntp_config_iPsSNTPtSAdiUiUi_
#define SM_SNTP_CONFIG_NAME "sm_sntp_config_iPsSNTPtSAdiUiUi_"
#endif
typedef int SM_SNTP_CONFIG_FN(struct sm_sntp_config_parms *configp);

ACUAPI int ACUTiNG_WINAPI sm_sntp_config(struct sm_sntp_config_parms *configp);


typedef struct sm_sntp_config_ipv6_parms {
	tSMSNTPId sntp;
	SOCKADDR_IN6 server;
	tSM_UT32 no_reply_delay;
	tSM_UT32 had_reply_delay;
} SM_SNTP_CONFIG_IPV6_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_sntp_config_ipv6 sm_sntp_config_ipv6_iPsSNTPtSA6dUiUi_
#define SM_SNTP_CONFIG_IPV6_NAME "sm_sntp_config_ipv6_iPsSNTPtSA6dUiUi_"
#endif
typedef int SM_SNTP_CONFIG_IPV6_FN(struct sm_sntp_config_ipv6_parms *configp);

ACUAPI int ACUTiNG_WINAPI sm_sntp_config_ipv6(struct sm_sntp_config_ipv6_parms *configp);


typedef struct sm_sntp_create_parms {
	tSMSNTPId sntp;
	tSMModuleId module;
	enum kSMSNTPType type;
	struct in_addr address;
	struct in6_addr ipv6_address;
} SM_SNTP_CREATE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_sntp_create sm_sntp_create_iPsSNTPtMte1_2_SIdSI6d_
#define SM_SNTP_CREATE_NAME "sm_sntp_create_iPsSNTPtMte1_2_SIdSI6d_"
#endif
typedef int SM_SNTP_CREATE_FN(struct sm_sntp_create_parms *createp);

ACUAPI int ACUTiNG_WINAPI sm_sntp_create(struct sm_sntp_create_parms *createp);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_sntp_destroy sm_sntp_destroy_iSNTPt
#define SM_SNTP_DESTROY_NAME "sm_sntp_destroy_iSNTPt"
#endif
typedef int SM_SNTP_DESTROY_FN(tSMSNTPId sntp);

ACUAPI int ACUTiNG_WINAPI sm_sntp_destroy(tSMSNTPId sntp);


typedef struct sm_sntp_event_parms {
	tSMSNTPId sntp;
	tSMEventId event;
} SM_SNTP_EVENT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_sntp_get_event sm_sntp_get_event_iPsSNTPtEt_
#define SM_SNTP_GET_EVENT_NAME "sm_sntp_get_event_iPsSNTPtEt_"
#endif
typedef int SM_SNTP_GET_EVENT_FN(struct sm_sntp_event_parms *eventp);

ACUAPI int ACUTiNG_WINAPI sm_sntp_get_event(struct sm_sntp_event_parms *eventp);


typedef struct sm_sntp_status_parms {
	tSMSNTPId sntp;
	enum kSMSNTPStatus status;
	union {
		struct {
			int has_sync;
		} sync;
	} u;
} SM_SNTP_STATUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_sntp_status sm_sntp_status_iPsSNTPte1_3_usi___
#define SM_SNTP_STATUS_NAME "sm_sntp_status_iPsSNTPte1_3_usi___"
#endif
typedef int SM_SNTP_STATUS_FN(struct sm_sntp_status_parms *statusp);

ACUAPI int ACUTiNG_WINAPI sm_sntp_status(struct sm_sntp_status_parms *statusp);



#endif
