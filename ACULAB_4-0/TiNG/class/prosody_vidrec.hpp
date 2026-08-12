#ifndef INCLUDED_PROSODY_VIDREC_HPP
#define INCLUDED_PROSODY_VIDREC_HPP

#include <string.h>

#include "prosody_error.hpp"

namespace Prosody {

#include "smvideo.h"

class Avfrec {
	tSMAVFrecId avfrec_;
	Avfrec(Avfrec &);
	Avfrec &operator=(Avfrec &) const;
public:
	Avfrec() : avfrec_(kSMNullAVFrecId) { }
	operator tSMAVFrecId() { return avfrec_; }
	Error free() {
		return Error(0);
	}
	~Avfrec() {
		free().ok();
	}
#include "gen/prosvid-avfrec.hpp"
};

}

#endif
