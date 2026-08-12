#ifndef INCLUDED_PROS_X_H
#define INCLUDED_PROS_X_H

#include "smcore.h"
#include "prosody_x.h"

#ifdef __cplusplus
extern "C" {
#endif

LOCALDEC int real_sm_open_prosody_x(SM_OPEN_PROSODY_X_PARMS *openp);
LOCALDEC int real_sm_open_prosody_s_v3(SM_OPEN_PROSODY_S_V3_PARMS *openp);

#ifdef __cplusplus
}
#endif

#endif
