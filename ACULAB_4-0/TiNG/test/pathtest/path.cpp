#include "path.hpp"
#include "cfg.hpp"
#include "run.hpp"
#include "worker.hpp"
#include "../Testlib/errcode_sm.h"

int PATH::init()
{
 return startworker(&tid_, &evs_);
}

Prosody::tSMDatafeedId PATH::PATHONE::datafeed()
{
 if (!pt_) {
	std::cerr << "Path not running - no datafeed\n";
	return 0;
 }
 Prosody::tSMDatafeedId df;
 Prosody::Error pe = pt_->get_datafeed().datafeed(&df);
 if (pe) {
	std::cerr << "sm_path_get_datafeed() failed: "
		<< errcode_sm(pe) << "\n";
	return 0;
 }
 return df;
}

char PATH::PATHONE::status()
{
 if (!pt_) return ' ';
 char c = '?';
 Prosody::kSMPathStatus sts;
 Prosody::Error pe = pt_->status()
 	.status(&sts);
 if (pe) {
	std::cerr << "sm_path_status() failed: "
		<< errcode_sm(pe) << "\n";
	return 0;
 }
 switch (sts) {
 case Prosody::kSMPathStatusRunning: return '*';
 }
 return c;
}

void PATH::status(std::ostream &os)
{
 unsigned del = 0;
 unsigned u;
 for (u=0; u < pt_.size(); u++) {
	char c = pt_[u].status();
	os << c;
	if (c == ' ') del++;
	else del = 0;
 }
 while (del-- > 0) pt_.pop_back();
}

static int setup_path(Prosody::Path *path, RUNSTATE *runs, CFG::CFGPATH *pcmd)
{
 if (pcmd->agc.agc || pcmd->agc.volume) {
	Prosody::Error pe = path->agc()
		.agc(pcmd->agc.agc).volume(pcmd->agc.volume);
	if (pe) {
		std::cerr << "sm_path_agc() failed: "
			<< errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (pcmd->ec.enable) {
	RUNNABLE *reftyp = runs->lookup(pcmd->ec.ref.type);
	RUNONE *ref = reftyp->nth(runs, pcmd->ec.ref.pos);
	Prosody::Error pe = path->echocancel()
		.enable(1)
		.reference(ref->datafeed())
		.non_linear(pcmd->ec.non_linear)
		.use_agc(pcmd->ec.use_agc)
		.fix_agc(pcmd->ec.fix_agc);
	if (pe) {
		std::cerr << "sm_path_agc() failed: "
			<< errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (pcmd->pitchshift.by) {
	Prosody::Error pe = path->pitchshift()
		.shift(pcmd->pitchshift.by);
	if (pe) {
		std::cerr << "sm_path_pitchshift() failed: "
			<< errcode_sm(pe) << "\n";
		return 1;
	}
 }
 if (pcmd->resample.uprate || pcmd->resample.downrate) {
	Prosody::Error pe = path->resample()
		.uprate(pcmd->resample.uprate)
		.downrate(pcmd->resample.downrate);
	if (pe) {
		std::cerr << "sm_path_resample() failed: "
			<< errcode_sm(pe) << "\n";
		return 1;
	}
 }
 return 0;
}

	// start a new path
int PATH::start(CFG *cfg, RUNSTATE *runs, Prosody::Module *mod, Prosody::tSMDatafeedId *dfp)
{
 (void) runs;
 Prosody::Path *path = new Prosody::Path;
 Prosody::Error pe = path->create().module(*mod);
 if (pe) {
	std::cerr << "sm_path_create() failed: "
		<< errcode_sm(pe) << "\n";
 } else {
	if (!setup_path(path, runs, &cfg->path)) {
		pe = path->datafeed_connect().data_source(*dfp);
		if (pe) {
			std::cerr << "sm_path_datafeed_connect() failed: "
				<< errcode_sm(pe) << "\n";
		} else {
			Prosody::tSMDatafeedId df = *dfp;
			pe = path->get_datafeed().datafeed(dfp);
			if (pe) {
				std::cerr << "sm_path_get_datafeed() failed: "
					<< errcode_sm(pe) << "\n";
			} else {
				pt_.push_back(path);
				return 0;
			}
			*dfp = df;	// restore it
		}
	}
 }
 std::cerr << "Path not started\n";
 delete path;
 return 1;
}

RUNONE *PATH::nth(RUNSTATE *runs, unsigned long v)
{
 (void) runs;
 if (v < pt_.size()) {
	PATHONE *po = &pt_[v];
	if (po->running()) return po;
 }
 std::cerr << "path[" << v << "] not running\n";
 return 0;
}

int PATH::PATHONE::adjust(CFG *cfg, RUNSTATE *runs)
{
 Prosody::Error pe = pt_->agc()
	.agc(cfg->path.agc.agc).volume(cfg->path.agc.volume);
 if (pe) {
	std::cerr << "sm_path_agc() failed: "
		<< errcode_sm(pe) << "\n";
	return 1;
 }
 RUNNABLE *reftyp = runs->lookup(cfg->path.ec.ref.type);
 RUNONE *ref = reftyp->nth(runs, cfg->path.ec.ref.pos);
 pe = pt_->echocancel()
	.enable(cfg->path.ec.enable)
	.reference(ref->datafeed())
	.non_linear(cfg->path.ec.non_linear)
	.use_agc(cfg->path.ec.use_agc)
	.fix_agc(cfg->path.ec.fix_agc);
 if (pe) {
	std::cerr << "sm_path_agc() failed: "
		<< errcode_sm(pe) << "\n";
	return 1;
 }
 pe = pt_->pitchshift()
 	.shift(cfg->path.pitchshift.by);
 if (pe) {
	std::cerr << "sm_path_pitchshift() failed: "
		<< errcode_sm(pe) << "\n";
	return 1;
 }
 return 0;
}

int PATH::PATHONE::halt(CFG *cfg, RUNSTATE *runs)
{
 (void) cfg;
 (void) runs;
 Prosody::Error pe = pt_->destroy();
 if (pe) {
	std::cerr << "sm_path_destroy() failed: " << errcode_sm(pe) << "\n";
	return 1;
 }
 delete pt_;
 pt_ = 0;
 return 0;
}

void PATH::stop(CFG *cfg, RUNSTATE *runs)
{
 if (pt_.size()) pt_[pt_.size() - 1].halt(cfg, runs);
}

void PATH::fini()
{
 if (stopworker(tid_, &evs_)) {
 	std::cerr << "Error stopping path worker\n";
 }
}
