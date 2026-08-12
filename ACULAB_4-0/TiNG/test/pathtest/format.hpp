#ifndef INCLUDED_FORMAT_HPP
#define INCLUDED_FORMAT_HPP

#include "../../class/prosody.hpp"

char *parseformat(Prosody::kSMDataFormat *code, char *name);
const char *formatname(Prosody::kSMDataFormat code);

#endif
