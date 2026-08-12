#include "cardconn_px.h"
#include "assp_conn.h"
#include "assp_group.h"
#include "create_socket.h"
#include "smtypes.h"
#include "cardconn.h"
#include "px_assp.h"
#include "px_conn.h"
#include "px_event.h"
#include "px_ioctl_xfer.h"
#include "px_ioctls.h"
#include "px_util.h"
#include "../cmndev.h"
#include "trace.h"
#include "TiNGo_datafeed.h"
#include "../hsif/kernel.h"
#include "../hsif/kernel_x.h"
#include "../hsif/typecode.h"
#include "../px_ppc/cardinfod/cardinfod_protocol.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef TiNGTYPE_LINUX
#define TiNG_POSIX_SOCKETS
#endif

#ifdef TiNGTYPE_QNX
#define TiNG_POSIX_SOCKETS
#endif


#ifdef TiNG_POSIX_SOCKETS
#define USE_CLOSE_ON_EXEC
#define USE_POLL
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#ifndef closesocket
#define closesocket(fd) close(fd)
#endif
#include "POSIX/socket.h"
#endif


#ifdef TiNGTYPE_WINNT
#define USE_SELECT
typedef int socklen_t;
#endif

#define arlen(x) (sizeof(x)/sizeof(*(x)))

STATIC ERRCODE px_setblock(CARDCONN *cx, int b)
{
 (void) cx;
 (void) b;
 return OK;
}

STATIC ERRCODE px_read(CARDCONN *cx, void *buf, unsigned nc, unsigned *nr)
{
 PX_CARDCONN_VTBL_DATA *pvp = cx->vtbl_data;
 ASSP_CHAN *chancx;
 PX_CHAN *pxchan;
 ERRCODE e;
 if (TiNGtrace > 2) olog("[read X %p %d", pvp, nc);
 if (pvp->dest_type != TiNG_DEST_CHANNEL) {
 	e.text = "Bad dest for read";
 	e.errnum = 0;
 	if (TiNGtrace > 1) olog(":err]");
 	return e;
 }
 pxchan = pvp->u.chan;
 *nr = 0;
 chancx = pxchan->chancx;
 lock_object(chancx->grp);
 for (; nc; ) {
 	PACKET *rxp = chancx->rxq;
 	if (!rxp) {
 		if (*nr) break;
		if (!asspgroup_isconnected(chancx->grp)) break;
		if (pxchan->ctstate == CT_IDLE) break;
 		// if (non-blocking) break;
		prep_unlock_object(chancx->grp);
		cv_wait(&chancx->rxcv, &chancx->grp->pm.mx);
		prep_lock_object(chancx->grp);
 	} else {
		uint8_t *dp = rxp->datastart;
		unsigned len = rxp->dataend - dp;
		if (len < 4) {
			// bogus or empty packet
		} else {
			unsigned char *nxd;	// next data item
			unsigned tlen;
			if (len < 8 || getmsg(&dp) & 0xffff0000) {
				// not data - must be msg or SPACE
				break;
			}
			tlen = getmsg(&dp);
			if (tlen & 0xffff0000) {
				// nonzero type: not data - maybe strings etc.
				break;
			}
			if (tlen > len - 8) tlen = len - 8;	// cope with bogus packet
				/* If the length is N, then there is
				 * (-N) mod 4 bytes of padding. Therefore, if
				 * we do not consume all the data, we must
				 * leave X bytes such that X mod 4 == N mod 4
				 * or, to more explicit, we must take a
				 * multiple of four bytes.
				 */
			nxd = dp + tlen;
			if (tlen & 3) nxd += 4 - (tlen & 3);	// add padding
				// now nxd points to the next data item
			if (tlen > nc) {
				unsigned touse = nc & ~3; // == 0 mod 4
				len = tlen - touse;
				tlen = touse;
			} else {	// use it all
				len = 0;	// length remaining
			}
			memcpy(buf, dp, tlen);
			dp += tlen;
			*nr += tlen;
			if (len) {
				rxp->datastart = dp -= 8;
				dp = mkmsg(dp, 0);	// fake data indicator
				mkmsg(dp, len);		// remainder
				break;
			}
			rxp->datastart = nxd;
			if (nxd < rxp->dataend) break;	// msg follows
			buf = (char *) buf + tlen;
			nc -= tlen;
		}
		chancx->rxq = rxp->next;
		if (!chancx->rxq) chancx->rxq_last = &chancx->rxq;
		assp_conn_free_packet(chancx->grp->conn, rxp);
	}
 }
 if (*nr && asspgroup_isconnected(chancx->grp)) {
	PACKET *txp = task_txpacket(chancx->grp, chancx, 64);
	unsigned bits = *nr << 3;
	for (; bits; ) {
		unsigned nb = bits;
		if (nb > 0x7fff) nb = 0x7fff;
		txp->dataend = mkmsg(txp->dataend, (nb << 16) | TASK_GENID_S(chancx->taskgen, chancx->taskid));
		bits -= nb;
	}
	assp_conn_send(chancx->grp->conn, txp);
 }
 px_update_event(pxchan);
 unlock_object(chancx->grp);
 if (TiNGtrace > 2) olog("->%d]", *nr);
 return OK;
}

STATIC ERRCODE px_write(CARDCONN *cx, void *buf, unsigned nc, unsigned *nw)
{
 PX_CARDCONN_VTBL_DATA *pvp = cx->vtbl_data;
 ASSP_CHAN *chancx;
 PX_CHAN *pxchan;
 ASSP_GROUP *grp;
 ERRCODE e;
 if (TiNGtrace > 2) olog("[write X %p %d", pvp, nc);
 if (pvp->dest_type != TiNG_DEST_CHANNEL) {
 	e.text = "Bad dest for write";
 	e.errnum = 0;
 	if (TiNGtrace > 1) olog(":err]");
 	return e;
 }
 pxchan = pvp->u.chan;
 *nw = 0;
 chancx = pxchan->chancx;
 grp = chancx->grp;
 lock_object(grp);
 	// if there is a message waiting, only accept data if we
 	// were over the threshold already. This allows for replay and data:
 	// replay: writes are attempted only when over the threshold, so
 	//	all writes pass this test, and go around the loop until
 	//	the data is written (which must be all of it)
 	// data: writes attempted because of messages will find the threshold
 	//	is not reached, so no data will be accepted. other writes
 	//	wait until all data has been accepted or a message is waiting.
 if (pxchan->writecap << 3 < pxchan->minready) {
	more_writecap(pxchan);
	if (pxchan->writecap << 3 < pxchan->minready && chancx->rxq) nc = 0;
 }
 for (; nc;) {
	if (!asspgroup_isconnected(grp)) break;
 	if (!pxchan->writecap) {
		more_writecap(pxchan);
		if (!pxchan->writecap) {
			if (*nw || chancx->rxq) break;
			// if (non-blocking) break;
			prep_unlock_object(grp);
			cv_wait(&chancx->rxcv, &grp->pm.mx);
			prep_lock_object(grp);
		}
	} else {
		PACKET *txp = task_txpacket(grp, chancx, nc + 8);
		unsigned len = txp->rawend - txp->datastart - 8;
		const unsigned pktlimit = 1100;
		if (len > pktlimit) len = pktlimit;
		if (len > nc) len = nc;
		if (len > pxchan->writecap) len = pxchan->writecap;
		txp->dataend = mkmsg(txp->dataend, 0 | TASK_GENID_S(chancx->taskgen, chancx->taskid)); // data
		txp->dataend = mkmsg(txp->dataend, len); // length
		memcpy(txp->dataend, buf, len);
		txp->dataend += len;
		if (len & 3) txp->dataend += 4 - (len & 3);
		*nw += len;
		nc -= len;
		buf = (char *) buf + len;
		if (grp->conn->crypt && (grp->conn->crypt->level & (1<<0))) {
			txp->u.tx.payloadplaintext = 1;
			txp->u.tx.mergeinhibit = 2;
		}
		assp_conn_send(grp->conn, txp);
		pxchan->writecap -= len;
	}
 }
 update_minready(pxchan);
 px_update_event(pxchan);
 unlock_object(grp);
 if (TiNGtrace > 2) olog("->%d]", *nw);
 return OK;
}

STATIC ERRCODE px_ioctl(CARDCONN *cx, unsigned long cmd, void *ap, unsigned asiz)
{
 PX_CARDCONN_VTBL_DATA *pvp = cx->vtbl_data;
 unsigned osiz;
 int status;
 if (TiNGtrace > 1) {
#ifdef TiNGTYPE_LINUX
	if (cmd >= 'P') {
		olog("[ioctl X %p TiNG(%d) %d", pvp, cmd - 'P', asiz);
	} else {
		olog("[ioctl X %p %08lx %d", pvp, cmd, asiz);
	}
#endif
#ifdef TiNGTYPE_QNX
	if (cmd >= 'P') {
		olog("[ioctl X %p TiNG(%d) %d", pvp, cmd - 'P', asiz);
	} else {
		olog("[ioctl X %p %08lx %d", pvp, cmd, asiz);
	}
#endif
#ifdef TiNGTYPE_WINNT
	if ((cmd & 0xffff0003) == 0xcb000000) {
		olog("[ioctl X %p TiNG(%d) %d", pvp,
			((cmd >> 2) & 0xfff) - 0xcb0,
			asiz);
	} else {
		olog("[ioctl X %p %08lx %d", pvp, cmd, asiz);
	}
#endif
 }
 status = px_ioctls(pvp, cmd, ap, asiz);
 if (TiNGtrace > 1) olog("]");
 if (status) {
 	ERRCODE e;
 	e.text = "Prosody X ioctl-library failed";
 	e.errnum = 0;
 	if (TiNGtrace > 1) olog(":err]");
 	return e;
 }
 osiz = *(U32 *)ap;
 if (osiz < asiz) memset((char *) ap + osiz, 0, asiz - osiz);
 return OK;
}

STATIC ERRCODE px_msgstring(CARDCONN *cx, unsigned long id, void *buf, unsigned len)
{
 PX_CARDCONN_VTBL_DATA *pvp = cx->vtbl_data;
 ERRCODE e;
 if (TiNGtrace > 1) olog("[msgstring %x %lx %p %d", pvp, id, buf, len);
 if (pvp->dest_type != TiNG_DEST_CHANNEL) {
 	if (TiNGtrace) olog("px_msgstring: bad dest: %d\n", pvp->dest_type);
 	e.text = "Prosody X ioctl-library failed";
 	e.errnum = 0;
 	if (TiNGtrace > 1) olog(":err]");
 	return e;
 }
 if (pvp->u.chan->ctstate == CT_RUNNING) {
	ASSP_CHAN *chancx = pvp->u.chan->chancx;
	ASSP_GROUP *grp = chancx->grp;
	lock_object(grp);
 	if (asspgroup_isconnected(grp)) {
		PACKET *pkt = task_txpacket(grp, chancx, len + 12); // 12 for taskid, 'string' / len, and id
		pkt->dataend = mkmsg(pkt->dataend, TASK_GENID_S(chancx->taskgen, chancx->taskid));
		pkt->dataend = mkmsg(pkt->dataend, len + 0x10004); // string
		pkt->dataend = mkmsg(pkt->dataend, id);
		memcpy(pkt->dataend, buf, len);
		pkt->dataend += len;
		if (len & 3) pkt->dataend += 4 - (len & 3);
		assp_conn_send(grp->conn, pkt);
	} else {
		if (TiNGtrace > 1) olog("px_msgstring: discarding tx msg - not connected");
	}
	unlock_object(grp);
 } else {
 	if (TiNGtrace > 1) olog("px_msgstring: discarding tx msg - not running");
 }
 if (TiNGtrace > 1) olog("]");
 return OK;
}

STATIC ERRCODE px_rdmsgstring(CARDCONN *cx, unsigned long id, unsigned long id_mask, unsigned maxlen, void *s, unsigned *len)
{
 PX_CARDCONN_VTBL_DATA *pvp = cx->vtbl_data;
 ASSP_CHAN *chancx;
 ASSP_GROUP *grp;
 PX_CHAN *pxchan;
 PACKET *rxp;
 ERRCODE e;
 if (TiNGtrace > 1) olog("[rdmsgstring %x %lx %lx %p %d", pvp, id, id_mask, s, maxlen);
 if (pvp->dest_type != TiNG_DEST_CHANNEL) {
 	if (TiNGtrace) olog("px_rdmsgstring: bad dest: %d\n", pvp->dest_type);
 	e.text = "Prosody X ioctl-library failed";
 	e.errnum = 0;
 	if (TiNGtrace > 1) olog(":err]");
 	return e;
 }
 pxchan = pvp->u.chan;
 // NB len and s can be NULL (when just discarding a string)
 if (len) *len = ~0u; // assume no matching message
 chancx = pxchan->chancx;
 grp = chancx->grp;
 lock_object(grp);
 rxp = chancx->rxq;
 if (rxp) {
 	uint8_t *dp = rxp->datastart;
	unsigned plen = rxp->dataend - dp;
	unsigned tlen;
	if (plen < 4) { // not enough for msg
		rxp->datastart = rxp->dataend; // discard remaining
	} else if (!(getmsg(&dp) & 0xffff0000)) { // not std msg or space
		if (plen < 8) { // not enough room for payload type
			rxp->datastart = rxp->dataend; // discard remaining
		} else {
			tlen = getmsg(&dp);
			if ((tlen & 0xffff0000) == 0x00010000) { // type is string
				tlen &= 0xffff;
				if (plen < 12 || tlen < 4) { // no room for string id, or invalid data length
					rxp->datastart = rxp->dataend; // discard remaining
				} else {
					unsigned sid = getmsg(&dp); // string id
					unsigned cplen;
					if ((sid & id_mask) == (id & id_mask)) {
						// we have our string
						tlen -= 4; // less length of stringid
						if (tlen > plen - 12) tlen = plen - 12;	// cope with bogus packet
						cplen = (tlen > maxlen) ? maxlen : tlen;
						if (cplen) memcpy(s, dp, cplen); // when s is NULL (discarding) maxlen must be zero, so cplen is zero
						if (TiNGtrace > 1) olog("->%d", cplen);
						if (len) *len = cplen;
						// FIXME: should we log string content
						
						rxp->datastart = dp + ((tlen + 3) & ~3); // tlen + pad

					} // else leave for someone else to read
				}
			} // else not string, so leave for get data
		}
	} // else not payload, leave for rdmsg

	// are we finished with the packet?
	if (rxp->datastart >= rxp->dataend) {
		chancx->rxq = rxp->next;
		if (!chancx->rxq) chancx->rxq_last = &chancx->rxq;
		assp_conn_free_packet(grp->conn, rxp);
	}
 }
 px_update_event(pxchan);
 unlock_object(grp);
 if (TiNGtrace > 1) olog("]");
 return OK;
}

STATIC ERRCODE px_event(CARDCONN *cx, EVNT *ev)
{
 PX_CARDCONN_VTBL_DATA *pvp = cx->vtbl_data;
 if (pvp->dest_type == TiNG_DEST_MODULE) {
	lock_object(&pvp->u.mod->modgroup);
	*ev = pvp->u.mod->modgroup.stopped_event;
	unlock_object(&pvp->u.mod->modgroup);
	return OK;
 } else if (pvp->dest_type == TiNG_DEST_CHANNEL) {
	*ev = pvp->u.chan->chancx->cxevnt;
	return OK;
 } else {
 	ERRCODE e;
 	e.text = "Bad dest for px_event";
 	e.errnum = 0;
 	return e;
 }
}

STATIC ERRCODE px_shutdown(CARDCONN *cx)
{
 PX_CARDCONN_VTBL_DATA *pvp = cx->vtbl_data;
 if (TiNGtrace > 1) olog("[shutdown X %p", pvp);
 switch (pvp->dest_type) {
 case TiNG_DEST_CARD:
	break;
 case TiNG_DEST_MODULE:
 	shutdownProsodyXModule(pvp->u.mod);
	break;
 case TiNG_DEST_CHANNEL:
	break;
 }
 if (TiNGtrace > 1) olog("]\n");
 return OK;
}


STATIC ERRCODE px_sec(CARDCONN *cx, int level, int keyId, void* context, tSMSecCallback encdec )
{
 PX_CARDCONN_VTBL_DATA *pvp = cx->vtbl_data;

 ASSP_GROUP *grp = &pvp->u.mod->modgroup;

 grp->conn->crypt = malloc(sizeof(*grp->conn->crypt));
 grp->conn->crypt->level = level;
 grp->conn->crypt->keyId = keyId;
 grp->conn->crypt->encdec = encdec;
 grp->conn->crypt->context = context;
 grp->conn->crypt->firstKeyIdPending = 1;
 grp->conn->crypt->keyChangePending = 0;
 lock_object(grp);
 kickoffgroup(grp);
 if (asspgroup_isconnected(grp)) 
 {
	PACKET* pkt = assp_conn_new_packet(grp->conn, 4*4);
	if (pkt) {
		//Aimed at task zero
		pkt->dataend = mkmsg(pkt->dataend, MESSAGE(MSG_SECKEYID, 3));
		pkt->dataend = mkmsg(pkt->dataend, keyId);
		pkt->dataend = mkmsg(pkt->dataend, level);
		pkt->dataend = mkmsg(pkt->dataend, MESSAGE(MSG_PAD, 1));

		pkt->u.tx.changeover = 1;
		pkt->u.tx.mergeinhibit = 1;

		assp_conn_send(grp->conn, pkt);
	}
 } 
 unlock_object(grp);
 return OK;
}


STATIC ERRCODE px_close(CARDCONN *cx)
{
 PX_CARDCONN_VTBL_DATA *pvp = cx->vtbl_data;
 if (TiNGtrace > 1) olog("[close X %p", pvp);
 switch (pvp->dest_type) {
 case TiNG_DEST_CARD:
	break;
 case TiNG_DEST_MODULE:
 	freeProsodyXModule(pvp->u.mod);
	break;
 case TiNG_DEST_CHANNEL: {
 	PX_CHAN *pxchan = pvp->u.chan;
 	ASSP_GROUP *grp = pxchan->chancx->grp;
	lock_object(grp);
	if (pxchan->ctstate == CT_RUNNING && asspgroup_isconnected(grp)) {
		PACKET *pkt = task_txpacket(grp, pxchan->chancx, 4);
		pkt->dataend = mkmsg(pkt->dataend, MESSAGE(MSG_DEAD, 1) | TASK_GENID_S(pxchan->chancx->taskgen, pxchan->chancx->taskid));
		assp_conn_send(grp->conn, pkt);
	}
	if (pxchan->ctstate != CT_IDLE) task_nextgen(pxchan->chancx);
	assp_chan_close(pxchan->chancx);
	unlock_object(grp);
	free(pxchan);
 }
 	break;
 }
 refcnt_dec(&pvp->id->ref);
 free(pvp);
 cx->vtbl_data = 0;
 cardconn(cx);
 if (TiNGtrace > 1) olog("]\n");
 return OK;
}

STATIC ERRCODE px_open(CARDCONN *newcx, struct cardconnvtbl *vtbl, PX_CARDID *cid)
{
 PX_CARDCONN_VTBL_DATA *pvp = malloc(sizeof(*pvp));
 if (!pvp) {
	ERRCODE e;
	e.text = "malloc() failed";
	e.errnum = errno;
	if (TiNGtrace) {
		olog("Device: ProsodyX %s\n", cid->name);
		printerr(e);
	}
	return e;
 }
 newcx->vtbl_data = pvp;
 pvp->id = cid;
 refcnt_inc(&cid->ref);
 newcx->vtbl = vtbl;
 pvp->dest_type = TiNG_DEST_CARD;
 return OK;
}

STATIC ERRCODE px_clone(CARDCONN *newcx, CARDCONN *oldcx)
{
 PX_CARDCONN_VTBL_DATA *pvp = oldcx->vtbl_data;
 ERRCODE e =  px_open(newcx, oldcx->vtbl, pvp->id);
 if (!ERR(e) && pvp->dest_type == TiNG_DEST_MODULE) {
	PX_CARDCONN_VTBL_DATA *npvp = newcx->vtbl_data;
 	npvp->dest_type = pvp->dest_type;
 	npvp->u.mod = pvp->u.mod;
 	refcnt_inc(&pvp->u.mod->ref);
 }
 if (TiNGtrace > 1) olog("[clone X %p -> %p]", pvp, newcx->vtbl_data);
 return e;
}

STATIC ERRCODE px_discardmsgs(CARDCONN *cx, unsigned discard)
{
 PX_CARDCONN_VTBL_DATA *pvp = cx->vtbl_data;
 ASSP_CHAN *chancx;
 PX_CHAN *pxchan;
 pxchan = pvp->u.chan;
 chancx = pxchan->chancx;
 chancx->discardmsgs = discard;
 return OK;
}

STATIC struct cardconnvtbl px_vtbl = {
	px_clone,
	px_setblock,
	px_read,
	px_write,
	px_ioctl,
	px_msgstring,
	px_rdmsgstring,
	px_discardmsgs,
	px_event,
	px_shutdown,
	px_sec,
	px_close,
};

/*
 * a config parser returns 0 when it gets a valid response
 * and non-zero otherwise. It also updates PX_CARDID with
 * the data received
 */
typedef int (*config_parser)(PX_CARDID *cid, char *buf);

/*
 * format expected: mod\tbaseport\tlastport[\tasspmonport[\tmodule_addr]]...\n
 * note the last asspmon port value is used
 */
STATIC int parseconfig(PX_CARDID *cid, char *buf)
{
	if (buf[0] != CARDINFO_OK) return 1;
	++buf;
	cid->nmod = 0;
	while (*buf) {
		int physmod;
		int baseport;
		int endport;
		int asspmonport;
		int matched;
		char *pend = strchr(buf, '\n');
		if (pend) {
			*pend++ = 0; // one line at a time
		}
		matched = sscanf(buf, "%i %i %i %i %255s", &physmod, &baseport, &endport, &asspmonport, cid->mod[cid->nmod].addrname);
		if (matched < 3) {
			// invalid buf
			if (TiNGtrace) olog("invalid data in modlist: %s\n", buf);
			return 1;
		} else if (matched == 3) {
			asspmonport = 0;
			cid->mod[cid->nmod].addrname[0] = 0;
		} else if (matched == 4) {
			cid->mod[cid->nmod].addrname[0] = 0;
		}
		//skip to end of line
		if (pend) {
			buf = pend;
		} else {
			break;
		}

		// fill in cid
		cid->mod[cid->nmod].physmod = physmod;
		cid->mod[cid->nmod].firstport = baseport;
		cid->mod[cid->nmod].lastport = endport;
		cid->asspmonport = asspmonport;
		if (++cid->nmod >= arlen(cid->mod)) {
			if (TiNGtrace) olog("ignoring excess modules: %s\n", buf);
			return 0;
		}

	}
	return 0;
}

#define CARDINFOD_MAX_BOGUS_WAKEUPS 4

STATIC int sock_cmpaddr(struct sockaddr_storage *first, struct sockaddr_storage *second)
{
	if (first->ss_family == second->ss_family) {
		if (first->ss_family == AF_INET) {
			struct sockaddr_in *pf = (struct sockaddr_in *) first;
			struct sockaddr_in *ps = (struct sockaddr_in *) second;
			if (pf->sin_addr.s_addr == ps->sin_addr.s_addr
					&& pf->sin_port == ps->sin_port)
				return 0;
		} else if (first->ss_family == AF_INET6) {
			struct sockaddr_in6 *pf = (struct sockaddr_in6 *) first;
			struct sockaddr_in6 *ps = (struct sockaddr_in6 *) second;
			if (memcmp(&pf->sin6_addr.s6_addr, &ps->sin6_addr.s6_addr, sizeof(struct in6_addr)) == 0
					&& pf->sin6_port == ps->sin6_port)
				return 0;
		}
	}
	
	return -1;
}

STATIC ERRCODE cardinfod_cmd(SOCKET sock, struct sockaddr_storage *ss, socklen_t sslen, const char *cmd, config_parser parse, PX_CARDID *cid)
{
 char buf[1024];
 unsigned tmo_mS = 10;
 (void)sslen;
 for (;;) {
	int bogus = CARDINFOD_MAX_BOGUS_WAKEUPS;
	int i = send(sock, cmd, strlen(cmd), 0);
	if (i != (int)strlen(cmd)) {
		ERRCODE e;
		if (i == -1) {
			osockerr("send() failed");
		} else {
			if (TiNGtrace > 0) olog("send() sent %d, not %d\n", i, sizeof(cmd)-1);
		}
		e.errnum = 0;
		e.text = "Cannot sent command to cardinfod";
		return e;
	}
	tmo_mS *= 2;
	for (;bogus--;) {
#ifdef USE_SELECT
		struct timeval tmoval;
		fd_set rdset;
		int rxready;
		FD_ZERO(&rdset);
		FD_SET(sock, &rdset);
		tmoval.tv_sec = tmo_mS / 1000;
		tmoval.tv_usec = (tmo_mS % 1000) * 1000;
		i = select(sock+1, &rdset, 0, 0, &tmoval);
		if (i == -1) {
			ERRCODE e;
			osockerr("select() failed");
			e.errnum = 0;
			e.text = "Cannot select() on cardinfod socket";
			return e;
		}
		rxready = FD_ISSET(sock, &rdset);
#endif
#ifdef USE_POLL
		struct pollfd pfd;
		int rxready;
		pfd.fd = sock;
		pfd.events = POLLIN;
		i = poll(&pfd, 1, tmo_mS);
		if (i == -1) {
			ERRCODE e;
			if (errno == EINTR) continue;
			osockerr("poll() failed");
			e.errnum = 0;
			e.text = "Cannot poll() on cardinfod socket";
			return e;
		}
		rxready = pfd.revents;
#endif
		if (rxready) {
			struct sockaddr_storage from;
			socklen_t fromlen = sizeof(from);
			i = recvfrom(sock, buf, sizeof(buf) - 1, 0, (struct sockaddr *) &from, &fromlen);
			if (i < 0) {
				ERRCODE e;
				osockerr("recv() failed");
				e.errnum = 0;
				e.text = "Cannot get reply from cardinfod";
				return e;
			}
			if (sock_cmpaddr(&from, ss) != 0) {
				if (TiNGtrace) olog("Got packet not from cardinfod\n");
			} else {
				buf[i] = '\0';
				if (!parse(cid, buf)) return OK;
			}
			// bogus packet, just wait for next without retransmit
		} else {
			break;
		}
	}
	if (tmo_mS >= 60000) {
		ERRCODE e;
		e.text = "No response from cardinfod";
		e.errnum = 0;
		return e;
	} else {
		if (TiNGtrace > 5) olog("No response from cardinfod - retrying\n");
	}
 }
}

STATIC ERRCODE get_modlist(PX_CARDID *cid, SOCKET sock, struct sockaddr_storage *ss, socklen_t sslen)
{
 static const char cmd[] = CARDINFO_CMD_MODLIST;
 ERRCODE e;
 e = cardinfod_cmd(sock, ss, sslen, cmd, parseconfig, cid);
 if (ERR(e)) return e;
 return OK;
}

STATIC int parsetdminfo(PX_CARDID * cid, char *buf)
{
 char *ep;
 char *sp;
 unsigned sc,tsc;
 unsigned baseStream;
 cid->ntdmcards = 0;
 if (buf[0] != CARDINFO_OK) {
	if (strlen(buf) >= CARDINFO_BAD_RESPONSE_CHECK_LEN
			&& !memcmp(buf, CARDINFO_BAD_RESPONSE_HEAD, CARDINFO_BAD_RESPONSE_CHECK_LEN)
			&& !strstr(buf, CARDINFO_CMD_TDMLIST)) {
		cid->ntdmcards = -1; // older PS system not supporting tdmlist
		return 0; // valid as cardinfo command not supported
	}
	return 1;
 }
 if (!buf[1]) return 0; // empty payload is considered valid
 sp = &buf[1];

 while (*sp) {
	ep = strchr(sp,'\\');
	if (ep == 0) {
		if (TiNGtrace) olog("Ignoring bogus cardinfo tdmlist info: '%s'\n", buf);
		return 1;
	} else if (cid->ntdmcards == arlen(cid->tdmcards)) {
		if (TiNGtrace) olog("Ignoring bogus cardinfo tdmlist info: '%s'\n", buf);
		return 1;
	} else {
		*ep = 0;
		strcpy(&(cid->tdmcards[cid->ntdmcards].serialno[0]),sp);
		sp = ep+1;

		baseStream = strtoul(sp, &ep, 0);
		if (sp == ep) {
			if (TiNGtrace) olog("Ignoring bogus cardinfo tdmlist info: '%s'\n", sp);
			return 1;
		}
		if ((*ep) != '\\') {
			if (TiNGtrace) olog("Ignoring bogus cardinfo tdmlist info: '%s'\n", sp);
			return 1;
		}
		sp = ep+1;

		sc = strtoul(sp, &ep, 0);
		if (sp == ep) {
			if (TiNGtrace) olog("Ignoring bogus cardinfo tdmlist info: '%s'\n", sp);
			return 1;
		}
		if ((*ep) != '\\') {
			if (TiNGtrace) olog("Ignoring bogus cardinfo tdmlist info: '%s'\n", sp);
			return 1;
		}
		sp=ep+1;

		tsc = strtoul(sp, &ep, 0);

		cid->tdmcards[cid->ntdmcards].firststream = baseStream;
		cid->tdmcards[cid->ntdmcards].laststream = baseStream + sc-1;
		cid->tdmcards[cid->ntdmcards].timeslots = tsc;

		while (*ep && (*ep != '\n'));
		if (*ep == '\n') ep++;
		sp = ep;
		cid->ntdmcards += 1;
	}
 }
 return 0;
}

STATIC ERRCODE get_tdmlist(PX_CARDID *cid, SOCKET sock, struct sockaddr_storage *ss, socklen_t sslen)
{
 static const char cmd[] = CARDINFO_CMD_TDMLIST;
 ERRCODE e;
 e = cardinfod_cmd(sock, ss, sslen, cmd, parsetdminfo, cid);
 if (ERR(e)) return e;
 return OK;
}

STATIC int parsecardinfo(PX_CARDID * cid, char *buf)
{
 char *ep;
 char *sp;
 char *serial;
 unsigned ct;
 if (buf[0] != CARDINFO_OK) {
	if (strlen(buf) >= CARDINFO_BAD_RESPONSE_CHECK_LEN
			&& !memcmp(buf, CARDINFO_BAD_RESPONSE_HEAD, CARDINFO_BAD_RESPONSE_CHECK_LEN)
			&& !strstr(buf, CARDINFO_CMD_CARDINFO)) {
		return 0; // valid as cardinfo command not supported
	}
	return 1;
 }
 if (!buf[1]) return 0; // empty payload is considered valid
 // first is number for card type;
 sp = buf + 1;
 ct = strtoul(sp, &ep, 0);
 if (sp == ep) {
	if (TiNGtrace) olog("Ignoring bogus cardinfo info: '%s'\n", buf);
	return 1;
 }
 sp = ep;
 if (*sp++ != '\n') {
	if (TiNGtrace) olog("Ignoring bogus cardinfo info: '%s'\n", buf);
	return 1;
 }
 for (ep = sp; *ep != '\n'; ep++) {
	 if (!*ep) {
		if (TiNGtrace) olog("Ignoring bogus cardinfo info: '%s'\n", buf);
		return 1;
	 }
 }
 *ep = 0;
 serial = sp;
 sp = ++ep;
 for (; *ep != '\n'; ep++) {
	 if (!*ep) {
		if (TiNGtrace) olog("Ignoring bogus cardinfo info: '%s'\n", buf);
		return 1;
	 }
 }
 *ep = 0;
 // got all we're expecting
 switch (ct) {
 case CARDINFO_CARD_TYPE_PX_NUM:
	 cid->card_type = TiNG_CARDTYPE_X;
	 break;
 case CARDINFO_CARD_TYPE_PSV3_NUM:
	 cid->card_type = TiNG_CARDTYPE_SOFT_S;
	 break;
 case CARDINFO_CARD_TYPE_SIMULATION_NUM:
 default:
	 cid->card_type = TiNG_CARDTYPE_UNK;
	 break;
 }
 strncpy(cid->serialno, serial, sizeof(cid->serialno)-1);
 cid->serialno[sizeof(cid->serialno)-1] = 0;
 strncpy(cid->verinfo, sp, sizeof(cid->verinfo) - 1);
 cid->verinfo[sizeof(cid->verinfo)-1] = 0;
 return 0;
}

STATIC ERRCODE get_cardinfo(PX_CARDID *cid, SOCKET sock, struct sockaddr_storage *ss, socklen_t sslen)
{
 static const char cmd[] = CARDINFO_CMD_CARDINFO;
 ERRCODE e;
 e = cardinfod_cmd(sock, ss, sslen, cmd, parsecardinfo, cid);
 if (ERR(e)) return e;
 return OK;
}

STATIC ERRCODE find_config(PX_CARDID *cid)
{
 ERRCODE err;
 struct sockaddr_storage ss;
 socklen_t sslen;
 int s;
 SOCKET sock = create_connected_socket(cid->name, 0, cid->remport, 0, 0, &ss, &sslen); // might be a name, udp and not assp
 if (sock == -1) {
	osockerr("create_connected_socket() failed");
	err.text = "Cannot create UDP socket";
	err.errnum = 0;
	return err;
 }
 s = getnameinfo((struct sockaddr*)&ss, sslen, cid->remaddr, sizeof(cid->remaddr), NULL, 0, NI_NUMERICHOST);
 if (s) {
	olog("getnameinfo failed: %s\n", gai_strerror(s));
	err.text = "Cannot get remote address";
	err.errnum = 0;
	closesocket(sock);
	return err;
 }
 err = get_modlist(cid, sock, &ss, sslen);
 if (!ERR(err)) err = get_cardinfo(cid, sock, &ss, sslen);
 if (!ERR(err)) err = get_tdmlist(cid, sock, &ss, sslen);
 if (ERR(err)) {
	if (TiNGtrace > 0) olog("Cannot connect to %s:cardinfod\n", cid->name);
 }
 closesocket(sock);
 return err;
}

STATIC void free_cid(void *p)
{
 PX_CARDID *cid = p;
 free(cid->name);
 free(cid);
}

STATIC ERRCODE px_psv3_cardconn_open(CARDCONN *cx, struct card *card, int cardtype);

LOCALDEF ERRCODE px_cardconn_open(CARDCONN *cx, struct card *card)
{
 return px_psv3_cardconn_open(cx, card, TiNG_CARDTYPE_X);
}

LOCALDEF ERRCODE psv3_cardconn_open(CARDCONN *cx, struct card *card)
{
 return px_psv3_cardconn_open(cx, card, TiNG_CARDTYPE_SOFT_S);
}

STATIC ERRCODE px_psv3_cardconn_open(CARDCONN *cx, struct card *card, int cardtype)
{
 PX_CARDID *cid = malloc(sizeof(*cid));
 char *key;
 char *port;
 ERRCODE e;
 if (!cid || !(cid->name = strdup(card->cardname))) {
	free(cid);
	e.text = "malloc() failed";
	e.errnum = errno;
	if (TiNGtrace) {
		olog("Device: %s\n", card->cardname);
		printerr(e);
	}
	return e;
 }
 cid->serialno[0] = 0; // this will need to come from cardinfod
 cid->verinfo[0] = 0; // this will need to come from cardinfod
 cid->card_type = cardtype; // this will need to come from cardinfod
 cid->key = 0;
 cid->remport = 2030;
 cid->asspmonport = 0;
 cid->ntdmcards = 0;
 port = 0;
 key = cid->name;
 if (*key == '[') {
	for(;*key; key++) {
		*key = *(key + 1); // move everything down one place
		if (*key == ']') {
			*key = 0;
			key += 2; // skip \0 and original ]
			break;
		}
	}
 }
 for (; *key; key++) {
	if (*key == ':' && !port) {
		*key = '\0';
		port = key + 1;
	}
 	if (*key == '/') {
 		*key = '\0';
 		cid->key = key + 1;
 		break;
 	}
 }
 refcnt(&cid->ref, free_cid, cid);
 if (!port) port = "2030"; // the default port
 if (port) {
	char *end;
	cid->remport = strtol(port, &end, 0);
	if (!cid->remport || *end != '\0') {
		e.text = "Cannot interpret port";
		e.errnum = 0;
		if (TiNGtrace) {
			olog("Cannot interpret port for: %s\n", card->cardname);
		}
		refcnt_dec(&cid->ref);
		return e;
	}
 }
 e = find_config(cid);
 if (!cid->asspmonport) cid->asspmonport = cid->remport + 1; // default asspmon port to 1 greater than cardinfod
 if (TiNGtrace > 1) olog("ASSPmon port %u\n", cid->asspmonport);
 if (!ERR(e)) e = px_open(cx, &px_vtbl, cid);
 refcnt_dec(&cid->ref);
 if (TiNGtrace > 1) {
	if (ERR(e)) olog("[open X failed]");
	else olog("[open X %p]", cx->vtbl_data);
 }
 return e;
}
