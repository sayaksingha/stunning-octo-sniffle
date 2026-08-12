/* activertprx.c - a general RTP receive */

#define arlen(x) (sizeof(x)/sizeof(*(x)))

#include "activertprx.hpp"
#include "error_sm.hpp"
#include "../Testlib/taistamp.h"

#include <iostream>

	// Note that we must not delete the resources since the main
	// thread might be in the middle of adjusting a parameter
void ActiveRtprx::cleanup()
{
 state(DONE);
}

	// the handler for when a vmprx's event is signalled
	// note that this is run by the worker thread
int ActiveRtprx::handler()
{
 Prosody::tSM_INT no_data;
 Prosody::kSMVMPrxStatus sts;
 Prosody::Error e = vr_->status()
 	.status(&sts)
 	.u_run_no_data(&no_data);
 if (e) {
	std::cerr <<  error(error(error_sm(e), "sm_vmprx_status() failed"));
	return 1;
 }
 switch (sts) {
 case Prosody::kSMVMPrxStatusRunning:
	if (no_data) state(LOST);
	else state(RUN);
 	return 0;	// ignore it
 case Prosody::kSMVMPrxStatusStopped:
	state(DONE);
 	return 1;
 case Prosody::kSMVMPrxStatusEndTone:
	state(ENDTONE);
 	return 0;	// ignore it
 case Prosody::kSMVMPrxStatusDetectTone:
	state(TONE);
 	return 0;	// ignore it
 case Prosody::kSMVMPrxStatusNewSSRC:
 	state(NEWSSRC);
 	return 0;
 case Prosody::kSMVMPrxStatusGotPorts:
	state(PORTS);
 	return 0;	// ignore it
 case Prosody::kSMVMPrxStatusUnhandledPayload:
	state(PAYLOAD);
 	return 0;	// ignore it
 }
 	// NOTREACHED
 return 1;
}
