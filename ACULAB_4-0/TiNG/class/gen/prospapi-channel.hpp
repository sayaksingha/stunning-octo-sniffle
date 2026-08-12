private:
	class ReplayStart {
		mutable int used_;
		SM_REPLAY_PARMS replayp_;
	public:
		ReplayStart(const ReplayStart &old) {
			*this = old;
			old.used_ = 1;
		};
		ReplayStart() : used_(0) {
			memset(&replayp_, 0, sizeof(replayp_));
		};
		operator Error() {
			Error e(Prosody::sm_replay_start(&replayp_));
			used_ = 1;
			return e;
		};
		~ReplayStart() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_replay_start not called\n");
				abort();
			};
		};
		ReplayStart &channel(tSMChannelId v) {
			replayp_.channel = v;
			return *this;
		};
		ReplayStart &background(tSMChannelId v) {
			replayp_.background = v;
			return *this;
		};
		ReplayStart &volume(tSM_INT v) {
			replayp_.volume = v;
			return *this;
		};
		ReplayStart &agc(tSM_INT v) {
			replayp_.agc = v;
			return *this;
		};
		ReplayStart &speed(tSM_INT v) {
			replayp_.speed = v;
			return *this;
		};
		ReplayStart &type(enum kSMDataFormat v) {
			replayp_.type = v;
			return *this;
		};
		ReplayStart &data_length(tSM_UT32 v) {
			replayp_.data_length = v;
			return *this;
		};
		ReplayStart &sampling_rate(tSM_UT32 v) {
			replayp_.sampling_rate = v;
			return *this;
		};
	};
public:
	ReplayStart replay_start() {
		return ReplayStart().channel(channel_);
	}
private:
	class PutReplayData {
		mutable int used_;
		SM_TS_DATA_PARMS datap_;
	public:
		PutReplayData(const PutReplayData &old) {
			*this = old;
			old.used_ = 1;
		};
		PutReplayData() : used_(0) {
			memset(&datap_, 0, sizeof(datap_));
		};
		operator Error() {
			Error e(Prosody::sm_put_replay_data(&datap_));
			used_ = 1;
			return e;
		};
		~PutReplayData() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_put_replay_data not called\n");
				abort();
			};
		};
		PutReplayData &channel(tSMChannelId v) {
			datap_.channel = v;
			return *this;
		};
		PutReplayData &data(char* v) {
			datap_.data = v;
			return *this;
		};
		PutReplayData &length(tSM_INT v) {
			datap_.length = v;
			return *this;
		};
	};
public:
	PutReplayData put_replay_data() {
		return PutReplayData().channel(channel_);
	}
private:
	class PutLastReplayData {
		mutable int used_;
		SM_TS_DATA_PARMS datap_;
	public:
		PutLastReplayData(const PutLastReplayData &old) {
			*this = old;
			old.used_ = 1;
		};
		PutLastReplayData() : used_(0) {
			memset(&datap_, 0, sizeof(datap_));
		};
		operator Error() {
			Error e(Prosody::sm_put_last_replay_data(&datap_));
			used_ = 1;
			return e;
		};
		~PutLastReplayData() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_put_last_replay_data not called\n");
				abort();
			};
		};
		PutLastReplayData &channel(tSMChannelId v) {
			datap_.channel = v;
			return *this;
		};
		PutLastReplayData &data(char* v) {
			datap_.data = v;
			return *this;
		};
		PutLastReplayData &length(tSM_INT v) {
			datap_.length = v;
			return *this;
		};
	};
public:
	PutLastReplayData put_last_replay_data() {
		return PutLastReplayData().channel(channel_);
	}
private:
	class ReplayStatus {
		mutable int used_;
		SM_REPLAY_STATUS_PARMS statusp_;
		tSMChannelId *channelP_;
		enum kSMReplayStatus *statusP_;
		tSM_UT32 *offsetP_;
	public:
		ReplayStatus(const ReplayStatus &old) {
			*this = old;
			old.used_ = 1;
		};
		ReplayStatus() : used_(0) {
			memset(&statusp_, 0, sizeof(statusp_));
			channelP_ = 0;
			statusP_ = 0;
			offsetP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_replay_status(&statusp_));
			used_ = 1;
			if (channelP_) *channelP_ = statusp_.channel;
			if (statusP_) *statusP_ = statusp_.status;
			if (offsetP_) *offsetP_ = statusp_.offset;
			return e;
		};
		~ReplayStatus() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_replay_status not called\n");
				abort();
			};
		};
		ReplayStatus &channel(tSMChannelId *v) {
			channelP_ = v;
			return *this;
		};
		ReplayStatus &channel(tSMChannelId v) {
			statusp_.channel = v;
			return *this;
		};
		ReplayStatus &status(enum kSMReplayStatus *v) {
			statusP_ = v;
			return *this;
		};
		ReplayStatus &offset(tSM_UT32 *v) {
			offsetP_ = v;
			return *this;
		};
	};
public:
	ReplayStatus replay_status() {
		return ReplayStatus().channel(&channel_).channel(channel_);
	}
private:
	class ReplayAbort {
		mutable int used_;
		SM_REPLAY_ABORT_PARMS abortp_;
		tSM_UT32 *offsetP_;
	public:
		ReplayAbort(const ReplayAbort &old) {
			*this = old;
			old.used_ = 1;
		};
		ReplayAbort() : used_(0) {
			memset(&abortp_, 0, sizeof(abortp_));
			offsetP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_replay_abort(&abortp_));
			used_ = 1;
			if (offsetP_) *offsetP_ = abortp_.offset;
			return e;
		};
		~ReplayAbort() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_replay_abort not called\n");
				abort();
			};
		};
		ReplayAbort &channel(tSMChannelId v) {
			abortp_.channel = v;
			return *this;
		};
		ReplayAbort &offset(tSM_UT32 *v) {
			offsetP_ = v;
			return *this;
		};
		ReplayAbort &nowait(tSM_UT32 v) {
			abortp_.nowait = v;
			return *this;
		};
	};
public:
	ReplayAbort replay_abort() {
		return ReplayAbort().channel(channel_);
	}
private:
	class ReplayAdjust {
		mutable int used_;
		SM_REPLAY_ADJUST_PARMS adjustp_;
	public:
		ReplayAdjust(const ReplayAdjust &old) {
			*this = old;
			old.used_ = 1;
		};
		ReplayAdjust() : used_(0) {
			memset(&adjustp_, 0, sizeof(adjustp_));
		};
		operator Error() {
			Error e(Prosody::sm_replay_adjust(&adjustp_));
			used_ = 1;
			return e;
		};
		~ReplayAdjust() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_replay_adjust not called\n");
				abort();
			};
		};
		ReplayAdjust &channel(tSMChannelId v) {
			adjustp_.channel = v;
			return *this;
		};
		ReplayAdjust &background(tSMChannelId v) {
			adjustp_.background = v;
			return *this;
		};
		ReplayAdjust &volume(tSM_INT v) {
			adjustp_.volume = v;
			return *this;
		};
		ReplayAdjust &agc(tSM_INT v) {
			adjustp_.agc = v;
			return *this;
		};
		ReplayAdjust &speed(tSM_INT v) {
			adjustp_.speed = v;
			return *this;
		};
	};
public:
	ReplayAdjust replay_adjust() {
		return ReplayAdjust().channel(channel_);
	}
private:
	class PutAudioData {
		mutable int used_;
		SM_AUDIO_DATA_PARMS datap_;
		tSM_INT *done_lengthP_;
	public:
		PutAudioData(const PutAudioData &old) {
			*this = old;
			old.used_ = 1;
		};
		PutAudioData() : used_(0) {
			memset(&datap_, 0, sizeof(datap_));
			done_lengthP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_put_audio_data(&datap_));
			used_ = 1;
			if (done_lengthP_) *done_lengthP_ = datap_.done_length;
			return e;
		};
		~PutAudioData() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_put_audio_data not called\n");
				abort();
			};
		};
		PutAudioData &channel(tSMChannelId v) {
			datap_.channel = v;
			return *this;
		};
		PutAudioData &data(char* v) {
			datap_.data = v;
			return *this;
		};
		PutAudioData &max_length(tSM_INT v) {
			datap_.max_length = v;
			return *this;
		};
		PutAudioData &done_length(tSM_INT *v) {
			done_lengthP_ = v;
			return *this;
		};
	};
public:
	PutAudioData put_audio_data() {
		return PutAudioData().channel(channel_);
	}
private:
	class RecordStart {
		mutable int used_;
		SM_RECORD_PARMS recordp_;
	public:
		RecordStart(const RecordStart &old) {
			*this = old;
			old.used_ = 1;
		};
		RecordStart() : used_(0) {
			memset(&recordp_, 0, sizeof(recordp_));
		};
		operator Error() {
			Error e(Prosody::sm_record_start(&recordp_));
			used_ = 1;
			return e;
		};
		~RecordStart() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_record_start not called\n");
				abort();
			};
		};
		RecordStart &channel(tSMChannelId v) {
			recordp_.channel = v;
			return *this;
		};
		RecordStart &alt_data_source(tSMChannelId v) {
			recordp_.alt_data_source = v;
			return *this;
		};
		RecordStart &type(enum kSMDataFormat v) {
			recordp_.type = v;
			return *this;
		};
		RecordStart &silence_elimination(tSM_UT32 v) {
			recordp_.silence_elimination = v;
			return *this;
		};
		RecordStart &tone_elimination_mode(enum kSMToneDetection v) {
			recordp_.tone_elimination_mode = v;
			return *this;
		};
		RecordStart &tone_elimination_set_id(tSM_UT32 v) {
			recordp_.tone_elimination_set_id = v;
			return *this;
		};
		RecordStart &max_octets(tSM_UT32 v) {
			recordp_.max_octets = v;
			return *this;
		};
		RecordStart &max_elapsed_time(tSM_UT32 v) {
			recordp_.max_elapsed_time = v;
			return *this;
		};
		RecordStart &max_silence(tSM_UT32 v) {
			recordp_.max_silence = v;
			return *this;
		};
		RecordStart &agc(tSM_INT v) {
			recordp_.agc = v;
			return *this;
		};
		RecordStart &volume(tSM_INT v) {
			recordp_.volume = v;
			return *this;
		};
		RecordStart &alt_data_source_type(enum kSMRecordAltSource v) {
			recordp_.alt_data_source_type = v;
			return *this;
		};
		RecordStart &sampling_rate(tSM_UT32 v) {
			recordp_.sampling_rate = v;
			return *this;
		};
		RecordStart &min_noise_level(double v) {
			recordp_.min_noise_level = v;
			return *this;
		};
		RecordStart &grunt_threshold(double v) {
			recordp_.grunt_threshold = v;
			return *this;
		};
		RecordStart &grunt_holdoff(tSM_UT32 v) {
			recordp_.grunt_holdoff = v;
			return *this;
		};
		RecordStart &max_initial_silence(tSM_UT32 v) {
			recordp_.max_initial_silence = v;
			return *this;
		};
	};
public:
	RecordStart record_start() {
		return RecordStart().channel(channel_);
	}
private:
	class GetRecordedData {
		mutable int used_;
		SM_TS_DATA_PARMS datap_;
		tSM_INT *lengthP_;
	public:
		GetRecordedData(const GetRecordedData &old) {
			*this = old;
			old.used_ = 1;
		};
		GetRecordedData() : used_(0) {
			memset(&datap_, 0, sizeof(datap_));
			lengthP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_get_recorded_data(&datap_));
			used_ = 1;
			if (lengthP_) *lengthP_ = datap_.length;
			return e;
		};
		~GetRecordedData() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_get_recorded_data not called\n");
				abort();
			};
		};
		GetRecordedData &channel(tSMChannelId v) {
			datap_.channel = v;
			return *this;
		};
		GetRecordedData &data(char* v) {
			datap_.data = v;
			return *this;
		};
		GetRecordedData &length(tSM_INT *v) {
			lengthP_ = v;
			return *this;
		};
	};
public:
	GetRecordedData get_recorded_data() {
		return GetRecordedData().channel(channel_);
	}
private:
	class RecordStatus {
		mutable int used_;
		SM_RECORD_STATUS_PARMS statusp_;
		tSMChannelId *channelP_;
		enum kSMRecordStatus *statusP_;
		enum kSMRecognition *recog_typeP_;
		tSM_INT *param0P_;
		tSM_INT *param1P_;
		enum kSMRecordHowTerminated *termination_reasonP_;
		tSM_UT32 *termination_octetsP_;
		tSM_UT32 *sample_rateP_;
	public:
		RecordStatus(const RecordStatus &old) {
			*this = old;
			old.used_ = 1;
		};
		RecordStatus() : used_(0) {
			memset(&statusp_, 0, sizeof(statusp_));
			channelP_ = 0;
			statusP_ = 0;
			recog_typeP_ = 0;
			param0P_ = 0;
			param1P_ = 0;
			termination_reasonP_ = 0;
			termination_octetsP_ = 0;
			sample_rateP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_record_status(&statusp_));
			used_ = 1;
			if (channelP_) *channelP_ = statusp_.channel;
			if (statusP_) *statusP_ = statusp_.status;
			if (recog_typeP_) *recog_typeP_ = statusp_.recog_type;
			if (param0P_) *param0P_ = statusp_.param0;
			if (param1P_) *param1P_ = statusp_.param1;
			if (termination_reasonP_) *termination_reasonP_ = statusp_.termination_reason;
			if (termination_octetsP_) *termination_octetsP_ = statusp_.termination_octets;
			if (sample_rateP_) *sample_rateP_ = statusp_.sample_rate;
			return e;
		};
		~RecordStatus() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_record_status not called\n");
				abort();
			};
		};
		RecordStatus &channel(tSMChannelId *v) {
			channelP_ = v;
			return *this;
		};
		RecordStatus &channel(tSMChannelId v) {
			statusp_.channel = v;
			return *this;
		};
		RecordStatus &status(enum kSMRecordStatus *v) {
			statusP_ = v;
			return *this;
		};
		RecordStatus &recog_type(enum kSMRecognition *v) {
			recog_typeP_ = v;
			return *this;
		};
		RecordStatus &param0(tSM_INT *v) {
			param0P_ = v;
			return *this;
		};
		RecordStatus &param1(tSM_INT *v) {
			param1P_ = v;
			return *this;
		};
		RecordStatus &termination_reason(enum kSMRecordHowTerminated *v) {
			termination_reasonP_ = v;
			return *this;
		};
		RecordStatus &termination_octets(tSM_UT32 *v) {
			termination_octetsP_ = v;
			return *this;
		};
		RecordStatus &sample_rate(tSM_UT32 *v) {
			sample_rateP_ = v;
			return *this;
		};
	};
public:
	RecordStatus record_status() {
		return RecordStatus().channel(&channel_).channel(channel_);
	}
private:
	class RecordAbort {
		mutable int used_;
		SM_RECORD_ABORT_PARMS abortp_;
	public:
		RecordAbort(const RecordAbort &old) {
			*this = old;
			old.used_ = 1;
		};
		RecordAbort() : used_(0) {
			memset(&abortp_, 0, sizeof(abortp_));
		};
		operator Error() {
			Error e(Prosody::sm_record_abort(&abortp_));
			used_ = 1;
			return e;
		};
		~RecordAbort() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_record_abort not called\n");
				abort();
			};
		};
		RecordAbort &channel(tSMChannelId v) {
			abortp_.channel = v;
			return *this;
		};
		RecordAbort &discard(tSM_INT v) {
			abortp_.discard = v;
			return *this;
		};
	};
public:
	RecordAbort record_abort() {
		return RecordAbort().channel(channel_);
	}
private:
	class RecordAgcAdjust {
		mutable int used_;
		SM_RECORD_AGC_ADJUST_PARMS recadjp_;
	public:
		RecordAgcAdjust(const RecordAgcAdjust &old) {
			*this = old;
			old.used_ = 1;
		};
		RecordAgcAdjust() : used_(0) {
			memset(&recadjp_, 0, sizeof(recadjp_));
		};
		operator Error() {
			Error e(Prosody::sm_record_agc_adjust(&recadjp_));
			used_ = 1;
			return e;
		};
		~RecordAgcAdjust() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_record_agc_adjust not called\n");
				abort();
			};
		};
		RecordAgcAdjust &channel(tSMChannelId v) {
			recadjp_.channel = v;
			return *this;
		};
		RecordAgcAdjust &gain(tSM_INT v) {
			recadjp_.gain = v;
			return *this;
		};
	};
public:
	RecordAgcAdjust record_agc_adjust() {
		return RecordAgcAdjust().channel(channel_);
	}
private:
	class RecordAgcAdjustSettings {
		mutable int used_;
		SM_RECORD_AGC_ADJUST_SETTINGS_PARMS recadjp_;
	public:
		RecordAgcAdjustSettings(const RecordAgcAdjustSettings &old) {
			*this = old;
			old.used_ = 1;
		};
		RecordAgcAdjustSettings() : used_(0) {
			memset(&recadjp_, 0, sizeof(recadjp_));
		};
		operator Error() {
			Error e(Prosody::sm_record_agc_adjust_settings(&recadjp_));
			used_ = 1;
			return e;
		};
		~RecordAgcAdjustSettings() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_record_agc_adjust_settings not called\n");
				abort();
			};
		};
		RecordAgcAdjustSettings &channel(tSMChannelId v) {
			recadjp_.channel = v;
			return *this;
		};
		RecordAgcAdjustSettings &max_level_decay(float v) {
			recadjp_.max_level_decay = v;
			return *this;
		};
		RecordAgcAdjustSettings &target_level(float v) {
			recadjp_.target_level = v;
			return *this;
		};
	};
public:
	RecordAgcAdjustSettings record_agc_adjust_settings() {
		return RecordAgcAdjustSettings().channel(channel_);
	}
private:
	class PlayTone {
		mutable int used_;
		SM_PLAY_TONE_PARMS tonep_;
	public:
		PlayTone(const PlayTone &old) {
			*this = old;
			old.used_ = 1;
		};
		PlayTone() : used_(0) {
			memset(&tonep_, 0, sizeof(tonep_));
		};
		operator Error() {
			Error e(Prosody::sm_play_tone(&tonep_));
			used_ = 1;
			return e;
		};
		~PlayTone() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_play_tone not called\n");
				abort();
			};
		};
		PlayTone &channel(tSMChannelId v) {
			tonep_.channel = v;
			return *this;
		};
		PlayTone &duration(tSM_UT32 v) {
			tonep_.duration = v;
			return *this;
		};
		PlayTone &wait_for_completion(tSM_INT v) {
			tonep_.wait_for_completion = v;
			return *this;
		};
		PlayTone &tone_id(tSM_INT v) {
			tonep_.tone_id = v;
			return *this;
		};
	};
public:
	PlayTone play_tone() {
		return PlayTone().channel(channel_);
	}
private:
	class PlayToneStatus {
		mutable int used_;
		SM_PLAY_TONE_STATUS_PARMS statusp_;
		tSMChannelId *channelP_;
		enum kSMPlayToneStatus *statusP_;
	public:
		PlayToneStatus(const PlayToneStatus &old) {
			*this = old;
			old.used_ = 1;
		};
		PlayToneStatus() : used_(0) {
			memset(&statusp_, 0, sizeof(statusp_));
			channelP_ = 0;
			statusP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_play_tone_status(&statusp_));
			used_ = 1;
			if (channelP_) *channelP_ = statusp_.channel;
			if (statusP_) *statusP_ = statusp_.status;
			return e;
		};
		~PlayToneStatus() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_play_tone_status not called\n");
				abort();
			};
		};
		PlayToneStatus &channel(tSMChannelId *v) {
			channelP_ = v;
			return *this;
		};
		PlayToneStatus &channel(tSMChannelId v) {
			statusp_.channel = v;
			return *this;
		};
		PlayToneStatus &status(enum kSMPlayToneStatus *v) {
			statusP_ = v;
			return *this;
		};
	};
public:
	PlayToneStatus play_tone_status() {
		return PlayToneStatus().channel(&channel_).channel(channel_);
	}
private:
	class PlayToneAbort {
		mutable int used_;
		tSMChannelId channel_;
	public:
		PlayToneAbort(const PlayToneAbort &old) {
			*this = old;
			old.used_ = 1;
		};
		PlayToneAbort(tSMChannelId channel) : used_(0) {
			channel_ = channel;
		};
		operator Error() {
			Error e(Prosody::sm_play_tone_abort(channel_));
			used_ = 1;
			return e;
		};
		~PlayToneAbort() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_play_tone_abort not called\n");
				abort();
			};
		};
	};
public:
	PlayToneAbort play_tone_abort() {
		return PlayToneAbort(channel_);
	}
private:
	class PlayToneList {
		mutable int used_;
		SM_PLAY_TONE_LIST_PARMS tonep_;
	public:
		PlayToneList(const PlayToneList &old) {
			*this = old;
			old.used_ = 1;
		};
		PlayToneList() : used_(0) {
			memset(&tonep_, 0, sizeof(tonep_));
		};
		operator Error() {
			Error e(Prosody::sm_play_tone_list(&tonep_));
			used_ = 1;
			return e;
		};
		~PlayToneList() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_play_tone_list not called\n");
				abort();
			};
		};
		PlayToneList &channel(tSMChannelId v) {
			tonep_.channel = v;
			return *this;
		};
		PlayToneList &tones(SM_PLAY_TONE_LIST_PARMS::sm_play_tone_item* v) {
			tonep_.tones = v;
			return *this;
		};
		PlayToneList &tone_count(tSM_INT v) {
			tonep_.tone_count = v;
			return *this;
		};
	};
public:
	PlayToneList play_tone_list() {
		return PlayToneList().channel(channel_);
	}
private:
	class PlayToneListStatus {
		mutable int used_;
		SM_PLAY_TONE_LIST_STATUS_PARMS statusp_;
		tSMChannelId *channelP_;
		enum kSMPlayToneListStatus *statusP_;
	public:
		PlayToneListStatus(const PlayToneListStatus &old) {
			*this = old;
			old.used_ = 1;
		};
		PlayToneListStatus() : used_(0) {
			memset(&statusp_, 0, sizeof(statusp_));
			channelP_ = 0;
			statusP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_play_tone_list_status(&statusp_));
			used_ = 1;
			if (channelP_) *channelP_ = statusp_.channel;
			if (statusP_) *statusP_ = statusp_.status;
			return e;
		};
		~PlayToneListStatus() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_play_tone_list_status not called\n");
				abort();
			};
		};
		PlayToneListStatus &channel(tSMChannelId *v) {
			channelP_ = v;
			return *this;
		};
		PlayToneListStatus &channel(tSMChannelId v) {
			statusp_.channel = v;
			return *this;
		};
		PlayToneListStatus &status(enum kSMPlayToneListStatus *v) {
			statusP_ = v;
			return *this;
		};
	};
public:
	PlayToneListStatus play_tone_list_status() {
		return PlayToneListStatus().channel(&channel_).channel(channel_);
	}
private:
	class PlayToneListAbort {
		mutable int used_;
		tSMChannelId channel_;
	public:
		PlayToneListAbort(const PlayToneListAbort &old) {
			*this = old;
			old.used_ = 1;
		};
		PlayToneListAbort(tSMChannelId channel) : used_(0) {
			channel_ = channel;
		};
		operator Error() {
			Error e(Prosody::sm_play_tone_list_abort(channel_));
			used_ = 1;
			return e;
		};
		~PlayToneListAbort() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_play_tone_list_abort not called\n");
				abort();
			};
		};
	};
public:
	PlayToneListAbort play_tone_list_abort() {
		return PlayToneListAbort(channel_);
	}
private:
	class PlayCptone {
		mutable int used_;
		SM_PLAY_CPTONE_PARMS cptonep_;
	public:
		PlayCptone(const PlayCptone &old) {
			*this = old;
			old.used_ = 1;
		};
		PlayCptone() : used_(0) {
			memset(&cptonep_, 0, sizeof(cptonep_));
		};
		operator Error() {
			Error e(Prosody::sm_play_cptone(&cptonep_));
			used_ = 1;
			return e;
		};
		~PlayCptone() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_play_cptone not called\n");
				abort();
			};
		};
		PlayCptone &channel(tSMChannelId v) {
			cptonep_.channel = v;
			return *this;
		};
		PlayCptone &duration(tSM_UT32 v) {
			cptonep_.duration = v;
			return *this;
		};
		PlayCptone &wait_for_completion(tSM_INT v) {
			cptonep_.wait_for_completion = v;
			return *this;
		};
		PlayCptone &type(enum kSMPlayCPToneType v) {
			cptonep_.type = v;
			return *this;
		};
		PlayCptone &tone_count(tSM_INT v) {
			cptonep_.tone_count = v;
			return *this;
		};
	};
public:
	PlayCptone play_cptone() {
		return PlayCptone().channel(channel_);
	}
private:
	class PlayCptoneStatus {
		mutable int used_;
		SM_PLAY_CPTONE_STATUS_PARMS statusp_;
		tSMChannelId *channelP_;
		enum kSMPlayCPToneStatus *statusP_;
	public:
		PlayCptoneStatus(const PlayCptoneStatus &old) {
			*this = old;
			old.used_ = 1;
		};
		PlayCptoneStatus() : used_(0) {
			memset(&statusp_, 0, sizeof(statusp_));
			channelP_ = 0;
			statusP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_play_cptone_status(&statusp_));
			used_ = 1;
			if (channelP_) *channelP_ = statusp_.channel;
			if (statusP_) *statusP_ = statusp_.status;
			return e;
		};
		~PlayCptoneStatus() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_play_cptone_status not called\n");
				abort();
			};
		};
		PlayCptoneStatus &channel(tSMChannelId *v) {
			channelP_ = v;
			return *this;
		};
		PlayCptoneStatus &channel(tSMChannelId v) {
			statusp_.channel = v;
			return *this;
		};
		PlayCptoneStatus &status(enum kSMPlayCPToneStatus *v) {
			statusP_ = v;
			return *this;
		};
	};
public:
	PlayCptoneStatus play_cptone_status() {
		return PlayCptoneStatus().channel(&channel_).channel(channel_);
	}
private:
	class PlayCptoneAbort {
		mutable int used_;
		tSMChannelId channel_;
	public:
		PlayCptoneAbort(const PlayCptoneAbort &old) {
			*this = old;
			old.used_ = 1;
		};
		PlayCptoneAbort(tSMChannelId channel) : used_(0) {
			channel_ = channel;
		};
		operator Error() {
			Error e(Prosody::sm_play_cptone_abort(channel_));
			used_ = 1;
			return e;
		};
		~PlayCptoneAbort() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_play_cptone_abort not called\n");
				abort();
			};
		};
	};
public:
	PlayCptoneAbort play_cptone_abort() {
		return PlayCptoneAbort(channel_);
	}
private:
	class PlayDigits {
		mutable int used_;
		SM_PLAY_DIGITS_PARMS digitsp_;
	public:
		PlayDigits(const PlayDigits &old) {
			*this = old;
			old.used_ = 1;
		};
		PlayDigits() : used_(0) {
			memset(&digitsp_, 0, sizeof(digitsp_));
		};
		operator Error() {
			Error e(Prosody::sm_play_digits(&digitsp_));
			used_ = 1;
			return e;
		};
		~PlayDigits() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_play_digits not called\n");
				abort();
			};
		};
		PlayDigits &channel(tSMChannelId v) {
			digitsp_.channel = v;
			return *this;
		};
		PlayDigits &wait_for_completion(tSM_INT v) {
			digitsp_.wait_for_completion = v;
			return *this;
		};
		PlayDigits &digits_type(enum kSMDigitType v) {
			digitsp_.digits.type = v;
			return *this;
		};
		PlayDigits &digits_qualifier(tSM_INT v) {
			digitsp_.digits.qualifier = v;
			return *this;
		};
		PlayDigits &digits_digit_string(char *v) {
			strncpy(digitsp_.digits.digit_string, v, sizeof(digitsp_.digits.digit_string));
			return *this;
		};
		PlayDigits &digits_inter_digit_delay(tSM_INT v) {
			digitsp_.digits.inter_digit_delay = v;
			return *this;
		};
		PlayDigits &digits_digit_duration(tSM_INT v) {
			digitsp_.digits.digit_duration = v;
			return *this;
		};
	};
public:
	PlayDigits play_digits() {
		return PlayDigits().channel(channel_);
	}
private:
	class PlayDigitsStatus {
		mutable int used_;
		SM_PLAY_DIGITS_STATUS_PARMS statusp_;
		tSMChannelId *channelP_;
		enum kSMPlayDigitsStatus *statusP_;
	public:
		PlayDigitsStatus(const PlayDigitsStatus &old) {
			*this = old;
			old.used_ = 1;
		};
		PlayDigitsStatus() : used_(0) {
			memset(&statusp_, 0, sizeof(statusp_));
			channelP_ = 0;
			statusP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_play_digits_status(&statusp_));
			used_ = 1;
			if (channelP_) *channelP_ = statusp_.channel;
			if (statusP_) *statusP_ = statusp_.status;
			return e;
		};
		~PlayDigitsStatus() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_play_digits_status not called\n");
				abort();
			};
		};
		PlayDigitsStatus &channel(tSMChannelId *v) {
			channelP_ = v;
			return *this;
		};
		PlayDigitsStatus &channel(tSMChannelId v) {
			statusp_.channel = v;
			return *this;
		};
		PlayDigitsStatus &status(enum kSMPlayDigitsStatus *v) {
			statusP_ = v;
			return *this;
		};
	};
public:
	PlayDigitsStatus play_digits_status() {
		return PlayDigitsStatus().channel(&channel_).channel(channel_);
	}
private:
	class ListenFor {
		mutable int used_;
		SM_LISTEN_FOR_PARMS listenp_;
	public:
		ListenFor(const ListenFor &old) {
			*this = old;
			old.used_ = 1;
		};
		ListenFor() : used_(0) {
			memset(&listenp_, 0, sizeof(listenp_));
		};
		operator Error() {
			Error e(Prosody::sm_listen_for(&listenp_));
			used_ = 1;
			return e;
		};
		~ListenFor() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_listen_for not called\n");
				abort();
			};
		};
		ListenFor &channel(tSMChannelId v) {
			listenp_.channel = v;
			return *this;
		};
		ListenFor &tone_detection_mode(enum kSMToneDetection v) {
			listenp_.tone_detection_mode = v;
			return *this;
		};
		ListenFor &active_tone_set_id(tSM_INT v) {
			listenp_.active_tone_set_id = v;
			return *this;
		};
		ListenFor &map_tones_to_digits(enum kSMDigitMapping v) {
			listenp_.map_tones_to_digits = v;
			return *this;
		};
		ListenFor &enable_cptone_recognition(tSM_INT v) {
			listenp_.enable_cptone_recognition = v;
			return *this;
		};
		ListenFor &enable_grunt_detection(tSM_INT v) {
			listenp_.enable_grunt_detection = v;
			return *this;
		};
		ListenFor &grunt_latency(tSM_INT v) {
			listenp_.grunt_latency = v;
			return *this;
		};
		ListenFor &min_noise_level(double v) {
			listenp_.min_noise_level = v;
			return *this;
		};
		ListenFor &grunt_threshold(double v) {
			listenp_.grunt_threshold = v;
			return *this;
		};
		ListenFor &grunt_holdoff(tSM_UT32 v) {
			listenp_.grunt_holdoff = v;
			return *this;
		};
	};
public:
	ListenFor listen_for() {
		return ListenFor().channel(channel_);
	}
private:
	class AnsListenFor {
		mutable int used_;
		SM_ANS_LISTEN_FOR_PARMS listenp_;
	public:
		AnsListenFor(const AnsListenFor &old) {
			*this = old;
			old.used_ = 1;
		};
		AnsListenFor() : used_(0) {
			memset(&listenp_, 0, sizeof(listenp_));
		};
		operator Error() {
			Error e(Prosody::sm_ans_listen_for(&listenp_));
			used_ = 1;
			return e;
		};
		~AnsListenFor() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_ans_listen_for not called\n");
				abort();
			};
		};
		AnsListenFor &channel(tSMChannelId v) {
			listenp_.channel = v;
			return *this;
		};
		AnsListenFor &detection_mode(enum kSMANSMode v) {
			listenp_.detection_mode = v;
			return *this;
		};
	};
public:
	AnsListenFor ans_listen_for() {
		return AnsListenFor().channel(channel_);
	}
private:
	class AsrListenFor {
		mutable int used_;
		SM_ASR_LISTEN_FOR_PARMS listenp_;
	public:
		AsrListenFor(const AsrListenFor &old) {
			*this = old;
			old.used_ = 1;
		};
		AsrListenFor() : used_(0) {
			memset(&listenp_, 0, sizeof(listenp_));
		};
		operator Error() {
			Error e(Prosody::sm_asr_listen_for(&listenp_));
			used_ = 1;
			return e;
		};
		~AsrListenFor() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_asr_listen_for not called\n");
				abort();
			};
		};
		AsrListenFor &channel(tSMChannelId v) {
			listenp_.channel = v;
			return *this;
		};
		AsrListenFor &vocab_item_count(tSM_INT v) {
			listenp_.vocab_item_count = v;
			return *this;
		};
		AsrListenFor &vocab_item_ids(tSM_UT32* v) {
			listenp_.vocab_item_ids = v;
			return *this;
		};
		AsrListenFor &vocab_recog_ids(tSM_INT* v) {
			listenp_.vocab_recog_ids = v;
			return *this;
		};
		AsrListenFor &asr_mode(enum kSMASRMode v) {
			listenp_.asr_mode = v;
			return *this;
		};
		AsrListenFor &specific_parameters(SM_ASR_LISTEN_FOR_PARMS::sm_asr_characteristics* v) {
			listenp_.specific_parameters = v;
			return *this;
		};
		AsrListenFor &sidetone(tSMChannelId v) {
			listenp_.sidetone = v;
			return *this;
		};
	};
public:
	AsrListenFor asr_listen_for() {
		return AsrListenFor().channel(channel_);
	}
private:
	class GetRecognised {
		mutable int used_;
		SM_RECOGNISED_PARMS recogp_;
		tSMChannelId *channelP_;
		enum kSMRecognition *typeP_;
		tSM_INT *param0P_;
		tSM_INT *param1P_;
	public:
		GetRecognised(const GetRecognised &old) {
			*this = old;
			old.used_ = 1;
		};
		GetRecognised() : used_(0) {
			memset(&recogp_, 0, sizeof(recogp_));
			channelP_ = 0;
			typeP_ = 0;
			param0P_ = 0;
			param1P_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_get_recognised(&recogp_));
			used_ = 1;
			if (channelP_) *channelP_ = recogp_.channel;
			if (typeP_) *typeP_ = recogp_.type;
			if (param0P_) *param0P_ = recogp_.param0;
			if (param1P_) *param1P_ = recogp_.param1;
			return e;
		};
		~GetRecognised() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_get_recognised not called\n");
				abort();
			};
		};
		GetRecognised &channel(tSMChannelId *v) {
			channelP_ = v;
			return *this;
		};
		GetRecognised &channel(tSMChannelId v) {
			recogp_.channel = v;
			return *this;
		};
		GetRecognised &type(enum kSMRecognition *v) {
			typeP_ = v;
			return *this;
		};
		GetRecognised &param0(tSM_INT *v) {
			param0P_ = v;
			return *this;
		};
		GetRecognised &param1(tSM_INT *v) {
			param1P_ = v;
			return *this;
		};
	};
public:
	GetRecognised get_recognised() {
		return GetRecognised().channel(&channel_).channel(channel_);
	}
private:
	class DiscardRecognised {
		mutable int used_;
		tSMChannelId channel_;
	public:
		DiscardRecognised(const DiscardRecognised &old) {
			*this = old;
			old.used_ = 1;
		};
		DiscardRecognised(tSMChannelId channel) : used_(0) {
			channel_ = channel;
		};
		operator Error() {
			Error e(Prosody::sm_discard_recognised(channel_));
			used_ = 1;
			return e;
		};
		~DiscardRecognised() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_discard_recognised not called\n");
				abort();
			};
		};
	};
public:
	DiscardRecognised discard_recognised() {
		return DiscardRecognised(channel_);
	}
private:
	class ConfPrimStart {
		mutable int used_;
		SM_CONF_PRIM_START_PARMS confp_;
	public:
		ConfPrimStart(const ConfPrimStart &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfPrimStart() : used_(0) {
			memset(&confp_, 0, sizeof(confp_));
		};
		operator Error() {
			Error e(Prosody::sm_conf_prim_start(&confp_));
			used_ = 1;
			return e;
		};
		~ConfPrimStart() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_conf_prim_start not called\n");
				abort();
			};
		};
		ConfPrimStart &channel(tSMChannelId v) {
			confp_.channel = v;
			return *this;
		};
		ConfPrimStart &volume(tSM_INT v) {
			confp_.volume = v;
			return *this;
		};
		ConfPrimStart &agc(tSM_INT v) {
			confp_.agc = v;
			return *this;
		};
		ConfPrimStart &conf_type(enum kSMConfType v) {
			confp_.conf_type = v;
			return *this;
		};
	};
public:
	ConfPrimStart conf_prim_start() {
		return ConfPrimStart().channel(channel_);
	}
private:
	class ConfPrimClone {
		mutable int used_;
		SM_CONF_PRIM_CLONE_PARMS clonep_;
	public:
		ConfPrimClone(const ConfPrimClone &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfPrimClone() : used_(0) {
			memset(&clonep_, 0, sizeof(clonep_));
		};
		operator Error() {
			Error e(Prosody::sm_conf_prim_clone(&clonep_));
			used_ = 1;
			return e;
		};
		~ConfPrimClone() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_conf_prim_clone not called\n");
				abort();
			};
		};
		ConfPrimClone &channel(tSMChannelId v) {
			clonep_.channel = v;
			return *this;
		};
		ConfPrimClone &model(tSMChannelId v) {
			clonep_.model = v;
			return *this;
		};
	};
public:
	ConfPrimClone conf_prim_clone() {
		return ConfPrimClone().channel(channel_);
	}
private:
	class ConfPrimAttach {
		mutable int used_;
		SM_CONF_PRIM_ATTACH_PARMS confp_;
	public:
		ConfPrimAttach(const ConfPrimAttach &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfPrimAttach() : used_(0) {
			memset(&confp_, 0, sizeof(confp_));
		};
		operator Error() {
			Error e(Prosody::sm_conf_prim_attach(&confp_));
			used_ = 1;
			return e;
		};
		~ConfPrimAttach() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_conf_prim_attach not called\n");
				abort();
			};
		};
		ConfPrimAttach &channel(tSMChannelId v) {
			confp_.channel = v;
			return *this;
		};
		ConfPrimAttach &conf_type(enum kSMConfType v) {
			confp_.conf_type = v;
			return *this;
		};
	};
public:
	ConfPrimAttach conf_prim_attach() {
		return ConfPrimAttach().channel(channel_);
	}
private:
	class ConfPrimDetach {
		mutable int used_;
		SM_CONF_PRIM_DETACH_PARMS confp_;
	public:
		ConfPrimDetach(const ConfPrimDetach &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfPrimDetach() : used_(0) {
			memset(&confp_, 0, sizeof(confp_));
		};
		operator Error() {
			Error e(Prosody::sm_conf_prim_detach(&confp_));
			used_ = 1;
			return e;
		};
		~ConfPrimDetach() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_conf_prim_detach not called\n");
				abort();
			};
		};
		ConfPrimDetach &channel(tSMChannelId v) {
			confp_.channel = v;
			return *this;
		};
		ConfPrimDetach &conf_type(enum kSMConfType v) {
			confp_.conf_type = v;
			return *this;
		};
	};
public:
	ConfPrimDetach conf_prim_detach() {
		return ConfPrimDetach().channel(channel_);
	}
private:
	class ConfPrimAdd {
		mutable int used_;
		SM_CONF_PRIM_ADD_PARMS confp_;
		tSM_INT *idP_;
	public:
		ConfPrimAdd(const ConfPrimAdd &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfPrimAdd() : used_(0) {
			memset(&confp_, 0, sizeof(confp_));
			idP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_conf_prim_add(&confp_));
			used_ = 1;
			if (idP_) *idP_ = confp_.id;
			return e;
		};
		~ConfPrimAdd() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_conf_prim_add not called\n");
				abort();
			};
		};
		ConfPrimAdd &channel(tSMChannelId v) {
			confp_.channel = v;
			return *this;
		};
		ConfPrimAdd &participant(tSMChannelId v) {
			confp_.participant = v;
			return *this;
		};
		ConfPrimAdd &id(tSM_INT *v) {
			idP_ = v;
			return *this;
		};
		ConfPrimAdd &factor(float v) {
			confp_.factor = v;
			return *this;
		};
	};
public:
	ConfPrimAdd conf_prim_add() {
		return ConfPrimAdd().channel(channel_);
	}
private:
	class ConfPrimLeave {
		mutable int used_;
		SM_CONF_PRIM_LEAVE_PARMS confp_;
	public:
		ConfPrimLeave(const ConfPrimLeave &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfPrimLeave() : used_(0) {
			memset(&confp_, 0, sizeof(confp_));
		};
		operator Error() {
			Error e(Prosody::sm_conf_prim_leave(&confp_));
			used_ = 1;
			return e;
		};
		~ConfPrimLeave() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_conf_prim_leave not called\n");
				abort();
			};
		};
		ConfPrimLeave &channel(tSMChannelId v) {
			confp_.channel = v;
			return *this;
		};
		ConfPrimLeave &id(tSM_INT v) {
			confp_.id = v;
			return *this;
		};
	};
public:
	ConfPrimLeave conf_prim_leave() {
		return ConfPrimLeave().channel(channel_);
	}
private:
	class ConfPrimInfo {
		mutable int used_;
		SM_CONF_PRIM_INFO_PARMS confp_;
		tSM_INT *participant_countP_;
	public:
		ConfPrimInfo(const ConfPrimInfo &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfPrimInfo() : used_(0) {
			memset(&confp_, 0, sizeof(confp_));
			participant_countP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_conf_prim_info(&confp_));
			used_ = 1;
			if (participant_countP_) *participant_countP_ = confp_.participant_count;
			return e;
		};
		~ConfPrimInfo() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_conf_prim_info not called\n");
				abort();
			};
		};
		ConfPrimInfo &channel(tSMChannelId v) {
			confp_.channel = v;
			return *this;
		};
		ConfPrimInfo &participant_count(tSM_INT *v) {
			participant_countP_ = v;
			return *this;
		};
	};
public:
	ConfPrimInfo conf_prim_info() {
		return ConfPrimInfo().channel(channel_);
	}
private:
	class ConfPrimAdjInput {
		mutable int used_;
		SM_CONF_PRIM_ADJ_INPUT_PARMS confp_;
	public:
		ConfPrimAdjInput(const ConfPrimAdjInput &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfPrimAdjInput() : used_(0) {
			memset(&confp_, 0, sizeof(confp_));
		};
		operator Error() {
			Error e(Prosody::sm_conf_prim_adj_input(&confp_));
			used_ = 1;
			return e;
		};
		~ConfPrimAdjInput() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_conf_prim_adj_input not called\n");
				abort();
			};
		};
		ConfPrimAdjInput &channel(tSMChannelId v) {
			confp_.channel = v;
			return *this;
		};
		ConfPrimAdjInput &volume(tSM_INT v) {
			confp_.volume = v;
			return *this;
		};
		ConfPrimAdjInput &agc(tSM_INT v) {
			confp_.agc = v;
			return *this;
		};
	};
public:
	ConfPrimAdjInput conf_prim_adj_input() {
		return ConfPrimAdjInput().channel(channel_);
	}
private:
	class ConfPrimAdjInputSettings {
		mutable int used_;
		SM_CONF_PRIM_ADJ_INPUT_SETTINGS_PARMS confp_;
	public:
		ConfPrimAdjInputSettings(const ConfPrimAdjInputSettings &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfPrimAdjInputSettings() : used_(0) {
			memset(&confp_, 0, sizeof(confp_));
		};
		operator Error() {
			Error e(Prosody::sm_conf_prim_adj_input_settings(&confp_));
			used_ = 1;
			return e;
		};
		~ConfPrimAdjInputSettings() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_conf_prim_adj_input_settings not called\n");
				abort();
			};
		};
		ConfPrimAdjInputSettings &channel(tSMChannelId v) {
			confp_.channel = v;
			return *this;
		};
		ConfPrimAdjInputSettings &max_level_decay(float v) {
			confp_.max_level_decay = v;
			return *this;
		};
		ConfPrimAdjInputSettings &target_level(float v) {
			confp_.target_level = v;
			return *this;
		};
	};
public:
	ConfPrimAdjInputSettings conf_prim_adj_input_settings() {
		return ConfPrimAdjInputSettings().channel(channel_);
	}
private:
	class ConfPrimAdjTracking {
		mutable int used_;
		SM_CONF_PRIM_ADJ_TRACKING_PARMS trackp_;
	public:
		ConfPrimAdjTracking(const ConfPrimAdjTracking &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfPrimAdjTracking() : used_(0) {
			memset(&trackp_, 0, sizeof(trackp_));
		};
		operator Error() {
			Error e(Prosody::sm_conf_prim_adj_tracking(&trackp_));
			used_ = 1;
			return e;
		};
		~ConfPrimAdjTracking() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_conf_prim_adj_tracking not called\n");
				abort();
			};
		};
		ConfPrimAdjTracking &channel(tSMChannelId v) {
			trackp_.channel = v;
			return *this;
		};
		ConfPrimAdjTracking &min_noise_level(double v) {
			trackp_.min_noise_level = v;
			return *this;
		};
		ConfPrimAdjTracking &speech_thresh(double v) {
			trackp_.speech_thresh = v;
			return *this;
		};
	};
public:
	ConfPrimAdjTracking conf_prim_adj_tracking() {
		return ConfPrimAdjTracking().channel(channel_);
	}
private:
	class ConfPrimAdjOutput {
		mutable int used_;
		SM_CONF_PRIM_ADJ_OUTPUT_PARMS confp_;
	public:
		ConfPrimAdjOutput(const ConfPrimAdjOutput &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfPrimAdjOutput() : used_(0) {
			memset(&confp_, 0, sizeof(confp_));
		};
		operator Error() {
			Error e(Prosody::sm_conf_prim_adj_output(&confp_));
			used_ = 1;
			return e;
		};
		~ConfPrimAdjOutput() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_conf_prim_adj_output not called\n");
				abort();
			};
		};
		ConfPrimAdjOutput &channel(tSMChannelId v) {
			confp_.channel = v;
			return *this;
		};
		ConfPrimAdjOutput &volume(tSM_INT v) {
			confp_.volume = v;
			return *this;
		};
		ConfPrimAdjOutput &agc(tSM_INT v) {
			confp_.agc = v;
			return *this;
		};
	};
public:
	ConfPrimAdjOutput conf_prim_adj_output() {
		return ConfPrimAdjOutput().channel(channel_);
	}
private:
	class ConfPrimAbort {
		mutable int used_;
		tSMChannelId channel_;
	public:
		ConfPrimAbort(const ConfPrimAbort &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfPrimAbort(tSMChannelId channel) : used_(0) {
			channel_ = channel;
		};
		operator Error() {
			Error e(Prosody::sm_conf_prim_abort(channel_));
			used_ = 1;
			return e;
		};
		~ConfPrimAbort() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_conf_prim_abort not called\n");
				abort();
			};
		};
	};
public:
	ConfPrimAbort conf_prim_abort() {
		return ConfPrimAbort(channel_);
	}
private:
	class SetSidetoneChannel {
		mutable int used_;
		SM_SET_SIDETONE_CHANNEL_PARMS sidetp_;
	public:
		SetSidetoneChannel(const SetSidetoneChannel &old) {
			*this = old;
			old.used_ = 1;
		};
		SetSidetoneChannel() : used_(0) {
			memset(&sidetp_, 0, sizeof(sidetp_));
		};
		operator Error() {
			Error e(Prosody::sm_set_sidetone_channel(&sidetp_));
			used_ = 1;
			return e;
		};
		~SetSidetoneChannel() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_set_sidetone_channel not called\n");
				abort();
			};
		};
		SetSidetoneChannel &channel(tSMChannelId v) {
			sidetp_.channel = v;
			return *this;
		};
		SetSidetoneChannel &output(tSMChannelId v) {
			sidetp_.output = v;
			return *this;
		};
	};
public:
	SetSidetoneChannel set_sidetone_channel() {
		return SetSidetoneChannel().channel(channel_);
	}
private:
	class ConfPrimStop {
		mutable int used_;
		SM_CONF_PRIM_STOP_PARMS stopp_;
	public:
		ConfPrimStop(const ConfPrimStop &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfPrimStop() : used_(0) {
			memset(&stopp_, 0, sizeof(stopp_));
		};
		operator Error() {
			Error e(Prosody::sm_conf_prim_stop(&stopp_));
			used_ = 1;
			return e;
		};
		~ConfPrimStop() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_conf_prim_stop not called\n");
				abort();
			};
		};
		ConfPrimStop &channel(tSMChannelId v) {
			stopp_.channel = v;
			return *this;
		};
		ConfPrimStop &no_wait(tSM_UT32 v) {
			stopp_.no_wait = v;
			return *this;
		};
	};
public:
	ConfPrimStop conf_prim_stop() {
		return ConfPrimStop().channel(channel_);
	}
private:
	class ConfPrimStatus {
		mutable int used_;
		SM_CONF_PRIM_STATUS_PARMS statusp_;
		enum kSMConfStatus *statusP_;
	public:
		ConfPrimStatus(const ConfPrimStatus &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfPrimStatus() : used_(0) {
			memset(&statusp_, 0, sizeof(statusp_));
			statusP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_conf_prim_status(&statusp_));
			used_ = 1;
			if (statusP_) *statusP_ = statusp_.status;
			return e;
		};
		~ConfPrimStatus() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_conf_prim_status not called\n");
				abort();
			};
		};
		ConfPrimStatus &channel(tSMChannelId v) {
			statusp_.channel = v;
			return *this;
		};
		ConfPrimStatus &status(enum kSMConfStatus *v) {
			statusP_ = v;
			return *this;
		};
	};
public:
	ConfPrimStatus conf_prim_status() {
		return ConfPrimStatus().channel(channel_);
	}
private:
	class ConfPrimConfigActivityReporting {
		mutable int used_;
		SM_CONF_PRIM_CONFIG_ACTIVITY_REPORTING_PARMS activityp_;
	public:
		ConfPrimConfigActivityReporting(const ConfPrimConfigActivityReporting &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfPrimConfigActivityReporting() : used_(0) {
			memset(&activityp_, 0, sizeof(activityp_));
		};
		operator Error() {
			Error e(Prosody::sm_conf_prim_config_activity_reporting(&activityp_));
			used_ = 1;
			return e;
		};
		~ConfPrimConfigActivityReporting() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_conf_prim_config_activity_reporting not called\n");
				abort();
			};
		};
		ConfPrimConfigActivityReporting &channel(tSMChannelId v) {
			activityp_.channel = v;
			return *this;
		};
		ConfPrimConfigActivityReporting &delay(tSM_UT32 v) {
			activityp_.delay = v;
			return *this;
		};
		ConfPrimConfigActivityReporting &sensitivity(tSM_UT32 v) {
			activityp_.sensitivity = v;
			return *this;
		};
	};
public:
	ConfPrimConfigActivityReporting conf_prim_config_activity_reporting() {
		return ConfPrimConfigActivityReporting().channel(channel_);
	}
private:
	class ConditionInput {
		mutable int used_;
		SM_CONDITION_INPUT_PARMS condp_;
	public:
		ConditionInput(const ConditionInput &old) {
			*this = old;
			old.used_ = 1;
		};
		ConditionInput() : used_(0) {
			memset(&condp_, 0, sizeof(condp_));
		};
		operator Error() {
			Error e(Prosody::sm_condition_input(&condp_));
			used_ = 1;
			return e;
		};
		~ConditionInput() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_condition_input not called\n");
				abort();
			};
		};
		ConditionInput &channel(tSMChannelId v) {
			condp_.channel = v;
			return *this;
		};
		ConditionInput &reference(tSMChannelId v) {
			condp_.reference = v;
			return *this;
		};
		ConditionInput &reference_type(enum kSMInputCondRef v) {
			condp_.reference_type = v;
			return *this;
		};
		ConditionInput &conditioning_type(enum kSMInputCond v) {
			condp_.conditioning_type = v;
			return *this;
		};
		ConditionInput &conditioning_param(tSM_INT v) {
			condp_.conditioning_param = v;
			return *this;
		};
		ConditionInput &alt_data_dest(tSMChannelId v) {
			condp_.alt_data_dest = v;
			return *this;
		};
		ConditionInput &alt_dest_type(enum kSMInputCondAltDest v) {
			condp_.alt_dest_type = v;
			return *this;
		};
		ConditionInput &ectest_type(int v) {
			condp_.ectest_type = v;
			return *this;
		};
		ConditionInput &ectest_gain(float v) {
			condp_.ectest_gain = v;
			return *this;
		};
		ConditionInput &ectest_delay(int v) {
			condp_.ectest_delay = v;
			return *this;
		};
		ConditionInput &ectest_flen(unsigned v) {
			condp_.ectest_flen = v;
			return *this;
		};
		ConditionInput &ectest_filt(float* v) {
			condp_.ectest_filt = v;
			return *this;
		};
	};
public:
	ConditionInput condition_input() {
		return ConditionInput().channel(channel_);
	}
private:
	class ConditionAdjust {
		mutable int used_;
		SM_CONDITION_ADJUST_PARMS condp_;
	public:
		ConditionAdjust(const ConditionAdjust &old) {
			*this = old;
			old.used_ = 1;
		};
		ConditionAdjust() : used_(0) {
			memset(&condp_, 0, sizeof(condp_));
		};
		operator Error() {
			Error e(Prosody::sm_condition_adjust(&condp_));
			used_ = 1;
			return e;
		};
		~ConditionAdjust() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_condition_adjust not called\n");
				abort();
			};
		};
		ConditionAdjust &channel(tSMChannelId v) {
			condp_.channel = v;
			return *this;
		};
		ConditionAdjust &adjust_type(enum kSMInputCondAdjust v) {
			condp_.adjust_type = v;
			return *this;
		};
		ConditionAdjust &adjust_value(tSM_INT v) {
			condp_.adjust_value = v;
			return *this;
		};
	};
public:
	ConditionAdjust condition_adjust() {
		return ConditionAdjust().channel(channel_);
	}
private:
	class ConditionAdjustSpan {
		mutable int used_;
		SM_CONDITION_ADJUST_SPAN_PARMS condp_;
	public:
		ConditionAdjustSpan(const ConditionAdjustSpan &old) {
			*this = old;
			old.used_ = 1;
		};
		ConditionAdjustSpan() : used_(0) {
			memset(&condp_, 0, sizeof(condp_));
		};
		operator Error() {
			Error e(Prosody::sm_condition_adjust_span(&condp_));
			used_ = 1;
			return e;
		};
		~ConditionAdjustSpan() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_condition_adjust_span not called\n");
				abort();
			};
		};
		ConditionAdjustSpan &channel(tSMChannelId v) {
			condp_.channel = v;
			return *this;
		};
		ConditionAdjustSpan &span(tSM_INT v) {
			condp_.span = v;
			return *this;
		};
	};
public:
	ConditionAdjustSpan condition_adjust_span() {
		return ConditionAdjustSpan().channel(channel_);
	}
private:
	class ConditionReinit {
		mutable int used_;
		tSMChannelId channel_;
	public:
		ConditionReinit(const ConditionReinit &old) {
			*this = old;
			old.used_ = 1;
		};
		ConditionReinit(tSMChannelId channel) : used_(0) {
			channel_ = channel;
		};
		operator Error() {
			Error e(Prosody::sm_condition_reinit(channel_));
			used_ = 1;
			return e;
		};
		~ConditionReinit() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_condition_reinit not called\n");
				abort();
			};
		};
	};
public:
	ConditionReinit condition_reinit() {
		return ConditionReinit(channel_);
	}
private:
	class CatsigListenFor {
		mutable int used_;
		SM_CATSIG_LISTEN_FOR_PARMS listenp_;
	public:
		CatsigListenFor(const CatsigListenFor &old) {
			*this = old;
			old.used_ = 1;
		};
		CatsigListenFor() : used_(0) {
			memset(&listenp_, 0, sizeof(listenp_));
		};
		operator Error() {
			Error e(Prosody::sm_catsig_listen_for(&listenp_));
			used_ = 1;
			return e;
		};
		~CatsigListenFor() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_catsig_listen_for not called\n");
				abort();
			};
		};
		CatsigListenFor &channel(tSMChannelId v) {
			listenp_.channel = v;
			return *this;
		};
		CatsigListenFor &catsig_alg_id(enum kBESPCatSigAlg v) {
			listenp_.catsig_alg_id = v;
			return *this;
		};
		CatsigListenFor &catsig_hint_param(tSM_INT v) {
			listenp_.catsig_hint_param = v;
			return *this;
		};
		CatsigListenFor &catsig_timeout(tSM_INT v) {
			listenp_.catsig_timeout = v;
			return *this;
		};
		CatsigListenFor &catsig_config_length(tSM_INT v) {
			listenp_.catsig_config_length = v;
			return *this;
		};
		CatsigListenFor &catsig_config_data(char* v) {
			listenp_.catsig_config_data = v;
			return *this;
		};
		CatsigListenFor &abort_catsig_alg(tSM_INT v) {
			listenp_.abort_catsig_alg = v;
			return *this;
		};
	};
public:
	CatsigListenFor catsig_listen_for() {
		return CatsigListenFor().channel(channel_);
	}
private:
	class CatsigSignallingHint {
		mutable int used_;
		SM_CATSIG_SIGNALLING_HINT_PARMS hintp_;
	public:
		CatsigSignallingHint(const CatsigSignallingHint &old) {
			*this = old;
			old.used_ = 1;
		};
		CatsigSignallingHint() : used_(0) {
			memset(&hintp_, 0, sizeof(hintp_));
		};
		operator Error() {
			Error e(Prosody::sm_catsig_signalling_hint(&hintp_));
			used_ = 1;
			return e;
		};
		~CatsigSignallingHint() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_catsig_signalling_hint not called\n");
				abort();
			};
		};
		CatsigSignallingHint &channel(tSMChannelId v) {
			hintp_.channel = v;
			return *this;
		};
		CatsigSignallingHint &catsig_hint_param(tSM_INT v) {
			hintp_.catsig_hint_param = v;
			return *this;
		};
	};
public:
	CatsigSignallingHint catsig_signalling_hint() {
		return CatsigSignallingHint().channel(channel_);
	}
private:
	class SetInputThreshold {
		mutable int used_;
		SM_CHANNEL_SET_INPUT_THRESHOLD_PARMS thp_;
	public:
		SetInputThreshold(const SetInputThreshold &old) {
			*this = old;
			old.used_ = 1;
		};
		SetInputThreshold() : used_(0) {
			memset(&thp_, 0, sizeof(thp_));
		};
		operator Error() {
			Error e(Prosody::sm_channel_set_input_threshold(&thp_));
			used_ = 1;
			return e;
		};
		~SetInputThreshold() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_channel_set_input_threshold not called\n");
				abort();
			};
		};
		SetInputThreshold &channel(tSMChannelId v) {
			thp_.channel = v;
			return *this;
		};
		SetInputThreshold &minimum_bits(tSM_INT v) {
			thp_.minimum_bits = v;
			return *this;
		};
	};
public:
	SetInputThreshold set_input_threshold() {
		return SetInputThreshold().channel(channel_);
	}
private:
	class SetOutputThreshold {
		mutable int used_;
		SM_CHANNEL_SET_OUTPUT_THRESHOLD_PARMS thp_;
	public:
		SetOutputThreshold(const SetOutputThreshold &old) {
			*this = old;
			old.used_ = 1;
		};
		SetOutputThreshold() : used_(0) {
			memset(&thp_, 0, sizeof(thp_));
		};
		operator Error() {
			Error e(Prosody::sm_channel_set_output_threshold(&thp_));
			used_ = 1;
			return e;
		};
		~SetOutputThreshold() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_channel_set_output_threshold not called\n");
				abort();
			};
		};
		SetOutputThreshold &channel(tSMChannelId v) {
			thp_.channel = v;
			return *this;
		};
		SetOutputThreshold &minimum_bits(tSM_INT v) {
			thp_.minimum_bits = v;
			return *this;
		};
	};
public:
	SetOutputThreshold set_output_threshold() {
		return SetOutputThreshold().channel(channel_);
	}
private:
	class PlayToneListPhaseReverse {
		mutable int used_;
		SM_PLAY_TONE_LIST_PHASE_REVERSE_PARMS pp_;
	public:
		PlayToneListPhaseReverse(const PlayToneListPhaseReverse &old) {
			*this = old;
			old.used_ = 1;
		};
		PlayToneListPhaseReverse() : used_(0) {
			memset(&pp_, 0, sizeof(pp_));
		};
		operator Error() {
			Error e(Prosody::sm_play_tone_list_phase_reverse(&pp_));
			used_ = 1;
			return e;
		};
		~PlayToneListPhaseReverse() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_play_tone_list_phase_reverse not called\n");
				abort();
			};
		};
		PlayToneListPhaseReverse &channel(tSMChannelId v) {
			pp_.channel = v;
			return *this;
		};
		PlayToneListPhaseReverse &period(tSM_UT32 v) {
			pp_.period = v;
			return *this;
		};
	};
public:
	PlayToneListPhaseReverse play_tone_list_phase_reverse() {
		return PlayToneListPhaseReverse().channel(channel_);
	}
private:
	class BeepListenFor {
		mutable int used_;
		SM_BEEP_LISTEN_FOR_PARMS listenp_;
	public:
		BeepListenFor(const BeepListenFor &old) {
			*this = old;
			old.used_ = 1;
		};
		BeepListenFor() : used_(0) {
			memset(&listenp_, 0, sizeof(listenp_));
		};
		operator Error() {
			Error e(Prosody::sm_beep_listen_for(&listenp_));
			used_ = 1;
			return e;
		};
		~BeepListenFor() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_beep_listen_for not called\n");
				abort();
			};
		};
		BeepListenFor &channel(tSMChannelId v) {
			listenp_.channel = v;
			return *this;
		};
		BeepListenFor &min_duration(tSM_INT v) {
			listenp_.min_duration = v;
			return *this;
		};
		BeepListenFor &upper_limit(double v) {
			listenp_.upper_limit = v;
			return *this;
		};
		BeepListenFor &lower_limit(double v) {
			listenp_.lower_limit = v;
			return *this;
		};
	};
public:
	BeepListenFor beep_listen_for() {
		return BeepListenFor().channel(channel_);
	}
private:
	class OnhookListenFor {
		mutable int used_;
		SM_ONHOOK_LISTEN_FOR_PARMS listenp_;
	public:
		OnhookListenFor(const OnhookListenFor &old) {
			*this = old;
			old.used_ = 1;
		};
		OnhookListenFor() : used_(0) {
			memset(&listenp_, 0, sizeof(listenp_));
		};
		operator Error() {
			Error e(Prosody::sm_onhook_listen_for(&listenp_));
			used_ = 1;
			return e;
		};
		~OnhookListenFor() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_onhook_listen_for not called\n");
				abort();
			};
		};
		OnhookListenFor &channel(tSMChannelId v) {
			listenp_.channel = v;
			return *this;
		};
		OnhookListenFor &enable(tSM_INT v) {
			listenp_.enable = v;
			return *this;
		};
		OnhookListenFor &pre_pulse_max_power(double v) {
			listenp_.pre_pulse_max_power = v;
			return *this;
		};
		OnhookListenFor &pulse_min_power(double v) {
			listenp_.pulse_min_power = v;
			return *this;
		};
		OnhookListenFor &post_pulse_floor(double v) {
			listenp_.post_pulse_floor = v;
			return *this;
		};
		OnhookListenFor &latitude(double v) {
			listenp_.latitude = v;
			return *this;
		};
		OnhookListenFor &count_ratio(double v) {
			listenp_.count_ratio = v;
			return *this;
		};
		OnhookListenFor &total_ratio(double v) {
			listenp_.total_ratio = v;
			return *this;
		};
		OnhookListenFor &max_duration(tSM_INT v) {
			listenp_.max_duration = v;
			return *this;
		};
	};
public:
	OnhookListenFor onhook_listen_for() {
		return OnhookListenFor().channel(channel_);
	}
