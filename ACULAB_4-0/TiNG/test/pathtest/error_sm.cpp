/* error_sm.cpp - convert Prosody error codes to error */

#include "../Testlib/errcode_sm.h"
#include "error_sm.hpp"

error error_sm(int code)
{
 return error(errcode_sm(code));
}
