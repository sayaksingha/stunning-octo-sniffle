/* util.hpp - miscellaneous small useful functions */

#ifndef INCLUDED_UTIL_HPP
#define INCLUDED_UTIL_HPP

#include "../../class/error_class.hpp"

namespace Prosody_Test {
#include "../Testlib/error.h"
}

error err_t2error(Prosody_Test::err_t err);

#endif
