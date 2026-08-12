/* prosi460muxapi.h - Prosody I.460 Mux API declarations */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

#ifndef INCLUDED_PUBDOC_PROSI460MUXAPI_H
#define INCLUDED_PUBDOC_PROSI460MUXAPI_H

#ifdef ACULAB_TiNG_VERSION_NUM
#if ACULAB_TiNG_VERSION_NUM != 1504
#error ACULAB_TiNG_VERSION_NUM does not match 1504
#endif
#else
#define ACULAB_TiNG_VERSION_NUM 1504
#define ACULAB_TiNG_VERSION_CHANGED 
#define ACULAB_TiNG_VERSION_STRING "m1504."
#endif

enum kSMI460muxStatus {
	kSMI460muxStatusRunning,
	kSMI460muxStatusStopped,
};



typedef struct sm_i460mux_create_parms {
	tSMI460muxId i460mux;
	tSMModuleId module;
} SM_I460MUX_CREATE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_i460mux_create sm_i460mux_create_iPsIMtMt_
#define SM_I460MUX_CREATE_NAME "sm_i460mux_create_iPsIMtMt_"
#endif
typedef int SM_I460MUX_CREATE_FN(struct sm_i460mux_create_parms *createp);

ACUAPI int ACUTiNG_WINAPI sm_i460mux_create(struct sm_i460mux_create_parms *createp);


typedef struct sm_i460mux_datafeed_connect_parms {
	tSMDatafeedId data_source;
	tSMI460muxId i460mux;
	tSMI460InputId input_id;
} SM_I460MUX_DATAFEED_CONNECT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_i460mux_datafeed_connect sm_i460mux_datafeed_connect_iPsDtIMtIPt_
#define SM_I460MUX_DATAFEED_CONNECT_NAME "sm_i460mux_datafeed_connect_iPsDtIMtIPt_"
#endif
typedef int SM_I460MUX_DATAFEED_CONNECT_FN(struct sm_i460mux_datafeed_connect_parms *datafeedp);

ACUAPI int ACUTiNG_WINAPI sm_i460mux_datafeed_connect(struct sm_i460mux_datafeed_connect_parms *datafeedp);


typedef struct sm_i460mux_datafeed_disconnect_parms {
	tSMI460muxId i460mux;
	tSMI460InputId input_id;
} SM_I460MUX_DATAFEED_DISCONNECT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_i460mux_datafeed_disconnect sm_i460mux_datafeed_disconnect_iPsIMtIPt_
#define SM_I460MUX_DATAFEED_DISCONNECT_NAME "sm_i460mux_datafeed_disconnect_iPsIMtIPt_"
#endif
typedef int SM_I460MUX_DATAFEED_DISCONNECT_FN(struct sm_i460mux_datafeed_disconnect_parms *datafeedp);

ACUAPI int ACUTiNG_WINAPI sm_i460mux_datafeed_disconnect(struct sm_i460mux_datafeed_disconnect_parms *datafeedp);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_i460mux_destroy sm_i460mux_destroy_iIMt
#define SM_I460MUX_DESTROY_NAME "sm_i460mux_destroy_iIMt"
#endif
typedef int SM_I460MUX_DESTROY_FN(tSMI460muxId mux);

ACUAPI int ACUTiNG_WINAPI sm_i460mux_destroy(tSMI460muxId mux);


typedef struct sm_i460mux_datafeed_parms {
	tSMI460muxId i460mux;
	tSMDatafeedId datafeed;
} SM_I460MUX_DATAFEED_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_i460mux_get_datafeed sm_i460mux_get_datafeed_iPsIMtDt_
#define SM_I460MUX_GET_DATAFEED_NAME "sm_i460mux_get_datafeed_iPsIMtDt_"
#endif
typedef int SM_I460MUX_GET_DATAFEED_FN(struct sm_i460mux_datafeed_parms *datafeedp);

ACUAPI int ACUTiNG_WINAPI sm_i460mux_get_datafeed(struct sm_i460mux_datafeed_parms *datafeedp);


typedef struct sm_i460mux_event_parms {
	tSMI460muxId i460mux;
	tSMEventId event;
} SM_I460MUX_EVENT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_i460mux_get_event sm_i460mux_get_event_iPsIMtEt_
#define SM_I460MUX_GET_EVENT_NAME "sm_i460mux_get_event_iPsIMtEt_"
#endif
typedef int SM_I460MUX_GET_EVENT_FN(struct sm_i460mux_event_parms *eventp);

ACUAPI int ACUTiNG_WINAPI sm_i460mux_get_event(struct sm_i460mux_event_parms *eventp);


typedef struct sm_i460mux_status_parms {
	tSMI460muxId i460mux;
	enum kSMI460muxStatus status;
} SM_I460MUX_STATUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_i460mux_status sm_i460mux_status_iPsIMte1_2__
#define SM_I460MUX_STATUS_NAME "sm_i460mux_status_iPsIMte1_2__"
#endif
typedef int SM_I460MUX_STATUS_FN(struct sm_i460mux_status_parms *statusp);

ACUAPI int ACUTiNG_WINAPI sm_i460mux_status(struct sm_i460mux_status_parms *statusp);



#endif
