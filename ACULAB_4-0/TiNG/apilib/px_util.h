#ifndef INCLUDED_PX_UTIL_H
#define INCLUDED_PX_UTIL_H

#include <stdint.h>

#include "assp_group.h"

LOCALDEC uint8_t *mkmsg(uint8_t *sp, uint32_t msg);
LOCALDEC uint32_t getmsg(uint8_t **sp);
LOCALDEC unsigned char *msgitemend(uint32_t msg, unsigned char *sp);
LOCALDEC PACKET *task_txpacket(ASSP_GROUP *grp, ASSP_CHAN *cxchan, uint8_t size);

#endif
