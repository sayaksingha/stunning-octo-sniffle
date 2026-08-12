#ifndef INCLUDED_PROSODY_TDMRX_HPP
#define INCLUDED_PROSODY_TDMRX_HPP

#include <string.h>

#include "prosody_error.hpp"

namespace Prosody {

#include "smbesp.h"
#include "smdrvr.h"
#include "smdc.h"

class Tdmrx {
	tSMTDMrxId tdmrx_;
	Tdmrx(Tdmrx &);
	Tdmrx &operator=(Tdmrx &) const;
public:
	Tdmrx() : tdmrx_(kSMNullTDMrxId) { }
	operator tSMTDMrxId() { return tdmrx_; }
	Error free() {
		return Error(0);
	}
	~Tdmrx() {
		free().ok();
	}
#include "gen/prosgen-tdmrx.hpp"
};

}

#endif
