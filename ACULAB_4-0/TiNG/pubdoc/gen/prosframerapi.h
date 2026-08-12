/* prosframerapi.h - Prosody Framer API declarations */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

#ifndef INCLUDED_PUBDOC_PROSFRAMERAPI_H
#define INCLUDED_PUBDOC_PROSFRAMERAPI_H

#ifdef ACULAB_TiNG_VERSION_NUM
#if ACULAB_TiNG_VERSION_NUM != 1504
#error ACULAB_TiNG_VERSION_NUM does not match 1504
#endif
#else
#define ACULAB_TiNG_VERSION_NUM 1504
#define ACULAB_TiNG_VERSION_CHANGED 
#define ACULAB_TiNG_VERSION_STRING "m1504."
#endif

enum kSMFramerStatus {
	kSMFramerStatusRunning,
	kSMFramerStatusStopped,
	kSMFramerStatusNewState,
};



typedef struct sm_framer_create_parms {
	tSMFramerId framer;
	tSMModuleId module;
} SM_FRAMER_CREATE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_framer_create sm_framer_create_iPsFRtMt_
#define SM_FRAMER_CREATE_NAME "sm_framer_create_iPsFRtMt_"
#endif
typedef int SM_FRAMER_CREATE_FN(struct sm_framer_create_parms *createp);

ACUAPI int ACUTiNG_WINAPI sm_framer_create(struct sm_framer_create_parms *createp);


typedef struct sm_framer_datafeed_connect_parms {
	tSMDatafeedId data_source;
	tSMFramerId framer;
	tSM_INT direction;
} SM_FRAMER_DATAFEED_CONNECT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_framer_datafeed_connect sm_framer_datafeed_connect_iPsDtFRti_
#define SM_FRAMER_DATAFEED_CONNECT_NAME "sm_framer_datafeed_connect_iPsDtFRti_"
#endif
typedef int SM_FRAMER_DATAFEED_CONNECT_FN(struct sm_framer_datafeed_connect_parms *datafeedp);

ACUAPI int ACUTiNG_WINAPI sm_framer_datafeed_connect(struct sm_framer_datafeed_connect_parms *datafeedp);


typedef struct sm_framer_datafeed_disconnect_parms {
	tSMFramerId framer;
	tSM_INT direction;
} SM_FRAMER_DATAFEED_DISCONNECT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_framer_datafeed_disconnect sm_framer_datafeed_disconnect_iPsFRti_
#define SM_FRAMER_DATAFEED_DISCONNECT_NAME "sm_framer_datafeed_disconnect_iPsFRti_"
#endif
typedef int SM_FRAMER_DATAFEED_DISCONNECT_FN(struct sm_framer_datafeed_disconnect_parms *datafeedp);

ACUAPI int ACUTiNG_WINAPI sm_framer_datafeed_disconnect(struct sm_framer_datafeed_disconnect_parms *datafeedp);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_framer_destroy sm_framer_destroy_iFRt
#define SM_FRAMER_DESTROY_NAME "sm_framer_destroy_iFRt"
#endif
typedef int SM_FRAMER_DESTROY_FN(tSMFramerId framer);

ACUAPI int ACUTiNG_WINAPI sm_framer_destroy(tSMFramerId framer);


typedef struct sm_framer_datafeed_parms {
	tSMFramerId framer;
	tSM_INT direction;
	tSMDatafeedId datafeed;
} SM_FRAMER_DATAFEED_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_framer_get_datafeed sm_framer_get_datafeed_iPsFRtiDt_
#define SM_FRAMER_GET_DATAFEED_NAME "sm_framer_get_datafeed_iPsFRtiDt_"
#endif
typedef int SM_FRAMER_GET_DATAFEED_FN(struct sm_framer_datafeed_parms *datafeedp);

ACUAPI int ACUTiNG_WINAPI sm_framer_get_datafeed(struct sm_framer_datafeed_parms *datafeedp);


typedef struct sm_framer_event_parms {
	tSMFramerId framer;
	tSMEventId event;
} SM_FRAMER_EVENT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_framer_get_event sm_framer_get_event_iPsFRtEt_
#define SM_FRAMER_GET_EVENT_NAME "sm_framer_get_event_iPsFRtEt_"
#endif
typedef int SM_FRAMER_GET_EVENT_FN(struct sm_framer_event_parms *eventp);

ACUAPI int ACUTiNG_WINAPI sm_framer_get_event(struct sm_framer_event_parms *eventp);


typedef struct sm_framer_status_parms {
	tSMFramerId framer;
	enum kSMFramerStatus status;
	tSM_INT new_state;
} SM_FRAMER_STATUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_framer_status sm_framer_status_iPsFRte1_3_i_
#define SM_FRAMER_STATUS_NAME "sm_framer_status_iPsFRte1_3_i_"
#endif
typedef int SM_FRAMER_STATUS_FN(struct sm_framer_status_parms *statusp);

ACUAPI int ACUTiNG_WINAPI sm_framer_status(struct sm_framer_status_parms *statusp);



#endif
