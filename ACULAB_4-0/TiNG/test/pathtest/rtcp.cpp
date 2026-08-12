#include "rtcp.hpp"
#include "cfg.hpp"
#include "worker.hpp"
#include "../Testlib/errcode_sm.h"

int RTCP::init()
{
 return startworker(&tid_, &evs_);
}

Prosody::tSMDatafeedId RTCP::RTCPONE::datafeed()
{
 std::cerr << "RTCP never has a datafeed\n";
 return 0;
}

char RTCP::RTCPONE::status()
{
 if (!ar_) return ' ';
 char c = '?';
 switch (ar_->state()) {
 case ActiveRtcp::IDLE: return '.';
 case ActiveRtcp::BUSY: c = '*'; break;
 case ActiveRtcp::DONE:
	delete ar_;
	ar_ = 0;
 	return '-';
 }
 ar_->idle();
 return c;
}

void RTCP::status(std::ostream &os)
{
 unsigned del = 0;
 unsigned u;
 for (u=0; u < rr_.size(); u++) {
 	char c = rr_[u].status();
 	os << c;
 	if (c == ' ') del++;
 	else del = 0;
 }
 while (del-- > 0) rr_.pop_back();
}

	// start a new rtpchand
int RTCP::start(CFG *cfg, RUNSTATE *runs, Prosody::Module *mod, Prosody::tSMDatafeedId *dfp)
{
 (void) runs;
 FILE *outf = 0;
 if (*cfg->rtcp.file) {
	outf = fopen(cfg->rtcp.file, "wb");
	if (!outf) {
 		perror("fopen() failed");
 		std::cerr << "Cannot open rtpchand file: " << cfg->rtcp.file << "\n";
 		return 1;
	}
 }
 Prosody::Rtcphand *hand = new Prosody::Rtcphand;
 Prosody::Error pe = hand->create()
 	.module(*mod);
 if (pe) {
 	std::cerr << "sm_rtcphand_create() failed: "
 		<< errcode_sm(pe) << "\n";
 } else {
	Prosody::tSMEventId evid;
	pe = hand->get_event().event(&evid);
	if (pe) {
		std::cerr << "sm_rtcphand_get_event() failed: "
			<< errcode_sm(pe) << "\n";
	} else {
		Prosody::Event *ev = new Prosody::Event(evid);
		ActiveRtcp *rep = new ActiveRtcp(hand, ev, outf);
		if (rep) {
			error e = evs_.insert(rep);
			if (!e) {
				RTCPONE ro(hand, rep);
				if (!ro.adjust(cfg, runs)) {
					rr_.push_back(ro);
					*dfp = 0;
					return 0;
				}
			} else {
				std::cerr << e << " - Cannot insert into eventset\n";
			}
		} else {
			std::cerr << "no memory for ActiveRtcp\n";
		}
		delete ev;
	}
 }
 std::cerr << "Rtcp not started\n";
 delete hand;
 if (outf) fclose(outf);
 return 1;
}

int RTCP::RTCPONE::adjust(CFG *cfg, RUNSTATE *runs)
{
 (void) runs;
 Prosody::Error e = hand_->config_bandwidth()
 	.rtcp_bw(cfg->rtcp.bw)
 	.rtcprx_bw(cfg->rtcp.rxbw)
 	.rtcptx_bw(cfg->rtcp.txbw);
 if (e) {
	std::cerr << "sm_rtcphand_config_bandwidth() failed: " << errcode_sm(e) << "\n";
	return 1;
 }
 e = hand_->config_reports()
 	.reports(cfg->rtcp.reports);
 if (e) {
	std::cerr << "sm_rtcphand_config_reports() failed: " << errcode_sm(e) << "\n";
	return 1;
 }
 std::vector<char *> *sdes = cfg->rtcp.sdes.val();
 unsigned u;
 for (u=0; u < sdes->size(); u++) {
 	char *s = (*sdes)[u];
 	if (s) {
 		e = hand_->config_sdes()
 			.itemnum(u)
 			.stringval(s)
 			.stringlen(strlen(s));
		if (e) {
			std::cerr << "sm_rtcphand_config_sdes() failed: " << errcode_sm(e) << "\n";
			return 1;
		}
	}
 }
 return 0;
}

RUNONE *RTCP::nth(RUNSTATE *runs, unsigned long v)
{
 (void) runs;
 if (v < rr_.size()) {
	RTCPONE *ro = &rr_[v];
	if (ro->running()) return ro;
 }
 std::cerr << "rtcp[" << v << "] not running\n";
 return 0;
}

Prosody::tSMRTCPHandId RTCP::nthID(RUNSTATE *runs, unsigned long v)
{
 (void) runs;
 if (v < rr_.size()) {
	RTCPONE *ro = &rr_[v];
	if (ro->running()) return *ro->hand_;
 }
 return 0;
}

int RTCP::RTCPONE::halt(CFG *cfg, RUNSTATE *runs)
{
 (void) cfg;
 (void) runs;
 return 0;
}

void RTCP::stop(CFG *cfg, RUNSTATE *runs)
{
 if (rr_.size()) rr_[rr_.size() - 1].halt(cfg, runs);
}

void RTCP::fini()
{
 if (stopworker(tid_, &evs_)) {
 	std::cerr << "Error stopping rtpchand worker\n";
 }
}
