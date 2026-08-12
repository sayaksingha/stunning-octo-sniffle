#ifndef INCLUDED_RTCP_HPP
#define INCLUDED_RTCP_HPP

#include "runnable.hpp"

#include <vector>
#include "../../class/eventset.hpp"
#include "../../class/prosody.hpp"
#include "activertcp.hpp"

class RTCP : public RUNNABLE {
public:
	RTCP() { }
	int init();
	int start(CFG *cfg, RUNSTATE *runs, Prosody::Module *mod, Prosody::tSMDatafeedId *idp);
	RUNONE *nth(RUNSTATE *runs, unsigned long v);
	Prosody::tSMRTCPHandId nthID(RUNSTATE *runs, unsigned long v);
	void stop(CFG *cfg, RUNSTATE *runs);
	void status(std::ostream &os);
	void fini();
	~RTCP() { }
	const char *name() { return "rtcp"; }
	class RTCPONE : public RUNONE {
		Prosody::Rtcphand *hand_;
		ActiveRtcp *ar_;
		//friend int RTCP::start(CFG *cfg, RUNSTATE *runs, Prosody::Module *mod, Prosody::tSMDatafeedId *idp);
		friend class RTCP;	// old MSVC cannot handle RTCP::start()
		RTCPONE(Prosody::Rtcphand *hand,
			ActiveRtcp *ar) : hand_(hand), ar_(ar) { }
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
	std::vector<RTCPONE> rr_;
};

#endif
