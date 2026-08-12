#ifndef INCLUDED_CARD_HPP
#define INCLUDED_CARD_HPP

#include "runnable.hpp"

#include <iostream>
#include <vector>

class CARD : public RUNNABLE {
	int dummy;
public:
	CARD() { }
	int init() { return 0; }
	int start(CFG *cfg, RUNSTATE *runs, Prosody::Module *mod, Prosody::tSMDatafeedId *idp) {
		(void) cfg;
		(void) runs;
		(void) mod;
		(void) idp;
		std::cerr << "Command 'card' cannot be run - configuration only\n";
		return 1;
	}
	RUNONE *nth(RUNSTATE *runs, unsigned long v) {
		(void) runs;
		(void) v;
		std::cerr << "Command 'card' cannot be run - configuration only\n";
		return 0;
	}
	void stop(CFG *cfg, RUNSTATE *runs) { (void) cfg; (void) runs; }
	void status(std::ostream &os) { (void) os; }
	void fini() { }
	~CARD() { }
	const char *name() { return "card"; }
};

#endif
