#ifndef INCLUDED_RTPRX_HPP
#define INCLUDED_RTPRX_HPP

#include "runnable.hpp"

#include <vector>
#include "../../class/eventset.hpp"
#include "../../class/prosody.hpp"
#include "activertprx.hpp"

class RTPRX : public RUNNABLE {
public:
	RTPRX() { }
	int init();
	int start(CFG *cfg, RUNSTATE *runs, Prosody::Module *mod, Prosody::tSMDatafeedId *idp);
	RUNONE *nth(RUNSTATE *runs, unsigned long v);
	Prosody::tSMVMPrxId nthID(RUNSTATE *runs, unsigned long v);
	void stop(CFG *cfg, RUNSTATE *runs);
	void status(std::ostream &os);
	void fini();
	~RTPRX() { }
	const char *name() { return "rtprx"; }
	class RTPRXONE : public RUNONE {
		Prosody::Vmprx *vr_;
		ActiveRtprx *ar_;
		CFG::CFGRTPRX cfg_;		// the current configuration for vr_
		//friend int RTPRX::start(CFG *cfg, RUNSTATE *runs, Prosody::Module *mod, Prosody::tSMDatafeedId *idp);
		friend class RTPRX;	// old MSVC cannot handle RTPRX::start()
		RTPRXONE(Prosody::Vmprx *vr,
			ActiveRtprx *ar) : vr_(vr), ar_(ar) { }
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
	std::vector<RTPRXONE> rr_;
};

#endif
