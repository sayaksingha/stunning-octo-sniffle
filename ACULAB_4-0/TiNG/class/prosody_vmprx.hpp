#ifndef INCLUDED_PROSODY_VMPRX_HPP
#define INCLUDED_PROSODY_VMPRX_HPP

#include <string.h>

#include "prosody_error.hpp"

namespace Prosody {

#include "smrtp.h"

class Vmprx {
	tSMVMPrxId vmprx_;
	Vmprx(Vmprx &);
	Vmprx &operator=(Vmprx &) const;
public:
	Vmprx() : vmprx_(kSMNullVMPrxId) { }
	operator tSMVMPrxId() { return vmprx_; }
	Error free() {
		return Error(0);
	}
	~Vmprx() {
		free().ok();
	}
#include "gen/prosrtpapi-vmprx.hpp"
};

}

#endif
