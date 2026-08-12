#ifndef INCLUDED_TDMRX_HPP
#define INCLUDED_TDMRX_HPP

#include <vector>
#include "runnable.hpp"
#include "../../class/prosody.hpp"

class TDMRX : public RUNNABLE {
public:
	TDMRX() { }
	int init() { return 0; }
	int start(CFG *cfg, RUNSTATE *runs, Prosody::Module *mod, Prosody::tSMDatafeedId *idp);
	RUNONE *nth(RUNSTATE *runs, unsigned long v);
	void stop(CFG *cfg, RUNSTATE *runs);
	void status(std::ostream &os);
	void fini() { }
	~TDMRX() { }
	const char *name() { return "tdmrx"; }
	class TDMRXONE : public RUNONE {
		Prosody::Tdmrx *tdm_;
		//friend int TDMRX::start(CFG *cfg, RUNSTATE *runs, Prosody::Module *mod, Prosody::tSMDatafeedId *idp);
		friend class TDMRX;	// old MSVC cannot handle TDMRX::start()
		TDMRXONE(Prosody::Tdmrx *tdm) : tdm_(tdm) { }
	public:
		Prosody::tSMDatafeedId datafeed();
		bool running() {
			return tdm_;
		}
		int adjust(CFG *cfg, RUNSTATE *runs);
		int halt(CFG *cfg, RUNSTATE *runs);
		char status();
	};
private:
	std::vector<TDMRXONE> tdm_;
};

#endif
