class CFGRXCODEC : public CFGVAL {
public:
	char *parse(CFG *cfg, char *cmd);
	std::ostream &put(std::ostream &os);
	class CFGALAW : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGPAYLOAD payload_type;
		CFGPLCMODE plc_mode;
	} alaw;
	class CFGAMRNB : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGINT aligned;
		CFGINT gsmefr;
		CFGINT payload_type;
		CFGPLCMODE plc_mode;
	} amrnb;
	class CFGCOMFORT_NOISE : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGINT payload_type;
		CFGPLCMODE plc_mode;
	} comfort_noise;
	class CFGEVRC : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGINT payload_type;
		CFGPLCMODE plc_mode;
		CFGINT post_filter;
		CFGEVRCRTPMODE rtp_mode;
	} evrc;
	class CFGG723_1 : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGINT payload_type;
		CFGPLCMODE plc_mode;
		CFGINT post_filter;
	} g723_1;
	class CFGG726 : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGINT bits;
		CFGINT payload_type;
		CFGPLCMODE plc_mode;
	} g726;
	class CFGG728 : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGINT payload_type;
		CFGPLCMODE plc_mode;
		CFGINT post_filter;
		CFGINT rate;
	} g728;
	class CFGG729AB : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGINT payload_type;
		CFGPLCMODE plc_mode;
	} g729ab;
	class CFGG729I : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGG729MODE g729_mode;
		CFGINT payload_type;
	} g729i;
	class CFGGSMEFR : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGINT payload_type;
		CFGPLCMODE plc_mode;
	} gsmefr;
	class CFGGSMFR : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGINT payload_type;
		CFGPLCMODE plc_mode;
		CFGGSMFR_VARIANT variant;
	} gsmfr;
	class CFGILBC : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGINT enhancer;
		CFGINT frame_len;
		CFGINT payload_type;
		CFGPLCMODE plc_mode;
	} ilbc;
	class CFGISAC : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGINT payload_type;
	} isac;
	class CFGL16 : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGINT payload_type;
		CFGPLCMODE plc_mode;
	} l16;
	class CFGL8 : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGINT payload_type;
		CFGPLCMODE plc_mode;
	} l8;
	class CFGMELPE : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGINT payload_type;
	} melpe;
	class CFGMULAW : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGINT payload_type;
		CFGPLCMODE plc_mode;
	} mulaw;
	class CFGRFC2833 : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGINT payload_type;
		CFGPLCMODE plc_mode;
	} rfc2833;
	class CFGRFC4040 : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGINT payload_type;
	} rfc4040;
	class CFGSMV : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGINT payload_type;
		CFGPLCMODE plc_mode;
	} smv;
};
class CFGTXCODEC : public CFGVAL {
public:
	char *parse(CFG *cfg, char *cmd);
	std::ostream &put(std::ostream &os);
	class CFGALAW : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGVADMODE VADMode;
		CFGPAYLOAD payload_type;
		CFGINT ptime;
	} alaw;
	class CFGAMRNB : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGVADMODE VADMode;
		CFGINT aligned;
		CFGINT frames_per_packet;
		CFGINT gsmefr;
		CFGINT payload_type;
	} amrnb;
	class CFGCOMFORT_NOISE : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGINT payload_type;
	} comfort_noise;
	class CFGEVRC : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGVADMODE VADMode;
		CFGINT frames_per_packet;
		CFGINT high_pass_filter;
		CFGEVRCRATE max_rate;
		CFGEVRCRATE min_rate;
		CFGINT noise_suppression_filter;
		CFGINT payload_type;
		CFGEVRCRTPMODE rtp_mode;
	} evrc;
	class CFGG723_1 : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGINT frames_per_packet;
		CFGINT high_pass_filter;
		CFGINT payload_type;
		CFGINT rate;
		CFGINT silence_compression;
	} g723_1;
	class CFGG726 : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGVADMODE VADMode;
		CFGINT bits;
		CFGINT packetlen;
		CFGINT payload_type;
	} g726;
	class CFGG728 : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGVADMODE VADMode;
		CFGINT payload_type;
		CFGINT ptime;
		CFGINT rate;
	} g728;
	class CFGG729AB : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGVADMODE VADMode;
		CFGINT payload_type;
		CFGINT ptime;
	} g729ab;
	class CFGG729I : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGVADMODE VADMode;
		CFGG729MODE g729_mode;
		CFGINT payload_type;
		CFGINT ptime;
	} g729i;
	class CFGGSMEFR : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGVADMODE VADMode;
		CFGINT frames_per_packet;
		CFGINT payload_type;
	} gsmefr;
	class CFGGSMFR : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGVADMODE VADMode;
		CFGINT frames_per_packet;
		CFGINT payload_type;
		CFGGSMFR_VARIANT variant;
	} gsmfr;
	class CFGILBC : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGVADMODE VADMode;
		CFGINT frame_len;
		CFGINT frames_per_packet;
		CFGINT payload_type;
	} ilbc;
	class CFGISAC : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGINT channel_associated_mode;
		CFGVMPRX partner;
		CFGINT payload_type;
	} isac;
	class CFGL16 : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGVADMODE VADMode;
		CFGINT payload_type;
		CFGINT ptime;
	} l16;
	class CFGL8 : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGVADMODE VADMode;
		CFGINT payload_type;
		CFGINT ptime;
	} l8;
	class CFGMELPE : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGINT payload_type;
		CFGINT ptime;
	} melpe;
	class CFGMULAW : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGVADMODE VADMode;
		CFGINT payload_type;
		CFGINT ptime;
	} mulaw;
	class CFGRFC2833 : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGINT payload_type;
	} rfc2833;
	class CFGRFC4040 : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGINT packetlen;
		CFGINT payload_type;
	} rfc4040;
	class CFGSMV : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGVADMODE VADMode;
		CFGINT frames_per_packet;
		CFGINT payload_type;
	} smv;
};

class CFG : public CFGVAL {
public:
	char *parse(CFG *cfg, char *cmd);
	std::ostream &put(std::ostream &os);
	CFGCARD card;
	CFGMOD mod;
	class CFGPATH : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		class CFGAGC : public CFGVAL {
		public:
			char *parse(CFG *cfg, char *cmd);
			std::ostream &put(std::ostream &os);
			CFGINT agc;
			CFGINT volume;
		} agc;
		class CFGEC : public CFGVAL {
		public:
			char *parse(CFG *cfg, char *cmd);
			std::ostream &put(std::ostream &os);
			CFGINT delay;
			CFGINT enable;
			CFGINT fix_agc;
			CFGINT non_linear;
			CFGSOURCE ref;
			CFGINT span;
			CFGINT use_agc;
		} ec;
		class CFGMIX : public CFGVAL {
		public:
			char *parse(CFG *cfg, char *cmd);
			std::ostream &put(std::ostream &os);
			CFGINT enable;
			CFGSOURCE mixin;
			CFGINT volume;
		} mix;
		class CFGPITCHSHIFT : public CFGVAL {
		public:
			char *parse(CFG *cfg, char *cmd);
			std::ostream &put(std::ostream &os);
			CFGFLOAT by;
		} pitchshift;
		class CFGRESAMPLE : public CFGVAL {
		public:
			char *parse(CFG *cfg, char *cmd);
			std::ostream &put(std::ostream &os);
			CFGINT downrate;
			CFGINT uprate;
		} resample;
	} path;
	class CFGPLAY : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGINT agc;
		CFGSTRING file;
		CFGFORMAT format;
		CFGONCE once;
		CFGINT outrate;
		CFGINT rate;
		CFGINT speed;
		CFGINT threshold;
		CFGINT volume;
	} play;
	class CFGREC : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGINT agc;
		CFGSTRING file;
		CFGFORMAT format;
		CFGINT gain;
		CFGINT rate;
		CFGINT threshold;
		CFGINT volume;
	} rec;
	class CFGRTCP : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGINT bw;
		CFGSTRING file;
		CFGINT reports;
		CFGINT rxbw;
		CFGSVEC sdes;
		CFGINT txbw;
	} rtcp;
	class CFGRTPRX : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGRXCODEC codec;
		CFGINT dataloss;
		CFGINT detect;
		CFGINT initjitter;
		CFGADDR local;
		CFGINT maxjitter;
		CFGINT rate;
		CFGINT regen;
		CFGZINT rtcp;
		CFGINT unhandled;
	} rtprx;
	class CFGRTPTX : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGTXCODEC codec;
		CFGADDR dest;
		CFGPORT destport;
		CFGINT isac_ptime;
		CFGINT isac_rate;
		CFGINT rate;
		CFGZINT rtcp;
		CFGADDR rtcpdest;
		CFGPORT rtcpdestport;
		CFGADDR rtcpsrc;
		CFGPORT rtcpsrcport;
		CFGADDR src;
		CFGPORT srcport;
		CFGINT tosrtcp;
		CFGINT tosrtp;
	} rtptx;
	class CFGTDMRX : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGTIMESLOT ts;
	} tdmrx;
	class CFGTDMTX : public CFGVAL {
	public:
		char *parse(CFG *cfg, char *cmd);
		std::ostream &put(std::ostream &os);
		CFGTIMESLOT ts;
	} tdmtx;
};
int reconfig_rxcodec(CFGRXCODEC *oldcfg, CFGRXCODEC *newcfg, Prosody::Vmprx *vrx);
int reconfig_txcodec(CFGTXCODEC *oldcfg, CFGTXCODEC *newcfg, Prosody::Vmptx *vtx);
