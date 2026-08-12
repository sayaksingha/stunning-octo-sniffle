/* prossprt.h - Prosody SPRT end point API declarations */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

#ifndef INCLUDED_PUBDOC_PROSSPRT_H
#define INCLUDED_PUBDOC_PROSSPRT_H

#ifdef ACULAB_TiNG_VERSION_NUM
#if ACULAB_TiNG_VERSION_NUM != 1504
#error ACULAB_TiNG_VERSION_NUM does not match 1504
#endif
#else
#define ACULAB_TiNG_VERSION_NUM 1504
#define ACULAB_TiNG_VERSION_CHANGED 
#define ACULAB_TiNG_VERSION_STRING "m1504."
#endif

enum kSMSPRTDataDirection {
	kSMSPRTDataDirectionTx,
	kSMSPRTDataDirectionRx,
};

enum kSMSPRTDataFormat {
	kSMSPRTDataFormatNone,
	kSMSPRTDataFormatIOctet,
	kSMSPRTDataFormatIOctetCS,
	kSMSPRTDataFormatIRawOctet,
};

enum kSMSPRTStatus {
	kSMSPRTStatusRunning,
	kSMSPRTStatusStopped,
	kSMSPRTStatusMessage,
};



typedef struct sm_sprt_config_parms {
	tSMSPRTId sprt;
	SOCKADDR_IN dest;
	SOCKADDR_IN src;
	int payload_type;
} SM_SPRT_CONFIG_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_sprt_config sm_sprt_config_iPsSPRTtSAdSAdi_
#define SM_SPRT_CONFIG_NAME "sm_sprt_config_iPsSPRTtSAdSAdi_"
#endif
typedef int SM_SPRT_CONFIG_FN(struct sm_sprt_config_parms *configp);

ACUAPI int ACUTiNG_WINAPI sm_sprt_config(struct sm_sprt_config_parms *configp);


typedef struct sm_sprt_config_data_channel_parms {
	tSMSPRTId sprt;
	int data_channel;
	int max_payload_length;
	int max_latency;
} SM_SPRT_CONFIG_DATA_CHANNEL_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_sprt_config_data_channel sm_sprt_config_data_channel_iPsSPRTtiii_
#define SM_SPRT_CONFIG_DATA_CHANNEL_NAME "sm_sprt_config_data_channel_iPsSPRTtiii_"
#endif
typedef int SM_SPRT_CONFIG_DATA_CHANNEL_FN(struct sm_sprt_config_data_channel_parms *configp);

ACUAPI int ACUTiNG_WINAPI sm_sprt_config_data_channel(struct sm_sprt_config_data_channel_parms *configp);


typedef struct sm_sprt_config_data_format_parms {
	tSMSPRTId sprt;
	enum kSMSPRTDataDirection direction;
	enum kSMSPRTDataFormat format;
} SM_SPRT_CONFIG_DATA_FORMAT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_sprt_config_data_format sm_sprt_config_data_format_iPsSPRTte1_2_e1_4__
#define SM_SPRT_CONFIG_DATA_FORMAT_NAME "sm_sprt_config_data_format_iPsSPRTte1_2_e1_4__"
#endif
typedef int SM_SPRT_CONFIG_DATA_FORMAT_FN(struct sm_sprt_config_data_format_parms *configp);

ACUAPI int ACUTiNG_WINAPI sm_sprt_config_data_format(struct sm_sprt_config_data_format_parms *configp);


typedef struct sm_sprt_config_ipv6_parms {
	tSMSPRTId sprt;
	SOCKADDR_IN6 dest;
	SOCKADDR_IN6 src;
	int payload_type;
} SM_SPRT_CONFIG_IPV6_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_sprt_config_ipv6 sm_sprt_config_ipv6_iPsSPRTtSA6dSA6di_
#define SM_SPRT_CONFIG_IPV6_NAME "sm_sprt_config_ipv6_iPsSPRTtSA6dSA6di_"
#endif
typedef int SM_SPRT_CONFIG_IPV6_FN(struct sm_sprt_config_ipv6_parms *configp);

ACUAPI int ACUTiNG_WINAPI sm_sprt_config_ipv6(struct sm_sprt_config_ipv6_parms *configp);


typedef struct sm_sprt_config_reliable_channel_parms {
	tSMSPRTId sprt;
	int channel;
	int window_size;
	int timer_TA01;
	int timer_TA02;
	int timer_TR03;
} SM_SPRT_CONFIG_RELIABLE_CHANNEL_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_sprt_config_reliable_channel sm_sprt_config_reliable_channel_iPsSPRTtiiiii_
#define SM_SPRT_CONFIG_RELIABLE_CHANNEL_NAME "sm_sprt_config_reliable_channel_iPsSPRTtiiiii_"
#endif
typedef int SM_SPRT_CONFIG_RELIABLE_CHANNEL_FN(struct sm_sprt_config_reliable_channel_parms *configp);

ACUAPI int ACUTiNG_WINAPI sm_sprt_config_reliable_channel(struct sm_sprt_config_reliable_channel_parms *configp);


typedef struct sm_sprt_create_parms {
	tSMSPRTId sprt;
	tSMModuleId module;
} SM_SPRT_CREATE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_sprt_create sm_sprt_create_iPsSPRTtMt_
#define SM_SPRT_CREATE_NAME "sm_sprt_create_iPsSPRTtMt_"
#endif
typedef int SM_SPRT_CREATE_FN(struct sm_sprt_create_parms *createp);

ACUAPI int ACUTiNG_WINAPI sm_sprt_create(struct sm_sprt_create_parms *createp);


typedef struct sm_sprt_datafeed_connect_parms {
	tSMDatafeedId data_source;
	tSMSPRTId sprt;
} SM_SPRT_DATAFEED_CONNECT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_sprt_datafeed_connect sm_sprt_datafeed_connect_iPsDtSPRTt_
#define SM_SPRT_DATAFEED_CONNECT_NAME "sm_sprt_datafeed_connect_iPsDtSPRTt_"
#endif
typedef int SM_SPRT_DATAFEED_CONNECT_FN(struct sm_sprt_datafeed_connect_parms *datafeedp);

ACUAPI int ACUTiNG_WINAPI sm_sprt_datafeed_connect(struct sm_sprt_datafeed_connect_parms *datafeedp);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_sprt_destroy sm_sprt_destroy_iSPRTt
#define SM_SPRT_DESTROY_NAME "sm_sprt_destroy_iSPRTt"
#endif
typedef int SM_SPRT_DESTROY_FN(tSMSPRTId sprt);

ACUAPI int ACUTiNG_WINAPI sm_sprt_destroy(tSMSPRTId sprt);


typedef struct sm_sprt_get_datafeed_parms {
	tSMSPRTId sprt;
	tSMDatafeedId datafeed;
} SM_SPRT_GET_DATAFEED_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_sprt_get_datafeed sm_sprt_get_datafeed_iPsSPRTtDt_
#define SM_SPRT_GET_DATAFEED_NAME "sm_sprt_get_datafeed_iPsSPRTtDt_"
#endif
typedef int SM_SPRT_GET_DATAFEED_FN(struct sm_sprt_get_datafeed_parms *datafeedp);

ACUAPI int ACUTiNG_WINAPI sm_sprt_get_datafeed(struct sm_sprt_get_datafeed_parms *datafeedp);


typedef struct sm_sprt_get_event_parms {
	tSMSPRTId sprt;
	tSMEventId event;
} SM_SPRT_GET_EVENT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_sprt_get_event sm_sprt_get_event_iPsSPRTtEt_
#define SM_SPRT_GET_EVENT_NAME "sm_sprt_get_event_iPsSPRTtEt_"
#endif
typedef int SM_SPRT_GET_EVENT_FN(struct sm_sprt_get_event_parms *eventp);

ACUAPI int ACUTiNG_WINAPI sm_sprt_get_event(struct sm_sprt_get_event_parms *eventp);


typedef struct sm_sprt_read_message_parms {
	tSMSPRTId sprt;
	char *message;
	int max_length;
	int length;
} SM_SPRT_READ_MESSAGE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_sprt_read_message sm_sprt_read_message_iPsSPRTtPcii_
#define SM_SPRT_READ_MESSAGE_NAME "sm_sprt_read_message_iPsSPRTtPcii_"
#endif
typedef int SM_SPRT_READ_MESSAGE_FN(struct sm_sprt_read_message_parms *datafeedp);

ACUAPI int ACUTiNG_WINAPI sm_sprt_read_message(struct sm_sprt_read_message_parms *datafeedp);


typedef struct sm_sprt_send_message_parms {
	tSMSPRTId sprt;
	char *message;
	int length;
} SM_SPRT_SEND_MESSAGE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_sprt_send_message sm_sprt_send_message_iPsSPRTtPci_
#define SM_SPRT_SEND_MESSAGE_NAME "sm_sprt_send_message_iPsSPRTtPci_"
#endif
typedef int SM_SPRT_SEND_MESSAGE_FN(struct sm_sprt_send_message_parms *datafeedp);

ACUAPI int ACUTiNG_WINAPI sm_sprt_send_message(struct sm_sprt_send_message_parms *datafeedp);


typedef struct sm_sprt_status_parms {
	tSMSPRTId sprt;
	enum kSMSPRTStatus status;
} SM_SPRT_STATUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_sprt_status sm_sprt_status_iPsSPRTte1_3__
#define SM_SPRT_STATUS_NAME "sm_sprt_status_iPsSPRTte1_3__"
#endif
typedef int SM_SPRT_STATUS_FN(struct sm_sprt_status_parms *statusp);

ACUAPI int ACUTiNG_WINAPI sm_sprt_status(struct sm_sprt_status_parms *statusp);



#endif
