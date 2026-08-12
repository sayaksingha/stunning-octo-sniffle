/* px_ioctl_task.c - ioctl code for read/write task */

#include "px_ioctl_task.h"
#include "../hsif/kernel.h"
#include "../hsif/kernel_x.h"
#include "TiNGcommon.h"
#include "assp_conn.h"
#include "px_event.h"
#include "px_util.h"
#include "trace.h"

LOCALDEF int px_rdtask(PX_CHAN *pxchan, struct ting_task *p)
{
 (void) pxchan;
 (void) p;
 if (TiNGtrace) olog("rdtask: not implemented\n");
 return 1;
}

LOCALDEF int px_wrtask(PX_CHAN *pxchan, struct ting_task *p)
{
 ASSP_CHAN *chancx = pxchan->chancx;
 ASSP_GROUP *grp = chancx->grp;
 PACKET *pkt;
 int sts = 0;
 lock_object(grp);
 pkt = task_txpacket(grp, chancx, 4*(1+3));	// DEAD + CREATE
 if (pxchan->ctstate == CT_RUNNING) {
	pkt->dataend = mkmsg(pkt->dataend, MESSAGE(MSG_DEAD, 1) | TASK_GENID_S(chancx->taskgen, chancx->taskid));
	pxchan->ctstate = CT_STOPPING;
 }
 if (p->tasktypecode) {		// start a new task
	PACKET *d;
	handle_dead(pxchan); // the new task prevents the last task dead being read, so handle it now
		// any pending messages/data will be for the old task, so we can discard them
	while ((d = chancx->rxq) != 0) {
		chancx->rxq = d->next;
		assp_conn_free_packet(chancx->grp->conn, d);
	}
	chancx->rxq_last = &chancx->rxq;

	if (task_genisactive(chancx)) {
		sts = err(ERR_SM_DEVERR);
		pkt->dataend = pkt->datastart; // discard without sending DEAD to existing task
	} else {
		if (TiNGtrace > 2) {
			uint_least32_t  code = p->tasktypecode;
			olog("%p: grp=%p starting task type %08x (%c%c%c%c), tid=%08x",
				pxchan, grp, code,
				code & 0xff, (code>>8) & 0xff,
				(code>>16) & 0xff, ((code>>24) & 0xff) ? ((code>>24) & 0xff) : ' ',
				TASK_GENID_L(chancx->taskgen, chancx->taskid));
		}
			//Aimed at task zero
		pkt->dataend = mkmsg(pkt->dataend, MESSAGE(MSG_CREATE, 3));
		pkt->dataend = mkmsg(pkt->dataend, TASK_GENID_L(chancx->taskgen, chancx->taskid));
		pkt->dataend = mkmsg(pkt->dataend, p->tasktypecode);
		pxchan->ctstate = CT_RUNNING;
		task_activegen(chancx);
	}
 }
 if (pkt->dataend == pkt->datastart || !asspgroup_isconnected(grp)) {
		// nothing to do - discard the packet
	assp_conn_free_packet(grp->conn, pkt);
 } else {
	assp_conn_send(grp->conn, pkt);
 }
 px_update_event(pxchan);
 unlock_object(grp);
 return sts;
}
