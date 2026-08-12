private:
	class Create {
		mutable int used_;
		SM_AVFREC_CREATE_PARMS vcp_;
		tSMAVFrecId *avfrecP_;
	public:
		Create(const Create &old) {
			*this = old;
			old.used_ = 1;
		};
		Create() : used_(0) {
			memset(&vcp_, 0, sizeof(vcp_));
			avfrecP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_avfrec_create(&vcp_));
			used_ = 1;
			if (avfrecP_) *avfrecP_ = vcp_.avfrec;
			return e;
		};
		~Create() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_avfrec_create not called\n");
				abort();
			};
		};
		Create &avfrec(tSMAVFrecId *v) {
			avfrecP_ = v;
			return *this;
		};
		Create &module(tSMModuleId v) {
			vcp_.module = v;
			return *this;
		};
	};
public:
	Create create() {
		return Create().avfrec(&avfrec_);
	}
private:
	class Destroy {
		mutable int used_;
		tSMAVFrecId avfrec_;
	public:
		Destroy(const Destroy &old) {
			*this = old;
			old.used_ = 1;
		};
		Destroy(tSMAVFrecId avfrec) : used_(0) {
			avfrec_ = avfrec;
		};
		operator Error() {
			Error e(Prosody::sm_avfrec_destroy(avfrec_));
			used_ = 1;
			return e;
		};
		~Destroy() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_avfrec_destroy not called\n");
				abort();
			};
		};
	};
public:
	Destroy destroy() {
		Destroy temp(avfrec_);
		
		avfrec_ = 0;
		return temp;
	}
private:
	class Sync {
		mutable int used_;
		SM_AVFREC_SYNC_PARMS avfsp_;
	public:
		Sync(const Sync &old) {
			*this = old;
			old.used_ = 1;
		};
		Sync() : used_(0) {
			memset(&avfsp_, 0, sizeof(avfsp_));
		};
		operator Error() {
			Error e(Prosody::sm_avfrec_sync(&avfsp_));
			used_ = 1;
			return e;
		};
		~Sync() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_avfrec_sync not called\n");
				abort();
			};
		};
		Sync &avfrec(tSMAVFrecId v) {
			avfsp_.avfrec = v;
			return *this;
		};
		Sync &sync_avfrec(tSMAVFrecId v) {
			avfsp_.sync_avfrec = v;
			return *this;
		};
	};
public:
	Sync sync() {
		return Sync().avfrec(avfrec_);
	}
private:
	class Start {
		mutable int used_;
		SM_AVFREC_START_PARMS avfsp_;
	public:
		Start(const Start &old) {
			*this = old;
			old.used_ = 1;
		};
		Start() : used_(0) {
			memset(&avfsp_, 0, sizeof(avfsp_));
		};
		operator Error() {
			Error e(Prosody::sm_avfrec_start(&avfsp_));
			used_ = 1;
			return e;
		};
		~Start() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_avfrec_start not called\n");
				abort();
			};
		};
		Start &avfrec(tSMAVFrecId v) {
			avfsp_.avfrec = v;
			return *this;
		};
		Start &src_type(enum kSMAVFInputType v) {
			avfsp_.src_type = v;
			return *this;
		};
		Start &u_src_type_audio_payload_type(tSM_INT v) {
			avfsp_.u_src_type.audio.payload_type = v;
			return *this;
		};
		Start &u_src_type_audio_dest_type(enum kSMAudioDataFormat v) {
			avfsp_.u_src_type.audio.dest_type = v;
			return *this;
		};
		Start &u_src_type_audio_u_dest_type_amrnb_bitrate(tSM_INT v) {
			avfsp_.u_src_type.audio.u_dest_type.amrnb.bitrate = v;
			return *this;
		};
		Start &u_src_type_audio_u_dest_type_amrnb_vad_enable(tSM_INT v) {
			avfsp_.u_src_type.audio.u_dest_type.amrnb.vad_enable = v;
			return *this;
		};
		Start &hbuf_size(tSM_INT v) {
			avfsp_.hbuf_size = v;
			return *this;
		};
		Start &test_mode(tSM_INT v) {
			avfsp_.test_mode = v;
			return *this;
		};
	};
public:
	Start start() {
		return Start().avfrec(avfrec_);
	}
private:
	class Stop {
		mutable int used_;
		SM_AVFREC_STOP_PARMS avfsp_;
	public:
		Stop(const Stop &old) {
			*this = old;
			old.used_ = 1;
		};
		Stop() : used_(0) {
			memset(&avfsp_, 0, sizeof(avfsp_));
		};
		operator Error() {
			Error e(Prosody::sm_avfrec_stop(&avfsp_));
			used_ = 1;
			return e;
		};
		~Stop() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_avfrec_stop not called\n");
				abort();
			};
		};
		Stop &avfrec(tSMAVFrecId v) {
			avfsp_.avfrec = v;
			return *this;
		};
		Stop &immediate(tSM_INT v) {
			avfsp_.immediate = v;
			return *this;
		};
	};
public:
	Stop stop() {
		return Stop().avfrec(avfrec_);
	}
private:
	class DatafeedConnect {
		mutable int used_;
		SM_AVFREC_DATAFEED_CONNECT_PARMS datafeedp_;
	public:
		DatafeedConnect(const DatafeedConnect &old) {
			*this = old;
			old.used_ = 1;
		};
		DatafeedConnect() : used_(0) {
			memset(&datafeedp_, 0, sizeof(datafeedp_));
		};
		operator Error() {
			Error e(Prosody::sm_avfrec_datafeed_connect(&datafeedp_));
			used_ = 1;
			return e;
		};
		~DatafeedConnect() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_avfrec_datafeed_connect not called\n");
				abort();
			};
		};
		DatafeedConnect &avfrec(tSMAVFrecId v) {
			datafeedp_.avfrec = v;
			return *this;
		};
		DatafeedConnect &data_source(tSMDatafeedId v) {
			datafeedp_.data_source = v;
			return *this;
		};
	};
public:
	DatafeedConnect datafeed_connect() {
		return DatafeedConnect().avfrec(avfrec_);
	}
private:
	class GetData {
		mutable int used_;
		SM_AVFREC_GET_DATA_PARMS datap_;
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
			Error e(Prosody::sm_avfrec_get_data(&datap_));
			used_ = 1;
			if (done_lengthP_) *done_lengthP_ = datap_.done_length;
			return e;
		};
		~GetData() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_avfrec_get_data not called\n");
				abort();
			};
		};
		GetData &avfrec(tSMAVFrecId v) {
			datap_.avfrec = v;
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
		return GetData().avfrec(avfrec_);
	}
private:
	class GetEvent {
		mutable int used_;
		SM_AVFREC_EVENT_PARMS eventp_;
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
			Error e(Prosody::sm_avfrec_get_event(&eventp_));
			used_ = 1;
			if (eventP_) *eventP_ = eventp_.event;
			return e;
		};
		~GetEvent() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_avfrec_get_event not called\n");
				abort();
			};
		};
		GetEvent &avfrec(tSMAVFrecId v) {
			eventp_.avfrec = v;
			return *this;
		};
		GetEvent &event(tSMEventId *v) {
			eventP_ = v;
			return *this;
		};
	};
public:
	GetEvent get_event() {
		return GetEvent().avfrec(avfrec_);
	}
private:
	class Status {
		mutable int used_;
		SM_AVFREC_STATUS_PARMS statusp_;
		enum kSMAVFrecordStatus *statusP_;
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
			Error e(Prosody::sm_avfrec_status(&statusp_));
			used_ = 1;
			if (statusP_) *statusP_ = statusp_.status;
			return e;
		};
		~Status() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_avfrec_status not called\n");
				abort();
			};
		};
		Status &avfrec(tSMAVFrecId v) {
			statusp_.avfrec = v;
			return *this;
		};
		Status &status(enum kSMAVFrecordStatus *v) {
			statusP_ = v;
			return *this;
		};
	};
public:
	Status status() {
		return Status().avfrec(avfrec_);
	}
