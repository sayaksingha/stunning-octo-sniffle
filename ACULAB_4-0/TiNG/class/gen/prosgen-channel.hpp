private:
	class AllocPlaced {
		mutable int used_;
		SM_CHANNEL_ALLOC_PLACED_PARMS channelp_;
		tSMChannelId *channelP_;
	public:
		AllocPlaced(const AllocPlaced &old) {
			*this = old;
			old.used_ = 1;
		};
		AllocPlaced() : used_(0) {
			memset(&channelp_, 0, sizeof(channelp_));
			channelP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_channel_alloc_placed(&channelp_));
			used_ = 1;
			if (channelP_) *channelP_ = channelp_.channel;
			return e;
		};
		~AllocPlaced() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_channel_alloc_placed not called\n");
				abort();
			};
		};
		AllocPlaced &channel(tSMChannelId *v) {
			channelP_ = v;
			return *this;
		};
		AllocPlaced &type(enum kSMChannelType v) {
			channelp_.type = v;
			return *this;
		};
		AllocPlaced &module(tSMModuleId v) {
			channelp_.module = v;
			return *this;
		};
		AllocPlaced &caps_mask(tSM_INT v) {
			channelp_.caps_mask = v;
			return *this;
		};
	};
public:
	AllocPlaced alloc_placed() {
		return AllocPlaced().channel(&channel_);
	}
private:
	class SetOutputRate {
		mutable int used_;
		SM_CHANNEL_SET_OUTPUT_RATE_PARMS ratep_;
	public:
		SetOutputRate(const SetOutputRate &old) {
			*this = old;
			old.used_ = 1;
		};
		SetOutputRate() : used_(0) {
			memset(&ratep_, 0, sizeof(ratep_));
		};
		operator Error() {
			Error e(Prosody::sm_channel_set_output_rate(&ratep_));
			used_ = 1;
			return e;
		};
		~SetOutputRate() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_channel_set_output_rate not called\n");
				abort();
			};
		};
		SetOutputRate &channel(tSMChannelId v) {
			ratep_.channel = v;
			return *this;
		};
		SetOutputRate &sample_rate(tSM_INT v) {
			ratep_.sample_rate = v;
			return *this;
		};
	};
public:
	SetOutputRate set_output_rate() {
		return SetOutputRate().channel(channel_);
	}
private:
	class SwitchOutput {
		mutable int used_;
		SM_SWITCH_CHANNEL_PARMS switchp_;
	public:
		SwitchOutput(const SwitchOutput &old) {
			*this = old;
			old.used_ = 1;
		};
		SwitchOutput() : used_(0) {
			memset(&switchp_, 0, sizeof(switchp_));
		};
		operator Error() {
			Error e(Prosody::sm_switch_channel_output(&switchp_));
			used_ = 1;
			return e;
		};
		~SwitchOutput() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_switch_channel_output not called\n");
				abort();
			};
		};
		SwitchOutput &channel(tSMChannelId v) {
			switchp_.channel = v;
			return *this;
		};
		SwitchOutput &st(tSM_INT v) {
			switchp_.st = v;
			return *this;
		};
		SwitchOutput &ts(tSM_INT v) {
			switchp_.ts = v;
			return *this;
		};
		SwitchOutput &type(enum kSMTimeslotType v) {
			switchp_.type = v;
			return *this;
		};
	};
public:
	SwitchOutput switch_output() {
		return SwitchOutput().channel(channel_);
	}
private:
	class SwitchInput {
		mutable int used_;
		SM_SWITCH_CHANNEL_PARMS switchp_;
	public:
		SwitchInput(const SwitchInput &old) {
			*this = old;
			old.used_ = 1;
		};
		SwitchInput() : used_(0) {
			memset(&switchp_, 0, sizeof(switchp_));
		};
		operator Error() {
			Error e(Prosody::sm_switch_channel_input(&switchp_));
			used_ = 1;
			return e;
		};
		~SwitchInput() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_switch_channel_input not called\n");
				abort();
			};
		};
		SwitchInput &channel(tSMChannelId v) {
			switchp_.channel = v;
			return *this;
		};
		SwitchInput &st(tSM_INT v) {
			switchp_.st = v;
			return *this;
		};
		SwitchInput &ts(tSM_INT v) {
			switchp_.ts = v;
			return *this;
		};
		SwitchInput &type(enum kSMTimeslotType v) {
			switchp_.type = v;
			return *this;
		};
	};
public:
	SwitchInput switch_input() {
		return SwitchInput().channel(channel_);
	}
private:
	class Release {
		mutable int used_;
		tSMChannelId channel_;
	public:
		Release(const Release &old) {
			*this = old;
			old.used_ = 1;
		};
		Release(tSMChannelId channel) : used_(0) {
			channel_ = channel;
		};
		operator Error() {
			Error e(Prosody::sm_channel_release(channel_));
			used_ = 1;
			return e;
		};
		~Release() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_channel_release not called\n");
				abort();
			};
		};
	};
public:
	Release release() {
		Release temp(channel_);
		
		channel_ = 0;
		return temp;
	}
private:
	class Info {
		mutable int used_;
		SM_CHANNEL_INFO_PARMS infop_;
		tSM_INT *cardP_;
		tSM_INT *ostP_;
		tSM_INT *otsP_;
		enum kSMTimeslotType *otypeP_;
		tSM_INT *istP_;
		tSM_INT *itsP_;
		enum kSMTimeslotType *itypeP_;
		tSM_INT *groupP_;
		tSM_INT *caps_maskP_;
	public:
		Info(const Info &old) {
			*this = old;
			old.used_ = 1;
		};
		Info() : used_(0) {
			memset(&infop_, 0, sizeof(infop_));
			cardP_ = 0;
			ostP_ = 0;
			otsP_ = 0;
			otypeP_ = 0;
			istP_ = 0;
			itsP_ = 0;
			itypeP_ = 0;
			groupP_ = 0;
			caps_maskP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_channel_info(&infop_));
			used_ = 1;
			if (cardP_) *cardP_ = infop_.card;
			if (ostP_) *ostP_ = infop_.ost;
			if (otsP_) *otsP_ = infop_.ots;
			if (otypeP_) *otypeP_ = infop_.otype;
			if (istP_) *istP_ = infop_.ist;
			if (itsP_) *itsP_ = infop_.its;
			if (itypeP_) *itypeP_ = infop_.itype;
			if (groupP_) *groupP_ = infop_.group;
			if (caps_maskP_) *caps_maskP_ = infop_.caps_mask;
			return e;
		};
		~Info() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_channel_info not called\n");
				abort();
			};
		};
		Info &channel(tSMChannelId v) {
			infop_.channel = v;
			return *this;
		};
		Info &card(tSM_INT *v) {
			cardP_ = v;
			return *this;
		};
		Info &ost(tSM_INT *v) {
			ostP_ = v;
			return *this;
		};
		Info &ots(tSM_INT *v) {
			otsP_ = v;
			return *this;
		};
		Info &otype(enum kSMTimeslotType *v) {
			otypeP_ = v;
			return *this;
		};
		Info &ist(tSM_INT *v) {
			istP_ = v;
			return *this;
		};
		Info &its(tSM_INT *v) {
			itsP_ = v;
			return *this;
		};
		Info &itype(enum kSMTimeslotType *v) {
			itypeP_ = v;
			return *this;
		};
		Info &group(tSM_INT *v) {
			groupP_ = v;
			return *this;
		};
		Info &caps_mask(tSM_INT *v) {
			caps_maskP_ = v;
			return *this;
		};
	};
public:
	Info info() {
		return Info().channel(channel_);
	}
private:
	class GetIx {
		mutable int used_;
		tSMChannelId channel_;
	public:
		GetIx(const GetIx &old) {
			*this = old;
			old.used_ = 1;
		};
		GetIx(tSMChannelId channel) : used_(0) {
			channel_ = channel;
		};
		operator Error() {
			Error e(Prosody::sm_get_channel_ix(channel_));
			used_ = 1;
			return e;
		};
		~GetIx() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_get_channel_ix not called\n");
				abort();
			};
		};
	};
public:
	GetIx get_ix() {
		return GetIx(channel_);
	}
private:
	class GetType {
		mutable int used_;
		tSMChannelId channel_;
	public:
		GetType(const GetType &old) {
			*this = old;
			old.used_ = 1;
		};
		GetType(tSMChannelId channel) : used_(0) {
			channel_ = channel;
		};
		operator Error() {
			Error e(Prosody::sm_get_channel_type(channel_));
			used_ = 1;
			return e;
		};
		~GetType() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_get_channel_type not called\n");
				abort();
			};
		};
	};
public:
	GetType get_type() {
		return GetType(channel_);
	}
private:
	class SetEvent {
		mutable int used_;
		SM_CHANNEL_SET_EVENT_PARMS eventp_;
	public:
		SetEvent(const SetEvent &old) {
			*this = old;
			old.used_ = 1;
		};
		SetEvent() : used_(0) {
			memset(&eventp_, 0, sizeof(eventp_));
		};
		operator Error() {
			Error e(Prosody::sm_channel_set_event(&eventp_));
			used_ = 1;
			return e;
		};
		~SetEvent() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_channel_set_event not called\n");
				abort();
			};
		};
		SetEvent &channel(tSMChannelId v) {
			eventp_.channel = v;
			return *this;
		};
		SetEvent &event_type(tSM_INT v) {
			eventp_.event_type = v;
			return *this;
		};
		SetEvent &issue_events(tSM_INT v) {
			eventp_.issue_events = v;
			return *this;
		};
		SetEvent &event(tSMEventId v) {
			eventp_.event = v;
			return *this;
		};
	};
public:
	SetEvent set_event() {
		return SetEvent().channel(channel_);
	}
private:
	class GetModuleId {
		mutable int used_;
		tSMChannelId channel_;
	public:
		GetModuleId(const GetModuleId &old) {
			*this = old;
			old.used_ = 1;
		};
		GetModuleId(tSMChannelId channel) : used_(0) {
			channel_ = channel;
		};
		operator Error() {
			Error e(Prosody::sm_get_channel_module_id(channel_));
			used_ = 1;
			return e;
		};
		~GetModuleId() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_get_channel_module_id not called\n");
				abort();
			};
		};
	};
public:
	GetModuleId get_module_id() {
		return GetModuleId(channel_);
	}
private:
	class GetDatafeed {
		mutable int used_;
		SM_CHANNEL_DATAFEED_PARMS datafeedp_;
		tSMDatafeedId *datafeedP_;
	public:
		GetDatafeed(const GetDatafeed &old) {
			*this = old;
			old.used_ = 1;
		};
		GetDatafeed() : used_(0) {
			memset(&datafeedp_, 0, sizeof(datafeedp_));
			datafeedP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_channel_get_datafeed(&datafeedp_));
			used_ = 1;
			if (datafeedP_) *datafeedP_ = datafeedp_.datafeed;
			return e;
		};
		~GetDatafeed() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_channel_get_datafeed not called\n");
				abort();
			};
		};
		GetDatafeed &channel(tSMChannelId v) {
			datafeedp_.channel = v;
			return *this;
		};
		GetDatafeed &datafeed(tSMDatafeedId *v) {
			datafeedP_ = v;
			return *this;
		};
	};
public:
	GetDatafeed get_datafeed() {
		return GetDatafeed().channel(channel_);
	}
private:
	class DatafeedConnect {
		mutable int used_;
		SM_CHANNEL_DATAFEED_CONNECT_PARMS datafeedp_;
	public:
		DatafeedConnect(const DatafeedConnect &old) {
			*this = old;
			old.used_ = 1;
		};
		DatafeedConnect() : used_(0) {
			memset(&datafeedp_, 0, sizeof(datafeedp_));
		};
		operator Error() {
			Error e(Prosody::sm_channel_datafeed_connect(&datafeedp_));
			used_ = 1;
			return e;
		};
		~DatafeedConnect() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_channel_datafeed_connect not called\n");
				abort();
			};
		};
		DatafeedConnect &data_source(tSMDatafeedId v) {
			datafeedp_.data_source = v;
			return *this;
		};
		DatafeedConnect &channel(tSMChannelId v) {
			datafeedp_.channel = v;
			return *this;
		};
	};
public:
	DatafeedConnect datafeed_connect() {
		return DatafeedConnect().channel(channel_);
	}
private:
	class CollectorConnect {
		mutable int used_;
		SM_CHANNEL_COLLECTOR_CONNECT_PARMS dp_;
	public:
		CollectorConnect(const CollectorConnect &old) {
			*this = old;
			old.used_ = 1;
		};
		CollectorConnect() : used_(0) {
			memset(&dp_, 0, sizeof(dp_));
		};
		operator Error() {
			Error e(Prosody::sm_channel_collector_connect(&dp_));
			used_ = 1;
			return e;
		};
		~CollectorConnect() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_channel_collector_connect not called\n");
				abort();
			};
		};
		CollectorConnect &source(tSMCollectorId v) {
			dp_.source = v;
			return *this;
		};
		CollectorConnect &channel(tSMChannelId v) {
			dp_.channel = v;
			return *this;
		};
	};
public:
	CollectorConnect collector_connect() {
		return CollectorConnect().channel(channel_);
	}
private:
	class DispatcherConnect {
		mutable int used_;
		SM_CHANNEL_DISPATCHER_CONNECT_PARMS dp_;
	public:
		DispatcherConnect(const DispatcherConnect &old) {
			*this = old;
			old.used_ = 1;
		};
		DispatcherConnect() : used_(0) {
			memset(&dp_, 0, sizeof(dp_));
		};
		operator Error() {
			Error e(Prosody::sm_channel_dispatcher_connect(&dp_));
			used_ = 1;
			return e;
		};
		~DispatcherConnect() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_channel_dispatcher_connect not called\n");
				abort();
			};
		};
		DispatcherConnect &dest(tSMDispatcherId v) {
			dp_.dest = v;
			return *this;
		};
		DispatcherConnect &channel(tSMChannelId v) {
			dp_.channel = v;
			return *this;
		};
	};
public:
	DispatcherConnect dispatcher_connect() {
		return DispatcherConnect().channel(channel_);
	}
private:
	class InputDataRedirect {
		mutable int used_;
		SM_CHANNEL_INPUT_DATA_REDIRECT_PARMS redp_;
		tSMDatafeedId *datafeedP_;
	public:
		InputDataRedirect(const InputDataRedirect &old) {
			*this = old;
			old.used_ = 1;
		};
		InputDataRedirect() : used_(0) {
			memset(&redp_, 0, sizeof(redp_));
			datafeedP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_channel_input_data_redirect(&redp_));
			used_ = 1;
			if (datafeedP_) *datafeedP_ = redp_.datafeed;
			return e;
		};
		~InputDataRedirect() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_channel_input_data_redirect not called\n");
				abort();
			};
		};
		InputDataRedirect &channel(tSMChannelId v) {
			redp_.channel = v;
			return *this;
		};
		InputDataRedirect &enable(tSM_INT v) {
			redp_.enable = v;
			return *this;
		};
		InputDataRedirect &datafeed(tSMDatafeedId *v) {
			datafeedP_ = v;
			return *this;
		};
	};
public:
	InputDataRedirect input_data_redirect() {
		return InputDataRedirect().channel(channel_);
	}
private:
	class OutputSourceDatafeedConnect {
		mutable int used_;
		SM_CHANNEL_OUTPUT_SOURCE_DATAFEED_CONNECT_PARMS outsrcp_;
	public:
		OutputSourceDatafeedConnect(const OutputSourceDatafeedConnect &old) {
			*this = old;
			old.used_ = 1;
		};
		OutputSourceDatafeedConnect() : used_(0) {
			memset(&outsrcp_, 0, sizeof(outsrcp_));
		};
		operator Error() {
			Error e(Prosody::sm_channel_output_source_datafeed_connect(&outsrcp_));
			used_ = 1;
			return e;
		};
		~OutputSourceDatafeedConnect() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_channel_output_source_datafeed_connect not called\n");
				abort();
			};
		};
		OutputSourceDatafeedConnect &channel(tSMChannelId v) {
			outsrcp_.channel = v;
			return *this;
		};
		OutputSourceDatafeedConnect &data_source(tSMDatafeedId v) {
			outsrcp_.data_source = v;
			return *this;
		};
		OutputSourceDatafeedConnect &threshold(tSM_INT v) {
			outsrcp_.threshold = v;
			return *this;
		};
		OutputSourceDatafeedConnect &timeout(tSM_INT v) {
			outsrcp_.timeout = v;
			return *this;
		};
	};
public:
	OutputSourceDatafeedConnect output_source_datafeed_connect() {
		return OutputSourceDatafeedConnect().channel(channel_);
	}
