#ifndef INCLUDED_PROSODY_CHANNEL_HPP
#define INCLUDED_PROSODY_CHANNEL_HPP

#include <string.h>

#include "prosody_error.hpp"

namespace Prosody {

#include "smbesp.h"
#include "smdrvr.h"
#include "smdc.h"

class Channel {
	tSMChannelId channel_;
	Channel(Channel &);
	Channel &operator=(Channel &) const;
public:
	Channel() : channel_(kSMNullChannelId) { }
	operator tSMChannelId() { return channel_; }
	Error free() {
		int rc = 0;
		if (channel_ != kSMNullChannelId) {
			rc = sm_channel_release(channel_);
			channel_ = kSMNullChannelId;
		}
		return Error(rc);
	}
	~Channel() {
		free().ok();
	}
#include "gen/prosgen-channel.hpp"
#include "gen/prospapi-channel.hpp"
#include "gen/prosdc-channel.hpp"
};

#undef kSMNullChannelId 
static const tSMChannelId kSMNullChannelId = 0;
}

#endif
