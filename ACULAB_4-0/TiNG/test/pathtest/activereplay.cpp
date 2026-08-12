/* activereplay.c - a general replay */

#define arlen(x) (sizeof(x)/sizeof(*(x)))

#include "activereplay.hpp"
#include "error_sm.hpp"
#include <errno.h>

error ActiveReplay::abort(int nowait)
{
 once_ = ONCE_ONLY;
 Prosody::Error err = chan_->replay_abort().nowait(nowait);
 if (err) return error(error(error_sm(err), "sm_replay_abort() failed"));
 return error();
}

	// Note that we must not delete the resources since the main
	// thread might be in the middle of adjusting a parameter
void ActiveReplay::cleanup()
{
 state(DONE);
}

	// we have capacity for more data on a channel - give it
	// some data if there is any to give it
int ActiveReplay::handle_capacity(int had_eof)
{
 char buff[2048];
 size_t nc = fp_ ? fread(buff, 1, xfersize_, fp_) : xfersize_;
 if (fp_ && ferror(fp_)) {
	std::cerr << error(error(errno, "fread() failed"), "Cannot read replay data file");
	return 1;
 }
 if (nc) {
	Prosody::Error e = chan_->put_replay_data()
		.data(buff)
		.length(nc);
	if (e) {
		std::cerr << error(error(error_sm(e), "sm_put_replay_data() failed"));
		return 1;
	}
	state(BUSY);
	return 0;
 }
 if (had_eof) {	// EOF after rewind - avoid getting stuck
	error err("File is empty");
	std::cerr << err;
 	return 1;
 }
 if (once_ != ONCE_ONLY) {
		// playing repeatedly - rewind ready for repeat
	if (fp_ && fseek(fp_, 0, SEEK_SET)) {
		std::cerr << error(error(errno, "fseek() failed"), "Cannot rewind data file");
		return 1;
	}
	if (once_ == ONCE_NOT) return handle_capacity(1);
		// else must be ONCE_REPEAT
 }
	// only playing file once - finish at EOF
	// then we may restart or quit
 Prosody::Error e = chan_->put_last_replay_data()
	.data(buff)
	.length(nc);
 if (e) {
	std::cerr << error(error(error_sm(e), "sm_put_last_replay_data() failed"));
	return 1;
 }
 state(LAST);
 return 0;
}

	// the handler for when a channel's event is signalled
	// note that this is run by the worker thread
int ActiveReplay::handler()
{
 enum Prosody::kSMReplayStatus sts;
 Prosody::Error e = chan_->replay_status().status(&sts);
 if (e) {
	std::cerr <<  error(error(error_sm(e), "sm_replay_status() failed"));
	return 1;
 }
 switch (sts) {
 case Prosody::kSMReplayStatusComplete:
#ifdef TEST_EVENT
	test_event(ajp->event, "after completion");
#endif
	if (once_ == ONCE_REPEAT) {
		e = chan_->replay_start().type(Prosody::kSMDataFormatALawPCM);
		if (e) {
			std::cerr <<  error(error(error_sm(e), "sm_replay_start() failed"));
			return 1;
		}
		state(STARTING);
		return 0;
	}
 	return 1;
 case Prosody::kSMReplayStatusCompleteData:
 	state(COMPDATA);
 	return 0;	// ignore it
 case Prosody::kSMReplayStatusNoCapacity:
 	state(NOCAP);
 	return 0;	// ignore it
 case Prosody::kSMReplayStatusUnderrun:
	state(UNDERRUN);
 	return 0;	// ignore it
 case Prosody::kSMReplayStatusHasCapacity:
	state(BUSY);
 	int r = handle_capacity();
#ifdef HAS_SYNC
	error err = checksync();
	if (err) {
		std::cerr <<  error(err, "checksync() failed");
		return 1;
	}
#endif
	return r;
 }
 	// NOTREACHED
 return 1;
}
