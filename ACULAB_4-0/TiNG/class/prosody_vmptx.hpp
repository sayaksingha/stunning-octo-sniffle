#ifndef INCLUDED_PROSODY_VMPTX_HPP
#define INCLUDED_PROSODY_VMPTX_HPP

#include <string.h>

#include "prosody_error.hpp"

namespace Prosody {

#include "smrtp.h"

class Vmptx {
	tSMVMPtxId vmptx_;
	Vmptx(Vmptx &);
	Vmptx &operator=(Vmptx &) const;
public:
	Vmptx() : vmptx_(kSMNullVMPtxId) { }
	operator tSMVMPtxId() { return vmptx_; }
	Error free() {
		return Error(0);
	}
	~Vmptx() {
		free().ok();
	}
#include "gen/prosrtpapi-vmptx.hpp"
};

}

#endif
