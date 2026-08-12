/* prosh223api.h - Prosody H.223 Mux API declarations */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

#ifndef INCLUDED_PUBDOC_PROSH223API_H
#define INCLUDED_PUBDOC_PROSH223API_H

#ifdef ACULAB_TiNG_VERSION_NUM
#if ACULAB_TiNG_VERSION_NUM != 1504
#error ACULAB_TiNG_VERSION_NUM does not match 1504
#endif
#else
#define ACULAB_TiNG_VERSION_NUM 1504
#define ACULAB_TiNG_VERSION_CHANGED 
#define ACULAB_TiNG_VERSION_STRING "m1504."
#endif

enum kSMH223muxTableEntryDirection {
	kSMH223muxTableEntryDirectionTransmit,
	kSMH223muxTableEntryDirectionReceive,
};

enum kSMH223muxChannelDirection {
	kSMH223muxChannelDirectionTransmit,
	kSMH223muxChannelDirectionReceive,
};

enum kSMH223muxInputType {
	kSMH223muxInputTypeH245Ctl,
	kSMH223muxInputTypeAudio,
	kSMH223muxInputTypeVideo,
	kSMH223muxInputTypeMux,
};

enum kSMH223muxOutputType {
	kSMH223muxOutputTypeH245Ctl,
	kSMH223muxOutputTypeAudio,
	kSMH223muxOutputTypeVideo,
	kSMH223muxOutputTypeMux,
};

enum kSMH223muxTransmissionBitOrder {
	kSMH223muxTransmissionBitOrderLSBFirst,
	kSMH223muxTransmissionBitOrderMSBFirst,
};

enum kSMH223muxLogicalChannelType {
	kSMH223muxLogicalChannelTypeUnidirectional,
	kSMH223muxLogicalChannelTypeBidirectional,
};

enum kSMH223muxLogicalChannelDirection {
	kSMH223muxLogicalChannelDirectionTransmit,
	kSMH223muxLogicalChannelDirectionReceive,
};

enum kSMH223muxMarkUnit {
	kSMH223muxMarkUnitBytes,
	kSMH223muxMarkUnitALSDU,
};

enum kSMH223muxAdaptationLayerType {
	kSMH223muxAdaptationLayerTypeNonStandard,
	kSMH223muxAdaptationLayerTypeAL1Framed,
	kSMH223muxAdaptationLayerTypeAL1NotFramed,
	kSMH223muxAdaptationLayerTypeAL2WithoutSequenceNumbers,
	kSMH223muxAdaptationLayerTypeAL2WithSequenceNumbers,
	kSMH223muxAdaptationLayerTypeAL3,
};

enum kSMH223muxReconfigLevelChangeDirection {
	kSMH223muxReconfigLevelChangeDirectionTransmit,
	kSMH223muxReconfigLevelChangeDirectionReceive,
};

enum kSMH223muxReconfigChoice {
	kSMH223muxReconfigModeChange,
	kSMH223muxReconfigAnnexADoubleFlag,
};

enum kSMH223muxReconfigModeChange {
	kSMH223muxReconfigModeChangeToLevel0,
	kSMH223muxReconfigModeChangeToLevel1,
	kSMH223muxReconfigModeChangeToLevel2,
	kSMH223muxReconfigModeChangeToLevel2WithOptionalHeader,
};

enum kSMH223muxReconfigAnnexADoubleFlag {
	kSMH223muxReconfigAnnexADoubleFlagStart,
	kSMH223muxReconfigAnnexADoubleFlagStop,
};

enum kSMH223muxSetCapsRole {
	kSMH223muxSetCapsRoleLocal,
	kSMH223muxSetCapsRoleRemote,
};

enum kSMH223muxStatus {
	kSMH223muxStatusRunning,
	kSMH223muxStatusStopped,
	kSMH223muxStatusSessionStarted,
	kSMH223muxStatusCreateSessionFailed,
	kSMH223muxStatusDeleteSessionFailed,
	kSMH223muxStatusSetCapsFailed,
	kSMH223muxStatusAddEntryFailed,
	kSMH223muxStatusOpenLCFailed,
	kSMH223muxStatusCloseLCFailed,
	kSMH223muxStatusReconfigLevelChange,
	kSMH223muxStatusReconfigFailed,
	kSMH223muxStatusInternalError,
	kSMH223muxStatusVideoPFrameDrop,
	kSMH223muxStatusVideoPFrameSend,
};



typedef struct sm_h223mux_add_entry_parms {
	tSMH223muxId h223mux;
	enum kSMH223muxTableEntryDirection direction;
	tSM_INT entry_number;
	tSM_INT descriptor_len;
	char *descriptor;
} SM_H223MUX_ADD_ENTRY_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_h223mux_add_entry sm_h223mux_add_entry_iPsHMte1_2_iiPc_
#define SM_H223MUX_ADD_ENTRY_NAME "sm_h223mux_add_entry_iPsHMte1_2_iiPc_"
#endif
typedef int SM_H223MUX_ADD_ENTRY_FN(struct sm_h223mux_add_entry_parms *entryp);

ACUAPI int ACUTiNG_WINAPI sm_h223mux_add_entry(struct sm_h223mux_add_entry_parms *entryp);


typedef struct sm_h223mux_close_lc_parms {
	tSMH223muxId h223mux;
	enum kSMH223muxChannelDirection direction;
	tSM_INT lcn;
} SM_H223MUX_CLOSE_LC_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_h223mux_close_lc sm_h223mux_close_lc_iPsHMte1_2_i_
#define SM_H223MUX_CLOSE_LC_NAME "sm_h223mux_close_lc_iPsHMte1_2_i_"
#endif
typedef int SM_H223MUX_CLOSE_LC_FN(struct sm_h223mux_close_lc_parms *close_lcp);

ACUAPI int ACUTiNG_WINAPI sm_h223mux_close_lc(struct sm_h223mux_close_lc_parms *close_lcp);


typedef struct sm_h223mux_create_parms {
	tSMH223muxId h223mux;
	tSMModuleId module;
} SM_H223MUX_CREATE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_h223mux_create sm_h223mux_create_iPsHMtMt_
#define SM_H223MUX_CREATE_NAME "sm_h223mux_create_iPsHMtMt_"
#endif
typedef int SM_H223MUX_CREATE_FN(struct sm_h223mux_create_parms *createp);

ACUAPI int ACUTiNG_WINAPI sm_h223mux_create(struct sm_h223mux_create_parms *createp);


typedef struct sm_h223mux_datafeed_connect_parms {
	tSMDatafeedId datafeed;
	enum kSMH223muxInputType port;
	tSMH223muxId h223mux;
} SM_H223MUX_DATAFEED_CONNECT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_h223mux_datafeed_connect sm_h223mux_datafeed_connect_iPsDte1_4_HMt_
#define SM_H223MUX_DATAFEED_CONNECT_NAME "sm_h223mux_datafeed_connect_iPsDte1_4_HMt_"
#endif
typedef int SM_H223MUX_DATAFEED_CONNECT_FN(struct sm_h223mux_datafeed_connect_parms *datafeedp);

ACUAPI int ACUTiNG_WINAPI sm_h223mux_datafeed_connect(struct sm_h223mux_datafeed_connect_parms *datafeedp);


typedef struct sm_h223mux_delete_session_parms {
	tSMH223muxId h223mux;
} SM_H223MUX_DELETE_SESSION_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_h223mux_delete_session sm_h223mux_delete_session_iPsHMt_
#define SM_H223MUX_DELETE_SESSION_NAME "sm_h223mux_delete_session_iPsHMt_"
#endif
typedef int SM_H223MUX_DELETE_SESSION_FN(struct sm_h223mux_delete_session_parms *sessionp);

ACUAPI int ACUTiNG_WINAPI sm_h223mux_delete_session(struct sm_h223mux_delete_session_parms *sessionp);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_h223mux_destroy sm_h223mux_destroy_iHMt
#define SM_H223MUX_DESTROY_NAME "sm_h223mux_destroy_iHMt"
#endif
typedef int SM_H223MUX_DESTROY_FN(tSMH223muxId mux);

ACUAPI int ACUTiNG_WINAPI sm_h223mux_destroy(tSMH223muxId mux);


typedef struct sm_h223mux_datafeed_parms {
	tSMH223muxId h223mux;
	enum kSMH223muxOutputType port;
	tSMDatafeedId datafeed;
} SM_H223MUX_DATAFEED_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_h223mux_get_datafeed sm_h223mux_get_datafeed_iPsHMte1_4_Dt_
#define SM_H223MUX_GET_DATAFEED_NAME "sm_h223mux_get_datafeed_iPsHMte1_4_Dt_"
#endif
typedef int SM_H223MUX_GET_DATAFEED_FN(struct sm_h223mux_datafeed_parms *datafeedp);

ACUAPI int ACUTiNG_WINAPI sm_h223mux_get_datafeed(struct sm_h223mux_datafeed_parms *datafeedp);


typedef struct sm_h223mux_event_parms {
	tSMH223muxId h223mux;
	tSMEventId event;
} SM_H223MUX_EVENT_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_h223mux_get_event sm_h223mux_get_event_iPsHMtEt_
#define SM_H223MUX_GET_EVENT_NAME "sm_h223mux_get_event_iPsHMtEt_"
#endif
typedef int SM_H223MUX_GET_EVENT_FN(struct sm_h223mux_event_parms *eventp);

ACUAPI int ACUTiNG_WINAPI sm_h223mux_get_event(struct sm_h223mux_event_parms *eventp);


typedef struct sm_h223mux_new_session_parms {
	tSMH223muxId h223mux;
	tSM_INT level;
	enum kSMH223muxTransmissionBitOrder bit_order;
} SM_H223MUX_NEW_SESSION_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_h223mux_new_session sm_h223mux_new_session_iPsHMtie1_2__
#define SM_H223MUX_NEW_SESSION_NAME "sm_h223mux_new_session_iPsHMtie1_2__"
#endif
typedef int SM_H223MUX_NEW_SESSION_FN(struct sm_h223mux_new_session_parms *sessionp);

ACUAPI int ACUTiNG_WINAPI sm_h223mux_new_session(struct sm_h223mux_new_session_parms *sessionp);


typedef struct sm_h223mux_open_lc_parms {
	tSMH223muxId h223mux;
	tSM_UT32 segmentable_flag;
	enum kSMH223muxLogicalChannelType type;
	struct {
		tSM_INT lcn;
		enum kSMH223muxLogicalChannelDirection direction;
		struct {
			enum kSMH223muxMarkUnit mark_unit;
			tSM_INT hi_mark;
			tSM_INT lo_mark;
		} transmit_lc;
		enum kSMH223muxAdaptationLayerType adaptation_layer_type;
		union {
			struct {
				tSM_INT control_field_octets;
				tSM_INT send_buffer_size;
			} al3;
		} u_al;
	} forward_channel;
	struct {
		tSM_INT lcn;
		enum kSMH223muxLogicalChannelDirection direction;
		struct {
			enum kSMH223muxMarkUnit mark_unit;
			tSM_INT hi_mark;
			tSM_INT lo_mark;
		} transmit_lc;
		enum kSMH223muxAdaptationLayerType adaptation_layer_type;
		union {
			struct {
				tSM_INT control_field_octets;
				tSM_INT send_buffer_size;
			} al3;
		} u_al;
	} reverse_channel;
} SM_H223MUX_OPEN_LC_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_h223mux_open_lc sm_h223mux_open_lc_iPsHMtUie1_2_sie1_2_se1_2_ii_e1_6_usii___sie1_2_se1_2_ii_e1_6_usii____
#define SM_H223MUX_OPEN_LC_NAME "sm_h223mux_open_lc_iPsHMtUie1_2_sie1_2_se1_2_ii_e1_6_usii___sie1_2_se1_2_ii_e1_6_usii____"
#endif
typedef int SM_H223MUX_OPEN_LC_FN(struct sm_h223mux_open_lc_parms *open_lcp);

ACUAPI int ACUTiNG_WINAPI sm_h223mux_open_lc(struct sm_h223mux_open_lc_parms *open_lcp);


typedef struct sm_h223mux_reconfig_parms {
	tSMH223muxId h223mux;
	enum kSMH223muxReconfigLevelChangeDirection direction;
	struct {
		enum kSMH223muxReconfigChoice choice;
		union {
			enum kSMH223muxReconfigModeChange mode_change;
			enum kSMH223muxReconfigAnnexADoubleFlag annex_a_double_flag;
		} u;
	} multiplex_reconfiguration;
} SM_H223MUX_RECONFIG_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_h223mux_reconfig sm_h223mux_reconfig_iPsHMte1_2_se1_2_ue1_4_e1_2____
#define SM_H223MUX_RECONFIG_NAME "sm_h223mux_reconfig_iPsHMte1_2_se1_2_ue1_4_e1_2____"
#endif
typedef int SM_H223MUX_RECONFIG_FN(struct sm_h223mux_reconfig_parms *reconfigp);

ACUAPI int ACUTiNG_WINAPI sm_h223mux_reconfig(struct sm_h223mux_reconfig_parms *reconfigp);


typedef struct sm_h223mux_set_caps_parms {
	tSMH223muxId h223mux;
	enum kSMH223muxSetCapsRole role;
	tSM_INT optional_ext;
	tSM_UT32 transport_with_i_frames;
	tSM_UT32 video_with_AL1;
	tSM_UT32 video_with_AL2;
	tSM_UT32 video_with_AL3;
	tSM_UT32 audio_with_AL1;
	tSM_UT32 audio_with_AL2;
	tSM_UT32 audio_with_AL3;
	tSM_UT32 data_with_AL1;
	tSM_UT32 data_with_AL2;
	tSM_UT32 data_with_AL3;
	tSM_INT maximum_AL2_sdu_size;
	tSM_INT maximum_AL3_sdu_size;
	tSM_INT maximum_delay_jitter;
	tSM_UT32 max_MUX_PDU_size_capability;
	tSM_UT32 nsrp_support;
	tSM_INT bit_rate_o;
	struct {
		tSM_UT32 mode_change_capability;
		tSM_UT32 h223_AnnexA;
		tSM_UT32 h223_AnnexA_double_flag;
		tSM_UT32 h223_AnnexB;
		tSM_UT32 h223_AnnexB_with_header;
	} mobile_operation_transmit_capability_o;
	struct {
		tSM_INT maximum_sample_size;
		tSM_INT maximum_payload_length;
	} mobile_multilink_frame_capability_o;
	struct {
		tSM_UT32 video_with_AL1M;
		tSM_UT32 video_with_AL2M;
		tSM_UT32 video_with_AL3M;
		tSM_UT32 audio_with_AL1M;
		tSM_UT32 audio_with_AL2M;
		tSM_UT32 audio_with_AL3M;
		tSM_UT32 data_with_AL1M;
		tSM_UT32 data_with_AL2M;
		tSM_UT32 data_with_AL3M;
		tSM_UT32 alpdu_interleaving;
		tSM_INT maximum_AL1M_pdu_size;
		tSM_INT maximum_AL2M_pdu_size;
		tSM_INT maximum_AL3M_pdu_size;
		tSM_UT32 rs_code_capability;
	} H223_AnnexC_capability_o;
} SM_H223MUX_SET_CAPS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_h223mux_set_caps sm_h223mux_set_caps_iPsHMte1_2_iUiUiUiUiUiUiUiUiUiUiiiiUiUiisUiUiUiUiUi_sii_sUiUiUiUiUiUiUiUiUiUiiiiUi__
#define SM_H223MUX_SET_CAPS_NAME "sm_h223mux_set_caps_iPsHMte1_2_iUiUiUiUiUiUiUiUiUiUiiiiUiUiisUiUiUiUiUi_sii_sUiUiUiUiUiUiUiUiUiUiiiiUi__"
#endif
typedef int SM_H223MUX_SET_CAPS_FN(struct sm_h223mux_set_caps_parms *capsp);

ACUAPI int ACUTiNG_WINAPI sm_h223mux_set_caps(struct sm_h223mux_set_caps_parms *capsp);


typedef struct sm_h223mux_status_parms {
	tSMH223muxId h223mux;
	enum kSMH223muxStatus status;
	union {
		struct {
			tSM_INT level;
		} session_transport;
		struct {
			tSM_INT num_errors;
		} internal_error;
	} u;
} SM_H223MUX_STATUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_h223mux_status sm_h223mux_status_iPsHMte1_14_usi_si___
#define SM_H223MUX_STATUS_NAME "sm_h223mux_status_iPsHMte1_14_usi_si___"
#endif
typedef int SM_H223MUX_STATUS_FN(struct sm_h223mux_status_parms *statusp);

ACUAPI int ACUTiNG_WINAPI sm_h223mux_status(struct sm_h223mux_status_parms *statusp);


typedef struct sm_h223mux_stop_parms {
	tSMH223muxId h223mux;
} SM_H223MUX_STOP_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_h223mux_stop sm_h223mux_stop_iPsHMt_
#define SM_H223MUX_STOP_NAME "sm_h223mux_stop_iPsHMt_"
#endif
typedef int SM_H223MUX_STOP_FN(struct sm_h223mux_stop_parms *stopp);

ACUAPI int ACUTiNG_WINAPI sm_h223mux_stop(struct sm_h223mux_stop_parms *stopp);



#endif
