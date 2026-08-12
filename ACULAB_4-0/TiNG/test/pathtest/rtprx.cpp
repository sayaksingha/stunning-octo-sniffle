#include "rtprx.hpp"
#include "cfg.hpp"
#include "run.hpp"
#include "worker.hpp"
#include "../Testlib/errcode_sm.h"

int RTPRX::init()
{
 return startworker(&tid_, &evs_);
}

Prosody::tSMDatafeedId RTPRX::RTPRXONE::datafeed()
{
 if (!ar_) {
 	std::cerr << "RTP rx not running - no datafeed\n";
 	return 0;
 }
 Prosody::tSMDatafeedId df;
 Prosody::Error pe = vr_->get_datafeed().datafeed(&df);
 if (pe) {
	std::cerr << "sm_vmprx_get_datafeed() failed: "
		<< errcode_sm(pe) << "\n";
	return 0;
 }
 return df;
}

char RTPRX::RTPRXONE::status()
{
 if (!vr_) return ' ';
 char c = '?';
 switch (ar_->state()) {
 case ActiveRtprx::IDLE: return '.';
 case ActiveRtprx::RUN: c = '*'; break;
 case ActiveRtprx::LOST: c = 'L'; break;
 case ActiveRtprx::PORTS: c = 'G'; break;
 case ActiveRtprx::NEWSSRC: c = 'S'; break;
 case ActiveRtprx::TONE: c = 'T'; break;
 case ActiveRtprx::ENDTONE: c = 'E'; break;
 case ActiveRtprx::PAYLOAD: c = 'y'; break;
 case ActiveRtprx::DONE:
	delete vr_;
	vr_ = 0;
 	return '-';
 }
 ar_->idle();
 return c;
}

void RTPRX::status(std::ostream &os)
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

	// start a new RTP rx
int RTPRX::start(CFG *cfg, RUNSTATE *runs, Prosody::Module *mod, Prosody::tSMDatafeedId *dfp)
{
 (void) runs;
 Prosody::Vmprx *vr = new Prosody::Vmprx;
 Prosody::Error pe = vr->create()
 	.module(*mod)
 	.address(cfg->rtprx.local);
 if (pe) {
 	std::cerr << "sm_vmprx_create() failed: "
 		<< errcode_sm(pe) << "\n";
 } else {
	Prosody::tSMEventId pev;
	pe = vr->get_event().event(&pev);
	if (pe) {
		std::cerr << "sm_vmprx_get_event() failed: "
			<< errcode_sm(pe) << "\n";
	} else {
		Prosody::Event ev(pev);
		Prosody::tSMDatafeedId df = *dfp;
		pe = vr->get_datafeed().datafeed(dfp);
		if (pe) {
			std::cerr << "sm_vmprx_get_datafeed() failed: "
			<< errcode_sm(pe) << "\n";
		} else {
			ActiveRtprx *rep = new ActiveRtprx(vr, &ev);
			if (rep) {
				RTPRXONE ro(vr, rep);
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
				std::cerr << "no memory for ActiveRtprx\n";
			}
		}
		*dfp = df;	// restore it
	}
 }
 std::cerr << "RTP rx not started\n";
 delete vr;
 return 1;
}

int CFGVMPRX::update_partner(RUNSTATE *runs)
{
 if (value == ~0u) vmprx_ = 0;
 else {
	vmprx_ = runs->rtprx.nthID(runs, value);
	if (!vmprx_) {
		std::cerr << "Partner not running\n" << std::endl;
		return 1;
	}
 }
 return 0;
}

int RTPRX::RTPRXONE::adjust(CFG *newcfg, RUNSTATE *runs)
{
 if (reconfig_rxcodec(&cfg_.codec, &newcfg->rtprx.codec, vr_)) return 1;
 if (cfg_.rate != newcfg->rtprx.rate) {
	Prosody::Error pe = vr_->config_sample_rate()
		.sample_rate(newcfg->rtprx.rate);
	if (pe) {
		std::cerr << "sm_rtprx_config_sample_rate() failed: " << errcode_sm(pe) << "\n";
		return 1;
 	}
 }
 if (cfg_.dataloss != newcfg->rtprx.dataloss) {
	Prosody::Error pe = vr_->config_dataloss()
		.loss_threshold(newcfg->rtprx.dataloss);
	if (pe) {
		std::cerr << "sm_rtprx_config_dataloss() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (cfg_.maxjitter != newcfg->rtprx.maxjitter
	|| cfg_.initjitter != newcfg->rtprx.initjitter) {
	Prosody::Error pe = vr_->config_jitter()
		.max_delay_ms(newcfg->rtprx.maxjitter)
		.initial_delay_ms(newcfg->rtprx.initjitter);
	if (pe) {
		std::cerr << "sm_rtprx_config_jitter() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (cfg_.unhandled != newcfg->rtprx.unhandled) {
	Prosody::Error pe = vr_->config_unhandled_payload_reporting()
		.delay(newcfg->rtprx.unhandled);
	if (pe) {
		std::cerr << "sm_rtprx_config_unhandled_payload_reporting() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (cfg_.rtcp != newcfg->rtprx.rtcp) {
	Prosody::tSMRTCPHandId rtcpid = runs->rtcp.nthID(runs, newcfg->rtprx.rtcp);
	Prosody::Error pe = vr_->set_rtcphand().rtcphand(rtcpid);
	if (pe) {
		std::cerr << "sm_rtprx_set_rtcphand() failed: " << errcode_sm(pe) << "\n";
		return 1;
	}
 }
 cfg_ = newcfg->rtprx;
 return 0;
}

RUNONE *RTPRX::nth(RUNSTATE *runs, unsigned long v)
{
 (void) runs;
 if (v < rr_.size()) {
	RTPRXONE *ro = &rr_[v];
	if (ro->running()) return ro;
 }
 std::cerr << "rtprx[" << v << "] not running\n";
 return 0;
}

Prosody::tSMVMPrxId RTPRX::nthID(RUNSTATE *runs, unsigned long v)
{
 (void) runs;
 if (v < rr_.size()) {
	RTPRXONE *ro = &rr_[v];
	if (ro->running()) return *ro->vr_;
 }
 std::cerr << "rtprx[" << v << "] not running\n";
 return 0;
}

int RTPRX::RTPRXONE::halt(CFG *cfg, RUNSTATE *runs)
{
 (void) cfg;
 (void) runs;
 Prosody::Error pe = vr_->stop();
 if (pe) {
	std::cerr << "sm_vmprx_stop() failed: " << errcode_sm(pe) << "\n";
	return 1;
 }
 return 0;
}

void RTPRX::stop(CFG *cfg, RUNSTATE *runs)
{
 if (rr_.size()) rr_[rr_.size() - 1].halt(cfg, runs);
}

void RTPRX::fini()
{
 if (stopworker(tid_, &evs_)) {
 	std::cerr << "Error stopping rtprx worker\n";
 }
}
