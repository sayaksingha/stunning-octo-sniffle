/* px_ioctl_friend.c - ioctl code for read/write friend */


#include "px_ioctl_friend.h"
#include "../hsif/kernel.h"
#include "TiNGcommon.h"
#include "px_util.h"
#include "trace.h"
#include "assp_conn.h"
#include "px_event.h"
#include "px_util.h"
#include "../hsif/kernel.h"
#include "../hsif/kernel_x.h"

LOCALDEF int px_rdfriend(PX_CHAN *pxchan, struct ting_friend *p)
{
 (void) pxchan;
 (void) p;
 if (TiNGtrace) olog("rdfriend: not implemented\n");
 return 1;
}

LOCALDEF int px_wrfriend(PX_CHAN *pxchan, struct ting_friend *p)
{
 ASSP_GROUP *grp = pxchan->chancx->grp;
 int hastype = !!p->type;
 int sts = 0;
 lock_object(grp);
 if (asspgroup_isconnected(grp) && pxchan->ctstate == CT_RUNNING) {
	ASSP_CHAN *fchancx = channomagic2chan(grp, p->channo, p->magic);
	PACKET *pkt = task_txpacket(grp, pxchan->chancx, 4*3);
	pkt->dataend = mkmsg(pkt->dataend, MESSAGE(MSG_FRIEND, 2+hastype) | TASK_GENID_S(pxchan->chancx->taskgen, pxchan->chancx->taskid));
	pkt->dataend = mkmsg(pkt->dataend, fchancx ? TASK_GENID_L(fchancx->taskgen, fchancx->taskid) : 0);
	if (hastype) pkt->dataend = mkmsg(pkt->dataend, p->type);
	assp_conn_send(grp->conn, pkt);
 }
 unlock_object(grp);
 return sts;
}
