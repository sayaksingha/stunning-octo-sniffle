#ifndef INLCUDED_MODOPEN_H
#define INLCUDED_MODOPEN_H

#include "smdrvr.h"
#include "error.h"

err_t modopen(tSMCardId *card_idp, tSMModuleId *modp, char *spec, unsigned modno);
err_t modclose(tSMModuleId mod);

#endif
