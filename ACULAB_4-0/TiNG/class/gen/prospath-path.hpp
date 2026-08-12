private:
	class Create {
		mutable int used_;
		SM_PATH_CREATE_PARMS createp_;
		tSMPathId *pathP_;
	public:
		Create(const Create &old) {
			*this = old;
			old.used_ = 1;
		};
		Create() : used_(0) {
			memset(&createp_, 0, sizeof(createp_));
			pathP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_path_create(&createp_));
			used_ = 1;
			if (pathP_) *pathP_ = createp_.path;
			return e;
		};
		~Create() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_path_create not called\n");
				abort();
			};
		};
		Create &path(tSMPathId *v) {
			pathP_ = v;
			return *this;
		};
		Create &module(tSMModuleId v) {
			createp_.module = v;
			return *this;
		};
	};
public:
	Create create() {
		return Create().path(&path_);
	}
private:
	class Echocancel {
		mutable int used_;
		SM_PATH_ECHOCANCEL_PARMS cancelp_;
	public:
		Echocancel(const Echocancel &old) {
			*this = old;
			old.used_ = 1;
		};
		Echocancel() : used_(0) {
			memset(&cancelp_, 0, sizeof(cancelp_));
		};
		operator Error() {
			Error e(Prosody::sm_path_echocancel(&cancelp_));
			used_ = 1;
			return e;
		};
		~Echocancel() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_path_echocancel not called\n");
				abort();
			};
		};
		Echocancel &path(tSMPathId v) {
			cancelp_.path = v;
			return *this;
		};
		Echocancel &enable(tSM_INT v) {
			cancelp_.enable = v;
			return *this;
		};
		Echocancel &reference(tSMDatafeedId v) {
			cancelp_.reference = v;
			return *this;
		};
		Echocancel &non_linear(tSM_INT v) {
			cancelp_.non_linear = v;
			return *this;
		};
		Echocancel &mode(enum kSMPathEchoNonLinearMode v) {
			cancelp_.mode = v;
			return *this;
		};
		Echocancel &use_agc(tSM_INT v) {
			cancelp_.use_agc = v;
			return *this;
		};
		Echocancel &fix_agc(tSM_INT v) {
			cancelp_.fix_agc = v;
			return *this;
		};
		Echocancel &span(tSM_INT v) {
			cancelp_.span = v;
			return *this;
		};
		Echocancel &delay(tSM_INT v) {
			cancelp_.delay = v;
			return *this;
		};
		Echocancel &tone_action(enum kSMPathEchoToneAction v) {
			cancelp_.tone_action = v;
			return *this;
		};
	};
public:
	Echocancel echocancel() {
		return Echocancel().path(path_);
	}
private:
	class Agc {
		mutable int used_;
		SM_PATH_AGC_PARMS agcp_;
	public:
		Agc(const Agc &old) {
			*this = old;
			old.used_ = 1;
		};
		Agc() : used_(0) {
			memset(&agcp_, 0, sizeof(agcp_));
		};
		operator Error() {
			Error e(Prosody::sm_path_agc(&agcp_));
			used_ = 1;
			return e;
		};
		~Agc() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_path_agc not called\n");
				abort();
			};
		};
		Agc &path(tSMPathId v) {
			agcp_.path = v;
			return *this;
		};
		Agc &agc(tSM_INT v) {
			agcp_.agc = v;
			return *this;
		};
		Agc &volume(tSM_INT v) {
			agcp_.volume = v;
			return *this;
		};
	};
public:
	Agc agc() {
		return Agc().path(path_);
	}
private:
	class AgcAdjustSettings {
		mutable int used_;
		SM_PATH_AGC_ADJUST_SETTINGS_PARMS agcadjp_;
	public:
		AgcAdjustSettings(const AgcAdjustSettings &old) {
			*this = old;
			old.used_ = 1;
		};
		AgcAdjustSettings() : used_(0) {
			memset(&agcadjp_, 0, sizeof(agcadjp_));
		};
		operator Error() {
			Error e(Prosody::sm_path_agc_adjust_settings(&agcadjp_));
			used_ = 1;
			return e;
		};
		~AgcAdjustSettings() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_path_agc_adjust_settings not called\n");
				abort();
			};
		};
		AgcAdjustSettings &path(tSMPathId v) {
			agcadjp_.path = v;
			return *this;
		};
		AgcAdjustSettings &max_level_decay(float v) {
			agcadjp_.max_level_decay = v;
			return *this;
		};
		AgcAdjustSettings &target_level(float v) {
			agcadjp_.target_level = v;
			return *this;
		};
	};
public:
	AgcAdjustSettings agc_adjust_settings() {
		return AgcAdjustSettings().path(path_);
	}
private:
	class Pitchshift {
		mutable int used_;
		SM_PATH_PITCHSHIFT_PARMS pitchshiftp_;
	public:
		Pitchshift(const Pitchshift &old) {
			*this = old;
			old.used_ = 1;
		};
		Pitchshift() : used_(0) {
			memset(&pitchshiftp_, 0, sizeof(pitchshiftp_));
		};
		operator Error() {
			Error e(Prosody::sm_path_pitchshift(&pitchshiftp_));
			used_ = 1;
			return e;
		};
		~Pitchshift() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_path_pitchshift not called\n");
				abort();
			};
		};
		Pitchshift &path(tSMPathId v) {
			pitchshiftp_.path = v;
			return *this;
		};
		Pitchshift &shift(float v) {
			pitchshiftp_.shift = v;
			return *this;
		};
	};
public:
	Pitchshift pitchshift() {
		return Pitchshift().path(path_);
	}
private:
	class Mix {
		mutable int used_;
		SM_PATH_MIX_PARMS mixp_;
	public:
		Mix(const Mix &old) {
			*this = old;
			old.used_ = 1;
		};
		Mix() : used_(0) {
			memset(&mixp_, 0, sizeof(mixp_));
		};
		operator Error() {
			Error e(Prosody::sm_path_mix(&mixp_));
			used_ = 1;
			return e;
		};
		~Mix() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_path_mix not called\n");
				abort();
			};
		};
		Mix &path(tSMPathId v) {
			mixp_.path = v;
			return *this;
		};
		Mix &enable(tSM_INT v) {
			mixp_.enable = v;
			return *this;
		};
		Mix &mixin(tSMDatafeedId v) {
			mixp_.mixin = v;
			return *this;
		};
		Mix &volume(tSM_INT v) {
			mixp_.volume = v;
			return *this;
		};
	};
public:
	Mix mix() {
		return Mix().path(path_);
	}
private:
	class Resample {
		mutable int used_;
		SM_PATH_RESAMPLE_PARMS resamplep_;
	public:
		Resample(const Resample &old) {
			*this = old;
			old.used_ = 1;
		};
		Resample() : used_(0) {
			memset(&resamplep_, 0, sizeof(resamplep_));
		};
		operator Error() {
			Error e(Prosody::sm_path_resample(&resamplep_));
			used_ = 1;
			return e;
		};
		~Resample() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_path_resample not called\n");
				abort();
			};
		};
		Resample &path(tSMPathId v) {
			resamplep_.path = v;
			return *this;
		};
		Resample &uprate(tSM_INT v) {
			resamplep_.uprate = v;
			return *this;
		};
		Resample &downrate(tSM_INT v) {
			resamplep_.downrate = v;
			return *this;
		};
	};
public:
	Resample resample() {
		return Resample().path(path_);
	}
private:
	class GetDatafeed {
		mutable int used_;
		SM_PATH_DATAFEED_PARMS datafeedp_;
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
			Error e(Prosody::sm_path_get_datafeed(&datafeedp_));
			used_ = 1;
			if (datafeedP_) *datafeedP_ = datafeedp_.datafeed;
			return e;
		};
		~GetDatafeed() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_path_get_datafeed not called\n");
				abort();
			};
		};
		GetDatafeed &path(tSMPathId v) {
			datafeedp_.path = v;
			return *this;
		};
		GetDatafeed &datafeed(tSMDatafeedId *v) {
			datafeedP_ = v;
			return *this;
		};
	};
public:
	GetDatafeed get_datafeed() {
		return GetDatafeed().path(path_);
	}
private:
	class Destroy {
		mutable int used_;
		tSMPathId path_;
	public:
		Destroy(const Destroy &old) {
			*this = old;
			old.used_ = 1;
		};
		Destroy(tSMPathId path) : used_(0) {
			path_ = path;
		};
		operator Error() {
			Error e(Prosody::sm_path_destroy(path_));
			used_ = 1;
			return e;
		};
		~Destroy() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_path_destroy not called\n");
				abort();
			};
		};
	};
public:
	Destroy destroy() {
		Destroy temp(path_);
		
		path_ = 0;
		return temp;
	}
private:
	class Status {
		mutable int used_;
		SM_PATH_STATUS_PARMS statusp_;
		enum kSMPathStatus *statusP_;
		tSM_INT *u_tone_idP_;
		unsigned *u_tone_durationP_;
	public:
		Status(const Status &old) {
			*this = old;
			old.used_ = 1;
		};
		Status() : used_(0) {
			memset(&statusp_, 0, sizeof(statusp_));
			statusP_ = 0;
			u_tone_idP_ = 0;
			u_tone_durationP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_path_status(&statusp_));
			used_ = 1;
			if (statusP_) *statusP_ = statusp_.status;
			if (u_tone_idP_) *u_tone_idP_ = statusp_.u.tone.id;
			if (u_tone_durationP_) *u_tone_durationP_ = statusp_.u.tone.duration;
			return e;
		};
		~Status() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_path_status not called\n");
				abort();
			};
		};
		Status &path(tSMPathId v) {
			statusp_.path = v;
			return *this;
		};
		Status &status(enum kSMPathStatus *v) {
			statusP_ = v;
			return *this;
		};
		Status &u_tone_id(tSM_INT *v) {
			u_tone_idP_ = v;
			return *this;
		};
		Status &u_tone_duration(unsigned *v) {
			u_tone_durationP_ = v;
			return *this;
		};
	};
public:
	Status status() {
		return Status().path(path_);
	}
private:
	class GetEvent {
		mutable int used_;
		SM_PATH_EVENT_PARMS eventp_;
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
			Error e(Prosody::sm_path_get_event(&eventp_));
			used_ = 1;
			if (eventP_) *eventP_ = eventp_.event;
			return e;
		};
		~GetEvent() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_path_get_event not called\n");
				abort();
			};
		};
		GetEvent &path(tSMPathId v) {
			eventp_.path = v;
			return *this;
		};
		GetEvent &event(tSMEventId *v) {
			eventP_ = v;
			return *this;
		};
	};
public:
	GetEvent get_event() {
		return GetEvent().path(path_);
	}
private:
	class DatafeedConnect {
		mutable int used_;
		SM_PATH_DATAFEED_CONNECT_PARMS datafeedp_;
	public:
		DatafeedConnect(const DatafeedConnect &old) {
			*this = old;
			old.used_ = 1;
		};
		DatafeedConnect() : used_(0) {
			memset(&datafeedp_, 0, sizeof(datafeedp_));
		};
		operator Error() {
			Error e(Prosody::sm_path_datafeed_connect(&datafeedp_));
			used_ = 1;
			return e;
		};
		~DatafeedConnect() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_path_datafeed_connect not called\n");
				abort();
			};
		};
		DatafeedConnect &path(tSMPathId v) {
			datafeedp_.path = v;
			return *this;
		};
		DatafeedConnect &data_source(tSMDatafeedId v) {
			datafeedp_.data_source = v;
			return *this;
		};
	};
public:
	DatafeedConnect datafeed_connect() {
		return DatafeedConnect().path(path_);
	}
private:
	class AvfAmrnbDec {
		mutable int used_;
		SM_PATH_AVF_AMRNB_DEC_PARMS amrnbdecparms_;
	public:
		AvfAmrnbDec(const AvfAmrnbDec &old) {
			*this = old;
			old.used_ = 1;
		};
		AvfAmrnbDec() : used_(0) {
			memset(&amrnbdecparms_, 0, sizeof(amrnbdecparms_));
		};
		operator Error() {
			Error e(Prosody::sm_path_avf_amrnb_dec(&amrnbdecparms_));
			used_ = 1;
			return e;
		};
		~AvfAmrnbDec() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_path_avf_amrnb_dec not called\n");
				abort();
			};
		};
		AvfAmrnbDec &path(tSMPathId v) {
			amrnbdecparms_.path = v;
			return *this;
		};
		AvfAmrnbDec &enable(tSM_INT v) {
			amrnbdecparms_.enable = v;
			return *this;
		};
	};
public:
	AvfAmrnbDec avf_amrnb_dec() {
		return AvfAmrnbDec().path(path_);
	}
private:
	class AvfAmrnbEnc {
		mutable int used_;
		SM_PATH_AVF_AMRNB_ENC_PARMS amrnbencparms_;
	public:
		AvfAmrnbEnc(const AvfAmrnbEnc &old) {
			*this = old;
			old.used_ = 1;
		};
		AvfAmrnbEnc() : used_(0) {
			memset(&amrnbencparms_, 0, sizeof(amrnbencparms_));
		};
		operator Error() {
			Error e(Prosody::sm_path_avf_amrnb_enc(&amrnbencparms_));
			used_ = 1;
			return e;
		};
		~AvfAmrnbEnc() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_path_avf_amrnb_enc not called\n");
				abort();
			};
		};
		AvfAmrnbEnc &path(tSMPathId v) {
			amrnbencparms_.path = v;
			return *this;
		};
		AvfAmrnbEnc &enable(tSM_INT v) {
			amrnbencparms_.enable = v;
			return *this;
		};
		AvfAmrnbEnc &payload_type(tSM_INT v) {
			amrnbencparms_.payload_type = v;
			return *this;
		};
		AvfAmrnbEnc &bitrate(tSM_INT v) {
			amrnbencparms_.bitrate = v;
			return *this;
		};
		AvfAmrnbEnc &vad_enable(tSM_INT v) {
			amrnbencparms_.vad_enable = v;
			return *this;
		};
	};
public:
	AvfAmrnbEnc avf_amrnb_enc() {
		return AvfAmrnbEnc().path(path_);
	}
private:
	class Delay {
		mutable int used_;
		SM_PATH_DELAY_PARMS delayparms_;
	public:
		Delay(const Delay &old) {
			*this = old;
			old.used_ = 1;
		};
		Delay() : used_(0) {
			memset(&delayparms_, 0, sizeof(delayparms_));
		};
		operator Error() {
			Error e(Prosody::sm_path_delay(&delayparms_));
			used_ = 1;
			return e;
		};
		~Delay() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_path_delay not called\n");
				abort();
			};
		};
		Delay &path(tSMPathId v) {
			delayparms_.path = v;
			return *this;
		};
		Delay &enable(tSM_INT v) {
			delayparms_.enable = v;
			return *this;
		};
		Delay &delay(tSM_INT v) {
			delayparms_.delay = v;
			return *this;
		};
	};
public:
	Delay delay() {
		return Delay().path(path_);
	}
private:
	class Emphasis {
		mutable int used_;
		SM_PATH_EMPHASIS_PARMS emphparms_;
	public:
		Emphasis(const Emphasis &old) {
			*this = old;
			old.used_ = 1;
		};
		Emphasis() : used_(0) {
			memset(&emphparms_, 0, sizeof(emphparms_));
		};
		operator Error() {
			Error e(Prosody::sm_path_emphasis(&emphparms_));
			used_ = 1;
			return e;
		};
		~Emphasis() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_path_emphasis not called\n");
				abort();
			};
		};
		Emphasis &path(tSMPathId v) {
			emphparms_.path = v;
			return *this;
		};
		Emphasis &enable(tSM_INT v) {
			emphparms_.enable = v;
			return *this;
		};
		Emphasis &mode(enum kSMPathEmphMode v) {
			emphparms_.mode = v;
			return *this;
		};
		Emphasis &gain(tSM_INT v) {
			emphparms_.gain = v;
			return *this;
		};
	};
public:
	Emphasis emphasis() {
		return Emphasis().path(path_);
	}
private:
	class Tonedetect {
		mutable int used_;
		SM_PATH_TONEDETECT_PARMS tdparms_;
	public:
		Tonedetect(const Tonedetect &old) {
			*this = old;
			old.used_ = 1;
		};
		Tonedetect() : used_(0) {
			memset(&tdparms_, 0, sizeof(tdparms_));
		};
		operator Error() {
			Error e(Prosody::sm_path_tonedetect(&tdparms_));
			used_ = 1;
			return e;
		};
		~Tonedetect() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_path_tonedetect not called\n");
				abort();
			};
		};
		Tonedetect &path(tSMPathId v) {
			tdparms_.path = v;
			return *this;
		};
		Tonedetect &elim(tSM_INT v) {
			tdparms_.elim = v;
			return *this;
		};
		Tonedetect &min_duration(enum kSMPathToneDetectMode v) {
			tdparms_.min_duration = v;
			return *this;
		};
		Tonedetect &tone_set_id(tSM_INT v) {
			tdparms_.tone_set_id = v;
			return *this;
		};
	};
public:
	Tonedetect tonedetect() {
		return Tonedetect().path(path_);
	}
private:
	class Stop {
		mutable int used_;
		SM_PATH_STOP_PARMS stopp_;
	public:
		Stop(const Stop &old) {
			*this = old;
			old.used_ = 1;
		};
		Stop() : used_(0) {
			memset(&stopp_, 0, sizeof(stopp_));
		};
		operator Error() {
			Error e(Prosody::sm_path_stop(&stopp_));
			used_ = 1;
			return e;
		};
		~Stop() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_path_stop not called\n");
				abort();
			};
		};
		Stop &path(tSMPathId v) {
			stopp_.path = v;
			return *this;
		};
	};
public:
	Stop stop() {
		return Stop().path(path_);
	}
private:
	class Ttyeliminate {
		mutable int used_;
		SM_PATH_TTYELIMINATE_PARMS ttyeliminatep_;
	public:
		Ttyeliminate(const Ttyeliminate &old) {
			*this = old;
			old.used_ = 1;
		};
		Ttyeliminate() : used_(0) {
			memset(&ttyeliminatep_, 0, sizeof(ttyeliminatep_));
		};
		operator Error() {
			Error e(Prosody::sm_path_ttyeliminate(&ttyeliminatep_));
			used_ = 1;
			return e;
		};
		~Ttyeliminate() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_path_ttyeliminate not called\n");
				abort();
			};
		};
		Ttyeliminate &path(tSMPathId v) {
			ttyeliminatep_.path = v;
			return *this;
		};
		Ttyeliminate &enable(tSM_INT v) {
			ttyeliminatep_.enable = v;
			return *this;
		};
	};
public:
	Ttyeliminate ttyeliminate() {
		return Ttyeliminate().path(path_);
	}
