#include "parsecmd.hpp"
#include "cmdutil.hpp"
#include "error_sm.hpp"
#include <iostream>
#include <new>

static char *wlookup(CFGVAL **cfgp, RUNNABLE **runp, char *cmd, CFG *cfg, RUNSTATE *runs)
{
 char *s;
 s = prefix(cmd, runs->card.name());
 if (s) {
 	*cfgp = &cfg->card;
 	*runp = &runs->card;
 	return s;
 }
 s = prefix(cmd, runs->mod.name());
 if (s) {
 	*cfgp = &cfg->mod;
 	*runp = &runs->mod;
 	return s;
 }
 s = prefix(cmd, runs->path.name());
 if (s) {
 	*cfgp = &cfg->path;
 	*runp = &runs->path;
 	return s;
 }
 s = prefix(cmd, runs->play.name());
 if (s) {
 	*cfgp = &cfg->play;
 	*runp = &runs->play;
 	return s;
 }
 s = prefix(cmd, runs->rec.name());
 if (s) {
 	*cfgp = &cfg->rec;
 	*runp = &runs->rec;
 	return s;
 }
 s = prefix(cmd, runs->rtcp.name());
 if (s) {
 	*cfgp = &cfg->rtcp;
 	*runp = &runs->rtcp;
 	return s;
 }
 s = prefix(cmd, runs->rtprx.name());
 if (s) {
 	*cfgp = &cfg->rtprx;
 	*runp = &runs->rtprx;
 	return s;
 }
 s = prefix(cmd, runs->rtptx.name());
 if (s) {
 	*cfgp = &cfg->rtptx;
 	*runp = &runs->rtptx;
 	return s;
 }
 s = prefix(cmd, runs->tdmrx.name());
 if (s) {
 	*cfgp = &cfg->tdmrx;
 	*runp = &runs->tdmrx;
 	return s;
 }
 s = prefix(cmd, runs->tdmtx.name());
 if (s) {
 	*cfgp = &cfg->tdmtx;
 	*runp = &runs->tdmtx;
 	return s;
 }
 std::cerr << "Unknown command: '" << cmd << "'\n";
 return 0;
}

static int adjstop(CFG *cfg, RUNSTATE *runs, char *cmd, int isadj)
{
 CFGVAL *cp;
 RUNNABLE *rp;
 cmd = wlookup(&cp, &rp, skipspc(cmd), cfg, runs);
 if (!cmd) return 1;
 if (*cmd != '[') {
	std::cerr << "Expected '[range]' for "
		<< (isadj ? "adjust" : "stop") << " command, got '"
		<< cmd << "'\n";
	return 1;
 }
 unsigned long n = strtoul(cmd + 1, &cmd, 0);
 unsigned long x = n;
 if (*cmd == ':') {
	x = strtoul(cmd + 1, &cmd, 0);
 }
 if (*cmd != ']') {
	std::cerr << "Expected ']' after range for "
		<< (isadj ? "adjust" : "stop") << " command, got '"
		<< cmd << "'\n";
	return 1;
 }
 cmd = cp->parse(cfg, skipspc(cmd + 1));
 if (!cmd) return 1;
 if (*cmd) {
	std::cerr << "Junk after "
		<< (isadj ? "adjust" : "stop")
		<< " command: '" << cmd << "'\n";
	return 1;
 }
 for (; n <= x; n++) {
 	RUNONE *ro = rp->nth(runs, n);
 	if (ro) {
 		if (isadj) {
			if (ro->adjust(cfg, runs)) return 1;
		} else {
			if (ro->halt(cfg, runs)) return 1;
		}
 	}
 }
 return 0;
}

int parsecmd(CFG *cfg, RUNSTATE *runs, char *cmd)
{
 cmd = skipspc(cmd);
 int run = 0;
 int show = 0;
 if (!*cmd) {
 	std::cout << "P:";
 	runs->play.status(std::cout);
 	std::cout << "R:";
 	runs->rec.status(std::cout);
 	std::cout << "H:";
 	runs->path.status(std::cout);
 	std::cout << "C:";
 	runs->rtcp.status(std::cout);
 	std::cout << "Rr:";
 	runs->rtprx.status(std::cout);
 	std::cout << "Rt:";
 	runs->rtptx.status(std::cout);
 	std::cout << "Tr:";
 	runs->tdmrx.status(std::cout);
 	std::cout << "Tt:";
 	runs->tdmtx.status(std::cout);
 	std::cout << "\n";
 	std::cout.flush();
 	return 0;
 }
 if (*cmd == '#') return 0;	// comments
 if (*cmd == '?') {
 	cmd = skipspc(cmd + 1);
 	if (!*cmd) {
		std::cout << "Commands:\n"
			"\t?CMD\tcurrent settings for CMD\n"
			"\t+CMD | CMD ...\tstart pipeline\n"
			"\t=CMD\tadjust item\n"
			"\t-CMD\tstop item\n"
			"\tCR\tshow status\n"
			"\tcard\n"
			"\tmod\n"
			"\tpath\n"
			"\tplay\n"
			"\trtcp\n"
			"\trtprx\n"
			"\trtptx\n"
			"\ttdmrx\n"
			"\ttdmtx\n";
		return 0;
	}
	show = 1;
 } else if (*cmd == '+') {
 	cmd = skipspc(cmd + 1);
 	run = 1;
 } else if (*cmd == '-') {
 	return adjstop(cfg, runs, cmd + 1, 0);
 } else if (*cmd == '=') {
 	return adjstop(cfg, runs, cmd + 1, 1);
 }
 Prosody::tSMDatafeedId df = 0;
 std::vector<RUNNABLE *> runlist;
 for (;;) {
	CFGVAL *cp;
	RUNNABLE *rp;
	cmd = rqspc(wlookup(&cp, &rp, cmd, cfg, runs));
	if (!cmd) break;
	if (run) {
		char *start = cmd;
		for (;;) {
			cmd = cp->parse(cfg, cmd);
			if (!cmd) break;
			cmd = skipspc(cmd);
			if (*cmd == '|' || !*cmd) break;
			if (cmd == start) {
				std::cerr << "Got '" << cmd << "' after command\n";
				cmd = 0;
				break;
			}
			start = cmd;
		}
		if (!cmd) break;
		Prosody::Module *mp = cfg->mod;
		if (!mp) break;
		if (rp->start(cfg, runs, mp, &df)) {
			break;
		}
		runlist.push_back(rp);
	} else {
		if (show) {
			std::cout << rp->name() << ":" << *cp << "\n";
		} else {
			char *start = cmd;
			for (;;) {
				cmd = cp->parse(cfg, skipspc(cmd));
				if (!cmd) return 0;
				cmd = skipspc(cmd);
				if (!*cmd) return 0;
				if (cmd == start) {
					std::cerr << "Got '" << cmd << "' after command\n";
					return 0;
				}
				start = cmd;
			}
		}
		return 0;
	}
 	cmd = skipspc(cmd);
 	if (!*cmd) return 0;
 	if (*cmd != '|') {
		std::cerr << "Expected '|' after command, got '"
			<< cmd << "'\n";
		break;
	}
 	cmd = skipspc(cmd + 1);
 }
 for (;;) {
 	unsigned u = runlist.size();
 	if (!u) return 1;
	runlist[u-1]->stop(cfg, runs);
 	runlist.pop_back();
 }
}
