/* prosgen.h - Prosody generic API declarations */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

#ifndef INCLUDED_PUBDOC_PROSGEN_H
#define INCLUDED_PUBDOC_PROSGEN_H

#ifdef ACULAB_TiNG_VERSION_NUM
#if ACULAB_TiNG_VERSION_NUM != 1504
#error ACULAB_TiNG_VERSION_NUM does not match 1504
#endif
#else
#define ACULAB_TiNG_VERSION_NUM 1504
#define ACULAB_TiNG_VERSION_CHANGED 
#define ACULAB_TiNG_VERSION_STRING "m1504."
#endif

#define kSMMaxRevAddText 64
#define kSMMaxSerialNoText 16
enum kSMChannelType {
	kSMChannelTypeInput,
	kSMChannelTypeOutput,
	kSMChannelTypeHalfDuplex,
	kSMChannelTypeFullDuplex,
};

enum kSMTimeslotType {
	kSMTimeslotTypeALaw,
	kSMTimeslotTypeMuLaw,
	kSMTimeslotTypeData,
};

enum kSMCollectorStatus {
	kSMCollectorStatusRunning,
	kSMCollectorStatusGotPorts,
	kSMCollectorStatusNewPeer,
	kSMCollectorStatusGotPortsIPv6,
	kSMCollectorStatusNewPeerIPv6,
};

enum kSMCarrierCardType {
	kSMCarrierCardTypeYetUnknown,
	kSMCarrierCardTypeISAS2,
	kSMCarrierCardTypeBR4,
	kSMCarrierCardTypeBR8,
	kSMCarrierCardTypePCIP1,
	kSMCarrierCardTypePCIC1,
	kSMCarrierCardTypePS,
	kSMCarrierCardTypePX,
};

enum kSMModuleStatus {
	kSMModuleStatusRunning,
	kSMModuleStatusShutdown,
};

enum kSMCollectorType {
	kSMCollectorTypeIPv4,
	kSMCollectorTypeIPv6,
};

enum kSMDispatcherType {
	kSMDispatcherTypeIPv4,
	kSMDispatcherTypeIPv6,
};



typedef struct sm_channel_alloc_placed_parms {
	tSMChannelId channel;
	enum kSMChannelType type;
	tSMModuleId module;
	tSM_INT caps_mask;
} SM_CHANNEL_ALLOC_PLACED_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_channel_alloc_placed sm_channel_alloc_placed_iPsCte1_4_Mti_
#define SM_CHANNEL_ALLOC_PLACED_NAME "sm_channel_alloc_placed_iPsCte1_4_Mti_"
#endif
typedef int SM_CHANNEL_ALLOC_PLACED_FN(struct sm_channel_alloc_placed_parms *channelp);

ACUAPI int ACUTiNG_WINAPI sm_channel_alloc_placed(struct sm_channel_alloc_placed_parms *channelp);


typedef struct sm_channel_collector_connect_parms {
	tSMCollectorId source;
	tSMChannelId channel;
} SM_CHANNEL_COLLECTOR_CONNECT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_channel_collector_connect sm_channel_collector_connect_iPsCeCt_
#define SM_CHANNEL_COLLECTOR_CONNECT_NAME "sm_channel_collector_connect_iPsCeCt_"
#endif
typedef int SM_CHANNEL_COLLECTOR_CONNECT_FN(struct sm_channel_collector_connect_parms *dp);

ACUAPI int ACUTiNG_WINAPI sm_channel_collector_connect(struct sm_channel_collector_connect_parms *dp);


typedef struct sm_channel_datafeed_connect_parms {
	tSMDatafeedId data_source;
	tSMChannelId channel;
} SM_CHANNEL_DATAFEED_CONNECT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_channel_datafeed_connect sm_channel_datafeed_connect_iPsDtCt_
#define SM_CHANNEL_DATAFEED_CONNECT_NAME "sm_channel_datafeed_connect_iPsDtCt_"
#endif
typedef int SM_CHANNEL_DATAFEED_CONNECT_FN(struct sm_channel_datafeed_connect_parms *datafeedp);

ACUAPI int ACUTiNG_WINAPI sm_channel_datafeed_connect(struct sm_channel_datafeed_connect_parms *datafeedp);


typedef struct sm_channel_dispatcher_connect_parms {
	tSMDispatcherId dest;
	tSMChannelId channel;
} SM_CHANNEL_DISPATCHER_CONNECT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_channel_dispatcher_connect sm_channel_dispatcher_connect_iPsDeCt_
#define SM_CHANNEL_DISPATCHER_CONNECT_NAME "sm_channel_dispatcher_connect_iPsDeCt_"
#endif
typedef int SM_CHANNEL_DISPATCHER_CONNECT_FN(struct sm_channel_dispatcher_connect_parms *dp);

ACUAPI int ACUTiNG_WINAPI sm_channel_dispatcher_connect(struct sm_channel_dispatcher_connect_parms *dp);


typedef struct sm_channel_datafeed_parms {
	tSMChannelId channel;
	tSMDatafeedId datafeed;
} SM_CHANNEL_DATAFEED_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_channel_get_datafeed sm_channel_get_datafeed_iPsCtDt_
#define SM_CHANNEL_GET_DATAFEED_NAME "sm_channel_get_datafeed_iPsCtDt_"
#endif
typedef int SM_CHANNEL_GET_DATAFEED_FN(struct sm_channel_datafeed_parms *datafeedp);

ACUAPI int ACUTiNG_WINAPI sm_channel_get_datafeed(struct sm_channel_datafeed_parms *datafeedp);


typedef struct sm_channel_info_parms {
	tSMChannelId channel;
	tSM_INT card;
	tSM_INT ost;
	tSM_INT ots;
	enum kSMTimeslotType otype;
	tSM_INT ist;
	tSM_INT its;
	enum kSMTimeslotType itype;
	tSM_INT group;
	tSM_INT caps_mask;
} SM_CHANNEL_INFO_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_channel_info sm_channel_info_iPsCtiiie1_3_iie1_3_ii_
#define SM_CHANNEL_INFO_NAME "sm_channel_info_iPsCtiiie1_3_iie1_3_ii_"
#endif
typedef int SM_CHANNEL_INFO_FN(struct sm_channel_info_parms *infop);

ACUAPI int ACUTiNG_WINAPI sm_channel_info(struct sm_channel_info_parms *infop);


typedef struct sm_channel_input_data_redirect_parms {
	tSMChannelId channel;
	tSM_INT enable;
	tSMDatafeedId datafeed;
} SM_CHANNEL_INPUT_DATA_REDIRECT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_channel_input_data_redirect sm_channel_input_data_redirect_iPsCtiDt_
#define SM_CHANNEL_INPUT_DATA_REDIRECT_NAME "sm_channel_input_data_redirect_iPsCtiDt_"
#endif
typedef int SM_CHANNEL_INPUT_DATA_REDIRECT_FN(struct sm_channel_input_data_redirect_parms *redp);

ACUAPI int ACUTiNG_WINAPI sm_channel_input_data_redirect(struct sm_channel_input_data_redirect_parms *redp);


typedef struct sm_channel_output_source_datafeed_connect_parms {
	tSMChannelId channel;
	tSMDatafeedId data_source;
	tSM_INT threshold;
	tSM_INT timeout;
} SM_CHANNEL_OUTPUT_SOURCE_DATAFEED_CONNECT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_channel_output_source_datafeed_connect sm_channel_output_source_datafeed_connect_iPsCtDtii_
#define SM_CHANNEL_OUTPUT_SOURCE_DATAFEED_CONNECT_NAME "sm_channel_output_source_datafeed_connect_iPsCtDtii_"
#endif
typedef int SM_CHANNEL_OUTPUT_SOURCE_DATAFEED_CONNECT_FN(struct sm_channel_output_source_datafeed_connect_parms *outsrcp);

ACUAPI int ACUTiNG_WINAPI sm_channel_output_source_datafeed_connect(struct sm_channel_output_source_datafeed_connect_parms *outsrcp);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_channel_release sm_channel_release_iCt
#define SM_CHANNEL_RELEASE_NAME "sm_channel_release_iCt"
#endif
typedef int SM_CHANNEL_RELEASE_FN(tSMChannelId channel);

ACUAPI int ACUTiNG_WINAPI sm_channel_release(tSMChannelId channel);


typedef struct sm_channel_set_event_parms {
	tSMChannelId channel;
	tSM_INT event_type;
	tSM_INT issue_events;
	tSMEventId event;
} SM_CHANNEL_SET_EVENT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_channel_set_event sm_channel_set_event_iPsCtiiEt_
#define SM_CHANNEL_SET_EVENT_NAME "sm_channel_set_event_iPsCtiiEt_"
#endif
typedef int SM_CHANNEL_SET_EVENT_FN(struct sm_channel_set_event_parms *eventp);

ACUAPI int ACUTiNG_WINAPI sm_channel_set_event(struct sm_channel_set_event_parms *eventp);


typedef struct sm_channel_set_output_rate_parms {
	tSMChannelId channel;
	tSM_INT sample_rate;
} SM_CHANNEL_SET_OUTPUT_RATE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_channel_set_output_rate sm_channel_set_output_rate_iPsCti_
#define SM_CHANNEL_SET_OUTPUT_RATE_NAME "sm_channel_set_output_rate_iPsCti_"
#endif
typedef int SM_CHANNEL_SET_OUTPUT_RATE_FN(struct sm_channel_set_output_rate_parms *ratep);

ACUAPI int ACUTiNG_WINAPI sm_channel_set_output_rate(struct sm_channel_set_output_rate_parms *ratep);


typedef struct sm_close_module_parms {
	tSMModuleId module_id;
} SM_CLOSE_MODULE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_close_module sm_close_module_iPsMt_
#define SM_CLOSE_MODULE_NAME "sm_close_module_iPsMt_"
#endif
typedef int SM_CLOSE_MODULE_FN(struct sm_close_module_parms *closep);

ACUAPI int ACUTiNG_WINAPI sm_close_module(struct sm_close_module_parms *closep);


typedef struct sm_close_prosody_parms {
	tSMCardId card_id;
} SM_CLOSE_PROSODY_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_close_prosody sm_close_prosody_iPsCAt_
#define SM_CLOSE_PROSODY_NAME "sm_close_prosody_iPsCAt_"
#endif
typedef int SM_CLOSE_PROSODY_FN(struct sm_close_prosody_parms *closep);

ACUAPI int ACUTiNG_WINAPI sm_close_prosody(struct sm_close_prosody_parms *closep);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_collector_destroy sm_collector_destroy_iCe
#define SM_COLLECTOR_DESTROY_NAME "sm_collector_destroy_iCe"
#endif
typedef int SM_COLLECTOR_DESTROY_FN(tSMCollectorId collector);

ACUAPI int ACUTiNG_WINAPI sm_collector_destroy(tSMCollectorId collector);


typedef struct sm_collector_event_parms {
	tSMCollectorId collector;
	tSMEventId event;
} SM_COLLECTOR_EVENT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_collector_get_event sm_collector_get_event_iPsCeEt_
#define SM_COLLECTOR_GET_EVENT_NAME "sm_collector_get_event_iPsCeEt_"
#endif
typedef int SM_COLLECTOR_GET_EVENT_FN(struct sm_collector_event_parms *eventp);

ACUAPI int ACUTiNG_WINAPI sm_collector_get_event(struct sm_collector_event_parms *eventp);


typedef struct sm_collector_status_parms {
	tSMCollectorId collector;
	enum kSMCollectorStatus status;
	union {
		struct {
			struct in_addr address;
			int port;
		} ports;
		struct {
			struct in_addr address;
			int port;
		} peer;
		struct {
			struct in6_addr address;
			int port;
		} ports_ipv6;
		struct {
			struct in6_addr address;
			int port;
		} peer_ipv6;
	} u;
} SM_COLLECTOR_STATUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_collector_status sm_collector_status_iPsCee1_5_usSIdi_sSIdi_sSI6di_sSI6di___
#define SM_COLLECTOR_STATUS_NAME "sm_collector_status_iPsCee1_5_usSIdi_sSIdi_sSI6di_sSI6di___"
#endif
typedef int SM_COLLECTOR_STATUS_FN(struct sm_collector_status_parms *statusp);

ACUAPI int ACUTiNG_WINAPI sm_collector_status(struct sm_collector_status_parms *statusp);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_dispatcher_destroy sm_dispatcher_destroy_iDe
#define SM_DISPATCHER_DESTROY_NAME "sm_dispatcher_destroy_iDe"
#endif
typedef int SM_DISPATCHER_DESTROY_FN(tSMDispatcherId dispatcher);

ACUAPI int ACUTiNG_WINAPI sm_dispatcher_destroy(tSMDispatcherId dispatcher);


typedef struct sm_error_name_parms {
	tSM_INT code;
	char name[80];
} SM_ERROR_NAME_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_error_name sm_error_name_PcPsicr80_
#define SM_ERROR_NAME_NAME "sm_error_name_PcPsicr80_"
#endif
typedef char *SM_ERROR_NAME_FN(struct sm_error_name_parms *errp);

ACUAPI char *ACUTiNG_WINAPI sm_error_name(struct sm_error_name_parms *errp);


typedef struct sm_card_info_parms {
	tSMCardId card;
	enum kSMCarrierCardType card_type;
	tSM_INT card_detected;
	tSM_INT module_count;
	tSM_UT32 physical_address;
	tSM_UT32 io_address;
	tSM_UT32 physical_irq;
	char serial_no[kSMMaxSerialNoText];
} SM_CARD_INFO_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_get_card_info sm_get_card_info_iPsCAte1_8_iiUiUiUicr16_
#define SM_GET_CARD_INFO_NAME "sm_get_card_info_iPsCAte1_8_iiUiUiUicr16_"
#endif
typedef int SM_GET_CARD_INFO_FN(struct sm_card_info_parms *cardinfop);

ACUAPI int ACUTiNG_WINAPI sm_get_card_info(struct sm_card_info_parms *cardinfop);


typedef struct sm_card_rev_parms {
	tSMCardId card;
	tSM_INT rev[8];
	char rev_additional[kSMMaxRevAddText];
} SM_CARD_REV_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_get_card_rev sm_get_card_rev_iPsCAtir8cr64_
#define SM_GET_CARD_REV_NAME "sm_get_card_rev_iPsCAtir8cr64_"
#endif
typedef int SM_GET_CARD_REV_FN(struct sm_card_rev_parms *cardrevp);

ACUAPI int ACUTiNG_WINAPI sm_get_card_rev(struct sm_card_rev_parms *cardrevp);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_get_channel_type sm_get_channel_type_iCt
#define SM_GET_CHANNEL_TYPE_NAME "sm_get_channel_type_iCt"
#endif
typedef int SM_GET_CHANNEL_TYPE_FN(tSMChannelId channel);

ACUAPI int ACUTiNG_WINAPI sm_get_channel_type(tSMChannelId channel);


typedef struct sm_driver_info_parms {
	tSM_INT major;
	tSM_INT minor;
	tSM_INT step;
	tSM_INT custom;
	tSM_INT quality;
	tSM_INT buildno0;
	tSM_INT buildno1;
} SM_DRIVER_INFO_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_get_driver_info sm_get_driver_info_iPsiiiiiii_
#define SM_GET_DRIVER_INFO_NAME "sm_get_driver_info_iPsiiiiiii_"
#endif
typedef int SM_GET_DRIVER_INFO_FN(struct sm_driver_info_parms *drvinfop);

ACUAPI int ACUTiNG_WINAPI sm_get_driver_info(struct sm_driver_info_parms *drvinfop);


typedef struct sm_module_info_parms {
	tSMModuleId module;
	tSM_INT firmware_running;
	tSM_INT firmware_id;
	tSM_INT average_loading;
	tSM_INT max_loading;
	tSM_INT module_slot;
	tSM_INT min_stream;
	tSM_INT stream_count;
} SM_MODULE_INFO_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_get_module_info sm_get_module_info_iPsMtiiiiiii_
#define SM_GET_MODULE_INFO_NAME "sm_get_module_info_iPsMtiiiiiii_"
#endif
typedef int SM_GET_MODULE_INFO_FN(struct sm_module_info_parms *moduleinfop);

ACUAPI int ACUTiNG_WINAPI sm_get_module_info(struct sm_module_info_parms *moduleinfop);


typedef struct sm_stream_info_parms {
	tSMModuleId module;
	tSM_INT stream;
	char serial_no[kSMMaxSerialNoText];
	tSM_INT serial_port_ix;
	tSM_INT timeslot_count;
} SM_STREAM_INFO_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_get_stream_info sm_get_stream_info_iPsMticr16ii_
#define SM_GET_STREAM_INFO_NAME "sm_get_stream_info_iPsMticr16ii_"
#endif
typedef int SM_GET_STREAM_INFO_FN(struct sm_stream_info_parms *streaminfop);

ACUAPI int ACUTiNG_WINAPI sm_get_stream_info(struct sm_stream_info_parms *streaminfop);


typedef struct sm_module_event_parms {
	tSMModuleId module_id;
	tSMEventId event;
} SM_MODULE_EVENT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_module_get_event sm_module_get_event_iPsMtEt_
#define SM_MODULE_GET_EVENT_NAME "sm_module_get_event_iPsMtEt_"
#endif
typedef int SM_MODULE_GET_EVENT_FN(struct sm_module_event_parms *eventp);

ACUAPI int ACUTiNG_WINAPI sm_module_get_event(struct sm_module_event_parms *eventp);


typedef struct sm_module_status_parms {
	tSMModuleId module_id;
	enum kSMModuleStatus status;
} SM_MODULE_STATUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_module_status sm_module_status_iPsMte1_2__
#define SM_MODULE_STATUS_NAME "sm_module_status_iPsMte1_2__"
#endif
typedef int SM_MODULE_STATUS_FN(struct sm_module_status_parms *statusp);

ACUAPI int ACUTiNG_WINAPI sm_module_status(struct sm_module_status_parms *statusp);


typedef struct sm_open_module_parms {
	tSMCardId card_id;
	tSM_UT32 module_ix;
	tSMModuleId module_id;
} SM_OPEN_MODULE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_open_module sm_open_module_iPsCAtUiMt_
#define SM_OPEN_MODULE_NAME "sm_open_module_iPsCAtUiMt_"
#endif
typedef int SM_OPEN_MODULE_FN(struct sm_open_module_parms *module_parmsp);

ACUAPI int ACUTiNG_WINAPI sm_open_module(struct sm_open_module_parms *module_parmsp);


typedef struct sm_sec_module_parms {
	tSMModuleId module_id;
	tSM_INT exclude_data_to_module;
	tSM_INT exclude_data_from_module;
	tSM_INT key_id;
	char *context;
	tSMSecCallback encdec;
} SM_SEC_MODULE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_sec_module sm_sec_module_iPsMtiiiPcSCf_
#define SM_SEC_MODULE_NAME "sm_sec_module_iPsMtiiiPcSCf_"
#endif
typedef int SM_SEC_MODULE_FN(struct sm_sec_module_parms *module_parmsp);

ACUAPI int ACUTiNG_WINAPI sm_sec_module(struct sm_sec_module_parms *module_parmsp);


typedef struct sm_shutdown_module_parms {
	tSMModuleId module_id;
} SM_SHUTDOWN_MODULE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_shutdown_module sm_shutdown_module_iPsMt_
#define SM_SHUTDOWN_MODULE_NAME "sm_shutdown_module_iPsMt_"
#endif
typedef int SM_SHUTDOWN_MODULE_FN(struct sm_shutdown_module_parms *shutdownp);

ACUAPI int ACUTiNG_WINAPI sm_shutdown_module(struct sm_shutdown_module_parms *shutdownp);


typedef struct sm_switch_channel_parms {
	tSMChannelId channel;
	tSM_INT st;
	tSM_INT ts;
	enum kSMTimeslotType type;
} SM_SWITCH_CHANNEL_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_switch_channel_input sm_switch_channel_input_iPsCtiie1_3__
#define SM_SWITCH_CHANNEL_INPUT_NAME "sm_switch_channel_input_iPsCtiie1_3__"
#endif
typedef int SM_SWITCH_CHANNEL_INPUT_FN(struct sm_switch_channel_parms *switchp);

ACUAPI int ACUTiNG_WINAPI sm_switch_channel_input(struct sm_switch_channel_parms *switchp);



#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_switch_channel_output sm_switch_channel_output_iPsCtiie1_3__
#define SM_SWITCH_CHANNEL_OUTPUT_NAME "sm_switch_channel_output_iPsCtiie1_3__"
#endif
typedef int SM_SWITCH_CHANNEL_OUTPUT_FN(struct sm_switch_channel_parms *switchp);

ACUAPI int ACUTiNG_WINAPI sm_switch_channel_output(struct sm_switch_channel_parms *switchp);


typedef struct sm_tdmrx_create_parms {
	tSMTDMrxId tdmrx;
	tSMModuleId module;
	tSM_INT stream;
	tSM_INT timeslot;
	enum kSMTimeslotType type;
} SM_TDMRX_CREATE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_tdmrx_create sm_tdmrx_create_iPsTrMtiie1_3__
#define SM_TDMRX_CREATE_NAME "sm_tdmrx_create_iPsTrMtiie1_3__"
#endif
typedef int SM_TDMRX_CREATE_FN(struct sm_tdmrx_create_parms *tdmrxp);

ACUAPI int ACUTiNG_WINAPI sm_tdmrx_create(struct sm_tdmrx_create_parms *tdmrxp);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_tdmrx_destroy sm_tdmrx_destroy_iTr
#define SM_TDMRX_DESTROY_NAME "sm_tdmrx_destroy_iTr"
#endif
typedef int SM_TDMRX_DESTROY_FN(tSMTDMrxId tdmrx);

ACUAPI int ACUTiNG_WINAPI sm_tdmrx_destroy(tSMTDMrxId tdmrx);


typedef struct sm_tdmrx_datafeed_parms {
	tSMTDMrxId tdmrx;
	tSMDatafeedId datafeed;
} SM_TDMRX_DATAFEED_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_tdmrx_get_datafeed sm_tdmrx_get_datafeed_iPsTrDt_
#define SM_TDMRX_GET_DATAFEED_NAME "sm_tdmrx_get_datafeed_iPsTrDt_"
#endif
typedef int SM_TDMRX_GET_DATAFEED_FN(struct sm_tdmrx_datafeed_parms *datafeedp);

ACUAPI int ACUTiNG_WINAPI sm_tdmrx_get_datafeed(struct sm_tdmrx_datafeed_parms *datafeedp);


typedef struct sm_tdmtx_create_parms {
	tSMTDMtxId tdmtx;
	tSMModuleId module;
	tSM_INT stream;
	tSM_INT timeslot;
	enum kSMTimeslotType type;
} SM_TDMTX_CREATE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_tdmtx_create sm_tdmtx_create_iPsTtMtiie1_3__
#define SM_TDMTX_CREATE_NAME "sm_tdmtx_create_iPsTtMtiie1_3__"
#endif
typedef int SM_TDMTX_CREATE_FN(struct sm_tdmtx_create_parms *tdmtxp);

ACUAPI int ACUTiNG_WINAPI sm_tdmtx_create(struct sm_tdmtx_create_parms *tdmtxp);


typedef struct sm_tdmtx_datafeed_connect_parms {
	tSMDatafeedId data_source;
	tSMTDMtxId tdmtx;
} SM_TDMTX_DATAFEED_CONNECT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_tdmtx_datafeed_connect sm_tdmtx_datafeed_connect_iPsDtTt_
#define SM_TDMTX_DATAFEED_CONNECT_NAME "sm_tdmtx_datafeed_connect_iPsDtTt_"
#endif
typedef int SM_TDMTX_DATAFEED_CONNECT_FN(struct sm_tdmtx_datafeed_connect_parms *datafeedp);

ACUAPI int ACUTiNG_WINAPI sm_tdmtx_datafeed_connect(struct sm_tdmtx_datafeed_connect_parms *datafeedp);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_tdmtx_destroy sm_tdmtx_destroy_iTt
#define SM_TDMTX_DESTROY_NAME "sm_tdmtx_destroy_iTt"
#endif
typedef int SM_TDMTX_DESTROY_FN(tSMTDMtxId tdmtx);

ACUAPI int ACUTiNG_WINAPI sm_tdmtx_destroy(tSMTDMtxId tdmtx);


typedef struct sm_udp_collector_create_parms {
	tSMCollectorId collector;
	tSMModuleId module;
	enum kSMCollectorType type;
	struct in_addr address;
	struct in6_addr ipv6_address;
} SM_UDP_COLLECTOR_CREATE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_udp_collector_create sm_udp_collector_create_iPsCeMte1_2_SIdSI6d_
#define SM_UDP_COLLECTOR_CREATE_NAME "sm_udp_collector_create_iPsCeMte1_2_SIdSI6d_"
#endif
typedef int SM_UDP_COLLECTOR_CREATE_FN(struct sm_udp_collector_create_parms *dp);

ACUAPI int ACUTiNG_WINAPI sm_udp_collector_create(struct sm_udp_collector_create_parms *dp);


typedef struct sm_udp_dispatcher_create_parms {
	tSMDispatcherId dispatcher;
	tSMModuleId module;
	enum kSMDispatcherType type;
	SOCKADDR_IN destination;
	SOCKADDR_IN source;
	int TOS;
	SOCKADDR_IN6 destination_ipv6;
	SOCKADDR_IN6 source_ipv6;
} SM_UDP_DISPATCHER_CREATE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_udp_dispatcher_create sm_udp_dispatcher_create_iPsDeMte1_2_SAdSAdiSA6dSA6d_
#define SM_UDP_DISPATCHER_CREATE_NAME "sm_udp_dispatcher_create_iPsDeMte1_2_SAdSAdiSA6dSA6d_"
#endif
typedef int SM_UDP_DISPATCHER_CREATE_FN(struct sm_udp_dispatcher_create_parms *dp);

ACUAPI int ACUTiNG_WINAPI sm_udp_dispatcher_create(struct sm_udp_dispatcher_create_parms *dp);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define smd_ev_create smd_ev_create_iPEtCtii
#define SMD_EV_CREATE_NAME "smd_ev_create_iPEtCtii"
#endif
typedef int SMD_EV_CREATE_FN(tSMEventId *eventId, tSMChannelId channelId, int eventType, int eventChannelBinding);

ACUAPI int ACUTiNG_WINAPI smd_ev_create(tSMEventId *eventId, tSMChannelId channelId, int eventType, int eventChannelBinding);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define smd_ev_free smd_ev_free_iEt
#define SMD_EV_FREE_NAME "smd_ev_free_iEt"
#endif
typedef int SMD_EV_FREE_FN(tSMEventId eventId);

ACUAPI int ACUTiNG_WINAPI smd_ev_free(tSMEventId eventId);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define smd_ev_wait smd_ev_wait_iEt
#define SMD_EV_WAIT_NAME "smd_ev_wait_iEt"
#endif
typedef int SMD_EV_WAIT_FN(tSMEventId eventId);

ACUAPI int ACUTiNG_WINAPI smd_ev_wait(tSMEventId eventId);


typedef struct sm_config_module_sw_parms {
	tSMModuleId module;
	tSM_INT auto_assign_out_ts;
	tSM_INT auto_assign_in_ts;
	tSM_INT out_st0;
	tSM_INT out_st1;
	tSM_INT out_base_ts;
	enum kSMTimeslotType in_type;
	tSM_INT in_st0;
	tSM_INT in_st1;
	tSM_INT in_base_ts;
	enum kSMTimeslotType out_type;
} SM_CONFIG_MODULE_SW_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_config_module_switching sm_config_module_switching_iPsMtiiiiie1_3_iiie1_3__
#define SM_CONFIG_MODULE_SWITCHING_NAME "sm_config_module_switching_iPsMtiiiiie1_3_iiie1_3__"
#endif
typedef int SM_CONFIG_MODULE_SWITCHING_FN(struct sm_config_module_sw_parms *configp);

ACUAPI int ACUTiNG_WINAPI sm_config_module_switching(struct sm_config_module_sw_parms *configp);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_get_card_switch_id sm_get_card_switch_id_iCAt
#define SM_GET_CARD_SWITCH_ID_NAME "sm_get_card_switch_id_iCAt"
#endif
typedef int SM_GET_CARD_SWITCH_ID_FN(tSMCardId card);

ACUAPI int ACUTiNG_WINAPI sm_get_card_switch_id(tSMCardId card);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_get_channel_ix sm_get_channel_ix_iCt
#define SM_GET_CHANNEL_IX_NAME "sm_get_channel_ix_iCt"
#endif
typedef int SM_GET_CHANNEL_IX_FN(tSMChannelId channel);

ACUAPI int ACUTiNG_WINAPI sm_get_channel_ix(tSMChannelId channel);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_get_channel_module_id sm_get_channel_module_id_iCt
#define SM_GET_CHANNEL_MODULE_ID_NAME "sm_get_channel_module_id_iCt"
#endif
typedef int SM_GET_CHANNEL_MODULE_ID_FN(tSMChannelId channel);

ACUAPI int ACUTiNG_WINAPI sm_get_channel_module_id(tSMChannelId channel);


typedef struct sm_fwinfo_parms {
	tSMModuleId module;
	tSM_UT32 position;
	tSM_UT32 tasktypecode;
	tSM_UT32 version;
} SM_FWINFO_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_get_firmware_info sm_get_firmware_info_iPsMtUiUiUi_
#define SM_GET_FIRMWARE_INFO_NAME "sm_get_firmware_info_iPsMtUiUiUi_"
#endif
typedef int SM_GET_FIRMWARE_INFO_FN(struct sm_fwinfo_parms *fp);

ACUAPI int ACUTiNG_WINAPI sm_get_firmware_info(struct sm_fwinfo_parms *fp);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_get_module_card_id sm_get_module_card_id_iMt
#define SM_GET_MODULE_CARD_ID_NAME "sm_get_module_card_id_iMt"
#endif
typedef int SM_GET_MODULE_CARD_ID_FN(tSMModuleId mod_id);

ACUAPI int ACUTiNG_WINAPI sm_get_module_card_id(tSMModuleId mod_id);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_reset_module sm_reset_module_iMt
#define SM_RESET_MODULE_NAME "sm_reset_module_iMt"
#endif
typedef int SM_RESET_MODULE_FN(tSMModuleId module);

ACUAPI int ACUTiNG_WINAPI sm_reset_module(tSMModuleId module);



#endif
