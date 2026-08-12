#ifndef INCLUDED_REC_HPP
#define INCLUDED_REC_HPP

#include "runnable.hpp"

#include <vector>
#include "../../class/eventset.hpp"
#include "../../class/prosody.hpp"
#include "activerecord.hpp"

class REC : public RUNNABLE {
public:
	REC() { }
	int init();
	int start(CFG *cfg, RUNSTATE *runs, Prosody::Module *mod, Prosody::tSMDatafeedId *idp);
	RUNONE *nth(RUNSTATE *runs, unsigned long v);
	void stop(CFG *cfg, RUNSTATE *runs);
	void status(std::ostream &os);
	void fini();
	~REC() { }
	const char *name() { return "rec"; }
	class RECONE : public RUNONE {
		Prosody::Channel *ch_;
		Prosody::Event *ev_;
		ActiveRecord *ar_;
		//friend int REC::start(CFG *cfg, RUNSTATE *runs, Prosody::Module *mod, Prosody::tSMDatafeedId *idp);
		friend class REC;	// old MSVC cannot handle REC::start()
		RECONE(Prosody::Channel *ch,
			Prosody::Event *ev,
			ActiveRecord *ar) : ch_(ch), ev_(ev), ar_(ar) { }
	public:
		Prosody::tSMDatafeedId datafeed();
		bool running() {
			return ar_;
		}
		int adjust(CFG *cfg, RUNSTATE *runs);
		int halt(CFG *cfg, RUNSTATE *runs);
		char status();
	};
private:
	pthread_t tid_;
	EventSet evs_;
	std::vector<RECONE> re_;
};

#endif
