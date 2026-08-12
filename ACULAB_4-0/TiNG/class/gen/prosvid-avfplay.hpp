private:
	class Create {
		mutable int used_;
		SM_AVFPLAY_CREATE_PARMS avfcp_;
		tSMAVFplayId *avfplayP_;
	public:
		Create(const Create &old) {
			*this = old;
			old.used_ = 1;
		};
		Create() : used_(0) {
			memset(&avfcp_, 0, sizeof(avfcp_));
			avfplayP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_avfplay_create(&avfcp_));
			used_ = 1;
			if (avfplayP_) *avfplayP_ = avfcp_.avfplay;
			return e;
		};
		~Create() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_avfplay_create not called\n");
				abort();
			};
		};
		Create &avfplay(tSMAVFplayId *v) {
			avfplayP_ = v;
			return *this;
		};
		Create &module(tSMModuleId v) {
			avfcp_.module = v;
			return *this;
		};
	};
public:
	Create create() {
		return Create().avfplay(&avfplay_);
	}
private:
	class Destroy {
		mutable int used_;
		tSMAVFplayId avfplay_;
	public:
		Destroy(const Destroy &old) {
			*this = old;
			old.used_ = 1;
		};
		Destroy(tSMAVFplayId avfplay) : used_(0) {
			avfplay_ = avfplay;
		};
		operator Error() {
			Error e(Prosody::sm_avfplay_destroy(avfplay_));
			used_ = 1;
			return e;
		};
		~Destroy() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_avfplay_destroy not called\n");
				abort();
			};
		};
	};
public:
	Destroy destroy() {
		Destroy temp(avfplay_);
		
		avfplay_ = 0;
		return temp;
	}
private:
	class Sync {
		mutable int used_;
		SM_AVFPLAY_SYNC_PARMS avfsp_;
	public:
		Sync(const Sync &old) {
			*this = old;
			old.used_ = 1;
		};
		Sync() : used_(0) {
			memset(&avfsp_, 0, sizeof(avfsp_));
		};
		operator Error() {
			Error e(Prosody::sm_avfplay_sync(&avfsp_));
			used_ = 1;
			return e;
		};
		~Sync() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_avfplay_sync not called\n");
				abort();
			};
		};
		Sync &avfplay(tSMAVFplayId v) {
			avfsp_.avfplay = v;
			return *this;
		};
		Sync &sync_avfplay(tSMAVFplayId v) {
			avfsp_.sync_avfplay = v;
			return *this;
		};
	};
public:
	Sync sync() {
		return Sync().avfplay(avfplay_);
	}
private:
	class Start {
		mutable int used_;
		SM_AVFPLAY_START_PARMS avfsp_;
	public:
		Start(const Start &old) {
			*this = old;
			old.used_ = 1;
		};
		Start() : used_(0) {
			memset(&avfsp_, 0, sizeof(avfsp_));
		};
		operator Error() {
			Error e(Prosody::sm_avfplay_start(&avfsp_));
			used_ = 1;
			return e;
		};
		~Start() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_avfplay_start not called\n");
				abort();
			};
		};
		Start &avfplay(tSMAVFplayId v) {
			avfsp_.avfplay = v;
			return *this;
		};
		Start &dest_type(enum kSMAVFOutputType v) {
			avfsp_.dest_type = v;
			return *this;
		};
		Start &u_dest_type_audio_sampling_rate(tSM_INT v) {
			avfsp_.u_dest_type.audio.sampling_rate = v;
			return *this;
		};
		Start &u_dest_type_audio_src_type(enum kSMAudioDataFormat v) {
			avfsp_.u_dest_type.audio.src_type = v;
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
		return Start().avfplay(avfplay_);
	}
private:
	class Stop {
		mutable int used_;
		SM_AVFPLAY_STOP_PARMS avfsp_;
	public:
		Stop(const Stop &old) {
			*this = old;
			old.used_ = 1;
		};
		Stop() : used_(0) {
			memset(&avfsp_, 0, sizeof(avfsp_));
		};
		operator Error() {
			Error e(Prosody::sm_avfplay_stop(&avfsp_));
			used_ = 1;
			return e;
		};
		~Stop() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_avfplay_stop not called\n");
				abort();
			};
		};
		Stop &avfplay(tSMAVFplayId v) {
			avfsp_.avfplay = v;
			return *this;
		};
		Stop &immediate(tSM_INT v) {
			avfsp_.immediate = v;
			return *this;
		};
	};
public:
	Stop stop() {
		return Stop().avfplay(avfplay_);
	}
private:
	class GetDatafeed {
		mutable int used_;
		SM_AVFPLAY_DATAFEED_PARMS datafeedp_;
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
			Error e(Prosody::sm_avfplay_get_datafeed(&datafeedp_));
			used_ = 1;
			if (datafeedP_) *datafeedP_ = datafeedp_.datafeed;
			return e;
		};
		~GetDatafeed() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_avfplay_get_datafeed not called\n");
				abort();
			};
		};
		GetDatafeed &avfplay(tSMAVFplayId v) {
			datafeedp_.avfplay = v;
			return *this;
		};
		GetDatafeed &datafeed(tSMDatafeedId *v) {
			datafeedP_ = v;
			return *this;
		};
	};
public:
	GetDatafeed get_datafeed() {
		return GetDatafeed().avfplay(avfplay_);
	}
private:
	class GetEvent {
		mutable int used_;
		SM_AVFPLAY_EVENT_PARMS eventp_;
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
			Error e(Prosody::sm_avfplay_get_event(&eventp_));
			used_ = 1;
			if (eventP_) *eventP_ = eventp_.event;
			return e;
		};
		~GetEvent() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_avfplay_get_event not called\n");
				abort();
			};
		};
		GetEvent &avfplay(tSMAVFplayId v) {
			eventp_.avfplay = v;
			return *this;
		};
		GetEvent &event(tSMEventId *v) {
			eventP_ = v;
			return *this;
		};
	};
public:
	GetEvent get_event() {
		return GetEvent().avfplay(avfplay_);
	}
private:
	class PutData {
		mutable int used_;
		SM_AVFPLAY_PUT_DATA_PARMS datap_;
		tSM_INT *done_lengthP_;
	public:
		PutData(const PutData &old) {
			*this = old;
			old.used_ = 1;
		};
		PutData() : used_(0) {
			memset(&datap_, 0, sizeof(datap_));
			done_lengthP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_avfplay_put_data(&datap_));
			used_ = 1;
			if (done_lengthP_) *done_lengthP_ = datap_.done_length;
			return e;
		};
		~PutData() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_avfplay_put_data not called\n");
				abort();
			};
		};
		PutData &avfplay(tSMAVFplayId v) {
			datap_.avfplay = v;
			return *this;
		};
		PutData &data(char* v) {
			datap_.data = v;
			return *this;
		};
		PutData &max_length(tSM_INT v) {
			datap_.max_length = v;
			return *this;
		};
		PutData &done_length(tSM_INT *v) {
			done_lengthP_ = v;
			return *this;
		};
	};
public:
	PutData put_data() {
		return PutData().avfplay(avfplay_);
	}
private:
	class Status {
		mutable int used_;
		SM_AVFPLAY_STATUS_PARMS statusp_;
		enum kSMAVFplayStatus *statusP_;
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
			Error e(Prosody::sm_avfplay_status(&statusp_));
			used_ = 1;
			if (statusP_) *statusP_ = statusp_.status;
			return e;
		};
		~Status() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_avfplay_status not called\n");
				abort();
			};
		};
		Status &avfplay(tSMAVFplayId v) {
			statusp_.avfplay = v;
			return *this;
		};
		Status &status(enum kSMAVFplayStatus *v) {
			statusP_ = v;
			return *this;
		};
	};
public:
	Status status() {
		return Status().avfplay(avfplay_);
	}
