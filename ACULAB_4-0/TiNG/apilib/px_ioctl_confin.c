/* px_ioctl_confin.c - ioctl code for read/write confin */

#include "px_ioctl_confin.h"
#include "px_ioctl_msg.h"
#include "../hsif/conf.h"
#include "../hsif/kernel.h"
#include "../hsif/kernel_x.h"
#include "TiNGcommon.h"
#include "px_util.h"
#include "trace.h"

LOCALDEF int px_rdconfin(PX_CHAN *pxchan, struct ting_confin *p)
{
 (void) pxchan;
 (void) p;
 if (TiNGtrace) olog("rdconfin: not implemented\n");
 return 1;
}

LOCALDEF int px_wrconfin(PX_CHAN *pxchan, struct ting_confin *p)
{
 ASSP_GROUP *grp = pxchan->chancx->grp;
 int sts = 0;
 lock_object(grp);
 if (!asspgroup_isconnected(grp)) {
	if (TiNGtrace) olog("wrconfin: connection closed");
	sts = 1;
 } else {
	ASSP_CHAN *fchancx = channomagic2chan(grp, p->channo, p->magic);
 	PACKET *pkt = task_txpacket(grp, pxchan->chancx, 4*3);
 	pkt->dataend = mkmsg(pkt->dataend, MESSAGE(MSG_CONFMAN, 3) | TASK_GENID_S(pxchan->chancx->taskgen, pxchan->chancx->taskid));
 		//NOTE: pxchan->chancx and fchancx must be on the same ASSP_CONN
 	pkt->dataend = mkmsg(pkt->dataend, fchancx ? TASK_GENID_L(fchancx->taskgen, fchancx->taskid) : 0);
	pkt->dataend = mkmsg(pkt->dataend, p->id);
	assp_conn_send(grp->conn, pkt);
 }
 unlock_object(grp);
 return sts;
}
