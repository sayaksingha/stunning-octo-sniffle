private:
	class ConfigBandwidth {
		mutable int used_;
		SM_RTCPHAND_CONFIG_BANDWIDTH_PARMS rvp_;
	public:
		ConfigBandwidth(const ConfigBandwidth &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigBandwidth() : used_(0) {
			memset(&rvp_, 0, sizeof(rvp_));
		};
		operator Error() {
			Error e(Prosody::sm_rtcphand_config_bandwidth(&rvp_));
			used_ = 1;
			return e;
		};
		~ConfigBandwidth() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_rtcphand_config_bandwidth not called\n");
				abort();
			};
		};
		ConfigBandwidth &rtcphand(tSMRTCPHandId v) {
			rvp_.rtcphand = v;
			return *this;
		};
		ConfigBandwidth &rtcp_bw(tSM_INT v) {
			rvp_.rtcp_bw = v;
			return *this;
		};
		ConfigBandwidth &rtcprx_bw(tSM_INT v) {
			rvp_.rtcprx_bw = v;
			return *this;
		};
		ConfigBandwidth &rtcptx_bw(tSM_INT v) {
			rvp_.rtcptx_bw = v;
			return *this;
		};
	};
public:
	ConfigBandwidth config_bandwidth() {
		return ConfigBandwidth().rtcphand(rtcphand_);
	}
private:
	class ConfigReports {
		mutable int used_;
		SM_RTCPHAND_CONFIG_REPORTS_PARMS rvp_;
	public:
		ConfigReports(const ConfigReports &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigReports() : used_(0) {
			memset(&rvp_, 0, sizeof(rvp_));
		};
		operator Error() {
			Error e(Prosody::sm_rtcphand_config_reports(&rvp_));
			used_ = 1;
			return e;
		};
		~ConfigReports() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_rtcphand_config_reports not called\n");
				abort();
			};
		};
		ConfigReports &rtcphand(tSMRTCPHandId v) {
			rvp_.rtcphand = v;
			return *this;
		};
		ConfigReports &reports(tSM_INT v) {
			rvp_.reports = v;
			return *this;
		};
	};
public:
	ConfigReports config_reports() {
		return ConfigReports().rtcphand(rtcphand_);
	}
private:
	class ConfigOptions {
		mutable int used_;
		SM_RTCPHAND_CONFIG_OPTIONS_PARMS rvp_;
	public:
		ConfigOptions(const ConfigOptions &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigOptions() : used_(0) {
			memset(&rvp_, 0, sizeof(rvp_));
		};
		operator Error() {
			Error e(Prosody::sm_rtcphand_config_options(&rvp_));
			used_ = 1;
			return e;
		};
		~ConfigOptions() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_rtcphand_config_options not called\n");
				abort();
			};
		};
		ConfigOptions &rtcphand(tSMRTCPHandId v) {
			rvp_.rtcphand = v;
			return *this;
		};
		ConfigOptions &rtcptx_suppress_xr(tSM_INT v) {
			rvp_.rtcptx_suppress_xr = v;
			return *this;
		};
	};
public:
	ConfigOptions config_options() {
		return ConfigOptions().rtcphand(rtcphand_);
	}
private:
	class ConfigSdes {
		mutable int used_;
		SM_RTCPHAND_CONFIG_SDES_PARMS rvp_;
	public:
		ConfigSdes(const ConfigSdes &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigSdes() : used_(0) {
			memset(&rvp_, 0, sizeof(rvp_));
		};
		operator Error() {
			Error e(Prosody::sm_rtcphand_config_sdes(&rvp_));
			used_ = 1;
			return e;
		};
		~ConfigSdes() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_rtcphand_config_sdes not called\n");
				abort();
			};
		};
		ConfigSdes &rtcphand(tSMRTCPHandId v) {
			rvp_.rtcphand = v;
			return *this;
		};
		ConfigSdes &itemnum(tSM_INT v) {
			rvp_.itemnum = v;
			return *this;
		};
		ConfigSdes &stringlen(tSM_INT v) {
			rvp_.stringlen = v;
			return *this;
		};
		ConfigSdes &stringval(char* v) {
			rvp_.stringval = v;
			return *this;
		};
	};
public:
	ConfigSdes config_sdes() {
		return ConfigSdes().rtcphand(rtcphand_);
	}
private:
	class Create {
		mutable int used_;
		SM_RTCPHAND_CREATE_PARMS rtcphandp_;
		tSMRTCPHandId *rtcphandP_;
	public:
		Create(const Create &old) {
			*this = old;
			old.used_ = 1;
		};
		Create() : used_(0) {
			memset(&rtcphandp_, 0, sizeof(rtcphandp_));
			rtcphandP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_rtcphand_create(&rtcphandp_));
			used_ = 1;
			if (rtcphandP_) *rtcphandP_ = rtcphandp_.rtcphand;
			return e;
		};
		~Create() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_rtcphand_create not called\n");
				abort();
			};
		};
		Create &rtcphand(tSMRTCPHandId *v) {
			rtcphandP_ = v;
			return *this;
		};
		Create &module(tSMModuleId v) {
			rtcphandp_.module = v;
			return *this;
		};
	};
public:
	Create create() {
		return Create().rtcphand(&rtcphand_);
	}
private:
	class Destroy {
		mutable int used_;
		tSMRTCPHandId rtcphand_;
	public:
		Destroy(const Destroy &old) {
			*this = old;
			old.used_ = 1;
		};
		Destroy(tSMRTCPHandId rtcphand) : used_(0) {
			rtcphand_ = rtcphand;
		};
		operator Error() {
			Error e(Prosody::sm_rtcphand_destroy(rtcphand_));
			used_ = 1;
			return e;
		};
		~Destroy() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_rtcphand_destroy not called\n");
				abort();
			};
		};
	};
public:
	Destroy destroy() {
		Destroy temp(rtcphand_);
		
		rtcphand_ = 0;
		return temp;
	}
private:
	class GetData {
		mutable int used_;
		SM_RTCPHAND_GET_DATA_PARMS datap_;
		tSM_INT *done_lengthP_;
	public:
		GetData(const GetData &old) {
			*this = old;
			old.used_ = 1;
		};
		GetData() : used_(0) {
			memset(&datap_, 0, sizeof(datap_));
			done_lengthP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_rtcphand_get_data(&datap_));
			used_ = 1;
			if (done_lengthP_) *done_lengthP_ = datap_.done_length;
			return e;
		};
		~GetData() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_rtcphand_get_data not called\n");
				abort();
			};
		};
		GetData &rtcphand(tSMRTCPHandId v) {
			datap_.rtcphand = v;
			return *this;
		};
		GetData &data(char* v) {
			datap_.data = v;
			return *this;
		};
		GetData &max_length(tSM_INT v) {
			datap_.max_length = v;
			return *this;
		};
		GetData &done_length(tSM_INT *v) {
			done_lengthP_ = v;
			return *this;
		};
	};
public:
	GetData get_data() {
		return GetData().rtcphand(rtcphand_);
	}
private:
	class GetEvent {
		mutable int used_;
		SM_RTCPHAND_EVENT_PARMS eventp_;
		tSMEventId *eventP_;
	public:
		GetEvent(const GetEvent &old) {
			*this = old;
			old.used_ = 1;
		};
		GetEvent() : used_(0) {
			memset(&eventp_, 0, sizeof(eventp_));
			eventP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_rtcphand_get_event(&eventp_));
			used_ = 1;
			if (eventP_) *eventP_ = eventp_.event;
			return e;
		};
		~GetEvent() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_rtcphand_get_event not called\n");
				abort();
			};
		};
		GetEvent &rtcphand(tSMRTCPHandId v) {
			eventp_.rtcphand = v;
			return *this;
		};
		GetEvent &event(tSMEventId *v) {
			eventP_ = v;
			return *this;
		};
	};
public:
	GetEvent get_event() {
		return GetEvent().rtcphand(rtcphand_);
	}
private:
	class RequestStatistics {
		mutable int used_;
		SM_RTCPHAND_REQUEST_STATISTICS_PARMS rrsp_;
	public:
		RequestStatistics(const RequestStatistics &old) {
			*this = old;
			old.used_ = 1;
		};
		RequestStatistics() : used_(0) {
			memset(&rrsp_, 0, sizeof(rrsp_));
		};
		operator Error() {
			Error e(Prosody::sm_rtcphand_request_statistics(&rrsp_));
			used_ = 1;
			return e;
		};
		~RequestStatistics() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_rtcphand_request_statistics not called\n");
				abort();
			};
		};
		RequestStatistics &rtcphand(tSMRTCPHandId v) {
			rrsp_.rtcphand = v;
			return *this;
		};
		RequestStatistics &statcode(tSM_INT v) {
			rrsp_.statcode = v;
			return *this;
		};
	};
public:
	RequestStatistics request_statistics() {
		return RequestStatistics().rtcphand(rtcphand_);
	}
private:
	class Status {
		mutable int used_;
		SM_RTCPHAND_STATUS_PARMS statusp_;
		enum kSMRTCPHandStatus *statusP_;
	public:
		Status(const Status &old) {
			*this = old;
			old.used_ = 1;
		};
		Status() : used_(0) {
			memset(&statusp_, 0, sizeof(statusp_));
			statusP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_rtcphand_status(&statusp_));
			used_ = 1;
			if (statusP_) *statusP_ = statusp_.status;
			return e;
		};
		~Status() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_rtcphand_status not called\n");
				abort();
			};
		};
		Status &rtcphand(tSMRTCPHandId v) {
			statusp_.rtcphand = v;
			return *this;
		};
		Status &status(enum kSMRTCPHandStatus *v) {
			statusP_ = v;
			return *this;
		};
	};
public:
	Status status() {
		return Status().rtcphand(rtcphand_);
	}
