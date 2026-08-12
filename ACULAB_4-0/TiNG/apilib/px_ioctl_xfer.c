/* px_ioctl_xfer.c - ioctl code for read/write xfer */

#include "px_ioctl_xfer.h"
#include "assp_conn.h"
#include "px_event.h"
#include "px_util.h"
#include "../hsif/kernel.h"
#include "../hsif/kernel_x.h"
#include "TiNGcommon.h"
#include "trace.h"

LOCALDEF int px_rdxfer(PX_CHAN *pxchan, struct ting_xfer *p)
{
 (void) pxchan;
 (void) p;
 if (TiNGtrace) olog("rdxfer: not implemented\n");
 return 1;
}

LOCALDEF int px_wrxfer(PX_CHAN *pxchan, struct ting_xfer *p)
{
 if (pxchan->ctstate == CT_RUNNING) {
	ASSP_GROUP *grp = pxchan->chancx->grp;
	lock_object(grp);
	if (p->minimum_bits == 0) {
		pxchan->no_capacity_ev = 1;
	} else {
		pxchan->no_capacity_ev = 0;
 		pxchan->minready_set = p->minimum_bits;
 		pxchan->timeout_milliseconds = p->timeout_milliseconds;
		if (pxchan->minready_set < 0) {
			pxchan->minready = pxchan->dspbufsize + pxchan->minready_set;
		} else {
			pxchan->minready = pxchan->minready_set;
		}
		update_minready(pxchan);
	}
	px_update_event(pxchan);
	if (p->nomore && asspgroup_isconnected(grp)) {
		PACKET *pkt;
		pxchan->minready = p->minimum_bits;
		pkt = task_txpacket(grp, pxchan->chancx, 4);
		pkt->dataend = mkmsg(pkt->dataend, MESSAGE(MSG_NOMORE, 1) | TASK_GENID_S(pxchan->chancx->taskgen, pxchan->chancx->taskid));
		assp_conn_send(grp->conn, pkt);
	}
	unlock_object(grp);
 }
 return 0;
}
