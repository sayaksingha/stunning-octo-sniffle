/* txqueue.c - tx queueing stuff */

#include <stdio.h>
#include "txqueue.h"

#ifdef CHECK_TXQ
#include <stdlib.h>
#define Log printf

void txq_verify(TXQUEUE *qp, enum txstatus sts)
{
 unsigned qlen = 0;
 PACKET *prev = 0;
 PACKET *txp;
 for (prev = 0, txp = qp->first; txp; txp = txp->next) {
	if (txp->u.tx.status != sts) {
		Log("Internal error: on txq: status (%d) != %d\n",
			txp->u.tx.status, sts);
		abort();
	}
	if (txp->u.tx.qprev != prev) {
		Log("Internal error: on txq: u.tx.qprev (%p) != %p\n",
			txp->u.tx.qprev, prev);
		abort();
	}
	if (prev) {
		if (!qp->okorder(prev, txp)) {
			Log("Internal error: on txq: prev (%p)->seq (%d) >= %p->seq (%d)\n",
				prev, prev->seq, txp, txp->seq);
			abort();
		}
	}
	prev = txp;
	qlen++;
 }
 if (qp->last != prev) {
	Log("Internal error: txq.last (%p) != %p\n",
		qp->last, prev);
	abort();
 }
 if (qp->qlen != qlen) {
	Log("Internal error: qlen (%d) != %d\n",
		qp->qlen, qlen);
	abort();
 }
}

void txq_require(TXQUEUE *qp, PACKET *txp)
{
 PACKET *tqp;
 for (tqp = qp->first; tqp != txp; tqp = tqp->next) {
	if (!tqp) {
		Log("Internal error: not on txq: %p->seq (%d)\n",
			txp, txp->seq);
		abort();
	}
 }
}

#endif

void txq_clear(TXQUEUE *qp)
{
 qp->first = qp->last = 0;
 qp->qlen = 0;
}

void txq_init(TXQUEUE *qp, int (*okorder)(PACKET *l, PACKET *r))
{
 txq_clear(qp);
 qp->okorder = okorder;
}

void txq_dequeue(TXQUEUE *qp, PACKET *tp)
{
 if (tp->next) tp->next->u.tx.qprev = tp->u.tx.qprev;
 else qp->last = tp->u.tx.qprev;
 if (tp->u.tx.qprev) tp->u.tx.qprev->next = tp->next;
 else qp->first = tp->next;
 qp->qlen--;
}

void txq_enqueue(TXQUEUE *qp, PACKET *pkt)
{
 PACKET *inspos = qp->last;
 for (;;) {
		// all packets after inspos come after pkt
	if (!inspos) {
		pkt->next = qp->first;
		qp->first = pkt;
		break;
	}
	if (qp->okorder(inspos, pkt)) {
			// insert after inspos
		pkt->next = inspos->next;
		inspos->next = pkt;
		break;
	}
	inspos = inspos->u.tx.qprev;
 }
 pkt->u.tx.qprev = inspos;
 if (pkt->next) pkt->next->u.tx.qprev = pkt;
 else qp->last = pkt;
 qp->qlen++;
}
