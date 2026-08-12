#ifndef INCLUDED_PROSODY_MODULE_HPP
#define INCLUDED_PROSODY_MODULE_HPP

#include <string.h>

#include "prosody_error.hpp"

namespace Prosody {

#include "smbesp.h"
#include "smdrvr.h"
#include "smdc.h"

class Module {
	tSMModuleId module_;
	Module(Module &);
	Module &operator=(Module &) const;
public:
	Module() : module_(kSMNullModuleId) { }
	operator tSMModuleId() { return module_; }
	Error free() {
		int rc = 0;
		if (module_ != kSMNullModuleId) {
			SM_CLOSE_MODULE_PARMS mp;
			memset(&mp, 0, sizeof(mp));
			mp.module_id = module_;
			rc = sm_close_module(&mp);
			module_ = kSMNullModuleId;
		}
		return Error(rc);
	}
	~Module() {
		free().ok();
	}
#include "gen/prosgen-module.hpp"
#include "gen/prospapi-module.hpp"
};

}

#endif
