#ifndef INCLUDED_PROSODY_EVENT_H
#define INCLUDED_PROSODY_EVENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "prosody_error.hpp"

namespace Prosody {

#include "smbesp.h"

class Event {
	int alloc_;	// 0=none, 1=alloc, 2=from ctor
	tSMEventId ev_;
	Event(Event &);
	Event &operator=(Event &) const;
public:
	Event() : alloc_(0) { }
	Event(tSMEventId ev) : alloc_(2), ev_(ev) { }
	operator tSMEventId() {
		if (!alloc_) {
			fprintf(stderr, "Using non-allocated event\n");
			abort();
		}
		return ev_;
	}
	Error free() {
		int rc = 0;
		if (alloc_ == 1) {
			rc = smd_ev_free(ev_);
			alloc_ = 0;
		}
		return Error(rc);
	}
	~Event() {
		free().ok();
	}
	Error create(int type, tSMChannelId chan) {
		int rc = smd_ev_create(&ev_, chan, type, kSMChannelSpecificEvent);
		if (!rc) {
			alloc_ = 1;
			SM_CHANNEL_SET_EVENT_PARMS ep;
			memset(&ep, 0, sizeof(ep));
			ep.channel = chan;
			ep.event_type = type;
			ep.issue_events = kSMChannelSpecificEvent;
			ep.event = ev_;
			rc = sm_channel_set_event(&ep);
			if (rc) {
				smd_ev_free(ev_);
				alloc_ = 0;
			}
		}
		return Error(rc);
	}
	Error createnonidle(int type, tSMChannelId chan) {
		int rc = smd_ev_create(&ev_, chan, type,
			kSMChannelSpecificEventNonIdle);
		if (!rc) {
			alloc_ = 1;
			SM_CHANNEL_SET_EVENT_PARMS ep;
			memset(&ep, 0, sizeof(ep));
			ep.channel = kSMNullChannelId;
			ep.event_type = type;
			ep.issue_events = kSMChannelSpecificEvent;
			ep.event = ev_;
			rc = sm_channel_set_event(&ep);
			if (rc) {
				smd_ev_free(ev_);
				alloc_ = 0;
			}
		}
		return Error(rc);
	}
	Error create(int type) {
		int rc = smd_ev_create(&ev_, 0, type, kSMAnyChannelEvent);
		if (!rc) alloc_ = 1;
		return Error(rc);
	}
	Error wait() {
		return Error(smd_ev_wait(ev_));
	}
};

}

#endif
