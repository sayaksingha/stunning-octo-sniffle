private:
	class Config {
		mutable int used_;
		SMDC_CHANNEL_CONFIG_PARMS configp_;
	public:
		Config(const Config &old) {
			*this = old;
			old.used_ = 1;
		};
		Config() : used_(0) {
			memset(&configp_, 0, sizeof(configp_));
		};
		operator Error() {
			Error e(Prosody::smdc_channel_config(&configp_));
			used_ = 1;
			return e;
		};
		~Config() {
			if (!used_) {
				fprintf(stderr, "Prosody function smdc_channel_config not called\n");
				abort();
			};
		};
		Config &channel(tSMChannelId v) {
			configp_.channel = v;
			return *this;
		};
		Config &protocol(tSM_INT v) {
			configp_.protocol = v;
			return *this;
		};
		Config &config_length(tSM_INT v) {
			configp_.config_length = v;
			return *this;
		};
		Config &config_data(void* v) {
			configp_.config_data = v;
			return *this;
		};
		Config &encoding(tSM_INT v) {
			configp_.encoding = v;
			return *this;
		};
		Config &encoding_config_length(tSM_INT v) {
			configp_.encoding_config_length = v;
			return *this;
		};
		Config &encoding_config_data(void* v) {
			configp_.encoding_config_data = v;
			return *this;
		};
	};
public:
	Config config() {
		return Config().channel(channel_);
	}
private:
	class RxConfigV34 {
		mutable int used_;
		SMDC_RX_CONFIG_V34_PARMS configp_;
	public:
		RxConfigV34(const RxConfigV34 &old) {
			*this = old;
			old.used_ = 1;
		};
		RxConfigV34() : used_(0) {
			memset(&configp_, 0, sizeof(configp_));
		};
		operator Error() {
			Error e(Prosody::smdc_rx_config_v34(&configp_));
			used_ = 1;
			return e;
		};
		~RxConfigV34() {
			if (!used_) {
				fprintf(stderr, "Prosody function smdc_rx_config_v34 not called\n");
				abort();
			};
		};
		RxConfigV34 &channel(tSMChannelId v) {
			configp_.channel = v;
			return *this;
		};
		RxConfigV34 &duplex(enum kSMDCV34Duplex v) {
			configp_.duplex = v;
			return *this;
		};
		RxConfigV34 &min_bitrate(tSM_INT v) {
			configp_.min_bitrate = v;
			return *this;
		};
		RxConfigV34 &max_bitrate(tSM_INT v) {
			configp_.max_bitrate = v;
			return *this;
		};
		RxConfigV34 &v8fsk_metric(tSM_INT v) {
			configp_.v8fsk_metric = v;
			return *this;
		};
	};
public:
	RxConfigV34 rx_config_v34() {
		return RxConfigV34().channel(channel_);
	}
private:
	class TxConfigV34 {
		mutable int used_;
		SMDC_TX_CONFIG_V34_PARMS configp_;
	public:
		TxConfigV34(const TxConfigV34 &old) {
			*this = old;
			old.used_ = 1;
		};
		TxConfigV34() : used_(0) {
			memset(&configp_, 0, sizeof(configp_));
		};
		operator Error() {
			Error e(Prosody::smdc_tx_config_v34(&configp_));
			used_ = 1;
			return e;
		};
		~TxConfigV34() {
			if (!used_) {
				fprintf(stderr, "Prosody function smdc_tx_config_v34 not called\n");
				abort();
			};
		};
		TxConfigV34 &channel(tSMChannelId v) {
			configp_.channel = v;
			return *this;
		};
		TxConfigV34 &mode(enum kSMDCV34Mode v) {
			configp_.mode = v;
			return *this;
		};
		TxConfigV34 &polling(enum kSMDCV34Polling v) {
			configp_.polling = v;
			return *this;
		};
		TxConfigV34 &duplex(enum kSMDCV34Duplex v) {
			configp_.duplex = v;
			return *this;
		};
		TxConfigV34 &min_bitrate(tSM_INT v) {
			configp_.min_bitrate = v;
			return *this;
		};
		TxConfigV34 &max_bitrate(tSM_INT v) {
			configp_.max_bitrate = v;
			return *this;
		};
	};
public:
	TxConfigV34 tx_config_v34() {
		return TxConfigV34().channel(channel_);
	}
private:
	class RxConfigT38 {
		mutable int used_;
		SMDC_RX_CONFIG_T38_PARMS configp_;
	public:
		RxConfigT38(const RxConfigT38 &old) {
			*this = old;
			old.used_ = 1;
		};
		RxConfigT38() : used_(0) {
			memset(&configp_, 0, sizeof(configp_));
		};
		operator Error() {
			Error e(Prosody::smdc_rx_config_t38(&configp_));
			used_ = 1;
			return e;
		};
		~RxConfigT38() {
			if (!used_) {
				fprintf(stderr, "Prosody function smdc_rx_config_t38 not called\n");
				abort();
			};
		};
		RxConfigT38 &channel(tSMChannelId v) {
			configp_.channel = v;
			return *this;
		};
		RxConfigT38 &PreCorrigendum(tSM_INT v) {
			configp_.PreCorrigendum = v;
			return *this;
		};
		RxConfigT38 &Protocol(tSM_INT v) {
			configp_.Protocol = v;
			return *this;
		};
	};
public:
	RxConfigT38 rx_config_t38() {
		return RxConfigT38().channel(channel_);
	}
private:
	class TxConfigT38 {
		mutable int used_;
		SMDC_TX_CONFIG_T38_PARMS configp_;
	public:
		TxConfigT38(const TxConfigT38 &old) {
			*this = old;
			old.used_ = 1;
		};
		TxConfigT38() : used_(0) {
			memset(&configp_, 0, sizeof(configp_));
		};
		operator Error() {
			Error e(Prosody::smdc_tx_config_t38(&configp_));
			used_ = 1;
			return e;
		};
		~TxConfigT38() {
			if (!used_) {
				fprintf(stderr, "Prosody function smdc_tx_config_t38 not called\n");
				abort();
			};
		};
		TxConfigT38 &channel(tSMChannelId v) {
			configp_.channel = v;
			return *this;
		};
		TxConfigT38 &PreCorrigendum(tSM_INT v) {
			configp_.PreCorrigendum = v;
			return *this;
		};
		TxConfigT38 &Protocol(tSM_INT v) {
			configp_.Protocol = v;
			return *this;
		};
		TxConfigT38 &max_packet_length(tSM_UT32 v) {
			configp_.max_packet_length = v;
			return *this;
		};
	};
public:
	TxConfigT38 tx_config_t38() {
		return TxConfigT38().channel(channel_);
	}
private:
	class TxV34LineControlT38 {
		mutable int used_;
		SMDC_TX_V34_LINE_CONTROL_T38_PARMS configp_;
	public:
		TxV34LineControlT38(const TxV34LineControlT38 &old) {
			*this = old;
			old.used_ = 1;
		};
		TxV34LineControlT38() : used_(0) {
			memset(&configp_, 0, sizeof(configp_));
		};
		operator Error() {
			Error e(Prosody::smdc_tx_v34_line_control_t38(&configp_));
			used_ = 1;
			return e;
		};
		~TxV34LineControlT38() {
			if (!used_) {
				fprintf(stderr, "Prosody function smdc_tx_v34_line_control_t38 not called\n");
				abort();
			};
		};
		TxV34LineControlT38 &channel(tSMChannelId v) {
			configp_.channel = v;
			return *this;
		};
		TxV34LineControlT38 &primary_bitrate(tSM_UT32 v) {
			configp_.primary_bitrate = v;
			return *this;
		};
	};
public:
	TxV34LineControlT38 tx_v34_line_control_t38() {
		return TxV34LineControlT38().channel(channel_);
	}
private:
	class RxEncoding {
		mutable int used_;
		SMDC_ENCODING_PARMS encp_;
	public:
		RxEncoding(const RxEncoding &old) {
			*this = old;
			old.used_ = 1;
		};
		RxEncoding() : used_(0) {
			memset(&encp_, 0, sizeof(encp_));
		};
		operator Error() {
			Error e(Prosody::smdc_rx_encoding(&encp_));
			used_ = 1;
			return e;
		};
		~RxEncoding() {
			if (!used_) {
				fprintf(stderr, "Prosody function smdc_rx_encoding not called\n");
				abort();
			};
		};
		RxEncoding &channel(tSMChannelId v) {
			encp_.channel = v;
			return *this;
		};
		RxEncoding &encoding(tSM_INT v) {
			encp_.encoding = v;
			return *this;
		};
		RxEncoding &encoding_config_length(tSM_INT v) {
			encp_.encoding_config_length = v;
			return *this;
		};
		RxEncoding &encoding_config_data(void* v) {
			encp_.encoding_config_data = v;
			return *this;
		};
	};
public:
	RxEncoding rx_encoding() {
		return RxEncoding().channel(channel_);
	}
private:
	class TxEncoding {
		mutable int used_;
		SMDC_ENCODING_PARMS encp_;
	public:
		TxEncoding(const TxEncoding &old) {
			*this = old;
			old.used_ = 1;
		};
		TxEncoding() : used_(0) {
			memset(&encp_, 0, sizeof(encp_));
		};
		operator Error() {
			Error e(Prosody::smdc_tx_encoding(&encp_));
			used_ = 1;
			return e;
		};
		~TxEncoding() {
			if (!used_) {
				fprintf(stderr, "Prosody function smdc_tx_encoding not called\n");
				abort();
			};
		};
		TxEncoding &channel(tSMChannelId v) {
			encp_.channel = v;
			return *this;
		};
		TxEncoding &encoding(tSM_INT v) {
			encp_.encoding = v;
			return *this;
		};
		TxEncoding &encoding_config_length(tSM_INT v) {
			encp_.encoding_config_length = v;
			return *this;
		};
		TxEncoding &encoding_config_data(void* v) {
			encp_.encoding_config_data = v;
			return *this;
		};
	};
public:
	TxEncoding tx_encoding() {
		return TxEncoding().channel(channel_);
	}
private:
	class LineControl {
		mutable int used_;
		SMDC_LINE_CONTROL_PARMS controlp_;
	public:
		LineControl(const LineControl &old) {
			*this = old;
			old.used_ = 1;
		};
		LineControl() : used_(0) {
			memset(&controlp_, 0, sizeof(controlp_));
		};
		operator Error() {
			Error e(Prosody::smdc_line_control(&controlp_));
			used_ = 1;
			return e;
		};
		~LineControl() {
			if (!used_) {
				fprintf(stderr, "Prosody function smdc_line_control not called\n");
				abort();
			};
		};
		LineControl &channel(tSMChannelId v) {
			controlp_.channel = v;
			return *this;
		};
		LineControl &cmd(enum kSMDCLineCtlCmd v) {
			controlp_.cmd = v;
			return *this;
		};
		LineControl &aux_mask(tSM_UT32 v) {
			controlp_.aux_mask = v;
			return *this;
		};
		LineControl &aux_toggle(tSM_UT32 v) {
			controlp_.aux_toggle = v;
			return *this;
		};
		LineControl &shorttrain(tSM_UT32 v) {
			controlp_.shorttrain = v;
			return *this;
		};
		LineControl &tep(tSM_INT v) {
			controlp_.tep = v;
			return *this;
		};
		LineControl &prefix_bitlength(tSM_UT32 v) {
			controlp_.prefix_bitlength = v;
			return *this;
		};
		LineControl &prefix_data(tSM_UT32* v) {
			controlp_.prefix_data = v;
			return *this;
		};
		LineControl &suffix_bitlength(tSM_UT32 v) {
			controlp_.suffix_bitlength = v;
			return *this;
		};
		LineControl &suffix_data(tSM_UT32* v) {
			controlp_.suffix_data = v;
			return *this;
		};
		LineControl &retrain_speed(tSM_UT32 v) {
			controlp_.retrain_speed = v;
			return *this;
		};
	};
public:
	LineControl line_control() {
		return LineControl().channel(channel_);
	}
private:
	class LineControlV34 {
		mutable int used_;
		SMDC_LINE_CONTROL_V34_PARMS controlp_;
	public:
		LineControlV34(const LineControlV34 &old) {
			*this = old;
			old.used_ = 1;
		};
		LineControlV34() : used_(0) {
			memset(&controlp_, 0, sizeof(controlp_));
		};
		operator Error() {
			Error e(Prosody::smdc_line_control_v34(&controlp_));
			used_ = 1;
			return e;
		};
		~LineControlV34() {
			if (!used_) {
				fprintf(stderr, "Prosody function smdc_line_control_v34 not called\n");
				abort();
			};
		};
		LineControlV34 &channel(tSMChannelId v) {
			controlp_.channel = v;
			return *this;
		};
		LineControlV34 &cmd(enum kSMDCV34LineCtlCmd v) {
			controlp_.cmd = v;
			return *this;
		};
		LineControlV34 &u_bitrate_min_bitrate(tSM_INT v) {
			controlp_.u.bitrate.min_bitrate = v;
			return *this;
		};
		LineControlV34 &u_bitrate_max_bitrate(tSM_INT v) {
			controlp_.u.bitrate.max_bitrate = v;
			return *this;
		};
	};
public:
	LineControlV34 line_control_v34() {
		return LineControlV34().channel(channel_);
	}
private:
	class LineStatus {
		mutable int used_;
		SMDC_LINE_STATUS_PARMS statusp_;
		enum kSMDCLinkStatus *link_statusP_;
		enum kSMDCLineRxStatus *rx_statusP_;
		enum kSMDCLineTxStatus *tx_statusP_;
		tSM_INT *tx_ctsP_;
	public:
		LineStatus(const LineStatus &old) {
			*this = old;
			old.used_ = 1;
		};
		LineStatus() : used_(0) {
			memset(&statusp_, 0, sizeof(statusp_));
			link_statusP_ = 0;
			rx_statusP_ = 0;
			tx_statusP_ = 0;
			tx_ctsP_ = 0;
		};
		operator Error() {
			Error e(Prosody::smdc_line_status(&statusp_));
			used_ = 1;
			if (link_statusP_) *link_statusP_ = statusp_.link_status;
			if (rx_statusP_) *rx_statusP_ = statusp_.rx_status;
			if (tx_statusP_) *tx_statusP_ = statusp_.tx_status;
			if (tx_ctsP_) *tx_ctsP_ = statusp_.tx_cts;
			return e;
		};
		~LineStatus() {
			if (!used_) {
				fprintf(stderr, "Prosody function smdc_line_status not called\n");
				abort();
			};
		};
		LineStatus &channel(tSMChannelId v) {
			statusp_.channel = v;
			return *this;
		};
		LineStatus &link_status(enum kSMDCLinkStatus *v) {
			link_statusP_ = v;
			return *this;
		};
		LineStatus &rx_status(enum kSMDCLineRxStatus *v) {
			rx_statusP_ = v;
			return *this;
		};
		LineStatus &tx_status(enum kSMDCLineTxStatus *v) {
			tx_statusP_ = v;
			return *this;
		};
		LineStatus &tx_cts(tSM_INT *v) {
			tx_ctsP_ = v;
			return *this;
		};
	};
public:
	LineStatus line_status() {
		return LineStatus().channel(channel_);
	}
private:
	class LineStatusV34 {
		mutable int used_;
		SMDC_LINE_STATUS_V34_PARMS statusp_;
		enum kSMDCLinkStatus *link_statusP_;
		enum kSMDCLineRxStatus *rx_statusP_;
		enum kSMDCLineTxStatus *tx_statusP_;
		tSM_INT *control_bitrateP_;
		tSM_INT *primary_bitrateP_;
		tSM_INT *snr_statusP_;
		tSM_INT *fdx_rx_bitrateP_;
		tSM_INT *fdx_tx_bitrateP_;
	public:
		LineStatusV34(const LineStatusV34 &old) {
			*this = old;
			old.used_ = 1;
		};
		LineStatusV34() : used_(0) {
			memset(&statusp_, 0, sizeof(statusp_));
			link_statusP_ = 0;
			rx_statusP_ = 0;
			tx_statusP_ = 0;
			control_bitrateP_ = 0;
			primary_bitrateP_ = 0;
			snr_statusP_ = 0;
			fdx_rx_bitrateP_ = 0;
			fdx_tx_bitrateP_ = 0;
		};
		operator Error() {
			Error e(Prosody::smdc_line_status_v34(&statusp_));
			used_ = 1;
			if (link_statusP_) *link_statusP_ = statusp_.link_status;
			if (rx_statusP_) *rx_statusP_ = statusp_.rx_status;
			if (tx_statusP_) *tx_statusP_ = statusp_.tx_status;
			if (control_bitrateP_) *control_bitrateP_ = statusp_.control_bitrate;
			if (primary_bitrateP_) *primary_bitrateP_ = statusp_.primary_bitrate;
			if (snr_statusP_) *snr_statusP_ = statusp_.snr_status;
			if (fdx_rx_bitrateP_) *fdx_rx_bitrateP_ = statusp_.fdx_rx_bitrate;
			if (fdx_tx_bitrateP_) *fdx_tx_bitrateP_ = statusp_.fdx_tx_bitrate;
			return e;
		};
		~LineStatusV34() {
			if (!used_) {
				fprintf(stderr, "Prosody function smdc_line_status_v34 not called\n");
				abort();
			};
		};
		LineStatusV34 &channel(tSMChannelId v) {
			statusp_.channel = v;
			return *this;
		};
		LineStatusV34 &link_status(enum kSMDCLinkStatus *v) {
			link_statusP_ = v;
			return *this;
		};
		LineStatusV34 &rx_status(enum kSMDCLineRxStatus *v) {
			rx_statusP_ = v;
			return *this;
		};
		LineStatusV34 &tx_status(enum kSMDCLineTxStatus *v) {
			tx_statusP_ = v;
			return *this;
		};
		LineStatusV34 &control_bitrate(tSM_INT *v) {
			control_bitrateP_ = v;
			return *this;
		};
		LineStatusV34 &primary_bitrate(tSM_INT *v) {
			primary_bitrateP_ = v;
			return *this;
		};
		LineStatusV34 &snr_status(tSM_INT *v) {
			snr_statusP_ = v;
			return *this;
		};
		LineStatusV34 &fdx_rx_bitrate(tSM_INT *v) {
			fdx_rx_bitrateP_ = v;
			return *this;
		};
		LineStatusV34 &fdx_tx_bitrate(tSM_INT *v) {
			fdx_tx_bitrateP_ = v;
			return *this;
		};
	};
public:
	LineStatusV34 line_status_v34() {
		return LineStatusV34().channel(channel_);
	}
private:
	class TxStatus {
		mutable int used_;
		SMDC_TX_STATUS_PARMS statusp_;
		tSM_INT *capacityP_;
		enum kSMDCTxStatus *statusP_;
		tSM_INT *flowP_;
	public:
		TxStatus(const TxStatus &old) {
			*this = old;
			old.used_ = 1;
		};
		TxStatus() : used_(0) {
			memset(&statusp_, 0, sizeof(statusp_));
			capacityP_ = 0;
			statusP_ = 0;
			flowP_ = 0;
		};
		operator Error() {
			Error e(Prosody::smdc_tx_status(&statusp_));
			used_ = 1;
			if (capacityP_) *capacityP_ = statusp_.capacity;
			if (statusP_) *statusP_ = statusp_.status;
			if (flowP_) *flowP_ = statusp_.flow;
			return e;
		};
		~TxStatus() {
			if (!used_) {
				fprintf(stderr, "Prosody function smdc_tx_status not called\n");
				abort();
			};
		};
		TxStatus &channel(tSMChannelId v) {
			statusp_.channel = v;
			return *this;
		};
		TxStatus &capacity(tSM_INT *v) {
			capacityP_ = v;
			return *this;
		};
		TxStatus &status(enum kSMDCTxStatus *v) {
			statusP_ = v;
			return *this;
		};
		TxStatus &flow(tSM_INT *v) {
			flowP_ = v;
			return *this;
		};
	};
public:
	TxStatus tx_status() {
		return TxStatus().channel(channel_);
	}
private:
	class TxControl {
		mutable int used_;
		SMDC_TX_CONTROL_PARMS controlp_;
	public:
		TxControl(const TxControl &old) {
			*this = old;
			old.used_ = 1;
		};
		TxControl() : used_(0) {
			memset(&controlp_, 0, sizeof(controlp_));
		};
		operator Error() {
			Error e(Prosody::smdc_tx_control(&controlp_));
			used_ = 1;
			return e;
		};
		~TxControl() {
			if (!used_) {
				fprintf(stderr, "Prosody function smdc_tx_control not called\n");
				abort();
			};
		};
		TxControl &channel(tSMChannelId v) {
			controlp_.channel = v;
			return *this;
		};
		TxControl &cmd(enum kSMDCTxCtl v) {
			controlp_.cmd = v;
			return *this;
		};
		TxControl &capacity(tSM_INT v) {
			controlp_.capacity = v;
			return *this;
		};
		TxControl &blocking(tSM_INT v) {
			controlp_.blocking = v;
			return *this;
		};
	};
public:
	TxControl tx_control() {
		return TxControl().channel(channel_);
	}
private:
	class TxData {
		mutable int used_;
		SMDC_DATA_PARMS datap_;
		tSM_INT *done_lengthP_;
	public:
		TxData(const TxData &old) {
			*this = old;
			old.used_ = 1;
		};
		TxData() : used_(0) {
			memset(&datap_, 0, sizeof(datap_));
			done_lengthP_ = 0;
		};
		operator Error() {
			Error e(Prosody::smdc_tx_data(&datap_));
			used_ = 1;
			if (done_lengthP_) *done_lengthP_ = datap_.done_length;
			return e;
		};
		~TxData() {
			if (!used_) {
				fprintf(stderr, "Prosody function smdc_tx_data not called\n");
				abort();
			};
		};
		TxData &channel(tSMChannelId v) {
			datap_.channel = v;
			return *this;
		};
		TxData &data(char* v) {
			datap_.data = v;
			return *this;
		};
		TxData &max_length(tSM_INT v) {
			datap_.max_length = v;
			return *this;
		};
		TxData &done_length(tSM_INT *v) {
			done_lengthP_ = v;
			return *this;
		};
	};
public:
	TxData tx_data() {
		return TxData().channel(channel_);
	}
private:
	class TxToneIndicatorT38 {
		mutable int used_;
		SMDC_TX_TONE_INDICATOR_T38_PARMS tonep_;
	public:
		TxToneIndicatorT38(const TxToneIndicatorT38 &old) {
			*this = old;
			old.used_ = 1;
		};
		TxToneIndicatorT38() : used_(0) {
			memset(&tonep_, 0, sizeof(tonep_));
		};
		operator Error() {
			Error e(Prosody::smdc_tx_tone_indicator_t38(&tonep_));
			used_ = 1;
			return e;
		};
		~TxToneIndicatorT38() {
			if (!used_) {
				fprintf(stderr, "Prosody function smdc_tx_tone_indicator_t38 not called\n");
				abort();
			};
		};
		TxToneIndicatorT38 &channel(tSMChannelId v) {
			tonep_.channel = v;
			return *this;
		};
		TxToneIndicatorT38 &tone_type(enum kSMDCT38ToneType v) {
			tonep_.tone_type = v;
			return *this;
		};
	};
public:
	TxToneIndicatorT38 tx_tone_indicator_t38() {
		return TxToneIndicatorT38().channel(channel_);
	}
private:
	class RxStatus {
		mutable int used_;
		SMDC_RX_STATUS_PARMS statusp_;
		tSM_INT *available_octetsP_;
		enum kSMDCRxStatus *statusP_;
		tSM_INT *flowP_;
		tSM_INT *u_carrier_carrierP_;
		tSM_INT *u_metric_metricP_;
	public:
		RxStatus(const RxStatus &old) {
			*this = old;
			old.used_ = 1;
		};
		RxStatus() : used_(0) {
			memset(&statusp_, 0, sizeof(statusp_));
			available_octetsP_ = 0;
			statusP_ = 0;
			flowP_ = 0;
			u_carrier_carrierP_ = 0;
			u_metric_metricP_ = 0;
		};
		operator Error() {
			Error e(Prosody::smdc_rx_status(&statusp_));
			used_ = 1;
			if (available_octetsP_) *available_octetsP_ = statusp_.available_octets;
			if (statusP_) *statusP_ = statusp_.status;
			if (flowP_) *flowP_ = statusp_.flow;
			if (u_carrier_carrierP_) *u_carrier_carrierP_ = statusp_.u.carrier.carrier;
			if (u_metric_metricP_) *u_metric_metricP_ = statusp_.u.metric.metric;
			return e;
		};
		~RxStatus() {
			if (!used_) {
				fprintf(stderr, "Prosody function smdc_rx_status not called\n");
				abort();
			};
		};
		RxStatus &channel(tSMChannelId v) {
			statusp_.channel = v;
			return *this;
		};
		RxStatus &available_octets(tSM_INT *v) {
			available_octetsP_ = v;
			return *this;
		};
		RxStatus &status(enum kSMDCRxStatus *v) {
			statusP_ = v;
			return *this;
		};
		RxStatus &flow(tSM_INT *v) {
			flowP_ = v;
			return *this;
		};
		RxStatus &u_carrier_carrier(tSM_INT *v) {
			u_carrier_carrierP_ = v;
			return *this;
		};
		RxStatus &u_metric_metric(tSM_INT *v) {
			u_metric_metricP_ = v;
			return *this;
		};
	};
public:
	RxStatus rx_status() {
		return RxStatus().channel(channel_);
	}
private:
	class RxStatusT38 {
		mutable int used_;
		SMDC_RX_STATUS_T38_PARMS statusp_;
		tSM_INT *available_octetsP_;
		enum kSMDCRxT38Status *statusP_;
		tSM_INT *flowP_;
		enum kSMDCT38ToneType *u_tone_tone_typeP_;
		enum kSMDCT38_V34Type *u_v34_v34_typeP_;
		tSM_INT *u_carrier_carrierP_;
		tSM_INT *u_rate_rateP_;
		tSM_INT *u_protocol_protocolP_;
	public:
		RxStatusT38(const RxStatusT38 &old) {
			*this = old;
			old.used_ = 1;
		};
		RxStatusT38() : used_(0) {
			memset(&statusp_, 0, sizeof(statusp_));
			available_octetsP_ = 0;
			statusP_ = 0;
			flowP_ = 0;
			u_tone_tone_typeP_ = 0;
			u_v34_v34_typeP_ = 0;
			u_carrier_carrierP_ = 0;
			u_rate_rateP_ = 0;
			u_protocol_protocolP_ = 0;
		};
		operator Error() {
			Error e(Prosody::smdc_rx_status_t38(&statusp_));
			used_ = 1;
			if (available_octetsP_) *available_octetsP_ = statusp_.available_octets;
			if (statusP_) *statusP_ = statusp_.status;
			if (flowP_) *flowP_ = statusp_.flow;
			if (u_tone_tone_typeP_) *u_tone_tone_typeP_ = statusp_.u.tone.tone_type;
			if (u_v34_v34_typeP_) *u_v34_v34_typeP_ = statusp_.u.v34.v34_type;
			if (u_carrier_carrierP_) *u_carrier_carrierP_ = statusp_.u.carrier.carrier;
			if (u_rate_rateP_) *u_rate_rateP_ = statusp_.u.rate.rate;
			if (u_protocol_protocolP_) *u_protocol_protocolP_ = statusp_.u.protocol.protocol;
			return e;
		};
		~RxStatusT38() {
			if (!used_) {
				fprintf(stderr, "Prosody function smdc_rx_status_t38 not called\n");
				abort();
			};
		};
		RxStatusT38 &channel(tSMChannelId v) {
			statusp_.channel = v;
			return *this;
		};
		RxStatusT38 &available_octets(tSM_INT *v) {
			available_octetsP_ = v;
			return *this;
		};
		RxStatusT38 &status(enum kSMDCRxT38Status *v) {
			statusP_ = v;
			return *this;
		};
		RxStatusT38 &flow(tSM_INT *v) {
			flowP_ = v;
			return *this;
		};
		RxStatusT38 &u_tone_tone_type(enum kSMDCT38ToneType *v) {
			u_tone_tone_typeP_ = v;
			return *this;
		};
		RxStatusT38 &u_v34_v34_type(enum kSMDCT38_V34Type *v) {
			u_v34_v34_typeP_ = v;
			return *this;
		};
		RxStatusT38 &u_carrier_carrier(tSM_INT *v) {
			u_carrier_carrierP_ = v;
			return *this;
		};
		RxStatusT38 &u_rate_rate(tSM_INT *v) {
			u_rate_rateP_ = v;
			return *this;
		};
		RxStatusT38 &u_protocol_protocol(tSM_INT *v) {
			u_protocol_protocolP_ = v;
			return *this;
		};
	};
public:
	RxStatusT38 rx_status_t38() {
		return RxStatusT38().channel(channel_);
	}
private:
	class TxV34IndicatorT38 {
		mutable int used_;
		SMDC_TX_V34_INDICATOR_T38_PARMS tonep_;
		enum kSMDCT38_V34Type *v34_typeP_;
	public:
		TxV34IndicatorT38(const TxV34IndicatorT38 &old) {
			*this = old;
			old.used_ = 1;
		};
		TxV34IndicatorT38() : used_(0) {
			memset(&tonep_, 0, sizeof(tonep_));
			v34_typeP_ = 0;
		};
		operator Error() {
			Error e(Prosody::smdc_tx_v34_indicator_t38(&tonep_));
			used_ = 1;
			if (v34_typeP_) *v34_typeP_ = tonep_.v34_type;
			return e;
		};
		~TxV34IndicatorT38() {
			if (!used_) {
				fprintf(stderr, "Prosody function smdc_tx_v34_indicator_t38 not called\n");
				abort();
			};
		};
		TxV34IndicatorT38 &channel(tSMChannelId v) {
			tonep_.channel = v;
			return *this;
		};
		TxV34IndicatorT38 &v34_type(enum kSMDCT38_V34Type *v) {
			v34_typeP_ = v;
			return *this;
		};
		TxV34IndicatorT38 &v34_type(enum kSMDCT38_V34Type v) {
			tonep_.v34_type = v;
			return *this;
		};
	};
public:
	TxV34IndicatorT38 tx_v34_indicator_t38() {
		return TxV34IndicatorT38().channel(channel_);
	}
private:
	class RxControl {
		mutable int used_;
		SMDC_RX_CONTROL_PARMS controlp_;
	public:
		RxControl(const RxControl &old) {
			*this = old;
			old.used_ = 1;
		};
		RxControl() : used_(0) {
			memset(&controlp_, 0, sizeof(controlp_));
		};
		operator Error() {
			Error e(Prosody::smdc_rx_control(&controlp_));
			used_ = 1;
			return e;
		};
		~RxControl() {
			if (!used_) {
				fprintf(stderr, "Prosody function smdc_rx_control not called\n");
				abort();
			};
		};
		RxControl &channel(tSMChannelId v) {
			controlp_.channel = v;
			return *this;
		};
		RxControl &cmd(enum kSMDCRxCtl v) {
			controlp_.cmd = v;
			return *this;
		};
		RxControl &min_to_collect(tSM_INT v) {
			controlp_.min_to_collect = v;
			return *this;
		};
		RxControl &min_idle(tSM_INT v) {
			controlp_.min_idle = v;
			return *this;
		};
		RxControl &blocking(tSM_INT v) {
			controlp_.blocking = v;
			return *this;
		};
	};
public:
	RxControl rx_control() {
		return RxControl().channel(channel_);
	}
private:
	class RxData {
		mutable int used_;
		SMDC_DATA_PARMS datap_;
		tSM_INT *done_lengthP_;
	public:
		RxData(const RxData &old) {
			*this = old;
			old.used_ = 1;
		};
		RxData() : used_(0) {
			memset(&datap_, 0, sizeof(datap_));
			done_lengthP_ = 0;
		};
		operator Error() {
			Error e(Prosody::smdc_rx_data(&datap_));
			used_ = 1;
			if (done_lengthP_) *done_lengthP_ = datap_.done_length;
			return e;
		};
		~RxData() {
			if (!used_) {
				fprintf(stderr, "Prosody function smdc_rx_data not called\n");
				abort();
			};
		};
		RxData &channel(tSMChannelId v) {
			datap_.channel = v;
			return *this;
		};
		RxData &data(char* v) {
			datap_.data = v;
			return *this;
		};
		RxData &max_length(tSM_INT v) {
			datap_.max_length = v;
			return *this;
		};
		RxData &done_length(tSM_INT *v) {
			done_lengthP_ = v;
			return *this;
		};
	};
public:
	RxData rx_data() {
		return RxData().channel(channel_);
	}
private:
	class Stop {
		mutable int used_;
		SMDC_STOP_PARMS pp_;
	public:
		Stop(const Stop &old) {
			*this = old;
			old.used_ = 1;
		};
		Stop() : used_(0) {
			memset(&pp_, 0, sizeof(pp_));
		};
		operator Error() {
			Error e(Prosody::smdc_stop(&pp_));
			used_ = 1;
			return e;
		};
		~Stop() {
			if (!used_) {
				fprintf(stderr, "Prosody function smdc_stop not called\n");
				abort();
			};
		};
		Stop &channel(tSMChannelId v) {
			pp_.channel = v;
			return *this;
		};
	};
public:
	Stop stop() {
		return Stop().channel(channel_);
	}
private:
	class RxStop {
		mutable int used_;
		SMDC_STOP_PARMS pp_;
	public:
		RxStop(const RxStop &old) {
			*this = old;
			old.used_ = 1;
		};
		RxStop() : used_(0) {
			memset(&pp_, 0, sizeof(pp_));
		};
		operator Error() {
			Error e(Prosody::smdc_rx_stop(&pp_));
			used_ = 1;
			return e;
		};
		~RxStop() {
			if (!used_) {
				fprintf(stderr, "Prosody function smdc_rx_stop not called\n");
				abort();
			};
		};
		RxStop &channel(tSMChannelId v) {
			pp_.channel = v;
			return *this;
		};
	};
public:
	RxStop rx_stop() {
		return RxStop().channel(channel_);
	}
private:
	class TxStop {
		mutable int used_;
		SMDC_STOP_PARMS pp_;
	public:
		TxStop(const TxStop &old) {
			*this = old;
			old.used_ = 1;
		};
		TxStop() : used_(0) {
			memset(&pp_, 0, sizeof(pp_));
		};
		operator Error() {
			Error e(Prosody::smdc_tx_stop(&pp_));
			used_ = 1;
			return e;
		};
		~TxStop() {
			if (!used_) {
				fprintf(stderr, "Prosody function smdc_tx_stop not called\n");
				abort();
			};
		};
		TxStop &channel(tSMChannelId v) {
			pp_.channel = v;
			return *this;
		};
	};
public:
	TxStop tx_stop() {
		return TxStop().channel(channel_);
	}
