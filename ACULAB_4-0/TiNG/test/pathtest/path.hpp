#ifndef INCLUDED_PATH_HPP
#define INCLUDED_PATH_HPP

#include "runnable.hpp"

#include <vector>
#include "../../class/eventset.hpp"
#include "../../class/prosody.hpp"

class PATH : public RUNNABLE {
public:
	PATH() { }
	int init();
	int start(CFG *cfg, RUNSTATE *runs, Prosody::Module *mod, Prosody::tSMDatafeedId *idp);
	RUNONE *nth(RUNSTATE *runs, unsigned long v);
	void stop(CFG *cfg, RUNSTATE *runs);
	void status(std::ostream &os);
	void fini();
	~PATH() { }
	const char *name() { return "path"; }
	class PATHONE : public RUNONE {
		Prosody::Path *pt_;
		//friend int PATH::start(CFG *cfg, RUNSTATE *runs, Prosody::Module *mod, Prosody::tSMDatafeedId *idp);
		friend class PATH;	// old MSVC cannot handle PATH::start()
		PATHONE(Prosody::Path *pt) : pt_(pt) { }
	public:
		Prosody::tSMDatafeedId datafeed();
		bool running() {
			return pt_;
		}
		int adjust(CFG *cfg, RUNSTATE *runs);
		int halt(CFG *cfg, RUNSTATE *runs);
		char status();
	};
private:
	pthread_t tid_;
	EventSet evs_;
	std::vector<PATHONE> pt_;
};

#endif
