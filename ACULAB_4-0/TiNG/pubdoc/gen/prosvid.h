/* prosvid.h - Prosody Audio Video Format (AVF) API declarations */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

#ifndef INCLUDED_PUBDOC_PROSVID_H
#define INCLUDED_PUBDOC_PROSVID_H

#ifdef ACULAB_TiNG_VERSION_NUM
#if ACULAB_TiNG_VERSION_NUM != 1504
#error ACULAB_TiNG_VERSION_NUM does not match 1504
#endif
#else
#define ACULAB_TiNG_VERSION_NUM 1504
#define ACULAB_TiNG_VERSION_CHANGED 
#define ACULAB_TiNG_VERSION_STRING "m1504."
#endif

enum kSMAVFOutputType {
	kSMAVFOutputAVFVideo,
	kSMAVFOutputAVFAudio,
	kSMAVFOutputLinearAudio,
};

enum kSMAudioDataFormat {
	kSMAudioDataFormatALawPCM,
	kSMAudioDataFormatULawPCM,
	kSMAudioDataFormatAMRNB,
	kSMAudioDataFormat16bit,
	kSMAudioDataFormat8bit,
};

enum kSMAVFplayStatus {
	kSMAVFplayStatusRunning,
	kSMAVFplayStatusUnderrun,
	kSMAVFplayStatusCompleteData,
	kSMAVFplayStatusComplete,
};

enum kSMAVFInputType {
	kSMAVFInputAVFVideo,
	kSMAVFInputAVFAudio,
	kSMAVFInputLinearAudio,
};

enum kSMAVFrecordStatus {
	kSMAVFrecordStatusRunning,
	kSMAVFrecordStatusOverrun,
	kSMAVFrecordStatusCompleting,
	kSMAVFrecordStatusComplete,
};



typedef struct sm_avfplay_create_parms {
	tSMAVFplayId avfplay;
	tSMModuleId module;
} SM_AVFPLAY_CREATE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_avfplay_create sm_avfplay_create_iPsVPtMt_
#define SM_AVFPLAY_CREATE_NAME "sm_avfplay_create_iPsVPtMt_"
#endif
typedef int SM_AVFPLAY_CREATE_FN(struct sm_avfplay_create_parms *avfcp);

ACUAPI int ACUTiNG_WINAPI sm_avfplay_create(struct sm_avfplay_create_parms *avfcp);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_avfplay_destroy sm_avfplay_destroy_iVPt
#define SM_AVFPLAY_DESTROY_NAME "sm_avfplay_destroy_iVPt"
#endif
typedef int SM_AVFPLAY_DESTROY_FN(tSMAVFplayId avfplay);

ACUAPI int ACUTiNG_WINAPI sm_avfplay_destroy(tSMAVFplayId avfplay);


typedef struct sm_avfplay_datafeed_parms {
	tSMAVFplayId avfplay;
	tSMDatafeedId datafeed;
} SM_AVFPLAY_DATAFEED_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_avfplay_get_datafeed sm_avfplay_get_datafeed_iPsVPtDt_
#define SM_AVFPLAY_GET_DATAFEED_NAME "sm_avfplay_get_datafeed_iPsVPtDt_"
#endif
typedef int SM_AVFPLAY_GET_DATAFEED_FN(struct sm_avfplay_datafeed_parms *datafeedp);

ACUAPI int ACUTiNG_WINAPI sm_avfplay_get_datafeed(struct sm_avfplay_datafeed_parms *datafeedp);


typedef struct sm_avfplay_event_parms {
	tSMAVFplayId avfplay;
	tSMEventId event;
} SM_AVFPLAY_EVENT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_avfplay_get_event sm_avfplay_get_event_iPsVPtEt_
#define SM_AVFPLAY_GET_EVENT_NAME "sm_avfplay_get_event_iPsVPtEt_"
#endif
typedef int SM_AVFPLAY_GET_EVENT_FN(struct sm_avfplay_event_parms *eventp);

ACUAPI int ACUTiNG_WINAPI sm_avfplay_get_event(struct sm_avfplay_event_parms *eventp);


typedef struct sm_avfplay_put_data_parms {
	tSMAVFplayId avfplay;
	char *data;
	tSM_INT max_length;
	tSM_INT done_length;
} SM_AVFPLAY_PUT_DATA_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_avfplay_put_data sm_avfplay_put_data_iPsVPtPcii_
#define SM_AVFPLAY_PUT_DATA_NAME "sm_avfplay_put_data_iPsVPtPcii_"
#endif
typedef int SM_AVFPLAY_PUT_DATA_FN(struct sm_avfplay_put_data_parms *datap);

ACUAPI int ACUTiNG_WINAPI sm_avfplay_put_data(struct sm_avfplay_put_data_parms *datap);


typedef struct sm_avfplay_start_parms {
	tSMAVFplayId avfplay;
	enum kSMAVFOutputType dest_type;
	union {
		struct {
			tSM_INT sampling_rate;
			enum kSMAudioDataFormat src_type;
		} audio;
	} u_dest_type;
	tSM_INT hbuf_size;
	tSM_INT test_mode;
} SM_AVFPLAY_START_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_avfplay_start sm_avfplay_start_iPsVPte1_3_usie1_5___ii_
#define SM_AVFPLAY_START_NAME "sm_avfplay_start_iPsVPte1_3_usie1_5___ii_"
#endif
typedef int SM_AVFPLAY_START_FN(struct sm_avfplay_start_parms *avfsp);

ACUAPI int ACUTiNG_WINAPI sm_avfplay_start(struct sm_avfplay_start_parms *avfsp);


typedef struct sm_avfplay_status_parms {
	tSMAVFplayId avfplay;
	enum kSMAVFplayStatus status;
} SM_AVFPLAY_STATUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_avfplay_status sm_avfplay_status_iPsVPte1_4__
#define SM_AVFPLAY_STATUS_NAME "sm_avfplay_status_iPsVPte1_4__"
#endif
typedef int SM_AVFPLAY_STATUS_FN(struct sm_avfplay_status_parms *statusp);

ACUAPI int ACUTiNG_WINAPI sm_avfplay_status(struct sm_avfplay_status_parms *statusp);


typedef struct sm_avfplay_stop_parms {
	tSMAVFplayId avfplay;
	tSM_INT immediate;
} SM_AVFPLAY_STOP_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_avfplay_stop sm_avfplay_stop_iPsVPti_
#define SM_AVFPLAY_STOP_NAME "sm_avfplay_stop_iPsVPti_"
#endif
typedef int SM_AVFPLAY_STOP_FN(struct sm_avfplay_stop_parms *avfsp);

ACUAPI int ACUTiNG_WINAPI sm_avfplay_stop(struct sm_avfplay_stop_parms *avfsp);


typedef struct sm_avfplay_sync_parms {
	tSMAVFplayId avfplay;
	tSMAVFplayId sync_avfplay;
} SM_AVFPLAY_SYNC_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_avfplay_sync sm_avfplay_sync_iPsVPtVPt_
#define SM_AVFPLAY_SYNC_NAME "sm_avfplay_sync_iPsVPtVPt_"
#endif
typedef int SM_AVFPLAY_SYNC_FN(struct sm_avfplay_sync_parms *avfsp);

ACUAPI int ACUTiNG_WINAPI sm_avfplay_sync(struct sm_avfplay_sync_parms *avfsp);


typedef struct sm_avfrec_create_parms {
	tSMAVFrecId avfrec;
	tSMModuleId module;
} SM_AVFREC_CREATE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_avfrec_create sm_avfrec_create_iPsVRtMt_
#define SM_AVFREC_CREATE_NAME "sm_avfrec_create_iPsVRtMt_"
#endif
typedef int SM_AVFREC_CREATE_FN(struct sm_avfrec_create_parms *vcp);

ACUAPI int ACUTiNG_WINAPI sm_avfrec_create(struct sm_avfrec_create_parms *vcp);


typedef struct sm_avfrec_datafeed_connect_parms {
	tSMAVFrecId avfrec;
	tSMDatafeedId data_source;
} SM_AVFREC_DATAFEED_CONNECT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_avfrec_datafeed_connect sm_avfrec_datafeed_connect_iPsVRtDt_
#define SM_AVFREC_DATAFEED_CONNECT_NAME "sm_avfrec_datafeed_connect_iPsVRtDt_"
#endif
typedef int SM_AVFREC_DATAFEED_CONNECT_FN(struct sm_avfrec_datafeed_connect_parms *datafeedp);

ACUAPI int ACUTiNG_WINAPI sm_avfrec_datafeed_connect(struct sm_avfrec_datafeed_connect_parms *datafeedp);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_avfrec_destroy sm_avfrec_destroy_iVRt
#define SM_AVFREC_DESTROY_NAME "sm_avfrec_destroy_iVRt"
#endif
typedef int SM_AVFREC_DESTROY_FN(tSMAVFrecId avfrec);

ACUAPI int ACUTiNG_WINAPI sm_avfrec_destroy(tSMAVFrecId avfrec);


typedef struct sm_avfrec_get_data_parms {
	tSMAVFrecId avfrec;
	char *data;
	tSM_INT max_length;
	tSM_INT done_length;
} SM_AVFREC_GET_DATA_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_avfrec_get_data sm_avfrec_get_data_iPsVRtPcii_
#define SM_AVFREC_GET_DATA_NAME "sm_avfrec_get_data_iPsVRtPcii_"
#endif
typedef int SM_AVFREC_GET_DATA_FN(struct sm_avfrec_get_data_parms *datap);

ACUAPI int ACUTiNG_WINAPI sm_avfrec_get_data(struct sm_avfrec_get_data_parms *datap);


typedef struct sm_avfrec_event_parms {
	tSMAVFrecId avfrec;
	tSMEventId event;
} SM_AVFREC_EVENT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_avfrec_get_event sm_avfrec_get_event_iPsVRtEt_
#define SM_AVFREC_GET_EVENT_NAME "sm_avfrec_get_event_iPsVRtEt_"
#endif
typedef int SM_AVFREC_GET_EVENT_FN(struct sm_avfrec_event_parms *eventp);

ACUAPI int ACUTiNG_WINAPI sm_avfrec_get_event(struct sm_avfrec_event_parms *eventp);


typedef struct sm_avfrec_start_parms {
	tSMAVFrecId avfrec;
	enum kSMAVFInputType src_type;
	union {
		struct {
			tSM_INT payload_type;
			enum kSMAudioDataFormat dest_type;
			union {
				struct {
					tSM_INT bitrate;
					tSM_INT vad_enable;
				} amrnb;
			} u_dest_type;
		} audio;
	} u_src_type;
	tSM_INT hbuf_size;
	tSM_INT test_mode;
} SM_AVFREC_START_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_avfrec_start sm_avfrec_start_iPsVRte1_3_usie1_5_usii____ii_
#define SM_AVFREC_START_NAME "sm_avfrec_start_iPsVRte1_3_usie1_5_usii____ii_"
#endif
typedef int SM_AVFREC_START_FN(struct sm_avfrec_start_parms *avfsp);

ACUAPI int ACUTiNG_WINAPI sm_avfrec_start(struct sm_avfrec_start_parms *avfsp);


typedef struct sm_avfrec_status_parms {
	tSMAVFrecId avfrec;
	enum kSMAVFrecordStatus status;
} SM_AVFREC_STATUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_avfrec_status sm_avfrec_status_iPsVRte1_4__
#define SM_AVFREC_STATUS_NAME "sm_avfrec_status_iPsVRte1_4__"
#endif
typedef int SM_AVFREC_STATUS_FN(struct sm_avfrec_status_parms *statusp);

ACUAPI int ACUTiNG_WINAPI sm_avfrec_status(struct sm_avfrec_status_parms *statusp);


typedef struct sm_avfrec_stop_parms {
	tSMAVFrecId avfrec;
	tSM_INT immediate;
} SM_AVFREC_STOP_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_avfrec_stop sm_avfrec_stop_iPsVRti_
#define SM_AVFREC_STOP_NAME "sm_avfrec_stop_iPsVRti_"
#endif
typedef int SM_AVFREC_STOP_FN(struct sm_avfrec_stop_parms *avfsp);

ACUAPI int ACUTiNG_WINAPI sm_avfrec_stop(struct sm_avfrec_stop_parms *avfsp);


typedef struct sm_avfrec_sync_parms {
	tSMAVFrecId avfrec;
	tSMAVFrecId sync_avfrec;
} SM_AVFREC_SYNC_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_avfrec_sync sm_avfrec_sync_iPsVRtVRt_
#define SM_AVFREC_SYNC_NAME "sm_avfrec_sync_iPsVRtVRt_"
#endif
typedef int SM_AVFREC_SYNC_FN(struct sm_avfrec_sync_parms *avfsp);

ACUAPI int ACUTiNG_WINAPI sm_avfrec_sync(struct sm_avfrec_sync_parms *avfsp);



#endif
