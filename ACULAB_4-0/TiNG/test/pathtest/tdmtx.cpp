#include "tdmtx.hpp"
#include "cfg.hpp"
#include "run.hpp"
#include "../Testlib/errcode_sm.h"
#include <iostream>

Prosody::tSMDatafeedId TDMTX::TDMTXONE::datafeed()
{
 std::cerr << "tdmtx never has datafeed\n";
 return 0;
}

char TDMTX::TDMTXONE::status()
{
 if (!tdm_) return ' ';
 return '.';
}

void TDMTX::status(std::ostream &os)
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

int TDMTX::start(CFG *cfg, RUNSTATE *runs, Prosody::Module *mod, Prosody::tSMDatafeedId *dfp)
{
 (void) runs;
 Prosody::Tdmtx *tdmtx = new Prosody::Tdmtx;
 Prosody::kSMTimeslotType type;
 switch (cfg->tdmtx.ts.format) {
 case 'a': type = Prosody::kSMTimeslotTypeALaw; break;
 case 'u': type = Prosody::kSMTimeslotTypeMuLaw; break;
 case 'd': type = Prosody::kSMTimeslotTypeData; break;
 default:
 	std::cerr << "Oops! tdmtx timeslot type is "
 		<< cfg->tdmtx.ts.format << "\n";
	return 1;
 }
 Prosody::Error pe = tdmtx->create()
 	.module(*mod)
 	.stream(cfg->tdmtx.ts.stream)
 	.timeslot(cfg->tdmtx.ts.timeslot)
 	.type(type);
 if (pe) {
 	std::cerr << "sm_tdmtx_create() failed: "
 		<< errcode_sm(pe) << "\n";
 } else {
	pe = tdmtx->datafeed_connect().data_source(*dfp);
	if (pe) {
		std::cerr << "sm_tdmtx_datafeed_connect() failed: "
			<< errcode_sm(pe) << "\n";
	} else {
		*dfp = 0;
		TDMTXONE to(tdmtx);
		tdm_.push_back(to);
		return 0;
	}
 }
 std::cerr << "Tdmtx not started\n";
 delete tdmtx;
 return 1;
}

int TDMTX::TDMTXONE::adjust(CFG *cfg, RUNSTATE *runs)
{
 (void) cfg;
 (void) runs;
 return 0;
}

RUNONE *TDMTX::nth(RUNSTATE *runs, unsigned long v)
{
 (void) runs;
 if (v < tdm_.size()) {
	TDMTXONE *to = &tdm_[v];
	if (to->running()) return to;
 }
 std::cerr << "tdmtx[" << v << "] not running\n";
 return 0;
}

int TDMTX::TDMTXONE::halt(CFG *cfg, RUNSTATE *runs)
{
 (void) cfg;
 (void) runs;
 Prosody::Error pe = tdm_->destroy();
 if (pe) {
	std::cerr << "sm_tdmtx_destroy() failed: " << errcode_sm(pe) << "\n";
	return 1;
 }
 delete tdm_;
 tdm_ = 0;
 return 0;
}

void TDMTX::stop(CFG *cfg, RUNSTATE *runs)
{
 if (tdm_.size()) tdm_[tdm_.size() - 1].halt(cfg, runs);
}
