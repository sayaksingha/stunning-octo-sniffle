#ifndef INCLUDED_CFG_HPP
#define INCLUDED_CFG_HPP

#ifdef TiNGTYPE_LINUX
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

#ifdef TiNGTYPE_QNX
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif


#ifdef TiNGTYPE_WINNT
#include "../../libutil/WINNT/wind.h"
#endif

#include <iostream>
#include <map>
#include <vector>

#include "cmdutil.hpp"
#include "format.hpp"
#include "../../class/prosody.hpp"
#include "activereplay.hpp"

	// a (complete) configuration
	// this is automatically generated
class CFG;

// the following are in alphabetical order - easier to find stuff!

	// anything configurable
class CFGVAL {
public:
	/* given a pointer to a textual description of a value, parse it
	 * if valid, set the value and return a pointer to after the description
	 * if invalid, return 0
	 */
	virtual char *parse(CFG *cfg, char *cmd) = 0;
	/* write out a textual representation of the value
	 */
	virtual std::ostream &put(std::ostream &os) = 0;
	virtual ~CFGVAL() { }
};

	// for convenience
inline std::ostream &operator<<(std::ostream &os, CFGVAL &val) {
	return val.put(os);
}

	// an IP address
class CFGADDR : public CFGVAL {
	struct in_addr addr;
public:
	CFGADDR() {
		memset(&addr, 0, sizeof(addr));
	}
	operator in_addr() {
		return addr;
	}
	char *parse(CFG *cfg, char *cmd);
	std::ostream &put(std::ostream &os) {
		return os << " " << inet_ntoa(addr);
	}
};

	// a Prosody card
class CFGCARD : public CFGVAL {
	unsigned curcard_;
	std::vector<Prosody::Card *> cards_;
	std::vector<std::string> names_;
public:
	CFGCARD() : curcard_(0) { }
	operator Prosody::Card *() {
		if (cards_.size() <= curcard_) {
			std::cerr << "No Prosody card open\n";
			return 0;
		}
		return cards_[curcard_];
	}
	std::string name() {
		return names_[curcard_];
	}
	char *parse(CFG *cfg, char *cmd);
	std::ostream &put(std::ostream &os) {
		unsigned u;
		os << " " << curcard_ << " # from";
		for (u=0; u < cards_.size(); u++) {
			os << " " << u << "=" << names_[u];
		}
		return os;
	}
	~CFGCARD() {
		for (int i=cards_.size(); i--; ) {
			Prosody::Card *cp = cards_[i];
			Prosody::Error e = cp->close_prosody();
			e.ok();
			delete cp;
		}
	}
private:
	char *cmdcard_pci(char *cmd);
	char *cmdcard_s(char *cmd);
	char *cmdcard_x(char *cmd);
};

	// a codec (as used by sm_vmp[rt]x_config_codec(), which is obsolete)
class CFGCODEC : public CFGVAL {
public:
	typedef std::map<Prosody::kSMCodecType,int> MAP;
	MAP codecs;
	CFGCODEC() { }
	char *parse(CFG *cfg, char *cmd);
	std::ostream &put(std::ostream &os) {
		MAP::const_iterator i = codecs.begin();
		for (; i != codecs.end(); i++) {
			os << " " << i->first << "=" << i->second << ";";
		}
		return os;
	}
};

	// the EVRC rate
class CFGEVRCRATE : public CFGVAL {
	Prosody::kSMEVRCRate val;
public:
	CFGEVRCRATE() : val(Prosody::kSMEVRCRateFull) { }
	operator Prosody::kSMEVRCRate() {
		return val;
	}
	char *parse(CFG *cfg, char *cmd) {
		(void) cfg;
		switch (*cmd) {
		case '1': val = Prosody::kSMEVRCRateFull; return cmd+1;
		case '2': val = Prosody::kSMEVRCRateHalf; return cmd+1;
		case '8': val = Prosody::kSMEVRCRateEighth; return cmd+1;
		}
		std::cerr << "Expected '1', '2', or '8'; got '"
				<< cmd << "'\n";
		return 0;
	}
	std::ostream &put(std::ostream &os) {
		switch (val) {
		case Prosody::kSMEVRCRateFull: return os << " 1";
		case Prosody::kSMEVRCRateHalf: return os << " 2";
		case Prosody::kSMEVRCRateEighth: return os << " 8";
		}
		return os << " (?err)";
	}
};

	// the EVRC RTP mode
class CFGEVRCRTPMODE : public CFGVAL {
	Prosody::kSMEVRCRTPMode val;
public:
	CFGEVRCRTPMODE() : val(Prosody::kSMEVRCRTPModeHeaderless) { }
	operator Prosody::kSMEVRCRTPMode() {
		return val;
	}
	char *parse(CFG *cfg, char *cmd) {
		(void) cfg;
		switch (*cmd) {
		case 'h': val = Prosody::kSMEVRCRTPModeHeaderless; return cmd+1;
		case '2': val = Prosody::kSMEVRCRTPModeRFC2658; return cmd+1;
		case '3': val = Prosody::kSMEVRCRTPModeRFC3558; return cmd+1;
		}
		std::cerr << "Expected 'h', '2', or '8'; got '"
				<< cmd << "'\n";
		return 0;
	}
	std::ostream &put(std::ostream &os) {
		switch (val) {
		case Prosody::kSMEVRCRTPModeHeaderless: return os << " h";
		case Prosody::kSMEVRCRTPModeRFC2658: return os << " 2";
		case Prosody::kSMEVRCRTPModeRFC3558: return os << " 3";
		}
		return os << " (?err)";
	}
};

	// an arbitrary floating point value
class CFGFLOAT : public CFGVAL {
	float value;
public:
	CFGFLOAT() : value(0) { }
	operator float() {
		return value;
	}
	char *parse(CFG *cfg, char *cmd) {
		(void) cfg;
		value = strtod(cmd, &cmd);
		return cmd;
	}
	std::ostream &put(std::ostream &os) {
		return os << " " << value;
	}
};

	// a (named) data format as used by play and record
class CFGFORMAT : public CFGVAL {
	Prosody::kSMDataFormat fmt;
public:
	CFGFORMAT() : fmt(Prosody::kSMDataFormatALawPCM) { }
	operator Prosody::kSMDataFormat() {
		return fmt;
	}
	char *parse(CFG *cfg, char *cmd) {
		(void) cfg;
		return parseformat(&fmt, cmd);
	}
	std::ostream &put(std::ostream &os) {
		const char *fn = formatname(fmt);
		if (fn) {
			return os << " " << fn;
		} else {
			return os << " #" << fmt;
		}
	}
};

	// the G.729 mode
class CFGG729MODE : public CFGVAL {
	Prosody::kSMG729IMode val;
public:
	CFGG729MODE() : val(Prosody::kSMG729IModeG729D) { }
	operator Prosody::kSMG729IMode() {
		return val;
	}
	char *parse(CFG *cfg, char *cmd) {
		(void) cfg;
		switch (*cmd) {
		case 'd': val = Prosody::kSMG729IModeG729D; return cmd+1;
		case 's': val = Prosody::kSMG729IModeG729; return cmd+1;
		case 'e': val = Prosody::kSMG729IModeG729E; return cmd+1;
		}
		std::cerr << "Expected 'd', 'e', or '-'; got '"
				<< cmd << "'\n";
		return 0;
	}
	std::ostream &put(std::ostream &os) {
		switch (val) {
		case Prosody::kSMG729IModeG729D: return os << " d";
		case Prosody::kSMG729IModeG729: return os << " -";
		case Prosody::kSMG729IModeG729E: return os << " e";
		}
		return os << " (?err)";
	}
};

	// the GSM full-rate variant
class CFGGSMFR_VARIANT : public CFGVAL {
	Prosody::kSMGSMVariant val;
public:
	CFGGSMFR_VARIANT() : val(Prosody::kSMGSMVariantStandard) { }
	operator Prosody::kSMGSMVariant() {
		return val;
	}
	char *parse(CFG *cfg, char *cmd) {
		(void) cfg;
		switch (*cmd) {
		case 's': val = Prosody::kSMGSMVariantStandard; return cmd+1;
		case 'm': val = Prosody::kSMGSMVariantMS; return cmd+1;
		}
		std::cerr << "Expected 's' or 'm'; got '"
				<< cmd << "'\n";
		return 0;
	}
	std::ostream &put(std::ostream &os) {
		switch (val) {
		case Prosody::kSMGSMVariantStandard: return os << " s";
		case Prosody::kSMGSMVariantMS: return os << " m";
		}
		return os << " (?err)";
	}
};

	// an ordinary integer
class CFGINT : public CFGVAL {
	int value;
public:
	CFGINT() : value(0) { }
	operator int() {
		return value;
	}
	char *parse(CFG *cfg, char *cmd) {
		(void) cfg;
		value = strtoul(cmd, &cmd, 0);
		return cmd;
	}
	std::ostream &put(std::ostream &os) {
		return os << " " << value;
	}
};

	// a Prosody module
class CFGMOD : public CFGVAL {
	unsigned curmod_;
	std::vector<Prosody::Module *> mods_;
	std::vector<std::string> names_;
public:
	CFGMOD() : curmod_(0) { }
	operator Prosody::Module *() {
		if (mods_.size() <= curmod_) {
			std::cerr << "No Prosody module open\n";
			return 0;
		}
		return mods_[curmod_];
	}
	char *parse(CFG *cfg, char *cmd);
	std::ostream &put(std::ostream &os) {
		unsigned u;
		os << " " << curmod_ << " # from";
		for (u=0; u < mods_.size(); u++) {
			os << " " << u << "=" << names_[u];
		}
		return os;
	}
	~CFGMOD() {
		for (int i=mods_.size(); i--; ) {
			Prosody::Module *mp = mods_[i];
			Prosody::Error e = mp->close_module();
			e.ok();
			delete mp;
		}
	}
};

	// a replay repetition code: once, rewind, or repeat
class CFGONCE : public CFGVAL {
	ActiveReplay::Once once;
public:
	CFGONCE() : once(ActiveReplay::ONCE_NOT) { }
	operator ActiveReplay::Once() {
		return once;
	}
	char *parse(CFG *cfg, char *cmd) {
		(void) cfg;
		switch (*cmd) {
		case 'c': once = ActiveReplay::ONCE_NOT; return cmd+1;
		case 'o': once = ActiveReplay::ONCE_ONLY; return cmd+1;
		case 'r': once = ActiveReplay::ONCE_REPEAT; return cmd+1;
		}
		std::cerr << "Expected 'c', 'o', or 'r'; got '"
				<< cmd << "'\n";
		return 0;
	}
	std::ostream &put(std::ostream &os) {
		switch (once) {
		case ActiveReplay::ONCE_NOT: return os << " c";
		case ActiveReplay::ONCE_ONLY: return os << " o";
		case ActiveReplay::ONCE_REPEAT: return os << " r";
		}
		return os << " (?err)";
	}
};

	// a payload type - just an integer that defaults to -1 (not used)
class CFGPAYLOAD : public CFGVAL {
	int value;
public:
	CFGPAYLOAD() : value(-1) { }
	operator int() {
		return value;
	}
	char *parse(CFG *cfg, char *cmd) {
		(void) cfg;
		value = strtoul(cmd, &cmd, 0);
		return cmd;
	}
	std::ostream &put(std::ostream &os) {
		return os << " " << value;
	}
};

	// a vmprx PLC configuration (i.e. on or off)
class CFGPLCMODE : public CFGVAL {
	Prosody::kSMPLCMode val;
public:
	CFGPLCMODE() : val(Prosody::kSMPLCModeDisabled) { }
	operator Prosody::kSMPLCMode() {
		return val;
	}
	char *parse(CFG *cfg, char *cmd) {
		(void) cfg;
		switch (*cmd) {
		case '0': val = Prosody::kSMPLCModeDisabled; return cmd+1;
		case '1': val = Prosody::kSMPLCModeEnabled; return cmd+1;
		}
		std::cerr << "Expected '0' or '1'; got '"
				<< cmd << "'\n";
		return 0;
	}
	std::ostream &put(std::ostream &os) {
		switch (val) {
		case Prosody::kSMPLCModeDisabled: return os << " 0";
		case Prosody::kSMPLCModeEnabled: return os << " 1";
		}
		return os << " (?err)";
	}
};

	// an IP port number
class CFGPORT : public CFGVAL {
	int port;
public:
	CFGPORT() : port(0) { }
	operator short() {
		return port;
	}
	char *parse(CFG *cfg, char *cmd) {
		(void) cfg;
		port = htons(strtoul(cmd, &cmd, 0));
		return cmd;
	}
	std::ostream &put(std::ostream &os) {
		return os << " " << ntohs(port);
	}
};

	// a source of data suitable for a pipeline
class CFGSOURCE : public CFGVAL {
public:
	char *type;
	int pos;
	CFGSOURCE() : type(0), pos(0) { }
	char *parse(CFG *cfg, char *cmd) {
		(void) cfg;
		char *t;
		char *s;
		if ((s = prefix(cmd, t = "play")));
		else if ((s = prefix(cmd, t = "rec")));
		else if ((s = prefix(cmd, t = "tdmrx")));
		else if ((s = prefix(cmd, t = "tdmtx")));
		else {
			std::cerr << "Expected item type: 'play', 'rec', 'tdmrx', or 'tdmtx', got '"
				<< cmd << "'\n";
			return 0;
		}
		if (*s != '[') {
			std::cerr << "Expected '[' after item type, got '"
				<< s << "'\n";
			return 0;
		}
		unsigned v = strtoul(s + 1, &cmd, 0);
		if (*cmd != ']') {
			std::cerr << "Expected ']' after item position, got '"
				<< cmd << "'\n";
			return 0;
		}
		pos = v;
		type = t;
		return cmd + 1;
	}
	std::ostream &put(std::ostream &os) {
		if (type) {
			return os << " " << type << "[" << pos << "]";
		} else {
			return os << " none";
		}
	}
};

	// a string
class CFGSTRING : public CFGVAL {
	std::string value;
public:
	CFGSTRING() { }
	operator const char *() {
		return value.c_str();
	}
	char *parse(CFG *cfg, char *cmd) {
		(void) cfg;
		return rdstring(&value, cmd);
	}
	std::ostream &put(std::ostream &os) {
		os << " \"";
		for (unsigned i = 0; i < value.size(); i++) {
			char c = value[i];
			if (c == '\\') os << "\\\\";
			else if (c < ' ' || c >= 127) {
				os << "\\"
					<< ((c >> 6) & 7)
					<< ((c >> 3) & 7)
					<< (c & 7);
			} else os << c;
		}
		os << "\"";
		return os;
	}
};

	// a list of strings
class CFGSVEC : public CFGVAL {
	std::vector<char *> slist;
public:
	CFGSVEC() { }
	std::vector<char *> *val() { return &slist; };
	char *parse(CFG *cfg, char *cmd);
	std::ostream &put(std::ostream &os) {
		unsigned next = 0;
		unsigned any = 0;
		os << " ";
		for (unsigned i = 0; i < slist.size(); i++) {
			char *s = slist[i];
			if (s) {
				if (any) os << ",";
				if (next != i) os << i << "=";
				next = i + 1;
				os << "\"";
				for (;;) {
					char c = *s++;
					if (!c) break;
					if (c == '\\') os << "\\\\";
					else if (c < ' ' || c >= 127) {
						os << "\\"
							<< ((c >> 6) & 7)
							<< ((c >> 3) & 7)
							<< (c & 7);
					} else os << c;
				}
				os << "\"";
				any = 1;
			}
		}
		return os;
	}
	~CFGSVEC() {
		for (unsigned i = 0; i < slist.size(); i++) {
			free(slist[i]);
		}
	}
};

	// a TDM timeslot
class CFGTIMESLOT : public CFGVAL {
public:
	unsigned stream;
	unsigned timeslot;
	char format;
	CFGTIMESLOT() : stream(0), timeslot(0), format('a') { }
	char *parse(CFG *cfg, char *cmd) {
		(void) cfg;
		unsigned s = strtoul(cmd, &cmd, 0);
		if (*cmd != ':') {
			std::cerr << "Expected ':' after stream, got '"
				<< cmd << "'\n";
			return 0;
		}
		unsigned t = strtoul(cmd + 1, &cmd, 0);
		format = 'a';
		if (*cmd == ':') {
			switch (*++cmd) {
			case 'a': case 'u': case 'd': format = *cmd; break;
			default:
				std::cerr << "Expected timeslot type (a, u, d), got '" << *cmd << "'\n";
				return 0;
			}
			cmd++;
		}
		stream = s;
		timeslot = t;
		return cmd;
	}
	std::ostream &put(std::ostream &os) {
		return os << " " << stream
			<< ":" << timeslot
			<< ":" << format;
	}
};

	// the VAD mode
class CFGVADMODE : public CFGVAL {
	Prosody::kSMVMPTxVADMode val;
public:
	CFGVADMODE() : val(Prosody::kSMVMPTxVADModeDisabled) { }
	operator Prosody::kSMVMPTxVADMode() {
		return val;
	}
	char *parse(CFG *cfg, char *cmd) {
		(void) cfg;
		switch (*cmd) {
		case 'd': val = Prosody::kSMVMPTxVADModeDisabled; return cmd+1;
		case 'e': val = Prosody::kSMVMPTxVADModeEnabled; return cmd+1;
		case 'c': val = Prosody::kSMVMPTxVADModeComfortNoise; return cmd+1;
		}
		std::cerr << "Expected 'd', 'e', or 'c'; got '"
				<< cmd << "'\n";
		return 0;
	}
	std::ostream &put(std::ostream &os) {
		switch (val) {
		case Prosody::kSMVMPTxVADModeDisabled: return os << " d";
		case Prosody::kSMVMPTxVADModeEnabled: return os << " e";
		case Prosody::kSMVMPTxVADModeComfortNoise: return os << " c";
		}
		return os << " (?err)";
	}
};

	// an optional vmprx
class CFGVMPRX : public CFGVAL {
	unsigned value;
	Prosody::tSMVMPrxId vmprx_;
public:
	CFGVMPRX() : value(~0u), vmprx_(0) { }
	operator Prosody::tSMVMPrxId() {
		if (value == ~0u) return 0;
		else if (!vmprx_) abort();
		return vmprx_;
	}
	operator unsigned() {
		return value;
	}
	bool operator !=(const CFGVMPRX &rhs) const {
		return value != rhs.value;
	}
	int update_partner(class RUNSTATE *runs);
	char *parse(CFG *cfg, char *cmd) {
		(void) cfg;
		if (*cmd == '-') {
			value = ~0u;
			return cmd + 1;
		} else value = strtoul(cmd, &cmd, 0);
		return cmd;
	}
	std::ostream &put(std::ostream &os) {
		if (value == ~0u) {
			return os << " -";
		} else {
			return os << " " << value;
		}
	}
};

	// an optional vmptx
class CFGVMPTX : public CFGVAL {
	unsigned value;
	Prosody::tSMVMPtxId vmptx_;
public:
	CFGVMPTX() : value(~0u), vmptx_(0) { }
	operator Prosody::tSMVMPtxId() {
		if (value == ~0u) return 0;
		else if (!vmptx_) abort();
		return vmptx_;
	}
	operator unsigned() {
		return value;
	}
	bool operator !=(const CFGVMPTX &rhs) const {
		return value != rhs.value;
	}
	int update_partner(class RUNSTATE *runs);
	char *parse(CFG *cfg, char *cmd) {
		(void) cfg;
		if (*cmd == '-') {
			value = ~0u;
			return cmd + 1;
		} else value = strtoul(cmd, &cmd, 0);
		return cmd;
	}
	std::ostream &put(std::ostream &os) {
		if (value == ~0u) {
			return os << " -";
		} else {
			return os << " " << value;
		}
	}
};

	// an optional integer - value ~0u if not specified
class CFGZINT : public CFGVAL {
	unsigned value;
public:
	CFGZINT() : value(~0u) { }
	operator unsigned() {
		return value;
	}
	char *parse(CFG *cfg, char *cmd) {
		(void) cfg;
		if (*cmd == '-') {
			value = ~0u;
			return cmd + 1;
		} else value = strtoul(cmd, &cmd, 0);
		return cmd;
	}
	std::ostream &put(std::ostream &os) {
		if (value == ~0u) {
			return os << " -";
		} else {
			return os << " " << value;
		}
	}
};

#include "gen/cfg.hpp"

#endif
