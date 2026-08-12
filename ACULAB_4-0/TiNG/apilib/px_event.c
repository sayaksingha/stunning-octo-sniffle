/* px_event.c - PX event internals */

#include "px_event.h"
#include "TiNGcommon.h"
#include "px_util.h"
#include "trace.h"
#include "../hsif/kernel.h"
#include "../hsif/kernel_x.h"
#include <string.h>

LOCALDEF void update_minready(PX_CHAN *pxchan)
{
 ASSP_GROUP *grp = pxchan->chancx->grp;
 if (asspgroup_isconnected(grp) && pxchan->ctstate == CT_RUNNING) {
	int mr = pxchan->writecap * 8;	// convert to bits
	if (!pxchan->timeout_milliseconds && mr + 0u < pxchan->minready) mr = pxchan->minready - mr;
	else mr = pxchan->minready;
	if (mr != pxchan->minready_dsp
		|| pxchan->timeout_milliseconds != pxchan->timeout_dsp) {
		PACKET *txp = task_txpacket(grp, pxchan->chancx, 4*(1+3));
		txp->dataend = mkmsg(txp->dataend, MESSAGE(MSG_SET_MINREP, 3) | TASK_GENID_S(pxchan->chancx->taskgen, pxchan->chancx->taskid));
		txp->dataend = mkmsg(txp->dataend, mr);
		txp->dataend = mkmsg(txp->dataend, pxchan->timeout_milliseconds);
		assp_conn_send(grp->conn, txp);
		pxchan->minready_dsp = mr;
		pxchan->timeout_dsp = pxchan->timeout_milliseconds;
	}
 }
}

LOCALDEF int more_writecap(PX_CHAN *pxchan)
{
 int seen_space = 0;
 ASSP_CHAN *chancx = pxchan->chancx;
 PACKET **prevp = &chancx->rxq;
 for (;;) {
	PACKET *rxp = *prevp;
	uint8_t *dp;
	if (!rxp) break;
	dp = rxp->datastart;
	for (;;) {
		uint32_t msg;
		if (rxp->dataend - dp < 4) break;	// no more
		msg = getmsg(&dp);
		if (msg & 0x80000000) return seen_space;
		if (msg & 0x7fff0000) {		// SPACE
			pxchan->writecap += MSG2CODE(msg) >> 3;	// bytes
			//printf("DATA: writecap=%d\n", pxchan->writecap);
			rxp->datastart = dp;
			seen_space = 1;
		} else return seen_space;	// DATA
	}
	if (rxp->dataend - rxp->datastart < 4) {
		ASSP_GROUP *grp = chancx->grp;
		// empty - discard it
		*prevp = rxp->next;
		if (!rxp->next) chancx->rxq_last = prevp;
		assp_conn_free_packet(grp->conn, rxp);
	} else {
		prevp = &rxp->next;
	}
 }
 return seen_space;
}

// return: 0 if no message waiting, non-zero otherwise
LOCALDEF int capacity(uint32_t *cap, PX_CHAN *pxchan)
{
 if (pxchan->chandir == CD_WRITE) {
	if (more_writecap(pxchan)) update_minready(pxchan);
 	*cap = pxchan->writecap << 3;	// convert to bits
 	// printf("writecap -> %d\n", *cap);
 	return !!pxchan->chancx->rxq;
 } else {
	PACKET *rxp = pxchan->chancx->rxq;
	*cap = 0;
	for (;;) {
		uint8_t *dp;
		uint32_t msg;
		if (!rxp) {
			if (pxchan->ctstate == CT_IDLE || !asspgroup_isconnected(pxchan->chancx->grp)) {
				return 1; // fake DEAD message is 'pending'
			}
			return 0;
		}
		//olog("id %d: have DATA\n", pxchan->chancx->taskid);
		dp = rxp->datastart;
		for (;;) {
			if (rxp->dataend - dp < 4) break;
			msg = getmsg(&dp);
			if (msg & 0x80000000) return 1; // standard message
			if (msg & 0x7fff0000) {		// SPACE message
				*cap += MSG2CODE(msg);
				// printf("cap(%d)\n", MSG2CODE(msg));
			} else {			// DATA message
				if (rxp->dataend - dp < 4) {
					//bogus packet
					// FIXME: get rid of it
				}
				msg = getmsg(&dp);	// length field
				if (msg & 0xffff0000) return 1; // not a PAYLOAD message, could be string etc.
				*cap += msg << 3;	// converted to bits
				// printf("cap(%d)\n", msg << 3);
				dp += msg;		// skip the data
				if (msg & 3) dp += 4 - (msg & 3);	// add padding
			}
		}
		rxp = rxp->next;
 	}
 }
}

LOCALDEF int px_update_event(PX_CHAN *pxchan)
{
 int evstate = 0;
 if (pxchan->ctstate == CT_IDLE) {
 	//olog("ctstate == IDLE\n");
 	evstate = 1;
 } else if (!asspgroup_isconnected(pxchan->chancx->grp)) {
	evstate = 1;
	//olog("connection not running\n");
 } else {
 	unsigned cap;
	unsigned min_cap = (pxchan->timeout_milliseconds) ? 1u : pxchan->minready; // with a timeout any data makes channel ready
	if (capacity(&cap, pxchan)) {
		//olog("id %d has msg\n", pxchan->chancx->taskid);
		evstate = 1;
	}
	if (cap >= min_cap && pxchan->ctstate == CT_RUNNING) {
		if (pxchan->no_capacity_ev == 0) {
			//olog("id: %d cap (%d) >= minready (%d)\n", pxchan->chancx->taskid, cap, pxchan->minready);
			evstate = 1;
		} else {
			//olog("id: %d cap (%d) >= minready (%d), data ready event disabled\n", pxchan->chancx->taskid, cap, pxchan->minready);
		}
	} else {
		//olog("id %d cap (%d) - minready (%d)\n", pxchan->chancx->taskid, cap, pxchan->minready);
	}
 }
#ifdef PX_CHAN_EVENT_DURATION_LOGGING
 px_chan_event_log(pxchan, evstate);
#endif
 if (evstate) {
	if (evnt_signal(pxchan->chancx->cxevnt) && TiNGtrace) olog("failed to signal chan evnt");
 } else evnt_clear(pxchan->chancx->cxevnt);
 return evstate;
}
