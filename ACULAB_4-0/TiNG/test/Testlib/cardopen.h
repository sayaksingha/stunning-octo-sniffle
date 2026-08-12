#ifndef INLCUDED_CARDOPEN_H
#define INLCUDED_CARDOPEN_H

#include "smdrvr.h"
#include "error.h"

err_t cardopen(tSMCardId *cardp, char *card);
err_t cardclose(tSMCardId card);

#endif
