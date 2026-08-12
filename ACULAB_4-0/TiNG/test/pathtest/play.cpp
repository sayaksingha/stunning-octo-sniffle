#include "play.hpp"
#include "cfg.hpp"
#include "worker.hpp"
#include "../Testlib/errcode_sm.h"

int PLAY::init()
{
 return startworker(&tid_, &evs_);
}

Prosody::tSMDatafeedId PLAY::PLAYONE::datafeed()
{
 if (!ch_) {
 	std::cerr << "Play not running - no datafeed\n";
 	return 0;
 }
 Prosody::tSMDatafeedId df;
 Prosody::Error pe = ch_->get_datafeed().datafeed(&df);
 if (pe) {
	std::cerr << "sm_channel_get_datafeed() failed: "
		<< errcode_sm(pe) << "\n";
	return 0;
 }
 return df;
}

char PLAY::PLAYONE::status()
{
 if (!ar_) return ' ';
 char c = '?';
 switch (ar_->state()) {
 case ActiveReplay::IDLE: return '.';
 case ActiveReplay::STARTING: c = 'S'; break;
 case ActiveReplay::NOCAP: c = 'N'; break;
 case ActiveReplay::LAST: c = 'L'; break;
 case ActiveReplay::COMPDATA: c = 'D'; break;
 case ActiveReplay::BUSY: c = '*'; break;
 case ActiveReplay::UNDERRUN: c = 'u'; break;
 case ActiveReplay::DONE:
	Prosody::Error pe = ch_->set_event()
		.event_type(Prosody::kSMEventTypeWriteData)
		.issue_events(Prosody::kSMChannelNoEvent);
	pe.ok();
	delete ar_;
	ar_ = 0;
	return '-';
 }
 ar_->idle();
 return c;
}

void PLAY::status(std::ostream &os)
{
 unsigned del = 0;
 unsigned u;
 for (u=0; u < pl_.size(); u++) {
	char c = pl_[u].status();
	os << c;
	if (c == ' ') del++;
	else del = 0;
 }
 while (del-- > 0) pl_.pop_back();
}

	// start playing
static int start_replay(Prosody::Channel &chan, Prosody::Event &ev, CFG *cfg)
{
 if (cfg->play.threshold) {
	Prosody::Error e = chan.set_output_threshold().minimum_bits(cfg->play.threshold);
	if (e) {
		std::cerr << "sm_channel_set_output_threshold() failed: " << errcode_sm(e) << "\n";
		return 1;
	}
 }
#ifdef TEST_EVENT
 test_event(ev, "before start");
#else
 (void) ev;
#endif
 if (cfg->play.outrate) {
	Prosody::Error e = chan.set_output_rate().sample_rate(cfg->play.outrate);
	if (e) {
		std::cerr << "sm_channel_set_output_rate() failed: " << errcode_sm(e) << "\n";
		return 1;
	}
 }
 Prosody::Error e = chan.replay_start()
// rp.background = cfg->play.bgchan;
	.volume(cfg->play.volume)
	.speed(cfg->play.speed)
	.agc(cfg->play.agc)
	.type(cfg->play.format)
	.sampling_rate(cfg->play.rate)
	//.data_length(cfg->play.max_octets);
	;
 if (e) {
 	std::cerr << "sm_replay_start() failed: " << errcode_sm(e) << "\n";
	return 1;
 }
#ifdef TEST_EVENT
 test_event(ev, "after start");
 Sleep(1000);
 test_event(ev, "after start + 1S");
#endif
#ifdef HAS_SYNC
 if (ajp->mf->sync) {
	SM_SYNC_ADD_PARMS sa;
	memset(&sa, 0, sizeof(sa));
	sa.channel = ajp->mf->chan;
	sa.synchroniser = ajp->mf->sync->synchroniser;
	e = sm_sync_add(&sa);
	if (e) return prosody_error(e, "sm_sync_add() failed");
 }
#endif
 return error();
}

	// start a new replay
int PLAY::start(CFG *cfg, RUNSTATE *runs, Prosody::Module *mod, Prosody::tSMDatafeedId *dfp)
{
 (void) runs;
 FILE *inf = 0;
 if (*cfg->play.file) {
 	inf = fopen(cfg->play.file, "rb");
 	if (!inf) {
 		perror("fopen() failed");
 		std::cerr << "Cannot open replay file: "
 			<< cfg->play.file << "\n";
 		return 1;
	}
 }
 Prosody::Channel *chan = new Prosody::Channel;
 Prosody::Error pe = chan->alloc_placed()
 	.module(*mod)
 	.type(Prosody::kSMChannelTypeOutput);
 if (pe) {
 	std::cerr << "sm_channel_alloc_placed() failed: "
 		<< errcode_sm(pe) << "\n";
 } else {
	Prosody::Event *ev = new Prosody::Event;
	pe = ev->create(Prosody::kSMEventTypeWriteData, *chan);
	if (pe) {
		std::cerr << "smd_ev_create() failed: "
			<< errcode_sm(pe) << "\n";
	} else {
		pe = chan->set_event().event(*ev).event_type(Prosody::kSMEventTypeWriteData).issue_events(Prosody::kSMChannelSpecificEvent);
		if (pe) {
			std::cerr << "sm_channel_set_event() failed: "
				<< errcode_sm(pe) << "\n";
		} else {
			Prosody::tSMDatafeedId df = *dfp;
			pe = chan->get_datafeed().datafeed(dfp);
			if (pe) {
				std::cerr << "sm_channel_get_datafeed() failed: "
				<< errcode_sm(pe) << "\n";
			} else {
				if (!start_replay(*chan, *ev, cfg)) {
					ActiveReplay *rep = new ActiveReplay(chan, ev, inf, cfg->play.threshold > 0 ? cfg->play.threshold / 8 : 2048, cfg->play.once);
					if (rep) {
						error e = evs_.insert(rep);
						if (!e) {
							PLAYONE po(chan, ev, rep);
							pl_.push_back(po);
							return 0;
						}
						std::cerr << e << " - Cannot insert into eventset\n";
					} else {
						std::cerr << "no memory for ActiveReplay\n";
					}
				}
			}
			*dfp = df;	// restore it
			pe = chan->set_event().event_type(Prosody::kSMEventTypeWriteData).issue_events(Prosody::kSMChannelNoEvent);
			pe.ok();
		}
	}
	delete ev;
 }
 std::cerr << "Play not started\n";
 delete chan;
 if (inf) fclose(inf);
 return 1;
}

int PLAY::PLAYONE::adjust(CFG *cfg, RUNSTATE *runs)
{
 (void) runs;
 if (cfg->play.threshold) {
	Prosody::Error e = ch_->set_output_threshold().minimum_bits(cfg->play.threshold);
	if (e) {
		std::cerr << "sm_channel_set_output_threshold() failed: " << errcode_sm(e) << "\n";
		return 1;
	}
 }
 Prosody::Error pe = ch_->replay_adjust()
 	// FIXME: do background
 	.volume(cfg->play.volume)
	.agc(cfg->play.agc)
	.speed(cfg->play.speed);
 if (pe) {
	std::cerr << "sm_replay_adjust() failed: " << errcode_sm(pe) << "\n";
	return 1;
 }
 return 0;
}

RUNONE *PLAY::nth(RUNSTATE *runs, unsigned long v)
{
 (void) runs;
 if (v < pl_.size()) {
	PLAYONE *po = &pl_[v];
	if (po->running()) return po;
 }
 std::cerr << "play[" << v << "] not running\n";
 return 0;
}

int PLAY::PLAYONE::halt(CFG *cfg, RUNSTATE *runs)
{
 if (!ar_) return 0;
 (void) cfg;
 (void) runs;
 Prosody::Error pe = ch_->replay_abort().nowait(1);
 if (pe) {
	std::cerr << "sm_replay_abort() failed: " << errcode_sm(pe) << "\n";
	return 1;
 }
 return 0;
}

void PLAY::stop(CFG *cfg, RUNSTATE *runs)
{
 if (pl_.size()) pl_[pl_.size() - 1].halt(cfg, runs);
}

void PLAY::fini()
{
 if (stopworker(tid_, &evs_)) {
 	std::cerr << "Error stopping play worker\n";
 }
}
