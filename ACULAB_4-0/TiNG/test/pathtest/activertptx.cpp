/* activertptx.c - a general RTP receive */

#define arlen(x) (sizeof(x)/sizeof(*(x)))

#include "activertptx.hpp"
#include "error_sm.hpp"
#include "../Testlib/taistamp.h"

#include <iostream>

	// Note that we must not delete the resources since the main
	// thread might be in the middle of adjusting a parameter
void ActiveRtptx::cleanup()
{
 state(DONE);
}

	// the handler for when a vmptx's event is signalled
	// note that this is run by the worker thread
int ActiveRtptx::handler()
{
 Prosody::kSMVMPtxStatus sts;
 Prosody::Error e = vt_->status()
 	.status(&sts);
 if (e) {
	std::cerr <<  error(error(error_sm(e), "sm_vmptx_status() failed"));
	return 1;
 }
 switch (sts) {
 case Prosody::kSMVMPtxStatusRunning:
	state(RUN);
 	return 0;	// ignore it
 case Prosody::kSMVMPtxStatusSSRC:
	state(SSRC);
 	return 0;	// ignore it
 case Prosody::kSMVMPtxStatusStopped:
	state(DONE);
 	return 1;
 case Prosody::kSMVMPtxStatusToneOngoing:
	state(TONECONT);
 	return 0;	// ignore it
 case Prosody::kSMVMPtxStatusToneCompleted:
	state(TONE);
 	return 0;	// ignore it
 }
 	// NOTREACHED
 return 1;
}
