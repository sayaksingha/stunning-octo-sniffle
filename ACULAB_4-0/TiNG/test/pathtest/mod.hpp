#ifndef INCLUDED_MOD_HPP
#define INCLUDED_MOD_HPP

#include "runnable.hpp"

#include <iostream>
#include <vector>

class MOD : public RUNNABLE {
public:
	MOD() { }
	int init() { return 0; }
	int start(CFG *cfg, RUNSTATE *runs, Prosody::Module *mod, Prosody::tSMDatafeedId *idp) {
		(void) cfg;
		(void) runs;
		(void) mod;
		(void) idp;
		std::cerr << "Command 'mod' cannot be run - configuration only\n";
		return 1;
	}
	RUNONE *nth(RUNSTATE *runs, unsigned long v) {
		(void) runs;
		(void) v;
		std::cerr << "Command 'mod' cannot be run - configuration only\n";
		return 0;
	}
	void stop(CFG *cfg, RUNSTATE *runs) { (void) cfg; (void) runs; }
	void status(std::ostream &os) { (void) os; }
	void fini() { }
	~MOD() { }
	const char *name() { return "mod"; }
};

#endif
