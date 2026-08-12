private:
	class VidmprxSyncMedia {
		mutable int used_;
		SM_VIDMPRX_SYNC_MEDIA_PARMS syncmediap_;
	public:
		VidmprxSyncMedia(const VidmprxSyncMedia &old) {
			*this = old;
			old.used_ = 1;
		};
		VidmprxSyncMedia() : used_(0) {
			memset(&syncmediap_, 0, sizeof(syncmediap_));
		};
		operator Error() {
			Error e(Prosody::sm_vidmprx_sync_media(&syncmediap_));
			used_ = 1;
			return e;
		};
		~VidmprxSyncMedia() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vidmprx_sync_media not called\n");
				abort();
			};
		};
		VidmprxSyncMedia &vidmprx(tSMVidMPrxId v) {
			syncmediap_.vidmprx = v;
			return *this;
		};
		VidmprxSyncMedia &vmprx(tSMVMPrxId v) {
			syncmediap_.vmprx = v;
			return *this;
		};
	};
public:
	VidmprxSyncMedia vidmprx_sync_media() {
		return VidmprxSyncMedia().vmprx(vmprx_);
	}
private:
	class ConfigAuthenticationHmacSha1 {
		mutable int used_;
		SM_VMPRX_CONFIG_AUTHENTICATION_HMAC_SHA1_PARMS pp_;
	public:
		ConfigAuthenticationHmacSha1(const ConfigAuthenticationHmacSha1 &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigAuthenticationHmacSha1() : used_(0) {
			memset(&pp_, 0, sizeof(pp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_authentication_hmac_sha1(&pp_));
			used_ = 1;
			return e;
		};
		~ConfigAuthenticationHmacSha1() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_authentication_hmac_sha1 not called\n");
				abort();
			};
		};
		ConfigAuthenticationHmacSha1 &vmprx(tSMVMPrxId v) {
			pp_.vmprx = v;
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
		return ConfigAuthenticationHmacSha1().vmprx(vmprx_);
	}
private:
	class ConfigAuthenticationNull {
		mutable int used_;
		SM_VMPRX_CONFIG_AUTHENTICATION_NULL_PARMS pp_;
	public:
		ConfigAuthenticationNull(const ConfigAuthenticationNull &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigAuthenticationNull() : used_(0) {
			memset(&pp_, 0, sizeof(pp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_authentication_null(&pp_));
			used_ = 1;
			return e;
		};
		~ConfigAuthenticationNull() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_authentication_null not called\n");
				abort();
			};
		};
		ConfigAuthenticationNull &vmprx(tSMVMPrxId v) {
			pp_.vmprx = v;
			return *this;
		};
	};
public:
	ConfigAuthenticationNull config_authentication_null() {
		return ConfigAuthenticationNull().vmprx(vmprx_);
	}
private:
	class ConfigCodecAlaw {
		mutable int used_;
		SM_VMPRX_CODEC_ALAW_PARMS codecp_;
	public:
		ConfigCodecAlaw(const ConfigCodecAlaw &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecAlaw() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_alaw(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecAlaw() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_alaw not called\n");
				abort();
			};
		};
		ConfigCodecAlaw &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
			return *this;
		};
		ConfigCodecAlaw &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecAlaw &plc_mode(enum kSMPLCMode v) {
			codecp_.plc_mode = v;
			return *this;
		};
	};
public:
	ConfigCodecAlaw config_codec_alaw() {
		return ConfigCodecAlaw().vmprx(vmprx_);
	}
private:
	class ConfigCodecAmrnb {
		mutable int used_;
		SM_VMPRX_CODEC_AMRNB_PARMS codecp_;
	public:
		ConfigCodecAmrnb(const ConfigCodecAmrnb &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecAmrnb() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_amrnb(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecAmrnb() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_amrnb not called\n");
				abort();
			};
		};
		ConfigCodecAmrnb &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
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
		ConfigCodecAmrnb &plc_mode(enum kSMPLCMode v) {
			codecp_.plc_mode = v;
			return *this;
		};
		ConfigCodecAmrnb &report_cmr(tSM_INT v) {
			codecp_.report_cmr = v;
			return *this;
		};
	};
public:
	ConfigCodecAmrnb config_codec_amrnb() {
		return ConfigCodecAmrnb().vmprx(vmprx_);
	}
private:
	class ConfigCodecG7221 {
		mutable int used_;
		SM_VMPRX_CODEC_G722_1_PARMS codecp_;
	public:
		ConfigCodecG7221(const ConfigCodecG7221 &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecG7221() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_g722_1(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecG7221() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_g722_1 not called\n");
				abort();
			};
		};
		ConfigCodecG7221 &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
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
		ConfigCodecG7221 &plc_mode(enum kSMPLCMode v) {
			codecp_.plc_mode = v;
			return *this;
		};
	};
public:
	ConfigCodecG7221 config_codec_g722_1() {
		return ConfigCodecG7221().vmprx(vmprx_);
	}
private:
	class ConfigCodecSilk {
		mutable int used_;
		SM_VMPRX_CODEC_SILK_PARMS codecp_;
	public:
		ConfigCodecSilk(const ConfigCodecSilk &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecSilk() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_silk(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecSilk() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_silk not called\n");
				abort();
			};
		};
		ConfigCodecSilk &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
			return *this;
		};
		ConfigCodecSilk &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecSilk &plc_mode(enum kSMPLCMode v) {
			codecp_.plc_mode = v;
			return *this;
		};
	};
public:
	ConfigCodecSilk config_codec_silk() {
		return ConfigCodecSilk().vmprx(vmprx_);
	}
private:
	class ConfigCodecAmrwb {
		mutable int used_;
		SM_VMPRX_CODEC_AMRWB_PARMS codecp_;
	public:
		ConfigCodecAmrwb(const ConfigCodecAmrwb &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecAmrwb() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_amrwb(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecAmrwb() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_amrwb not called\n");
				abort();
			};
		};
		ConfigCodecAmrwb &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
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
		ConfigCodecAmrwb &plc_mode(enum kSMPLCMode v) {
			codecp_.plc_mode = v;
			return *this;
		};
		ConfigCodecAmrwb &report_cmr(tSM_INT v) {
			codecp_.report_cmr = v;
			return *this;
		};
	};
public:
	ConfigCodecAmrwb config_codec_amrwb() {
		return ConfigCodecAmrwb().vmprx(vmprx_);
	}
private:
	class ConfigCodecComfortNoise {
		mutable int used_;
		SM_VMPRX_CODEC_COMFORT_NOISE_PARMS codecp_;
	public:
		ConfigCodecComfortNoise(const ConfigCodecComfortNoise &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecComfortNoise() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_comfort_noise(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecComfortNoise() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_comfort_noise not called\n");
				abort();
			};
		};
		ConfigCodecComfortNoise &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
			return *this;
		};
		ConfigCodecComfortNoise &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecComfortNoise &plc_mode(enum kSMPLCMode v) {
			codecp_.plc_mode = v;
			return *this;
		};
	};
public:
	ConfigCodecComfortNoise config_codec_comfort_noise() {
		return ConfigCodecComfortNoise().vmprx(vmprx_);
	}
private:
	class ConfigCodecEvrc {
		mutable int used_;
		SM_VMPRX_CODEC_EVRC_PARMS codecp_;
	public:
		ConfigCodecEvrc(const ConfigCodecEvrc &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecEvrc() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_evrc(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecEvrc() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_evrc not called\n");
				abort();
			};
		};
		ConfigCodecEvrc &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
			return *this;
		};
		ConfigCodecEvrc &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecEvrc &post_filter(tSM_INT v) {
			codecp_.post_filter = v;
			return *this;
		};
		ConfigCodecEvrc &rtp_mode(enum kSMEVRCRTPMode v) {
			codecp_.rtp_mode = v;
			return *this;
		};
		ConfigCodecEvrc &plc_mode(enum kSMPLCMode v) {
			codecp_.plc_mode = v;
			return *this;
		};
	};
public:
	ConfigCodecEvrc config_codec_evrc() {
		return ConfigCodecEvrc().vmprx(vmprx_);
	}
private:
	class ConfigCodecG7231 {
		mutable int used_;
		SM_VMPRX_CODEC_G723_1_PARMS codecp_;
	public:
		ConfigCodecG7231(const ConfigCodecG7231 &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecG7231() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_g723_1(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecG7231() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_g723_1 not called\n");
				abort();
			};
		};
		ConfigCodecG7231 &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
			return *this;
		};
		ConfigCodecG7231 &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecG7231 &post_filter(tSM_INT v) {
			codecp_.post_filter = v;
			return *this;
		};
		ConfigCodecG7231 &plc_mode(enum kSMPLCMode v) {
			codecp_.plc_mode = v;
			return *this;
		};
	};
public:
	ConfigCodecG7231 config_codec_g723_1() {
		return ConfigCodecG7231().vmprx(vmprx_);
	}
private:
	class ConfigCodecG722 {
		mutable int used_;
		SM_VMPRX_CODEC_G722_PARMS codecp_;
	public:
		ConfigCodecG722(const ConfigCodecG722 &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecG722() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_g722(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecG722() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_g722 not called\n");
				abort();
			};
		};
		ConfigCodecG722 &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
			return *this;
		};
		ConfigCodecG722 &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecG722 &plc_mode(enum kSMPLCMode v) {
			codecp_.plc_mode = v;
			return *this;
		};
	};
public:
	ConfigCodecG722 config_codec_g722() {
		return ConfigCodecG722().vmprx(vmprx_);
	}
private:
	class ConfigCodecG726 {
		mutable int used_;
		SM_VMPRX_CODEC_G726_PARMS codecp_;
	public:
		ConfigCodecG726(const ConfigCodecG726 &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecG726() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_g726(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecG726() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_g726 not called\n");
				abort();
			};
		};
		ConfigCodecG726 &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
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
		ConfigCodecG726 &plc_mode(enum kSMPLCMode v) {
			codecp_.plc_mode = v;
			return *this;
		};
		ConfigCodecG726 &variant(enum kSMG726Variant v) {
			codecp_.variant = v;
			return *this;
		};
	};
public:
	ConfigCodecG726 config_codec_g726() {
		return ConfigCodecG726().vmprx(vmprx_);
	}
private:
	class ConfigCodecG728 {
		mutable int used_;
		SM_VMPRX_CODEC_G728_PARMS codecp_;
	public:
		ConfigCodecG728(const ConfigCodecG728 &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecG728() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_g728(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecG728() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_g728 not called\n");
				abort();
			};
		};
		ConfigCodecG728 &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
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
		ConfigCodecG728 &post_filter(tSM_INT v) {
			codecp_.post_filter = v;
			return *this;
		};
		ConfigCodecG728 &plc_mode(enum kSMPLCMode v) {
			codecp_.plc_mode = v;
			return *this;
		};
	};
public:
	ConfigCodecG728 config_codec_g728() {
		return ConfigCodecG728().vmprx(vmprx_);
	}
private:
	class ConfigCodecG729ab {
		mutable int used_;
		SM_VMPRX_CODEC_G729AB_PARMS codecp_;
	public:
		ConfigCodecG729ab(const ConfigCodecG729ab &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecG729ab() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_g729ab(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecG729ab() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_g729ab not called\n");
				abort();
			};
		};
		ConfigCodecG729ab &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
			return *this;
		};
		ConfigCodecG729ab &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecG729ab &plc_mode(enum kSMPLCMode v) {
			codecp_.plc_mode = v;
			return *this;
		};
	};
public:
	ConfigCodecG729ab config_codec_g729ab() {
		return ConfigCodecG729ab().vmprx(vmprx_);
	}
private:
	class ConfigCodecG729i {
		mutable int used_;
		SM_VMPRX_CODEC_G729I_PARMS codecp_;
	public:
		ConfigCodecG729i(const ConfigCodecG729i &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecG729i() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_g729i(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecG729i() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_g729i not called\n");
				abort();
			};
		};
		ConfigCodecG729i &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
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
	};
public:
	ConfigCodecG729i config_codec_g729i() {
		return ConfigCodecG729i().vmprx(vmprx_);
	}
private:
	class ConfigCodecGsmefr {
		mutable int used_;
		SM_VMPRX_CODEC_GSMEFR_PARMS codecp_;
	public:
		ConfigCodecGsmefr(const ConfigCodecGsmefr &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecGsmefr() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_gsmefr(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecGsmefr() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_gsmefr not called\n");
				abort();
			};
		};
		ConfigCodecGsmefr &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
			return *this;
		};
		ConfigCodecGsmefr &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecGsmefr &plc_mode(enum kSMPLCMode v) {
			codecp_.plc_mode = v;
			return *this;
		};
	};
public:
	ConfigCodecGsmefr config_codec_gsmefr() {
		return ConfigCodecGsmefr().vmprx(vmprx_);
	}
private:
	class ConfigCodecGsmfr {
		mutable int used_;
		SM_VMPRX_CODEC_GSMFR_PARMS codecp_;
	public:
		ConfigCodecGsmfr(const ConfigCodecGsmfr &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecGsmfr() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_gsmfr(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecGsmfr() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_gsmfr not called\n");
				abort();
			};
		};
		ConfigCodecGsmfr &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
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
		ConfigCodecGsmfr &plc_mode(enum kSMPLCMode v) {
			codecp_.plc_mode = v;
			return *this;
		};
	};
public:
	ConfigCodecGsmfr config_codec_gsmfr() {
		return ConfigCodecGsmfr().vmprx(vmprx_);
	}
private:
	class ConfigCodecGsmhr {
		mutable int used_;
		SM_VMPRX_CODEC_GSMHR_PARMS codecp_;
	public:
		ConfigCodecGsmhr(const ConfigCodecGsmhr &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecGsmhr() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_gsmhr(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecGsmhr() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_gsmhr not called\n");
				abort();
			};
		};
		ConfigCodecGsmhr &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
			return *this;
		};
		ConfigCodecGsmhr &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecGsmhr &plc_mode(enum kSMPLCMode v) {
			codecp_.plc_mode = v;
			return *this;
		};
	};
public:
	ConfigCodecGsmhr config_codec_gsmhr() {
		return ConfigCodecGsmhr().vmprx(vmprx_);
	}
private:
	class ConfigCodecIlbc {
		mutable int used_;
		SM_VMPRX_CODEC_ILBC_PARMS codecp_;
	public:
		ConfigCodecIlbc(const ConfigCodecIlbc &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecIlbc() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_ilbc(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecIlbc() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_ilbc not called\n");
				abort();
			};
		};
		ConfigCodecIlbc &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
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
		ConfigCodecIlbc &enhancer(tSM_INT v) {
			codecp_.enhancer = v;
			return *this;
		};
		ConfigCodecIlbc &plc_mode(enum kSMPLCMode v) {
			codecp_.plc_mode = v;
			return *this;
		};
	};
public:
	ConfigCodecIlbc config_codec_ilbc() {
		return ConfigCodecIlbc().vmprx(vmprx_);
	}
private:
	class ConfigCodecIsac {
		mutable int used_;
		SM_VMPRX_CODEC_ISAC_PARMS codecp_;
	public:
		ConfigCodecIsac(const ConfigCodecIsac &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecIsac() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_isac(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecIsac() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_isac not called\n");
				abort();
			};
		};
		ConfigCodecIsac &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
			return *this;
		};
		ConfigCodecIsac &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
	};
public:
	ConfigCodecIsac config_codec_isac() {
		return ConfigCodecIsac().vmprx(vmprx_);
	}
private:
	class ConfigCodecL16 {
		mutable int used_;
		SM_VMPRX_CODEC_L16_PARMS codecp_;
	public:
		ConfigCodecL16(const ConfigCodecL16 &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecL16() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_l16(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecL16() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_l16 not called\n");
				abort();
			};
		};
		ConfigCodecL16 &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
			return *this;
		};
		ConfigCodecL16 &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecL16 &plc_mode(enum kSMPLCMode v) {
			codecp_.plc_mode = v;
			return *this;
		};
	};
public:
	ConfigCodecL16 config_codec_l16() {
		return ConfigCodecL16().vmprx(vmprx_);
	}
private:
	class ConfigCodecL8 {
		mutable int used_;
		SM_VMPRX_CODEC_L8_PARMS codecp_;
	public:
		ConfigCodecL8(const ConfigCodecL8 &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecL8() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_l8(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecL8() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_l8 not called\n");
				abort();
			};
		};
		ConfigCodecL8 &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
			return *this;
		};
		ConfigCodecL8 &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecL8 &plc_mode(enum kSMPLCMode v) {
			codecp_.plc_mode = v;
			return *this;
		};
	};
public:
	ConfigCodecL8 config_codec_l8() {
		return ConfigCodecL8().vmprx(vmprx_);
	}
private:
	class ConfigCodecMelpe {
		mutable int used_;
		SM_VMPRX_CODEC_MELPE_PARMS codecp_;
	public:
		ConfigCodecMelpe(const ConfigCodecMelpe &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecMelpe() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_melpe(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecMelpe() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_melpe not called\n");
				abort();
			};
		};
		ConfigCodecMelpe &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
			return *this;
		};
		ConfigCodecMelpe &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
	};
public:
	ConfigCodecMelpe config_codec_melpe() {
		return ConfigCodecMelpe().vmprx(vmprx_);
	}
private:
	class ConfigCodecMulaw {
		mutable int used_;
		SM_VMPRX_CODEC_MULAW_PARMS codecp_;
	public:
		ConfigCodecMulaw(const ConfigCodecMulaw &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecMulaw() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_mulaw(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecMulaw() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_mulaw not called\n");
				abort();
			};
		};
		ConfigCodecMulaw &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
			return *this;
		};
		ConfigCodecMulaw &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecMulaw &plc_mode(enum kSMPLCMode v) {
			codecp_.plc_mode = v;
			return *this;
		};
	};
public:
	ConfigCodecMulaw config_codec_mulaw() {
		return ConfigCodecMulaw().vmprx(vmprx_);
	}
private:
	class ConfigCodecOpus {
		mutable int used_;
		SM_VMPRX_CODEC_OPUS_PARMS codecp_;
	public:
		ConfigCodecOpus(const ConfigCodecOpus &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecOpus() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_opus(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecOpus() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_opus not called\n");
				abort();
			};
		};
		ConfigCodecOpus &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
			return *this;
		};
		ConfigCodecOpus &decode_fec(tSM_INT v) {
			codecp_.decode_fec = v;
			return *this;
		};
		ConfigCodecOpus &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
	};
public:
	ConfigCodecOpus config_codec_opus() {
		return ConfigCodecOpus().vmprx(vmprx_);
	}
private:
	class ConfigCodecRfc2833 {
		mutable int used_;
		SM_VMPRX_CODEC_RFC2833_PARMS codecp_;
	public:
		ConfigCodecRfc2833(const ConfigCodecRfc2833 &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecRfc2833() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_rfc2833(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecRfc2833() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_rfc2833 not called\n");
				abort();
			};
		};
		ConfigCodecRfc2833 &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
			return *this;
		};
		ConfigCodecRfc2833 &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecRfc2833 &plc_mode(enum kSMPLCMode v) {
			codecp_.plc_mode = v;
			return *this;
		};
	};
public:
	ConfigCodecRfc2833 config_codec_rfc2833() {
		return ConfigCodecRfc2833().vmprx(vmprx_);
	}
private:
	class ConfigCodecRfc4040 {
		mutable int used_;
		SM_VMPRX_CODEC_RFC4040_PARMS codecp_;
	public:
		ConfigCodecRfc4040(const ConfigCodecRfc4040 &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecRfc4040() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_rfc4040(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecRfc4040() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_rfc4040 not called\n");
				abort();
			};
		};
		ConfigCodecRfc4040 &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
			return *this;
		};
		ConfigCodecRfc4040 &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
	};
public:
	ConfigCodecRfc4040 config_codec_rfc4040() {
		return ConfigCodecRfc4040().vmprx(vmprx_);
	}
private:
	class ConfigCodecSmv {
		mutable int used_;
		SM_VMPRX_CODEC_SMV_PARMS codecp_;
	public:
		ConfigCodecSmv(const ConfigCodecSmv &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecSmv() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_smv(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecSmv() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_smv not called\n");
				abort();
			};
		};
		ConfigCodecSmv &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
			return *this;
		};
		ConfigCodecSmv &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecSmv &plc_mode(enum kSMPLCMode v) {
			codecp_.plc_mode = v;
			return *this;
		};
	};
public:
	ConfigCodecSmv config_codec_smv() {
		return ConfigCodecSmv().vmprx(vmprx_);
	}
private:
	class ConfigCodecSpeex {
		mutable int used_;
		SM_VMPRX_CODEC_SPEEX_PARMS codecp_;
	public:
		ConfigCodecSpeex(const ConfigCodecSpeex &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecSpeex() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_speex(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecSpeex() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_speex not called\n");
				abort();
			};
		};
		ConfigCodecSpeex &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
			return *this;
		};
		ConfigCodecSpeex &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
		ConfigCodecSpeex &nb_channels(tSM_UT32 v) {
			codecp_.nb_channels = v;
			return *this;
		};
		ConfigCodecSpeex &sampling_rate(tSM_UT32 v) {
			codecp_.sampling_rate = v;
			return *this;
		};
		ConfigCodecSpeex &bandwidth(enum kSMSpeexMode v) {
			codecp_.bandwidth = v;
			return *this;
		};
		ConfigCodecSpeex &plc_mode(enum kSMPLCMode v) {
			codecp_.plc_mode = v;
			return *this;
		};
	};
public:
	ConfigCodecSpeex config_codec_speex() {
		return ConfigCodecSpeex().vmprx(vmprx_);
	}
private:
	class ConfigCodecSpeexMode {
		mutable int used_;
		SM_VMPRX_CODEC_SPEEX_MODE_PARMS codecp_;
	public:
		ConfigCodecSpeexMode(const ConfigCodecSpeexMode &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecSpeexMode() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_speex_mode(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecSpeexMode() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_speex_mode not called\n");
				abort();
			};
		};
		ConfigCodecSpeexMode &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
			return *this;
		};
		ConfigCodecSpeexMode &enh(tSM_UT32 v) {
			codecp_.enh = v;
			return *this;
		};
	};
public:
	ConfigCodecSpeexMode config_codec_speex_mode() {
		return ConfigCodecSpeexMode().vmprx(vmprx_);
	}
private:
	class ConfigCodecTetra {
		mutable int used_;
		SM_VMPRX_CODEC_TETRA_PARMS codecp_;
	public:
		ConfigCodecTetra(const ConfigCodecTetra &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecTetra() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_tetra(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecTetra() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_tetra not called\n");
				abort();
			};
		};
		ConfigCodecTetra &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
			return *this;
		};
		ConfigCodecTetra &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
	};
public:
	ConfigCodecTetra config_codec_tetra() {
		return ConfigCodecTetra().vmprx(vmprx_);
	}
private:
	class ConfigDataloss {
		mutable int used_;
		SM_VMPRX_DATALOSS_PARMS datalossp_;
	public:
		ConfigDataloss(const ConfigDataloss &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigDataloss() : used_(0) {
			memset(&datalossp_, 0, sizeof(datalossp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_dataloss(&datalossp_));
			used_ = 1;
			return e;
		};
		~ConfigDataloss() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_dataloss not called\n");
				abort();
			};
		};
		ConfigDataloss &vmprx(tSMVMPrxId v) {
			datalossp_.vmprx = v;
			return *this;
		};
		ConfigDataloss &loss_threshold(tSM_INT v) {
			datalossp_.loss_threshold = v;
			return *this;
		};
	};
public:
	ConfigDataloss config_dataloss() {
		return ConfigDataloss().vmprx(vmprx_);
	}
private:
	class ConfigEncryptionAesCm {
		mutable int used_;
		SM_VMPRX_CONFIG_ENCRYPTION_AES_CM_PARMS pp_;
	public:
		ConfigEncryptionAesCm(const ConfigEncryptionAesCm &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigEncryptionAesCm() : used_(0) {
			memset(&pp_, 0, sizeof(pp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_encryption_aes_cm(&pp_));
			used_ = 1;
			return e;
		};
		~ConfigEncryptionAesCm() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_encryption_aes_cm not called\n");
				abort();
			};
		};
		ConfigEncryptionAesCm &vmprx(tSMVMPrxId v) {
			pp_.vmprx = v;
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
		return ConfigEncryptionAesCm().vmprx(vmprx_);
	}
private:
	class ConfigEncryptionAesF8 {
		mutable int used_;
		SM_VMPRX_CONFIG_ENCRYPTION_AES_F8_PARMS pp_;
	public:
		ConfigEncryptionAesF8(const ConfigEncryptionAesF8 &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigEncryptionAesF8() : used_(0) {
			memset(&pp_, 0, sizeof(pp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_encryption_aes_f8(&pp_));
			used_ = 1;
			return e;
		};
		~ConfigEncryptionAesF8() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_encryption_aes_f8 not called\n");
				abort();
			};
		};
		ConfigEncryptionAesF8 &vmprx(tSMVMPrxId v) {
			pp_.vmprx = v;
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
		return ConfigEncryptionAesF8().vmprx(vmprx_);
	}
private:
	class ConfigEncryptionNull {
		mutable int used_;
		SM_VMPRX_CONFIG_ENCRYPTION_NULL_PARMS pp_;
	public:
		ConfigEncryptionNull(const ConfigEncryptionNull &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigEncryptionNull() : used_(0) {
			memset(&pp_, 0, sizeof(pp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_encryption_null(&pp_));
			used_ = 1;
			return e;
		};
		~ConfigEncryptionNull() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_encryption_null not called\n");
				abort();
			};
		};
		ConfigEncryptionNull &vmprx(tSMVMPrxId v) {
			pp_.vmprx = v;
			return *this;
		};
	};
public:
	ConfigEncryptionNull config_encryption_null() {
		return ConfigEncryptionNull().vmprx(vmprx_);
	}
private:
	class ConfigProfileSpecific {
		mutable int used_;
		SM_VMPRX_CONFIG_PROFILE_SPECIFIC_PARMS psp_;
	public:
		ConfigProfileSpecific(const ConfigProfileSpecific &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigProfileSpecific() : used_(0) {
			memset(&psp_, 0, sizeof(psp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_profile_specific(&psp_));
			used_ = 1;
			return e;
		};
		~ConfigProfileSpecific() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_profile_specific not called\n");
				abort();
			};
		};
		ConfigProfileSpecific &vmprx(tSMVMPrxId v) {
			psp_.vmprx = v;
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
		return ConfigProfileSpecific().vmprx(vmprx_);
	}
private:
	class ConfigJitter {
		mutable int used_;
		SM_VMPRX_JITTER_PARMS jitterp_;
	public:
		ConfigJitter(const ConfigJitter &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigJitter() : used_(0) {
			memset(&jitterp_, 0, sizeof(jitterp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_jitter(&jitterp_));
			used_ = 1;
			return e;
		};
		~ConfigJitter() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_jitter not called\n");
				abort();
			};
		};
		ConfigJitter &vmprx(tSMVMPrxId v) {
			jitterp_.vmprx = v;
			return *this;
		};
		ConfigJitter &max_delay_ms(tSM_UT32 v) {
			jitterp_.max_delay_ms = v;
			return *this;
		};
		ConfigJitter &initial_delay_ms(tSM_UT32 v) {
			jitterp_.initial_delay_ms = v;
			return *this;
		};
	};
public:
	ConfigJitter config_jitter() {
		return ConfigJitter().vmprx(vmprx_);
	}
private:
	class ConfigJitterMode {
		mutable int used_;
		SM_VMPRX_CONFIG_JITTER_MODE_PARMS jitterp_;
	public:
		ConfigJitterMode(const ConfigJitterMode &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigJitterMode() : used_(0) {
			memset(&jitterp_, 0, sizeof(jitterp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_jitter_mode(&jitterp_));
			used_ = 1;
			return e;
		};
		~ConfigJitterMode() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_jitter_mode not called\n");
				abort();
			};
		};
		ConfigJitterMode &vmprx(tSMVMPrxId v) {
			jitterp_.vmprx = v;
			return *this;
		};
		ConfigJitterMode &mode(enum kSMVMPrxJitterBufferMode v) {
			jitterp_.mode = v;
			return *this;
		};
		ConfigJitterMode &u_adaptive_target_delay(tSM_UT32 v) {
			jitterp_.u.adaptive.target_delay = v;
			return *this;
		};
		ConfigJitterMode &u_adaptive_freq_upper_tolerance(float v) {
			jitterp_.u.adaptive.freq_upper_tolerance = v;
			return *this;
		};
		ConfigJitterMode &u_adaptive_freq_lower_tolerance(float v) {
			jitterp_.u.adaptive.freq_lower_tolerance = v;
			return *this;
		};
	};
public:
	ConfigJitterMode config_jitter_mode() {
		return ConfigJitterMode().vmprx(vmprx_);
	}
private:
	class ConfigJitterResyncNotify {
		mutable int used_;
		SM_VMPRX_CONFIG_JITTER_RESYNC_NOTIFY_PARMS notifyp_;
	public:
		ConfigJitterResyncNotify(const ConfigJitterResyncNotify &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigJitterResyncNotify() : used_(0) {
			memset(&notifyp_, 0, sizeof(notifyp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_jitter_resync_notify(&notifyp_));
			used_ = 1;
			return e;
		};
		~ConfigJitterResyncNotify() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_jitter_resync_notify not called\n");
				abort();
			};
		};
		ConfigJitterResyncNotify &vmprx(tSMVMPrxId v) {
			notifyp_.vmprx = v;
			return *this;
		};
		ConfigJitterResyncNotify &enable(tSM_INT v) {
			notifyp_.enable = v;
			return *this;
		};
	};
public:
	ConfigJitterResyncNotify config_jitter_resync_notify() {
		return ConfigJitterResyncNotify().vmprx(vmprx_);
	}
private:
	class ConfigSampleRate {
		mutable int used_;
		SM_VMPRX_SAMPLE_RATE_PARMS sample_ratep_;
	public:
		ConfigSampleRate(const ConfigSampleRate &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigSampleRate() : used_(0) {
			memset(&sample_ratep_, 0, sizeof(sample_ratep_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_sample_rate(&sample_ratep_));
			used_ = 1;
			return e;
		};
		~ConfigSampleRate() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_sample_rate not called\n");
				abort();
			};
		};
		ConfigSampleRate &vmprx(tSMVMPrxId v) {
			sample_ratep_.vmprx = v;
			return *this;
		};
		ConfigSampleRate &sample_rate(tSM_UT32 v) {
			sample_ratep_.sample_rate = v;
			return *this;
		};
	};
public:
	ConfigSampleRate config_sample_rate() {
		return ConfigSampleRate().vmprx(vmprx_);
	}
private:
	class ConfigTones {
		mutable int used_;
		SM_VMPRX_TONE_PARMS tonep_;
	public:
		ConfigTones(const ConfigTones &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigTones() : used_(0) {
			memset(&tonep_, 0, sizeof(tonep_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_tones(&tonep_));
			used_ = 1;
			return e;
		};
		~ConfigTones() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_tones not called\n");
				abort();
			};
		};
		ConfigTones &vmprx(tSMVMPrxId v) {
			tonep_.vmprx = v;
			return *this;
		};
		ConfigTones &regen_tones(tSM_INT v) {
			tonep_.regen_tones = v;
			return *this;
		};
		ConfigTones &detect_tones(tSM_INT v) {
			tonep_.detect_tones = v;
			return *this;
		};
		ConfigTones &enforce_tone_spacing(tSM_INT v) {
			tonep_.enforce_tone_spacing = v;
			return *this;
		};
	};
public:
	ConfigTones config_tones() {
		return ConfigTones().vmprx(vmprx_);
	}
private:
	class ConfigUnhandledPayloadReporting {
		mutable int used_;
		SM_VMPRX_UNHANDLED_PAYLOAD_REPORTING_PARMS pp_;
	public:
		ConfigUnhandledPayloadReporting(const ConfigUnhandledPayloadReporting &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigUnhandledPayloadReporting() : used_(0) {
			memset(&pp_, 0, sizeof(pp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_unhandled_payload_reporting(&pp_));
			used_ = 1;
			return e;
		};
		~ConfigUnhandledPayloadReporting() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_unhandled_payload_reporting not called\n");
				abort();
			};
		};
		ConfigUnhandledPayloadReporting &vmprx(tSMVMPrxId v) {
			pp_.vmprx = v;
			return *this;
		};
		ConfigUnhandledPayloadReporting &delay(tSM_INT v) {
			pp_.delay = v;
			return *this;
		};
	};
public:
	ConfigUnhandledPayloadReporting config_unhandled_payload_reporting() {
		return ConfigUnhandledPayloadReporting().vmprx(vmprx_);
	}
private:
	class Create {
		mutable int used_;
		SM_VMPRX_CREATE_PARMS vmprxp_;
		tSMVMPrxId *vmprxP_;
	public:
		Create(const Create &old) {
			*this = old;
			old.used_ = 1;
		};
		Create() : used_(0) {
			memset(&vmprxp_, 0, sizeof(vmprxp_));
			vmprxP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_create(&vmprxp_));
			used_ = 1;
			if (vmprxP_) *vmprxP_ = vmprxp_.vmprx;
			return e;
		};
		~Create() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_create not called\n");
				abort();
			};
		};
		Create &vmprx(tSMVMPrxId *v) {
			vmprxP_ = v;
			return *this;
		};
		Create &module(tSMModuleId v) {
			vmprxp_.module = v;
			return *this;
		};
		Create &type(enum kSMVMPrxType v) {
			vmprxp_.type = v;
			return *this;
		};
		Create &address(in_addr v) {
			vmprxp_.address = v;
			return *this;
		};
		Create &ipv6_address(in6_addr v) {
			vmprxp_.ipv6_address = v;
			return *this;
		};
		Create &mux_rtcp(tSM_INT v) {
			vmprxp_.mux_rtcp = v;
			return *this;
		};
	};
public:
	Create create() {
		return Create().vmprx(&vmprx_);
	}
private:
	class Destroy {
		mutable int used_;
		tSMVMPrxId vmprx_;
	public:
		Destroy(const Destroy &old) {
			*this = old;
			old.used_ = 1;
		};
		Destroy(tSMVMPrxId vmprx) : used_(0) {
			vmprx_ = vmprx;
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_destroy(vmprx_));
			used_ = 1;
			return e;
		};
		~Destroy() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_destroy not called\n");
				abort();
			};
		};
	};
public:
	Destroy destroy() {
		Destroy temp(vmprx_);
		
		vmprx_ = 0;
		return temp;
	}
private:
	class GetDatafeed {
		mutable int used_;
		SM_VMPRX_DATAFEED_PARMS datafeedp_;
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
			Error e(Prosody::sm_vmprx_get_datafeed(&datafeedp_));
			used_ = 1;
			if (datafeedP_) *datafeedP_ = datafeedp_.datafeed;
			return e;
		};
		~GetDatafeed() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_get_datafeed not called\n");
				abort();
			};
		};
		GetDatafeed &vmprx(tSMVMPrxId v) {
			datafeedp_.vmprx = v;
			return *this;
		};
		GetDatafeed &datafeed(tSMDatafeedId *v) {
			datafeedP_ = v;
			return *this;
		};
	};
public:
	GetDatafeed get_datafeed() {
		return GetDatafeed().vmprx(vmprx_);
	}
private:
	class GetEvent {
		mutable int used_;
		SM_VMPRX_EVENT_PARMS eventp_;
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
			Error e(Prosody::sm_vmprx_get_event(&eventp_));
			used_ = 1;
			if (eventP_) *eventP_ = eventp_.event;
			return e;
		};
		~GetEvent() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_get_event not called\n");
				abort();
			};
		};
		GetEvent &vmprx(tSMVMPrxId v) {
			eventp_.vmprx = v;
			return *this;
		};
		GetEvent &event(tSMEventId *v) {
			eventP_ = v;
			return *this;
		};
	};
public:
	GetEvent get_event() {
		return GetEvent().vmprx(vmprx_);
	}
private:
	class GetPorts {
		mutable int used_;
		SM_VMPRX_PORT_PARMS portp_;
		int *RTP_portP_;
		int *RTCP_portP_;
		in_addr *addressP_;
		in6_addr *ipv6_addressP_;
	public:
		GetPorts(const GetPorts &old) {
			*this = old;
			old.used_ = 1;
		};
		GetPorts() : used_(0) {
			memset(&portp_, 0, sizeof(portp_));
			RTP_portP_ = 0;
			RTCP_portP_ = 0;
			addressP_ = 0;
			ipv6_addressP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_get_ports(&portp_));
			used_ = 1;
			if (RTP_portP_) *RTP_portP_ = portp_.RTP_port;
			if (RTCP_portP_) *RTCP_portP_ = portp_.RTCP_port;
			if (addressP_) *addressP_ = portp_.address;
			if (ipv6_addressP_) *ipv6_addressP_ = portp_.ipv6_address;
			return e;
		};
		~GetPorts() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_get_ports not called\n");
				abort();
			};
		};
		GetPorts &vmprx(tSMVMPrxId v) {
			portp_.vmprx = v;
			return *this;
		};
		GetPorts &RTP_port(int *v) {
			RTP_portP_ = v;
			return *this;
		};
		GetPorts &RTCP_port(int *v) {
			RTCP_portP_ = v;
			return *this;
		};
		GetPorts &address(in_addr *v) {
			addressP_ = v;
			return *this;
		};
		GetPorts &ipv6_address(in6_addr *v) {
			ipv6_addressP_ = v;
			return *this;
		};
		GetPorts &nowait(tSM_UT32 v) {
			portp_.nowait = v;
			return *this;
		};
	};
public:
	GetPorts get_ports() {
		return GetPorts().vmprx(vmprx_);
	}
private:
	class SetRtcphand {
		mutable int used_;
		SM_VMPRX_SET_RTCPHAND_PARMS rvp_;
	public:
		SetRtcphand(const SetRtcphand &old) {
			*this = old;
			old.used_ = 1;
		};
		SetRtcphand() : used_(0) {
			memset(&rvp_, 0, sizeof(rvp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_set_rtcphand(&rvp_));
			used_ = 1;
			return e;
		};
		~SetRtcphand() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_set_rtcphand not called\n");
				abort();
			};
		};
		SetRtcphand &vmprx(tSMVMPrxId v) {
			rvp_.vmprx = v;
			return *this;
		};
		SetRtcphand &rtcphand(tSMRTCPHandId v) {
			rvp_.rtcphand = v;
			return *this;
		};
	};
public:
	SetRtcphand set_rtcphand() {
		return SetRtcphand().vmprx(vmprx_);
	}
private:
	class Status {
		mutable int used_;
		SM_VMPRX_STATUS_PARMS statusp_;
		enum kSMVMPrxStatus *statusP_;
		tSM_INT *u_tone_idP_;
		double *u_tone_volumeP_;
		unsigned *u_tone_durationP_;
		int *u_ports_RTP_PortP_;
		int *u_ports_RTCP_PortP_;
		in_addr *u_ports_addressP_;
		tSM_INT *u_run_no_dataP_;
		in_addr *u_ssrc_addressP_;
		int *u_ssrc_portP_;
		int *u_ssrc_ssrcP_;
		int *u_payload_typeP_;
		int *u_codec_specific_payload_typeP_;
		int *u_ports_ipv6_RTP_PortP_;
		int *u_ports_ipv6_RTCP_PortP_;
		in6_addr *u_ports_ipv6_addressP_;
		in6_addr *u_ssrc_ipv6_addressP_;
		int *u_ssrc_ipv6_portP_;
		int *u_ssrc_ipv6_ssrcP_;
		int *u_bind_request_recv_portP_;
		enum kSMAddressType *u_bind_request_address_typeP_;
		in_addr *u_bind_request_address_ipv4P_;
		in6_addr *u_bind_request_address_ipv6P_;
		int *u_bind_request_portP_;
		tSM_UT32 *u_bind_request_priorityP_;
		int *u_bind_request_use_candidateP_;
		int *u_bind_request_remote_is_controllingP_;
		int *u_bind_response_recv_portP_;
		enum kSMVMPrxSTUNResult *u_bind_response_resultP_;
		tSM_INT *u_bind_response_error_codeP_;
		enum kSMAddressType *u_bind_response_address_typeP_;
		in_addr *u_bind_response_address_ipv4P_;
		in6_addr *u_bind_response_address_ipv6P_;
		int *u_bind_response_portP_;
		int *u_bind_response_local_was_controllingP_;
		tSM_UT32 *u_jbresync_timestampP_;
		tSM_UT64 *u_jbresync_localtimerefP_;
		tSM_INT *u_profile_specific_notification_codeP_;
	public:
		Status(const Status &old) {
			*this = old;
			old.used_ = 1;
		};
		Status() : used_(0) {
			memset(&statusp_, 0, sizeof(statusp_));
			statusP_ = 0;
			u_tone_idP_ = 0;
			u_tone_volumeP_ = 0;
			u_tone_durationP_ = 0;
			u_ports_RTP_PortP_ = 0;
			u_ports_RTCP_PortP_ = 0;
			u_ports_addressP_ = 0;
			u_run_no_dataP_ = 0;
			u_ssrc_addressP_ = 0;
			u_ssrc_portP_ = 0;
			u_ssrc_ssrcP_ = 0;
			u_payload_typeP_ = 0;
			u_codec_specific_payload_typeP_ = 0;
			u_ports_ipv6_RTP_PortP_ = 0;
			u_ports_ipv6_RTCP_PortP_ = 0;
			u_ports_ipv6_addressP_ = 0;
			u_ssrc_ipv6_addressP_ = 0;
			u_ssrc_ipv6_portP_ = 0;
			u_ssrc_ipv6_ssrcP_ = 0;
			u_bind_request_recv_portP_ = 0;
			u_bind_request_address_typeP_ = 0;
			u_bind_request_address_ipv4P_ = 0;
			u_bind_request_address_ipv6P_ = 0;
			u_bind_request_portP_ = 0;
			u_bind_request_priorityP_ = 0;
			u_bind_request_use_candidateP_ = 0;
			u_bind_request_remote_is_controllingP_ = 0;
			u_bind_response_recv_portP_ = 0;
			u_bind_response_resultP_ = 0;
			u_bind_response_error_codeP_ = 0;
			u_bind_response_address_typeP_ = 0;
			u_bind_response_address_ipv4P_ = 0;
			u_bind_response_address_ipv6P_ = 0;
			u_bind_response_portP_ = 0;
			u_bind_response_local_was_controllingP_ = 0;
			u_jbresync_timestampP_ = 0;
			u_jbresync_localtimerefP_ = 0;
			u_profile_specific_notification_codeP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_status(&statusp_));
			used_ = 1;
			if (statusP_) *statusP_ = statusp_.status;
			if (u_tone_idP_) *u_tone_idP_ = statusp_.u.tone.id;
			if (u_tone_volumeP_) *u_tone_volumeP_ = statusp_.u.tone.volume;
			if (u_tone_durationP_) *u_tone_durationP_ = statusp_.u.tone.duration;
			if (u_ports_RTP_PortP_) *u_ports_RTP_PortP_ = statusp_.u.ports.RTP_Port;
			if (u_ports_RTCP_PortP_) *u_ports_RTCP_PortP_ = statusp_.u.ports.RTCP_Port;
			if (u_ports_addressP_) *u_ports_addressP_ = statusp_.u.ports.address;
			if (u_run_no_dataP_) *u_run_no_dataP_ = statusp_.u.run.no_data;
			if (u_ssrc_addressP_) *u_ssrc_addressP_ = statusp_.u.ssrc.address;
			if (u_ssrc_portP_) *u_ssrc_portP_ = statusp_.u.ssrc.port;
			if (u_ssrc_ssrcP_) *u_ssrc_ssrcP_ = statusp_.u.ssrc.ssrc;
			if (u_payload_typeP_) *u_payload_typeP_ = statusp_.u.payload.type;
			if (u_codec_specific_payload_typeP_) *u_codec_specific_payload_typeP_ = statusp_.u.codec_specific.payload_type;
			if (u_ports_ipv6_RTP_PortP_) *u_ports_ipv6_RTP_PortP_ = statusp_.u.ports_ipv6.RTP_Port;
			if (u_ports_ipv6_RTCP_PortP_) *u_ports_ipv6_RTCP_PortP_ = statusp_.u.ports_ipv6.RTCP_Port;
			if (u_ports_ipv6_addressP_) *u_ports_ipv6_addressP_ = statusp_.u.ports_ipv6.address;
			if (u_ssrc_ipv6_addressP_) *u_ssrc_ipv6_addressP_ = statusp_.u.ssrc_ipv6.address;
			if (u_ssrc_ipv6_portP_) *u_ssrc_ipv6_portP_ = statusp_.u.ssrc_ipv6.port;
			if (u_ssrc_ipv6_ssrcP_) *u_ssrc_ipv6_ssrcP_ = statusp_.u.ssrc_ipv6.ssrc;
			if (u_bind_request_recv_portP_) *u_bind_request_recv_portP_ = statusp_.u.bind_request.recv_port;
			if (u_bind_request_address_typeP_) *u_bind_request_address_typeP_ = statusp_.u.bind_request.address_type;
			if (u_bind_request_address_ipv4P_) *u_bind_request_address_ipv4P_ = statusp_.u.bind_request.address.ipv4;
			if (u_bind_request_address_ipv6P_) *u_bind_request_address_ipv6P_ = statusp_.u.bind_request.address.ipv6;
			if (u_bind_request_portP_) *u_bind_request_portP_ = statusp_.u.bind_request.port;
			if (u_bind_request_priorityP_) *u_bind_request_priorityP_ = statusp_.u.bind_request.priority;
			if (u_bind_request_use_candidateP_) *u_bind_request_use_candidateP_ = statusp_.u.bind_request.use_candidate;
			if (u_bind_request_remote_is_controllingP_) *u_bind_request_remote_is_controllingP_ = statusp_.u.bind_request.remote_is_controlling;
			if (u_bind_response_recv_portP_) *u_bind_response_recv_portP_ = statusp_.u.bind_response.recv_port;
			if (u_bind_response_resultP_) *u_bind_response_resultP_ = statusp_.u.bind_response.result;
			if (u_bind_response_error_codeP_) *u_bind_response_error_codeP_ = statusp_.u.bind_response.error_code;
			if (u_bind_response_address_typeP_) *u_bind_response_address_typeP_ = statusp_.u.bind_response.address_type;
			if (u_bind_response_address_ipv4P_) *u_bind_response_address_ipv4P_ = statusp_.u.bind_response.address.ipv4;
			if (u_bind_response_address_ipv6P_) *u_bind_response_address_ipv6P_ = statusp_.u.bind_response.address.ipv6;
			if (u_bind_response_portP_) *u_bind_response_portP_ = statusp_.u.bind_response.port;
			if (u_bind_response_local_was_controllingP_) *u_bind_response_local_was_controllingP_ = statusp_.u.bind_response.local_was_controlling;
			if (u_jbresync_timestampP_) *u_jbresync_timestampP_ = statusp_.u.jbresync.timestamp;
			if (u_jbresync_localtimerefP_) *u_jbresync_localtimerefP_ = statusp_.u.jbresync.localtimeref;
			if (u_profile_specific_notification_codeP_) *u_profile_specific_notification_codeP_ = statusp_.u.profile_specific.notification_code;
			return e;
		};
		~Status() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_status not called\n");
				abort();
			};
		};
		Status &vmprx(tSMVMPrxId v) {
			statusp_.vmprx = v;
			return *this;
		};
		Status &status(enum kSMVMPrxStatus *v) {
			statusP_ = v;
			return *this;
		};
		Status &u_tone_id(tSM_INT *v) {
			u_tone_idP_ = v;
			return *this;
		};
		Status &u_tone_volume(double *v) {
			u_tone_volumeP_ = v;
			return *this;
		};
		Status &u_tone_duration(unsigned *v) {
			u_tone_durationP_ = v;
			return *this;
		};
		Status &u_ports_RTP_Port(int *v) {
			u_ports_RTP_PortP_ = v;
			return *this;
		};
		Status &u_ports_RTCP_Port(int *v) {
			u_ports_RTCP_PortP_ = v;
			return *this;
		};
		Status &u_ports_address(in_addr *v) {
			u_ports_addressP_ = v;
			return *this;
		};
		Status &u_run_no_data(tSM_INT *v) {
			u_run_no_dataP_ = v;
			return *this;
		};
		Status &u_ssrc_address(in_addr *v) {
			u_ssrc_addressP_ = v;
			return *this;
		};
		Status &u_ssrc_port(int *v) {
			u_ssrc_portP_ = v;
			return *this;
		};
		Status &u_ssrc_ssrc(int *v) {
			u_ssrc_ssrcP_ = v;
			return *this;
		};
		Status &u_payload_type(int *v) {
			u_payload_typeP_ = v;
			return *this;
		};
		Status &u_codec_specific_payload_type(int *v) {
			u_codec_specific_payload_typeP_ = v;
			return *this;
		};
		Status &u_ports_ipv6_RTP_Port(int *v) {
			u_ports_ipv6_RTP_PortP_ = v;
			return *this;
		};
		Status &u_ports_ipv6_RTCP_Port(int *v) {
			u_ports_ipv6_RTCP_PortP_ = v;
			return *this;
		};
		Status &u_ports_ipv6_address(in6_addr *v) {
			u_ports_ipv6_addressP_ = v;
			return *this;
		};
		Status &u_ssrc_ipv6_address(in6_addr *v) {
			u_ssrc_ipv6_addressP_ = v;
			return *this;
		};
		Status &u_ssrc_ipv6_port(int *v) {
			u_ssrc_ipv6_portP_ = v;
			return *this;
		};
		Status &u_ssrc_ipv6_ssrc(int *v) {
			u_ssrc_ipv6_ssrcP_ = v;
			return *this;
		};
		Status &u_bind_request_recv_port(int *v) {
			u_bind_request_recv_portP_ = v;
			return *this;
		};
		Status &u_bind_request_address_type(enum kSMAddressType *v) {
			u_bind_request_address_typeP_ = v;
			return *this;
		};
		Status &u_bind_request_address_ipv4(in_addr *v) {
			u_bind_request_address_ipv4P_ = v;
			return *this;
		};
		Status &u_bind_request_address_ipv6(in6_addr *v) {
			u_bind_request_address_ipv6P_ = v;
			return *this;
		};
		Status &u_bind_request_port(int *v) {
			u_bind_request_portP_ = v;
			return *this;
		};
		Status &u_bind_request_priority(tSM_UT32 *v) {
			u_bind_request_priorityP_ = v;
			return *this;
		};
		Status &u_bind_request_use_candidate(int *v) {
			u_bind_request_use_candidateP_ = v;
			return *this;
		};
		Status &u_bind_request_remote_is_controlling(int *v) {
			u_bind_request_remote_is_controllingP_ = v;
			return *this;
		};
		Status &u_bind_response_recv_port(int *v) {
			u_bind_response_recv_portP_ = v;
			return *this;
		};
		Status &u_bind_response_result(enum kSMVMPrxSTUNResult *v) {
			u_bind_response_resultP_ = v;
			return *this;
		};
		Status &u_bind_response_error_code(tSM_INT *v) {
			u_bind_response_error_codeP_ = v;
			return *this;
		};
		Status &u_bind_response_address_type(enum kSMAddressType *v) {
			u_bind_response_address_typeP_ = v;
			return *this;
		};
		Status &u_bind_response_address_ipv4(in_addr *v) {
			u_bind_response_address_ipv4P_ = v;
			return *this;
		};
		Status &u_bind_response_address_ipv6(in6_addr *v) {
			u_bind_response_address_ipv6P_ = v;
			return *this;
		};
		Status &u_bind_response_port(int *v) {
			u_bind_response_portP_ = v;
			return *this;
		};
		Status &u_bind_response_local_was_controlling(int *v) {
			u_bind_response_local_was_controllingP_ = v;
			return *this;
		};
		Status &u_jbresync_timestamp(tSM_UT32 *v) {
			u_jbresync_timestampP_ = v;
			return *this;
		};
		Status &u_jbresync_localtimeref(tSM_UT64 *v) {
			u_jbresync_localtimerefP_ = v;
			return *this;
		};
		Status &u_profile_specific_notification_code(tSM_INT *v) {
			u_profile_specific_notification_codeP_ = v;
			return *this;
		};
	};
public:
	Status status() {
		return Status().vmprx(vmprx_);
	}
private:
	class StatusDiscardCodecSpecific {
		mutable int used_;
		tSMVMPrxId vmprx_;
	public:
		StatusDiscardCodecSpecific(const StatusDiscardCodecSpecific &old) {
			*this = old;
			old.used_ = 1;
		};
		StatusDiscardCodecSpecific(tSMVMPrxId vmprx) : used_(0) {
			vmprx_ = vmprx;
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_status_discard_codec_specific(vmprx_));
			used_ = 1;
			return e;
		};
		~StatusDiscardCodecSpecific() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_status_discard_codec_specific not called\n");
				abort();
			};
		};
	};
public:
	StatusDiscardCodecSpecific status_discard_codec_specific() {
		return StatusDiscardCodecSpecific(vmprx_);
	}
private:
	class Stop {
		mutable int used_;
		SM_VMPRX_STOP_PARMS stopp_;
	public:
		Stop(const Stop &old) {
			*this = old;
			old.used_ = 1;
		};
		Stop() : used_(0) {
			memset(&stopp_, 0, sizeof(stopp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_stop(&stopp_));
			used_ = 1;
			return e;
		};
		~Stop() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_stop not called\n");
				abort();
			};
		};
		Stop &vmprx(tSMVMPrxId v) {
			stopp_.vmprx = v;
			return *this;
		};
	};
public:
	Stop stop() {
		return Stop().vmprx(vmprx_);
	}
private:
	class StatusCodecAmrnb {
		mutable int used_;
		SM_VMPRX_STATUS_CODEC_AMRNB_PARMS amrnbp_;
		enum kSMVMPrxAMRNBStatus *statusP_;
		tSM_INT *u_cmr_bitrateP_;
	public:
		StatusCodecAmrnb(const StatusCodecAmrnb &old) {
			*this = old;
			old.used_ = 1;
		};
		StatusCodecAmrnb() : used_(0) {
			memset(&amrnbp_, 0, sizeof(amrnbp_));
			statusP_ = 0;
			u_cmr_bitrateP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_status_codec_amrnb(&amrnbp_));
			used_ = 1;
			if (statusP_) *statusP_ = amrnbp_.status;
			if (u_cmr_bitrateP_) *u_cmr_bitrateP_ = amrnbp_.u.cmr.bitrate;
			return e;
		};
		~StatusCodecAmrnb() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_status_codec_amrnb not called\n");
				abort();
			};
		};
		StatusCodecAmrnb &vmprx(tSMVMPrxId v) {
			amrnbp_.vmprx = v;
			return *this;
		};
		StatusCodecAmrnb &payload_type(tSM_INT v) {
			amrnbp_.payload_type = v;
			return *this;
		};
		StatusCodecAmrnb &status(enum kSMVMPrxAMRNBStatus *v) {
			statusP_ = v;
			return *this;
		};
		StatusCodecAmrnb &u_cmr_bitrate(tSM_INT *v) {
			u_cmr_bitrateP_ = v;
			return *this;
		};
	};
public:
	StatusCodecAmrnb status_codec_amrnb() {
		return StatusCodecAmrnb().vmprx(vmprx_);
	}
private:
	class StatusCodecAmrwb {
		mutable int used_;
		SM_VMPRX_STATUS_CODEC_AMRWB_PARMS amrwbp_;
		enum kSMVMPrxAMRWBStatus *statusP_;
		tSM_INT *u_cmr_bitrateP_;
	public:
		StatusCodecAmrwb(const StatusCodecAmrwb &old) {
			*this = old;
			old.used_ = 1;
		};
		StatusCodecAmrwb() : used_(0) {
			memset(&amrwbp_, 0, sizeof(amrwbp_));
			statusP_ = 0;
			u_cmr_bitrateP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_status_codec_amrwb(&amrwbp_));
			used_ = 1;
			if (statusP_) *statusP_ = amrwbp_.status;
			if (u_cmr_bitrateP_) *u_cmr_bitrateP_ = amrwbp_.u.cmr.bitrate;
			return e;
		};
		~StatusCodecAmrwb() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_status_codec_amrwb not called\n");
				abort();
			};
		};
		StatusCodecAmrwb &vmprx(tSMVMPrxId v) {
			amrwbp_.vmprx = v;
			return *this;
		};
		StatusCodecAmrwb &payload_type(tSM_INT v) {
			amrwbp_.payload_type = v;
			return *this;
		};
		StatusCodecAmrwb &status(enum kSMVMPrxAMRWBStatus *v) {
			statusP_ = v;
			return *this;
		};
		StatusCodecAmrwb &u_cmr_bitrate(tSM_INT *v) {
			u_cmr_bitrateP_ = v;
			return *this;
		};
	};
public:
	StatusCodecAmrwb status_codec_amrwb() {
		return StatusCodecAmrwb().vmprx(vmprx_);
	}
private:
	class ConfigCodecSse {
		mutable int used_;
		SM_VMPRX_CODEC_SSE_PARMS codecp_;
	public:
		ConfigCodecSse(const ConfigCodecSse &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecSse() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_sse(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecSse() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_sse not called\n");
				abort();
			};
		};
		ConfigCodecSse &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
			return *this;
		};
		ConfigCodecSse &payload_type(tSM_INT v) {
			codecp_.payload_type = v;
			return *this;
		};
	};
public:
	ConfigCodecSse config_codec_sse() {
		return ConfigCodecSse().vmprx(vmprx_);
	}
private:
	class StatusCodecSse {
		mutable int used_;
		SM_VMPRX_STATUS_CODEC_SSE_PARMS ssep_;
		enum kSMVMPrxSSEStatus *statusP_;
		char* *payloadP_;
		tSM_INT *lengthP_;
	public:
		StatusCodecSse(const StatusCodecSse &old) {
			*this = old;
			old.used_ = 1;
		};
		StatusCodecSse() : used_(0) {
			memset(&ssep_, 0, sizeof(ssep_));
			statusP_ = 0;
			payloadP_ = 0;
			lengthP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_status_codec_sse(&ssep_));
			used_ = 1;
			if (statusP_) *statusP_ = ssep_.status;
			if (payloadP_) *payloadP_ = ssep_.payload;
			if (lengthP_) *lengthP_ = ssep_.length;
			return e;
		};
		~StatusCodecSse() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_status_codec_sse not called\n");
				abort();
			};
		};
		StatusCodecSse &vmprx(tSMVMPrxId v) {
			ssep_.vmprx = v;
			return *this;
		};
		StatusCodecSse &payload_type(tSM_INT v) {
			ssep_.payload_type = v;
			return *this;
		};
		StatusCodecSse &status(enum kSMVMPrxSSEStatus *v) {
			statusP_ = v;
			return *this;
		};
		StatusCodecSse &payload(char* *v) {
			payloadP_ = v;
			return *this;
		};
		StatusCodecSse &payload(char* v) {
			ssep_.payload = v;
			return *this;
		};
		StatusCodecSse &max_length(tSM_INT v) {
			ssep_.max_length = v;
			return *this;
		};
		StatusCodecSse &length(tSM_INT *v) {
			lengthP_ = v;
			return *this;
		};
	};
public:
	StatusCodecSse status_codec_sse() {
		return StatusCodecSse().vmprx(vmprx_);
	}
private:
	class StatusProfileSpecific {
		mutable int used_;
		SM_VMPRX_STATUS_PROFILE_SPECIFIC_PARMS psp_;
		char* *payloadP_;
		tSM_INT *lengthP_;
	public:
		StatusProfileSpecific(const StatusProfileSpecific &old) {
			*this = old;
			old.used_ = 1;
		};
		StatusProfileSpecific() : used_(0) {
			memset(&psp_, 0, sizeof(psp_));
			payloadP_ = 0;
			lengthP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_status_profile_specific(&psp_));
			used_ = 1;
			if (payloadP_) *payloadP_ = psp_.payload;
			if (lengthP_) *lengthP_ = psp_.length;
			return e;
		};
		~StatusProfileSpecific() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_status_profile_specific not called\n");
				abort();
			};
		};
		StatusProfileSpecific &vmprx(tSMVMPrxId v) {
			psp_.vmprx = v;
			return *this;
		};
		StatusProfileSpecific &notification_code(tSM_INT v) {
			psp_.notification_code = v;
			return *this;
		};
		StatusProfileSpecific &payload(char* *v) {
			payloadP_ = v;
			return *this;
		};
		StatusProfileSpecific &payload(char* v) {
			psp_.payload = v;
			return *this;
		};
		StatusProfileSpecific &max_length(tSM_INT v) {
			psp_.max_length = v;
			return *this;
		};
		StatusProfileSpecific &length(tSM_INT *v) {
			lengthP_ = v;
			return *this;
		};
	};
public:
	StatusProfileSpecific status_profile_specific() {
		return StatusProfileSpecific().vmprx(vmprx_);
	}
private:
	class ConfigSprt {
		mutable int used_;
		SM_VMPRX_CONFIG_SPRT_PARMS configp_;
	public:
		ConfigSprt(const ConfigSprt &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigSprt() : used_(0) {
			memset(&configp_, 0, sizeof(configp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_sprt(&configp_));
			used_ = 1;
			return e;
		};
		~ConfigSprt() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_sprt not called\n");
				abort();
			};
		};
		ConfigSprt &vmprx(tSMVMPrxId v) {
			configp_.vmprx = v;
			return *this;
		};
		ConfigSprt &payload_type(tSM_INT v) {
			configp_.payload_type = v;
			return *this;
		};
		ConfigSprt &sprt(tSMSPRTId v) {
			configp_.sprt = v;
			return *this;
		};
	};
public:
	ConfigSprt config_sprt() {
		return ConfigSprt().vmprx(vmprx_);
	}
private:
	class ConfigForwarding {
		mutable int used_;
		SM_VMPRX_CONFIG_FORWARDING_PARMS configp_;
	public:
		ConfigForwarding(const ConfigForwarding &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigForwarding() : used_(0) {
			memset(&configp_, 0, sizeof(configp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_forwarding(&configp_));
			used_ = 1;
			return e;
		};
		~ConfigForwarding() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_forwarding not called\n");
				abort();
			};
		};
		ConfigForwarding &vmprx(tSMVMPrxId v) {
			configp_.vmprx = v;
			return *this;
		};
		ConfigForwarding &mode(enum kVMPrxFwdMode v) {
			configp_.mode = v;
			return *this;
		};
		ConfigForwarding &dest_type(enum kSMVMPrxFwdDestType v) {
			configp_.dest_type = v;
			return *this;
		};
		ConfigForwarding &u_ipv4_destination(SOCKADDR_IN v) {
			configp_.u.ipv4.destination = v;
			return *this;
		};
		ConfigForwarding &u_ipv4_source(SOCKADDR_IN v) {
			configp_.u.ipv4.source = v;
			return *this;
		};
		ConfigForwarding &u_ipv4_TOS(int v) {
			configp_.u.ipv4.TOS = v;
			return *this;
		};
		ConfigForwarding &u_ipv6_destination(SOCKADDR_IN6 v) {
			configp_.u.ipv6.destination = v;
			return *this;
		};
		ConfigForwarding &u_ipv6_source(SOCKADDR_IN6 v) {
			configp_.u.ipv6.source = v;
			return *this;
		};
		ConfigForwarding &num_types(int v) {
			configp_.num_types = v;
			return *this;
		};
	};
public:
	ConfigForwarding config_forwarding() {
		return ConfigForwarding().vmprx(vmprx_);
	}
private:
	class ConfigCodecRttext {
		mutable int used_;
		SM_VMPRX_CODEC_RTTEXT_PARMS codecp_;
	public:
		ConfigCodecRttext(const ConfigCodecRttext &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodecRttext() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec_rttext(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodecRttext() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec_rttext not called\n");
				abort();
			};
		};
		ConfigCodecRttext &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
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
		ConfigCodecRttext &out_of_order_delay(tSM_INT v) {
			codecp_.out_of_order_delay = v;
			return *this;
		};
	};
public:
	ConfigCodecRttext config_codec_rttext() {
		return ConfigCodecRttext().vmprx(vmprx_);
	}
private:
	class IceStunConfig {
		mutable int used_;
		SM_VMPRX_ICE_STUN_CONFIG_PARMS configp_;
	public:
		IceStunConfig(const IceStunConfig &old) {
			*this = old;
			old.used_ = 1;
		};
		IceStunConfig() : used_(0) {
			memset(&configp_, 0, sizeof(configp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_ice_stun_config(&configp_));
			used_ = 1;
			return e;
		};
		~IceStunConfig() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_ice_stun_config not called\n");
				abort();
			};
		};
		IceStunConfig &vmprx(tSMVMPrxId v) {
			configp_.vmprx = v;
			return *this;
		};
		IceStunConfig &local_role(enum kSMICERole v) {
			configp_.local_role = v;
			return *this;
		};
		IceStunConfig &local_ice_ufrag(char* v) {
			configp_.local_ice_ufrag = v;
			return *this;
		};
		IceStunConfig &local_ice_ufrag_length(tSM_UT32 v) {
			configp_.local_ice_ufrag_length = v;
			return *this;
		};
		IceStunConfig &local_ice_pwd(char* v) {
			configp_.local_ice_pwd = v;
			return *this;
		};
		IceStunConfig &local_ice_pwd_length(tSM_UT32 v) {
			configp_.local_ice_pwd_length = v;
			return *this;
		};
		IceStunConfig &role_tiebreaker(tSM_UT64 v) {
			configp_.role_tiebreaker = v;
			return *this;
		};
	};
public:
	IceStunConfig ice_stun_config() {
		return IceStunConfig().vmprx(vmprx_);
	}
private:
	class IceStunChangeRole {
		mutable int used_;
		SM_VMPRX_ICE_STUN_CHANGE_ROLE_PARMS rolep_;
	public:
		IceStunChangeRole(const IceStunChangeRole &old) {
			*this = old;
			old.used_ = 1;
		};
		IceStunChangeRole() : used_(0) {
			memset(&rolep_, 0, sizeof(rolep_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_ice_stun_change_role(&rolep_));
			used_ = 1;
			return e;
		};
		~IceStunChangeRole() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_ice_stun_change_role not called\n");
				abort();
			};
		};
		IceStunChangeRole &vmprx(tSMVMPrxId v) {
			rolep_.vmprx = v;
			return *this;
		};
		IceStunChangeRole &local_role(enum kSMICERole v) {
			rolep_.local_role = v;
			return *this;
		};
	};
public:
	IceStunChangeRole ice_stun_change_role() {
		return IceStunChangeRole().vmprx(vmprx_);
	}
private:
	class IceStunRemoteCredentials {
		mutable int used_;
		SM_VMPRX_ICE_STUN_REMOTE_CREDENTIALS_PARMS credentialsp_;
	public:
		IceStunRemoteCredentials(const IceStunRemoteCredentials &old) {
			*this = old;
			old.used_ = 1;
		};
		IceStunRemoteCredentials() : used_(0) {
			memset(&credentialsp_, 0, sizeof(credentialsp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_ice_stun_remote_credentials(&credentialsp_));
			used_ = 1;
			return e;
		};
		~IceStunRemoteCredentials() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_ice_stun_remote_credentials not called\n");
				abort();
			};
		};
		IceStunRemoteCredentials &vmprx(tSMVMPrxId v) {
			credentialsp_.vmprx = v;
			return *this;
		};
		IceStunRemoteCredentials &remote_ice_ufrag(char* v) {
			credentialsp_.remote_ice_ufrag = v;
			return *this;
		};
		IceStunRemoteCredentials &remote_ice_ufrag_length(tSM_UT32 v) {
			credentialsp_.remote_ice_ufrag_length = v;
			return *this;
		};
		IceStunRemoteCredentials &remote_ice_pwd(char* v) {
			credentialsp_.remote_ice_pwd = v;
			return *this;
		};
		IceStunRemoteCredentials &remote_ice_pwd_length(tSM_UT32 v) {
			credentialsp_.remote_ice_pwd_length = v;
			return *this;
		};
	};
public:
	IceStunRemoteCredentials ice_stun_remote_credentials() {
		return IceStunRemoteCredentials().vmprx(vmprx_);
	}
private:
	class IceStunSendBindRequest {
		mutable int used_;
		SM_VMPRX_ICE_STUN_SEND_BIND_REQUEST_PARMS requestp_;
	public:
		IceStunSendBindRequest(const IceStunSendBindRequest &old) {
			*this = old;
			old.used_ = 1;
		};
		IceStunSendBindRequest() : used_(0) {
			memset(&requestp_, 0, sizeof(requestp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_ice_stun_send_bind_request(&requestp_));
			used_ = 1;
			return e;
		};
		~IceStunSendBindRequest() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_ice_stun_send_bind_request not called\n");
				abort();
			};
		};
		IceStunSendBindRequest &vmprx(tSMVMPrxId v) {
			requestp_.vmprx = v;
			return *this;
		};
		IceStunSendBindRequest &use_RTCP_port(int v) {
			requestp_.use_RTCP_port = v;
			return *this;
		};
		IceStunSendBindRequest &transaction_id(char *v) {
			strncpy(requestp_.transaction_id, v, sizeof(requestp_.transaction_id));
			return *this;
		};
		IceStunSendBindRequest &destination_ipv4(SOCKADDR_IN v) {
			requestp_.destination.ipv4 = v;
			return *this;
		};
		IceStunSendBindRequest &destination_ipv6(SOCKADDR_IN6 v) {
			requestp_.destination.ipv6 = v;
			return *this;
		};
		IceStunSendBindRequest &priority(tSM_UT32 v) {
			requestp_.priority = v;
			return *this;
		};
		IceStunSendBindRequest &use_candidate(tSM_INT v) {
			requestp_.use_candidate = v;
			return *this;
		};
	};
public:
	IceStunSendBindRequest ice_stun_send_bind_request() {
		return IceStunSendBindRequest().vmprx(vmprx_);
	}
private:
	class ConfigCodec {
		mutable int used_;
		SM_VMPRX_CODEC_PARMS codecp_;
	public:
		ConfigCodec(const ConfigCodec &old) {
			*this = old;
			old.used_ = 1;
		};
		ConfigCodec() : used_(0) {
			memset(&codecp_, 0, sizeof(codecp_));
		};
		operator Error() {
			Error e(Prosody::sm_vmprx_config_codec(&codecp_));
			used_ = 1;
			return e;
		};
		~ConfigCodec() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_vmprx_config_codec not called\n");
				abort();
			};
		};
		ConfigCodec &vmprx(tSMVMPrxId v) {
			codecp_.vmprx = v;
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
		ConfigCodec &plc_mode(enum kSMPLCMode v) {
			codecp_.plc_mode = v;
			return *this;
		};
	};
public:
	ConfigCodec config_codec() {
		return ConfigCodec().vmprx(vmprx_);
	}
