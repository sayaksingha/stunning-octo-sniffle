#ifndef INCLUDED_COOKIE_H
#define INCLUDED_COOKIE_H

#include "assp.h"
#include "assp_impl.h"

#define ERR_COOKIE_EXPIRED -1

void cookie_init(void);
uint8_t *bake_cookie(TCB *tcb, uint8_t *sp, RPACKET *pkt);
int nice_cookie(TCB *tcb, PACKET *pkt, uint8_t *scoo, uint8_t *ecoo);

#endif
