char *CFGRXCODEC::CFGALAW::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if ((s = wprefix(cmd, "plc_mode"))) return plc_mode.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: payload_type plc_mode" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGRXCODEC::CFGALAW::put(std::ostream &os) {
	return os
		<< " payload_type" << payload_type
		<< " plc_mode" << plc_mode
	;
}

char *CFGRXCODEC::CFGAMRNB::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "aligned"))) return aligned.parse(cfg, s);
	if ((s = wprefix(cmd, "gsmefr"))) return gsmefr.parse(cfg, s);
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if ((s = wprefix(cmd, "plc_mode"))) return plc_mode.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: aligned gsmefr payload_type plc_mode" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGRXCODEC::CFGAMRNB::put(std::ostream &os) {
	return os
		<< " aligned" << aligned
		<< " gsmefr" << gsmefr
		<< " payload_type" << payload_type
		<< " plc_mode" << plc_mode
	;
}

char *CFGRXCODEC::CFGCOMFORT_NOISE::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if ((s = wprefix(cmd, "plc_mode"))) return plc_mode.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: payload_type plc_mode" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGRXCODEC::CFGCOMFORT_NOISE::put(std::ostream &os) {
	return os
		<< " payload_type" << payload_type
		<< " plc_mode" << plc_mode
	;
}

char *CFGRXCODEC::CFGEVRC::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if ((s = wprefix(cmd, "plc_mode"))) return plc_mode.parse(cfg, s);
	if ((s = wprefix(cmd, "post_filter"))) return post_filter.parse(cfg, s);
	if ((s = wprefix(cmd, "rtp_mode"))) return rtp_mode.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: payload_type plc_mode post_filter rtp_mode" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGRXCODEC::CFGEVRC::put(std::ostream &os) {
	return os
		<< " payload_type" << payload_type
		<< " plc_mode" << plc_mode
		<< " post_filter" << post_filter
		<< " rtp_mode" << rtp_mode
	;
}

char *CFGRXCODEC::CFGG723_1::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if ((s = wprefix(cmd, "plc_mode"))) return plc_mode.parse(cfg, s);
	if ((s = wprefix(cmd, "post_filter"))) return post_filter.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: payload_type plc_mode post_filter" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGRXCODEC::CFGG723_1::put(std::ostream &os) {
	return os
		<< " payload_type" << payload_type
		<< " plc_mode" << plc_mode
		<< " post_filter" << post_filter
	;
}

char *CFGRXCODEC::CFGG726::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "bits"))) return bits.parse(cfg, s);
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if ((s = wprefix(cmd, "plc_mode"))) return plc_mode.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: bits payload_type plc_mode" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGRXCODEC::CFGG726::put(std::ostream &os) {
	return os
		<< " bits" << bits
		<< " payload_type" << payload_type
		<< " plc_mode" << plc_mode
	;
}

char *CFGRXCODEC::CFGG728::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if ((s = wprefix(cmd, "plc_mode"))) return plc_mode.parse(cfg, s);
	if ((s = wprefix(cmd, "post_filter"))) return post_filter.parse(cfg, s);
	if ((s = wprefix(cmd, "rate"))) return rate.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: payload_type plc_mode post_filter rate" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGRXCODEC::CFGG728::put(std::ostream &os) {
	return os
		<< " payload_type" << payload_type
		<< " plc_mode" << plc_mode
		<< " post_filter" << post_filter
		<< " rate" << rate
	;
}

char *CFGRXCODEC::CFGG729AB::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if ((s = wprefix(cmd, "plc_mode"))) return plc_mode.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: payload_type plc_mode" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGRXCODEC::CFGG729AB::put(std::ostream &os) {
	return os
		<< " payload_type" << payload_type
		<< " plc_mode" << plc_mode
	;
}

char *CFGRXCODEC::CFGG729I::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "g729_mode"))) return g729_mode.parse(cfg, s);
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: g729_mode payload_type" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGRXCODEC::CFGG729I::put(std::ostream &os) {
	return os
		<< " g729_mode" << g729_mode
		<< " payload_type" << payload_type
	;
}

char *CFGRXCODEC::CFGGSMEFR::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if ((s = wprefix(cmd, "plc_mode"))) return plc_mode.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: payload_type plc_mode" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGRXCODEC::CFGGSMEFR::put(std::ostream &os) {
	return os
		<< " payload_type" << payload_type
		<< " plc_mode" << plc_mode
	;
}

char *CFGRXCODEC::CFGGSMFR::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if ((s = wprefix(cmd, "plc_mode"))) return plc_mode.parse(cfg, s);
	if ((s = wprefix(cmd, "variant"))) return variant.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: payload_type plc_mode variant" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGRXCODEC::CFGGSMFR::put(std::ostream &os) {
	return os
		<< " payload_type" << payload_type
		<< " plc_mode" << plc_mode
		<< " variant" << variant
	;
}

char *CFGRXCODEC::CFGILBC::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "enhancer"))) return enhancer.parse(cfg, s);
	if ((s = wprefix(cmd, "frame_len"))) return frame_len.parse(cfg, s);
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if ((s = wprefix(cmd, "plc_mode"))) return plc_mode.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: enhancer frame_len payload_type plc_mode" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGRXCODEC::CFGILBC::put(std::ostream &os) {
	return os
		<< " enhancer" << enhancer
		<< " frame_len" << frame_len
		<< " payload_type" << payload_type
		<< " plc_mode" << plc_mode
	;
}

char *CFGRXCODEC::CFGISAC::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: payload_type" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGRXCODEC::CFGISAC::put(std::ostream &os) {
	return os
		<< " payload_type" << payload_type
	;
}

char *CFGRXCODEC::CFGL16::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if ((s = wprefix(cmd, "plc_mode"))) return plc_mode.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: payload_type plc_mode" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGRXCODEC::CFGL16::put(std::ostream &os) {
	return os
		<< " payload_type" << payload_type
		<< " plc_mode" << plc_mode
	;
}

char *CFGRXCODEC::CFGL8::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if ((s = wprefix(cmd, "plc_mode"))) return plc_mode.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: payload_type plc_mode" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGRXCODEC::CFGL8::put(std::ostream &os) {
	return os
		<< " payload_type" << payload_type
		<< " plc_mode" << plc_mode
	;
}

char *CFGRXCODEC::CFGMELPE::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: payload_type" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGRXCODEC::CFGMELPE::put(std::ostream &os) {
	return os
		<< " payload_type" << payload_type
	;
}

char *CFGRXCODEC::CFGMULAW::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if ((s = wprefix(cmd, "plc_mode"))) return plc_mode.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: payload_type plc_mode" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGRXCODEC::CFGMULAW::put(std::ostream &os) {
	return os
		<< " payload_type" << payload_type
		<< " plc_mode" << plc_mode
	;
}

char *CFGRXCODEC::CFGRFC2833::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if ((s = wprefix(cmd, "plc_mode"))) return plc_mode.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: payload_type plc_mode" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGRXCODEC::CFGRFC2833::put(std::ostream &os) {
	return os
		<< " payload_type" << payload_type
		<< " plc_mode" << plc_mode
	;
}

char *CFGRXCODEC::CFGRFC4040::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: payload_type" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGRXCODEC::CFGRFC4040::put(std::ostream &os) {
	return os
		<< " payload_type" << payload_type
	;
}

char *CFGRXCODEC::CFGSMV::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if ((s = wprefix(cmd, "plc_mode"))) return plc_mode.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: payload_type plc_mode" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGRXCODEC::CFGSMV::put(std::ostream &os) {
	return os
		<< " payload_type" << payload_type
		<< " plc_mode" << plc_mode
	;
}

char *CFGRXCODEC::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "alaw"))) return alaw.parse(cfg, s);
	if ((s = wprefix(cmd, "amrnb"))) return amrnb.parse(cfg, s);
	if ((s = wprefix(cmd, "comfort_noise"))) return comfort_noise.parse(cfg, s);
	if ((s = wprefix(cmd, "evrc"))) return evrc.parse(cfg, s);
	if ((s = wprefix(cmd, "g723_1"))) return g723_1.parse(cfg, s);
	if ((s = wprefix(cmd, "g726"))) return g726.parse(cfg, s);
	if ((s = wprefix(cmd, "g728"))) return g728.parse(cfg, s);
	if ((s = wprefix(cmd, "g729ab"))) return g729ab.parse(cfg, s);
	if ((s = wprefix(cmd, "g729i"))) return g729i.parse(cfg, s);
	if ((s = wprefix(cmd, "gsmefr"))) return gsmefr.parse(cfg, s);
	if ((s = wprefix(cmd, "gsmfr"))) return gsmfr.parse(cfg, s);
	if ((s = wprefix(cmd, "ilbc"))) return ilbc.parse(cfg, s);
	if ((s = wprefix(cmd, "isac"))) return isac.parse(cfg, s);
	if ((s = wprefix(cmd, "l16"))) return l16.parse(cfg, s);
	if ((s = wprefix(cmd, "l8"))) return l8.parse(cfg, s);
	if ((s = wprefix(cmd, "melpe"))) return melpe.parse(cfg, s);
	if ((s = wprefix(cmd, "mulaw"))) return mulaw.parse(cfg, s);
	if ((s = wprefix(cmd, "rfc2833"))) return rfc2833.parse(cfg, s);
	if ((s = wprefix(cmd, "rfc4040"))) return rfc4040.parse(cfg, s);
	if ((s = wprefix(cmd, "smv"))) return smv.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: alaw amrnb comfort_noise evrc g723_1 g726 g728 g729ab g729i gsmefr gsmfr ilbc isac l16 l8 melpe mulaw rfc2833 rfc4040 smv" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGRXCODEC::put(std::ostream &os) {
	return os
		<< " alaw" << alaw
		<< " amrnb" << amrnb
		<< " comfort_noise" << comfort_noise
		<< " evrc" << evrc
		<< " g723_1" << g723_1
		<< " g726" << g726
		<< " g728" << g728
		<< " g729ab" << g729ab
		<< " g729i" << g729i
		<< " gsmefr" << gsmefr
		<< " gsmfr" << gsmfr
		<< " ilbc" << ilbc
		<< " isac" << isac
		<< " l16" << l16
		<< " l8" << l8
		<< " melpe" << melpe
		<< " mulaw" << mulaw
		<< " rfc2833" << rfc2833
		<< " rfc4040" << rfc4040
		<< " smv" << smv
	;
}

char *CFGTXCODEC::CFGALAW::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "VADMode"))) return VADMode.parse(cfg, s);
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if ((s = wprefix(cmd, "ptime"))) return ptime.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: VADMode payload_type ptime" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGTXCODEC::CFGALAW::put(std::ostream &os) {
	return os
		<< " VADMode" << VADMode
		<< " payload_type" << payload_type
		<< " ptime" << ptime
	;
}

char *CFGTXCODEC::CFGAMRNB::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "VADMode"))) return VADMode.parse(cfg, s);
	if ((s = wprefix(cmd, "aligned"))) return aligned.parse(cfg, s);
	if ((s = wprefix(cmd, "frames_per_packet"))) return frames_per_packet.parse(cfg, s);
	if ((s = wprefix(cmd, "gsmefr"))) return gsmefr.parse(cfg, s);
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: VADMode aligned frames_per_packet gsmefr payload_type" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGTXCODEC::CFGAMRNB::put(std::ostream &os) {
	return os
		<< " VADMode" << VADMode
		<< " aligned" << aligned
		<< " frames_per_packet" << frames_per_packet
		<< " gsmefr" << gsmefr
		<< " payload_type" << payload_type
	;
}

char *CFGTXCODEC::CFGCOMFORT_NOISE::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: payload_type" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGTXCODEC::CFGCOMFORT_NOISE::put(std::ostream &os) {
	return os
		<< " payload_type" << payload_type
	;
}

char *CFGTXCODEC::CFGEVRC::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "VADMode"))) return VADMode.parse(cfg, s);
	if ((s = wprefix(cmd, "frames_per_packet"))) return frames_per_packet.parse(cfg, s);
	if ((s = wprefix(cmd, "high_pass_filter"))) return high_pass_filter.parse(cfg, s);
	if ((s = wprefix(cmd, "max_rate"))) return max_rate.parse(cfg, s);
	if ((s = wprefix(cmd, "min_rate"))) return min_rate.parse(cfg, s);
	if ((s = wprefix(cmd, "noise_suppression_filter"))) return noise_suppression_filter.parse(cfg, s);
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if ((s = wprefix(cmd, "rtp_mode"))) return rtp_mode.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: VADMode frames_per_packet high_pass_filter max_rate min_rate noise_suppression_filter payload_type rtp_mode" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGTXCODEC::CFGEVRC::put(std::ostream &os) {
	return os
		<< " VADMode" << VADMode
		<< " frames_per_packet" << frames_per_packet
		<< " high_pass_filter" << high_pass_filter
		<< " max_rate" << max_rate
		<< " min_rate" << min_rate
		<< " noise_suppression_filter" << noise_suppression_filter
		<< " payload_type" << payload_type
		<< " rtp_mode" << rtp_mode
	;
}

char *CFGTXCODEC::CFGG723_1::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "frames_per_packet"))) return frames_per_packet.parse(cfg, s);
	if ((s = wprefix(cmd, "high_pass_filter"))) return high_pass_filter.parse(cfg, s);
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if ((s = wprefix(cmd, "rate"))) return rate.parse(cfg, s);
	if ((s = wprefix(cmd, "silence_compression"))) return silence_compression.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: frames_per_packet high_pass_filter payload_type rate silence_compression" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGTXCODEC::CFGG723_1::put(std::ostream &os) {
	return os
		<< " frames_per_packet" << frames_per_packet
		<< " high_pass_filter" << high_pass_filter
		<< " payload_type" << payload_type
		<< " rate" << rate
		<< " silence_compression" << silence_compression
	;
}

char *CFGTXCODEC::CFGG726::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "VADMode"))) return VADMode.parse(cfg, s);
	if ((s = wprefix(cmd, "bits"))) return bits.parse(cfg, s);
	if ((s = wprefix(cmd, "packetlen"))) return packetlen.parse(cfg, s);
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: VADMode bits packetlen payload_type" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGTXCODEC::CFGG726::put(std::ostream &os) {
	return os
		<< " VADMode" << VADMode
		<< " bits" << bits
		<< " packetlen" << packetlen
		<< " payload_type" << payload_type
	;
}

char *CFGTXCODEC::CFGG728::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "VADMode"))) return VADMode.parse(cfg, s);
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if ((s = wprefix(cmd, "ptime"))) return ptime.parse(cfg, s);
	if ((s = wprefix(cmd, "rate"))) return rate.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: VADMode payload_type ptime rate" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGTXCODEC::CFGG728::put(std::ostream &os) {
	return os
		<< " VADMode" << VADMode
		<< " payload_type" << payload_type
		<< " ptime" << ptime
		<< " rate" << rate
	;
}

char *CFGTXCODEC::CFGG729AB::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "VADMode"))) return VADMode.parse(cfg, s);
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if ((s = wprefix(cmd, "ptime"))) return ptime.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: VADMode payload_type ptime" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGTXCODEC::CFGG729AB::put(std::ostream &os) {
	return os
		<< " VADMode" << VADMode
		<< " payload_type" << payload_type
		<< " ptime" << ptime
	;
}

char *CFGTXCODEC::CFGG729I::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "VADMode"))) return VADMode.parse(cfg, s);
	if ((s = wprefix(cmd, "g729_mode"))) return g729_mode.parse(cfg, s);
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if ((s = wprefix(cmd, "ptime"))) return ptime.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: VADMode g729_mode payload_type ptime" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGTXCODEC::CFGG729I::put(std::ostream &os) {
	return os
		<< " VADMode" << VADMode
		<< " g729_mode" << g729_mode
		<< " payload_type" << payload_type
		<< " ptime" << ptime
	;
}

char *CFGTXCODEC::CFGGSMEFR::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "VADMode"))) return VADMode.parse(cfg, s);
	if ((s = wprefix(cmd, "frames_per_packet"))) return frames_per_packet.parse(cfg, s);
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: VADMode frames_per_packet payload_type" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGTXCODEC::CFGGSMEFR::put(std::ostream &os) {
	return os
		<< " VADMode" << VADMode
		<< " frames_per_packet" << frames_per_packet
		<< " payload_type" << payload_type
	;
}

char *CFGTXCODEC::CFGGSMFR::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "VADMode"))) return VADMode.parse(cfg, s);
	if ((s = wprefix(cmd, "frames_per_packet"))) return frames_per_packet.parse(cfg, s);
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if ((s = wprefix(cmd, "variant"))) return variant.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: VADMode frames_per_packet payload_type variant" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGTXCODEC::CFGGSMFR::put(std::ostream &os) {
	return os
		<< " VADMode" << VADMode
		<< " frames_per_packet" << frames_per_packet
		<< " payload_type" << payload_type
		<< " variant" << variant
	;
}

char *CFGTXCODEC::CFGILBC::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "VADMode"))) return VADMode.parse(cfg, s);
	if ((s = wprefix(cmd, "frame_len"))) return frame_len.parse(cfg, s);
	if ((s = wprefix(cmd, "frames_per_packet"))) return frames_per_packet.parse(cfg, s);
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: VADMode frame_len frames_per_packet payload_type" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGTXCODEC::CFGILBC::put(std::ostream &os) {
	return os
		<< " VADMode" << VADMode
		<< " frame_len" << frame_len
		<< " frames_per_packet" << frames_per_packet
		<< " payload_type" << payload_type
	;
}

char *CFGTXCODEC::CFGISAC::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "channel_associated_mode"))) return channel_associated_mode.parse(cfg, s);
	if ((s = wprefix(cmd, "partner"))) return partner.parse(cfg, s);
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: channel_associated_mode partner payload_type" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGTXCODEC::CFGISAC::put(std::ostream &os) {
	return os
		<< " channel_associated_mode" << channel_associated_mode
		<< " partner" << partner
		<< " payload_type" << payload_type
	;
}

char *CFGTXCODEC::CFGL16::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "VADMode"))) return VADMode.parse(cfg, s);
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if ((s = wprefix(cmd, "ptime"))) return ptime.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: VADMode payload_type ptime" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGTXCODEC::CFGL16::put(std::ostream &os) {
	return os
		<< " VADMode" << VADMode
		<< " payload_type" << payload_type
		<< " ptime" << ptime
	;
}

char *CFGTXCODEC::CFGL8::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "VADMode"))) return VADMode.parse(cfg, s);
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if ((s = wprefix(cmd, "ptime"))) return ptime.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: VADMode payload_type ptime" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGTXCODEC::CFGL8::put(std::ostream &os) {
	return os
		<< " VADMode" << VADMode
		<< " payload_type" << payload_type
		<< " ptime" << ptime
	;
}

char *CFGTXCODEC::CFGMELPE::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if ((s = wprefix(cmd, "ptime"))) return ptime.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: payload_type ptime" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGTXCODEC::CFGMELPE::put(std::ostream &os) {
	return os
		<< " payload_type" << payload_type
		<< " ptime" << ptime
	;
}

char *CFGTXCODEC::CFGMULAW::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "VADMode"))) return VADMode.parse(cfg, s);
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if ((s = wprefix(cmd, "ptime"))) return ptime.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: VADMode payload_type ptime" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGTXCODEC::CFGMULAW::put(std::ostream &os) {
	return os
		<< " VADMode" << VADMode
		<< " payload_type" << payload_type
		<< " ptime" << ptime
	;
}

char *CFGTXCODEC::CFGRFC2833::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: payload_type" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGTXCODEC::CFGRFC2833::put(std::ostream &os) {
	return os
		<< " payload_type" << payload_type
	;
}

char *CFGTXCODEC::CFGRFC4040::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "packetlen"))) return packetlen.parse(cfg, s);
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: packetlen payload_type" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGTXCODEC::CFGRFC4040::put(std::ostream &os) {
	return os
		<< " packetlen" << packetlen
		<< " payload_type" << payload_type
	;
}

char *CFGTXCODEC::CFGSMV::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "VADMode"))) return VADMode.parse(cfg, s);
	if ((s = wprefix(cmd, "frames_per_packet"))) return frames_per_packet.parse(cfg, s);
	if ((s = wprefix(cmd, "payload_type"))) return payload_type.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: VADMode frames_per_packet payload_type" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGTXCODEC::CFGSMV::put(std::ostream &os) {
	return os
		<< " VADMode" << VADMode
		<< " frames_per_packet" << frames_per_packet
		<< " payload_type" << payload_type
	;
}

char *CFGTXCODEC::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "alaw"))) return alaw.parse(cfg, s);
	if ((s = wprefix(cmd, "amrnb"))) return amrnb.parse(cfg, s);
	if ((s = wprefix(cmd, "comfort_noise"))) return comfort_noise.parse(cfg, s);
	if ((s = wprefix(cmd, "evrc"))) return evrc.parse(cfg, s);
	if ((s = wprefix(cmd, "g723_1"))) return g723_1.parse(cfg, s);
	if ((s = wprefix(cmd, "g726"))) return g726.parse(cfg, s);
	if ((s = wprefix(cmd, "g728"))) return g728.parse(cfg, s);
	if ((s = wprefix(cmd, "g729ab"))) return g729ab.parse(cfg, s);
	if ((s = wprefix(cmd, "g729i"))) return g729i.parse(cfg, s);
	if ((s = wprefix(cmd, "gsmefr"))) return gsmefr.parse(cfg, s);
	if ((s = wprefix(cmd, "gsmfr"))) return gsmfr.parse(cfg, s);
	if ((s = wprefix(cmd, "ilbc"))) return ilbc.parse(cfg, s);
	if ((s = wprefix(cmd, "isac"))) return isac.parse(cfg, s);
	if ((s = wprefix(cmd, "l16"))) return l16.parse(cfg, s);
	if ((s = wprefix(cmd, "l8"))) return l8.parse(cfg, s);
	if ((s = wprefix(cmd, "melpe"))) return melpe.parse(cfg, s);
	if ((s = wprefix(cmd, "mulaw"))) return mulaw.parse(cfg, s);
	if ((s = wprefix(cmd, "rfc2833"))) return rfc2833.parse(cfg, s);
	if ((s = wprefix(cmd, "rfc4040"))) return rfc4040.parse(cfg, s);
	if ((s = wprefix(cmd, "smv"))) return smv.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: alaw amrnb comfort_noise evrc g723_1 g726 g728 g729ab g729i gsmefr gsmfr ilbc isac l16 l8 melpe mulaw rfc2833 rfc4040 smv" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFGTXCODEC::put(std::ostream &os) {
	return os
		<< " alaw" << alaw
		<< " amrnb" << amrnb
		<< " comfort_noise" << comfort_noise
		<< " evrc" << evrc
		<< " g723_1" << g723_1
		<< " g726" << g726
		<< " g728" << g728
		<< " g729ab" << g729ab
		<< " g729i" << g729i
		<< " gsmefr" << gsmefr
		<< " gsmfr" << gsmfr
		<< " ilbc" << ilbc
		<< " isac" << isac
		<< " l16" << l16
		<< " l8" << l8
		<< " melpe" << melpe
		<< " mulaw" << mulaw
		<< " rfc2833" << rfc2833
		<< " rfc4040" << rfc4040
		<< " smv" << smv
	;
}

char *CFG::CFGPATH::CFGAGC::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "agc"))) return agc.parse(cfg, s);
	if ((s = wprefix(cmd, "volume"))) return volume.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: agc volume" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFG::CFGPATH::CFGAGC::put(std::ostream &os) {
	return os
		<< " agc" << agc
		<< " volume" << volume
	;
}

char *CFG::CFGPATH::CFGEC::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "delay"))) return delay.parse(cfg, s);
	if ((s = wprefix(cmd, "enable"))) return enable.parse(cfg, s);
	if ((s = wprefix(cmd, "fix_agc"))) return fix_agc.parse(cfg, s);
	if ((s = wprefix(cmd, "non_linear"))) return non_linear.parse(cfg, s);
	if ((s = wprefix(cmd, "ref"))) return ref.parse(cfg, s);
	if ((s = wprefix(cmd, "span"))) return span.parse(cfg, s);
	if ((s = wprefix(cmd, "use_agc"))) return use_agc.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: delay enable fix_agc non_linear ref span use_agc" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFG::CFGPATH::CFGEC::put(std::ostream &os) {
	return os
		<< " delay" << delay
		<< " enable" << enable
		<< " fix_agc" << fix_agc
		<< " non_linear" << non_linear
		<< " ref" << ref
		<< " span" << span
		<< " use_agc" << use_agc
	;
}

char *CFG::CFGPATH::CFGMIX::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "enable"))) return enable.parse(cfg, s);
	if ((s = wprefix(cmd, "mixin"))) return mixin.parse(cfg, s);
	if ((s = wprefix(cmd, "volume"))) return volume.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: enable mixin volume" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFG::CFGPATH::CFGMIX::put(std::ostream &os) {
	return os
		<< " enable" << enable
		<< " mixin" << mixin
		<< " volume" << volume
	;
}

char *CFG::CFGPATH::CFGPITCHSHIFT::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "by"))) return by.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: by" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFG::CFGPATH::CFGPITCHSHIFT::put(std::ostream &os) {
	return os
		<< " by" << by
	;
}

char *CFG::CFGPATH::CFGRESAMPLE::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "downrate"))) return downrate.parse(cfg, s);
	if ((s = wprefix(cmd, "uprate"))) return uprate.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: downrate uprate" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFG::CFGPATH::CFGRESAMPLE::put(std::ostream &os) {
	return os
		<< " downrate" << downrate
		<< " uprate" << uprate
	;
}

char *CFG::CFGPATH::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "agc"))) return agc.parse(cfg, s);
	if ((s = wprefix(cmd, "ec"))) return ec.parse(cfg, s);
	if ((s = wprefix(cmd, "mix"))) return mix.parse(cfg, s);
	if ((s = wprefix(cmd, "pitchshift"))) return pitchshift.parse(cfg, s);
	if ((s = wprefix(cmd, "resample"))) return resample.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: agc ec mix pitchshift resample" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFG::CFGPATH::put(std::ostream &os) {
	return os
		<< " agc" << agc
		<< " ec" << ec
		<< " mix" << mix
		<< " pitchshift" << pitchshift
		<< " resample" << resample
	;
}

char *CFG::CFGPLAY::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "agc"))) return agc.parse(cfg, s);
	if ((s = wprefix(cmd, "file"))) return file.parse(cfg, s);
	if ((s = wprefix(cmd, "format"))) return format.parse(cfg, s);
	if ((s = wprefix(cmd, "once"))) return once.parse(cfg, s);
	if ((s = wprefix(cmd, "outrate"))) return outrate.parse(cfg, s);
	if ((s = wprefix(cmd, "rate"))) return rate.parse(cfg, s);
	if ((s = wprefix(cmd, "speed"))) return speed.parse(cfg, s);
	if ((s = wprefix(cmd, "threshold"))) return threshold.parse(cfg, s);
	if ((s = wprefix(cmd, "volume"))) return volume.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: agc file format once outrate rate speed threshold volume" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFG::CFGPLAY::put(std::ostream &os) {
	return os
		<< " agc" << agc
		<< " file" << file
		<< " format" << format
		<< " once" << once
		<< " outrate" << outrate
		<< " rate" << rate
		<< " speed" << speed
		<< " threshold" << threshold
		<< " volume" << volume
	;
}

char *CFG::CFGREC::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "agc"))) return agc.parse(cfg, s);
	if ((s = wprefix(cmd, "file"))) return file.parse(cfg, s);
	if ((s = wprefix(cmd, "format"))) return format.parse(cfg, s);
	if ((s = wprefix(cmd, "gain"))) return gain.parse(cfg, s);
	if ((s = wprefix(cmd, "rate"))) return rate.parse(cfg, s);
	if ((s = wprefix(cmd, "threshold"))) return threshold.parse(cfg, s);
	if ((s = wprefix(cmd, "volume"))) return volume.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: agc file format gain rate threshold volume" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFG::CFGREC::put(std::ostream &os) {
	return os
		<< " agc" << agc
		<< " file" << file
		<< " format" << format
		<< " gain" << gain
		<< " rate" << rate
		<< " threshold" << threshold
		<< " volume" << volume
	;
}

char *CFG::CFGRTCP::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "bw"))) return bw.parse(cfg, s);
	if ((s = wprefix(cmd, "file"))) return file.parse(cfg, s);
	if ((s = wprefix(cmd, "reports"))) return reports.parse(cfg, s);
	if ((s = wprefix(cmd, "rxbw"))) return rxbw.parse(cfg, s);
	if ((s = wprefix(cmd, "sdes"))) return sdes.parse(cfg, s);
	if ((s = wprefix(cmd, "txbw"))) return txbw.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: bw file reports rxbw sdes txbw" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFG::CFGRTCP::put(std::ostream &os) {
	return os
		<< " bw" << bw
		<< " file" << file
		<< " reports" << reports
		<< " rxbw" << rxbw
		<< " sdes" << sdes
		<< " txbw" << txbw
	;
}

char *CFG::CFGRTPRX::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "codec"))) return codec.parse(cfg, s);
	if ((s = wprefix(cmd, "dataloss"))) return dataloss.parse(cfg, s);
	if ((s = wprefix(cmd, "detect"))) return detect.parse(cfg, s);
	if ((s = wprefix(cmd, "initjitter"))) return initjitter.parse(cfg, s);
	if ((s = wprefix(cmd, "local"))) return local.parse(cfg, s);
	if ((s = wprefix(cmd, "maxjitter"))) return maxjitter.parse(cfg, s);
	if ((s = wprefix(cmd, "rate"))) return rate.parse(cfg, s);
	if ((s = wprefix(cmd, "regen"))) return regen.parse(cfg, s);
	if ((s = wprefix(cmd, "rtcp"))) return rtcp.parse(cfg, s);
	if ((s = wprefix(cmd, "unhandled"))) return unhandled.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: codec dataloss detect initjitter local maxjitter rate regen rtcp unhandled" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFG::CFGRTPRX::put(std::ostream &os) {
	return os
		<< " codec" << codec
		<< " dataloss" << dataloss
		<< " detect" << detect
		<< " initjitter" << initjitter
		<< " local" << local
		<< " maxjitter" << maxjitter
		<< " rate" << rate
		<< " regen" << regen
		<< " rtcp" << rtcp
		<< " unhandled" << unhandled
	;
}

char *CFG::CFGRTPTX::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "codec"))) return codec.parse(cfg, s);
	if ((s = wprefix(cmd, "dest"))) return dest.parse(cfg, s);
	if ((s = wprefix(cmd, "destport"))) return destport.parse(cfg, s);
	if ((s = wprefix(cmd, "isac_ptime"))) return isac_ptime.parse(cfg, s);
	if ((s = wprefix(cmd, "isac_rate"))) return isac_rate.parse(cfg, s);
	if ((s = wprefix(cmd, "rate"))) return rate.parse(cfg, s);
	if ((s = wprefix(cmd, "rtcp"))) return rtcp.parse(cfg, s);
	if ((s = wprefix(cmd, "rtcpdest"))) return rtcpdest.parse(cfg, s);
	if ((s = wprefix(cmd, "rtcpdestport"))) return rtcpdestport.parse(cfg, s);
	if ((s = wprefix(cmd, "rtcpsrc"))) return rtcpsrc.parse(cfg, s);
	if ((s = wprefix(cmd, "rtcpsrcport"))) return rtcpsrcport.parse(cfg, s);
	if ((s = wprefix(cmd, "src"))) return src.parse(cfg, s);
	if ((s = wprefix(cmd, "srcport"))) return srcport.parse(cfg, s);
	if ((s = wprefix(cmd, "tosrtcp"))) return tosrtcp.parse(cfg, s);
	if ((s = wprefix(cmd, "tosrtp"))) return tosrtp.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: codec dest destport isac_ptime isac_rate rate rtcp rtcpdest rtcpdestport rtcpsrc rtcpsrcport src srcport tosrtcp tosrtp" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFG::CFGRTPTX::put(std::ostream &os) {
	return os
		<< " codec" << codec
		<< " dest" << dest
		<< " destport" << destport
		<< " isac_ptime" << isac_ptime
		<< " isac_rate" << isac_rate
		<< " rate" << rate
		<< " rtcp" << rtcp
		<< " rtcpdest" << rtcpdest
		<< " rtcpdestport" << rtcpdestport
		<< " rtcpsrc" << rtcpsrc
		<< " rtcpsrcport" << rtcpsrcport
		<< " src" << src
		<< " srcport" << srcport
		<< " tosrtcp" << tosrtcp
		<< " tosrtp" << tosrtp
	;
}

char *CFG::CFGTDMRX::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "ts"))) return ts.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: ts" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFG::CFGTDMRX::put(std::ostream &os) {
	return os
		<< " ts" << ts
	;
}

char *CFG::CFGTDMTX::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "ts"))) return ts.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: ts" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFG::CFGTDMTX::put(std::ostream &os) {
	return os
		<< " ts" << ts
	;
}

char *CFG::parse(CFG *cfg, char *cmd) {
	char *s;
	if (!*cmd) return cmd;
	if ((s = wprefix(cmd, "card"))) return card.parse(cfg, s);
	if ((s = wprefix(cmd, "mod"))) return mod.parse(cfg, s);
	if ((s = wprefix(cmd, "path"))) return path.parse(cfg, s);
	if ((s = wprefix(cmd, "play"))) return play.parse(cfg, s);
	if ((s = wprefix(cmd, "rec"))) return rec.parse(cfg, s);
	if ((s = wprefix(cmd, "rtcp"))) return rtcp.parse(cfg, s);
	if ((s = wprefix(cmd, "rtprx"))) return rtprx.parse(cfg, s);
	if ((s = wprefix(cmd, "rtptx"))) return rtptx.parse(cfg, s);
	if ((s = wprefix(cmd, "tdmrx"))) return tdmrx.parse(cfg, s);
	if ((s = wprefix(cmd, "tdmtx"))) return tdmtx.parse(cfg, s);
	if (*cmd == '?') {
		std::cerr << "Item names: card mod path play rec rtcp rtprx rtptx tdmrx tdmtx" << std::endl;
		return 0;
	}
	return cmd;
}

std::ostream &CFG::put(std::ostream &os) {
	return os
		<< " card" << card
		<< " mod" << mod
		<< " path" << path
		<< " play" << play
		<< " rec" << rec
		<< " rtcp" << rtcp
		<< " rtprx" << rtprx
		<< " rtptx" << rtptx
		<< " tdmrx" << tdmrx
		<< " tdmtx" << tdmtx
	;
}

int reconfig_rxcodec(CFGRXCODEC *oldcfg, CFGRXCODEC *newcfg, Prosody::Vmprx *vrx)
{
 if (oldcfg->alaw.payload_type != newcfg->alaw.payload_type || oldcfg->alaw.plc_mode != newcfg->alaw.plc_mode) {
	Prosody::Error pe = vrx->config_codec_alaw()
			.payload_type(newcfg->alaw.payload_type)
			.plc_mode(newcfg->alaw.plc_mode);
	if (pe) {
		std::cerr << "sm_rtprx_config_codec_alaw() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->amrnb.aligned != newcfg->amrnb.aligned || oldcfg->amrnb.gsmefr != newcfg->amrnb.gsmefr || oldcfg->amrnb.payload_type != newcfg->amrnb.payload_type || oldcfg->amrnb.plc_mode != newcfg->amrnb.plc_mode) {
	Prosody::Error pe = vrx->config_codec_amrnb()
			.aligned(newcfg->amrnb.aligned)
			.gsmefr(newcfg->amrnb.gsmefr)
			.payload_type(newcfg->amrnb.payload_type)
			.plc_mode(newcfg->amrnb.plc_mode);
	if (pe) {
		std::cerr << "sm_rtprx_config_codec_amrnb() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->comfort_noise.payload_type != newcfg->comfort_noise.payload_type || oldcfg->comfort_noise.plc_mode != newcfg->comfort_noise.plc_mode) {
	Prosody::Error pe = vrx->config_codec_comfort_noise()
			.payload_type(newcfg->comfort_noise.payload_type)
			.plc_mode(newcfg->comfort_noise.plc_mode);
	if (pe) {
		std::cerr << "sm_rtprx_config_codec_comfort_noise() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->evrc.payload_type != newcfg->evrc.payload_type || oldcfg->evrc.plc_mode != newcfg->evrc.plc_mode || oldcfg->evrc.post_filter != newcfg->evrc.post_filter || oldcfg->evrc.rtp_mode != newcfg->evrc.rtp_mode) {
	Prosody::Error pe = vrx->config_codec_evrc()
			.payload_type(newcfg->evrc.payload_type)
			.plc_mode(newcfg->evrc.plc_mode)
			.post_filter(newcfg->evrc.post_filter)
			.rtp_mode(newcfg->evrc.rtp_mode);
	if (pe) {
		std::cerr << "sm_rtprx_config_codec_evrc() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->g723_1.payload_type != newcfg->g723_1.payload_type || oldcfg->g723_1.plc_mode != newcfg->g723_1.plc_mode || oldcfg->g723_1.post_filter != newcfg->g723_1.post_filter) {
	Prosody::Error pe = vrx->config_codec_g723_1()
			.payload_type(newcfg->g723_1.payload_type)
			.plc_mode(newcfg->g723_1.plc_mode)
			.post_filter(newcfg->g723_1.post_filter);
	if (pe) {
		std::cerr << "sm_rtprx_config_codec_g723_1() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->g726.bits != newcfg->g726.bits || oldcfg->g726.payload_type != newcfg->g726.payload_type || oldcfg->g726.plc_mode != newcfg->g726.plc_mode) {
	Prosody::Error pe = vrx->config_codec_g726()
			.bits(newcfg->g726.bits)
			.payload_type(newcfg->g726.payload_type)
			.plc_mode(newcfg->g726.plc_mode);
	if (pe) {
		std::cerr << "sm_rtprx_config_codec_g726() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->g728.payload_type != newcfg->g728.payload_type || oldcfg->g728.plc_mode != newcfg->g728.plc_mode || oldcfg->g728.post_filter != newcfg->g728.post_filter || oldcfg->g728.rate != newcfg->g728.rate) {
	Prosody::Error pe = vrx->config_codec_g728()
			.payload_type(newcfg->g728.payload_type)
			.plc_mode(newcfg->g728.plc_mode)
			.post_filter(newcfg->g728.post_filter)
			.rate(newcfg->g728.rate);
	if (pe) {
		std::cerr << "sm_rtprx_config_codec_g728() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->g729ab.payload_type != newcfg->g729ab.payload_type || oldcfg->g729ab.plc_mode != newcfg->g729ab.plc_mode) {
	Prosody::Error pe = vrx->config_codec_g729ab()
			.payload_type(newcfg->g729ab.payload_type)
			.plc_mode(newcfg->g729ab.plc_mode);
	if (pe) {
		std::cerr << "sm_rtprx_config_codec_g729ab() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->g729i.g729_mode != newcfg->g729i.g729_mode || oldcfg->g729i.payload_type != newcfg->g729i.payload_type) {
	Prosody::Error pe = vrx->config_codec_g729i()
			.g729_mode(newcfg->g729i.g729_mode)
			.payload_type(newcfg->g729i.payload_type);
	if (pe) {
		std::cerr << "sm_rtprx_config_codec_g729i() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->gsmefr.payload_type != newcfg->gsmefr.payload_type || oldcfg->gsmefr.plc_mode != newcfg->gsmefr.plc_mode) {
	Prosody::Error pe = vrx->config_codec_gsmefr()
			.payload_type(newcfg->gsmefr.payload_type)
			.plc_mode(newcfg->gsmefr.plc_mode);
	if (pe) {
		std::cerr << "sm_rtprx_config_codec_gsmefr() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->gsmfr.payload_type != newcfg->gsmfr.payload_type || oldcfg->gsmfr.plc_mode != newcfg->gsmfr.plc_mode || oldcfg->gsmfr.variant != newcfg->gsmfr.variant) {
	Prosody::Error pe = vrx->config_codec_gsmfr()
			.payload_type(newcfg->gsmfr.payload_type)
			.plc_mode(newcfg->gsmfr.plc_mode)
			.variant(newcfg->gsmfr.variant);
	if (pe) {
		std::cerr << "sm_rtprx_config_codec_gsmfr() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->ilbc.enhancer != newcfg->ilbc.enhancer || oldcfg->ilbc.frame_len != newcfg->ilbc.frame_len || oldcfg->ilbc.payload_type != newcfg->ilbc.payload_type || oldcfg->ilbc.plc_mode != newcfg->ilbc.plc_mode) {
	Prosody::Error pe = vrx->config_codec_ilbc()
			.enhancer(newcfg->ilbc.enhancer)
			.frame_len(newcfg->ilbc.frame_len)
			.payload_type(newcfg->ilbc.payload_type)
			.plc_mode(newcfg->ilbc.plc_mode);
	if (pe) {
		std::cerr << "sm_rtprx_config_codec_ilbc() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->isac.payload_type != newcfg->isac.payload_type) {
	Prosody::Error pe = vrx->config_codec_isac()
			.payload_type(newcfg->isac.payload_type);
	if (pe) {
		std::cerr << "sm_rtprx_config_codec_isac() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->l16.payload_type != newcfg->l16.payload_type || oldcfg->l16.plc_mode != newcfg->l16.plc_mode) {
	Prosody::Error pe = vrx->config_codec_l16()
			.payload_type(newcfg->l16.payload_type)
			.plc_mode(newcfg->l16.plc_mode);
	if (pe) {
		std::cerr << "sm_rtprx_config_codec_l16() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->l8.payload_type != newcfg->l8.payload_type || oldcfg->l8.plc_mode != newcfg->l8.plc_mode) {
	Prosody::Error pe = vrx->config_codec_l8()
			.payload_type(newcfg->l8.payload_type)
			.plc_mode(newcfg->l8.plc_mode);
	if (pe) {
		std::cerr << "sm_rtprx_config_codec_l8() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->melpe.payload_type != newcfg->melpe.payload_type) {
	Prosody::Error pe = vrx->config_codec_melpe()
			.payload_type(newcfg->melpe.payload_type);
	if (pe) {
		std::cerr << "sm_rtprx_config_codec_melpe() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->mulaw.payload_type != newcfg->mulaw.payload_type || oldcfg->mulaw.plc_mode != newcfg->mulaw.plc_mode) {
	Prosody::Error pe = vrx->config_codec_mulaw()
			.payload_type(newcfg->mulaw.payload_type)
			.plc_mode(newcfg->mulaw.plc_mode);
	if (pe) {
		std::cerr << "sm_rtprx_config_codec_mulaw() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->rfc2833.payload_type != newcfg->rfc2833.payload_type || oldcfg->rfc2833.plc_mode != newcfg->rfc2833.plc_mode) {
	Prosody::Error pe = vrx->config_codec_rfc2833()
			.payload_type(newcfg->rfc2833.payload_type)
			.plc_mode(newcfg->rfc2833.plc_mode);
	if (pe) {
		std::cerr << "sm_rtprx_config_codec_rfc2833() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->rfc4040.payload_type != newcfg->rfc4040.payload_type) {
	Prosody::Error pe = vrx->config_codec_rfc4040()
			.payload_type(newcfg->rfc4040.payload_type);
	if (pe) {
		std::cerr << "sm_rtprx_config_codec_rfc4040() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->smv.payload_type != newcfg->smv.payload_type || oldcfg->smv.plc_mode != newcfg->smv.plc_mode) {
	Prosody::Error pe = vrx->config_codec_smv()
			.payload_type(newcfg->smv.payload_type)
			.plc_mode(newcfg->smv.plc_mode);
	if (pe) {
		std::cerr << "sm_rtprx_config_codec_smv() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 return 0;
}

int reconfig_txcodec(CFGTXCODEC *oldcfg, CFGTXCODEC *newcfg, Prosody::Vmptx *vtx)
{
 if (oldcfg->alaw.VADMode != newcfg->alaw.VADMode || oldcfg->alaw.payload_type != newcfg->alaw.payload_type || oldcfg->alaw.ptime != newcfg->alaw.ptime) {
	Prosody::Error pe = vtx->config_codec_alaw()
			.VADMode(newcfg->alaw.VADMode)
			.payload_type(newcfg->alaw.payload_type)
			.ptime(newcfg->alaw.ptime);
	if (pe) {
		std::cerr << "sm_rtptx_config_codec_alaw() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->amrnb.VADMode != newcfg->amrnb.VADMode || oldcfg->amrnb.aligned != newcfg->amrnb.aligned || oldcfg->amrnb.frames_per_packet != newcfg->amrnb.frames_per_packet || oldcfg->amrnb.gsmefr != newcfg->amrnb.gsmefr || oldcfg->amrnb.payload_type != newcfg->amrnb.payload_type) {
	Prosody::Error pe = vtx->config_codec_amrnb()
			.VADMode(newcfg->amrnb.VADMode)
			.aligned(newcfg->amrnb.aligned)
			.frames_per_packet(newcfg->amrnb.frames_per_packet)
			.gsmefr(newcfg->amrnb.gsmefr)
			.payload_type(newcfg->amrnb.payload_type);
	if (pe) {
		std::cerr << "sm_rtptx_config_codec_amrnb() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->comfort_noise.payload_type != newcfg->comfort_noise.payload_type) {
	Prosody::Error pe = vtx->config_codec_comfort_noise()
			.payload_type(newcfg->comfort_noise.payload_type);
	if (pe) {
		std::cerr << "sm_rtptx_config_codec_comfort_noise() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->evrc.VADMode != newcfg->evrc.VADMode || oldcfg->evrc.frames_per_packet != newcfg->evrc.frames_per_packet || oldcfg->evrc.high_pass_filter != newcfg->evrc.high_pass_filter || oldcfg->evrc.max_rate != newcfg->evrc.max_rate || oldcfg->evrc.min_rate != newcfg->evrc.min_rate || oldcfg->evrc.noise_suppression_filter != newcfg->evrc.noise_suppression_filter || oldcfg->evrc.payload_type != newcfg->evrc.payload_type || oldcfg->evrc.rtp_mode != newcfg->evrc.rtp_mode) {
	Prosody::Error pe = vtx->config_codec_evrc()
			.VADMode(newcfg->evrc.VADMode)
			.frames_per_packet(newcfg->evrc.frames_per_packet)
			.high_pass_filter(newcfg->evrc.high_pass_filter)
			.max_rate(newcfg->evrc.max_rate)
			.min_rate(newcfg->evrc.min_rate)
			.noise_suppression_filter(newcfg->evrc.noise_suppression_filter)
			.payload_type(newcfg->evrc.payload_type)
			.rtp_mode(newcfg->evrc.rtp_mode);
	if (pe) {
		std::cerr << "sm_rtptx_config_codec_evrc() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->g723_1.frames_per_packet != newcfg->g723_1.frames_per_packet || oldcfg->g723_1.high_pass_filter != newcfg->g723_1.high_pass_filter || oldcfg->g723_1.payload_type != newcfg->g723_1.payload_type || oldcfg->g723_1.rate != newcfg->g723_1.rate || oldcfg->g723_1.silence_compression != newcfg->g723_1.silence_compression) {
	Prosody::Error pe = vtx->config_codec_g723_1()
			.frames_per_packet(newcfg->g723_1.frames_per_packet)
			.high_pass_filter(newcfg->g723_1.high_pass_filter)
			.payload_type(newcfg->g723_1.payload_type)
			.rate(newcfg->g723_1.rate)
			.silence_compression(newcfg->g723_1.silence_compression);
	if (pe) {
		std::cerr << "sm_rtptx_config_codec_g723_1() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->g726.VADMode != newcfg->g726.VADMode || oldcfg->g726.bits != newcfg->g726.bits || oldcfg->g726.packetlen != newcfg->g726.packetlen || oldcfg->g726.payload_type != newcfg->g726.payload_type) {
	Prosody::Error pe = vtx->config_codec_g726()
			.VADMode(newcfg->g726.VADMode)
			.bits(newcfg->g726.bits)
			.packetlen(newcfg->g726.packetlen)
			.payload_type(newcfg->g726.payload_type);
	if (pe) {
		std::cerr << "sm_rtptx_config_codec_g726() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->g728.VADMode != newcfg->g728.VADMode || oldcfg->g728.payload_type != newcfg->g728.payload_type || oldcfg->g728.ptime != newcfg->g728.ptime || oldcfg->g728.rate != newcfg->g728.rate) {
	Prosody::Error pe = vtx->config_codec_g728()
			.VADMode(newcfg->g728.VADMode)
			.payload_type(newcfg->g728.payload_type)
			.ptime(newcfg->g728.ptime)
			.rate(newcfg->g728.rate);
	if (pe) {
		std::cerr << "sm_rtptx_config_codec_g728() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->g729ab.VADMode != newcfg->g729ab.VADMode || oldcfg->g729ab.payload_type != newcfg->g729ab.payload_type || oldcfg->g729ab.ptime != newcfg->g729ab.ptime) {
	Prosody::Error pe = vtx->config_codec_g729ab()
			.VADMode(newcfg->g729ab.VADMode)
			.payload_type(newcfg->g729ab.payload_type)
			.ptime(newcfg->g729ab.ptime);
	if (pe) {
		std::cerr << "sm_rtptx_config_codec_g729ab() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->g729i.VADMode != newcfg->g729i.VADMode || oldcfg->g729i.g729_mode != newcfg->g729i.g729_mode || oldcfg->g729i.payload_type != newcfg->g729i.payload_type || oldcfg->g729i.ptime != newcfg->g729i.ptime) {
	Prosody::Error pe = vtx->config_codec_g729i()
			.VADMode(newcfg->g729i.VADMode)
			.g729_mode(newcfg->g729i.g729_mode)
			.payload_type(newcfg->g729i.payload_type)
			.ptime(newcfg->g729i.ptime);
	if (pe) {
		std::cerr << "sm_rtptx_config_codec_g729i() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->gsmefr.VADMode != newcfg->gsmefr.VADMode || oldcfg->gsmefr.frames_per_packet != newcfg->gsmefr.frames_per_packet || oldcfg->gsmefr.payload_type != newcfg->gsmefr.payload_type) {
	Prosody::Error pe = vtx->config_codec_gsmefr()
			.VADMode(newcfg->gsmefr.VADMode)
			.frames_per_packet(newcfg->gsmefr.frames_per_packet)
			.payload_type(newcfg->gsmefr.payload_type);
	if (pe) {
		std::cerr << "sm_rtptx_config_codec_gsmefr() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->gsmfr.VADMode != newcfg->gsmfr.VADMode || oldcfg->gsmfr.frames_per_packet != newcfg->gsmfr.frames_per_packet || oldcfg->gsmfr.payload_type != newcfg->gsmfr.payload_type || oldcfg->gsmfr.variant != newcfg->gsmfr.variant) {
	Prosody::Error pe = vtx->config_codec_gsmfr()
			.VADMode(newcfg->gsmfr.VADMode)
			.frames_per_packet(newcfg->gsmfr.frames_per_packet)
			.payload_type(newcfg->gsmfr.payload_type)
			.variant(newcfg->gsmfr.variant);
	if (pe) {
		std::cerr << "sm_rtptx_config_codec_gsmfr() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->ilbc.VADMode != newcfg->ilbc.VADMode || oldcfg->ilbc.frame_len != newcfg->ilbc.frame_len || oldcfg->ilbc.frames_per_packet != newcfg->ilbc.frames_per_packet || oldcfg->ilbc.payload_type != newcfg->ilbc.payload_type) {
	Prosody::Error pe = vtx->config_codec_ilbc()
			.VADMode(newcfg->ilbc.VADMode)
			.frame_len(newcfg->ilbc.frame_len)
			.frames_per_packet(newcfg->ilbc.frames_per_packet)
			.payload_type(newcfg->ilbc.payload_type);
	if (pe) {
		std::cerr << "sm_rtptx_config_codec_ilbc() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->isac.channel_associated_mode != newcfg->isac.channel_associated_mode || oldcfg->isac.partner != newcfg->isac.partner || oldcfg->isac.payload_type != newcfg->isac.payload_type) {
	Prosody::Error pe = vtx->config_codec_isac()
			.channel_associated_mode(newcfg->isac.channel_associated_mode)
			.partner(newcfg->isac.partner)
			.payload_type(newcfg->isac.payload_type);
	if (pe) {
		std::cerr << "sm_rtptx_config_codec_isac() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->l16.VADMode != newcfg->l16.VADMode || oldcfg->l16.payload_type != newcfg->l16.payload_type || oldcfg->l16.ptime != newcfg->l16.ptime) {
	Prosody::Error pe = vtx->config_codec_l16()
			.VADMode(newcfg->l16.VADMode)
			.payload_type(newcfg->l16.payload_type)
			.ptime(newcfg->l16.ptime);
	if (pe) {
		std::cerr << "sm_rtptx_config_codec_l16() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->l8.VADMode != newcfg->l8.VADMode || oldcfg->l8.payload_type != newcfg->l8.payload_type || oldcfg->l8.ptime != newcfg->l8.ptime) {
	Prosody::Error pe = vtx->config_codec_l8()
			.VADMode(newcfg->l8.VADMode)
			.payload_type(newcfg->l8.payload_type)
			.ptime(newcfg->l8.ptime);
	if (pe) {
		std::cerr << "sm_rtptx_config_codec_l8() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->melpe.payload_type != newcfg->melpe.payload_type || oldcfg->melpe.ptime != newcfg->melpe.ptime) {
	Prosody::Error pe = vtx->config_codec_melpe()
			.payload_type(newcfg->melpe.payload_type)
			.ptime(newcfg->melpe.ptime);
	if (pe) {
		std::cerr << "sm_rtptx_config_codec_melpe() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->mulaw.VADMode != newcfg->mulaw.VADMode || oldcfg->mulaw.payload_type != newcfg->mulaw.payload_type || oldcfg->mulaw.ptime != newcfg->mulaw.ptime) {
	Prosody::Error pe = vtx->config_codec_mulaw()
			.VADMode(newcfg->mulaw.VADMode)
			.payload_type(newcfg->mulaw.payload_type)
			.ptime(newcfg->mulaw.ptime);
	if (pe) {
		std::cerr << "sm_rtptx_config_codec_mulaw() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->rfc2833.payload_type != newcfg->rfc2833.payload_type) {
	Prosody::Error pe = vtx->config_codec_rfc2833()
			.payload_type(newcfg->rfc2833.payload_type);
	if (pe) {
		std::cerr << "sm_rtptx_config_codec_rfc2833() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->rfc4040.packetlen != newcfg->rfc4040.packetlen || oldcfg->rfc4040.payload_type != newcfg->rfc4040.payload_type) {
	Prosody::Error pe = vtx->config_codec_rfc4040()
			.packetlen(newcfg->rfc4040.packetlen)
			.payload_type(newcfg->rfc4040.payload_type);
	if (pe) {
		std::cerr << "sm_rtptx_config_codec_rfc4040() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (oldcfg->smv.VADMode != newcfg->smv.VADMode || oldcfg->smv.frames_per_packet != newcfg->smv.frames_per_packet || oldcfg->smv.payload_type != newcfg->smv.payload_type) {
	Prosody::Error pe = vtx->config_codec_smv()
			.VADMode(newcfg->smv.VADMode)
			.frames_per_packet(newcfg->smv.frames_per_packet)
			.payload_type(newcfg->smv.payload_type);
	if (pe) {
		std::cerr << "sm_rtptx_config_codec_smv() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 return 0;
}

