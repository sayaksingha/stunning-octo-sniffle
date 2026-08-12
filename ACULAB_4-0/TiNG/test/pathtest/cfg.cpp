#include "cfg.hpp"
#include "error_sm.hpp"
#include "../Testlib/errcode_sm.h"

#include "gen/cfg.cpp"

#ifndef INADDR_NONE
#define INADDR_NONE ((in_addr_t) -1)
#endif

char *CFGADDR::parse(CFG *cfg, char *cmd)
{
 (void) cfg;
 char astr[256];
 cmd = copyword(astr, sizeof(astr), cmd);
 if (!cmd) {
	std::cerr << "Cannot copy IP address\n";
	return 0;
 }
 struct hostent *hp = gethostbyname(astr);
 if (hp) {
 	addr.s_addr = *(unsigned long *) hp->h_addr;
 } else {
 	addr.s_addr = inet_addr(astr);
 	if (addr.s_addr == INADDR_NONE) {
 		std::cerr << "Cannot interpret address: '" << astr << "'\n";
 		return 0;
 	}
 }
 return cmd;
}

char *CFGCARD::cmdcard_x(char *cmd)
{
 Prosody::Card *cp = new Prosody::Card(cards_.size() + 0x1234);
 if (!cp) {
	std::cerr << "new Prosody::Card failed\n";
	return 0;
 }
 char addr[256];
 cmd = copyword(addr, sizeof(addr), cmd);
 if (!cmd) {
	std::cerr << "Cannot copy card IP address\n";
	return 0;
 }
 Prosody::Error e = cp->open_prosody_x()
	.px_locator(addr);
 if (e) {
	std::cerr << error_sm(e);
	std::cerr << "Cannot open Prosody X card '" << addr << "'\n";
	delete cp;
	return 0;
 }
 cards_.push_back(cp);
 std::string s("x:");
 s += addr;
 names_.push_back(s);
 return cmd;
}


char *CFGCARD::parse(CFG *cfg, char *cmd)
{
 (void) cfg;
 if (*cmd == '?') {
 	std::cerr << "Usage: card +cardspec | card N\n"
 		"where cardspec is one of:\n"
 		"+x:Prosody_X_IP_address\n";
	return 0;
 }
 if (*cmd != '+') {
 	unsigned long v = strtoul(cmd, &cmd, 0);
 	if (v >= cards_.size()) {
 		std::cerr << "No such card: " << v
 			<< ", cards are [0.." << cards_.size() << ")\n";
		return 0;
	}
	curcard_ = v;
	return cmd;
 }
 if (!*++cmd || cmd[1] != ':') {
 	std::cerr << "Error: expected card spec c:..., not '" << cmd << "'\n";
 	return 0;
 }
 if (*cmd == 'x') return cmdcard_x(cmd + 2);
 std::cerr << "Unknown card type: '" << *cmd << "'\n";
 std::cerr << "Valid types: x s p\n";
 return 0;
}

char *CFGCODEC::parse(CFG *cfg, char *cmd)
{
 (void) cfg;
 Prosody::kSMCodecType c;
 char *s;
 for (;;) {
	if ((s = prefix(cmd, "a"))) {
		c = Prosody::kSMCodecTypeAlaw;
	} else if ((s = prefix(cmd, "u"))) {
		c = Prosody::kSMCodecTypeMulaw;
	} else if ((s = prefix(cmd, "729ab"))) {
		c = Prosody::kSMCodecTypeG729AB;
	} else if ((s = prefix(cmd, "t"))) {
		c = Prosody::kSMCodecTypeRFC2833;
	} else if (*cmd == '?') {
		std::cerr << "Codec types: a u 729ab t\n";
		return 0;
	} else return cmd;
	if (*s == '=') {
		unsigned long v = strtoul(s + 1, &cmd, 0);
		if (*cmd != ';') {
			std::cerr << "Expected ';' after codec number, got '"
				<< cmd << "'\n";
			return 0;
		}
		codecs[c] = v;
	} else {
		if (*cmd != ';') {
			std::cerr << "Expected ';' or '=N' after codec type, got '"
				<< cmd << "'\n";
			return 0;
		}
		codecs.erase(c);
	}
	cmd++;
 }
}

char *CFGMOD::parse(CFG *cfg, char *cmd)
{
 if (*cmd != '+') {
 	unsigned long v = strtoul(cmd, &cmd, 0);
 	if (v >= mods_.size()) {
 		std::cerr << "No such module: " << v
 			<< ", modules are [0.." << mods_.size() << ")\n";
		return 0;
	}
	curmod_ = v;
	return cmd;
 }
 Prosody::Card *cardp = cfg->card;
 if (!cardp) return 0;
 unsigned long v = strtoul(cmd + 1, &cmd, 0);
 Prosody::Module *modp = new Prosody::Module();
 if (!modp) {
	std::cerr << "new Prosody::Module failed\n";
	return 0;
 }
 Prosody::Error e = modp->open_module()
 	.card_id(*cardp)
 	.module_ix(v);
 if (e) {
	std::cerr << error_sm(e);
	std::cerr << "Cannot open Prosody module '" << v << "'\n";
	delete modp;
	return 0;
 }
 char num[40];
 sprintf(num, "[%lu]", v);
 std::string name = cfg->card.name() + num;
 mods_.push_back(modp);
 names_.push_back(name);
 return cmd;
}

// parses strings: [N=]"..." { , [N=]"..."}*
char *CFGSVEC::parse(CFG *cfg, char *cmd)
{
 unsigned pos = 0;
 (void) cfg;
 for (;;) {
	char *sp;
	unsigned long v = strtoul(cmd, &sp, 0);
	if (*sp == '=') {
		pos = v;
		cmd = sp + 1;
	}
	if (isspace(*cmd)) return cmd;
	std::string str;
	cmd = rdstring(&str, cmd);
	if (!cmd) return 0;
	char *c_str = strdup(str.c_str());
	while (pos >= slist.size()) slist.push_back(0);
	slist[pos] = c_str;
	if (*cmd != ',') return cmd;
	cmd++;
	pos++;
 }
}
