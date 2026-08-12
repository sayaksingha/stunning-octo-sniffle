/* activerecord.c - a general record */

#define arlen(x) (sizeof(x)/sizeof(*(x)))

	// Enable this to test usage in the style of the data comms API
//#define CHECK_DATA_FIRST

#include "activerecord.hpp"
#include "error_sm.hpp"
#include "../Testlib/taistamp.h"

#include <errno.h>
#include <iostream>

error ActiveRecord::abort(int discard)
{
 Prosody::Error err = chan_->record_abort().discard(discard);
 if (err) return error(error(error_sm(err), "sm_record_abort() failed"));
 return error();
}

	// Note that we must not delete the resources since the main
	// thread might be in the middle of adjusting a parameter
void ActiveRecord::cleanup()
{
 state(DONE);
}

	// we have capacity for more data on a channel - give it
	// some data if there is any to give it
int ActiveRecord::handle_data()
{
 char buff[2048];
 Prosody::tSM_INT length;
 Prosody::Error e = chan_->get_recorded_data()
 	.data(buff)
 	.length(&length);
 if (e) {
	std::cerr << error(error(error_sm(e), "sm_get_recorded_data() failed"));
	return 1;
 }
 if (!fp_) return 0;
 fwrite(buff, 1, length, fp_);
 if (ferror(fp_)) {
	std::cerr << error(error(errno, "fwrite() failed"), "Cannot write record data file");
	return 1;
 }
 return 0;
}

	// the handler for when a channel's event is signalled
	// note that this is run by the worker thread
int ActiveRecord::handler()
{
#ifdef CHECK_DATA_FIRST
 {
  char buff[2048];
  Prosody::tSM_INT length;
  Prosody::Error e = chan_->get_recorded_data()
 	.data(buff)
 	.length(&length);
  if (e) {
	std::cerr << error(error(error_sm(e), "sm_get_recorded_data() failed"));
	return 1;
  }
  if (length) {
	state(BUSY);
	if (!fp_) return 0;
	fwrite(buff, 1, length, fp_);
	if (ferror(fp_)) {
		std::cerr << error(error(errno, "fwrite() failed"), "Cannot write record data file");
		return 1;
	}
	return 0;
  }
 }
#endif
 enum Prosody::kSMRecordStatus sts;
 enum Prosody::kSMRecordHowTerminated reason;
 enum Prosody::kSMRecognition type;
 Prosody::tSM_INT p0, p1;
 Prosody::Error e = chan_->record_status()
 	.status(&sts)
 	.termination_reason(&reason)
 	.recog_type(&type)
 	.param0(&p0)
 	.param1(&p1);
 if (e) {
	std::cerr <<  error(error(error_sm(e), "sm_record_status() failed"));
	return 1;
 }
 switch (sts) {
 case Prosody::kSMRecordStatusOverrun:
	state(OVERRUN);
 	return 0;	// ignore it
 case Prosody::kSMRecordStatusComplete:
#ifdef TEST_EVENT
	test_event(ajp->event, "after completion");
#endif
	if (reason == Prosody::kSMRecordHowTerminatedError) {
		std::cerr << "Record terminated by error\n";
	}
 	return 1;
 case Prosody::kSMRecordStatusCompleteData:
 	state(COMPDATA);
 	return 0;	// ignore it
 case Prosody::kSMRecordStatusData:
#ifdef CHECK_DATA_FIRST
 	return 0;
#else
	state(BUSY);
 	return handle_data();
#endif
 case Prosody::kSMRecordStatusNoData:
	state(NODATA);
#ifdef HAS_SYNC
	error err = checksync();
	if (err) {
		std::cerr <<  error(err, "checksync() failed");
		return 1;
	}
#endif
 	return 0;	// ignore it
 case Prosody::kSMRecordStatusRecognition:
        std::cout << "recognised: type="
        	<< type << ", p0="
        	<< p0 << ", p1="
        	<< p1 << "\n";
	return 0;
 }
 	// NOTREACHED
 return 1;
}
