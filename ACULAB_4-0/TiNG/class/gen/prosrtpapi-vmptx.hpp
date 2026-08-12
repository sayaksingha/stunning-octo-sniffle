private:
	class Config {
		mutable int used_;
		SM_VMPTX_CONFIG_PARMS configp_;
	public:
		Config(const Config &old) {
			*this = old;
			old.used_ = 1;
		};
		Config() : used_(0) {
			memset(&configp_, 0, sizeof(configp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config(&configp_));
			used_ = 1;
			return e;
		};
		~Config() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config not called\n");
				abort();
			};
		};
		Config &vmptx(tSMVMPtxId v) {
			configp_.vmptx = v;
			return *this;
		};
		Config &destination_rtp(SOCKADDR_IN v) {
			configp_.destination_rtp = v;
			return *this;
		};
		Config &source_rtp(SOCKADDR_IN v) {
			configp_.source_rtp = v;
			return *this;
		};
		Config &TOS_RTP(int v) {
			configp_.TOS_RTP = v;
			return *this;
		};
		Config &destination_rtcp(SOCKADDR_IN v) {
			configp_.destination_rtcp = v;
			return *this;
		};
		Config &source_rtcp(SOCKADDR_IN v) {
			configp_.source_rtcp = v;
			return *this;
		};
		Config &TOS_RTCP(int v) {
			configp_.TOS_RTCP = v;
			return *this;
		};
	};
public:
	Config config() {
		return Config().vmptx(vmptx_);
	}
private:
	class ConfigIpv6 {
		mutable int used_;
		SM_VMPTX_CONFIG_IPV6_PARMS configp_;
	public:
		ConfigIpv6(const ConfigIpv6 &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigIpv6() : used_(0) {
			memset(&configp_, 0, sizeof(configp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_ipv6(&configp_));
			used_ = 1;
			return e;
		};
		~ConfigIpv6() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_ipv6 not called\n");
				abort();
			};
		};
		ConfigIpv6 &vmptx(tSMVMPtxId v) {
			configp_.vmptx = v;
			return *this;
		};
		ConfigIpv6 &destination_rtp(SOCKADDR_IN6 v) {
			configp_.destination_rtp = v;
			return *this;
		};
		ConfigIpv6 &source_rtp(SOCKADDR_IN6 v) {
			configp_.source_rtp = v;
			return *this;
		};
		ConfigIpv6 &destination_rtcp(SOCKADDR_IN6 v) {
			configp_.destination_rtcp = v;
			return *this;
		};
		ConfigIpv6 &source_rtcp(SOCKADDR_IN6 v) {
			configp_.source_rtcp = v;
			return *this;
		};
	};
public:
	ConfigIpv6 config_ipv6() {
		return ConfigIpv6().vmptx(vmptx_);
	}
private:
	class ConfigAuthenticationHmacSha1 {
		mutable int used_;
		SM_VMPTX_CONFIG_AUTHENTICATION_HMAC_SHA1_PARMS pp_;
	public:
		ConfigAuthenticationHmacSha1(const ConfigAuthenticationHmacSha1 &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigAuthenticationHmacSha1() : used_(0) {
			memset(&pp_, 0, sizeof(pp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_authentication_hmac_sha1(&pp_));
			used_ = 1;
			return e;
		};
		~ConfigAuthenticationHmacSha1() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_authentication_hmac_sha1 not called\n");
				abort();
			};
		};
		ConfigAuthenticationHmacSha1 &vmptx(tSMVMPtxId v) {
			pp_.vmptx = v;
			return *this;
		};
		ConfigAuthenticationHmacSha1 &keylen(tSM_INT v) {
			pp_.keylen = v;
			return *this;
		};
		ConfigAuthenticationHmacSha1 &key(char* v) {
			pp_.key = v;
			return *this;
		};
		ConfigAuthenticationHmacSha1 &taglen(tSM_INT v) {
			pp_.taglen = v;
			return *this;
		};
	};
public:
	ConfigAuthenticationHmacSha1 config_authentication_hmac_sha1() {
		return ConfigAuthenticationHmacSha1().vmptx(vmptx_);
	}
private:
	class ConfigAuthenticationNull {
		mutable int used_;
		SM_VMPTX_CONFIG_AUTHENTICATION_NULL_PARMS pp_;
	public:
		ConfigAuthenticationNull(const ConfigAuthenticationNull &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigAuthenticationNull() : used_(0) {
			memset(&pp_, 0, sizeof(pp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_authentication_null(&pp_));
			used_ = 1;
			return e;
		};
		~ConfigAuthenticationNull() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_authentication_null not called\n");
				abort();
			};
		};
		ConfigAuthenticationNull &vmptx(tSMVMPtxId v) {
			pp_.vmptx = v;
			return *this;
		};
	};
public:
	ConfigAuthenticationNull config_authentication_null() {
		return ConfigAuthenticationNull().vmptx(vmptx_);
	}
private:
	class ConfigCodecAlaw {
		mutable int used_;
		SM_VMPTX_CODEC_ALAW_PARMS codecp_;
	public:
		ConfigCodecAlaw(const ConfigCodecAlaw &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecAlaw() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_alaw(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecAlaw() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_alaw not called\n");
				abort();
			};
		};
		ConfigCodecAlaw &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecAlaw &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecAlaw &VADMode(enum kSMVMPTxVADMode v) {
			codecp_.VADMode = v;
			return *this;
		};
		ConfigCodecAlaw &ptime(tSM_INT v) {
			codecp_.ptime = v;
			return *this;
		};
	};
public:
	ConfigCodecAlaw config_codec_alaw() {
		return ConfigCodecAlaw().vmptx(vmptx_);
	}
private:
	class ConfigCodecAmrnb {
		mutable int used_;
		SM_VMPTX_CODEC_AMRNB_PARMS codecp_;
	public:
		ConfigCodecAmrnb(const ConfigCodecAmrnb &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecAmrnb() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_amrnb(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecAmrnb() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_amrnb not called\n");
				abort();
			};
		};
		ConfigCodecAmrnb &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecAmrnb &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecAmrnb &aligned(tSM_INT v) {
			codecp_.aligned = v;
			return *this;
		};
		ConfigCodecAmrnb &gsmefr(tSM_INT v) {
			codecp_.gsmefr = v;
			return *this;
		};
		ConfigCodecAmrnb &VADMode(enum kSMVMPTxVADMode v) {
			codecp_.VADMode = v;
			return *this;
		};
		ConfigCodecAmrnb &frames_per_packet(tSM_INT v) {
			codecp_.frames_per_packet = v;
			return *this;
		};
	};
public:
	ConfigCodecAmrnb config_codec_amrnb() {
		return ConfigCodecAmrnb().vmptx(vmptx_);
	}
private:
	class ConfigCodecAmrnbCmr {
		mutable int used_;
		SM_VMPTX_CONFIG_CODEC_AMRNB_CMR_PARMS codecp_;
	public:
		ConfigCodecAmrnbCmr(const ConfigCodecAmrnbCmr &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecAmrnbCmr() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_amrnb_cmr(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecAmrnbCmr() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_amrnb_cmr not called\n");
				abort();
			};
		};
		ConfigCodecAmrnbCmr &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecAmrnbCmr &bitrate(tSM_INT v) {
			codecp_.bitrate = v;
			return *this;
		};
	};
public:
	ConfigCodecAmrnbCmr config_codec_amrnb_cmr() {
		return ConfigCodecAmrnbCmr().vmptx(vmptx_);
	}
private:
	class ConfigCodecAmrnbMode {
		mutable int used_;
		SM_VMPTX_CONFIG_CODEC_AMRNB_MODE_PARMS codecp_;
	public:
		ConfigCodecAmrnbMode(const ConfigCodecAmrnbMode &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecAmrnbMode() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_amrnb_mode(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecAmrnbMode() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_amrnb_mode not called\n");
				abort();
			};
		};
		ConfigCodecAmrnbMode &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecAmrnbMode &bitrate(tSM_INT v) {
			codecp_.bitrate = v;
			return *this;
		};
	};
public:
	ConfigCodecAmrnbMode config_codec_amrnb_mode() {
		return ConfigCodecAmrnbMode().vmptx(vmptx_);
	}
private:
	class ConfigCodecG7221 {
		mutable int used_;
		SM_VMPTX_CODEC_G722_1_PARMS codecp_;
	public:
		ConfigCodecG7221(const ConfigCodecG7221 &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecG7221() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_g722_1(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecG7221() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_g722_1 not called\n");
				abort();
			};
		};
		ConfigCodecG7221 &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecG7221 &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecG7221 &bitrate(tSM_INT v) {
			codecp_.bitrate = v;
			return *this;
		};
		ConfigCodecG7221 &frames_per_packet(tSM_INT v) {
			codecp_.frames_per_packet = v;
			return *this;
		};
	};
public:
	ConfigCodecG7221 config_codec_g722_1() {
		return ConfigCodecG7221().vmptx(vmptx_);
	}
private:
	class ConfigCodecSilk {
		mutable int used_;
		SM_VMPTX_CODEC_SILK_PARMS codecp_;
	public:
		ConfigCodecSilk(const ConfigCodecSilk &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecSilk() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_silk(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecSilk() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_silk not called\n");
				abort();
			};
		};
		ConfigCodecSilk &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecSilk &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecSilk &sample_rate(tSM_INT v) {
			codecp_.sample_rate = v;
			return *this;
		};
		ConfigCodecSilk &internal_rate(tSM_INT v) {
			codecp_.internal_rate = v;
			return *this;
		};
		ConfigCodecSilk &bit_rate(tSM_INT v) {
			codecp_.bit_rate = v;
			return *this;
		};
		ConfigCodecSilk &frames_per_packet(tSM_INT v) {
			codecp_.frames_per_packet = v;
			return *this;
		};
		ConfigCodecSilk &packet_loss(tSM_INT v) {
			codecp_.packet_loss = v;
			return *this;
		};
		ConfigCodecSilk &complexity(tSM_INT v) {
			codecp_.complexity = v;
			return *this;
		};
		ConfigCodecSilk &use_fec(tSM_INT v) {
			codecp_.use_fec = v;
			return *this;
		};
		ConfigCodecSilk &use_dtx(tSM_INT v) {
			codecp_.use_dtx = v;
			return *this;
		};
	};
public:
	ConfigCodecSilk config_codec_silk() {
		return ConfigCodecSilk().vmptx(vmptx_);
	}
private:
	class ConfigCodecSilkMode {
		mutable int used_;
		SM_VMPTX_CONFIG_CODEC_SILK_MODE_PARMS codecp_;
	public:
		ConfigCodecSilkMode(const ConfigCodecSilkMode &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecSilkMode() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_silk_mode(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecSilkMode() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_silk_mode not called\n");
				abort();
			};
		};
		ConfigCodecSilkMode &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecSilkMode &sample_rate(tSM_INT v) {
			codecp_.sample_rate = v;
			return *this;
		};
		ConfigCodecSilkMode &internal_rate(tSM_INT v) {
			codecp_.internal_rate = v;
			return *this;
		};
		ConfigCodecSilkMode &bit_rate(tSM_INT v) {
			codecp_.bit_rate = v;
			return *this;
		};
		ConfigCodecSilkMode &frames_per_packet(tSM_INT v) {
			codecp_.frames_per_packet = v;
			return *this;
		};
		ConfigCodecSilkMode &packet_loss(tSM_INT v) {
			codecp_.packet_loss = v;
			return *this;
		};
		ConfigCodecSilkMode &complexity(tSM_INT v) {
			codecp_.complexity = v;
			return *this;
		};
		ConfigCodecSilkMode &use_fec(tSM_INT v) {
			codecp_.use_fec = v;
			return *this;
		};
		ConfigCodecSilkMode &use_dtx(tSM_INT v) {
			codecp_.use_dtx = v;
			return *this;
		};
	};
public:
	ConfigCodecSilkMode config_codec_silk_mode() {
		return ConfigCodecSilkMode().vmptx(vmptx_);
	}
private:
	class ConfigCodecAmrwb {
		mutable int used_;
		SM_VMPTX_CODEC_AMRWB_PARMS codecp_;
	public:
		ConfigCodecAmrwb(const ConfigCodecAmrwb &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecAmrwb() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_amrwb(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecAmrwb() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_amrwb not called\n");
				abort();
			};
		};
		ConfigCodecAmrwb &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecAmrwb &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecAmrwb &aligned(tSM_INT v) {
			codecp_.aligned = v;
			return *this;
		};
		ConfigCodecAmrwb &VADMode(tSM_INT v) {
			codecp_.VADMode = v;
			return *this;
		};
		ConfigCodecAmrwb &frames_per_packet(tSM_INT v) {
			codecp_.frames_per_packet = v;
			return *this;
		};
	};
public:
	ConfigCodecAmrwb config_codec_amrwb() {
		return ConfigCodecAmrwb().vmptx(vmptx_);
	}
private:
	class ConfigCodecAmrwbCmr {
		mutable int used_;
		SM_VMPTX_CONFIG_CODEC_AMRWB_CMR_PARMS codecp_;
	public:
		ConfigCodecAmrwbCmr(const ConfigCodecAmrwbCmr &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecAmrwbCmr() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_amrwb_cmr(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecAmrwbCmr() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_amrwb_cmr not called\n");
				abort();
			};
		};
		ConfigCodecAmrwbCmr &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecAmrwbCmr &bitrate(tSM_INT v) {
			codecp_.bitrate = v;
			return *this;
		};
	};
public:
	ConfigCodecAmrwbCmr config_codec_amrwb_cmr() {
		return ConfigCodecAmrwbCmr().vmptx(vmptx_);
	}
private:
	class ConfigCodecAmrwbMode {
		mutable int used_;
		SM_VMPTX_CONFIG_CODEC_AMRWB_MODE_PARMS codecp_;
	public:
		ConfigCodecAmrwbMode(const ConfigCodecAmrwbMode &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecAmrwbMode() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_amrwb_mode(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecAmrwbMode() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_amrwb_mode not called\n");
				abort();
			};
		};
		ConfigCodecAmrwbMode &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecAmrwbMode &bitrate(tSM_INT v) {
			codecp_.bitrate = v;
			return *this;
		};
	};
public:
	ConfigCodecAmrwbMode config_codec_amrwb_mode() {
		return ConfigCodecAmrwbMode().vmptx(vmptx_);
	}
private:
	class ConfigCodecSpeex {
		mutable int used_;
		SM_VMPTX_CODEC_SPEEX_PARMS codecp_;
	public:
		ConfigCodecSpeex(const ConfigCodecSpeex &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecSpeex() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_speex(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecSpeex() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_speex not called\n");
				abort();
			};
		};
		ConfigCodecSpeex &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecSpeex &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecSpeex &bandwidth(enum kSMSpeexMode v) {
			codecp_.bandwidth = v;
			return *this;
		};
		ConfigCodecSpeex &VADMode(enum kSMVMPTxVADMode v) {
			codecp_.VADMode = v;
			return *this;
		};
		ConfigCodecSpeex &frames_per_packet(tSM_INT v) {
			codecp_.frames_per_packet = v;
			return *this;
		};
	};
public:
	ConfigCodecSpeex config_codec_speex() {
		return ConfigCodecSpeex().vmptx(vmptx_);
	}
private:
	class ConfigCodecSpeexMode {
		mutable int used_;
		SM_VMPTX_CODEC_SPEEX_MODE_PARMS codecp_;
	public:
		ConfigCodecSpeexMode(const ConfigCodecSpeexMode &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecSpeexMode() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_speex_mode(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecSpeexMode() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_speex_mode not called\n");
				abort();
			};
		};
		ConfigCodecSpeexMode &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecSpeexMode &complexity(tSM_UT32 v) {
			codecp_.complexity = v;
			return *this;
		};
		ConfigCodecSpeexMode &quality(tSM_UT32 v) {
			codecp_.quality = v;
			return *this;
		};
		ConfigCodecSpeexMode &bitrate(tSM_UT32 v) {
			codecp_.bitrate = v;
			return *this;
		};
		ConfigCodecSpeexMode &denoiser(tSM_UT32 v) {
			codecp_.denoiser = v;
			return *this;
		};
	};
public:
	ConfigCodecSpeexMode config_codec_speex_mode() {
		return ConfigCodecSpeexMode().vmptx(vmptx_);
	}
private:
	class ConfigCodecComfortNoise {
		mutable int used_;
		SM_VMPTX_CODEC_COMFORT_NOISE_PARMS codecp_;
	public:
		ConfigCodecComfortNoise(const ConfigCodecComfortNoise &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecComfortNoise() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_comfort_noise(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecComfortNoise() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_comfort_noise not called\n");
				abort();
			};
		};
		ConfigCodecComfortNoise &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecComfortNoise &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
	};
public:
	ConfigCodecComfortNoise config_codec_comfort_noise() {
		return ConfigCodecComfortNoise().vmptx(vmptx_);
	}
private:
	class ConfigCodecEvrc {
		mutable int used_;
		SM_VMPTX_CODEC_EVRC_PARMS codecp_;
	public:
		ConfigCodecEvrc(const ConfigCodecEvrc &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecEvrc() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_evrc(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecEvrc() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_evrc not called\n");
				abort();
			};
		};
		ConfigCodecEvrc &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecEvrc &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecEvrc &high_pass_filter(tSM_INT v) {
			codecp_.high_pass_filter = v;
			return *this;
		};
		ConfigCodecEvrc &noise_suppression_filter(tSM_INT v) {
			codecp_.noise_suppression_filter = v;
			return *this;
		};
		ConfigCodecEvrc &rtp_mode(enum kSMEVRCRTPMode v) {
			codecp_.rtp_mode = v;
			return *this;
		};
		ConfigCodecEvrc &max_rate(enum kSMEVRCRate v) {
			codecp_.max_rate = v;
			return *this;
		};
		ConfigCodecEvrc &min_rate(enum kSMEVRCRate v) {
			codecp_.min_rate = v;
			return *this;
		};
		ConfigCodecEvrc &VADMode(enum kSMVMPTxVADMode v) {
			codecp_.VADMode = v;
			return *this;
		};
		ConfigCodecEvrc &frames_per_packet(tSM_INT v) {
			codecp_.frames_per_packet = v;
			return *this;
		};
	};
public:
	ConfigCodecEvrc config_codec_evrc() {
		return ConfigCodecEvrc().vmptx(vmptx_);
	}
private:
	class ConfigCodecG7231 {
		mutable int used_;
		SM_VMPTX_CODEC_G723_1_PARMS codecp_;
	public:
		ConfigCodecG7231(const ConfigCodecG7231 &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecG7231() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_g723_1(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecG7231() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_g723_1 not called\n");
				abort();
			};
		};
		ConfigCodecG7231 &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecG7231 &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecG7231 &high_pass_filter(tSM_INT v) {
			codecp_.high_pass_filter = v;
			return *this;
		};
		ConfigCodecG7231 &rate(tSM_INT v) {
			codecp_.rate = v;
			return *this;
		};
		ConfigCodecG7231 &silence_compression(tSM_INT v) {
			codecp_.silence_compression = v;
			return *this;
		};
		ConfigCodecG7231 &frames_per_packet(tSM_INT v) {
			codecp_.frames_per_packet = v;
			return *this;
		};
	};
public:
	ConfigCodecG7231 config_codec_g723_1() {
		return ConfigCodecG7231().vmptx(vmptx_);
	}
private:
	class ConfigCodecG722 {
		mutable int used_;
		SM_VMPTX_CODEC_G722_PARMS codecp_;
	public:
		ConfigCodecG722(const ConfigCodecG722 &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecG722() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_g722(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecG722() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_g722 not called\n");
				abort();
			};
		};
		ConfigCodecG722 &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecG722 &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecG722 &packetlen(tSM_INT v) {
			codecp_.packetlen = v;
			return *this;
		};
	};
public:
	ConfigCodecG722 config_codec_g722() {
		return ConfigCodecG722().vmptx(vmptx_);
	}
private:
	class ConfigCodecG726 {
		mutable int used_;
		SM_VMPTX_CODEC_G726_PARMS codecp_;
	public:
		ConfigCodecG726(const ConfigCodecG726 &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecG726() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_g726(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecG726() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_g726 not called\n");
				abort();
			};
		};
		ConfigCodecG726 &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecG726 &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecG726 &bits(tSM_INT v) {
			codecp_.bits = v;
			return *this;
		};
		ConfigCodecG726 &VADMode(enum kSMVMPTxVADMode v) {
			codecp_.VADMode = v;
			return *this;
		};
		ConfigCodecG726 &packetlen(tSM_INT v) {
			codecp_.packetlen = v;
			return *this;
		};
		ConfigCodecG726 &variant(enum kSMG726Variant v) {
			codecp_.variant = v;
			return *this;
		};
	};
public:
	ConfigCodecG726 config_codec_g726() {
		return ConfigCodecG726().vmptx(vmptx_);
	}
private:
	class ConfigCodecG728 {
		mutable int used_;
		SM_VMPTX_CODEC_G728_PARMS codecp_;
	public:
		ConfigCodecG728(const ConfigCodecG728 &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecG728() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_g728(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecG728() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_g728 not called\n");
				abort();
			};
		};
		ConfigCodecG728 &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecG728 &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecG728 &rate(tSM_INT v) {
			codecp_.rate = v;
			return *this;
		};
		ConfigCodecG728 &VADMode(enum kSMVMPTxVADMode v) {
			codecp_.VADMode = v;
			return *this;
		};
		ConfigCodecG728 &ptime(tSM_INT v) {
			codecp_.ptime = v;
			return *this;
		};
	};
public:
	ConfigCodecG728 config_codec_g728() {
		return ConfigCodecG728().vmptx(vmptx_);
	}
private:
	class ConfigCodecG729ab {
		mutable int used_;
		SM_VMPTX_CODEC_G729AB_PARMS codecp_;
	public:
		ConfigCodecG729ab(const ConfigCodecG729ab &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecG729ab() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_g729ab(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecG729ab() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_g729ab not called\n");
				abort();
			};
		};
		ConfigCodecG729ab &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecG729ab &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecG729ab &VADMode(enum kSMVMPTxVADMode v) {
			codecp_.VADMode = v;
			return *this;
		};
		ConfigCodecG729ab &ptime(tSM_INT v) {
			codecp_.ptime = v;
			return *this;
		};
	};
public:
	ConfigCodecG729ab config_codec_g729ab() {
		return ConfigCodecG729ab().vmptx(vmptx_);
	}
private:
	class ConfigCodecG729i {
		mutable int used_;
		SM_VMPTX_CODEC_G729I_PARMS codecp_;
	public:
		ConfigCodecG729i(const ConfigCodecG729i &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecG729i() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_g729i(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecG729i() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_g729i not called\n");
				abort();
			};
		};
		ConfigCodecG729i &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecG729i &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecG729i &g729_mode(enum kSMG729IMode v) {
			codecp_.g729_mode = v;
			return *this;
		};
		ConfigCodecG729i &VADMode(enum kSMVMPTxVADMode v) {
			codecp_.VADMode = v;
			return *this;
		};
		ConfigCodecG729i &ptime(tSM_INT v) {
			codecp_.ptime = v;
			return *this;
		};
	};
public:
	ConfigCodecG729i config_codec_g729i() {
		return ConfigCodecG729i().vmptx(vmptx_);
	}
private:
	class ConfigCodecGsmefr {
		mutable int used_;
		SM_VMPTX_CODEC_GSMEFR_PARMS codecp_;
	public:
		ConfigCodecGsmefr(const ConfigCodecGsmefr &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecGsmefr() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_gsmefr(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecGsmefr() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_gsmefr not called\n");
				abort();
			};
		};
		ConfigCodecGsmefr &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecGsmefr &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecGsmefr &VADMode(enum kSMVMPTxVADMode v) {
			codecp_.VADMode = v;
			return *this;
		};
		ConfigCodecGsmefr &frames_per_packet(tSM_INT v) {
			codecp_.frames_per_packet = v;
			return *this;
		};
	};
public:
	ConfigCodecGsmefr config_codec_gsmefr() {
		return ConfigCodecGsmefr().vmptx(vmptx_);
	}
private:
	class ConfigCodecGsmfr {
		mutable int used_;
		SM_VMPTX_CODEC_GSMFR_PARMS codecp_;
	public:
		ConfigCodecGsmfr(const ConfigCodecGsmfr &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecGsmfr() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_gsmfr(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecGsmfr() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_gsmfr not called\n");
				abort();
			};
		};
		ConfigCodecGsmfr &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecGsmfr &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecGsmfr &variant(enum kSMGSMVariant v) {
			codecp_.variant = v;
			return *this;
		};
		ConfigCodecGsmfr &VADMode(enum kSMVMPTxVADMode v) {
			codecp_.VADMode = v;
			return *this;
		};
		ConfigCodecGsmfr &frames_per_packet(tSM_INT v) {
			codecp_.frames_per_packet = v;
			return *this;
		};
	};
public:
	ConfigCodecGsmfr config_codec_gsmfr() {
		return ConfigCodecGsmfr().vmptx(vmptx_);
	}
private:
	class ConfigCodecGsmhr {
		mutable int used_;
		SM_VMPTX_CODEC_GSMHR_PARMS codecp_;
	public:
		ConfigCodecGsmhr(const ConfigCodecGsmhr &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecGsmhr() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_gsmhr(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecGsmhr() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_gsmhr not called\n");
				abort();
			};
		};
		ConfigCodecGsmhr &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecGsmhr &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecGsmhr &VADMode(enum kSMVMPTxVADMode v) {
			codecp_.VADMode = v;
			return *this;
		};
		ConfigCodecGsmhr &frames_per_packet(tSM_INT v) {
			codecp_.frames_per_packet = v;
			return *this;
		};
	};
public:
	ConfigCodecGsmhr config_codec_gsmhr() {
		return ConfigCodecGsmhr().vmptx(vmptx_);
	}
private:
	class ConfigCodecIlbc {
		mutable int used_;
		SM_VMPTX_CODEC_ILBC_PARMS codecp_;
	public:
		ConfigCodecIlbc(const ConfigCodecIlbc &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecIlbc() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_ilbc(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecIlbc() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_ilbc not called\n");
				abort();
			};
		};
		ConfigCodecIlbc &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecIlbc &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecIlbc &frame_len(tSM_INT v) {
			codecp_.frame_len = v;
			return *this;
		};
		ConfigCodecIlbc &VADMode(enum kSMVMPTxVADMode v) {
			codecp_.VADMode = v;
			return *this;
		};
		ConfigCodecIlbc &frames_per_packet(tSM_INT v) {
			codecp_.frames_per_packet = v;
			return *this;
		};
	};
public:
	ConfigCodecIlbc config_codec_ilbc() {
		return ConfigCodecIlbc().vmptx(vmptx_);
	}
private:
	class ConfigCodecIsac {
		mutable int used_;
		SM_VMPTX_CODEC_ISAC_PARMS codecp_;
	public:
		ConfigCodecIsac(const ConfigCodecIsac &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecIsac() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_isac(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecIsac() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_isac not called\n");
				abort();
			};
		};
		ConfigCodecIsac &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecIsac &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecIsac &partner(tSMVMPrxId v) {
			codecp_.partner = v;
			return *this;
		};
		ConfigCodecIsac &channel_associated_mode(tSM_INT v) {
			codecp_.channel_associated_mode = v;
			return *this;
		};
	};
public:
	ConfigCodecIsac config_codec_isac() {
		return ConfigCodecIsac().vmptx(vmptx_);
	}
private:
	class ConfigCodecL16 {
		mutable int used_;
		SM_VMPTX_CODEC_L16_PARMS codecp_;
	public:
		ConfigCodecL16(const ConfigCodecL16 &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecL16() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_l16(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecL16() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_l16 not called\n");
				abort();
			};
		};
		ConfigCodecL16 &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecL16 &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecL16 &VADMode(enum kSMVMPTxVADMode v) {
			codecp_.VADMode = v;
			return *this;
		};
		ConfigCodecL16 &ptime(tSM_INT v) {
			codecp_.ptime = v;
			return *this;
		};
	};
public:
	ConfigCodecL16 config_codec_l16() {
		return ConfigCodecL16().vmptx(vmptx_);
	}
private:
	class ConfigCodecL8 {
		mutable int used_;
		SM_VMPTX_CODEC_L8_PARMS codecp_;
	public:
		ConfigCodecL8(const ConfigCodecL8 &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecL8() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_l8(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecL8() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_l8 not called\n");
				abort();
			};
		};
		ConfigCodecL8 &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecL8 &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecL8 &VADMode(enum kSMVMPTxVADMode v) {
			codecp_.VADMode = v;
			return *this;
		};
		ConfigCodecL8 &ptime(tSM_INT v) {
			codecp_.ptime = v;
			return *this;
		};
	};
public:
	ConfigCodecL8 config_codec_l8() {
		return ConfigCodecL8().vmptx(vmptx_);
	}
private:
	class ConfigCodecMelpe {
		mutable int used_;
		SM_VMPTX_CODEC_MELPE_PARMS codecp_;
	public:
		ConfigCodecMelpe(const ConfigCodecMelpe &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecMelpe() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_melpe(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecMelpe() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_melpe not called\n");
				abort();
			};
		};
		ConfigCodecMelpe &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecMelpe &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecMelpe &ptime(tSM_INT v) {
			codecp_.ptime = v;
			return *this;
		};
	};
public:
	ConfigCodecMelpe config_codec_melpe() {
		return ConfigCodecMelpe().vmptx(vmptx_);
	}
private:
	class ConfigCodecMulaw {
		mutable int used_;
		SM_VMPTX_CODEC_MULAW_PARMS codecp_;
	public:
		ConfigCodecMulaw(const ConfigCodecMulaw &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecMulaw() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_mulaw(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecMulaw() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_mulaw not called\n");
				abort();
			};
		};
		ConfigCodecMulaw &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecMulaw &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecMulaw &VADMode(enum kSMVMPTxVADMode v) {
			codecp_.VADMode = v;
			return *this;
		};
		ConfigCodecMulaw &ptime(tSM_INT v) {
			codecp_.ptime = v;
			return *this;
		};
	};
public:
	ConfigCodecMulaw config_codec_mulaw() {
		return ConfigCodecMulaw().vmptx(vmptx_);
	}
private:
	class ConfigCodecOpus {
		mutable int used_;
		SM_VMPTX_CODEC_OPUS_PARMS codecp_;
	public:
		ConfigCodecOpus(const ConfigCodecOpus &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecOpus() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_opus(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecOpus() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_opus not called\n");
				abort();
			};
		};
		ConfigCodecOpus &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecOpus &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecOpus &sample_rate(tSM_INT v) {
			codecp_.sample_rate = v;
			return *this;
		};
		ConfigCodecOpus &bit_rate(tSM_INT v) {
			codecp_.bit_rate = v;
			return *this;
		};
		ConfigCodecOpus &ptime(tSM_INT v) {
			codecp_.ptime = v;
			return *this;
		};
		ConfigCodecOpus &packet_loss(tSM_INT v) {
			codecp_.packet_loss = v;
			return *this;
		};
		ConfigCodecOpus &complexity(tSM_INT v) {
			codecp_.complexity = v;
			return *this;
		};
		ConfigCodecOpus &use_fec(tSM_INT v) {
			codecp_.use_fec = v;
			return *this;
		};
		ConfigCodecOpus &use_dtx(tSM_INT v) {
			codecp_.use_dtx = v;
			return *this;
		};
		ConfigCodecOpus &rate_control(enum kSMOPUSRate v) {
			codecp_.rate_control = v;
			return *this;
		};
		ConfigCodecOpus &signal_type_hint(enum kSMOPUSSignalTypeHint v) {
			codecp_.signal_type_hint = v;
			return *this;
		};
		ConfigCodecOpus &application_type(enum kSMOPUSAppType v) {
			codecp_.application_type = v;
			return *this;
		};
		ConfigCodecOpus &disable_prediction(tSM_INT v) {
			codecp_.disable_prediction = v;
			return *this;
		};
	};
public:
	ConfigCodecOpus config_codec_opus() {
		return ConfigCodecOpus().vmptx(vmptx_);
	}
private:
	class ConfigCodecOpusMode {
		mutable int used_;
		SM_VMPTX_CONFIG_CODEC_OPUS_MODE_PARMS codecp_;
	public:
		ConfigCodecOpusMode(const ConfigCodecOpusMode &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecOpusMode() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_opus_mode(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecOpusMode() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_opus_mode not called\n");
				abort();
			};
		};
		ConfigCodecOpusMode &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecOpusMode &sample_rate(tSM_INT v) {
			codecp_.sample_rate = v;
			return *this;
		};
		ConfigCodecOpusMode &bit_rate(tSM_INT v) {
			codecp_.bit_rate = v;
			return *this;
		};
		ConfigCodecOpusMode &ptime(tSM_INT v) {
			codecp_.ptime = v;
			return *this;
		};
		ConfigCodecOpusMode &packet_loss(tSM_INT v) {
			codecp_.packet_loss = v;
			return *this;
		};
		ConfigCodecOpusMode &complexity(tSM_INT v) {
			codecp_.complexity = v;
			return *this;
		};
		ConfigCodecOpusMode &use_fec(tSM_INT v) {
			codecp_.use_fec = v;
			return *this;
		};
		ConfigCodecOpusMode &use_dtx(tSM_INT v) {
			codecp_.use_dtx = v;
			return *this;
		};
		ConfigCodecOpusMode &rate_control(enum kSMOPUSRate v) {
			codecp_.rate_control = v;
			return *this;
		};
		ConfigCodecOpusMode &signal_type_hint(enum kSMOPUSSignalTypeHint v) {
			codecp_.signal_type_hint = v;
			return *this;
		};
		ConfigCodecOpusMode &application_type(enum kSMOPUSAppType v) {
			codecp_.application_type = v;
			return *this;
		};
		ConfigCodecOpusMode &disable_prediction(tSM_INT v) {
			codecp_.disable_prediction = v;
			return *this;
		};
	};
public:
	ConfigCodecOpusMode config_codec_opus_mode() {
		return ConfigCodecOpusMode().vmptx(vmptx_);
	}
private:
	class ConfigCodecRfc2833 {
		mutable int used_;
		SM_VMPTX_CODEC_RFC2833_PARMS codecp_;
	public:
		ConfigCodecRfc2833(const ConfigCodecRfc2833 &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecRfc2833() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_rfc2833(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecRfc2833() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_rfc2833 not called\n");
				abort();
			};
		};
		ConfigCodecRfc2833 &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecRfc2833 &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
	};
public:
	ConfigCodecRfc2833 config_codec_rfc2833() {
		return ConfigCodecRfc2833().vmptx(vmptx_);
	}
private:
	class ConfigCodecRfc4040 {
		mutable int used_;
		SM_VMPTX_CODEC_RFC4040_PARMS codecp_;
	public:
		ConfigCodecRfc4040(const ConfigCodecRfc4040 &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecRfc4040() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_rfc4040(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecRfc4040() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_rfc4040 not called\n");
				abort();
			};
		};
		ConfigCodecRfc4040 &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecRfc4040 &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecRfc4040 &packetlen(tSM_INT v) {
			codecp_.packetlen = v;
			return *this;
		};
	};
public:
	ConfigCodecRfc4040 config_codec_rfc4040() {
		return ConfigCodecRfc4040().vmptx(vmptx_);
	}
private:
	class ConfigCodecSmv {
		mutable int used_;
		SM_VMPTX_CODEC_SMV_PARMS codecp_;
	public:
		ConfigCodecSmv(const ConfigCodecSmv &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecSmv() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_smv(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecSmv() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_smv not called\n");
				abort();
			};
		};
		ConfigCodecSmv &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecSmv &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecSmv &VADMode(enum kSMVMPTxVADMode v) {
			codecp_.VADMode = v;
			return *this;
		};
		ConfigCodecSmv &frames_per_packet(tSM_INT v) {
			codecp_.frames_per_packet = v;
			return *this;
		};
	};
public:
	ConfigCodecSmv config_codec_smv() {
		return ConfigCodecSmv().vmptx(vmptx_);
	}
private:
	class ConfigCodecTetra {
		mutable int used_;
		SM_VMPTX_CODEC_TETRA_PARMS codecp_;
	public:
		ConfigCodecTetra(const ConfigCodecTetra &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecTetra() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_tetra(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecTetra() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_tetra not called\n");
				abort();
			};
		};
		ConfigCodecTetra &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecTetra &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
	};
public:
	ConfigCodecTetra config_codec_tetra() {
		return ConfigCodecTetra().vmptx(vmptx_);
	}
private:
	class ConfigEncryptionAesCm {
		mutable int used_;
		SM_VMPTX_CONFIG_ENCRYPTION_AES_CM_PARMS pp_;
	public:
		ConfigEncryptionAesCm(const ConfigEncryptionAesCm &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigEncryptionAesCm() : used_(0) {
			memset(&pp_, 0, sizeof(pp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_encryption_aes_cm(&pp_));
			used_ = 1;
			return e;
		};
		~ConfigEncryptionAesCm() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_encryption_aes_cm not called\n");
				abort();
			};
		};
		ConfigEncryptionAesCm &vmptx(tSMVMPtxId v) {
			pp_.vmptx = v;
			return *this;
		};
		ConfigEncryptionAesCm &keylen(tSM_INT v) {
			pp_.keylen = v;
			return *this;
		};
		ConfigEncryptionAesCm &key(char* v) {
			pp_.key = v;
			return *this;
		};
	};
public:
	ConfigEncryptionAesCm config_encryption_aes_cm() {
		return ConfigEncryptionAesCm().vmptx(vmptx_);
	}
private:
	class ConfigEncryptionAesF8 {
		mutable int used_;
		SM_VMPTX_CONFIG_ENCRYPTION_AES_F8_PARMS pp_;
	public:
		ConfigEncryptionAesF8(const ConfigEncryptionAesF8 &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigEncryptionAesF8() : used_(0) {
			memset(&pp_, 0, sizeof(pp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_encryption_aes_f8(&pp_));
			used_ = 1;
			return e;
		};
		~ConfigEncryptionAesF8() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_encryption_aes_f8 not called\n");
				abort();
			};
		};
		ConfigEncryptionAesF8 &vmptx(tSMVMPtxId v) {
			pp_.vmptx = v;
			return *this;
		};
		ConfigEncryptionAesF8 &keylen(tSM_INT v) {
			pp_.keylen = v;
			return *this;
		};
		ConfigEncryptionAesF8 &key(char* v) {
			pp_.key = v;
			return *this;
		};
	};
public:
	ConfigEncryptionAesF8 config_encryption_aes_f8() {
		return ConfigEncryptionAesF8().vmptx(vmptx_);
	}
private:
	class ConfigEncryptionNull {
		mutable int used_;
		SM_VMPTX_CONFIG_ENCRYPTION_NULL_PARMS pp_;
	public:
		ConfigEncryptionNull(const ConfigEncryptionNull &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigEncryptionNull() : used_(0) {
			memset(&pp_, 0, sizeof(pp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_encryption_null(&pp_));
			used_ = 1;
			return e;
		};
		~ConfigEncryptionNull() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_encryption_null not called\n");
				abort();
			};
		};
		ConfigEncryptionNull &vmptx(tSMVMPtxId v) {
			pp_.vmptx = v;
			return *this;
		};
	};
public:
	ConfigEncryptionNull config_encryption_null() {
		return ConfigEncryptionNull().vmptx(vmptx_);
	}
private:
	class ConfigIsacRate {
		mutable int used_;
		SM_VMPTX_ISAC_RATE_PARMS codecp_;
	public:
		ConfigIsacRate(const ConfigIsacRate &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigIsacRate() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_isac_rate(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigIsacRate() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_isac_rate not called\n");
				abort();
			};
		};
		ConfigIsacRate &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigIsacRate &rate(tSM_INT v) {
			codecp_.rate = v;
			return *this;
		};
		ConfigIsacRate &ptime(tSM_INT v) {
			codecp_.ptime = v;
			return *this;
		};
	};
public:
	ConfigIsacRate config_isac_rate() {
		return ConfigIsacRate().vmptx(vmptx_);
	}
private:
	class ConfigProfileSpecific {
		mutable int used_;
		SM_VMPTX_CONFIG_PROFILE_SPECIFIC_PARMS psp_;
	public:
		ConfigProfileSpecific(const ConfigProfileSpecific &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigProfileSpecific() : used_(0) {
			memset(&psp_, 0, sizeof(psp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_profile_specific(&psp_));
			used_ = 1;
			return e;
		};
		~ConfigProfileSpecific() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_profile_specific not called\n");
				abort();
			};
		};
		ConfigProfileSpecific &vmptx(tSMVMPtxId v) {
			psp_.vmptx = v;
			return *this;
		};
		ConfigProfileSpecific &enable(tSM_INT v) {
			psp_.enable = v;
			return *this;
		};
		ConfigProfileSpecific &plugin(char* v) {
			psp_.plugin = v;
			return *this;
		};
		ConfigProfileSpecific &paramlen(tSM_INT v) {
			psp_.paramlen = v;
			return *this;
		};
		ConfigProfileSpecific &paramval(char* v) {
			psp_.paramval = v;
			return *this;
		};
	};
public:
	ConfigProfileSpecific config_profile_specific() {
		return ConfigProfileSpecific().vmptx(vmptx_);
	}
private:
	class ConfigSampleRate {
		mutable int used_;
		SM_VMPTX_SAMPLE_RATE_PARMS sample_ratep_;
	public:
		ConfigSampleRate(const ConfigSampleRate &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigSampleRate() : used_(0) {
			memset(&sample_ratep_, 0, sizeof(sample_ratep_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_sample_rate(&sample_ratep_));
			used_ = 1;
			return e;
		};
		~ConfigSampleRate() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_sample_rate not called\n");
				abort();
			};
		};
		ConfigSampleRate &vmptx(tSMVMPtxId v) {
			sample_ratep_.vmptx = v;
			return *this;
		};
		ConfigSampleRate &sample_rate(tSM_UT32 v) {
			sample_ratep_.sample_rate = v;
			return *this;
		};
	};
public:
	ConfigSampleRate config_sample_rate() {
		return ConfigSampleRate().vmptx(vmptx_);
	}
private:
	class ConfigTag {
		mutable int used_;
		SM_VMPTX_TAG_PARMS tagp_;
	public:
		ConfigTag(const ConfigTag &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigTag() : used_(0) {
			memset(&tagp_, 0, sizeof(tagp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_tag(&tagp_));
			used_ = 1;
			return e;
		};
		~ConfigTag() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_tag not called\n");
				abort();
			};
		};
		ConfigTag &vmptx(tSMVMPtxId v) {
			tagp_.vmptx = v;
			return *this;
		};
		ConfigTag &payload_type(tSM_INT v) {
			tagp_.payload_type = v;
			return *this;
		};
		ConfigTag &tag_type(enum kSMVMPTxTagType v) {
			tagp_.tag_type = v;
			return *this;
		};
		ConfigTag &min_time(tSM_UT32 v) {
			tagp_.min_time = v;
			return *this;
		};
		ConfigTag &max_time(tSM_UT32 v) {
			tagp_.max_time = v;
			return *this;
		};
	};
public:
	ConfigTag config_tag() {
		return ConfigTag().vmptx(vmptx_);
	}
private:
	class ConfigTones {
		mutable int used_;
		SM_VMPTX_TONE_PARMS tonep_;
	public:
		ConfigTones(const ConfigTones &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigTones() : used_(0) {
			memset(&tonep_, 0, sizeof(tonep_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_tones(&tonep_));
			used_ = 1;
			return e;
		};
		~ConfigTones() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_tones not called\n");
				abort();
			};
		};
		ConfigTones &vmptx(tSMVMPtxId v) {
			tonep_.vmptx = v;
			return *this;
		};
		ConfigTones &convert_tones(tSM_INT v) {
			tonep_.convert_tones = v;
			return *this;
		};
		ConfigTones &elim_tones(tSM_INT v) {
			tonep_.elim_tones = v;
			return *this;
		};
		ConfigTones &tone_set_id(tSMVMPTxToneSetId v) {
			tonep_.tone_set_id = v;
			return *this;
		};
		ConfigTones &min_duration(enum kSMVMPtxRFC2833MinDuration v) {
			tonep_.min_duration = v;
			return *this;
		};
	};
public:
	ConfigTones config_tones() {
		return ConfigTones().vmptx(vmptx_);
	}
private:
	class Create {
		mutable int used_;
		SM_VMPTX_CREATE_PARMS vmptxp_;
		tSMVMPtxId *vmptxP_;
	public:
		Create(const Create &old) {
			*this = old;
			old.used_ = 1;
		};
		Create() : used_(0) {
			memset(&vmptxp_, 0, sizeof(vmptxp_));
			vmptxP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_create(&vmptxp_));
			used_ = 1;
			if (vmptxP_) *vmptxP_ = vmptxp_.vmptx;
			return e;
		};
		~Create() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_create not called\n");
				abort();
			};
		};
		Create &vmptx(tSMVMPtxId *v) {
			vmptxP_ = v;
			return *this;
		};
		Create &module(tSMModuleId v) {
			vmptxp_.module = v;
			return *this;
		};
	};
public:
	Create create() {
		return Create().vmptx(&vmptx_);
	}
private:
	class DatafeedConnect {
		mutable int used_;
		SM_VMPTX_DATAFEED_CONNECT_PARMS datafeedp_;
	public:
		DatafeedConnect(const DatafeedConnect &old) {
			*this = old;
			old.used_ = 1;
		};
		DatafeedConnect() : used_(0) {
			memset(&datafeedp_, 0, sizeof(datafeedp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_datafeed_connect(&datafeedp_));
			used_ = 1;
			return e;
		};
		~DatafeedConnect() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_datafeed_connect not called\n");
				abort();
			};
		};
		DatafeedConnect &data_source(tSMDatafeedId v) {
			datafeedp_.data_source = v;
			return *this;
		};
		DatafeedConnect &vmptx(tSMVMPtxId v) {
			datafeedp_.vmptx = v;
			return *this;
		};
	};
public:
	DatafeedConnect datafeed_connect() {
		return DatafeedConnect().vmptx(vmptx_);
	}
private:
	class Destroy {
		mutable int used_;
		tSMVMPtxId vmptx_;
	public:
		Destroy(const Destroy &old) {
			*this = old;
			old.used_ = 1;
		};
		Destroy(tSMVMPtxId vmptx) : used_(0) {
			vmptx_ = vmptx;
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_destroy(vmptx_));
			used_ = 1;
			return e;
		};
		~Destroy() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_destroy not called\n");
				abort();
			};
		};
	};
public:
	Destroy destroy() {
		Destroy temp(vmptx_);
		
		vmptx_ = 0;
		return temp;
	}
private:
	class GenerateTones {
		mutable int used_;
		SM_VMPTX_GENERATE_TONES_PARMS tonep_;
	public:
		GenerateTones(const GenerateTones &old) {
			*this = old;
			old.used_ = 1;
		};
		GenerateTones() : used_(0) {
			memset(&tonep_, 0, sizeof(tonep_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_generate_tones(&tonep_));
			used_ = 1;
			return e;
		};
		~GenerateTones() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_generate_tones not called\n");
				abort();
			};
		};
		GenerateTones &vmptx(tSMVMPtxId v) {
			tonep_.vmptx = v;
			return *this;
		};
		GenerateTones &tones(tSM_INT* v) {
			tonep_.tones = v;
			return *this;
		};
		GenerateTones &num(tSM_INT v) {
			tonep_.num = v;
			return *this;
		};
		GenerateTones &duration(tSM_INT v) {
			tonep_.duration = v;
			return *this;
		};
		GenerateTones &interval(tSM_INT v) {
			tonep_.interval = v;
			return *this;
		};
		GenerateTones &volume(tSM_INT v) {
			tonep_.volume = v;
			return *this;
		};
	};
public:
	GenerateTones generate_tones() {
		return GenerateTones().vmptx(vmptx_);
	}
private:
	class GenerateTonesAbort {
		mutable int used_;
		SM_VMPTX_GENERATE_TONES_ABORT_PARMS tonep_;
	public:
		GenerateTonesAbort(const GenerateTonesAbort &old) {
			*this = old;
			old.used_ = 1;
		};
		GenerateTonesAbort() : used_(0) {
			memset(&tonep_, 0, sizeof(tonep_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_generate_tones_abort(&tonep_));
			used_ = 1;
			return e;
		};
		~GenerateTonesAbort() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_generate_tones_abort not called\n");
				abort();
			};
		};
		GenerateTonesAbort &vmptx(tSMVMPtxId v) {
			tonep_.vmptx = v;
			return *this;
		};
	};
public:
	GenerateTonesAbort generate_tones_abort() {
		return GenerateTonesAbort().vmptx(vmptx_);
	}
private:
	class GetEvent {
		mutable int used_;
		SM_VMPTX_EVENT_PARMS eventp_;
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
			Error e(Prosody::sm_vmptx_get_event(&eventp_));
			used_ = 1;
			if (eventP_) *eventP_ = eventp_.event;
			return e;
		};
		~GetEvent() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_get_event not called\n");
				abort();
			};
		};
		GetEvent &vmptx(tSMVMPtxId v) {
			eventp_.vmptx = v;
			return *this;
		};
		GetEvent &event(tSMEventId *v) {
			eventP_ = v;
			return *this;
		};
	};
public:
	GetEvent get_event() {
		return GetEvent().vmptx(vmptx_);
	}
private:
	class PropagateRtcpSrNtp {
		mutable int used_;
		SM_VMPTX_PROPAGATE_RTCP_SR_NTP_PARMS propp_;
	public:
		PropagateRtcpSrNtp(const PropagateRtcpSrNtp &old) {
			*this = old;
			old.used_ = 1;
		};
		PropagateRtcpSrNtp() : used_(0) {
			memset(&propp_, 0, sizeof(propp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_propagate_rtcp_sr_ntp(&propp_));
			used_ = 1;
			return e;
		};
		~PropagateRtcpSrNtp() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_propagate_rtcp_sr_ntp not called\n");
				abort();
			};
		};
		PropagateRtcpSrNtp &vmptx(tSMVMPtxId v) {
			propp_.vmptx = v;
			return *this;
		};
		PropagateRtcpSrNtp &localtimeref(tSM_UT64 v) {
			propp_.localtimeref = v;
			return *this;
		};
		PropagateRtcpSrNtp &ntp_timestamp(tSM_UT64 v) {
			propp_.ntp_timestamp = v;
			return *this;
		};
	};
public:
	PropagateRtcpSrNtp propagate_rtcp_sr_ntp() {
		return PropagateRtcpSrNtp().vmptx(vmptx_);
	}
private:
	class SetCsrc {
		mutable int used_;
		SM_VMPTX_SET_CSRC_PARMS csrcp_;
	public:
		SetCsrc(const SetCsrc &old) {
			*this = old;
			old.used_ = 1;
		};
		SetCsrc() : used_(0) {
			memset(&csrcp_, 0, sizeof(csrcp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_set_csrc(&csrcp_));
			used_ = 1;
			return e;
		};
		~SetCsrc() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_set_csrc not called\n");
				abort();
			};
		};
		SetCsrc &vmptx(tSMVMPtxId v) {
			csrcp_.vmptx = v;
			return *this;
		};
		SetCsrc &repeat_delay(unsigned v) {
			csrcp_.repeat_delay = v;
			return *this;
		};
		SetCsrc &packet_count(unsigned v) {
			csrcp_.packet_count = v;
			return *this;
		};
		SetCsrc &type(enum kSMVMPTxCSRCType v) {
			csrcp_.type = v;
			return *this;
		};
		SetCsrc &u_csrc_array_num_csrc(int v) {
			csrcp_.u.csrc_array.num_csrc = v;
			return *this;
		};
		SetCsrc &u_csrc_array_csrc(int* v) {
			csrcp_.u.csrc_array.csrc = v;
			return *this;
		};
		SetCsrc &u_csrc_list_csrc_list_id(tSMVMPTxCSRCListId v) {
			csrcp_.u.csrc_list.csrc_list_id = v;
			return *this;
		};
		SetCsrc &u_csrc_list_exclude_ssrc(int v) {
			csrcp_.u.csrc_list.exclude_ssrc = v;
			return *this;
		};
		SetCsrc &u_csrc_list_ssrc(int v) {
			csrcp_.u.csrc_list.ssrc = v;
			return *this;
		};
	};
public:
	SetCsrc set_csrc() {
		return SetCsrc().vmptx(vmptx_);
	}
private:
	class SetProfileSpecific {
		mutable int used_;
		SM_VMPTX_SET_PROFILE_SPECIFIC_PARMS psp_;
	public:
		SetProfileSpecific(const SetProfileSpecific &old) {
			*this = old;
			old.used_ = 1;
		};
		SetProfileSpecific() : used_(0) {
			memset(&psp_, 0, sizeof(psp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_set_profile_specific(&psp_));
			used_ = 1;
			return e;
		};
		~SetProfileSpecific() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_set_profile_specific not called\n");
				abort();
			};
		};
		SetProfileSpecific &vmptx(tSMVMPtxId v) {
			psp_.vmptx = v;
			return *this;
		};
		SetProfileSpecific &paramlen(tSM_INT v) {
			psp_.paramlen = v;
			return *this;
		};
		SetProfileSpecific &paramval(char* v) {
			psp_.paramval = v;
			return *this;
		};
	};
public:
	SetProfileSpecific set_profile_specific() {
		return SetProfileSpecific().vmptx(vmptx_);
	}
private:
	class SetRtcphand {
		mutable int used_;
		SM_VMPTX_SET_RTCPHAND_PARMS rvp_;
	public:
		SetRtcphand(const SetRtcphand &old) {
			*this = old;
			old.used_ = 1;
		};
		SetRtcphand() : used_(0) {
			memset(&rvp_, 0, sizeof(rvp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_set_rtcphand(&rvp_));
			used_ = 1;
			return e;
		};
		~SetRtcphand() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_set_rtcphand not called\n");
				abort();
			};
		};
		SetRtcphand &vmptx(tSMVMPtxId v) {
			rvp_.vmptx = v;
			return *this;
		};
		SetRtcphand &rtcphand(tSMRTCPHandId v) {
			rvp_.rtcphand = v;
			return *this;
		};
	};
public:
	SetRtcphand set_rtcphand() {
		return SetRtcphand().vmptx(vmptx_);
	}
private:
	class Status {
		mutable int used_;
		SM_VMPTX_STATUS_PARMS statusp_;
		enum kSMVMPtxStatus *statusP_;
		int *u_ssrc_ssrcP_;
	public:
		Status(const Status &old) {
			*this = old;
			old.used_ = 1;
		};
		Status() : used_(0) {
			memset(&statusp_, 0, sizeof(statusp_));
			statusP_ = 0;
			u_ssrc_ssrcP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_status(&statusp_));
			used_ = 1;
			if (statusP_) *statusP_ = statusp_.status;
			if (u_ssrc_ssrcP_) *u_ssrc_ssrcP_ = statusp_.u.ssrc.ssrc;
			return e;
		};
		~Status() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_status not called\n");
				abort();
			};
		};
		Status &vmptx(tSMVMPtxId v) {
			statusp_.vmptx = v;
			return *this;
		};
		Status &status(enum kSMVMPtxStatus *v) {
			statusP_ = v;
			return *this;
		};
		Status &u_ssrc_ssrc(int *v) {
			u_ssrc_ssrcP_ = v;
			return *this;
		};
	};
public:
	Status status() {
		return Status().vmptx(vmptx_);
	}
private:
	class Stop {
		mutable int used_;
		SM_VMPTX_STOP_PARMS stopp_;
	public:
		Stop(const Stop &old) {
			*this = old;
			old.used_ = 1;
		};
		Stop() : used_(0) {
			memset(&stopp_, 0, sizeof(stopp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_stop(&stopp_));
			used_ = 1;
			return e;
		};
		~Stop() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_stop not called\n");
				abort();
			};
		};
		Stop &vmptx(tSMVMPtxId v) {
			stopp_.vmptx = v;
			return *this;
		};
	};
public:
	Stop stop() {
		return Stop().vmptx(vmptx_);
	}
private:
	class ConfigCodecSse {
		mutable int used_;
		SM_VMPTX_CODEC_SSE_PARMS codecp_;
	public:
		ConfigCodecSse(const ConfigCodecSse &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecSse() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_sse(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecSse() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_sse not called\n");
				abort();
			};
		};
		ConfigCodecSse &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecSse &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecSse &retransmissions(tSM_INT v) {
			codecp_.retransmissions = v;
			return *this;
		};
		ConfigCodecSse &delay(tSM_INT v) {
			codecp_.delay = v;
			return *this;
		};
	};
public:
	ConfigCodecSse config_codec_sse() {
		return ConfigCodecSse().vmptx(vmptx_);
	}
private:
	class SendSse {
		mutable int used_;
		SM_VMPTX_SEND_SSE_PARMS sendssep_;
	public:
		SendSse(const SendSse &old) {
			*this = old;
			old.used_ = 1;
		};
		SendSse() : used_(0) {
			memset(&sendssep_, 0, sizeof(sendssep_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_send_sse(&sendssep_));
			used_ = 1;
			return e;
		};
		~SendSse() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_send_sse not called\n");
				abort();
			};
		};
		SendSse &vmptx(tSMVMPtxId v) {
			sendssep_.vmptx = v;
			return *this;
		};
		SendSse &payload(char* v) {
			sendssep_.payload = v;
			return *this;
		};
		SendSse &payload_length(tSM_INT v) {
			sendssep_.payload_length = v;
			return *this;
		};
	};
public:
	SendSse send_sse() {
		return SendSse().vmptx(vmptx_);
	}
private:
	class ConfigCodecRttext {
		mutable int used_;
		SM_VMPTX_CODEC_RTTEXT_PARMS codecp_;
	public:
		ConfigCodecRttext(const ConfigCodecRttext &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecRttext() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec_rttext(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecRttext() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec_rttext not called\n");
				abort();
			};
		};
		ConfigCodecRttext &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodecRttext &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecRttext &redundant_payload_type(tSM_INT v) {
			codecp_.redundant_payload_type = v;
			return *this;
		};
		ConfigCodecRttext &redundancy_level(tSM_INT v) {
			codecp_.redundancy_level = v;
			return *this;
		};
		ConfigCodecRttext &buffer_time(tSM_INT v) {
			codecp_.buffer_time = v;
			return *this;
		};
		ConfigCodecRttext &idle_time(tSM_INT v) {
			codecp_.idle_time = v;
			return *this;
		};
	};
public:
	ConfigCodecRttext config_codec_rttext() {
		return ConfigCodecRttext().vmptx(vmptx_);
	}
private:
	class GenerateJitter {
		mutable int used_;
		SM_VMPTX_GENERATE_JITTER_PARMS jitterp_;
	public:
		GenerateJitter(const GenerateJitter &old) {
			*this = old;
			old.used_ = 1;
		};
		GenerateJitter() : used_(0) {
			memset(&jitterp_, 0, sizeof(jitterp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_generate_jitter(&jitterp_));
			used_ = 1;
			return e;
		};
		~GenerateJitter() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_generate_jitter not called\n");
				abort();
			};
		};
		GenerateJitter &vmptx(tSMVMPtxId v) {
			jitterp_.vmptx = v;
			return *this;
		};
		GenerateJitter &external_jitter_threshold(tSM_INT v) {
			jitterp_.external_jitter_threshold = v;
			return *this;
		};
		GenerateJitter &inter_jitter_gap(tSM_INT v) {
			jitterp_.inter_jitter_gap = v;
			return *this;
		};
		GenerateJitter &added_jitter(tSM_INT v) {
			jitterp_.added_jitter = v;
			return *this;
		};
		GenerateJitter &max_packet_age(tSM_INT v) {
			jitterp_.max_packet_age = v;
			return *this;
		};
	};
public:
	GenerateJitter generate_jitter() {
		return GenerateJitter().vmptx(vmptx_);
	}
private:
	class ConfigCodec {
		mutable int used_;
		SM_VMPTX_CODEC_PARMS codecp_;
	public:
		ConfigCodec(const ConfigCodec &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodec() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmptx_config_codec(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodec() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmptx_config_codec not called\n");
				abort();
			};
		};
		ConfigCodec &vmptx(tSMVMPtxId v) {
			codecp_.vmptx = v;
			return *this;
		};
		ConfigCodec &codec(enum kSMCodecType v) {
			codecp_.codec = v;
			return *this;
		};
		ConfigCodec &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodec &VADMode(enum kSMVMPTxVADMode v) {
			codecp_.VADMode = v;
			return *this;
		};
		ConfigCodec &ptime(tSM_INT v) {
			codecp_.ptime = v;
			return *this;
		};
	};
public:
	ConfigCodec config_codec() {
		return ConfigCodec().vmptx(vmptx_);
	}
