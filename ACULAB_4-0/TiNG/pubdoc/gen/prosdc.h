/* prosdc.h - Prosody data communications API declarations */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

#ifndef INCLUDED_PUBDOC_PROSDC_H
#define INCLUDED_PUBDOC_PROSDC_H

#ifdef ACULAB_TiNG_VERSION_NUM
#if ACULAB_TiNG_VERSION_NUM != 1504
#error ACULAB_TiNG_VERSION_NUM does not match 1504
#endif
#else
#define ACULAB_TiNG_VERSION_NUM 1504
#define ACULAB_TiNG_VERSION_CHANGED 
#define ACULAB_TiNG_VERSION_STRING "m1504."
#endif

enum kSMDCLineCtlCmd {
	kSMDCLineCtlCmdInitiatorConnect,
	kSMDCLineCtlCmdResponderConnect,
	kSMDCLineCtlCmdDisconnect,
	kSMDCLineCtlCmdAssertRTS,
	kSMDCLineCtlCmdUnassertRTS,
	kSMDCLineCtlCmdSetAux,
	kSMDCLineCtlCmdSetPrefixSuffix,
	kSMDCLineCtlCmdSuspend,
	kSMDCLineCtlCmdResume,
	kSMDCLineCtlCmdResumeMonitorCarrier,
	kSMDCLineCtlCmdRetrain,
};

enum kSMDCV34LineCtlCmd {
	kSMDCV34LineCtlCmdRetrain,
	kSMDCV34LineCtlCmdRenegotiateUp,
	kSMDCV34LineCtlCmdRenegotiateDown,
	kSMDCV34LineCtlCmdEnablePrimary,
	kSMDCV34LineCtlCmdEnableControl,
	kSMDCV34LineCtlCmdBitrateTx,
	kSMDCV34LineCtlCmdBitrateRx,
};

enum kSMDCLinkStatus {
	kSMDCLinkStatusNotConnected,
	kSMDCLinkStatusConnecting,
	kSMDCLinkStatusConnected,
	kSMDCLinkStatusDisconnecting,
};

enum kSMDCLineRxStatus {
	kSMDCRxStatusNoCarrier,
	kSMDCRxStatusCarrierPresent,
	kSMDCRxStatusReceivingData,
};

enum kSMDCLineTxStatus {
	kSMDCTxStatusSilent,
	kSMDCTxStatusSendingCarrier,
	kSMDCTxStatusSendingData,
};

enum kSMDCV34Duplex {
	kSMDCV34HalfDuplex,
	kSMDCV34FullDuplex,
};

enum kSMDCRxCtl {
	kSMDCRxCtlDiscard,
	kSMDCRxCtlNotifyOnData,
	kSMDCRxCtlStop,
	kSMDCRxCtlResume,
};

enum kSMDCRxStatus {
	kSMDCRxStatusEmpty,
	kSMDCRxStatusData,
	kSMDCRxStatusOverrun,
	kSMDCRxStatusCarrier,
	kSMDCRxStatusFinished,
	kSMDCRxStatusSigMetric,
};

enum kSMDCRxT38Status {
	kSMDCRxT38StatusEmpty,
	kSMDCRxT38StatusTone,
	kSMDCRxT38StatusV34,
	kSMDCRxT38StatusData,
	kSMDCRxT38StatusOverrun,
	kSMDCRxT38StatusCarrier,
	kSMDCRxT38StatusFinished,
	kSMDCRxT38StatusProtocol,
	kSMDCRxT38StatusRate,
};

enum kSMDCT38ToneType {
	kSMDCT38ToneTypeCNG,
	kSMDCT38ToneTypeCED,
	kSMDCT38ToneTypeANSAM,
};

enum kSMDCT38_V34Type {
	kSMDCT38_V34Type_V34_CC_1200,
	kSMDCT38_V34Type_V34_PRI_CHAN,
	kSMDCT38_V34Type_V34_CC_RETRAIN,
};

enum kSMDCV34Mode {
	kSMDCV34ModeOriginating,
	kSMDCV34ModeAnswering,
};

enum kSMDCV34Polling {
	kSMDCV34ModeNormal,
	kSMDCV34ModePolling,
};

enum kSMDCTxCtl {
	kSMDCTxCtlDiscard,
	kSMDCTxCtlNotifyOnCapacity,
	kSMDCTxCtlNotifyOnEmpty,
	kSMDCTxCtlNoNotifyOnEmpty,
	kSMDCTxCtlFinish,
};

enum kSMDCTxStatus {
	kSMDCTxStatusEmpty,
	kSMDCTxStatusHasCapacity,
	kSMDCTxStatusNoCapacity,
	kSMDCTxStatusUnderrun,
	kSMDCTxStatusFinishing,
	kSMDCTxStatusFinished,
};



typedef struct smdc_channel_config_parms {
	tSMChannelId channel;
	tSM_INT protocol;
	tSM_INT config_length;
	void *config_data;
	tSM_INT encoding;
	tSM_INT encoding_config_length;
	void *encoding_config_data;
} SMDC_CHANNEL_CONFIG_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define smdc_channel_config smdc_channel_config_iPsCtiiPviiPv_
#define SMDC_CHANNEL_CONFIG_NAME "smdc_channel_config_iPsCtiiPviiPv_"
#endif
typedef int SMDC_CHANNEL_CONFIG_FN(struct smdc_channel_config_parms *configp);

ACUAPI int ACUTiNG_WINAPI smdc_channel_config(struct smdc_channel_config_parms *configp);


typedef struct smdc_line_control_parms {
	tSMChannelId channel;
	enum kSMDCLineCtlCmd cmd;
	tSM_UT32 aux_mask;
	tSM_UT32 aux_toggle;
	tSM_UT32 shorttrain;
	tSM_INT tep;
	tSM_UT32 prefix_bitlength;
	tSM_UT32 *prefix_data;
	tSM_UT32 suffix_bitlength;
	tSM_UT32 *suffix_data;
	tSM_UT32 retrain_speed;
} SMDC_LINE_CONTROL_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define smdc_line_control smdc_line_control_iPsCte1_11_UiUiUiiUiPUiUiPUiUi_
#define SMDC_LINE_CONTROL_NAME "smdc_line_control_iPsCte1_11_UiUiUiiUiPUiUiPUiUi_"
#endif
typedef int SMDC_LINE_CONTROL_FN(struct smdc_line_control_parms *controlp);

ACUAPI int ACUTiNG_WINAPI smdc_line_control(struct smdc_line_control_parms *controlp);


typedef struct smdc_line_control_v34_parms {
	tSMChannelId channel;
	enum kSMDCV34LineCtlCmd cmd;
	union {
		struct {
			tSM_INT min_bitrate;
			tSM_INT max_bitrate;
		} bitrate;
	} u;
} SMDC_LINE_CONTROL_V34_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define smdc_line_control_v34 smdc_line_control_v34_iPsCte1_7_usii___
#define SMDC_LINE_CONTROL_V34_NAME "smdc_line_control_v34_iPsCte1_7_usii___"
#endif
typedef int SMDC_LINE_CONTROL_V34_FN(struct smdc_line_control_v34_parms *controlp);

ACUAPI int ACUTiNG_WINAPI smdc_line_control_v34(struct smdc_line_control_v34_parms *controlp);


typedef struct smdc_line_status_parms {
	tSMChannelId channel;
	enum kSMDCLinkStatus link_status;
	enum kSMDCLineRxStatus rx_status;
	enum kSMDCLineTxStatus tx_status;
	tSM_INT tx_cts;
} SMDC_LINE_STATUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define smdc_line_status smdc_line_status_iPsCte1_4_e1_3_e1_3_i_
#define SMDC_LINE_STATUS_NAME "smdc_line_status_iPsCte1_4_e1_3_e1_3_i_"
#endif
typedef int SMDC_LINE_STATUS_FN(struct smdc_line_status_parms *statusp);

ACUAPI int ACUTiNG_WINAPI smdc_line_status(struct smdc_line_status_parms *statusp);


typedef struct smdc_line_status_v34_parms {
	tSMChannelId channel;
	enum kSMDCLinkStatus link_status;
	enum kSMDCLineRxStatus rx_status;
	enum kSMDCLineTxStatus tx_status;
	tSM_INT control_bitrate;
	tSM_INT primary_bitrate;
	tSM_INT snr_status;
	tSM_INT fdx_rx_bitrate;
	tSM_INT fdx_tx_bitrate;
} SMDC_LINE_STATUS_V34_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define smdc_line_status_v34 smdc_line_status_v34_iPsCte1_4_e1_3_e1_3_iiiii_
#define SMDC_LINE_STATUS_V34_NAME "smdc_line_status_v34_iPsCte1_4_e1_3_e1_3_iiiii_"
#endif
typedef int SMDC_LINE_STATUS_V34_FN(struct smdc_line_status_v34_parms *statusp);

ACUAPI int ACUTiNG_WINAPI smdc_line_status_v34(struct smdc_line_status_v34_parms *statusp);


typedef struct smdc_rx_config_t38_parms {
	tSMChannelId channel;
	tSM_INT PreCorrigendum;
	tSM_INT Protocol;
} SMDC_RX_CONFIG_T38_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define smdc_rx_config_t38 smdc_rx_config_t38_iPsCtii_
#define SMDC_RX_CONFIG_T38_NAME "smdc_rx_config_t38_iPsCtii_"
#endif
typedef int SMDC_RX_CONFIG_T38_FN(struct smdc_rx_config_t38_parms *configp);

ACUAPI int ACUTiNG_WINAPI smdc_rx_config_t38(struct smdc_rx_config_t38_parms *configp);


typedef struct smdc_rx_config_v34_parms {
	tSMChannelId channel;
	enum kSMDCV34Duplex duplex;
	tSM_INT min_bitrate;
	tSM_INT max_bitrate;
	tSM_INT v8fsk_metric;
} SMDC_RX_CONFIG_V34_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define smdc_rx_config_v34 smdc_rx_config_v34_iPsCte1_2_iii_
#define SMDC_RX_CONFIG_V34_NAME "smdc_rx_config_v34_iPsCte1_2_iii_"
#endif
typedef int SMDC_RX_CONFIG_V34_FN(struct smdc_rx_config_v34_parms *configp);

ACUAPI int ACUTiNG_WINAPI smdc_rx_config_v34(struct smdc_rx_config_v34_parms *configp);


typedef struct smdc_rx_control_parms {
	tSMChannelId channel;
	enum kSMDCRxCtl cmd;
	tSM_INT min_to_collect;
	tSM_INT min_idle;
	tSM_INT blocking;
} SMDC_RX_CONTROL_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define smdc_rx_control smdc_rx_control_iPsCte1_4_iii_
#define SMDC_RX_CONTROL_NAME "smdc_rx_control_iPsCte1_4_iii_"
#endif
typedef int SMDC_RX_CONTROL_FN(struct smdc_rx_control_parms *controlp);

ACUAPI int ACUTiNG_WINAPI smdc_rx_control(struct smdc_rx_control_parms *controlp);


typedef struct smdc_data_parms {
	tSMChannelId channel;
	char *data;
	tSM_INT max_length;
	tSM_INT done_length;
} SMDC_DATA_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define smdc_rx_data smdc_rx_data_iPsCtPcii_
#define SMDC_RX_DATA_NAME "smdc_rx_data_iPsCtPcii_"
#endif
typedef int SMDC_RX_DATA_FN(struct smdc_data_parms *datap);

ACUAPI int ACUTiNG_WINAPI smdc_rx_data(struct smdc_data_parms *datap);


typedef struct smdc_encoding_parms {
	tSMChannelId channel;
	tSM_INT encoding;
	tSM_INT encoding_config_length;
	void *encoding_config_data;
} SMDC_ENCODING_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define smdc_rx_encoding smdc_rx_encoding_iPsCtiiPv_
#define SMDC_RX_ENCODING_NAME "smdc_rx_encoding_iPsCtiiPv_"
#endif
typedef int SMDC_RX_ENCODING_FN(struct smdc_encoding_parms *encp);

ACUAPI int ACUTiNG_WINAPI smdc_rx_encoding(struct smdc_encoding_parms *encp);


typedef struct smdc_rx_status_parms {
	tSMChannelId channel;
	tSM_INT available_octets;
	enum kSMDCRxStatus status;
	tSM_INT flow;
	union {
		struct {
			tSM_INT carrier;
		} carrier;
		struct {
			tSM_INT metric;
		} metric;
	} u;
} SMDC_RX_STATUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define smdc_rx_status smdc_rx_status_iPsCtie1_6_iusi_si___
#define SMDC_RX_STATUS_NAME "smdc_rx_status_iPsCtie1_6_iusi_si___"
#endif
typedef int SMDC_RX_STATUS_FN(struct smdc_rx_status_parms *statusp);

ACUAPI int ACUTiNG_WINAPI smdc_rx_status(struct smdc_rx_status_parms *statusp);


typedef struct smdc_rx_status_t38_parms {
	tSMChannelId channel;
	tSM_INT available_octets;
	enum kSMDCRxT38Status status;
	tSM_INT flow;
	union {
		struct {
			enum kSMDCT38ToneType tone_type;
		} tone;
		struct {
			enum kSMDCT38_V34Type v34_type;
		} v34;
		struct {
			tSM_INT carrier;
		} carrier;
		struct {
			tSM_INT rate;
		} rate;
		struct {
			tSM_INT protocol;
		} protocol;
	} u;
} SMDC_RX_STATUS_T38_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define smdc_rx_status_t38 smdc_rx_status_t38_iPsCtie1_9_iuse1_3__se1_3__si_si_si___
#define SMDC_RX_STATUS_T38_NAME "smdc_rx_status_t38_iPsCtie1_9_iuse1_3__se1_3__si_si_si___"
#endif
typedef int SMDC_RX_STATUS_T38_FN(struct smdc_rx_status_t38_parms *statusp);

ACUAPI int ACUTiNG_WINAPI smdc_rx_status_t38(struct smdc_rx_status_t38_parms *statusp);


typedef struct smdc_stop_parms {
	tSMChannelId channel;
} SMDC_STOP_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define smdc_rx_stop smdc_rx_stop_iPsCt_
#define SMDC_RX_STOP_NAME "smdc_rx_stop_iPsCt_"
#endif
typedef int SMDC_RX_STOP_FN(struct smdc_stop_parms *pp);

ACUAPI int ACUTiNG_WINAPI smdc_rx_stop(struct smdc_stop_parms *pp);



#ifndef TiNG_USE_UNDECORATED_NAMES
#define smdc_stop smdc_stop_iPsCt_
#define SMDC_STOP_NAME "smdc_stop_iPsCt_"
#endif
typedef int SMDC_STOP_FN(struct smdc_stop_parms *pp);

ACUAPI int ACUTiNG_WINAPI smdc_stop(struct smdc_stop_parms *pp);


typedef struct smdc_tx_config_t38_parms {
	tSMChannelId channel;
	tSM_INT PreCorrigendum;
	tSM_INT Protocol;
	tSM_UT32 max_packet_length;
} SMDC_TX_CONFIG_T38_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define smdc_tx_config_t38 smdc_tx_config_t38_iPsCtiiUi_
#define SMDC_TX_CONFIG_T38_NAME "smdc_tx_config_t38_iPsCtiiUi_"
#endif
typedef int SMDC_TX_CONFIG_T38_FN(struct smdc_tx_config_t38_parms *configp);

ACUAPI int ACUTiNG_WINAPI smdc_tx_config_t38(struct smdc_tx_config_t38_parms *configp);


typedef struct smdc_tx_config_v34_parms {
	tSMChannelId channel;
	enum kSMDCV34Mode mode;
	enum kSMDCV34Polling polling;
	enum kSMDCV34Duplex duplex;
	tSM_INT min_bitrate;
	tSM_INT max_bitrate;
} SMDC_TX_CONFIG_V34_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define smdc_tx_config_v34 smdc_tx_config_v34_iPsCte1_2_e1_2_e1_2_ii_
#define SMDC_TX_CONFIG_V34_NAME "smdc_tx_config_v34_iPsCte1_2_e1_2_e1_2_ii_"
#endif
typedef int SMDC_TX_CONFIG_V34_FN(struct smdc_tx_config_v34_parms *configp);

ACUAPI int ACUTiNG_WINAPI smdc_tx_config_v34(struct smdc_tx_config_v34_parms *configp);


typedef struct smdc_tx_control_parms {
	tSMChannelId channel;
	enum kSMDCTxCtl cmd;
	tSM_INT capacity;
	tSM_INT blocking;
} SMDC_TX_CONTROL_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define smdc_tx_control smdc_tx_control_iPsCte1_5_ii_
#define SMDC_TX_CONTROL_NAME "smdc_tx_control_iPsCte1_5_ii_"
#endif
typedef int SMDC_TX_CONTROL_FN(struct smdc_tx_control_parms *controlp);

ACUAPI int ACUTiNG_WINAPI smdc_tx_control(struct smdc_tx_control_parms *controlp);



#ifndef TiNG_USE_UNDECORATED_NAMES
#define smdc_tx_data smdc_tx_data_iPsCtPcii_
#define SMDC_TX_DATA_NAME "smdc_tx_data_iPsCtPcii_"
#endif
typedef int SMDC_TX_DATA_FN(struct smdc_data_parms *datap);

ACUAPI int ACUTiNG_WINAPI smdc_tx_data(struct smdc_data_parms *datap);



#ifndef TiNG_USE_UNDECORATED_NAMES
#define smdc_tx_encoding smdc_tx_encoding_iPsCtiiPv_
#define SMDC_TX_ENCODING_NAME "smdc_tx_encoding_iPsCtiiPv_"
#endif
typedef int SMDC_TX_ENCODING_FN(struct smdc_encoding_parms *encp);

ACUAPI int ACUTiNG_WINAPI smdc_tx_encoding(struct smdc_encoding_parms *encp);


typedef struct smdc_tx_status_parms {
	tSMChannelId channel;
	tSM_INT capacity;
	enum kSMDCTxStatus status;
	tSM_INT flow;
} SMDC_TX_STATUS_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define smdc_tx_status smdc_tx_status_iPsCtie1_6_i_
#define SMDC_TX_STATUS_NAME "smdc_tx_status_iPsCtie1_6_i_"
#endif
typedef int SMDC_TX_STATUS_FN(struct smdc_tx_status_parms *statusp);

ACUAPI int ACUTiNG_WINAPI smdc_tx_status(struct smdc_tx_status_parms *statusp);



#ifndef TiNG_USE_UNDECORATED_NAMES
#define smdc_tx_stop smdc_tx_stop_iPsCt_
#define SMDC_TX_STOP_NAME "smdc_tx_stop_iPsCt_"
#endif
typedef int SMDC_TX_STOP_FN(struct smdc_stop_parms *pp);

ACUAPI int ACUTiNG_WINAPI smdc_tx_stop(struct smdc_stop_parms *pp);


typedef struct smdc_tx_tone_indicator_t38_parms {
	tSMChannelId channel;
	enum kSMDCT38ToneType tone_type;
} SMDC_TX_TONE_INDICATOR_T38_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define smdc_tx_tone_indicator_t38 smdc_tx_tone_indicator_t38_iPsCte1_3__
#define SMDC_TX_TONE_INDICATOR_T38_NAME "smdc_tx_tone_indicator_t38_iPsCte1_3__"
#endif
typedef int SMDC_TX_TONE_INDICATOR_T38_FN(struct smdc_tx_tone_indicator_t38_parms *tonep);

ACUAPI int ACUTiNG_WINAPI smdc_tx_tone_indicator_t38(struct smdc_tx_tone_indicator_t38_parms *tonep);


typedef struct smdc_tx_v34_indicator_t38_parms {
	tSMChannelId channel;
	enum kSMDCT38_V34Type v34_type;
} SMDC_TX_V34_INDICATOR_T38_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define smdc_tx_v34_indicator_t38 smdc_tx_v34_indicator_t38_iPsCte1_3__
#define SMDC_TX_V34_INDICATOR_T38_NAME "smdc_tx_v34_indicator_t38_iPsCte1_3__"
#endif
typedef int SMDC_TX_V34_INDICATOR_T38_FN(struct smdc_tx_v34_indicator_t38_parms *tonep);

ACUAPI int ACUTiNG_WINAPI smdc_tx_v34_indicator_t38(struct smdc_tx_v34_indicator_t38_parms *tonep);


typedef struct smdc_tx_v34_line_control_t38_parms {
	tSMChannelId channel;
	tSM_UT32 primary_bitrate;
} SMDC_TX_V34_LINE_CONTROL_T38_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define smdc_tx_v34_line_control_t38 smdc_tx_v34_line_control_t38_iPsCtUi_
#define SMDC_TX_V34_LINE_CONTROL_T38_NAME "smdc_tx_v34_line_control_t38_iPsCtUi_"
#endif
typedef int SMDC_TX_V34_LINE_CONTROL_T38_FN(struct smdc_tx_v34_line_control_t38_parms *configp);

ACUAPI int ACUTiNG_WINAPI smdc_tx_v34_line_control_t38(struct smdc_tx_v34_line_control_t38_parms *configp);



#endif
