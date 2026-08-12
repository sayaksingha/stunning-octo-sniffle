#ifndef INCLUDED_PROSODY_VIDPLAY_HPP
#define INCLUDED_PROSODY_VIDPLAY_HPP

#include <string.h>

#include "prosody_error.hpp"

namespace Prosody {

#include "smvideo.h"

class Avfplay {
	tSMAVFplayId avfplay_;
	Avfplay(Avfplay &);
	Avfplay &operator=(Avfplay &) const;
public:
	Avfplay() : avfplay_(kSMNullAVFplayId) { }
	operator tSMAVFplayId() { return avfplay_; }
	Error free() {
		return Error(0);
	}
	~Avfplay() {
		free().ok();
	}
#include "gen/prosvid-avfplay.hpp"
};

}

#endif
