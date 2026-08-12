#include "rtptx.hpp"
#include "cfg.hpp"
#include "run.hpp"
#include "worker.hpp"
#include "../Testlib/errcode_sm.h"

int RTPTX::init()
{
 return startworker(&tid_, &evs_);
}

Prosody::tSMDatafeedId RTPTX::RTPTXONE::datafeed()
{
 std::cerr << "RTP tx never has datafeed\n";
 return 0;
}

char RTPTX::RTPTXONE::status()
{
 if (!vt_) return ' ';
 char c = '?';
 switch (at_->state()) {
 case ActiveRtptx::IDLE: return '.';
 case ActiveRtptx::RUN: c = '*'; break;
 case ActiveRtptx::SSRC: c = 'S'; break;
 case ActiveRtptx::TONECONT: c = 't'; break;
 case ActiveRtptx::TONE: c = 'T'; break;
 case ActiveRtptx::DONE:
	delete vt_;
	vt_ = 0;
 	return '-';
 }
 at_->idle();
 return c;
}

void RTPTX::status(std::ostream &os)
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

	// start a new RTP tx
int RTPTX::start(CFG *cfg, RUNSTATE *runs, Prosody::Module *mod, Prosody::tSMDatafeedId *dfp)
{
 (void) runs;
 Prosody::Vmptx *vt = new Prosody::Vmptx;
 Prosody::Error pe = vt->create()
 	.module(*mod);
 if (pe) {
 	std::cerr << "sm_vmptx_create() failed: "
 		<< errcode_sm(pe) << "\n";
 } else {
	Prosody::tSMEventId pev;
	pe = vt->get_event().event(&pev);
	if (pe) {
		std::cerr << "sm_vmptx_get_event() failed: "
			<< errcode_sm(pe) << "\n";
	} else {
		Prosody::Event ev(pev);
		pe = vt->datafeed_connect().data_source(*dfp);
		if (pe) {
			std::cerr << "sm_vmptx_datafeed_connect() failed: "
				<< errcode_sm(pe) << "\n";
		} else {
			ActiveRtptx *rep = new ActiveRtptx(vt, &ev);
			if (rep) {
				RTPTXONE ro(vt, rep);
				if (!ro.adjust(cfg, runs)) {
					error e = evs_.insert(rep);
					if (!e) {
						rr_.push_back(ro);
						return 0;
					}
					std::cerr << e << " - Cannot insert into eventset\n";
				}
				delete rep;
			} else {
				std::cerr << "no memory for ActiveRtptx\n";
			}
		}
	}
 }
 std::cerr << "RTP tx not started\n";
 delete vt;
 return 1;
}

int CFGVMPTX::update_partner(RUNSTATE *runs)
{
 if (value == ~0u) vmptx_ = 0;
 else {
	vmptx_ = runs->rtptx.nthID(runs, value);
	if (!vmptx_) {
		std::cerr << "Partner not running\n" << std::endl;
		return 1;
	}
 }
 return 0;
}

int RTPTX::RTPTXONE::adjust(CFG *newcfg, RUNSTATE *runs)
{
 if (newcfg->rtptx.codec.isac.partner.update_partner(runs)) return 1;
 if (reconfig_txcodec(&cfg_.codec, &newcfg->rtptx.codec, vt_)) return 1;
 sockaddr_in dest_rtp;
 dest_rtp.sin_addr = newcfg->rtptx.dest;
 dest_rtp.sin_port = newcfg->rtptx.destport;
 sockaddr_in src_rtp;
 src_rtp.sin_addr = newcfg->rtptx.src;
 src_rtp.sin_port = newcfg->rtptx.srcport;
 sockaddr_in dest_rtcp;
 dest_rtcp.sin_addr = newcfg->rtptx.rtcpdest;
 dest_rtcp.sin_port = newcfg->rtptx.rtcpdestport;
 sockaddr_in src_rtcp;
 src_rtcp.sin_addr = newcfg->rtptx.rtcpsrc;
 src_rtcp.sin_port = newcfg->rtptx.rtcpsrcport;
 Prosody::Error pe = vt_->config()
 	.destination_rtp(dest_rtp)
 	.source_rtp(src_rtp)
 	.TOS_RTP(newcfg->rtptx.tosrtp)
 	.destination_rtcp(dest_rtcp)
 	.source_rtcp(src_rtcp)
 	.TOS_RTCP(newcfg->rtptx.tosrtcp);
 if (pe) {
	std::cerr << "sm_rtptx_config() failed: " << errcode_sm(pe) << "\n";
	return 1;
 }
 if (cfg_.rate != newcfg->rtptx.rate) {
	pe = vt_->config_sample_rate().sample_rate(newcfg->rtptx.rate);
	if (pe) {
		std::cerr << "sm_rtptx_config_sample_rate() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (cfg_.rtcp != newcfg->rtptx.rtcp) {
	Prosody::tSMRTCPHandId rtcpid = runs->rtcp.nthID(runs, newcfg->rtptx.rtcp);
	pe = vt_->set_rtcphand().rtcphand(rtcpid);
	if (pe) {
		std::cerr << "sm_rtptx_set_rtcphand() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 cfg_ = newcfg->rtptx;
 return 0;
}

RUNONE *RTPTX::nth(RUNSTATE *runs, unsigned long v)
{
 (void) runs;
 if (v < rr_.size()) {
	RTPTXONE *ro = &rr_[v];
	if (ro->running()) return ro;
 }
 std::cerr << "rtptx[" << v << "] not running\n";
 return 0;
}

Prosody::tSMVMPtxId RTPTX::nthID(RUNSTATE *runs, unsigned long v)
{
 (void) runs;
 if (v < rr_.size()) {
	RTPTXONE *ro = &rr_[v];
	if (ro->running()) return *ro->vt_;
 }
 std::cerr << "rtptx[" << v << "] not running\n";
 return 0;
}

int RTPTX::RTPTXONE::halt(CFG *cfg, RUNSTATE *runs)
{
 (void) cfg;
 (void) runs;
 Prosody::Error pe = vt_->stop();
 if (pe) {
	std::cerr << "sm_vmptx_stop() failed: " << errcode_sm(pe) << "\n";
	return 1;
 }
 return 0;
}

void RTPTX::stop(CFG *cfg, RUNSTATE *runs)
{
 if (rr_.size()) rr_[rr_.size() - 1].halt(cfg, runs);
}

void RTPTX::fini()
{
 if (stopworker(tid_, &evs_)) {
 	std::cerr << "Error stopping rtptx worker\n";
 }
}
