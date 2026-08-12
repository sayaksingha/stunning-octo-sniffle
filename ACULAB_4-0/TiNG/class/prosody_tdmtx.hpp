#ifndef INCLUDED_PROSODY_TDMTX_HPP
#define INCLUDED_PROSODY_TDMTX_HPP

#include <string.h>

#include "prosody_error.hpp"

namespace Prosody {

#include "smbesp.h"
#include "smdrvr.h"
#include "smdc.h"

class Tdmtx {
	tSMTDMtxId tdmtx_;
	Tdmtx(Tdmtx &);
	Tdmtx &operator=(Tdmtx &) const;
public:
	Tdmtx() : tdmtx_(kSMNullTDMtxId) { }
	operator tSMTDMtxId() { return tdmtx_; }
	Error free() {
		return Error(0);
	}
	~Tdmtx() {
		free().ok();
	}
#include "gen/prosgen-tdmtx.hpp"
};

}

#endif
