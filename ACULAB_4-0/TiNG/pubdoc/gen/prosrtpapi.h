/* prosrtpapi.h - Prosody RTP processing API declarations */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

#ifndef INCLUDED_PUBDOC_PROSRTPAPI_H
#define INCLUDED_PUBDOC_PROSRTPAPI_H

#ifdef ACULAB_TiNG_VERSION_NUM
#if ACULAB_TiNG_VERSION_NUM != 1504
#error ACULAB_TiNG_VERSION_NUM does not match 1504
#endif
#else
#define ACULAB_TiNG_VERSION_NUM 1504
#define ACULAB_TiNG_VERSION_CHANGED 
#define ACULAB_TiNG_VERSION_STRING "m1504."
#endif

#define kVMPrxFwdMaxTypes 8
enum kSMRTCPHandStatus {
	kSMRTCPHandStatusRunning,
};

enum kSMPLCMode {
	kSMPLCModeDisabled,
	kSMPLCModeEnabled,
};

enum kSMEVRCRTPMode {
	kSMEVRCRTPModeHeaderless,
	kSMEVRCRTPModeRFC2658,
	kSMEVRCRTPModeRFC3558,
};

enum kSMG726Variant {
	kSMG726VariantRFC3551,
	kSMG726VariantAAL2,
};

enum kSMG729IMode {
	kSMG729IModeG729D,
	kSMG729IModeG729,
	kSMG729IModeG729E,
};

enum kSMGSMVariant {
	kSMGSMVariantStandard,
	kSMGSMVariantMS,
};

enum kSMSpeexMode {
	kSMSpeexNarrowbandMode,
	kSMSpeexWidebandMode,
	kSMSpeexUltrawidebandMode,
};

enum kVMPrxFwdMode {
	kVMPrxFwdModeNone,
	kVMPrxFwdModeAll,
	kVMPrxFwdModeTypes,
};

enum kSMVMPrxFwdDestType {
	kSMVMPrxFwdDestTypeIPv4,
	kSMVMPrxFwdDestTypeIPv6,
};

enum kVMPrxFwdTranscoding {
	kVMPrxFwdTranscodingNone,
	kVMPrxFwdTranscodingAlawToMulaw,
	kVMPrxFwdTranscodingMulawToAlaw,
};

enum kSMVMPrxJitterBufferMode {
	kSMVMPrxJitterBufferModeNormal,
	kSMVMPrxJitterBufferModeAdaptive,
};

enum kSMVMPrxType {
	kSMVMPrxTypeIPv4,
	kSMVMPrxTypeIPv6,
};

enum kSMICERole {
	kSMICERoleNone,
	kSMICERoleLite,
	kSMICERoleControlled,
	kSMICERoleControlling,
};

enum kSMVMPrxStatus {
	kSMVMPrxStatusRunning,
	kSMVMPrxStatusStopped,
	kSMVMPrxStatusDetectTone,
	kSMVMPrxStatusEndTone,
	kSMVMPrxStatusGotPorts,
	kSMVMPrxStatusNewSSRC,
	kSMVMPrxStatusUnhandledPayload,
	kSMVMPrxStatusCodecSpecific,
	kSMVMPrxStatusGotPortsIPv6,
	kSMVMPrxStatusNewSSRCIPv6,
	kSMVMPrxStatusSTUNBindRequest,
	kSMVMPrxStatusSTUNBindResponse,
	kSMVMPrxStatusJBResync,
	kSMVMPrxStatusProfileSpecific,
};

enum kSMAddressType {
	kSMAddressTypeIPv4,
	kSMAddressTypeIPv6,
};

enum kSMVMPrxSTUNResult {
	kSMVMPrxSTUNResultSuccess,
	kSMVMPrxSTUNResultError,
	kSMVMPrxSTUNResultInvalid,
	kSMVMPrxSTUNResultTimeout,
};

enum kSMVMPrxAMRNBStatus {
	kSMVMPrxAMRNBStatusNone,
	kSMVMPrxAMRNBStatusNewCMR,
};

enum kSMVMPrxAMRWBStatus {
	kSMVMPrxAMRWBStatusNone,
	kSMVMPrxAMRWBStatusNewCMR,
};

enum kSMVMPrxSSEStatus {
	kSMVMPrxSSEStatusNone,
	kSMVMPrxSSEStatusMessage,
};

enum kSMVMPTxVADMode {
	kSMVMPTxVADModeDisabled,
	kSMVMPTxVADModeEnabled,
	kSMVMPTxVADModeComfortNoise,
};

enum kSMEVRCRate {
	kSMEVRCRateEighth,
	kSMEVRCRateHalf,
	kSMEVRCRateFull,
};

enum kSMOPUSRate {
	kSMOPUSRateConstrainedVariable,
	kSMOPUSRateUnconstrainedVariable,
	kSMOPUSRateConstant,
};

enum kSMOPUSSignalTypeHint {
	kSMOPUSSignalTypeHintNone,
	kSMOPUSSignalTypeHintVoice,
	kSMOPUSSignalTypeHintMusic,
};

enum kSMOPUSAppType {
	kSMOPUSAppTypeVoIP,
	kSMOPUSAppTypeAudio,
	kSMOPUSAppTypeLowDelay,
};

enum kSMVMPTxTagType {
	kSMVMPTxTagTypeUlaw,
	kSMVMPTxTagTypeAlaw,
};

enum kSMVMPtxRFC2833MinDuration {
	kSMVMPtxRFC2833MinDurationNoMinimum,
	kSMVMPtxRFC2833MinDuration40,
};

enum kSMVMPTxCSRCType {
	kSMVMPTxCSRCTypeArray,
	kSMVMPTxCSRCTypeList,
};

enum kSMVMPtxStatus {
	kSMVMPtxStatusRunning,
	kSMVMPtxStatusStopped,
	kSMVMPtxStatusToneOngoing,
	kSMVMPtxStatusToneCompleted,
	kSMVMPtxStatusSSRC,
};

enum kSMRFC3984PicSize {
	kSMRFC3984PicSizeSubQCIF=1,
	kSMRFC3984PicSizeQCIF,
	kSMRFC3984PicSizeCIF,
	kSMRFC3984PicSize4CIF,
	kSMRFC3984PicSize16CIF,
};

enum kSMVidMPrxType {
	kSMVidMPrxTypeIPv4,
	kSMVidMPrxTypeIPv6,
};

enum kSMVidMPrxStatus {
	kSMVidMPrxStatusRunning,
	kSMVidMPrxStatusStopped,
	kSMVidMPrxStatusGotPorts,
	kSMVidMPrxStatusNewSSRC,
	kSMVidMPrxStatusUnhandledPayload,
	kSMVidMPrxStatusInternalError,
	kSMVidMPrxStatusGotPortsIPv6,
	kSMVidMPrxStatusNewSSRCIPv6,
	kSMVidMPrxStatusSTUNBindRequest,
	kSMVidMPrxStatusSTUNBindResponse,
};

enum kSMVidMPrxSTUNResult {
	kSMVidMPrxSTUNResultSuccess,
	kSMVidMPrxSTUNResultError,
	kSMVidMPrxSTUNResultInvalid,
	kSMVidMPrxSTUNResultTimeout,
};

enum kSMVidMPtxStatus {
	kSMVidMPtxStatusRunning,
	kSMVidMPtxStatusStopped,
	kSMVidMPtxStatusSSRC,
	kSMVidMPtxStatusInternalError,
};

enum kSMCodecType {
	kSMCodecTypeAlaw,
	kSMCodecTypeMulaw,
	kSMCodecTypeG729AB,
	kSMCodecTypeRFC2833,
	kSMCodecTypeComfortNoise,
};



typedef struct sm_rtcphand_config_bandwidth_parms {
	tSMRTCPHandId rtcphand;
	tSM_INT rtcp_bw;
	tSM_INT rtcprx_bw;
	tSM_INT rtcptx_bw;
} SM_RTCPHAND_CONFIG_BANDWIDTH_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_rtcphand_config_bandwidth sm_rtcphand_config_bandwidth_iPsRsiii_
#define SM_RTCPHAND_CONFIG_BANDWIDTH_NAME "sm_rtcphand_config_bandwidth_iPsRsiii_"
#endif
typedef int SM_RTCPHAND_CONFIG_BANDWIDTH_FN(struct sm_rtcphand_config_bandwidth_parms *rvp);

ACUAPI int ACUTiNG_WINAPI sm_rtcphand_config_bandwidth(struct sm_rtcphand_config_bandwidth_parms *rvp);


typedef struct sm_rtcphand_config_options_parms {
	tSMRTCPHandId rtcphand;
	tSM_INT rtcptx_suppress_xr;
} SM_RTCPHAND_CONFIG_OPTIONS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_rtcphand_config_options sm_rtcphand_config_options_iPsRsi_
#define SM_RTCPHAND_CONFIG_OPTIONS_NAME "sm_rtcphand_config_options_iPsRsi_"
#endif
typedef int SM_RTCPHAND_CONFIG_OPTIONS_FN(struct sm_rtcphand_config_options_parms *rvp);

ACUAPI int ACUTiNG_WINAPI sm_rtcphand_config_options(struct sm_rtcphand_config_options_parms *rvp);


typedef struct sm_rtcphand_config_reports_parms {
	tSMRTCPHandId rtcphand;
	tSM_INT reports;
} SM_RTCPHAND_CONFIG_REPORTS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_rtcphand_config_reports sm_rtcphand_config_reports_iPsRsi_
#define SM_RTCPHAND_CONFIG_REPORTS_NAME "sm_rtcphand_config_reports_iPsRsi_"
#endif
typedef int SM_RTCPHAND_CONFIG_REPORTS_FN(struct sm_rtcphand_config_reports_parms *rvp);

ACUAPI int ACUTiNG_WINAPI sm_rtcphand_config_reports(struct sm_rtcphand_config_reports_parms *rvp);


typedef struct sm_rtcphand_config_sdes_parms {
	tSMRTCPHandId rtcphand;
	tSM_INT itemnum;
	tSM_INT stringlen;
	char *stringval;
} SM_RTCPHAND_CONFIG_SDES_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_rtcphand_config_sdes sm_rtcphand_config_sdes_iPsRsiiPc_
#define SM_RTCPHAND_CONFIG_SDES_NAME "sm_rtcphand_config_sdes_iPsRsiiPc_"
#endif
typedef int SM_RTCPHAND_CONFIG_SDES_FN(struct sm_rtcphand_config_sdes_parms *rvp);

ACUAPI int ACUTiNG_WINAPI sm_rtcphand_config_sdes(struct sm_rtcphand_config_sdes_parms *rvp);


typedef struct sm_rtcphand_create_parms {
	tSMRTCPHandId rtcphand;
	tSMModuleId module;
} SM_RTCPHAND_CREATE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_rtcphand_create sm_rtcphand_create_iPsRsMt_
#define SM_RTCPHAND_CREATE_NAME "sm_rtcphand_create_iPsRsMt_"
#endif
typedef int SM_RTCPHAND_CREATE_FN(struct sm_rtcphand_create_parms *rtcphandp);

ACUAPI int ACUTiNG_WINAPI sm_rtcphand_create(struct sm_rtcphand_create_parms *rtcphandp);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_rtcphand_destroy sm_rtcphand_destroy_iRs
#define SM_RTCPHAND_DESTROY_NAME "sm_rtcphand_destroy_iRs"
#endif
typedef int SM_RTCPHAND_DESTROY_FN(tSMRTCPHandId rtcphand);

ACUAPI int ACUTiNG_WINAPI sm_rtcphand_destroy(tSMRTCPHandId rtcphand);


typedef struct sm_rtcphand_get_data_parms {
	tSMRTCPHandId rtcphand;
	char *data;
	tSM_INT max_length;
	tSM_INT done_length;
} SM_RTCPHAND_GET_DATA_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_rtcphand_get_data sm_rtcphand_get_data_iPsRsPcii_
#define SM_RTCPHAND_GET_DATA_NAME "sm_rtcphand_get_data_iPsRsPcii_"
#endif
typedef int SM_RTCPHAND_GET_DATA_FN(struct sm_rtcphand_get_data_parms *datap);

ACUAPI int ACUTiNG_WINAPI sm_rtcphand_get_data(struct sm_rtcphand_get_data_parms *datap);


typedef struct sm_rtcphand_event_parms {
	tSMRTCPHandId rtcphand;
	tSMEventId event;
} SM_RTCPHAND_EVENT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_rtcphand_get_event sm_rtcphand_get_event_iPsRsEt_
#define SM_RTCPHAND_GET_EVENT_NAME "sm_rtcphand_get_event_iPsRsEt_"
#endif
typedef int SM_RTCPHAND_GET_EVENT_FN(struct sm_rtcphand_event_parms *eventp);

ACUAPI int ACUTiNG_WINAPI sm_rtcphand_get_event(struct sm_rtcphand_event_parms *eventp);


typedef struct sm_rtcphand_request_statistics_parms {
	tSMRTCPHandId rtcphand;
	tSM_INT statcode;
} SM_RTCPHAND_REQUEST_STATISTICS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_rtcphand_request_statistics sm_rtcphand_request_statistics_iPsRsi_
#define SM_RTCPHAND_REQUEST_STATISTICS_NAME "sm_rtcphand_request_statistics_iPsRsi_"
#endif
typedef int SM_RTCPHAND_REQUEST_STATISTICS_FN(struct sm_rtcphand_request_statistics_parms *rrsp);

ACUAPI int ACUTiNG_WINAPI sm_rtcphand_request_statistics(struct sm_rtcphand_request_statistics_parms *rrsp);


typedef struct sm_rtcphand_status_parms {
	tSMRTCPHandId rtcphand;
	enum kSMRTCPHandStatus status;
} SM_RTCPHAND_STATUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_rtcphand_status sm_rtcphand_status_iPsRse1_1__
#define SM_RTCPHAND_STATUS_NAME "sm_rtcphand_status_iPsRse1_1__"
#endif
typedef int SM_RTCPHAND_STATUS_FN(struct sm_rtcphand_status_parms *statusp);

ACUAPI int ACUTiNG_WINAPI sm_rtcphand_status(struct sm_rtcphand_status_parms *statusp);


typedef struct sm_vmprx_config_authentication_hmac_sha1_parms {
	tSMVMPrxId vmprx;
	tSM_INT keylen;
	char *key;
	tSM_INT taglen;
} SM_VMPRX_CONFIG_AUTHENTICATION_HMAC_SHA1_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_authentication_hmac_sha1 sm_vmprx_config_authentication_hmac_sha1_iPsVriPci_
#define SM_VMPRX_CONFIG_AUTHENTICATION_HMAC_SHA1_NAME "sm_vmprx_config_authentication_hmac_sha1_iPsVriPci_"
#endif
typedef int SM_VMPRX_CONFIG_AUTHENTICATION_HMAC_SHA1_FN(struct sm_vmprx_config_authentication_hmac_sha1_parms *pp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_authentication_hmac_sha1(struct sm_vmprx_config_authentication_hmac_sha1_parms *pp);


typedef struct sm_vmprx_config_authentication_null_parms {
	tSMVMPrxId vmprx;
} SM_VMPRX_CONFIG_AUTHENTICATION_NULL_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_authentication_null sm_vmprx_config_authentication_null_iPsVr_
#define SM_VMPRX_CONFIG_AUTHENTICATION_NULL_NAME "sm_vmprx_config_authentication_null_iPsVr_"
#endif
typedef int SM_VMPRX_CONFIG_AUTHENTICATION_NULL_FN(struct sm_vmprx_config_authentication_null_parms *pp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_authentication_null(struct sm_vmprx_config_authentication_null_parms *pp);


typedef struct sm_vmprx_codec_alaw_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
	enum kSMPLCMode plc_mode;
} SM_VMPRX_CODEC_ALAW_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_alaw sm_vmprx_config_codec_alaw_iPsVrie1_2__
#define SM_VMPRX_CONFIG_CODEC_ALAW_NAME "sm_vmprx_config_codec_alaw_iPsVrie1_2__"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_ALAW_FN(struct sm_vmprx_codec_alaw_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_alaw(struct sm_vmprx_codec_alaw_parms *codecp);


typedef struct sm_vmprx_codec_amrnb_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
	tSM_INT aligned;
	tSM_INT gsmefr;
	enum kSMPLCMode plc_mode;
	tSM_INT report_cmr;
} SM_VMPRX_CODEC_AMRNB_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_amrnb sm_vmprx_config_codec_amrnb_iPsVriiie1_2_i_
#define SM_VMPRX_CONFIG_CODEC_AMRNB_NAME "sm_vmprx_config_codec_amrnb_iPsVriiie1_2_i_"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_AMRNB_FN(struct sm_vmprx_codec_amrnb_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_amrnb(struct sm_vmprx_codec_amrnb_parms *codecp);


typedef struct sm_vmprx_codec_amrwb_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
	tSM_INT aligned;
	enum kSMPLCMode plc_mode;
	tSM_INT report_cmr;
} SM_VMPRX_CODEC_AMRWB_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_amrwb sm_vmprx_config_codec_amrwb_iPsVriie1_2_i_
#define SM_VMPRX_CONFIG_CODEC_AMRWB_NAME "sm_vmprx_config_codec_amrwb_iPsVriie1_2_i_"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_AMRWB_FN(struct sm_vmprx_codec_amrwb_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_amrwb(struct sm_vmprx_codec_amrwb_parms *codecp);


typedef struct sm_vmprx_codec_comfort_noise_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
	enum kSMPLCMode plc_mode;
} SM_VMPRX_CODEC_COMFORT_NOISE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_comfort_noise sm_vmprx_config_codec_comfort_noise_iPsVrie1_2__
#define SM_VMPRX_CONFIG_CODEC_COMFORT_NOISE_NAME "sm_vmprx_config_codec_comfort_noise_iPsVrie1_2__"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_COMFORT_NOISE_FN(struct sm_vmprx_codec_comfort_noise_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_comfort_noise(struct sm_vmprx_codec_comfort_noise_parms *codecp);


typedef struct sm_vmprx_codec_evrc_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
	tSM_INT post_filter;
	enum kSMEVRCRTPMode rtp_mode;
	enum kSMPLCMode plc_mode;
} SM_VMPRX_CODEC_EVRC_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_evrc sm_vmprx_config_codec_evrc_iPsVriie1_3_e1_2__
#define SM_VMPRX_CONFIG_CODEC_EVRC_NAME "sm_vmprx_config_codec_evrc_iPsVriie1_3_e1_2__"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_EVRC_FN(struct sm_vmprx_codec_evrc_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_evrc(struct sm_vmprx_codec_evrc_parms *codecp);


typedef struct sm_vmprx_codec_g722_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
	enum kSMPLCMode plc_mode;
} SM_VMPRX_CODEC_G722_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_g722 sm_vmprx_config_codec_g722_iPsVrie1_2__
#define SM_VMPRX_CONFIG_CODEC_G722_NAME "sm_vmprx_config_codec_g722_iPsVrie1_2__"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_G722_FN(struct sm_vmprx_codec_g722_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_g722(struct sm_vmprx_codec_g722_parms *codecp);


typedef struct sm_vmprx_codec_g722_1_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
	tSM_INT bitrate;
	enum kSMPLCMode plc_mode;
} SM_VMPRX_CODEC_G722_1_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_g722_1 sm_vmprx_config_codec_g722_1_iPsVriie1_2__
#define SM_VMPRX_CONFIG_CODEC_G722_1_NAME "sm_vmprx_config_codec_g722_1_iPsVriie1_2__"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_G722_1_FN(struct sm_vmprx_codec_g722_1_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_g722_1(struct sm_vmprx_codec_g722_1_parms *codecp);


typedef struct sm_vmprx_codec_g723_1_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
	tSM_INT post_filter;
	enum kSMPLCMode plc_mode;
} SM_VMPRX_CODEC_G723_1_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_g723_1 sm_vmprx_config_codec_g723_1_iPsVriie1_2__
#define SM_VMPRX_CONFIG_CODEC_G723_1_NAME "sm_vmprx_config_codec_g723_1_iPsVriie1_2__"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_G723_1_FN(struct sm_vmprx_codec_g723_1_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_g723_1(struct sm_vmprx_codec_g723_1_parms *codecp);


typedef struct sm_vmprx_codec_g726_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
	tSM_INT bits;
	enum kSMPLCMode plc_mode;
	enum kSMG726Variant variant;
} SM_VMPRX_CODEC_G726_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_g726 sm_vmprx_config_codec_g726_iPsVriie1_2_e1_2__
#define SM_VMPRX_CONFIG_CODEC_G726_NAME "sm_vmprx_config_codec_g726_iPsVriie1_2_e1_2__"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_G726_FN(struct sm_vmprx_codec_g726_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_g726(struct sm_vmprx_codec_g726_parms *codecp);


typedef struct sm_vmprx_codec_g728_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
	tSM_INT rate;
	tSM_INT post_filter;
	enum kSMPLCMode plc_mode;
} SM_VMPRX_CODEC_G728_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_g728 sm_vmprx_config_codec_g728_iPsVriiie1_2__
#define SM_VMPRX_CONFIG_CODEC_G728_NAME "sm_vmprx_config_codec_g728_iPsVriiie1_2__"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_G728_FN(struct sm_vmprx_codec_g728_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_g728(struct sm_vmprx_codec_g728_parms *codecp);


typedef struct sm_vmprx_codec_g729ab_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
	enum kSMPLCMode plc_mode;
} SM_VMPRX_CODEC_G729AB_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_g729ab sm_vmprx_config_codec_g729ab_iPsVrie1_2__
#define SM_VMPRX_CONFIG_CODEC_G729AB_NAME "sm_vmprx_config_codec_g729ab_iPsVrie1_2__"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_G729AB_FN(struct sm_vmprx_codec_g729ab_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_g729ab(struct sm_vmprx_codec_g729ab_parms *codecp);


typedef struct sm_vmprx_codec_g729i_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
	enum kSMG729IMode g729_mode;
} SM_VMPRX_CODEC_G729I_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_g729i sm_vmprx_config_codec_g729i_iPsVrie1_3__
#define SM_VMPRX_CONFIG_CODEC_G729I_NAME "sm_vmprx_config_codec_g729i_iPsVrie1_3__"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_G729I_FN(struct sm_vmprx_codec_g729i_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_g729i(struct sm_vmprx_codec_g729i_parms *codecp);


typedef struct sm_vmprx_codec_gsmefr_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
	enum kSMPLCMode plc_mode;
} SM_VMPRX_CODEC_GSMEFR_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_gsmefr sm_vmprx_config_codec_gsmefr_iPsVrie1_2__
#define SM_VMPRX_CONFIG_CODEC_GSMEFR_NAME "sm_vmprx_config_codec_gsmefr_iPsVrie1_2__"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_GSMEFR_FN(struct sm_vmprx_codec_gsmefr_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_gsmefr(struct sm_vmprx_codec_gsmefr_parms *codecp);


typedef struct sm_vmprx_codec_gsmfr_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
	enum kSMGSMVariant variant;
	enum kSMPLCMode plc_mode;
} SM_VMPRX_CODEC_GSMFR_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_gsmfr sm_vmprx_config_codec_gsmfr_iPsVrie1_2_e1_2__
#define SM_VMPRX_CONFIG_CODEC_GSMFR_NAME "sm_vmprx_config_codec_gsmfr_iPsVrie1_2_e1_2__"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_GSMFR_FN(struct sm_vmprx_codec_gsmfr_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_gsmfr(struct sm_vmprx_codec_gsmfr_parms *codecp);


typedef struct sm_vmprx_codec_gsmhr_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
	enum kSMPLCMode plc_mode;
} SM_VMPRX_CODEC_GSMHR_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_gsmhr sm_vmprx_config_codec_gsmhr_iPsVrie1_2__
#define SM_VMPRX_CONFIG_CODEC_GSMHR_NAME "sm_vmprx_config_codec_gsmhr_iPsVrie1_2__"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_GSMHR_FN(struct sm_vmprx_codec_gsmhr_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_gsmhr(struct sm_vmprx_codec_gsmhr_parms *codecp);


typedef struct sm_vmprx_codec_ilbc_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
	tSM_INT frame_len;
	tSM_INT enhancer;
	enum kSMPLCMode plc_mode;
} SM_VMPRX_CODEC_ILBC_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_ilbc sm_vmprx_config_codec_ilbc_iPsVriiie1_2__
#define SM_VMPRX_CONFIG_CODEC_ILBC_NAME "sm_vmprx_config_codec_ilbc_iPsVriiie1_2__"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_ILBC_FN(struct sm_vmprx_codec_ilbc_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_ilbc(struct sm_vmprx_codec_ilbc_parms *codecp);


typedef struct sm_vmprx_codec_isac_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
} SM_VMPRX_CODEC_ISAC_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_isac sm_vmprx_config_codec_isac_iPsVri_
#define SM_VMPRX_CONFIG_CODEC_ISAC_NAME "sm_vmprx_config_codec_isac_iPsVri_"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_ISAC_FN(struct sm_vmprx_codec_isac_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_isac(struct sm_vmprx_codec_isac_parms *codecp);


typedef struct sm_vmprx_codec_l16_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
	enum kSMPLCMode plc_mode;
} SM_VMPRX_CODEC_L16_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_l16 sm_vmprx_config_codec_l16_iPsVrie1_2__
#define SM_VMPRX_CONFIG_CODEC_L16_NAME "sm_vmprx_config_codec_l16_iPsVrie1_2__"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_L16_FN(struct sm_vmprx_codec_l16_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_l16(struct sm_vmprx_codec_l16_parms *codecp);


typedef struct sm_vmprx_codec_l8_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
	enum kSMPLCMode plc_mode;
} SM_VMPRX_CODEC_L8_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_l8 sm_vmprx_config_codec_l8_iPsVrie1_2__
#define SM_VMPRX_CONFIG_CODEC_L8_NAME "sm_vmprx_config_codec_l8_iPsVrie1_2__"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_L8_FN(struct sm_vmprx_codec_l8_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_l8(struct sm_vmprx_codec_l8_parms *codecp);


typedef struct sm_vmprx_codec_melpe_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
} SM_VMPRX_CODEC_MELPE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_melpe sm_vmprx_config_codec_melpe_iPsVri_
#define SM_VMPRX_CONFIG_CODEC_MELPE_NAME "sm_vmprx_config_codec_melpe_iPsVri_"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_MELPE_FN(struct sm_vmprx_codec_melpe_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_melpe(struct sm_vmprx_codec_melpe_parms *codecp);


typedef struct sm_vmprx_codec_mulaw_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
	enum kSMPLCMode plc_mode;
} SM_VMPRX_CODEC_MULAW_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_mulaw sm_vmprx_config_codec_mulaw_iPsVrie1_2__
#define SM_VMPRX_CONFIG_CODEC_MULAW_NAME "sm_vmprx_config_codec_mulaw_iPsVrie1_2__"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_MULAW_FN(struct sm_vmprx_codec_mulaw_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_mulaw(struct sm_vmprx_codec_mulaw_parms *codecp);


typedef struct sm_vmprx_codec_opus_parms {
	tSMVMPrxId vmprx;
	tSM_INT decode_fec;
	tSM_INT payload_type;
} SM_VMPRX_CODEC_OPUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_opus sm_vmprx_config_codec_opus_iPsVrii_
#define SM_VMPRX_CONFIG_CODEC_OPUS_NAME "sm_vmprx_config_codec_opus_iPsVrii_"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_OPUS_FN(struct sm_vmprx_codec_opus_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_opus(struct sm_vmprx_codec_opus_parms *codecp);


typedef struct sm_vmprx_codec_rfc2833_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
	enum kSMPLCMode plc_mode;
} SM_VMPRX_CODEC_RFC2833_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_rfc2833 sm_vmprx_config_codec_rfc2833_iPsVrie1_2__
#define SM_VMPRX_CONFIG_CODEC_RFC2833_NAME "sm_vmprx_config_codec_rfc2833_iPsVrie1_2__"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_RFC2833_FN(struct sm_vmprx_codec_rfc2833_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_rfc2833(struct sm_vmprx_codec_rfc2833_parms *codecp);


typedef struct sm_vmprx_codec_rfc4040_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
} SM_VMPRX_CODEC_RFC4040_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_rfc4040 sm_vmprx_config_codec_rfc4040_iPsVri_
#define SM_VMPRX_CONFIG_CODEC_RFC4040_NAME "sm_vmprx_config_codec_rfc4040_iPsVri_"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_RFC4040_FN(struct sm_vmprx_codec_rfc4040_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_rfc4040(struct sm_vmprx_codec_rfc4040_parms *codecp);


typedef struct sm_vmprx_codec_rttext_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
	tSM_INT redundant_payload_type;
	tSM_INT out_of_order_delay;
} SM_VMPRX_CODEC_RTTEXT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_rttext sm_vmprx_config_codec_rttext_iPsVriii_
#define SM_VMPRX_CONFIG_CODEC_RTTEXT_NAME "sm_vmprx_config_codec_rttext_iPsVriii_"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_RTTEXT_FN(struct sm_vmprx_codec_rttext_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_rttext(struct sm_vmprx_codec_rttext_parms *codecp);


typedef struct sm_vmprx_codec_silk_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
	enum kSMPLCMode plc_mode;
} SM_VMPRX_CODEC_SILK_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_silk sm_vmprx_config_codec_silk_iPsVrie1_2__
#define SM_VMPRX_CONFIG_CODEC_SILK_NAME "sm_vmprx_config_codec_silk_iPsVrie1_2__"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_SILK_FN(struct sm_vmprx_codec_silk_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_silk(struct sm_vmprx_codec_silk_parms *codecp);


typedef struct sm_vmprx_codec_smv_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
	enum kSMPLCMode plc_mode;
} SM_VMPRX_CODEC_SMV_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_smv sm_vmprx_config_codec_smv_iPsVrie1_2__
#define SM_VMPRX_CONFIG_CODEC_SMV_NAME "sm_vmprx_config_codec_smv_iPsVrie1_2__"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_SMV_FN(struct sm_vmprx_codec_smv_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_smv(struct sm_vmprx_codec_smv_parms *codecp);


typedef struct sm_vmprx_codec_speex_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
	tSM_UT32 nb_channels;
	tSM_UT32 sampling_rate;
	enum kSMSpeexMode bandwidth;
	enum kSMPLCMode plc_mode;
} SM_VMPRX_CODEC_SPEEX_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_speex sm_vmprx_config_codec_speex_iPsVriUiUie1_3_e1_2__
#define SM_VMPRX_CONFIG_CODEC_SPEEX_NAME "sm_vmprx_config_codec_speex_iPsVriUiUie1_3_e1_2__"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_SPEEX_FN(struct sm_vmprx_codec_speex_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_speex(struct sm_vmprx_codec_speex_parms *codecp);


typedef struct sm_vmprx_codec_speex_mode_parms {
	tSMVMPrxId vmprx;
	tSM_UT32 enh;
} SM_VMPRX_CODEC_SPEEX_MODE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_speex_mode sm_vmprx_config_codec_speex_mode_iPsVrUi_
#define SM_VMPRX_CONFIG_CODEC_SPEEX_MODE_NAME "sm_vmprx_config_codec_speex_mode_iPsVrUi_"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_SPEEX_MODE_FN(struct sm_vmprx_codec_speex_mode_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_speex_mode(struct sm_vmprx_codec_speex_mode_parms *codecp);


typedef struct sm_vmprx_codec_sse_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
} SM_VMPRX_CODEC_SSE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_sse sm_vmprx_config_codec_sse_iPsVri_
#define SM_VMPRX_CONFIG_CODEC_SSE_NAME "sm_vmprx_config_codec_sse_iPsVri_"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_SSE_FN(struct sm_vmprx_codec_sse_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_sse(struct sm_vmprx_codec_sse_parms *codecp);


typedef struct sm_vmprx_codec_tetra_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
} SM_VMPRX_CODEC_TETRA_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec_tetra sm_vmprx_config_codec_tetra_iPsVri_
#define SM_VMPRX_CONFIG_CODEC_TETRA_NAME "sm_vmprx_config_codec_tetra_iPsVri_"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_TETRA_FN(struct sm_vmprx_codec_tetra_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec_tetra(struct sm_vmprx_codec_tetra_parms *codecp);


typedef struct sm_vmprx_dataloss_parms {
	tSMVMPrxId vmprx;
	tSM_INT loss_threshold;
} SM_VMPRX_DATALOSS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_dataloss sm_vmprx_config_dataloss_iPsVri_
#define SM_VMPRX_CONFIG_DATALOSS_NAME "sm_vmprx_config_dataloss_iPsVri_"
#endif
typedef int SM_VMPRX_CONFIG_DATALOSS_FN(struct sm_vmprx_dataloss_parms *datalossp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_dataloss(struct sm_vmprx_dataloss_parms *datalossp);


typedef struct sm_vmprx_config_encryption_aes_cm_parms {
	tSMVMPrxId vmprx;
	tSM_INT keylen;
	char *key;
} SM_VMPRX_CONFIG_ENCRYPTION_AES_CM_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_encryption_aes_cm sm_vmprx_config_encryption_aes_cm_iPsVriPc_
#define SM_VMPRX_CONFIG_ENCRYPTION_AES_CM_NAME "sm_vmprx_config_encryption_aes_cm_iPsVriPc_"
#endif
typedef int SM_VMPRX_CONFIG_ENCRYPTION_AES_CM_FN(struct sm_vmprx_config_encryption_aes_cm_parms *pp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_encryption_aes_cm(struct sm_vmprx_config_encryption_aes_cm_parms *pp);


typedef struct sm_vmprx_config_encryption_aes_f8_parms {
	tSMVMPrxId vmprx;
	tSM_INT keylen;
	char *key;
} SM_VMPRX_CONFIG_ENCRYPTION_AES_F8_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_encryption_aes_f8 sm_vmprx_config_encryption_aes_f8_iPsVriPc_
#define SM_VMPRX_CONFIG_ENCRYPTION_AES_F8_NAME "sm_vmprx_config_encryption_aes_f8_iPsVriPc_"
#endif
typedef int SM_VMPRX_CONFIG_ENCRYPTION_AES_F8_FN(struct sm_vmprx_config_encryption_aes_f8_parms *pp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_encryption_aes_f8(struct sm_vmprx_config_encryption_aes_f8_parms *pp);


typedef struct sm_vmprx_config_encryption_null_parms {
	tSMVMPrxId vmprx;
} SM_VMPRX_CONFIG_ENCRYPTION_NULL_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_encryption_null sm_vmprx_config_encryption_null_iPsVr_
#define SM_VMPRX_CONFIG_ENCRYPTION_NULL_NAME "sm_vmprx_config_encryption_null_iPsVr_"
#endif
typedef int SM_VMPRX_CONFIG_ENCRYPTION_NULL_FN(struct sm_vmprx_config_encryption_null_parms *pp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_encryption_null(struct sm_vmprx_config_encryption_null_parms *pp);


typedef struct sm_vmprx_config_forwarding_parms {
	tSMVMPrxId vmprx;
	enum kVMPrxFwdMode mode;
	enum kSMVMPrxFwdDestType dest_type;
	union {
		struct {
			SOCKADDR_IN destination;
			SOCKADDR_IN source;
			int TOS;
		} ipv4;
		struct {
			SOCKADDR_IN6 destination;
			SOCKADDR_IN6 source;
		} ipv6;
	} u;
	int num_types;
	struct sm_vmprx_forward_types {
		int incoming_pt;
		int outgoing_pt;
		enum kVMPrxFwdTranscoding transcoding;
	} types[kVMPrxFwdMaxTypes];
} SM_VMPRX_CONFIG_FORWARDING_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_forwarding sm_vmprx_config_forwarding_iPsVre1_3_e1_2_usSAdSAdi_sSA6dSA6d__isiie1_3__r8_
#define SM_VMPRX_CONFIG_FORWARDING_NAME "sm_vmprx_config_forwarding_iPsVre1_3_e1_2_usSAdSAdi_sSA6dSA6d__isiie1_3__r8_"
#endif
typedef int SM_VMPRX_CONFIG_FORWARDING_FN(struct sm_vmprx_config_forwarding_parms *configp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_forwarding(struct sm_vmprx_config_forwarding_parms *configp);


typedef struct sm_vmprx_jitter_parms {
	tSMVMPrxId vmprx;
	tSM_UT32 max_delay_ms;
	tSM_UT32 initial_delay_ms;
} SM_VMPRX_JITTER_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_jitter sm_vmprx_config_jitter_iPsVrUiUi_
#define SM_VMPRX_CONFIG_JITTER_NAME "sm_vmprx_config_jitter_iPsVrUiUi_"
#endif
typedef int SM_VMPRX_CONFIG_JITTER_FN(struct sm_vmprx_jitter_parms *jitterp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_jitter(struct sm_vmprx_jitter_parms *jitterp);


typedef struct sm_vmprx_config_jitter_mode_parms {
	tSMVMPrxId vmprx;
	enum kSMVMPrxJitterBufferMode mode;
	union {
		struct {
			tSM_UT32 target_delay;
			float freq_upper_tolerance;
			float freq_lower_tolerance;
		} adaptive;
	} u;
} SM_VMPRX_CONFIG_JITTER_MODE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_jitter_mode sm_vmprx_config_jitter_mode_iPsVre1_2_usUiff___
#define SM_VMPRX_CONFIG_JITTER_MODE_NAME "sm_vmprx_config_jitter_mode_iPsVre1_2_usUiff___"
#endif
typedef int SM_VMPRX_CONFIG_JITTER_MODE_FN(struct sm_vmprx_config_jitter_mode_parms *jitterp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_jitter_mode(struct sm_vmprx_config_jitter_mode_parms *jitterp);


typedef struct sm_vmprx_config_jitter_resync_notify_parms {
	tSMVMPrxId vmprx;
	tSM_INT enable;
} SM_VMPRX_CONFIG_JITTER_RESYNC_NOTIFY_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_jitter_resync_notify sm_vmprx_config_jitter_resync_notify_iPsVri_
#define SM_VMPRX_CONFIG_JITTER_RESYNC_NOTIFY_NAME "sm_vmprx_config_jitter_resync_notify_iPsVri_"
#endif
typedef int SM_VMPRX_CONFIG_JITTER_RESYNC_NOTIFY_FN(struct sm_vmprx_config_jitter_resync_notify_parms *notifyp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_jitter_resync_notify(struct sm_vmprx_config_jitter_resync_notify_parms *notifyp);


typedef struct sm_vmprx_config_profile_specific_parms {
	tSMVMPrxId vmprx;
	tSM_INT enable;
	char *plugin;
	tSM_INT paramlen;
	char *paramval;
} SM_VMPRX_CONFIG_PROFILE_SPECIFIC_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_profile_specific sm_vmprx_config_profile_specific_iPsVriPciPc_
#define SM_VMPRX_CONFIG_PROFILE_SPECIFIC_NAME "sm_vmprx_config_profile_specific_iPsVriPciPc_"
#endif
typedef int SM_VMPRX_CONFIG_PROFILE_SPECIFIC_FN(struct sm_vmprx_config_profile_specific_parms *psp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_profile_specific(struct sm_vmprx_config_profile_specific_parms *psp);


typedef struct sm_vmprx_sample_rate_parms {
	tSMVMPrxId vmprx;
	tSM_UT32 sample_rate;
} SM_VMPRX_SAMPLE_RATE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_sample_rate sm_vmprx_config_sample_rate_iPsVrUi_
#define SM_VMPRX_CONFIG_SAMPLE_RATE_NAME "sm_vmprx_config_sample_rate_iPsVrUi_"
#endif
typedef int SM_VMPRX_CONFIG_SAMPLE_RATE_FN(struct sm_vmprx_sample_rate_parms *sample_ratep);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_sample_rate(struct sm_vmprx_sample_rate_parms *sample_ratep);


typedef struct sm_vmprx_config_sprt_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
	tSMSPRTId sprt;
} SM_VMPRX_CONFIG_SPRT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_sprt sm_vmprx_config_sprt_iPsVriSPRTt_
#define SM_VMPRX_CONFIG_SPRT_NAME "sm_vmprx_config_sprt_iPsVriSPRTt_"
#endif
typedef int SM_VMPRX_CONFIG_SPRT_FN(struct sm_vmprx_config_sprt_parms *configp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_sprt(struct sm_vmprx_config_sprt_parms *configp);


typedef struct sm_vmprx_tone_parms {
	tSMVMPrxId vmprx;
	tSM_INT regen_tones;
	tSM_INT detect_tones;
	tSM_INT enforce_tone_spacing;
} SM_VMPRX_TONE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_tones sm_vmprx_config_tones_iPsVriii_
#define SM_VMPRX_CONFIG_TONES_NAME "sm_vmprx_config_tones_iPsVriii_"
#endif
typedef int SM_VMPRX_CONFIG_TONES_FN(struct sm_vmprx_tone_parms *tonep);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_tones(struct sm_vmprx_tone_parms *tonep);


typedef struct sm_vmprx_unhandled_payload_reporting_parms {
	tSMVMPrxId vmprx;
	tSM_INT delay;
} SM_VMPRX_UNHANDLED_PAYLOAD_REPORTING_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_unhandled_payload_reporting sm_vmprx_config_unhandled_payload_reporting_iPsVri_
#define SM_VMPRX_CONFIG_UNHANDLED_PAYLOAD_REPORTING_NAME "sm_vmprx_config_unhandled_payload_reporting_iPsVri_"
#endif
typedef int SM_VMPRX_CONFIG_UNHANDLED_PAYLOAD_REPORTING_FN(struct sm_vmprx_unhandled_payload_reporting_parms *pp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_unhandled_payload_reporting(struct sm_vmprx_unhandled_payload_reporting_parms *pp);


typedef struct sm_vmprx_create_parms {
	tSMVMPrxId vmprx;
	tSMModuleId module;
	enum kSMVMPrxType type;
	struct in_addr address;
	struct in6_addr ipv6_address;
	tSM_INT mux_rtcp;
} SM_VMPRX_CREATE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_create sm_vmprx_create_iPsVrMte1_2_SIdSI6di_
#define SM_VMPRX_CREATE_NAME "sm_vmprx_create_iPsVrMte1_2_SIdSI6di_"
#endif
typedef int SM_VMPRX_CREATE_FN(struct sm_vmprx_create_parms *vmprxp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_create(struct sm_vmprx_create_parms *vmprxp);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_destroy sm_vmprx_destroy_iVr
#define SM_VMPRX_DESTROY_NAME "sm_vmprx_destroy_iVr"
#endif
typedef int SM_VMPRX_DESTROY_FN(tSMVMPrxId vmprx);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_destroy(tSMVMPrxId vmprx);


typedef struct sm_vmprx_datafeed_parms {
	tSMVMPrxId vmprx;
	tSMDatafeedId datafeed;
} SM_VMPRX_DATAFEED_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_get_datafeed sm_vmprx_get_datafeed_iPsVrDt_
#define SM_VMPRX_GET_DATAFEED_NAME "sm_vmprx_get_datafeed_iPsVrDt_"
#endif
typedef int SM_VMPRX_GET_DATAFEED_FN(struct sm_vmprx_datafeed_parms *datafeedp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_get_datafeed(struct sm_vmprx_datafeed_parms *datafeedp);


typedef struct sm_vmprx_event_parms {
	tSMVMPrxId vmprx;
	tSMEventId event;
} SM_VMPRX_EVENT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_get_event sm_vmprx_get_event_iPsVrEt_
#define SM_VMPRX_GET_EVENT_NAME "sm_vmprx_get_event_iPsVrEt_"
#endif
typedef int SM_VMPRX_GET_EVENT_FN(struct sm_vmprx_event_parms *eventp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_get_event(struct sm_vmprx_event_parms *eventp);


typedef struct sm_vmprx_port_parms {
	tSMVMPrxId vmprx;
	int RTP_port;
	int RTCP_port;
	struct in_addr address;
	struct in6_addr ipv6_address;
	tSM_UT32 nowait;
} SM_VMPRX_PORT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_get_ports sm_vmprx_get_ports_iPsVriiSIdSI6dUi_
#define SM_VMPRX_GET_PORTS_NAME "sm_vmprx_get_ports_iPsVriiSIdSI6dUi_"
#endif
typedef int SM_VMPRX_GET_PORTS_FN(struct sm_vmprx_port_parms *portp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_get_ports(struct sm_vmprx_port_parms *portp);


typedef struct sm_vmprx_ice_stun_change_role_parms {
	tSMVMPrxId vmprx;
	enum kSMICERole local_role;
} SM_VMPRX_ICE_STUN_CHANGE_ROLE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_ice_stun_change_role sm_vmprx_ice_stun_change_role_iPsVre1_4__
#define SM_VMPRX_ICE_STUN_CHANGE_ROLE_NAME "sm_vmprx_ice_stun_change_role_iPsVre1_4__"
#endif
typedef int SM_VMPRX_ICE_STUN_CHANGE_ROLE_FN(struct sm_vmprx_ice_stun_change_role_parms *rolep);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_ice_stun_change_role(struct sm_vmprx_ice_stun_change_role_parms *rolep);


typedef struct sm_vmprx_ice_stun_config_parms {
	tSMVMPrxId vmprx;
	enum kSMICERole local_role;
	char *local_ice_ufrag;
	tSM_UT32 local_ice_ufrag_length;
	char *local_ice_pwd;
	tSM_UT32 local_ice_pwd_length;
	tSM_UT64 role_tiebreaker;
} SM_VMPRX_ICE_STUN_CONFIG_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_ice_stun_config sm_vmprx_ice_stun_config_iPsVre1_4_PcUiPcUiU64i_
#define SM_VMPRX_ICE_STUN_CONFIG_NAME "sm_vmprx_ice_stun_config_iPsVre1_4_PcUiPcUiU64i_"
#endif
typedef int SM_VMPRX_ICE_STUN_CONFIG_FN(struct sm_vmprx_ice_stun_config_parms *configp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_ice_stun_config(struct sm_vmprx_ice_stun_config_parms *configp);


typedef struct sm_vmprx_ice_stun_remote_credentials_parms {
	tSMVMPrxId vmprx;
	char *remote_ice_ufrag;
	tSM_UT32 remote_ice_ufrag_length;
	char *remote_ice_pwd;
	tSM_UT32 remote_ice_pwd_length;
} SM_VMPRX_ICE_STUN_REMOTE_CREDENTIALS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_ice_stun_remote_credentials sm_vmprx_ice_stun_remote_credentials_iPsVrPcUiPcUi_
#define SM_VMPRX_ICE_STUN_REMOTE_CREDENTIALS_NAME "sm_vmprx_ice_stun_remote_credentials_iPsVrPcUiPcUi_"
#endif
typedef int SM_VMPRX_ICE_STUN_REMOTE_CREDENTIALS_FN(struct sm_vmprx_ice_stun_remote_credentials_parms *credentialsp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_ice_stun_remote_credentials(struct sm_vmprx_ice_stun_remote_credentials_parms *credentialsp);


typedef struct sm_vmprx_ice_stun_send_bind_request_parms {
	tSMVMPrxId vmprx;
	int use_RTCP_port;
	char transaction_id[12];
	union {
		SOCKADDR_IN ipv4;
		SOCKADDR_IN6 ipv6;
	} destination;
	tSM_UT32 priority;
	tSM_INT use_candidate;
} SM_VMPRX_ICE_STUN_SEND_BIND_REQUEST_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_ice_stun_send_bind_request sm_vmprx_ice_stun_send_bind_request_iPsVricr12uSAdSA6d_Uii_
#define SM_VMPRX_ICE_STUN_SEND_BIND_REQUEST_NAME "sm_vmprx_ice_stun_send_bind_request_iPsVricr12uSAdSA6d_Uii_"
#endif
typedef int SM_VMPRX_ICE_STUN_SEND_BIND_REQUEST_FN(struct sm_vmprx_ice_stun_send_bind_request_parms *requestp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_ice_stun_send_bind_request(struct sm_vmprx_ice_stun_send_bind_request_parms *requestp);


typedef struct sm_vmprx_set_rtcphand_parms {
	tSMVMPrxId vmprx;
	tSMRTCPHandId rtcphand;
} SM_VMPRX_SET_RTCPHAND_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_set_rtcphand sm_vmprx_set_rtcphand_iPsVrRs_
#define SM_VMPRX_SET_RTCPHAND_NAME "sm_vmprx_set_rtcphand_iPsVrRs_"
#endif
typedef int SM_VMPRX_SET_RTCPHAND_FN(struct sm_vmprx_set_rtcphand_parms *rvp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_set_rtcphand(struct sm_vmprx_set_rtcphand_parms *rvp);


typedef struct sm_vmprx_status_parms {
	tSMVMPrxId vmprx;
	enum kSMVMPrxStatus status;
	union {
		struct {
			tSM_INT id;
			double volume;
			unsigned duration;
		} tone;
		struct {
			int RTP_Port;
			int RTCP_Port;
			struct in_addr address;
		} ports;
		struct {
			tSM_INT no_data;
		} run;
		struct {
			struct in_addr address;
			int port;
			int ssrc;
		} ssrc;
		struct {
			int type;
		} payload;
		struct {
			int payload_type;
		} codec_specific;
		struct {
			int RTP_Port;
			int RTCP_Port;
			struct in6_addr address;
		} ports_ipv6;
		struct {
			struct in6_addr address;
			int port;
			int ssrc;
		} ssrc_ipv6;
		struct {
			int recv_port;
			enum kSMAddressType address_type;
			union {
				struct in_addr ipv4;
				struct in6_addr ipv6;
			} address;
			int port;
			tSM_UT32 priority;
			int use_candidate;
			int remote_is_controlling;
		} bind_request;
		struct {
			int recv_port;
			char transaction_id[12];
			enum kSMVMPrxSTUNResult result;
			tSM_INT error_code;
			enum kSMAddressType address_type;
			union {
				struct in_addr ipv4;
				struct in6_addr ipv6;
			} address;
			int port;
			int local_was_controlling;
		} bind_response;
		struct {
			tSM_UT32 timestamp;
			tSM_UT64 localtimeref;
		} jbresync;
		struct {
			tSM_INT notification_code;
		} profile_specific;
	} u;
} SM_VMPRX_STATUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_status sm_vmprx_status_iPsVre1_14_usidUi_siiSId_si_sSIdii_si_si_siiSI6d_sSI6dii_sie1_2_uSIdSI6d_iUiii_sicr12e1_4_ie1_2_uSIdSI6d_ii_sUiU64i_si___
#define SM_VMPRX_STATUS_NAME "sm_vmprx_status_iPsVre1_14_usidUi_siiSId_si_sSIdii_si_si_siiSI6d_sSI6dii_sie1_2_uSIdSI6d_iUiii_sicr12e1_4_ie1_2_uSIdSI6d_ii_sUiU64i_si___"
#endif
typedef int SM_VMPRX_STATUS_FN(struct sm_vmprx_status_parms *statusp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_status(struct sm_vmprx_status_parms *statusp);


typedef struct sm_vmprx_status_codec_amrnb_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
	enum kSMVMPrxAMRNBStatus status;
	union {
		struct {
			tSM_INT bitrate;
		} cmr;
	} u;
} SM_VMPRX_STATUS_CODEC_AMRNB_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_status_codec_amrnb sm_vmprx_status_codec_amrnb_iPsVrie1_2_usi___
#define SM_VMPRX_STATUS_CODEC_AMRNB_NAME "sm_vmprx_status_codec_amrnb_iPsVrie1_2_usi___"
#endif
typedef int SM_VMPRX_STATUS_CODEC_AMRNB_FN(struct sm_vmprx_status_codec_amrnb_parms *amrnbp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_status_codec_amrnb(struct sm_vmprx_status_codec_amrnb_parms *amrnbp);


typedef struct sm_vmprx_status_codec_amrwb_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
	enum kSMVMPrxAMRWBStatus status;
	union {
		struct {
			tSM_INT bitrate;
		} cmr;
	} u;
} SM_VMPRX_STATUS_CODEC_AMRWB_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_status_codec_amrwb sm_vmprx_status_codec_amrwb_iPsVrie1_2_usi___
#define SM_VMPRX_STATUS_CODEC_AMRWB_NAME "sm_vmprx_status_codec_amrwb_iPsVrie1_2_usi___"
#endif
typedef int SM_VMPRX_STATUS_CODEC_AMRWB_FN(struct sm_vmprx_status_codec_amrwb_parms *amrwbp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_status_codec_amrwb(struct sm_vmprx_status_codec_amrwb_parms *amrwbp);


typedef struct sm_vmprx_status_codec_sse_parms {
	tSMVMPrxId vmprx;
	tSM_INT payload_type;
	enum kSMVMPrxSSEStatus status;
	char *payload;
	tSM_INT max_length;
	tSM_INT length;
} SM_VMPRX_STATUS_CODEC_SSE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_status_codec_sse sm_vmprx_status_codec_sse_iPsVrie1_2_Pcii_
#define SM_VMPRX_STATUS_CODEC_SSE_NAME "sm_vmprx_status_codec_sse_iPsVrie1_2_Pcii_"
#endif
typedef int SM_VMPRX_STATUS_CODEC_SSE_FN(struct sm_vmprx_status_codec_sse_parms *ssep);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_status_codec_sse(struct sm_vmprx_status_codec_sse_parms *ssep);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_status_discard_codec_specific sm_vmprx_status_discard_codec_specific_iVr
#define SM_VMPRX_STATUS_DISCARD_CODEC_SPECIFIC_NAME "sm_vmprx_status_discard_codec_specific_iVr"
#endif
typedef int SM_VMPRX_STATUS_DISCARD_CODEC_SPECIFIC_FN(tSMVMPrxId vmprx);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_status_discard_codec_specific(tSMVMPrxId vmprx);


typedef struct sm_vmprx_status_profile_specific_parms {
	tSMVMPrxId vmprx;
	tSM_INT notification_code;
	char *payload;
	tSM_INT max_length;
	tSM_INT length;
} SM_VMPRX_STATUS_PROFILE_SPECIFIC_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_status_profile_specific sm_vmprx_status_profile_specific_iPsVriPcii_
#define SM_VMPRX_STATUS_PROFILE_SPECIFIC_NAME "sm_vmprx_status_profile_specific_iPsVriPcii_"
#endif
typedef int SM_VMPRX_STATUS_PROFILE_SPECIFIC_FN(struct sm_vmprx_status_profile_specific_parms *psp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_status_profile_specific(struct sm_vmprx_status_profile_specific_parms *psp);


typedef struct sm_vmprx_stop_parms {
	tSMVMPrxId vmprx;
} SM_VMPRX_STOP_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_stop sm_vmprx_stop_iPsVr_
#define SM_VMPRX_STOP_NAME "sm_vmprx_stop_iPsVr_"
#endif
typedef int SM_VMPRX_STOP_FN(struct sm_vmprx_stop_parms *stopp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_stop(struct sm_vmprx_stop_parms *stopp);


typedef struct sm_vmptx_config_parms {
	tSMVMPtxId vmptx;
	SOCKADDR_IN destination_rtp;
	SOCKADDR_IN source_rtp;
	int TOS_RTP;
	SOCKADDR_IN destination_rtcp;
	SOCKADDR_IN source_rtcp;
	int TOS_RTCP;
} SM_VMPTX_CONFIG_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config sm_vmptx_config_iPsVtSAdSAdiSAdSAdi_
#define SM_VMPTX_CONFIG_NAME "sm_vmptx_config_iPsVtSAdSAdiSAdSAdi_"
#endif
typedef int SM_VMPTX_CONFIG_FN(struct sm_vmptx_config_parms *configp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config(struct sm_vmptx_config_parms *configp);


typedef struct sm_vmptx_config_authentication_hmac_sha1_parms {
	tSMVMPtxId vmptx;
	tSM_INT keylen;
	char *key;
	tSM_INT taglen;
} SM_VMPTX_CONFIG_AUTHENTICATION_HMAC_SHA1_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_authentication_hmac_sha1 sm_vmptx_config_authentication_hmac_sha1_iPsVtiPci_
#define SM_VMPTX_CONFIG_AUTHENTICATION_HMAC_SHA1_NAME "sm_vmptx_config_authentication_hmac_sha1_iPsVtiPci_"
#endif
typedef int SM_VMPTX_CONFIG_AUTHENTICATION_HMAC_SHA1_FN(struct sm_vmptx_config_authentication_hmac_sha1_parms *pp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_authentication_hmac_sha1(struct sm_vmptx_config_authentication_hmac_sha1_parms *pp);


typedef struct sm_vmptx_config_authentication_null_parms {
	tSMVMPtxId vmptx;
} SM_VMPTX_CONFIG_AUTHENTICATION_NULL_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_authentication_null sm_vmptx_config_authentication_null_iPsVt_
#define SM_VMPTX_CONFIG_AUTHENTICATION_NULL_NAME "sm_vmptx_config_authentication_null_iPsVt_"
#endif
typedef int SM_VMPTX_CONFIG_AUTHENTICATION_NULL_FN(struct sm_vmptx_config_authentication_null_parms *pp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_authentication_null(struct sm_vmptx_config_authentication_null_parms *pp);


typedef struct sm_vmptx_codec_alaw_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
	enum kSMVMPTxVADMode VADMode;
	tSM_INT ptime;
} SM_VMPTX_CODEC_ALAW_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_alaw sm_vmptx_config_codec_alaw_iPsVtie1_3_i_
#define SM_VMPTX_CONFIG_CODEC_ALAW_NAME "sm_vmptx_config_codec_alaw_iPsVtie1_3_i_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_ALAW_FN(struct sm_vmptx_codec_alaw_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_alaw(struct sm_vmptx_codec_alaw_parms *codecp);


typedef struct sm_vmptx_codec_amrnb_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
	tSM_INT aligned;
	tSM_INT gsmefr;
	enum kSMVMPTxVADMode VADMode;
	tSM_INT frames_per_packet;
} SM_VMPTX_CODEC_AMRNB_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_amrnb sm_vmptx_config_codec_amrnb_iPsVtiiie1_3_i_
#define SM_VMPTX_CONFIG_CODEC_AMRNB_NAME "sm_vmptx_config_codec_amrnb_iPsVtiiie1_3_i_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_AMRNB_FN(struct sm_vmptx_codec_amrnb_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_amrnb(struct sm_vmptx_codec_amrnb_parms *codecp);


typedef struct sm_vmptx_config_codec_amrnb_cmr_parms {
	tSMVMPtxId vmptx;
	tSM_INT bitrate;
} SM_VMPTX_CONFIG_CODEC_AMRNB_CMR_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_amrnb_cmr sm_vmptx_config_codec_amrnb_cmr_iPsVti_
#define SM_VMPTX_CONFIG_CODEC_AMRNB_CMR_NAME "sm_vmptx_config_codec_amrnb_cmr_iPsVti_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_AMRNB_CMR_FN(struct sm_vmptx_config_codec_amrnb_cmr_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_amrnb_cmr(struct sm_vmptx_config_codec_amrnb_cmr_parms *codecp);


typedef struct sm_vmptx_config_codec_amrnb_mode_parms {
	tSMVMPtxId vmptx;
	tSM_INT bitrate;
} SM_VMPTX_CONFIG_CODEC_AMRNB_MODE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_amrnb_mode sm_vmptx_config_codec_amrnb_mode_iPsVti_
#define SM_VMPTX_CONFIG_CODEC_AMRNB_MODE_NAME "sm_vmptx_config_codec_amrnb_mode_iPsVti_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_AMRNB_MODE_FN(struct sm_vmptx_config_codec_amrnb_mode_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_amrnb_mode(struct sm_vmptx_config_codec_amrnb_mode_parms *codecp);


typedef struct sm_vmptx_codec_amrwb_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
	tSM_INT aligned;
	tSM_INT VADMode;
	tSM_INT frames_per_packet;
} SM_VMPTX_CODEC_AMRWB_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_amrwb sm_vmptx_config_codec_amrwb_iPsVtiiii_
#define SM_VMPTX_CONFIG_CODEC_AMRWB_NAME "sm_vmptx_config_codec_amrwb_iPsVtiiii_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_AMRWB_FN(struct sm_vmptx_codec_amrwb_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_amrwb(struct sm_vmptx_codec_amrwb_parms *codecp);


typedef struct sm_vmptx_config_codec_amrwb_cmr_parms {
	tSMVMPtxId vmptx;
	tSM_INT bitrate;
} SM_VMPTX_CONFIG_CODEC_AMRWB_CMR_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_amrwb_cmr sm_vmptx_config_codec_amrwb_cmr_iPsVti_
#define SM_VMPTX_CONFIG_CODEC_AMRWB_CMR_NAME "sm_vmptx_config_codec_amrwb_cmr_iPsVti_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_AMRWB_CMR_FN(struct sm_vmptx_config_codec_amrwb_cmr_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_amrwb_cmr(struct sm_vmptx_config_codec_amrwb_cmr_parms *codecp);


typedef struct sm_vmptx_config_codec_amrwb_mode_parms {
	tSMVMPtxId vmptx;
	tSM_INT bitrate;
} SM_VMPTX_CONFIG_CODEC_AMRWB_MODE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_amrwb_mode sm_vmptx_config_codec_amrwb_mode_iPsVti_
#define SM_VMPTX_CONFIG_CODEC_AMRWB_MODE_NAME "sm_vmptx_config_codec_amrwb_mode_iPsVti_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_AMRWB_MODE_FN(struct sm_vmptx_config_codec_amrwb_mode_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_amrwb_mode(struct sm_vmptx_config_codec_amrwb_mode_parms *codecp);


typedef struct sm_vmptx_codec_comfort_noise_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
} SM_VMPTX_CODEC_COMFORT_NOISE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_comfort_noise sm_vmptx_config_codec_comfort_noise_iPsVti_
#define SM_VMPTX_CONFIG_CODEC_COMFORT_NOISE_NAME "sm_vmptx_config_codec_comfort_noise_iPsVti_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_COMFORT_NOISE_FN(struct sm_vmptx_codec_comfort_noise_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_comfort_noise(struct sm_vmptx_codec_comfort_noise_parms *codecp);


typedef struct sm_vmptx_codec_evrc_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
	tSM_INT high_pass_filter;
	tSM_INT noise_suppression_filter;
	enum kSMEVRCRTPMode rtp_mode;
	enum kSMEVRCRate max_rate;
	enum kSMEVRCRate min_rate;
	enum kSMVMPTxVADMode VADMode;
	tSM_INT frames_per_packet;
} SM_VMPTX_CODEC_EVRC_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_evrc sm_vmptx_config_codec_evrc_iPsVtiiie1_3_e1_3_e1_3_e1_3_i_
#define SM_VMPTX_CONFIG_CODEC_EVRC_NAME "sm_vmptx_config_codec_evrc_iPsVtiiie1_3_e1_3_e1_3_e1_3_i_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_EVRC_FN(struct sm_vmptx_codec_evrc_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_evrc(struct sm_vmptx_codec_evrc_parms *codecp);


typedef struct sm_vmptx_codec_g722_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
	tSM_INT packetlen;
} SM_VMPTX_CODEC_G722_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_g722 sm_vmptx_config_codec_g722_iPsVtii_
#define SM_VMPTX_CONFIG_CODEC_G722_NAME "sm_vmptx_config_codec_g722_iPsVtii_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_G722_FN(struct sm_vmptx_codec_g722_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_g722(struct sm_vmptx_codec_g722_parms *codecp);


typedef struct sm_vmptx_codec_g722_1_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
	tSM_INT bitrate;
	tSM_INT frames_per_packet;
} SM_VMPTX_CODEC_G722_1_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_g722_1 sm_vmptx_config_codec_g722_1_iPsVtiii_
#define SM_VMPTX_CONFIG_CODEC_G722_1_NAME "sm_vmptx_config_codec_g722_1_iPsVtiii_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_G722_1_FN(struct sm_vmptx_codec_g722_1_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_g722_1(struct sm_vmptx_codec_g722_1_parms *codecp);


typedef struct sm_vmptx_codec_g723_1_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
	tSM_INT high_pass_filter;
	tSM_INT rate;
	tSM_INT silence_compression;
	tSM_INT frames_per_packet;
} SM_VMPTX_CODEC_G723_1_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_g723_1 sm_vmptx_config_codec_g723_1_iPsVtiiiii_
#define SM_VMPTX_CONFIG_CODEC_G723_1_NAME "sm_vmptx_config_codec_g723_1_iPsVtiiiii_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_G723_1_FN(struct sm_vmptx_codec_g723_1_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_g723_1(struct sm_vmptx_codec_g723_1_parms *codecp);


typedef struct sm_vmptx_codec_g726_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
	tSM_INT bits;
	enum kSMVMPTxVADMode VADMode;
	tSM_INT packetlen;
	enum kSMG726Variant variant;
} SM_VMPTX_CODEC_G726_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_g726 sm_vmptx_config_codec_g726_iPsVtiie1_3_ie1_2__
#define SM_VMPTX_CONFIG_CODEC_G726_NAME "sm_vmptx_config_codec_g726_iPsVtiie1_3_ie1_2__"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_G726_FN(struct sm_vmptx_codec_g726_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_g726(struct sm_vmptx_codec_g726_parms *codecp);


typedef struct sm_vmptx_codec_g728_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
	tSM_INT rate;
	enum kSMVMPTxVADMode VADMode;
	tSM_INT ptime;
} SM_VMPTX_CODEC_G728_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_g728 sm_vmptx_config_codec_g728_iPsVtiie1_3_i_
#define SM_VMPTX_CONFIG_CODEC_G728_NAME "sm_vmptx_config_codec_g728_iPsVtiie1_3_i_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_G728_FN(struct sm_vmptx_codec_g728_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_g728(struct sm_vmptx_codec_g728_parms *codecp);


typedef struct sm_vmptx_codec_g729ab_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
	enum kSMVMPTxVADMode VADMode;
	tSM_INT ptime;
} SM_VMPTX_CODEC_G729AB_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_g729ab sm_vmptx_config_codec_g729ab_iPsVtie1_3_i_
#define SM_VMPTX_CONFIG_CODEC_G729AB_NAME "sm_vmptx_config_codec_g729ab_iPsVtie1_3_i_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_G729AB_FN(struct sm_vmptx_codec_g729ab_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_g729ab(struct sm_vmptx_codec_g729ab_parms *codecp);


typedef struct sm_vmptx_codec_g729i_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
	enum kSMG729IMode g729_mode;
	enum kSMVMPTxVADMode VADMode;
	tSM_INT ptime;
} SM_VMPTX_CODEC_G729I_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_g729i sm_vmptx_config_codec_g729i_iPsVtie1_3_e1_3_i_
#define SM_VMPTX_CONFIG_CODEC_G729I_NAME "sm_vmptx_config_codec_g729i_iPsVtie1_3_e1_3_i_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_G729I_FN(struct sm_vmptx_codec_g729i_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_g729i(struct sm_vmptx_codec_g729i_parms *codecp);


typedef struct sm_vmptx_codec_gsmefr_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
	enum kSMVMPTxVADMode VADMode;
	tSM_INT frames_per_packet;
} SM_VMPTX_CODEC_GSMEFR_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_gsmefr sm_vmptx_config_codec_gsmefr_iPsVtie1_3_i_
#define SM_VMPTX_CONFIG_CODEC_GSMEFR_NAME "sm_vmptx_config_codec_gsmefr_iPsVtie1_3_i_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_GSMEFR_FN(struct sm_vmptx_codec_gsmefr_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_gsmefr(struct sm_vmptx_codec_gsmefr_parms *codecp);


typedef struct sm_vmptx_codec_gsmfr_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
	enum kSMGSMVariant variant;
	enum kSMVMPTxVADMode VADMode;
	tSM_INT frames_per_packet;
} SM_VMPTX_CODEC_GSMFR_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_gsmfr sm_vmptx_config_codec_gsmfr_iPsVtie1_2_e1_3_i_
#define SM_VMPTX_CONFIG_CODEC_GSMFR_NAME "sm_vmptx_config_codec_gsmfr_iPsVtie1_2_e1_3_i_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_GSMFR_FN(struct sm_vmptx_codec_gsmfr_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_gsmfr(struct sm_vmptx_codec_gsmfr_parms *codecp);


typedef struct sm_vmptx_codec_gsmhr_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
	enum kSMVMPTxVADMode VADMode;
	tSM_INT frames_per_packet;
} SM_VMPTX_CODEC_GSMHR_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_gsmhr sm_vmptx_config_codec_gsmhr_iPsVtie1_3_i_
#define SM_VMPTX_CONFIG_CODEC_GSMHR_NAME "sm_vmptx_config_codec_gsmhr_iPsVtie1_3_i_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_GSMHR_FN(struct sm_vmptx_codec_gsmhr_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_gsmhr(struct sm_vmptx_codec_gsmhr_parms *codecp);


typedef struct sm_vmptx_codec_ilbc_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
	tSM_INT frame_len;
	enum kSMVMPTxVADMode VADMode;
	tSM_INT frames_per_packet;
} SM_VMPTX_CODEC_ILBC_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_ilbc sm_vmptx_config_codec_ilbc_iPsVtiie1_3_i_
#define SM_VMPTX_CONFIG_CODEC_ILBC_NAME "sm_vmptx_config_codec_ilbc_iPsVtiie1_3_i_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_ILBC_FN(struct sm_vmptx_codec_ilbc_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_ilbc(struct sm_vmptx_codec_ilbc_parms *codecp);


typedef struct sm_vmptx_codec_isac_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
	tSMVMPrxId partner;
	tSM_INT channel_associated_mode;
} SM_VMPTX_CODEC_ISAC_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_isac sm_vmptx_config_codec_isac_iPsVtiVri_
#define SM_VMPTX_CONFIG_CODEC_ISAC_NAME "sm_vmptx_config_codec_isac_iPsVtiVri_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_ISAC_FN(struct sm_vmptx_codec_isac_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_isac(struct sm_vmptx_codec_isac_parms *codecp);


typedef struct sm_vmptx_codec_l16_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
	enum kSMVMPTxVADMode VADMode;
	tSM_INT ptime;
} SM_VMPTX_CODEC_L16_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_l16 sm_vmptx_config_codec_l16_iPsVtie1_3_i_
#define SM_VMPTX_CONFIG_CODEC_L16_NAME "sm_vmptx_config_codec_l16_iPsVtie1_3_i_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_L16_FN(struct sm_vmptx_codec_l16_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_l16(struct sm_vmptx_codec_l16_parms *codecp);


typedef struct sm_vmptx_codec_l8_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
	enum kSMVMPTxVADMode VADMode;
	tSM_INT ptime;
} SM_VMPTX_CODEC_L8_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_l8 sm_vmptx_config_codec_l8_iPsVtie1_3_i_
#define SM_VMPTX_CONFIG_CODEC_L8_NAME "sm_vmptx_config_codec_l8_iPsVtie1_3_i_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_L8_FN(struct sm_vmptx_codec_l8_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_l8(struct sm_vmptx_codec_l8_parms *codecp);


typedef struct sm_vmptx_codec_melpe_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
	tSM_INT ptime;
} SM_VMPTX_CODEC_MELPE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_melpe sm_vmptx_config_codec_melpe_iPsVtii_
#define SM_VMPTX_CONFIG_CODEC_MELPE_NAME "sm_vmptx_config_codec_melpe_iPsVtii_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_MELPE_FN(struct sm_vmptx_codec_melpe_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_melpe(struct sm_vmptx_codec_melpe_parms *codecp);


typedef struct sm_vmptx_codec_mulaw_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
	enum kSMVMPTxVADMode VADMode;
	tSM_INT ptime;
} SM_VMPTX_CODEC_MULAW_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_mulaw sm_vmptx_config_codec_mulaw_iPsVtie1_3_i_
#define SM_VMPTX_CONFIG_CODEC_MULAW_NAME "sm_vmptx_config_codec_mulaw_iPsVtie1_3_i_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_MULAW_FN(struct sm_vmptx_codec_mulaw_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_mulaw(struct sm_vmptx_codec_mulaw_parms *codecp);


typedef struct sm_vmptx_codec_opus_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
	tSM_INT sample_rate;
	tSM_INT bit_rate;
	tSM_INT ptime;
	tSM_INT packet_loss;
	tSM_INT complexity;
	tSM_INT use_fec;
	tSM_INT use_dtx;
	enum kSMOPUSRate rate_control;
	enum kSMOPUSSignalTypeHint signal_type_hint;
	enum kSMOPUSAppType application_type;
	tSM_INT disable_prediction;
} SM_VMPTX_CODEC_OPUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_opus sm_vmptx_config_codec_opus_iPsVtiiiiiiiie1_3_e1_3_e1_3_i_
#define SM_VMPTX_CONFIG_CODEC_OPUS_NAME "sm_vmptx_config_codec_opus_iPsVtiiiiiiiie1_3_e1_3_e1_3_i_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_OPUS_FN(struct sm_vmptx_codec_opus_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_opus(struct sm_vmptx_codec_opus_parms *codecp);


typedef struct sm_vmptx_config_codec_opus_mode_parms {
	tSMVMPtxId vmptx;
	tSM_INT sample_rate;
	tSM_INT bit_rate;
	tSM_INT ptime;
	tSM_INT packet_loss;
	tSM_INT complexity;
	tSM_INT use_fec;
	tSM_INT use_dtx;
	enum kSMOPUSRate rate_control;
	enum kSMOPUSSignalTypeHint signal_type_hint;
	enum kSMOPUSAppType application_type;
	tSM_INT disable_prediction;
} SM_VMPTX_CONFIG_CODEC_OPUS_MODE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_opus_mode sm_vmptx_config_codec_opus_mode_iPsVtiiiiiiie1_3_e1_3_e1_3_i_
#define SM_VMPTX_CONFIG_CODEC_OPUS_MODE_NAME "sm_vmptx_config_codec_opus_mode_iPsVtiiiiiiie1_3_e1_3_e1_3_i_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_OPUS_MODE_FN(struct sm_vmptx_config_codec_opus_mode_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_opus_mode(struct sm_vmptx_config_codec_opus_mode_parms *codecp);


typedef struct sm_vmptx_codec_rfc2833_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
} SM_VMPTX_CODEC_RFC2833_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_rfc2833 sm_vmptx_config_codec_rfc2833_iPsVti_
#define SM_VMPTX_CONFIG_CODEC_RFC2833_NAME "sm_vmptx_config_codec_rfc2833_iPsVti_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_RFC2833_FN(struct sm_vmptx_codec_rfc2833_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_rfc2833(struct sm_vmptx_codec_rfc2833_parms *codecp);


typedef struct sm_vmptx_codec_rfc4040_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
	tSM_INT packetlen;
} SM_VMPTX_CODEC_RFC4040_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_rfc4040 sm_vmptx_config_codec_rfc4040_iPsVtii_
#define SM_VMPTX_CONFIG_CODEC_RFC4040_NAME "sm_vmptx_config_codec_rfc4040_iPsVtii_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_RFC4040_FN(struct sm_vmptx_codec_rfc4040_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_rfc4040(struct sm_vmptx_codec_rfc4040_parms *codecp);


typedef struct sm_vmptx_codec_rttext_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
	tSM_INT redundant_payload_type;
	tSM_INT redundancy_level;
	tSM_INT buffer_time;
	tSM_INT idle_time;
} SM_VMPTX_CODEC_RTTEXT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_rttext sm_vmptx_config_codec_rttext_iPsVtiiiii_
#define SM_VMPTX_CONFIG_CODEC_RTTEXT_NAME "sm_vmptx_config_codec_rttext_iPsVtiiiii_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_RTTEXT_FN(struct sm_vmptx_codec_rttext_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_rttext(struct sm_vmptx_codec_rttext_parms *codecp);


typedef struct sm_vmptx_codec_silk_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
	tSM_INT sample_rate;
	tSM_INT internal_rate;
	tSM_INT bit_rate;
	tSM_INT frames_per_packet;
	tSM_INT packet_loss;
	tSM_INT complexity;
	tSM_INT use_fec;
	tSM_INT use_dtx;
} SM_VMPTX_CODEC_SILK_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_silk sm_vmptx_config_codec_silk_iPsVtiiiiiiiii_
#define SM_VMPTX_CONFIG_CODEC_SILK_NAME "sm_vmptx_config_codec_silk_iPsVtiiiiiiiii_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_SILK_FN(struct sm_vmptx_codec_silk_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_silk(struct sm_vmptx_codec_silk_parms *codecp);


typedef struct sm_vmptx_config_codec_silk_mode_parms {
	tSMVMPtxId vmptx;
	tSM_INT sample_rate;
	tSM_INT internal_rate;
	tSM_INT bit_rate;
	tSM_INT frames_per_packet;
	tSM_INT packet_loss;
	tSM_INT complexity;
	tSM_INT use_fec;
	tSM_INT use_dtx;
} SM_VMPTX_CONFIG_CODEC_SILK_MODE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_silk_mode sm_vmptx_config_codec_silk_mode_iPsVtiiiiiiii_
#define SM_VMPTX_CONFIG_CODEC_SILK_MODE_NAME "sm_vmptx_config_codec_silk_mode_iPsVtiiiiiiii_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_SILK_MODE_FN(struct sm_vmptx_config_codec_silk_mode_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_silk_mode(struct sm_vmptx_config_codec_silk_mode_parms *codecp);


typedef struct sm_vmptx_codec_smv_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
	enum kSMVMPTxVADMode VADMode;
	tSM_INT frames_per_packet;
} SM_VMPTX_CODEC_SMV_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_smv sm_vmptx_config_codec_smv_iPsVtie1_3_i_
#define SM_VMPTX_CONFIG_CODEC_SMV_NAME "sm_vmptx_config_codec_smv_iPsVtie1_3_i_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_SMV_FN(struct sm_vmptx_codec_smv_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_smv(struct sm_vmptx_codec_smv_parms *codecp);


typedef struct sm_vmptx_codec_speex_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
	enum kSMSpeexMode bandwidth;
	enum kSMVMPTxVADMode VADMode;
	tSM_INT frames_per_packet;
} SM_VMPTX_CODEC_SPEEX_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_speex sm_vmptx_config_codec_speex_iPsVtie1_3_e1_3_i_
#define SM_VMPTX_CONFIG_CODEC_SPEEX_NAME "sm_vmptx_config_codec_speex_iPsVtie1_3_e1_3_i_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_SPEEX_FN(struct sm_vmptx_codec_speex_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_speex(struct sm_vmptx_codec_speex_parms *codecp);


typedef struct sm_vmptx_codec_speex_mode_parms {
	tSMVMPtxId vmptx;
	tSM_UT32 complexity;
	tSM_UT32 quality;
	tSM_UT32 bitrate;
	tSM_UT32 denoiser;
} SM_VMPTX_CODEC_SPEEX_MODE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_speex_mode sm_vmptx_config_codec_speex_mode_iPsVtUiUiUiUi_
#define SM_VMPTX_CONFIG_CODEC_SPEEX_MODE_NAME "sm_vmptx_config_codec_speex_mode_iPsVtUiUiUiUi_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_SPEEX_MODE_FN(struct sm_vmptx_codec_speex_mode_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_speex_mode(struct sm_vmptx_codec_speex_mode_parms *codecp);


typedef struct sm_vmptx_codec_sse_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
	tSM_INT retransmissions;
	tSM_INT delay;
} SM_VMPTX_CODEC_SSE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_sse sm_vmptx_config_codec_sse_iPsVtiii_
#define SM_VMPTX_CONFIG_CODEC_SSE_NAME "sm_vmptx_config_codec_sse_iPsVtiii_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_SSE_FN(struct sm_vmptx_codec_sse_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_sse(struct sm_vmptx_codec_sse_parms *codecp);


typedef struct sm_vmptx_codec_tetra_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
} SM_VMPTX_CODEC_TETRA_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec_tetra sm_vmptx_config_codec_tetra_iPsVti_
#define SM_VMPTX_CONFIG_CODEC_TETRA_NAME "sm_vmptx_config_codec_tetra_iPsVti_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_TETRA_FN(struct sm_vmptx_codec_tetra_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec_tetra(struct sm_vmptx_codec_tetra_parms *codecp);


typedef struct sm_vmptx_config_encryption_aes_cm_parms {
	tSMVMPtxId vmptx;
	tSM_INT keylen;
	char *key;
} SM_VMPTX_CONFIG_ENCRYPTION_AES_CM_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_encryption_aes_cm sm_vmptx_config_encryption_aes_cm_iPsVtiPc_
#define SM_VMPTX_CONFIG_ENCRYPTION_AES_CM_NAME "sm_vmptx_config_encryption_aes_cm_iPsVtiPc_"
#endif
typedef int SM_VMPTX_CONFIG_ENCRYPTION_AES_CM_FN(struct sm_vmptx_config_encryption_aes_cm_parms *pp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_encryption_aes_cm(struct sm_vmptx_config_encryption_aes_cm_parms *pp);


typedef struct sm_vmptx_config_encryption_aes_f8_parms {
	tSMVMPtxId vmptx;
	tSM_INT keylen;
	char *key;
} SM_VMPTX_CONFIG_ENCRYPTION_AES_F8_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_encryption_aes_f8 sm_vmptx_config_encryption_aes_f8_iPsVtiPc_
#define SM_VMPTX_CONFIG_ENCRYPTION_AES_F8_NAME "sm_vmptx_config_encryption_aes_f8_iPsVtiPc_"
#endif
typedef int SM_VMPTX_CONFIG_ENCRYPTION_AES_F8_FN(struct sm_vmptx_config_encryption_aes_f8_parms *pp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_encryption_aes_f8(struct sm_vmptx_config_encryption_aes_f8_parms *pp);


typedef struct sm_vmptx_config_encryption_null_parms {
	tSMVMPtxId vmptx;
} SM_VMPTX_CONFIG_ENCRYPTION_NULL_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_encryption_null sm_vmptx_config_encryption_null_iPsVt_
#define SM_VMPTX_CONFIG_ENCRYPTION_NULL_NAME "sm_vmptx_config_encryption_null_iPsVt_"
#endif
typedef int SM_VMPTX_CONFIG_ENCRYPTION_NULL_FN(struct sm_vmptx_config_encryption_null_parms *pp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_encryption_null(struct sm_vmptx_config_encryption_null_parms *pp);


typedef struct sm_vmptx_config_ipv6_parms {
	tSMVMPtxId vmptx;
	SOCKADDR_IN6 destination_rtp;
	SOCKADDR_IN6 source_rtp;
	SOCKADDR_IN6 destination_rtcp;
	SOCKADDR_IN6 source_rtcp;
} SM_VMPTX_CONFIG_IPV6_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_ipv6 sm_vmptx_config_ipv6_iPsVtSA6dSA6dSA6dSA6d_
#define SM_VMPTX_CONFIG_IPV6_NAME "sm_vmptx_config_ipv6_iPsVtSA6dSA6dSA6dSA6d_"
#endif
typedef int SM_VMPTX_CONFIG_IPV6_FN(struct sm_vmptx_config_ipv6_parms *configp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_ipv6(struct sm_vmptx_config_ipv6_parms *configp);


typedef struct sm_vmptx_isac_rate_parms {
	tSMVMPtxId vmptx;
	tSM_INT rate;
	tSM_INT ptime;
} SM_VMPTX_ISAC_RATE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_isac_rate sm_vmptx_config_isac_rate_iPsVtii_
#define SM_VMPTX_CONFIG_ISAC_RATE_NAME "sm_vmptx_config_isac_rate_iPsVtii_"
#endif
typedef int SM_VMPTX_CONFIG_ISAC_RATE_FN(struct sm_vmptx_isac_rate_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_isac_rate(struct sm_vmptx_isac_rate_parms *codecp);


typedef struct sm_vmptx_config_profile_specific_parms {
	tSMVMPtxId vmptx;
	tSM_INT enable;
	char *plugin;
	tSM_INT paramlen;
	char *paramval;
} SM_VMPTX_CONFIG_PROFILE_SPECIFIC_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_profile_specific sm_vmptx_config_profile_specific_iPsVtiPciPc_
#define SM_VMPTX_CONFIG_PROFILE_SPECIFIC_NAME "sm_vmptx_config_profile_specific_iPsVtiPciPc_"
#endif
typedef int SM_VMPTX_CONFIG_PROFILE_SPECIFIC_FN(struct sm_vmptx_config_profile_specific_parms *psp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_profile_specific(struct sm_vmptx_config_profile_specific_parms *psp);


typedef struct sm_vmptx_sample_rate_parms {
	tSMVMPtxId vmptx;
	tSM_UT32 sample_rate;
} SM_VMPTX_SAMPLE_RATE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_sample_rate sm_vmptx_config_sample_rate_iPsVtUi_
#define SM_VMPTX_CONFIG_SAMPLE_RATE_NAME "sm_vmptx_config_sample_rate_iPsVtUi_"
#endif
typedef int SM_VMPTX_CONFIG_SAMPLE_RATE_FN(struct sm_vmptx_sample_rate_parms *sample_ratep);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_sample_rate(struct sm_vmptx_sample_rate_parms *sample_ratep);


typedef struct sm_vmptx_tag_parms {
	tSMVMPtxId vmptx;
	tSM_INT payload_type;
	enum kSMVMPTxTagType tag_type;
	tSM_UT32 min_time;
	tSM_UT32 max_time;
} SM_VMPTX_TAG_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_tag sm_vmptx_config_tag_iPsVtie1_2_UiUi_
#define SM_VMPTX_CONFIG_TAG_NAME "sm_vmptx_config_tag_iPsVtie1_2_UiUi_"
#endif
typedef int SM_VMPTX_CONFIG_TAG_FN(struct sm_vmptx_tag_parms *tagp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_tag(struct sm_vmptx_tag_parms *tagp);


typedef struct sm_vmptx_tone_parms {
	tSMVMPtxId vmptx;
	tSM_INT convert_tones;
	tSM_INT elim_tones;
	tSMVMPTxToneSetId tone_set_id;
	enum kSMVMPtxRFC2833MinDuration min_duration;
} SM_VMPTX_TONE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_tones sm_vmptx_config_tones_iPsVtiiVMTde1_2__
#define SM_VMPTX_CONFIG_TONES_NAME "sm_vmptx_config_tones_iPsVtiiVMTde1_2__"
#endif
typedef int SM_VMPTX_CONFIG_TONES_FN(struct sm_vmptx_tone_parms *tonep);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_tones(struct sm_vmptx_tone_parms *tonep);


typedef struct sm_vmptx_create_parms {
	tSMVMPtxId vmptx;
	tSMModuleId module;
} SM_VMPTX_CREATE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_create sm_vmptx_create_iPsVtMt_
#define SM_VMPTX_CREATE_NAME "sm_vmptx_create_iPsVtMt_"
#endif
typedef int SM_VMPTX_CREATE_FN(struct sm_vmptx_create_parms *vmptxp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_create(struct sm_vmptx_create_parms *vmptxp);


typedef struct sm_vmptx_create_csrc_list_parms {
	tSMVMPTxCSRCListId csrc_list;
	tSMModuleId module;
} SM_VMPTX_CREATE_CSRC_LIST_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_create_csrc_list sm_vmptx_create_csrc_list_iPsVMcMt_
#define SM_VMPTX_CREATE_CSRC_LIST_NAME "sm_vmptx_create_csrc_list_iPsVMcMt_"
#endif
typedef int SM_VMPTX_CREATE_CSRC_LIST_FN(struct sm_vmptx_create_csrc_list_parms *csrcp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_create_csrc_list(struct sm_vmptx_create_csrc_list_parms *csrcp);


typedef struct sm_vmptx_create_toneset_parms {
	tSMVMPTxToneSet *toneset;
	tSMModuleId module;
	tSMVMPTxToneSetId tone_set_id;
} SM_VMPTX_CREATE_TONESET_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_create_toneset sm_vmptx_create_toneset_iPsPVMsMtVMTd_
#define SM_VMPTX_CREATE_TONESET_NAME "sm_vmptx_create_toneset_iPsPVMsMtVMTd_"
#endif
typedef int SM_VMPTX_CREATE_TONESET_FN(struct sm_vmptx_create_toneset_parms *tonesetp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_create_toneset(struct sm_vmptx_create_toneset_parms *tonesetp);


typedef struct sm_vmptx_csrc_list_set_parms {
	tSMVMPTxCSRCListId csrc_list;
	int num_csrc;
	int *csrc;
} SM_VMPTX_CSRC_LIST_SET_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_csrc_list_set sm_vmptx_csrc_list_set_iPsVMciPi_
#define SM_VMPTX_CSRC_LIST_SET_NAME "sm_vmptx_csrc_list_set_iPsVMciPi_"
#endif
typedef int SM_VMPTX_CSRC_LIST_SET_FN(struct sm_vmptx_csrc_list_set_parms *csrcp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_csrc_list_set(struct sm_vmptx_csrc_list_set_parms *csrcp);


typedef struct sm_vmptx_datafeed_connect_parms {
	tSMDatafeedId data_source;
	tSMVMPtxId vmptx;
} SM_VMPTX_DATAFEED_CONNECT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_datafeed_connect sm_vmptx_datafeed_connect_iPsDtVt_
#define SM_VMPTX_DATAFEED_CONNECT_NAME "sm_vmptx_datafeed_connect_iPsDtVt_"
#endif
typedef int SM_VMPTX_DATAFEED_CONNECT_FN(struct sm_vmptx_datafeed_connect_parms *datafeedp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_datafeed_connect(struct sm_vmptx_datafeed_connect_parms *datafeedp);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_destroy sm_vmptx_destroy_iVt
#define SM_VMPTX_DESTROY_NAME "sm_vmptx_destroy_iVt"
#endif
typedef int SM_VMPTX_DESTROY_FN(tSMVMPtxId vmptx);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_destroy(tSMVMPtxId vmptx);


typedef struct sm_vmptx_destroy_csrc_list_parms {
	tSMVMPTxCSRCListId csrc_list;
} SM_VMPTX_DESTROY_CSRC_LIST_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_destroy_csrc_list sm_vmptx_destroy_csrc_list_iPsVMc_
#define SM_VMPTX_DESTROY_CSRC_LIST_NAME "sm_vmptx_destroy_csrc_list_iPsVMc_"
#endif
typedef int SM_VMPTX_DESTROY_CSRC_LIST_FN(struct sm_vmptx_destroy_csrc_list_parms *csrcp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_destroy_csrc_list(struct sm_vmptx_destroy_csrc_list_parms *csrcp);


typedef struct sm_vmptx_destroy_toneset_parms {
	tSMVMPTxToneSetId tone_set_id;
} SM_VMPTX_DESTROY_TONESET_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_destroy_toneset sm_vmptx_destroy_toneset_iPsVMTd_
#define SM_VMPTX_DESTROY_TONESET_NAME "sm_vmptx_destroy_toneset_iPsVMTd_"
#endif
typedef int SM_VMPTX_DESTROY_TONESET_FN(struct sm_vmptx_destroy_toneset_parms *tonep);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_destroy_toneset(struct sm_vmptx_destroy_toneset_parms *tonep);


typedef struct sm_vmptx_generate_jitter_parms {
	tSMVMPtxId vmptx;
	tSM_INT external_jitter_threshold;
	tSM_INT inter_jitter_gap;
	tSM_INT added_jitter;
	tSM_INT max_packet_age;
} SM_VMPTX_GENERATE_JITTER_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_generate_jitter sm_vmptx_generate_jitter_iPsVtiiii_
#define SM_VMPTX_GENERATE_JITTER_NAME "sm_vmptx_generate_jitter_iPsVtiiii_"
#endif
typedef int SM_VMPTX_GENERATE_JITTER_FN(struct sm_vmptx_generate_jitter_parms *jitterp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_generate_jitter(struct sm_vmptx_generate_jitter_parms *jitterp);


typedef struct sm_vmptx_generate_tones_parms {
	tSMVMPtxId vmptx;
	tSM_INT *tones;
	tSM_INT num;
	tSM_INT duration;
	tSM_INT interval;
	tSM_INT volume;
} SM_VMPTX_GENERATE_TONES_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_generate_tones sm_vmptx_generate_tones_iPsVtPiiiii_
#define SM_VMPTX_GENERATE_TONES_NAME "sm_vmptx_generate_tones_iPsVtPiiiii_"
#endif
typedef int SM_VMPTX_GENERATE_TONES_FN(struct sm_vmptx_generate_tones_parms *tonep);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_generate_tones(struct sm_vmptx_generate_tones_parms *tonep);


typedef struct sm_vmptx_generate_tones_abort_parms {
	tSMVMPtxId vmptx;
} SM_VMPTX_GENERATE_TONES_ABORT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_generate_tones_abort sm_vmptx_generate_tones_abort_iPsVt_
#define SM_VMPTX_GENERATE_TONES_ABORT_NAME "sm_vmptx_generate_tones_abort_iPsVt_"
#endif
typedef int SM_VMPTX_GENERATE_TONES_ABORT_FN(struct sm_vmptx_generate_tones_abort_parms *tonep);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_generate_tones_abort(struct sm_vmptx_generate_tones_abort_parms *tonep);


typedef struct sm_vmptx_event_parms {
	tSMVMPtxId vmptx;
	tSMEventId event;
} SM_VMPTX_EVENT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_get_event sm_vmptx_get_event_iPsVtEt_
#define SM_VMPTX_GET_EVENT_NAME "sm_vmptx_get_event_iPsVtEt_"
#endif
typedef int SM_VMPTX_GET_EVENT_FN(struct sm_vmptx_event_parms *eventp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_get_event(struct sm_vmptx_event_parms *eventp);


typedef struct sm_vmptx_propagate_rtcp_sr_ntp_parms {
	tSMVMPtxId vmptx;
	tSM_UT64 localtimeref;
	tSM_UT64 ntp_timestamp;
} SM_VMPTX_PROPAGATE_RTCP_SR_NTP_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_propagate_rtcp_sr_ntp sm_vmptx_propagate_rtcp_sr_ntp_iPsVtU64iU64i_
#define SM_VMPTX_PROPAGATE_RTCP_SR_NTP_NAME "sm_vmptx_propagate_rtcp_sr_ntp_iPsVtU64iU64i_"
#endif
typedef int SM_VMPTX_PROPAGATE_RTCP_SR_NTP_FN(struct sm_vmptx_propagate_rtcp_sr_ntp_parms *propp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_propagate_rtcp_sr_ntp(struct sm_vmptx_propagate_rtcp_sr_ntp_parms *propp);


typedef struct sm_vmptx_send_sse_parms {
	tSMVMPtxId vmptx;
	char *payload;
	tSM_INT payload_length;
} SM_VMPTX_SEND_SSE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_send_sse sm_vmptx_send_sse_iPsVtPci_
#define SM_VMPTX_SEND_SSE_NAME "sm_vmptx_send_sse_iPsVtPci_"
#endif
typedef int SM_VMPTX_SEND_SSE_FN(struct sm_vmptx_send_sse_parms *sendssep);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_send_sse(struct sm_vmptx_send_sse_parms *sendssep);


typedef struct sm_vmptx_set_csrc_parms {
	tSMVMPtxId vmptx;
	unsigned repeat_delay;
	unsigned packet_count;
	enum kSMVMPTxCSRCType type;
	union {
		struct {
			int num_csrc;
			int *csrc;
		} csrc_array;
		struct {
			tSMVMPTxCSRCListId csrc_list_id;
			int exclude_ssrc;
			int ssrc;
		} csrc_list;
	} u;
} SM_VMPTX_SET_CSRC_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_set_csrc sm_vmptx_set_csrc_iPsVtUiUie1_2_usiPi_sVMcii___
#define SM_VMPTX_SET_CSRC_NAME "sm_vmptx_set_csrc_iPsVtUiUie1_2_usiPi_sVMcii___"
#endif
typedef int SM_VMPTX_SET_CSRC_FN(struct sm_vmptx_set_csrc_parms *csrcp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_set_csrc(struct sm_vmptx_set_csrc_parms *csrcp);


typedef struct sm_vmptx_set_profile_specific_parms {
	tSMVMPtxId vmptx;
	tSM_INT paramlen;
	char *paramval;
} SM_VMPTX_SET_PROFILE_SPECIFIC_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_set_profile_specific sm_vmptx_set_profile_specific_iPsVtiPc_
#define SM_VMPTX_SET_PROFILE_SPECIFIC_NAME "sm_vmptx_set_profile_specific_iPsVtiPc_"
#endif
typedef int SM_VMPTX_SET_PROFILE_SPECIFIC_FN(struct sm_vmptx_set_profile_specific_parms *psp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_set_profile_specific(struct sm_vmptx_set_profile_specific_parms *psp);


typedef struct sm_vmptx_set_rtcphand_parms {
	tSMVMPtxId vmptx;
	tSMRTCPHandId rtcphand;
} SM_VMPTX_SET_RTCPHAND_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_set_rtcphand sm_vmptx_set_rtcphand_iPsVtRs_
#define SM_VMPTX_SET_RTCPHAND_NAME "sm_vmptx_set_rtcphand_iPsVtRs_"
#endif
typedef int SM_VMPTX_SET_RTCPHAND_FN(struct sm_vmptx_set_rtcphand_parms *rvp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_set_rtcphand(struct sm_vmptx_set_rtcphand_parms *rvp);


typedef struct sm_vmptx_status_parms {
	tSMVMPtxId vmptx;
	enum kSMVMPtxStatus status;
	union {
		struct {
			int ssrc;
		} ssrc;
	} u;
} SM_VMPTX_STATUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_status sm_vmptx_status_iPsVte1_5_usi___
#define SM_VMPTX_STATUS_NAME "sm_vmptx_status_iPsVte1_5_usi___"
#endif
typedef int SM_VMPTX_STATUS_FN(struct sm_vmptx_status_parms *statusp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_status(struct sm_vmptx_status_parms *statusp);


typedef struct sm_vmptx_stop_parms {
	tSMVMPtxId vmptx;
} SM_VMPTX_STOP_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_stop sm_vmptx_stop_iPsVt_
#define SM_VMPTX_STOP_NAME "sm_vmptx_stop_iPsVt_"
#endif
typedef int SM_VMPTX_STOP_FN(struct sm_vmptx_stop_parms *stopp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_stop(struct sm_vmptx_stop_parms *stopp);


typedef struct sm_vidmprx_codec_rfc3984_parms {
	tSMVidMPrxId vidmprx;
	tSM_INT payload_type;
	enum kSMRFC3984PicSize pic_size;
} SM_VIDMPRX_CODEC_RFC3984_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vidmprx_config_codec_rfc3984 sm_vidmprx_config_codec_rfc3984_iPsVIDrie1_5__
#define SM_VIDMPRX_CONFIG_CODEC_RFC3984_NAME "sm_vidmprx_config_codec_rfc3984_iPsVIDrie1_5__"
#endif
typedef int SM_VIDMPRX_CONFIG_CODEC_RFC3984_FN(struct sm_vidmprx_codec_rfc3984_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_config_codec_rfc3984(struct sm_vidmprx_codec_rfc3984_parms *codecp);


typedef struct sm_vidmprx_codec_rfc4629_parms {
	tSMVidMPrxId vidmprx;
	tSM_INT payload_type;
	tSM_INT h263_profile;
	tSM_INT h263_level;
} SM_VIDMPRX_CODEC_RFC4629_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vidmprx_config_codec_rfc4629 sm_vidmprx_config_codec_rfc4629_iPsVIDriii_
#define SM_VIDMPRX_CONFIG_CODEC_RFC4629_NAME "sm_vidmprx_config_codec_rfc4629_iPsVIDriii_"
#endif
typedef int SM_VIDMPRX_CONFIG_CODEC_RFC4629_FN(struct sm_vidmprx_codec_rfc4629_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_config_codec_rfc4629(struct sm_vidmprx_codec_rfc4629_parms *codecp);


typedef struct sm_vidmprx_dataloss_parms {
	tSMVidMPrxId vidmprx;
	tSM_INT loss_threshold;
} SM_VIDMPRX_DATALOSS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vidmprx_config_dataloss sm_vidmprx_config_dataloss_iPsVIDri_
#define SM_VIDMPRX_CONFIG_DATALOSS_NAME "sm_vidmprx_config_dataloss_iPsVIDri_"
#endif
typedef int SM_VIDMPRX_CONFIG_DATALOSS_FN(struct sm_vidmprx_dataloss_parms *datalossp);

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_config_dataloss(struct sm_vidmprx_dataloss_parms *datalossp);


typedef struct sm_vidmprx_jitter_parms {
	tSMVidMPrxId vidmprx;
	tSM_UT32 max_delay_ms;
	tSM_UT32 initial_delay_ms;
} SM_VIDMPRX_JITTER_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vidmprx_config_jitter sm_vidmprx_config_jitter_iPsVIDrUiUi_
#define SM_VIDMPRX_CONFIG_JITTER_NAME "sm_vidmprx_config_jitter_iPsVIDrUiUi_"
#endif
typedef int SM_VIDMPRX_CONFIG_JITTER_FN(struct sm_vidmprx_jitter_parms *jitterp);

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_config_jitter(struct sm_vidmprx_jitter_parms *jitterp);


typedef struct sm_vidmprx_unhandled_payload_reporting_parms {
	tSMVidMPrxId vidmprx;
	tSM_INT delay;
} SM_VIDMPRX_UNHANDLED_PAYLOAD_REPORTING_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vidmprx_config_unhandled_payload_reporting sm_vidmprx_config_unhandled_payload_reporting_iPsVIDri_
#define SM_VIDMPRX_CONFIG_UNHANDLED_PAYLOAD_REPORTING_NAME "sm_vidmprx_config_unhandled_payload_reporting_iPsVIDri_"
#endif
typedef int SM_VIDMPRX_CONFIG_UNHANDLED_PAYLOAD_REPORTING_FN(struct sm_vidmprx_unhandled_payload_reporting_parms *pp);

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_config_unhandled_payload_reporting(struct sm_vidmprx_unhandled_payload_reporting_parms *pp);


typedef struct sm_vidmprx_create_parms {
	tSMVidMPrxId vidmprx;
	tSMModuleId module;
	enum kSMVidMPrxType type;
	struct in_addr address;
	struct in6_addr ipv6_address;
} SM_VIDMPRX_CREATE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vidmprx_create sm_vidmprx_create_iPsVIDrMte1_2_SIdSI6d_
#define SM_VIDMPRX_CREATE_NAME "sm_vidmprx_create_iPsVIDrMte1_2_SIdSI6d_"
#endif
typedef int SM_VIDMPRX_CREATE_FN(struct sm_vidmprx_create_parms *vidmprxp);

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_create(struct sm_vidmprx_create_parms *vidmprxp);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vidmprx_destroy sm_vidmprx_destroy_iVIDr
#define SM_VIDMPRX_DESTROY_NAME "sm_vidmprx_destroy_iVIDr"
#endif
typedef int SM_VIDMPRX_DESTROY_FN(tSMVidMPrxId vidmprx);

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_destroy(tSMVidMPrxId vidmprx);


typedef struct sm_vidmprx_datafeed_parms {
	tSMVidMPrxId vidmprx;
	tSMDatafeedId datafeed;
} SM_VIDMPRX_DATAFEED_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vidmprx_get_datafeed sm_vidmprx_get_datafeed_iPsVIDrDt_
#define SM_VIDMPRX_GET_DATAFEED_NAME "sm_vidmprx_get_datafeed_iPsVIDrDt_"
#endif
typedef int SM_VIDMPRX_GET_DATAFEED_FN(struct sm_vidmprx_datafeed_parms *datafeedp);

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_get_datafeed(struct sm_vidmprx_datafeed_parms *datafeedp);


typedef struct sm_vidmprx_event_parms {
	tSMVidMPrxId vidmprx;
	tSMEventId event;
} SM_VIDMPRX_EVENT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vidmprx_get_event sm_vidmprx_get_event_iPsVIDrEt_
#define SM_VIDMPRX_GET_EVENT_NAME "sm_vidmprx_get_event_iPsVIDrEt_"
#endif
typedef int SM_VIDMPRX_GET_EVENT_FN(struct sm_vidmprx_event_parms *eventp);

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_get_event(struct sm_vidmprx_event_parms *eventp);


typedef struct sm_vidmprx_port_parms {
	tSMVidMPrxId vidmprx;
	int RTP_port;
	int RTCP_port;
	struct in_addr address;
	struct in6_addr ipv6_address;
	tSM_UT32 nowait;
} SM_VIDMPRX_PORT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vidmprx_get_ports sm_vidmprx_get_ports_iPsVIDriiSIdSI6dUi_
#define SM_VIDMPRX_GET_PORTS_NAME "sm_vidmprx_get_ports_iPsVIDriiSIdSI6dUi_"
#endif
typedef int SM_VIDMPRX_GET_PORTS_FN(struct sm_vidmprx_port_parms *portp);

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_get_ports(struct sm_vidmprx_port_parms *portp);


typedef struct sm_vidmprx_ice_stun_change_role_parms {
	tSMVidMPrxId vidmprx;
	enum kSMICERole local_role;
} SM_VIDMPRX_ICE_STUN_CHANGE_ROLE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vidmprx_ice_stun_change_role sm_vidmprx_ice_stun_change_role_iPsVIDre1_4__
#define SM_VIDMPRX_ICE_STUN_CHANGE_ROLE_NAME "sm_vidmprx_ice_stun_change_role_iPsVIDre1_4__"
#endif
typedef int SM_VIDMPRX_ICE_STUN_CHANGE_ROLE_FN(struct sm_vidmprx_ice_stun_change_role_parms *rolep);

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_ice_stun_change_role(struct sm_vidmprx_ice_stun_change_role_parms *rolep);


typedef struct sm_vidmprx_ice_stun_config_parms {
	tSMVidMPrxId vidmprx;
	enum kSMICERole local_role;
	char *local_ice_ufrag;
	tSM_UT32 local_ice_ufrag_length;
	char *local_ice_pwd;
	tSM_UT32 local_ice_pwd_length;
	tSM_UT64 role_tiebreaker;
} SM_VIDMPRX_ICE_STUN_CONFIG_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vidmprx_ice_stun_config sm_vidmprx_ice_stun_config_iPsVIDre1_4_PcUiPcUiU64i_
#define SM_VIDMPRX_ICE_STUN_CONFIG_NAME "sm_vidmprx_ice_stun_config_iPsVIDre1_4_PcUiPcUiU64i_"
#endif
typedef int SM_VIDMPRX_ICE_STUN_CONFIG_FN(struct sm_vidmprx_ice_stun_config_parms *configp);

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_ice_stun_config(struct sm_vidmprx_ice_stun_config_parms *configp);


typedef struct sm_vidmprx_ice_stun_remote_credentials_parms {
	tSMVidMPrxId vidmprx;
	char *remote_ice_ufrag;
	tSM_UT32 remote_ice_ufrag_length;
	char *remote_ice_pwd;
	tSM_UT32 remote_ice_pwd_length;
} SM_VIDMPRX_ICE_STUN_REMOTE_CREDENTIALS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vidmprx_ice_stun_remote_credentials sm_vidmprx_ice_stun_remote_credentials_iPsVIDrPcUiPcUi_
#define SM_VIDMPRX_ICE_STUN_REMOTE_CREDENTIALS_NAME "sm_vidmprx_ice_stun_remote_credentials_iPsVIDrPcUiPcUi_"
#endif
typedef int SM_VIDMPRX_ICE_STUN_REMOTE_CREDENTIALS_FN(struct sm_vidmprx_ice_stun_remote_credentials_parms *credentialsp);

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_ice_stun_remote_credentials(struct sm_vidmprx_ice_stun_remote_credentials_parms *credentialsp);


typedef struct sm_vidmprx_ice_stun_send_bind_request_parms {
	tSMVidMPrxId vidmprx;
	int use_RTCP_port;
	char transaction_id[12];
	union {
		SOCKADDR_IN ipv4;
		SOCKADDR_IN6 ipv6;
	} destination;
	tSM_UT32 priority;
	tSM_INT use_candidate;
} SM_VIDMPRX_ICE_STUN_SEND_BIND_REQUEST_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vidmprx_ice_stun_send_bind_request sm_vidmprx_ice_stun_send_bind_request_iPsVIDricr12uSAdSA6d_Uii_
#define SM_VIDMPRX_ICE_STUN_SEND_BIND_REQUEST_NAME "sm_vidmprx_ice_stun_send_bind_request_iPsVIDricr12uSAdSA6d_Uii_"
#endif
typedef int SM_VIDMPRX_ICE_STUN_SEND_BIND_REQUEST_FN(struct sm_vidmprx_ice_stun_send_bind_request_parms *requestp);

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_ice_stun_send_bind_request(struct sm_vidmprx_ice_stun_send_bind_request_parms *requestp);


typedef struct sm_vidmprx_status_parms {
	tSMVidMPrxId vidmprx;
	enum kSMVidMPrxStatus status;
	union {
		struct {
			int RTP_Port;
			int RTCP_Port;
			struct in_addr address;
		} ports;
		struct {
			tSM_INT no_data;
		} run;
		struct {
			struct in_addr address;
			int port;
			int ssrc;
		} ssrc;
		struct {
			int type;
		} payload;
		struct {
			int num_errors;
		} internal_error;
		struct {
			int RTP_Port;
			int RTCP_Port;
			struct in6_addr address;
		} ports_ipv6;
		struct {
			struct in6_addr address;
			int port;
			int ssrc;
		} ssrc_ipv6;
		struct {
			int recv_port;
			enum kSMAddressType address_type;
			union {
				struct in_addr ipv4;
				struct in6_addr ipv6;
			} address;
			int port;
			tSM_UT32 priority;
			int use_candidate;
			int remote_is_controlling;
		} bind_request;
		struct {
			int recv_port;
			char transaction_id[12];
			enum kSMVidMPrxSTUNResult result;
			tSM_INT error_code;
			enum kSMAddressType address_type;
			union {
				struct in_addr ipv4;
				struct in6_addr ipv6;
			} address;
			int port;
			int local_was_controlling;
		} bind_response;
	} u;
} SM_VIDMPRX_STATUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vidmprx_status sm_vidmprx_status_iPsVIDre1_10_usiiSId_si_sSIdii_si_si_siiSI6d_sSI6dii_sie1_2_uSIdSI6d_iUiii_sicr12e1_4_ie1_2_uSIdSI6d_ii___
#define SM_VIDMPRX_STATUS_NAME "sm_vidmprx_status_iPsVIDre1_10_usiiSId_si_sSIdii_si_si_siiSI6d_sSI6dii_sie1_2_uSIdSI6d_iUiii_sicr12e1_4_ie1_2_uSIdSI6d_ii___"
#endif
typedef int SM_VIDMPRX_STATUS_FN(struct sm_vidmprx_status_parms *statusp);

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_status(struct sm_vidmprx_status_parms *statusp);


typedef struct sm_vidmprx_stop_parms {
	tSMVidMPrxId vidmprx;
} SM_VIDMPRX_STOP_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vidmprx_stop sm_vidmprx_stop_iPsVIDr_
#define SM_VIDMPRX_STOP_NAME "sm_vidmprx_stop_iPsVIDr_"
#endif
typedef int SM_VIDMPRX_STOP_FN(struct sm_vidmprx_stop_parms *stopp);

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_stop(struct sm_vidmprx_stop_parms *stopp);


typedef struct sm_vidmprx_sync_media_parms {
	tSMVidMPrxId vidmprx;
	tSMVMPrxId vmprx;
} SM_VIDMPRX_SYNC_MEDIA_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vidmprx_sync_media sm_vidmprx_sync_media_iPsVIDrVr_
#define SM_VIDMPRX_SYNC_MEDIA_NAME "sm_vidmprx_sync_media_iPsVIDrVr_"
#endif
typedef int SM_VIDMPRX_SYNC_MEDIA_FN(struct sm_vidmprx_sync_media_parms *syncmediap);

ACUAPI int ACUTiNG_WINAPI sm_vidmprx_sync_media(struct sm_vidmprx_sync_media_parms *syncmediap);


typedef struct sm_vidmptx_config_parms {
	tSMVidMPtxId vidmptx;
	SOCKADDR_IN destination_rtp;
	SOCKADDR_IN source_rtp;
	int TOS_RTP;
	SOCKADDR_IN destination_rtcp;
	SOCKADDR_IN source_rtcp;
	int TOS_RTCP;
} SM_VIDMPTX_CONFIG_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vidmptx_config sm_vidmptx_config_iPsVIDtSAdSAdiSAdSAdi_
#define SM_VIDMPTX_CONFIG_NAME "sm_vidmptx_config_iPsVIDtSAdSAdiSAdSAdi_"
#endif
typedef int SM_VIDMPTX_CONFIG_FN(struct sm_vidmptx_config_parms *configp);

ACUAPI int ACUTiNG_WINAPI sm_vidmptx_config(struct sm_vidmptx_config_parms *configp);


typedef struct sm_vidmptx_codec_rfc3984_parms {
	tSMVidMPtxId vidmptx;
	tSM_INT payload_type;
	tSM_INT deaggregate;
} SM_VIDMPTX_CODEC_RFC3984_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vidmptx_config_codec_rfc3984 sm_vidmptx_config_codec_rfc3984_iPsVIDtii_
#define SM_VIDMPTX_CONFIG_CODEC_RFC3984_NAME "sm_vidmptx_config_codec_rfc3984_iPsVIDtii_"
#endif
typedef int SM_VIDMPTX_CONFIG_CODEC_RFC3984_FN(struct sm_vidmptx_codec_rfc3984_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vidmptx_config_codec_rfc3984(struct sm_vidmptx_codec_rfc3984_parms *codecp);


typedef struct sm_vidmptx_codec_rfc4629_parms {
	tSMVidMPtxId vidmptx;
	tSM_INT payload_type;
	tSM_INT rem_ext_pic_hdr;
} SM_VIDMPTX_CODEC_RFC4629_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vidmptx_config_codec_rfc4629 sm_vidmptx_config_codec_rfc4629_iPsVIDtii_
#define SM_VIDMPTX_CONFIG_CODEC_RFC4629_NAME "sm_vidmptx_config_codec_rfc4629_iPsVIDtii_"
#endif
typedef int SM_VIDMPTX_CONFIG_CODEC_RFC4629_FN(struct sm_vidmptx_codec_rfc4629_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vidmptx_config_codec_rfc4629(struct sm_vidmptx_codec_rfc4629_parms *codecp);


typedef struct sm_vidmptx_config_ipv6_parms {
	tSMVidMPtxId vidmptx;
	SOCKADDR_IN6 destination_rtp;
	SOCKADDR_IN6 source_rtp;
	SOCKADDR_IN6 destination_rtcp;
	SOCKADDR_IN6 source_rtcp;
} SM_VIDMPTX_CONFIG_IPV6_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vidmptx_config_ipv6 sm_vidmptx_config_ipv6_iPsVIDtSA6dSA6dSA6dSA6d_
#define SM_VIDMPTX_CONFIG_IPV6_NAME "sm_vidmptx_config_ipv6_iPsVIDtSA6dSA6dSA6dSA6d_"
#endif
typedef int SM_VIDMPTX_CONFIG_IPV6_FN(struct sm_vidmptx_config_ipv6_parms *configp);

ACUAPI int ACUTiNG_WINAPI sm_vidmptx_config_ipv6(struct sm_vidmptx_config_ipv6_parms *configp);


typedef struct sm_vidmptx_create_parms {
	tSMVidMPtxId vidmptx;
	tSMModuleId module;
} SM_VIDMPTX_CREATE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vidmptx_create sm_vidmptx_create_iPsVIDtMt_
#define SM_VIDMPTX_CREATE_NAME "sm_vidmptx_create_iPsVIDtMt_"
#endif
typedef int SM_VIDMPTX_CREATE_FN(struct sm_vidmptx_create_parms *vidmptxp);

ACUAPI int ACUTiNG_WINAPI sm_vidmptx_create(struct sm_vidmptx_create_parms *vidmptxp);


typedef struct sm_vidmptx_datafeed_connect_parms {
	tSMDatafeedId data_source;
	tSMVidMPtxId vidmptx;
} SM_VIDMPTX_DATAFEED_CONNECT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vidmptx_datafeed_connect sm_vidmptx_datafeed_connect_iPsDtVIDt_
#define SM_VIDMPTX_DATAFEED_CONNECT_NAME "sm_vidmptx_datafeed_connect_iPsDtVIDt_"
#endif
typedef int SM_VIDMPTX_DATAFEED_CONNECT_FN(struct sm_vidmptx_datafeed_connect_parms *datafeedp);

ACUAPI int ACUTiNG_WINAPI sm_vidmptx_datafeed_connect(struct sm_vidmptx_datafeed_connect_parms *datafeedp);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vidmptx_destroy sm_vidmptx_destroy_iVIDt
#define SM_VIDMPTX_DESTROY_NAME "sm_vidmptx_destroy_iVIDt"
#endif
typedef int SM_VIDMPTX_DESTROY_FN(tSMVidMPtxId vidmptx);

ACUAPI int ACUTiNG_WINAPI sm_vidmptx_destroy(tSMVidMPtxId vidmptx);


typedef struct sm_vidmptx_event_parms {
	tSMVidMPtxId vidmptx;
	tSMEventId event;
} SM_VIDMPTX_EVENT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vidmptx_get_event sm_vidmptx_get_event_iPsVIDtEt_
#define SM_VIDMPTX_GET_EVENT_NAME "sm_vidmptx_get_event_iPsVIDtEt_"
#endif
typedef int SM_VIDMPTX_GET_EVENT_FN(struct sm_vidmptx_event_parms *eventp);

ACUAPI int ACUTiNG_WINAPI sm_vidmptx_get_event(struct sm_vidmptx_event_parms *eventp);


typedef struct sm_vidmptx_status_parms {
	tSMVidMPtxId vidmptx;
	enum kSMVidMPtxStatus status;
	union {
		struct {
			int ssrc;
		} ssrc;
		struct {
			int num_errors;
		} internal_error;
	} u;
} SM_VIDMPTX_STATUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vidmptx_status sm_vidmptx_status_iPsVIDte1_4_usi_si___
#define SM_VIDMPTX_STATUS_NAME "sm_vidmptx_status_iPsVIDte1_4_usi_si___"
#endif
typedef int SM_VIDMPTX_STATUS_FN(struct sm_vidmptx_status_parms *statusp);

ACUAPI int ACUTiNG_WINAPI sm_vidmptx_status(struct sm_vidmptx_status_parms *statusp);


typedef struct sm_vidmptx_stop_parms {
	tSMVidMPtxId vidmptx;
} SM_VIDMPTX_STOP_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vidmptx_stop sm_vidmptx_stop_iPsVIDt_
#define SM_VIDMPTX_STOP_NAME "sm_vidmptx_stop_iPsVIDt_"
#endif
typedef int SM_VIDMPTX_STOP_FN(struct sm_vidmptx_stop_parms *stopp);

ACUAPI int ACUTiNG_WINAPI sm_vidmptx_stop(struct sm_vidmptx_stop_parms *stopp);


typedef struct sm_vmprx_codec_parms {
	tSMVMPrxId vmprx;
	enum kSMCodecType codec;
	tSM_INT payload_type;
	enum kSMPLCMode plc_mode;
} SM_VMPRX_CODEC_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmprx_config_codec sm_vmprx_config_codec_iPsVre1_5_ie1_2__
#define SM_VMPRX_CONFIG_CODEC_NAME "sm_vmprx_config_codec_iPsVre1_5_ie1_2__"
#endif
typedef int SM_VMPRX_CONFIG_CODEC_FN(struct sm_vmprx_codec_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmprx_config_codec(struct sm_vmprx_codec_parms *codecp);


typedef struct sm_vmptx_codec_parms {
	tSMVMPtxId vmptx;
	enum kSMCodecType codec;
	tSM_INT payload_type;
	enum kSMVMPTxVADMode VADMode;
	tSM_INT ptime;
} SM_VMPTX_CODEC_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_vmptx_config_codec sm_vmptx_config_codec_iPsVte1_5_ie1_3_i_
#define SM_VMPTX_CONFIG_CODEC_NAME "sm_vmptx_config_codec_iPsVte1_5_ie1_3_i_"
#endif
typedef int SM_VMPTX_CONFIG_CODEC_FN(struct sm_vmptx_codec_parms *codecp);

ACUAPI int ACUTiNG_WINAPI sm_vmptx_config_codec(struct sm_vmptx_codec_parms *codecp);



#endif
