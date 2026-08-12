/* pktqueue.h - packet queues in ASSP implementation */

#ifndef DEFINED_PKTQUEUE_H
#define DEFINED_PKTQUEUE_H

#include "assp.h"

int enqueue(RXQUEUE *q, PACKET *p);
void make_wtrdns(TCB *tcb, RXQUEUE *q, char *op, unsigned len, unsigned win);

#endif
