#ifndef INCLUDED_PROSODY_PATH_HPP
#define INCLUDED_PROSODY_PATH_HPP

#include <string.h>

#include "prosody_error.hpp"

namespace Prosody {

#include "smpath.h"

class Path {
	tSMPathId path_;
public:
	Path() : path_(kSMNullPathId) { }
	operator tSMPathId() { return path_; }
#include "gen/prospath-path.hpp"
};

}

#endif
