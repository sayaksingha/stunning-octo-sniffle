#include "tdmrx.hpp"
#include "cfg.hpp"
#include "run.hpp"
#include "../Testlib/errcode_sm.h"
#include <iostream>

Prosody::tSMDatafeedId TDMRX::TDMRXONE::datafeed()
{
 if (!tdm_) {
 	std::cerr << "tdmrx not running - no datafeed\n";
 	return 0;
 }
 Prosody::tSMDatafeedId df;
 Prosody::Error pe = tdm_->get_datafeed().datafeed(&df);
 if (pe) {
	std::cerr << "sm_tdmrx_get_datafeed() failed: "
		<< errcode_sm(pe) << "\n";
	return 0;
 }
 return df;
}

char TDMRX::TDMRXONE::status()
{
 if (!tdm_) return ' ';
 return '.';
}

void TDMRX::status(std::ostream &os)
{
 unsigned del = 0;
 unsigned u;
 for (u=0; u < tdm_.size(); u++) {
	char c = tdm_[u].status();
	os << c;
	if (c == ' ') del++;
	else del = 0;
 }
 while (del-- > 0) tdm_.pop_back();
}

int TDMRX::start(CFG *cfg, RUNSTATE *runs, Prosody::Module *mod, Prosody::tSMDatafeedId *dfp)
{
 (void) runs;
 if (*dfp) std::cerr << "Warning: tdmrx ignores input datafeed\n";
 Prosody::Tdmrx *tdmrx = new Prosody::Tdmrx;
 Prosody::kSMTimeslotType type;
 switch (cfg->tdmrx.ts.format) {
 case 'a': type = Prosody::kSMTimeslotTypeALaw; break;
 case 'u': type = Prosody::kSMTimeslotTypeMuLaw; break;
 case 'd': type = Prosody::kSMTimeslotTypeData; break;
 default:
 	std::cerr << "Oops! tdmrx timeslot type is "
 		<< cfg->tdmrx.ts.format << "\n";
	return 1;
 }
 Prosody::Error pe = tdmrx->create()
 	.module(*mod)
 	.stream(cfg->tdmrx.ts.stream)
 	.timeslot(cfg->tdmrx.ts.timeslot)
 	.type(type);
 if (pe) {
 	std::cerr << "sm_tdmrx_create() failed: "
 		<< errcode_sm(pe) << "\n";
 } else {
	Prosody::tSMDatafeedId df = *dfp;
	pe = tdmrx->get_datafeed().datafeed(dfp);
	if (pe) {
		std::cerr << "sm_tdmrx_get_datafeed() failed: "
			<< errcode_sm(pe) << "\n";
	} else {
		TDMRXONE ro(tdmrx);
		tdm_.push_back(ro);
		return 0;
	}
	*dfp = df;	// restore it
 }
 std::cerr << "Tdmrx not started\n";
 delete tdmrx;
 return 1;
}

int TDMRX::TDMRXONE::adjust(CFG *cfg, RUNSTATE *runs)
{
 (void) cfg;
 (void) runs;
 return 0;
}

RUNONE *TDMRX::nth(RUNSTATE *runs, unsigned long v)
{
 (void) runs;
 if (v < tdm_.size()) {
	TDMRXONE *to = &tdm_[v];
	if (to->running()) return to;
 }
 std::cerr << "tdmrx[" << v << "] not running\n";
 return 0;
}

int TDMRX::TDMRXONE::halt(CFG *cfg, RUNSTATE *runs)
{
 (void) cfg;
 (void) runs;
 Prosody::Error pe = tdm_->destroy();
 if (pe) {
	std::cerr << "sm_tdmrx_destroy() failed: " << errcode_sm(pe) << "\n";
	return 1;
 }
 delete tdm_;
 tdm_ = 0;
 return 0;
}

void TDMRX::stop(CFG *cfg, RUNSTATE *runs)
{
 if (tdm_.size()) tdm_[tdm_.size() - 1].halt(cfg, runs);
}
