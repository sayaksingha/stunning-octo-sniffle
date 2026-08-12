/* txqueue.h - tx queueing stuff */

#ifndef INCLUDED_TXQUEUE_H
#define INCLUDED_TXQUEUE_H

#include "assp.h"

// #define CHECK_TXQ

void txq_verify(TXQUEUE *qp, enum txstatus sts);
void txq_require(TXQUEUE *qp, PACKET *txp);
void txq_clear(TXQUEUE *qp);
void txq_init(TXQUEUE *qp, int (*okorder)(PACKET *l, PACKET *r));
void txq_dequeue(TXQUEUE *qp, PACKET *tp);
void txq_enqueue(TXQUEUE *qp, PACKET *tp);

#endif
