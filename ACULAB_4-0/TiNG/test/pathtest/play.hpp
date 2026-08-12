#ifndef INCLUDED_PLAY_HPP
#define INCLUDED_PLAY_HPP

#include "runnable.hpp"

#include <vector>
#include "../../class/eventset.hpp"
#include "../../class/prosody.hpp"
#include "activereplay.hpp"

class PLAY : public RUNNABLE {
public:
	PLAY() { }
	int init();
	int start(CFG *cfg, RUNSTATE *runs, Prosody::Module *mod, Prosody::tSMDatafeedId *idp);
	RUNONE *nth(RUNSTATE *runs, unsigned long v);
	void stop(CFG *cfg, RUNSTATE *runs);
	void status(std::ostream &os);
	void fini();
	~PLAY() { }
	const char *name() { return "play"; }
	class PLAYONE : public RUNONE {
		Prosody::Channel *ch_;
		Prosody::Event *ev_;
		ActiveReplay *ar_;
		//friend int PLAY::start(CFG *cfg, RUNSTATE *runs, Prosody::Module *mod, Prosody::tSMDatafeedId *idp);
		friend class PLAY;	// old MSVC cannot handle PLAY::start()
		PLAYONE(Prosody::Channel *ch,
			Prosody::Event *ev,
			ActiveReplay *ar) : ch_(ch), ev_(ev), ar_(ar) { }
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
	std::vector<PLAYONE> pl_;
};

#endif
