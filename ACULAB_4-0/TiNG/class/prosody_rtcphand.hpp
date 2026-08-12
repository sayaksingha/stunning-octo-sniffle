#ifndef INCLUDED_PROSODY_RTCPHAND_HPP
#define INCLUDED_PROSODY_RTCPHAND_HPP

#include <string.h>

#include "prosody_error.hpp"

namespace Prosody {

#include "smrtp.h"

class Rtcphand {
	tSMRTCPHandId rtcphand_;
	Rtcphand(Rtcphand &);
	Rtcphand &operator=(Rtcphand &) const;
public:
	Rtcphand() : rtcphand_(kSMNullRTCPHandId) { }
	operator tSMRTCPHandId() { return rtcphand_; }
	Error free() {
		int rc = 0;
		if (rtcphand_ != kSMNullRTCPHandId) {
			rc = sm_rtcphand_destroy(rtcphand_);
			rtcphand_ = kSMNullRTCPHandId;
		}
		return Error(rc);
	}
	~Rtcphand() {
		free().ok();
	}
#include "gen/prosrtpapi-rtcphand.hpp"
};

}

#endif
