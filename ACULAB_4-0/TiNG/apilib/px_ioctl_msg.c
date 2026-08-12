/* px_ioctl_msg.c - ioctl code for read/write msg */

#include "px_ioctl_msg.h"
#include "../hsif/kernel.h"
#include "../hsif/kernel_x.h"
#include "assp_conn.h"
#include "px_event.h"
#include "px_util.h"
#include "TiNGcommon.h"
#include "trace.h"

LOCALDEF void unsafe_px_rdmsg(PX_CHAN *pxchan, struct ting_message *p)
{
 ASSP_CHAN *cxchan = pxchan->chancx;
 p->msg[0] = 0;
 for (;;) {
	PACKET *pkt = cxchan->rxq;
	if (!pkt) {
		if (pxchan->ctstate == CT_IDLE) {
				// fake DEAD message
			p->msg[0] = MESSAGE(MSG_DEAD, 1);
			if (TiNGtrace > 3) olog("%p: fake DEAD\n", pxchan);
			handle_dead(pxchan);
			// printf("fake DEAD\n");
			break;
		}
		if (!asspgroup_isconnected(cxchan->grp)) {
				// fake DEAD message
			p->msg[0] = MESSAGE(MSG_DEAD, 2);
			p->msg[1] = MSG_DEAD_CAUSE_DISCO;
			if (TiNGtrace > 3) olog("%p: fake DEAD - disconnected\n", pxchan);
			handle_dead(pxchan);
			// printf("fake DEAD\n");
			break;
		}
		if (p->flags & IOCTL_TiNG_RDMSG_FLAG_NOWAIT) {
			// printf("nowait\n");
			break;
		}
		prep_unlock_object(cxchan->grp);
		cv_wait(&cxchan->rxcv, &cxchan->grp->pm.mx);
		prep_lock_object(cxchan->grp);
	} else {
		uint8_t *dp = pkt->datastart;
		if (pkt->dataend - dp < 4) {
			// empty - discard it
			// printf("empty\n");
			cxchan->rxq = pkt->next;
			if (!cxchan->rxq) cxchan->rxq_last = &cxchan->rxq;
			assp_conn_free_packet(cxchan->grp->conn, pkt);
		} else {
			uint32_t msg = getmsg(&dp);
			if (msg & 0x80000000) {
				unsigned msglen = MSG2LEN(msg);
				if (!msglen) msglen = 16;
				if (MSG2TASK_GEN(msg) != cxchan->taskgen) {
						// discard it
					pkt->datastart += 4 * msglen;
					// printf("wrong gen\n");
				} else {
					unsigned u;
					for (u=0; u < msglen; u++) {
						p->msg[u] = getmsg(&pkt->datastart);
					}
					// printf("read msg 0x%08lx\n", p->msg[0]);
					if (MSG2CODE(msg) == MSG2CODE(MESSAGE(MSG_READY, 4))) {
						pxchan->chandir = p->msg[3] ? CD_WRITE : CD_READ;
						pxchan->dspbufsize = (p->msg[2] - p->msg[1]) * 32;
						if (pxchan->minready_set < 0) {
							pxchan->minready = pxchan->dspbufsize + pxchan->minready_set;
						} else {
							pxchan->minready = pxchan->minready_set;
						}
					} else if (MSG2CODE(msg) >> 4 == MSG_DEAD) {
						if (TiNGtrace > 3) olog("%p: got DEAD cause %x\n", pxchan, p->msg[1]);
						handle_dead(pxchan);
					}
					if (pkt->dataend - pkt->datastart < 4) {
						// read last message in packet, so discard it
						cxchan->rxq = pkt->next;
						if (!cxchan->rxq) cxchan->rxq_last = &cxchan->rxq;
						assp_conn_free_packet(cxchan->grp->conn, pkt);
					}
					break;
				}
			} else {	// DATA message
				if (msg & 0x7fff0000) { // SPACE message
					pxchan->writecap += msg >> 16 >> 3;	// bytes
					pkt->datastart = dp;
				} else { // PAYLOAD message
					unsigned payloadlen;
					msg = getmsg(&dp);
					payloadlen = msg & 0xffff;
					msg >>= 16;
					if (msg == 0) { // real DATA message
						if (pxchan->chandir == CD_READ && payloadlen) break;
						// discard data if not reading!
					} else if (msg == 1) { // string
						uint8_t *tmp_dp = dp;
						msg = getmsg(&tmp_dp);
						if (p->flags & IOCTL_TiNG_RDMSG_FLAG_KEEPSTRINGS) {
							p->msg[0] = MESSAGE(MSG_STRING, 3);
							p->msg[1] = msg;
							p->msg[2] = payloadlen - 4; // less 4 for string id
							break;
						} else {
							if (TiNGtrace > 3) olog("%p: discarding STRING message %x\n", pxchan, msg);
						}
						
					}
					// discard payload
					if (payloadlen & 3) payloadlen += 4 - (payloadlen & 3);
					dp += payloadlen;
					pkt->datastart = dp;
				}
			}
		}
	}
 }
}

LOCALDEF int px_rdmsg(PX_CHAN *pxchan, struct ting_message *p)
{
 ASSP_GROUP *grp = pxchan->chancx->grp;
 lock_object(grp);
 unsafe_px_rdmsg(pxchan, p);
 p->hasmsg = capacity(&p->capacity, pxchan);
 // printf("cap=%d\n", p->capacity);
 px_update_event(pxchan);
 unlock_object(grp);
 return 0;
}

LOCALDEF int px_wrmsg(PX_CHAN *pxchan, struct ting_message *p)
{
 ASSP_GROUP *grp = pxchan->chancx->grp;
 if (!(p->msg[0] & 0x80000000)) die("Invalid message\n");
 if (pxchan->ctstate == CT_RUNNING) {
	unsigned len = MSG2LEN(p->msg[0]);
	unsigned u;
	if (!len) len = 16;
	lock_object(grp);
 	if (asspgroup_isconnected(grp)) {
		PACKET *pkt = task_txpacket(grp, pxchan->chancx, 4*17);
		pkt->dataend = mkmsg(pkt->dataend, (p->msg[0] & ~0xffff) | TASK_GENID_S(pxchan->chancx->taskgen, pxchan->chancx->taskid));
		for (u=1; u < len; u++) {
			pkt->dataend = mkmsg(pkt->dataend, p->msg[u]);
		}
		assp_conn_send(grp->conn, pkt);
	} else {
		if (TiNGtrace > 1) olog("px_wrmsg: discarding tx msg - not connected");
	}
	unlock_object(grp);
 } else {
 	if (TiNGtrace > 1) olog("px_wrmsg: discarding tx msg - not running");
 }
 return 0;
}
