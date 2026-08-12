/* activertcp.c - a rtcp handlers */

#define arlen(x) (sizeof(x)/sizeof(*(x)))

#include "activertcp.hpp"
#include "error_sm.hpp"
#include "../Testlib/taistamp.h"
#include "../../rtcplib/rtcp_show.h"

#include <errno.h>
#include <iostream>

	// Note that we must not delete the resources since the main
	// thread might be in the middle of adjusting a parameter
void ActiveRtcp::cleanup()
{
 state(DONE);
}

static int tosummary = 3;

	// note that this is run by the worker thread when the rtcphand
	// event is signalled
int ActiveRtcp::handler()
{
 Prosody::tSM_INT length;
 Prosody::Error e = hand_->get_data()
 	.data((char *) rtcp_buffer_end(&rb_))
 	.max_length(rtcp_buffer_needed(&rb_))
 	.done_length(&length);
 if (e) {
	std::cerr << error(error(error_sm(e), "sm_rtcphand_get_data() failed"));
	return 1;
 }
 if (length) {
	state(BUSY);
	if (!--tosummary) {
		e = hand_->request_statistics()
			.statcode(4);
		if (e) {
			std::cerr << error(error(error_sm(e), "sm_rtcphand_request_statistcis() failed"));
			return 1;
		}
	}
	{
	 unsigned u;
	 for (u=0; u < length + 0u; u++) {
	 	if (! ( u & 0xf) ) fprintf(fp_, "%04x", u);
	 	fprintf(fp_, " %02x", rtcp_buffer_end(&rb_)[u]);
	 	if ((u & 0xf) == 0xf) fprintf(fp_, "\n");
	 }
	 fprintf(fp_, "\n");
	}
	fflush(fp_);
	rtcp_buffer_append(&rb_, length);
	if (!fp_) return 0;
	for (;;) {
		if (rtcp_buffer_needed(&rb_)) break;
		show_prosody_rtcp(fp_, &rb_);
		rtcp_buffer_discard(&rb_);
	}
	fflush(fp_);
	if (ferror(fp_)) {
		std::cerr << error(error(errno, "fwrite() failed"), "Cannot write rtcp data file");
		return 1;
	}
	return 0;
 }
 enum Prosody::kSMRTCPHandStatus sts;
 e = hand_->status()
 	.status(&sts);
 if (e) {
	std::cerr <<  error(error(error_sm(e), "sm_rtcphand_status() failed"));
	return 1;
 }
 switch (sts) {
 case Prosody::kSMRTCPHandStatusRunning:
	state(IDLE);
 	return 0;	// ignore it
 }
 	// NOTREACHED
 return 1;
}
