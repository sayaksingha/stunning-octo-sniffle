#include "rec.hpp"
#include "cfg.hpp"
#include "worker.hpp"
#include "../Testlib/errcode_sm.h"

int REC::init()
{
 return startworker(&tid_, &evs_);
}

Prosody::tSMDatafeedId REC::RECONE::datafeed()
{
 std::cerr << "Record never has datafeed\n";
 return 0;
}

char REC::RECONE::status()
{
 if (!ar_) return ' ';
 char c = '?';
 switch (ar_->state()) {
 case ActiveRecord::IDLE: return '.';
 case ActiveRecord::STARTING: c = 'S'; break;
 case ActiveRecord::NODATA: c = 'N'; break;
 case ActiveRecord::LAST: c = 'L'; break;
 case ActiveRecord::COMPDATA: c = 'D'; break;
 case ActiveRecord::BUSY: c = '*'; break;
 case ActiveRecord::OVERRUN: c = 'u'; break;
 case ActiveRecord::DONE:
	Prosody::Error pe = ch_->set_event()
		.event_type(Prosody::kSMEventTypeReadData)
		.issue_events(Prosody::kSMChannelNoEvent);
	pe.ok();
	delete ar_;
	ar_ = 0;
 	return '-';
 }
 ar_->idle();
 return c;
}

void REC::status(std::ostream &os)
{
 unsigned del = 0;
 unsigned u;
 for (u=0; u < re_.size(); u++) {
 	char c = re_[u].status();
 	os << c;
 	if (c == ' ') del++;
 	else del = 0;
 }
 while (del-- > 0) re_.pop_back();
}

	// start recording
static int start_record(Prosody::Channel &chan, Prosody::Event &ev, CFG *cfg)
{
 if (cfg->rec.threshold) {
	Prosody::Error e = chan.set_input_threshold().minimum_bits(cfg->rec.threshold);
	if (e) {
		std::cerr << "sm_channel_set_input_threshold() failed: " << errcode_sm(e) << "\n";
		return 1;
	}
 }
#ifdef TEST_EVENT
 test_event(ev, "before start");
#else
 (void) ev;
#endif
 Prosody::Error e = chan.record_start()
 // .alt_data_source(?)
	.type(cfg->rec.format)
 // .silence_elimination(cfg->rec.silelim)
 // .tone_elimination_mode(cfg->rec.toneelim)
 // .tone_elimination_set_id(cfg->rec.toneset)
 // .max_octets(cfg->rec.max_octets)
 // .max_elapsed_time(cfg->rec.max_elapsed_time)
 // .max_silence(cfg->rec.max_silence)
	.agc(cfg->rec.agc)
	.volume(cfg->rec.volume)
	.sampling_rate(cfg->rec.rate)
	;
 if (e) {
 	std::cerr << "sm_record_start() failed: " << errcode_sm(e) << "\n";
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

	// start a new record
int REC::start(CFG *cfg, RUNSTATE *runs, Prosody::Module *mod, Prosody::tSMDatafeedId *dfp)
{
 (void) runs;
 FILE *outf = 0;
 if (*cfg->rec.file) {
	outf = fopen(cfg->rec.file, "wb");
	if (!outf) {
 		perror("fopen() failed");
 		std::cerr << "Cannot open record file: " << cfg->rec.file << "\n";
 		return 1;
	}
 }
 Prosody::Channel *chan = new Prosody::Channel;
 Prosody::Error pe = chan->alloc_placed()
 	.module(*mod)
 	.type(Prosody::kSMChannelTypeInput);
 if (pe) {
 	std::cerr << "sm_channel_alloc_placed() failed: "
 		<< errcode_sm(pe) << "\n";
 } else {
	Prosody::Event *ev = new Prosody::Event;
	pe = ev->create(Prosody::kSMEventTypeReadData, *chan);
	if (pe) {
		std::cerr << "smd_ev_create() failed: "
			<< errcode_sm(pe) << "\n";
	} else {
		pe = chan->set_event().event(*ev).event_type(Prosody::kSMEventTypeReadData).issue_events(Prosody::kSMChannelSpecificEvent);
		if (pe) {
			std::cerr << "sm_channel_set_event() failed: "
				<< errcode_sm(pe) << "\n";
		} else {
			pe = chan->datafeed_connect().data_source(*dfp);
			if (pe) {
				std::cerr << "sm_channel_get_datafeed() failed: "
				<< errcode_sm(pe) << "\n";
			} else {
				if (!start_record(*chan, *ev, cfg)) {
					ActiveRecord *rep = new ActiveRecord(chan, ev, outf, cfg->rec.threshold > 0 ? cfg->rec.threshold / 8 : 2048);
					if (rep) {
						error e = evs_.insert(rep);
						if (!e) {
							RECONE ro(chan, ev, rep);
							re_.push_back(ro);
							*dfp = 0;
							return 0;
						}
						std::cerr << e << " - Cannot insert into eventset\n";
					} else {
						std::cerr << "no memory for ActiveRecord\n";
					}
				}
			}
			pe = chan->set_event().event_type(Prosody::kSMEventTypeReadData).issue_events(Prosody::kSMChannelNoEvent);
			pe.ok();
		}
	}
	delete ev;
 }
 std::cerr << "Record not started\n";
 delete chan;
 if (outf) fclose(outf);
 return 1;
}

int REC::RECONE::adjust(CFG *cfg, RUNSTATE *runs)
{
 (void) runs;
 if (cfg->rec.threshold) {
	Prosody::Error e = ch_->set_input_threshold().minimum_bits(cfg->rec.threshold);
	if (e) {
		std::cerr << "sm_channel_set_input_threshold() failed: " << errcode_sm(e) << "\n";
		return 1;
	}
 }
 Prosody::Error pe = ch_->record_agc_adjust()
	.gain(cfg->rec.gain);
 if (pe) {
	std::cerr << "sm_record_agc_adjust() failed: " << errcode_sm(pe) << "\n";
	return 1;
 }
 return 0;
}

RUNONE *REC::nth(RUNSTATE *runs, unsigned long v)
{
 (void) runs;
 if (v < re_.size()) {
	RECONE *ro = &re_[v];
	if (ro->running()) return ro;
 }
 std::cerr << "rec[" << v << "] not running\n";
 return 0;
}

int REC::RECONE::halt(CFG *cfg, RUNSTATE *runs)
{
 (void) cfg;
 (void) runs;
 Prosody::Error pe = ch_->record_abort().discard(0);
 if (pe) {
	std::cerr << "sm_record_abort() failed: " << errcode_sm(pe) << "\n";
	return 1;
 }
 return 0;
}

void REC::stop(CFG *cfg, RUNSTATE *runs)
{
 if (re_.size()) re_[re_.size() - 1].halt(cfg, runs);
}

void REC::fini()
{
 if (stopworker(tid_, &evs_)) {
 	std::cerr << "Error stopping record worker\n";
 }
}
