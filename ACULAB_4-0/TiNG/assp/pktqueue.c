/* pktqueue.c - packet queues in ASSP implementation */

#include <stdio.h>
#include "pktqueue.h"

#if 0
void rxq_show(RXQUEUE *q)
{
 PACKET *p = q->frun;
 for (; p;) {
	PACKET *rp = p->u.rx.next;
	uint32_t seq = p->seq;
	unsigned rlen = 1;
	for (; rp; rp = rp->u.rx.next) {
		seq++;
		if (p == rp || p->u.rx.runnext == rp) {
			Log(0, "Internal error: rx %d on two runs\n",
				p->seq);
			abort();
		}
		if (rp->seq != seq) {
			Log(0, "Internal error: rx %d != %d\n",
				rp->seq, seq);
			abort();
		}
		rlen++;
	}
	printf("\t%d: %d\n", p->seq, p->u.rx.runlen);
	if (rlen != p->u.rx.runlen) {
		Log(0, "Internal error: runlen %d != %d\n",
			p->u.rx.runlen, rlen);
		abort();
	}
 	p = p->u.rx.runnext;
	if (p && (int32_t) (p->seq - seq) <= 0) {
		Log(0, "Internal error: rxq next %d >= %d\n",
			p->seq, seq);
		abort();
	}
 }
}
#endif

int enqueue(RXQUEUE *q, PACKET *p)
{
 PACKET **prevp = &q->frun;
 for (;;) {
	PACKET *rhp = *prevp;	// head of next run
	int32_t d;
	if (!rhp) {
		p->u.rx.runlen = 1;
		p->u.rx.runnext = 0;
		p->u.rx.runlast = p;
		p->next = 0;
		*prevp = p;
		return 0;
	}
	d = p->seq - rhp->seq;
	if ((unsigned) d == rhp->u.rx.runlen) { // next after the end of a run
		rhp->u.rx.runlast->next = p;
		rhp->u.rx.runlast = p;
		rhp->u.rx.runlen++;
		p->next = 0;
		p = rhp->u.rx.runnext;	// now see if we filled a gap
		if (p && rhp->u.rx.runlast->seq + 1 == p->seq) {
				// merge two runs which are now contiguous
			rhp->u.rx.runlast->next = p;
			rhp->u.rx.runlast = p->u.rx.runlast;
			rhp->u.rx.runnext = p->u.rx.runnext;
			rhp->u.rx.runlen += p->u.rx.runlen;
		}
		return 0;
	} else if (d < 0) {	// goes before rhp
		*prevp = p;
		if (d == -1) {	// just before this - it's new head of run
			p->next = rhp;
			p->u.rx.runnext = rhp->u.rx.runnext;
			p->u.rx.runlen = rhp->u.rx.runlen + 1;
			p->u.rx.runlast = rhp->u.rx.runlast;
				/* note - can't have filled a gap
				 * as then it would have been appended
				 * to the previous run
				 */
		} else {	// it's in a run by itself
			p->next = 0;
			p->u.rx.runnext = rhp;
			p->u.rx.runlen = 1;
			p->u.rx.runlast = p;
		}
		return 0;
	} else if ((unsigned) d < rhp->u.rx.runlen) {
		return 1;	// dup
	}
		// it comes after the end of this run - try the next
	prevp = &rhp->u.rx.runnext;
 }
}
