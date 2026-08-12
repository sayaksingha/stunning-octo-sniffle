#include "px_util.h"
#include "assp_group.h"
#include "../hsif/kernel.h"
#include "../hsif/kernel_x.h"

LOCALDEF uint8_t *mkmsg(uint8_t *sp, uint32_t msg)
{
 *sp++ = msg;
 *sp++ = msg >> 8;
 *sp++ = msg >> 16;
 *sp++ = msg >> 24;
// printf("msg %02x%02x%02x%02x\n",*(sp-4),*(sp-3),*(sp-2),*(sp-1));
 return sp;
}

LOCALDEF uint32_t getmsg(uint8_t **spp)
{
 uint8_t *sp = *spp;
 uint32_t m;
 m = *sp++;
 m |= *sp++ << 8;
 m |= *sp++ << 16;
 m |= *sp++ << 24;
 *spp = sp;
 return m;
}

LOCALDEF unsigned char *msgitemend(uint32_t msg, unsigned char *sp)
{
 if (msg & 0x80000000) {	// a message
 	unsigned msglen = MSG2LEN(msg);
 	if (!msglen) msglen = 17;
 	return sp + 4 * msglen - 4;
 }
 if (msg & 0xffff0000) {
 	return sp;	// SPACE
 }
 msg = getmsg(&sp) & 0xffff;	// DATA or string, ignore the type
 msg = (msg + 3) & ~3;		// add any padding
 return sp + msg;
}

LOCALDEF PACKET *task_txpacket(ASSP_GROUP *grp, ASSP_CHAN *cxchan, uint8_t size)
{
 PACKET *p = assp_conn_new_packet(grp->conn, size + 4*2);
 if (p) {
	uint32_t txb = TASKID2EXTRA(cxchan->taskid);
		// NB: this means that a cxchan must only send messages to
		// a) its own task
		// b) task 0
		// otherwise we will fill in the wrong EXTEND value
	if (txb != cxchan->grp->tx_extrabits) {
		cxchan->grp->tx_extrabits = txb;
		p->dataend = mkmsg(p->dataend, MESSAGE(MSG_TASKID_EXTEND, 2));
		p->dataend = mkmsg(p->dataend, txb);
	}
 } 
 return p;
}
