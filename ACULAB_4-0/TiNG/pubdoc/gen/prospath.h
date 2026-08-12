/* prospath.h - Prosody signal path processing API declarations */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

#ifndef INCLUDED_PUBDOC_PROSPATH_H
#define INCLUDED_PUBDOC_PROSPATH_H

#ifdef ACULAB_TiNG_VERSION_NUM
#if ACULAB_TiNG_VERSION_NUM != 1504
#error ACULAB_TiNG_VERSION_NUM does not match 1504
#endif
#else
#define ACULAB_TiNG_VERSION_NUM 1504
#define ACULAB_TiNG_VERSION_CHANGED 
#define ACULAB_TiNG_VERSION_STRING "m1504."
#endif

enum kSMPathEchoNonLinearMode {
	kSMPathEchoNonLinearModeMuting,
	kSMPathEchoNonLinearModeCNG,
};

enum kSMPathEchoToneAction {
	kSMPathEchoToneActionNone,
	kSMPathEchoTonetActionMuteTTY,
};

enum kSMPathEmphMode {
	kSMPathEmphModeHFEmphasis,
	kSMPathEmphModeHFDeemphasis,
};

enum kSMPathStatus {
	kSMPathStatusRunning,
	kSMPathStatusOngoingTone,
	kSMPathStatusEndTone,
	kSMPathStatusStopped,
};

enum kSMPathToneDetectMode {
	kSMPathToneDetectModeNone,
	kSMPathToneDetectModeMinDurationNoMinimum,
	kSMPathToneDetectModeMinDuration40,
	kSMPathToneDetectModeMinDuration64,
};



typedef struct sm_path_agc_parms {
	tSMPathId path;
	tSM_INT agc;
	tSM_INT volume;
} SM_PATH_AGC_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_path_agc sm_path_agc_iPsPtii_
#define SM_PATH_AGC_NAME "sm_path_agc_iPsPtii_"
#endif
typedef int SM_PATH_AGC_FN(struct sm_path_agc_parms *agcp);

ACUAPI int ACUTiNG_WINAPI sm_path_agc(struct sm_path_agc_parms *agcp);


typedef struct sm_path_agc_adjust_settings_parms {
	tSMPathId path;
	float max_level_decay;
	float target_level;
} SM_PATH_AGC_ADJUST_SETTINGS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_path_agc_adjust_settings sm_path_agc_adjust_settings_iPsPtff_
#define SM_PATH_AGC_ADJUST_SETTINGS_NAME "sm_path_agc_adjust_settings_iPsPtff_"
#endif
typedef int SM_PATH_AGC_ADJUST_SETTINGS_FN(struct sm_path_agc_adjust_settings_parms *agcadjp);

ACUAPI int ACUTiNG_WINAPI sm_path_agc_adjust_settings(struct sm_path_agc_adjust_settings_parms *agcadjp);


typedef struct sm_path_avf_amrnb_dec_parms {
	tSMPathId path;
	tSM_INT enable;
} SM_PATH_AVF_AMRNB_DEC_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_path_avf_amrnb_dec sm_path_avf_amrnb_dec_iPsPti_
#define SM_PATH_AVF_AMRNB_DEC_NAME "sm_path_avf_amrnb_dec_iPsPti_"
#endif
typedef int SM_PATH_AVF_AMRNB_DEC_FN(struct sm_path_avf_amrnb_dec_parms *amrnbdecparms);

ACUAPI int ACUTiNG_WINAPI sm_path_avf_amrnb_dec(struct sm_path_avf_amrnb_dec_parms *amrnbdecparms);


typedef struct sm_path_avf_amrnb_enc_parms {
	tSMPathId path;
	tSM_INT enable;
	tSM_INT payload_type;
	tSM_INT bitrate;
	tSM_INT vad_enable;
} SM_PATH_AVF_AMRNB_ENC_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_path_avf_amrnb_enc sm_path_avf_amrnb_enc_iPsPtiiii_
#define SM_PATH_AVF_AMRNB_ENC_NAME "sm_path_avf_amrnb_enc_iPsPtiiii_"
#endif
typedef int SM_PATH_AVF_AMRNB_ENC_FN(struct sm_path_avf_amrnb_enc_parms *amrnbencparms);

ACUAPI int ACUTiNG_WINAPI sm_path_avf_amrnb_enc(struct sm_path_avf_amrnb_enc_parms *amrnbencparms);


typedef struct sm_path_create_parms {
	tSMPathId path;
	tSMModuleId module;
} SM_PATH_CREATE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_path_create sm_path_create_iPsPtMt_
#define SM_PATH_CREATE_NAME "sm_path_create_iPsPtMt_"
#endif
typedef int SM_PATH_CREATE_FN(struct sm_path_create_parms *createp);

ACUAPI int ACUTiNG_WINAPI sm_path_create(struct sm_path_create_parms *createp);


typedef struct sm_path_datafeed_connect_parms {
	tSMPathId path;
	tSMDatafeedId data_source;
} SM_PATH_DATAFEED_CONNECT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_path_datafeed_connect sm_path_datafeed_connect_iPsPtDt_
#define SM_PATH_DATAFEED_CONNECT_NAME "sm_path_datafeed_connect_iPsPtDt_"
#endif
typedef int SM_PATH_DATAFEED_CONNECT_FN(struct sm_path_datafeed_connect_parms *datafeedp);

ACUAPI int ACUTiNG_WINAPI sm_path_datafeed_connect(struct sm_path_datafeed_connect_parms *datafeedp);


typedef struct sm_path_delay_parms {
	tSMPathId path;
	tSM_INT enable;
	tSM_INT delay;
} SM_PATH_DELAY_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_path_delay sm_path_delay_iPsPtii_
#define SM_PATH_DELAY_NAME "sm_path_delay_iPsPtii_"
#endif
typedef int SM_PATH_DELAY_FN(struct sm_path_delay_parms *delayparms);

ACUAPI int ACUTiNG_WINAPI sm_path_delay(struct sm_path_delay_parms *delayparms);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_path_destroy sm_path_destroy_iPt
#define SM_PATH_DESTROY_NAME "sm_path_destroy_iPt"
#endif
typedef int SM_PATH_DESTROY_FN(tSMPathId path);

ACUAPI int ACUTiNG_WINAPI sm_path_destroy(tSMPathId path);


typedef struct sm_path_echocancel_parms {
	tSMPathId path;
	tSM_INT enable;
	tSMDatafeedId reference;
	tSM_INT non_linear;
	enum kSMPathEchoNonLinearMode mode;
	tSM_INT use_agc;
	tSM_INT fix_agc;
	tSM_INT span;
	tSM_INT delay;
	enum kSMPathEchoToneAction tone_action;
} SM_PATH_ECHOCANCEL_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_path_echocancel sm_path_echocancel_iPsPtiDtie1_2_iiiie1_2__
#define SM_PATH_ECHOCANCEL_NAME "sm_path_echocancel_iPsPtiDtie1_2_iiiie1_2__"
#endif
typedef int SM_PATH_ECHOCANCEL_FN(struct sm_path_echocancel_parms *cancelp);

ACUAPI int ACUTiNG_WINAPI sm_path_echocancel(struct sm_path_echocancel_parms *cancelp);


typedef struct sm_path_emphasis_parms {
	tSMPathId path;
	tSM_INT enable;
	enum kSMPathEmphMode mode;
	tSM_INT gain;
} SM_PATH_EMPHASIS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_path_emphasis sm_path_emphasis_iPsPtie1_2_i_
#define SM_PATH_EMPHASIS_NAME "sm_path_emphasis_iPsPtie1_2_i_"
#endif
typedef int SM_PATH_EMPHASIS_FN(struct sm_path_emphasis_parms *emphparms);

ACUAPI int ACUTiNG_WINAPI sm_path_emphasis(struct sm_path_emphasis_parms *emphparms);


typedef struct sm_path_datafeed_parms {
	tSMPathId path;
	tSMDatafeedId datafeed;
} SM_PATH_DATAFEED_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_path_get_datafeed sm_path_get_datafeed_iPsPtDt_
#define SM_PATH_GET_DATAFEED_NAME "sm_path_get_datafeed_iPsPtDt_"
#endif
typedef int SM_PATH_GET_DATAFEED_FN(struct sm_path_datafeed_parms *datafeedp);

ACUAPI int ACUTiNG_WINAPI sm_path_get_datafeed(struct sm_path_datafeed_parms *datafeedp);


typedef struct sm_path_event_parms {
	tSMPathId path;
	tSMEventId event;
} SM_PATH_EVENT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_path_get_event sm_path_get_event_iPsPtEt_
#define SM_PATH_GET_EVENT_NAME "sm_path_get_event_iPsPtEt_"
#endif
typedef int SM_PATH_GET_EVENT_FN(struct sm_path_event_parms *eventp);

ACUAPI int ACUTiNG_WINAPI sm_path_get_event(struct sm_path_event_parms *eventp);


typedef struct sm_path_mix_parms {
	tSMPathId path;
	tSM_INT enable;
	tSMDatafeedId mixin;
	tSM_INT volume;
} SM_PATH_MIX_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_path_mix sm_path_mix_iPsPtiDti_
#define SM_PATH_MIX_NAME "sm_path_mix_iPsPtiDti_"
#endif
typedef int SM_PATH_MIX_FN(struct sm_path_mix_parms *mixp);

ACUAPI int ACUTiNG_WINAPI sm_path_mix(struct sm_path_mix_parms *mixp);


typedef struct sm_path_pitchshift_parms {
	tSMPathId path;
	float shift;
} SM_PATH_PITCHSHIFT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_path_pitchshift sm_path_pitchshift_iPsPtf_
#define SM_PATH_PITCHSHIFT_NAME "sm_path_pitchshift_iPsPtf_"
#endif
typedef int SM_PATH_PITCHSHIFT_FN(struct sm_path_pitchshift_parms *pitchshiftp);

ACUAPI int ACUTiNG_WINAPI sm_path_pitchshift(struct sm_path_pitchshift_parms *pitchshiftp);


typedef struct sm_path_resample_parms {
	tSMPathId path;
	tSM_INT uprate;
	tSM_INT downrate;
} SM_PATH_RESAMPLE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_path_resample sm_path_resample_iPsPtii_
#define SM_PATH_RESAMPLE_NAME "sm_path_resample_iPsPtii_"
#endif
typedef int SM_PATH_RESAMPLE_FN(struct sm_path_resample_parms *resamplep);

ACUAPI int ACUTiNG_WINAPI sm_path_resample(struct sm_path_resample_parms *resamplep);


typedef struct sm_path_status_parms {
	tSMPathId path;
	enum kSMPathStatus status;
	union {
		struct {
			tSM_INT id;
			unsigned duration;
		} tone;
	} u;
} SM_PATH_STATUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_path_status sm_path_status_iPsPte1_4_usiUi___
#define SM_PATH_STATUS_NAME "sm_path_status_iPsPte1_4_usiUi___"
#endif
typedef int SM_PATH_STATUS_FN(struct sm_path_status_parms *statusp);

ACUAPI int ACUTiNG_WINAPI sm_path_status(struct sm_path_status_parms *statusp);


typedef struct sm_path_stop_parms {
	tSMPathId path;
} SM_PATH_STOP_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_path_stop sm_path_stop_iPsPt_
#define SM_PATH_STOP_NAME "sm_path_stop_iPsPt_"
#endif
typedef int SM_PATH_STOP_FN(struct sm_path_stop_parms *stopp);

ACUAPI int ACUTiNG_WINAPI sm_path_stop(struct sm_path_stop_parms *stopp);


typedef struct sm_path_tonedetect_parms {
	tSMPathId path;
	tSM_INT elim;
	enum kSMPathToneDetectMode min_duration;
	tSM_INT tone_set_id;
} SM_PATH_TONEDETECT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_path_tonedetect sm_path_tonedetect_iPsPtie1_4_i_
#define SM_PATH_TONEDETECT_NAME "sm_path_tonedetect_iPsPtie1_4_i_"
#endif
typedef int SM_PATH_TONEDETECT_FN(struct sm_path_tonedetect_parms *tdparms);

ACUAPI int ACUTiNG_WINAPI sm_path_tonedetect(struct sm_path_tonedetect_parms *tdparms);


typedef struct sm_path_ttyeliminate_parms {
	tSMPathId path;
	tSM_INT enable;
} SM_PATH_TTYELIMINATE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_path_ttyeliminate sm_path_ttyeliminate_iPsPti_
#define SM_PATH_TTYELIMINATE_NAME "sm_path_ttyeliminate_iPsPti_"
#endif
typedef int SM_PATH_TTYELIMINATE_FN(struct sm_path_ttyeliminate_parms *ttyeliminatep);

ACUAPI int ACUTiNG_WINAPI sm_path_ttyeliminate(struct sm_path_ttyeliminate_parms *ttyeliminatep);



#endif
