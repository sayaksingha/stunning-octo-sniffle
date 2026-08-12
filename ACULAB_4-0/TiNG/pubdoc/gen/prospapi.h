/* prospapi.h - Prosody speech processing API declarations */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

#ifndef INCLUDED_PUBDOC_PROSPAPI_H
#define INCLUDED_PUBDOC_PROSPAPI_H

#ifdef ACULAB_TiNG_VERSION_NUM
#if ACULAB_TiNG_VERSION_NUM != 1504
#error ACULAB_TiNG_VERSION_NUM does not match 1504
#endif
#else
#define ACULAB_TiNG_VERSION_NUM 1504
#define ACULAB_TiNG_VERSION_CHANGED 
#define ACULAB_TiNG_VERSION_STRING "m1504."
#endif

#define kSMMaxCPToneStates 16
#define kSMMaxDigits 32
#define kSMMaxDigits_plus1 33
#define kSMMaxPlayCPToneCadences 8
enum kSMBESPCatSigParam {
	kSMBESPCatSigParamF_ln_nrg_coeff=2,
	kSMBESPCatSigParamF_min_Lmin,
	kSMBESPCatSigParamF_Lmin_decay,
	kSMBESPCatSigParamF_speech_thresh,
	kSMBESPCatSigParamF_debounce,
	kSMBESPCatSigParamI_min_valid_period_count,
	kSMBESPCatSigParamI_min_valid_count,
	kSMBESPCatSigParamI_glitch_count,
	kSMBESPCatSigParamI_qualify_count,
	kSMBESPCatSigParamI_alter_duration,
	kSMBESPCatSigParamI_max_valid_tone_cnt,
	kSMBESPCatSigParamI_min_valid_speech_cnt,
	kSMBESPCatSigParamI_threshold_samp_cnt,
	kSMBESPCatSigParamI_delay_time,
	kSMBESPCatSigParamI_period_time,
	kSMBESPCatSigParamI_min_off_count,
	kSMBESPCatSigParamI_min_period_off,
	kSMBESPCatSigParamF_initial_Lmax,
};

enum kAdjustToneSet {
	kAdjustToneSetFPParamId3rdPeak,
	kAdjustToneSetFPParamIdSNRatio,
	kAdjustToneSetFPParamIdMinPower,
	kAdjustToneSetFPParamIdTwist,
	kAdjustToneSetIntParamIdMinOnTime,
	kAdjustToneSetIntParamIdMinOffTime,
	kAdjustToneSetFPParamIdStartFreq,
	kAdjustToneSetFPParamIdStopFreq,
};

enum kSMANSMode {
	kSMANSModeDisable,
	kSMANSModeDetect,
};

enum kBESPCatSigAlg {
	kBESPCatSigAlgLiveSpeaker=1,
	kBESPCatSigAlgLiveSpeakerTone,
	kBESPCatSigAlgLiveSpeakerToneReport,
	kBESPCatSigAlgLiveSpeakerPlugin,
};

enum kSMInputCondAdjust {
	kSMInputCondAdjustNonLinearWithMuting,
	kSMInputCondAdjustNonLinearWithCNG,
	kSMInputCondAdjustAGC,
	kSMInputCondAdjustFixGain,
};

enum kSMInputCondRef {
	kSMInputCondRefNone,
	kSMInputCondRefUseInput,
	kSMInputCondRefUseOutput,
};

enum kSMInputCond {
	kSMInputCondNone,
	kSMInputCondEchoCancelation,
};

enum kSMInputCondAltDest {
	kSMInputCondAltDestNone,
	kSMInputCondAltDestInput,
	kSMInputCondAltDestOutput,
};

enum kSMConfType {
	kSMConfTypeStandard,
	kSMConfTypeIndividualVolume,
};

enum kSMConfStatus {
	kSMConfStatusRunning,
	kSMConfStatusStopped,
	kSMConfStatusActiveInputs,
};

enum kSMRecognition {
	kSMRecognisedNothing,
	kSMRecognisedTrainingDigit,
	kSMRecognisedDigit,
	kSMRecognisedTone,
	kSMRecognisedCPTone,
	kSMRecognisedGruntStart,
	kSMRecognisedGruntEnd,
	kSMRecognisedASRResult,
	kSMRecognisedASRUncertain,
	kSMRecognisedASRRejected,
	kSMRecognisedASRTimeout,
	kSMRecognisedCatSig,
	kSMRecognisedOverrun,
	kSMRecognisedANS,
	kSMRecognisedBeep,
	kSMRecognisedOnHook,
};

enum kSMToneDetection {
	kSMToneDetectionNone,
	kSMToneDetectionNoMinDuration,
	kSMToneDetectionMinDuration64,
	kSMToneDetectionMinDuration40,
	kSMToneEndDetectionNoMinDuration,
	kSMToneEndDetectionMinDuration64,
	kSMToneEndDetectionMinDuration40,
	kSMToneLenDetectionNoMinDuration,
	kSMToneLenDetectionMinDuration64,
	kSMToneLenDetectionMinDuration40,
	kSMToneDetectionAsListenFor,
};

enum kSMDigitMapping {
	kSMNoDigitMapping,
	kSMDTMFToneSetDigitMapping,
};

enum kSMPlayCPToneType {
	kSMPlayCPToneTypeOneShot,
	kSMPlayCPToneTypeRepeat,
	kSMPlayCPToneTypeContinuous,
};

enum kSMPlayCPToneStatus {
	kSMPlayCPToneStatusComplete,
	kSMPlayCPToneStatusOngoing,
};

enum kSMDigitType {
	kSMDTMFDigits,
};

enum kSMPlayDigitsStatus {
	kSMPlayDigitsStatusComplete,
	kSMPlayDigitsStatusOngoing,
};

enum kSMToneOperation {
	kSMToneOperationStop,
	kSMToneOperationSum,
	kSMToneOperationModulate,
};

enum kSMPlayToneListStatus {
	kSMPlayToneListStatusOngoing,
	kSMPlayToneListStatusComplete,
	kSMPlayToneListStatusHasCapacity,
	kSMPlayToneListStatusUnderrun,
};

enum kSMPlayToneStatus {
	kSMPlayToneStatusComplete,
	kSMPlayToneStatusOngoing,
};

enum kSMDataFormat {
	kSMDataFormatNone=0,
	kSMDataFormatALawPCM=30,
	kSMDataFormatULawPCM=31,
	kSMDataFormatOKIADPCM=32,
	kSMDataFormatACUBLKPCM=33,
	kSMDataFormat16bit=34,
	kSMDataFormat8bit=35,
	kSMDataFormatSigned8bit=36,
	kSMDataFormatIMAADPCM=17,
	kSMDataFormatSpeex=37,
};

enum kSMRecordAltSource {
	kSMRecordAltSourceDefault,
	kSMRecordAltSourceInput,
	kSMRecordAltSourceOutput,
};

enum kSMRecordStatus {
	kSMRecordStatusComplete,
	kSMRecordStatusCompleteData,
	kSMRecordStatusOverrun,
	kSMRecordStatusData,
	kSMRecordStatusNoData,
	kSMRecordStatusRecognition,
};

enum kSMRecordHowTerminated {
	kSMRecordHowTerminatedNotYet,
	kSMRecordHowTerminatedLength,
	kSMRecordHowTerminatedMaxTime,
	kSMRecordHowTerminatedSilence,
	kSMRecordHowTerminatedAborted,
	kSMRecordHowTerminatedError,
};

enum kSMReplayStatus {
	kSMReplayStatusComplete,
	kSMReplayStatusCompleteData,
	kSMReplayStatusUnderrun,
	kSMReplayStatusHasCapacity,
	kSMReplayStatusNoCapacity,
};

enum kSMASRMode {
	kSMASRModeDisabled,
	kSMASRModeOneShot,
	kSMASRModeContinuous,
};



typedef struct sm_input_cptone_parms {
	tSMModuleId module;
	tSM_INT state_count;
	struct sm_cptone_state {
		tSM_INT freq_id;
		tSM_INT maximum_cadence;
		tSM_INT minimum_cadence;
	} states[kSMMaxCPToneStates];
	tSM_INT id;
} SM_INPUT_CPTONE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_add_input_cptone sm_add_input_cptone_iPsMtisiii_r16i_
#define SM_ADD_INPUT_CPTONE_NAME "sm_add_input_cptone_iPsMtisiii_r16i_"
#endif
typedef int SM_ADD_INPUT_CPTONE_FN(struct sm_input_cptone_parms *cptonep);

ACUAPI int ACUTiNG_WINAPI sm_add_input_cptone(struct sm_input_cptone_parms *cptonep);


typedef struct sm_input_freq_coeffs_parms {
	tSMModuleId module;
	double upper_limit;
	double lower_limit;
	tSM_INT id;
} SM_INPUT_FREQ_COEFFS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_add_input_freq_coeffs sm_add_input_freq_coeffs_iPsMtddi_
#define SM_ADD_INPUT_FREQ_COEFFS_NAME "sm_add_input_freq_coeffs_iPsMtddi_"
#endif
typedef int SM_ADD_INPUT_FREQ_COEFFS_FN(struct sm_input_freq_coeffs_parms *freqcoeffp);

ACUAPI int ACUTiNG_WINAPI sm_add_input_freq_coeffs(struct sm_input_freq_coeffs_parms *freqcoeffp);


typedef struct sm_input_tone_set_parms {
	tSMModuleId module;
	tSM_INT band1_first_freq_coeffs_id;
	tSM_INT band1_freq_count;
	tSM_INT band2_first_freq_coeffs_id;
	tSM_INT band2_freq_count;
	double req_third_peak;
	double req_signal_to_noise_ratio;
	double req_minimum_power;
	double req_twist_for_dual_tone;
	tSM_INT id;
} SM_INPUT_TONE_SET_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_add_input_tone_set sm_add_input_tone_set_iPsMtiiiiddddi_
#define SM_ADD_INPUT_TONE_SET_NAME "sm_add_input_tone_set_iPsMtiiiiddddi_"
#endif
typedef int SM_ADD_INPUT_TONE_SET_FN(struct sm_input_tone_set_parms *tonesetp);

ACUAPI int ACUTiNG_WINAPI sm_add_input_tone_set(struct sm_input_tone_set_parms *tonesetp);


typedef struct sm_output_freq_parms {
	tSMModuleId module;
	double freq;
	double amplitude;
	tSM_INT id;
} SM_OUTPUT_FREQ_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_add_output_freq sm_add_output_freq_iPsMtddi_
#define SM_ADD_OUTPUT_FREQ_NAME "sm_add_output_freq_iPsMtddi_"
#endif
typedef int SM_ADD_OUTPUT_FREQ_FN(struct sm_output_freq_parms *freqp);

ACUAPI int ACUTiNG_WINAPI sm_add_output_freq(struct sm_output_freq_parms *freqp);


typedef struct sm_output_tone_parms {
	tSMModuleId module;
	tSM_INT component1_id;
	tSM_INT component2_id;
	tSM_INT id;
} SM_OUTPUT_TONE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_add_output_tone sm_add_output_tone_iPsMtiii_
#define SM_ADD_OUTPUT_TONE_NAME "sm_add_output_tone_iPsMtiii_"
#endif
typedef int SM_ADD_OUTPUT_TONE_FN(struct sm_output_tone_parms *tonep);

ACUAPI int ACUTiNG_WINAPI sm_add_output_tone(struct sm_output_tone_parms *tonep);


typedef struct sm_adjust_catsig_module_parms {
	tSMModuleId module;
	tSM_INT catsig_alg_id;
	enum kSMBESPCatSigParam parameter_id;
	union {
		double fp_value;
		tSM_INT int_value;
	} parameter_value;
} SM_ADJUST_CATSIG_MODULE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_adjust_catsig_module_params sm_adjust_catsig_module_params_iPsMtie2_19_udi__
#define SM_ADJUST_CATSIG_MODULE_PARAMS_NAME "sm_adjust_catsig_module_params_iPsMtie2_19_udi__"
#endif
typedef int SM_ADJUST_CATSIG_MODULE_PARAMS_FN(struct sm_adjust_catsig_module_parms *catsigp);

ACUAPI int ACUTiNG_WINAPI sm_adjust_catsig_module_params(struct sm_adjust_catsig_module_parms *catsigp);


typedef struct sm_adjust_tone_set_parms {
	tSMModuleId module;
	tSM_INT tone_set_id;
	enum kAdjustToneSet parameter_id;
	union {
		double fp_value;
		tSM_INT int_value;
	} parameter_value;
} SM_ADJUST_TONE_SET_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_adjust_input_tone_set sm_adjust_input_tone_set_iPsMtie1_8_udi__
#define SM_ADJUST_INPUT_TONE_SET_NAME "sm_adjust_input_tone_set_iPsMtie1_8_udi__"
#endif
typedef int SM_ADJUST_INPUT_TONE_SET_FN(struct sm_adjust_tone_set_parms *tonesetp);

ACUAPI int ACUTiNG_WINAPI sm_adjust_input_tone_set(struct sm_adjust_tone_set_parms *tonesetp);


typedef struct sm_ans_listen_for_parms {
	tSMChannelId channel;
	enum kSMANSMode detection_mode;
} SM_ANS_LISTEN_FOR_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_ans_listen_for sm_ans_listen_for_iPsCte1_2__
#define SM_ANS_LISTEN_FOR_NAME "sm_ans_listen_for_iPsCte1_2__"
#endif
typedef int SM_ANS_LISTEN_FOR_FN(struct sm_ans_listen_for_parms *listenp);

ACUAPI int ACUTiNG_WINAPI sm_ans_listen_for(struct sm_ans_listen_for_parms *listenp);


typedef struct sm_beep_listen_for_parms {
	tSMChannelId channel;
	tSM_INT min_duration;
	double upper_limit;
	double lower_limit;
} SM_BEEP_LISTEN_FOR_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_beep_listen_for sm_beep_listen_for_iPsCtidd_
#define SM_BEEP_LISTEN_FOR_NAME "sm_beep_listen_for_iPsCtidd_"
#endif
typedef int SM_BEEP_LISTEN_FOR_FN(struct sm_beep_listen_for_parms *listenp);

ACUAPI int ACUTiNG_WINAPI sm_beep_listen_for(struct sm_beep_listen_for_parms *listenp);


typedef struct sm_catsig_listen_for_parms {
	tSMChannelId channel;
	enum kBESPCatSigAlg catsig_alg_id;
	tSM_INT catsig_hint_param;
	tSM_INT catsig_timeout;
	tSM_INT catsig_config_length;
	char *catsig_config_data;
	tSM_INT abort_catsig_alg;
} SM_CATSIG_LISTEN_FOR_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_catsig_listen_for sm_catsig_listen_for_iPsCte1_4_iiiPci_
#define SM_CATSIG_LISTEN_FOR_NAME "sm_catsig_listen_for_iPsCte1_4_iiiPci_"
#endif
typedef int SM_CATSIG_LISTEN_FOR_FN(struct sm_catsig_listen_for_parms *listenp);

ACUAPI int ACUTiNG_WINAPI sm_catsig_listen_for(struct sm_catsig_listen_for_parms *listenp);


typedef struct sm_catsig_signalling_hint_parms {
	tSMChannelId channel;
	tSM_INT catsig_hint_param;
} SM_CATSIG_SIGNALLING_HINT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_catsig_signalling_hint sm_catsig_signalling_hint_iPsCti_
#define SM_CATSIG_SIGNALLING_HINT_NAME "sm_catsig_signalling_hint_iPsCti_"
#endif
typedef int SM_CATSIG_SIGNALLING_HINT_FN(struct sm_catsig_signalling_hint_parms *hintp);

ACUAPI int ACUTiNG_WINAPI sm_catsig_signalling_hint(struct sm_catsig_signalling_hint_parms *hintp);


typedef struct sm_channel_set_input_threshold_parms {
	tSMChannelId channel;
	tSM_INT minimum_bits;
} SM_CHANNEL_SET_INPUT_THRESHOLD_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_channel_set_input_threshold sm_channel_set_input_threshold_iPsCti_
#define SM_CHANNEL_SET_INPUT_THRESHOLD_NAME "sm_channel_set_input_threshold_iPsCti_"
#endif
typedef int SM_CHANNEL_SET_INPUT_THRESHOLD_FN(struct sm_channel_set_input_threshold_parms *thp);

ACUAPI int ACUTiNG_WINAPI sm_channel_set_input_threshold(struct sm_channel_set_input_threshold_parms *thp);


typedef struct sm_channel_set_output_threshold_parms {
	tSMChannelId channel;
	tSM_INT minimum_bits;
} SM_CHANNEL_SET_OUTPUT_THRESHOLD_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_channel_set_output_threshold sm_channel_set_output_threshold_iPsCti_
#define SM_CHANNEL_SET_OUTPUT_THRESHOLD_NAME "sm_channel_set_output_threshold_iPsCti_"
#endif
typedef int SM_CHANNEL_SET_OUTPUT_THRESHOLD_FN(struct sm_channel_set_output_threshold_parms *thp);

ACUAPI int ACUTiNG_WINAPI sm_channel_set_output_threshold(struct sm_channel_set_output_threshold_parms *thp);


typedef struct sm_condition_adjust_parms {
	tSMChannelId channel;
	enum kSMInputCondAdjust adjust_type;
	tSM_INT adjust_value;
} SM_CONDITION_ADJUST_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_condition_adjust sm_condition_adjust_iPsCte1_4_i_
#define SM_CONDITION_ADJUST_NAME "sm_condition_adjust_iPsCte1_4_i_"
#endif
typedef int SM_CONDITION_ADJUST_FN(struct sm_condition_adjust_parms *condp);

ACUAPI int ACUTiNG_WINAPI sm_condition_adjust(struct sm_condition_adjust_parms *condp);


typedef struct sm_condition_adjust_span_parms {
	tSMChannelId channel;
	tSM_INT span;
} SM_CONDITION_ADJUST_SPAN_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_condition_adjust_span sm_condition_adjust_span_iPsCti_
#define SM_CONDITION_ADJUST_SPAN_NAME "sm_condition_adjust_span_iPsCti_"
#endif
typedef int SM_CONDITION_ADJUST_SPAN_FN(struct sm_condition_adjust_span_parms *condp);

ACUAPI int ACUTiNG_WINAPI sm_condition_adjust_span(struct sm_condition_adjust_span_parms *condp);


typedef struct sm_condition_input_parms {
	tSMChannelId channel;
	tSMChannelId reference;
	enum kSMInputCondRef reference_type;
	enum kSMInputCond conditioning_type;
	tSM_INT conditioning_param;
	tSMChannelId alt_data_dest;
	enum kSMInputCondAltDest alt_dest_type;
	int ectest_type;
	float ectest_gain;
	int ectest_delay;
	unsigned ectest_flen;
	float *ectest_filt;
} SM_CONDITION_INPUT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_condition_input sm_condition_input_iPsCtCte1_3_e1_2_iCte1_3_ifiUiPf_
#define SM_CONDITION_INPUT_NAME "sm_condition_input_iPsCtCte1_3_e1_2_iCte1_3_ifiUiPf_"
#endif
typedef int SM_CONDITION_INPUT_FN(struct sm_condition_input_parms *condp);

ACUAPI int ACUTiNG_WINAPI sm_condition_input(struct sm_condition_input_parms *condp);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_condition_reinit sm_condition_reinit_iCt
#define SM_CONDITION_REINIT_NAME "sm_condition_reinit_iCt"
#endif
typedef int SM_CONDITION_REINIT_FN(tSMChannelId channel);

ACUAPI int ACUTiNG_WINAPI sm_condition_reinit(tSMChannelId channel);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_conf_prim_abort sm_conf_prim_abort_iCt
#define SM_CONF_PRIM_ABORT_NAME "sm_conf_prim_abort_iCt"
#endif
typedef int SM_CONF_PRIM_ABORT_FN(tSMChannelId channel);

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_abort(tSMChannelId channel);


typedef struct sm_conf_prim_add_parms {
	tSMChannelId channel;
	tSMChannelId participant;
	tSM_INT id;
	float factor;
} SM_CONF_PRIM_ADD_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_conf_prim_add sm_conf_prim_add_iPsCtCtif_
#define SM_CONF_PRIM_ADD_NAME "sm_conf_prim_add_iPsCtCtif_"
#endif
typedef int SM_CONF_PRIM_ADD_FN(struct sm_conf_prim_add_parms *confp);

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_add(struct sm_conf_prim_add_parms *confp);


typedef struct sm_conf_prim_adj_input_parms {
	tSMChannelId channel;
	tSM_INT volume;
	tSM_INT agc;
} SM_CONF_PRIM_ADJ_INPUT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_conf_prim_adj_input sm_conf_prim_adj_input_iPsCtii_
#define SM_CONF_PRIM_ADJ_INPUT_NAME "sm_conf_prim_adj_input_iPsCtii_"
#endif
typedef int SM_CONF_PRIM_ADJ_INPUT_FN(struct sm_conf_prim_adj_input_parms *confp);

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_adj_input(struct sm_conf_prim_adj_input_parms *confp);


typedef struct sm_conf_prim_adj_input_settings_parms {
	tSMChannelId channel;
	float max_level_decay;
	float target_level;
} SM_CONF_PRIM_ADJ_INPUT_SETTINGS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_conf_prim_adj_input_settings sm_conf_prim_adj_input_settings_iPsCtff_
#define SM_CONF_PRIM_ADJ_INPUT_SETTINGS_NAME "sm_conf_prim_adj_input_settings_iPsCtff_"
#endif
typedef int SM_CONF_PRIM_ADJ_INPUT_SETTINGS_FN(struct sm_conf_prim_adj_input_settings_parms *confp);

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_adj_input_settings(struct sm_conf_prim_adj_input_settings_parms *confp);


typedef struct sm_conf_prim_adj_output_parms {
	tSMChannelId channel;
	tSM_INT volume;
	tSM_INT agc;
} SM_CONF_PRIM_ADJ_OUTPUT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_conf_prim_adj_output sm_conf_prim_adj_output_iPsCtii_
#define SM_CONF_PRIM_ADJ_OUTPUT_NAME "sm_conf_prim_adj_output_iPsCtii_"
#endif
typedef int SM_CONF_PRIM_ADJ_OUTPUT_FN(struct sm_conf_prim_adj_output_parms *confp);

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_adj_output(struct sm_conf_prim_adj_output_parms *confp);


typedef struct sm_conf_prim_adj_tracking_parms {
	tSMChannelId channel;
	double min_noise_level;
	double speech_thresh;
} SM_CONF_PRIM_ADJ_TRACKING_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_conf_prim_adj_tracking sm_conf_prim_adj_tracking_iPsCtdd_
#define SM_CONF_PRIM_ADJ_TRACKING_NAME "sm_conf_prim_adj_tracking_iPsCtdd_"
#endif
typedef int SM_CONF_PRIM_ADJ_TRACKING_FN(struct sm_conf_prim_adj_tracking_parms *trackp);

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_adj_tracking(struct sm_conf_prim_adj_tracking_parms *trackp);


typedef struct sm_conf_prim_attach_parms {
	tSMChannelId channel;
	enum kSMConfType conf_type;
} SM_CONF_PRIM_ATTACH_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_conf_prim_attach sm_conf_prim_attach_iPsCte1_2__
#define SM_CONF_PRIM_ATTACH_NAME "sm_conf_prim_attach_iPsCte1_2__"
#endif
typedef int SM_CONF_PRIM_ATTACH_FN(struct sm_conf_prim_attach_parms *confp);

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_attach(struct sm_conf_prim_attach_parms *confp);


typedef struct sm_conf_prim_clone_parms {
	tSMChannelId channel;
	tSMChannelId model;
} SM_CONF_PRIM_CLONE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_conf_prim_clone sm_conf_prim_clone_iPsCtCt_
#define SM_CONF_PRIM_CLONE_NAME "sm_conf_prim_clone_iPsCtCt_"
#endif
typedef int SM_CONF_PRIM_CLONE_FN(struct sm_conf_prim_clone_parms *clonep);

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_clone(struct sm_conf_prim_clone_parms *clonep);


typedef struct sm_conf_prim_config_activity_reporting_parms {
	tSMChannelId channel;
	tSM_UT32 delay;
	tSM_UT32 sensitivity;
} SM_CONF_PRIM_CONFIG_ACTIVITY_REPORTING_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_conf_prim_config_activity_reporting sm_conf_prim_config_activity_reporting_iPsCtUiUi_
#define SM_CONF_PRIM_CONFIG_ACTIVITY_REPORTING_NAME "sm_conf_prim_config_activity_reporting_iPsCtUiUi_"
#endif
typedef int SM_CONF_PRIM_CONFIG_ACTIVITY_REPORTING_FN(struct sm_conf_prim_config_activity_reporting_parms *activityp);

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_config_activity_reporting(struct sm_conf_prim_config_activity_reporting_parms *activityp);


typedef struct sm_conf_prim_detach_parms {
	tSMChannelId channel;
	enum kSMConfType conf_type;
} SM_CONF_PRIM_DETACH_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_conf_prim_detach sm_conf_prim_detach_iPsCte1_2__
#define SM_CONF_PRIM_DETACH_NAME "sm_conf_prim_detach_iPsCte1_2__"
#endif
typedef int SM_CONF_PRIM_DETACH_FN(struct sm_conf_prim_detach_parms *confp);

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_detach(struct sm_conf_prim_detach_parms *confp);


typedef struct sm_conf_prim_info_parms {
	tSMChannelId channel;
	tSM_INT participant_count;
	char speakers[8];
} SM_CONF_PRIM_INFO_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_conf_prim_info sm_conf_prim_info_iPsCticr8_
#define SM_CONF_PRIM_INFO_NAME "sm_conf_prim_info_iPsCticr8_"
#endif
typedef int SM_CONF_PRIM_INFO_FN(struct sm_conf_prim_info_parms *confp);

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_info(struct sm_conf_prim_info_parms *confp);


typedef struct sm_conf_prim_leave_parms {
	tSMChannelId channel;
	tSM_INT id;
} SM_CONF_PRIM_LEAVE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_conf_prim_leave sm_conf_prim_leave_iPsCti_
#define SM_CONF_PRIM_LEAVE_NAME "sm_conf_prim_leave_iPsCti_"
#endif
typedef int SM_CONF_PRIM_LEAVE_FN(struct sm_conf_prim_leave_parms *confp);

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_leave(struct sm_conf_prim_leave_parms *confp);


typedef struct sm_conf_prim_start_parms {
	tSMChannelId channel;
	tSM_INT volume;
	tSM_INT agc;
	enum kSMConfType conf_type;
} SM_CONF_PRIM_START_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_conf_prim_start sm_conf_prim_start_iPsCtiie1_2__
#define SM_CONF_PRIM_START_NAME "sm_conf_prim_start_iPsCtiie1_2__"
#endif
typedef int SM_CONF_PRIM_START_FN(struct sm_conf_prim_start_parms *confp);

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_start(struct sm_conf_prim_start_parms *confp);


typedef struct sm_conf_prim_status_parms {
	tSMChannelId channel;
	enum kSMConfStatus status;
	union {
		struct {
			struct conf_active_input {
				tSM_INT id;
				tSM_INT power;
			} input[4];
		} active_inputs;
	} u;
} SM_CONF_PRIM_STATUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_conf_prim_status sm_conf_prim_status_iPsCte1_3_ussii_r4___
#define SM_CONF_PRIM_STATUS_NAME "sm_conf_prim_status_iPsCte1_3_ussii_r4___"
#endif
typedef int SM_CONF_PRIM_STATUS_FN(struct sm_conf_prim_status_parms *statusp);

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_status(struct sm_conf_prim_status_parms *statusp);


typedef struct sm_conf_prim_stop_parms {
	tSMChannelId channel;
	tSM_UT32 no_wait;
} SM_CONF_PRIM_STOP_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_conf_prim_stop sm_conf_prim_stop_iPsCtUi_
#define SM_CONF_PRIM_STOP_NAME "sm_conf_prim_stop_iPsCtUi_"
#endif
typedef int SM_CONF_PRIM_STOP_FN(struct sm_conf_prim_stop_parms *stopp);

ACUAPI int ACUTiNG_WINAPI sm_conf_prim_stop(struct sm_conf_prim_stop_parms *stopp);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_discard_recognised sm_discard_recognised_iCt
#define SM_DISCARD_RECOGNISED_NAME "sm_discard_recognised_iCt"
#endif
typedef int SM_DISCARD_RECOGNISED_FN(tSMChannelId channel);

ACUAPI int ACUTiNG_WINAPI sm_discard_recognised(tSMChannelId channel);


typedef struct sm_recognised_parms {
	tSMChannelId channel;
	enum kSMRecognition type;
	tSM_INT param0;
	tSM_INT param1;
} SM_RECOGNISED_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_get_recognised sm_get_recognised_iPsCte1_16_ii_
#define SM_GET_RECOGNISED_NAME "sm_get_recognised_iPsCte1_16_ii_"
#endif
typedef int SM_GET_RECOGNISED_FN(struct sm_recognised_parms *recogp);

ACUAPI int ACUTiNG_WINAPI sm_get_recognised(struct sm_recognised_parms *recogp);


typedef struct sm_ts_data_parms {
	tSMChannelId channel;
	char *data;
	tSM_INT length;
} SM_TS_DATA_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_get_recorded_data sm_get_recorded_data_iPsCtPci_
#define SM_GET_RECORDED_DATA_NAME "sm_get_recorded_data_iPsCtPci_"
#endif
typedef int SM_GET_RECORDED_DATA_FN(struct sm_ts_data_parms *datap);

ACUAPI int ACUTiNG_WINAPI sm_get_recorded_data(struct sm_ts_data_parms *datap);


typedef struct sm_listen_for_parms {
	tSMChannelId channel;
	enum kSMToneDetection tone_detection_mode;
	tSM_INT active_tone_set_id;
	enum kSMDigitMapping map_tones_to_digits;
	tSM_INT enable_cptone_recognition;
	tSM_INT enable_grunt_detection;
	tSM_INT grunt_latency;
	double min_noise_level;
	double grunt_threshold;
	tSM_UT32 grunt_holdoff;
} SM_LISTEN_FOR_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_listen_for sm_listen_for_iPsCte1_11_ie1_2_iiiddUi_
#define SM_LISTEN_FOR_NAME "sm_listen_for_iPsCte1_11_ie1_2_iiiddUi_"
#endif
typedef int SM_LISTEN_FOR_FN(struct sm_listen_for_parms *listenp);

ACUAPI int ACUTiNG_WINAPI sm_listen_for(struct sm_listen_for_parms *listenp);


typedef struct sm_onhook_listen_for_parms {
	tSMChannelId channel;
	tSM_INT enable;
	double pre_pulse_max_power;
	double pulse_min_power;
	double post_pulse_floor;
	double latitude;
	double count_ratio;
	double total_ratio;
	tSM_INT max_duration;
} SM_ONHOOK_LISTEN_FOR_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_onhook_listen_for sm_onhook_listen_for_iPsCtiddddddi_
#define SM_ONHOOK_LISTEN_FOR_NAME "sm_onhook_listen_for_iPsCtiddddddi_"
#endif
typedef int SM_ONHOOK_LISTEN_FOR_FN(struct sm_onhook_listen_for_parms *listenp);

ACUAPI int ACUTiNG_WINAPI sm_onhook_listen_for(struct sm_onhook_listen_for_parms *listenp);


typedef struct sm_play_cptone_parms {
	tSMChannelId channel;
	tSM_UT32 duration;
	tSM_INT wait_for_completion;
	enum kSMPlayCPToneType type;
	tSM_INT tone_count;
	struct sm_cadence {
		tSM_INT tone_id;
		tSM_INT on_cadence;
		tSM_INT off_cadence;
	} cadences[kSMMaxPlayCPToneCadences];
} SM_PLAY_CPTONE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_play_cptone sm_play_cptone_iPsCtUiie1_3_isiii_r8_
#define SM_PLAY_CPTONE_NAME "sm_play_cptone_iPsCtUiie1_3_isiii_r8_"
#endif
typedef int SM_PLAY_CPTONE_FN(struct sm_play_cptone_parms *cptonep);

ACUAPI int ACUTiNG_WINAPI sm_play_cptone(struct sm_play_cptone_parms *cptonep);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_play_cptone_abort sm_play_cptone_abort_iCt
#define SM_PLAY_CPTONE_ABORT_NAME "sm_play_cptone_abort_iCt"
#endif
typedef int SM_PLAY_CPTONE_ABORT_FN(tSMChannelId channel);

ACUAPI int ACUTiNG_WINAPI sm_play_cptone_abort(tSMChannelId channel);


typedef struct sm_play_cptone_status_parms {
	tSMChannelId channel;
	enum kSMPlayCPToneStatus status;
} SM_PLAY_CPTONE_STATUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_play_cptone_status sm_play_cptone_status_iPsCte1_2__
#define SM_PLAY_CPTONE_STATUS_NAME "sm_play_cptone_status_iPsCte1_2__"
#endif
typedef int SM_PLAY_CPTONE_STATUS_FN(struct sm_play_cptone_status_parms *statusp);

ACUAPI int ACUTiNG_WINAPI sm_play_cptone_status(struct sm_play_cptone_status_parms *statusp);


typedef struct sm_play_digits_parms {
	tSMChannelId channel;
	tSM_INT wait_for_completion;
	struct sm_digits {
		enum kSMDigitType type;
		tSM_INT qualifier;
		char digit_string[kSMMaxDigits_plus1];
		tSM_INT inter_digit_delay;
		tSM_INT digit_duration;
	} digits;
} SM_PLAY_DIGITS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_play_digits sm_play_digits_iPsCtise1_1_icr33ii__
#define SM_PLAY_DIGITS_NAME "sm_play_digits_iPsCtise1_1_icr33ii__"
#endif
typedef int SM_PLAY_DIGITS_FN(struct sm_play_digits_parms *digitsp);

ACUAPI int ACUTiNG_WINAPI sm_play_digits(struct sm_play_digits_parms *digitsp);


typedef struct sm_play_digits_status_parms {
	tSMChannelId channel;
	enum kSMPlayDigitsStatus status;
} SM_PLAY_DIGITS_STATUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_play_digits_status sm_play_digits_status_iPsCte1_2__
#define SM_PLAY_DIGITS_STATUS_NAME "sm_play_digits_status_iPsCte1_2__"
#endif
typedef int SM_PLAY_DIGITS_STATUS_FN(struct sm_play_digits_status_parms *statusp);

ACUAPI int ACUTiNG_WINAPI sm_play_digits_status(struct sm_play_digits_status_parms *statusp);


typedef struct sm_play_tone_parms {
	tSMChannelId channel;
	tSM_UT32 duration;
	tSM_INT wait_for_completion;
	tSM_INT tone_id;
} SM_PLAY_TONE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_play_tone sm_play_tone_iPsCtUiii_
#define SM_PLAY_TONE_NAME "sm_play_tone_iPsCtUiii_"
#endif
typedef int SM_PLAY_TONE_FN(struct sm_play_tone_parms *tonep);

ACUAPI int ACUTiNG_WINAPI sm_play_tone(struct sm_play_tone_parms *tonep);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_play_tone_abort sm_play_tone_abort_iCt
#define SM_PLAY_TONE_ABORT_NAME "sm_play_tone_abort_iCt"
#endif
typedef int SM_PLAY_TONE_ABORT_FN(tSMChannelId channel);

ACUAPI int ACUTiNG_WINAPI sm_play_tone_abort(tSMChannelId channel);


typedef struct sm_play_tone_list_parms {
	tSMChannelId channel;
	struct sm_play_tone_item {
		enum kSMToneOperation operation;
		tSM_UT32 duration;
		double frequency1;
		double amplitude1;
		double frequency2;
		double amplitude2;
	} *tones;
	tSM_INT tone_count;
} SM_PLAY_TONE_LIST_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_play_tone_list sm_play_tone_list_iPsCtPse1_3_Uidddd_i_
#define SM_PLAY_TONE_LIST_NAME "sm_play_tone_list_iPsCtPse1_3_Uidddd_i_"
#endif
typedef int SM_PLAY_TONE_LIST_FN(struct sm_play_tone_list_parms *tonep);

ACUAPI int ACUTiNG_WINAPI sm_play_tone_list(struct sm_play_tone_list_parms *tonep);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_play_tone_list_abort sm_play_tone_list_abort_iCt
#define SM_PLAY_TONE_LIST_ABORT_NAME "sm_play_tone_list_abort_iCt"
#endif
typedef int SM_PLAY_TONE_LIST_ABORT_FN(tSMChannelId channel);

ACUAPI int ACUTiNG_WINAPI sm_play_tone_list_abort(tSMChannelId channel);


typedef struct sm_play_tone_list_phase_reverse_parms {
	tSMChannelId channel;
	tSM_UT32 period;
} SM_PLAY_TONE_LIST_PHASE_REVERSE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_play_tone_list_phase_reverse sm_play_tone_list_phase_reverse_iPsCtUi_
#define SM_PLAY_TONE_LIST_PHASE_REVERSE_NAME "sm_play_tone_list_phase_reverse_iPsCtUi_"
#endif
typedef int SM_PLAY_TONE_LIST_PHASE_REVERSE_FN(struct sm_play_tone_list_phase_reverse_parms *pp);

ACUAPI int ACUTiNG_WINAPI sm_play_tone_list_phase_reverse(struct sm_play_tone_list_phase_reverse_parms *pp);


typedef struct sm_play_tone_list_status_parms {
	tSMChannelId channel;
	enum kSMPlayToneListStatus status;
} SM_PLAY_TONE_LIST_STATUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_play_tone_list_status sm_play_tone_list_status_iPsCte1_4__
#define SM_PLAY_TONE_LIST_STATUS_NAME "sm_play_tone_list_status_iPsCte1_4__"
#endif
typedef int SM_PLAY_TONE_LIST_STATUS_FN(struct sm_play_tone_list_status_parms *statusp);

ACUAPI int ACUTiNG_WINAPI sm_play_tone_list_status(struct sm_play_tone_list_status_parms *statusp);


typedef struct sm_play_tone_status_parms {
	tSMChannelId channel;
	enum kSMPlayToneStatus status;
} SM_PLAY_TONE_STATUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_play_tone_status sm_play_tone_status_iPsCte1_2__
#define SM_PLAY_TONE_STATUS_NAME "sm_play_tone_status_iPsCte1_2__"
#endif
typedef int SM_PLAY_TONE_STATUS_FN(struct sm_play_tone_status_parms *statusp);

ACUAPI int ACUTiNG_WINAPI sm_play_tone_status(struct sm_play_tone_status_parms *statusp);


typedef struct sm_audio_data_parms {
	tSMChannelId channel;
	char *data;
	tSM_INT max_length;
	tSM_INT done_length;
} SM_AUDIO_DATA_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_put_audio_data sm_put_audio_data_iPsCtPcii_
#define SM_PUT_AUDIO_DATA_NAME "sm_put_audio_data_iPsCtPcii_"
#endif
typedef int SM_PUT_AUDIO_DATA_FN(struct sm_audio_data_parms *datap);

ACUAPI int ACUTiNG_WINAPI sm_put_audio_data(struct sm_audio_data_parms *datap);



#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_put_last_replay_data sm_put_last_replay_data_iPsCtPci_
#define SM_PUT_LAST_REPLAY_DATA_NAME "sm_put_last_replay_data_iPsCtPci_"
#endif
typedef int SM_PUT_LAST_REPLAY_DATA_FN(struct sm_ts_data_parms *datap);

ACUAPI int ACUTiNG_WINAPI sm_put_last_replay_data(struct sm_ts_data_parms *datap);



#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_put_replay_data sm_put_replay_data_iPsCtPci_
#define SM_PUT_REPLAY_DATA_NAME "sm_put_replay_data_iPsCtPci_"
#endif
typedef int SM_PUT_REPLAY_DATA_FN(struct sm_ts_data_parms *datap);

ACUAPI int ACUTiNG_WINAPI sm_put_replay_data(struct sm_ts_data_parms *datap);


typedef struct sm_record_abort_parms {
	tSMChannelId channel;
	tSM_INT discard;
} SM_RECORD_ABORT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_record_abort sm_record_abort_iPsCti_
#define SM_RECORD_ABORT_NAME "sm_record_abort_iPsCti_"
#endif
typedef int SM_RECORD_ABORT_FN(struct sm_record_abort_parms *abortp);

ACUAPI int ACUTiNG_WINAPI sm_record_abort(struct sm_record_abort_parms *abortp);


typedef struct sm_record_agc_adjust_parms {
	tSMChannelId channel;
	tSM_INT gain;
} SM_RECORD_AGC_ADJUST_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_record_agc_adjust sm_record_agc_adjust_iPsCti_
#define SM_RECORD_AGC_ADJUST_NAME "sm_record_agc_adjust_iPsCti_"
#endif
typedef int SM_RECORD_AGC_ADJUST_FN(struct sm_record_agc_adjust_parms *recadjp);

ACUAPI int ACUTiNG_WINAPI sm_record_agc_adjust(struct sm_record_agc_adjust_parms *recadjp);


typedef struct sm_record_agc_adjust_settings_parms {
	tSMChannelId channel;
	float max_level_decay;
	float target_level;
} SM_RECORD_AGC_ADJUST_SETTINGS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_record_agc_adjust_settings sm_record_agc_adjust_settings_iPsCtff_
#define SM_RECORD_AGC_ADJUST_SETTINGS_NAME "sm_record_agc_adjust_settings_iPsCtff_"
#endif
typedef int SM_RECORD_AGC_ADJUST_SETTINGS_FN(struct sm_record_agc_adjust_settings_parms *recadjp);

ACUAPI int ACUTiNG_WINAPI sm_record_agc_adjust_settings(struct sm_record_agc_adjust_settings_parms *recadjp);


typedef struct sm_record_parms {
	tSMChannelId channel;
	tSMChannelId alt_data_source;
	enum kSMDataFormat type;
	tSM_UT32 silence_elimination;
	enum kSMToneDetection tone_elimination_mode;
	tSM_UT32 tone_elimination_set_id;
	tSM_UT32 max_octets;
	tSM_UT32 max_elapsed_time;
	tSM_UT32 max_silence;
	tSM_INT agc;
	tSM_INT volume;
	enum kSMRecordAltSource alt_data_source_type;
	tSM_UT32 sampling_rate;
	double min_noise_level;
	double grunt_threshold;
	tSM_UT32 grunt_holdoff;
	tSM_UT32 max_initial_silence;
} SM_RECORD_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_record_start sm_record_start_iPsCtCte0_0_17_17_30_37_Uie1_11_UiUiUiUiiie1_3_UiddUiUi_
#define SM_RECORD_START_NAME "sm_record_start_iPsCtCte0_0_17_17_30_37_Uie1_11_UiUiUiUiiie1_3_UiddUiUi_"
#endif
typedef int SM_RECORD_START_FN(struct sm_record_parms *recordp);

ACUAPI int ACUTiNG_WINAPI sm_record_start(struct sm_record_parms *recordp);


typedef struct sm_record_status_parms {
	tSMChannelId channel;
	enum kSMRecordStatus status;
	enum kSMRecognition recog_type;
	tSM_INT param0;
	tSM_INT param1;
	enum kSMRecordHowTerminated termination_reason;
	tSM_UT32 termination_octets;
	tSM_UT32 sample_rate;
} SM_RECORD_STATUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_record_status sm_record_status_iPsCte1_6_e1_16_iie1_6_UiUi_
#define SM_RECORD_STATUS_NAME "sm_record_status_iPsCte1_6_e1_16_iie1_6_UiUi_"
#endif
typedef int SM_RECORD_STATUS_FN(struct sm_record_status_parms *statusp);

ACUAPI int ACUTiNG_WINAPI sm_record_status(struct sm_record_status_parms *statusp);


typedef struct sm_replay_abort_parms {
	tSMChannelId channel;
	tSM_UT32 offset;
	tSM_UT32 nowait;
} SM_REPLAY_ABORT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_replay_abort sm_replay_abort_iPsCtUiUi_
#define SM_REPLAY_ABORT_NAME "sm_replay_abort_iPsCtUiUi_"
#endif
typedef int SM_REPLAY_ABORT_FN(struct sm_replay_abort_parms *abortp);

ACUAPI int ACUTiNG_WINAPI sm_replay_abort(struct sm_replay_abort_parms *abortp);


typedef struct sm_replay_adjust_parms {
	tSMChannelId channel;
	tSMChannelId background;
	tSM_INT volume;
	tSM_INT agc;
	tSM_INT speed;
} SM_REPLAY_ADJUST_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_replay_adjust sm_replay_adjust_iPsCtCtiii_
#define SM_REPLAY_ADJUST_NAME "sm_replay_adjust_iPsCtCtiii_"
#endif
typedef int SM_REPLAY_ADJUST_FN(struct sm_replay_adjust_parms *adjustp);

ACUAPI int ACUTiNG_WINAPI sm_replay_adjust(struct sm_replay_adjust_parms *adjustp);


typedef struct sm_replay_parms {
	tSMChannelId channel;
	tSMChannelId background;
	tSM_INT volume;
	tSM_INT agc;
	tSM_INT speed;
	enum kSMDataFormat type;
	tSM_UT32 data_length;
	tSM_UT32 sampling_rate;
} SM_REPLAY_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_replay_start sm_replay_start_iPsCtCtiiie0_0_17_17_30_37_UiUi_
#define SM_REPLAY_START_NAME "sm_replay_start_iPsCtCtiiie0_0_17_17_30_37_UiUi_"
#endif
typedef int SM_REPLAY_START_FN(struct sm_replay_parms *replayp);

ACUAPI int ACUTiNG_WINAPI sm_replay_start(struct sm_replay_parms *replayp);


typedef struct sm_replay_status_parms {
	tSMChannelId channel;
	enum kSMReplayStatus status;
	tSM_UT32 offset;
} SM_REPLAY_STATUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_replay_status sm_replay_status_iPsCte1_5_Ui_
#define SM_REPLAY_STATUS_NAME "sm_replay_status_iPsCte1_5_Ui_"
#endif
typedef int SM_REPLAY_STATUS_FN(struct sm_replay_status_parms *statusp);

ACUAPI int ACUTiNG_WINAPI sm_replay_status(struct sm_replay_status_parms *statusp);


typedef struct sm_reset_input_cptones_parms {
	tSMModuleId module;
	tSM_INT tone_set_id;
} SM_RESET_INPUT_CPTONES_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_reset_input_cptones sm_reset_input_cptones_iPsMti_
#define SM_RESET_INPUT_CPTONES_NAME "sm_reset_input_cptones_iPsMti_"
#endif
typedef int SM_RESET_INPUT_CPTONES_FN(struct sm_reset_input_cptones_parms *resetp);

ACUAPI int ACUTiNG_WINAPI sm_reset_input_cptones(struct sm_reset_input_cptones_parms *resetp);


typedef struct sm_input_vocab_parms {
	tSMModuleId module;
	char *filename;
	tSM_UT32 item_id;
} SM_INPUT_VOCAB_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_add_input_vocab sm_add_input_vocab_iPsMtPcUi_
#define SM_ADD_INPUT_VOCAB_NAME "sm_add_input_vocab_iPsMtPcUi_"
#endif
typedef int SM_ADD_INPUT_VOCAB_FN(struct sm_input_vocab_parms *vocabp);

ACUAPI int ACUTiNG_WINAPI sm_add_input_vocab(struct sm_input_vocab_parms *vocabp);


typedef struct sm_asr_listen_for_parms {
	tSMChannelId channel;
	tSM_INT vocab_item_count;
	tSM_UT32 *vocab_item_ids;
	tSM_INT *vocab_recog_ids;
	enum kSMASRMode asr_mode;
	struct sm_asr_characteristics {
		tSM_INT vfr_max_frames;
		double vfr_diff_threshold;
		tSM_INT pse_max_frames;
		tSM_INT pse_min_frames;
		double vit_soft_threshold;
		double vit_hard_threshold;
		double vit_snr_adjust;
	} *specific_parameters;
	tSMChannelId sidetone;
} SM_ASR_LISTEN_FOR_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_asr_listen_for sm_asr_listen_for_iPsCtiPUiPie1_3_Psidiiddd_Ct_
#define SM_ASR_LISTEN_FOR_NAME "sm_asr_listen_for_iPsCtiPUiPie1_3_Psidiiddd_Ct_"
#endif
typedef int SM_ASR_LISTEN_FOR_FN(struct sm_asr_listen_for_parms *listenp);

ACUAPI int ACUTiNG_WINAPI sm_asr_listen_for(struct sm_asr_listen_for_parms *listenp);


typedef struct sm_reset_input_vocabs_parms {
	tSMModuleId module;
} SM_RESET_INPUT_VOCABS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_reset_input_vocabs sm_reset_input_vocabs_iPsMt_
#define SM_RESET_INPUT_VOCABS_NAME "sm_reset_input_vocabs_iPsMt_"
#endif
typedef int SM_RESET_INPUT_VOCABS_FN(struct sm_reset_input_vocabs_parms *resetp);

ACUAPI int ACUTiNG_WINAPI sm_reset_input_vocabs(struct sm_reset_input_vocabs_parms *resetp);


typedef struct sm_set_sidetone_channel_parms {
	tSMChannelId channel;
	tSMChannelId output;
} SM_SET_SIDETONE_CHANNEL_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_set_sidetone_channel sm_set_sidetone_channel_iPsCtCt_
#define SM_SET_SIDETONE_CHANNEL_NAME "sm_set_sidetone_channel_iPsCtCt_"
#endif
typedef int SM_SET_SIDETONE_CHANNEL_FN(struct sm_set_sidetone_channel_parms *sidetp);

ACUAPI int ACUTiNG_WINAPI sm_set_sidetone_channel(struct sm_set_sidetone_channel_parms *sidetp);



#endif
