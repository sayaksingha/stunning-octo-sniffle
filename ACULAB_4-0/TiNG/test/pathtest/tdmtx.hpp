#ifndef INCLUDED_TDMTX_HPP
#define INCLUDED_TDMTX_HPP

#include <vector>
#include "runnable.hpp"
#include "../../class/prosody.hpp"

class TDMTX : public RUNNABLE {
public:
	TDMTX() { }
	int init() { return 0; }
	int start(CFG *cfg, RUNSTATE *runs, Prosody::Module *mod, Prosody::tSMDatafeedId *idp);
	RUNONE *nth(RUNSTATE *runs, unsigned long v);
	void stop(CFG *cfg, RUNSTATE *runs);
	void status(std::ostream &os);
	void fini() { }
	~TDMTX() { }
	const char *name() { return "tdmtx"; }
	class TDMTXONE : public RUNONE {
		Prosody::Tdmtx *tdm_;
		//friend int TDMTX::start(CFG *cfg, RUNSTATE *runs, Prosody::Module *mod, Prosody::tSMDatafeedId *idp);
		friend class TDMTX;	// old MSVC cannot handle TDMTX::start()
		TDMTXONE(Prosody::Tdmtx *tdm) : tdm_(tdm) { }
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
	std::vector<TDMTXONE> tdm_;
};

#endif
