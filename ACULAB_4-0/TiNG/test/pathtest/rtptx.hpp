#ifndef INCLUDED_RTPTX_HPP
#define INCLUDED_RTPTX_HPP

#include "runnable.hpp"

#include <vector>
#include "../../class/eventset.hpp"
#include "../../class/prosody.hpp"
#include "activertptx.hpp"

class RTPTX : public RUNNABLE {
public:
	RTPTX() { }
	int init();
	int start(CFG *cfg, RUNSTATE *runs, Prosody::Module *mod, Prosody::tSMDatafeedId *idp);
	RUNONE *nth(RUNSTATE *runs, unsigned long v);
	Prosody::tSMVMPtxId nthID(RUNSTATE *runs, unsigned long v);
	void stop(CFG *cfg, RUNSTATE *runs);
	void status(std::ostream &os);
	void fini();
	~RTPTX() { }
	const char *name() { return "rtptx"; }
	class RTPTXONE : public RUNONE {
		Prosody::Vmptx *vt_;
		ActiveRtptx *at_;
		CFG::CFGRTPTX cfg_;		// the current configuration for vt_
		//friend int RTPTX::start(CFG *cfg, RUNSTATE *runs, Prosody::Module *mod, Prosody::tSMDatafeedId *idp);
		friend class RTPTX;	// old MSVC cannot handle RTPTX::start()
		RTPTXONE(Prosody::Vmptx *vt,
			ActiveRtptx *at) : vt_(vt), at_(at) { }
	public:
		Prosody::tSMDatafeedId datafeed();
		bool running() {
			return at_;
		}
		int adjust(CFG *cfg, RUNSTATE *runs);
		int halt(CFG *cfg, RUNSTATE *runs);
		char status();
	};
private:
	pthread_t tid_;
	EventSet evs_;
	std::vector<RTPTXONE> rr_;
};

#endif
