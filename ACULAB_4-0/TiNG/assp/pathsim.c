#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asspdbg.h"
#include "decode_str.h"
#include "gasdev.h"
#include "pathsim.h"
#include "vseprintf.h"

#ifdef TiNGTYPE_WINNT
#include <stdlib.h>
#endif

#define arlen(x) (sizeof(x)/sizeof(*(x)))

#ifdef _MSC_VER

/* disable stupid warnings about conversions
 */

#pragma warning(disable:4244)
#pragma warning(disable:4305)
/* and about argument conversions (does anyone still use a
   non-prototype compiler?) */
#pragma warning(disable:4761)

#endif

typedef TCBPATH_IMPL PATH;	// synonym for brevity

static void *dupmem(void *m, unsigned len)
{
 if (m) {
	void *p = malloc(len);
	if (!p) {
		perror("malloc() failed");
		printf("Cannot duplicate %d bytes\n", len);
		exit(1);
	}
	memcpy(p, m, len);
	return p;
 }
 return 0;
}

static uint8_t *validate_cookie(PATH *path, TCB *tcb, MD5_STREAM *msp, uint8_t *sp, uint8_t *ecoo)
{
 (void) path;
 (void) tcb;
 (void) msp;
 (void) ecoo;
 return sp;
}

static uint8_t *generate_cookie(PATH *path, TCB *tcb, MD5_STREAM *msp, uint8_t *sp)
{
 (void) path;
 (void) tcb;
 (void) msp;
 return sp;
}

static uint8_t *make_keycode(PATH *path, TCB *tcb, uint8_t *sp, uint8_t *ep, uint8_t *op)
{
 (void) path;
 (void) tcb;
 (void) sp;
 (void) ep;
 memset(op, 0, 16);
 return op + 16;
}

static NETPACKET *netpacket(void)
{
 NETPACKET *np = malloc(sizeof(*np));
 if (!np) {
	perror("malloc() failed");
 	printf("Cannot allocate a NETPACKET\n");
	exit(1);
 }
 return np;
}

static void netpacket_dtor(NETPACKET *np)
{
 free(np->data);
 free(np);
}

static PACKET *rx_freelist;

static PACKET *rxalloc(PATH *path, unsigned maxrx)
{
 PACKET *rxp;
 (void) path;
 (void) maxrx;
 if (rx_freelist) {
 	rxp = rx_freelist;
	rx_freelist = rx_freelist->next;
 } else {
 	rxp = malloc(sizeof(*rxp));
	if (!rxp) {
		perror("malloc() failed");
		printf("Cannot allocate %d bytes for rx frame\n",
			sizeof(*rxp));
		return 0;
	}
 }
 memset(rxp->data, 0xb5, sizeof(rxp->data));
 rxp->rawstart = rxp->data;
 rxp->dataend = rxp->data + arlen(rxp->dataend);
 return rxp;
}

static void rxfree(PATH *path, PACKET *rxp)
{
 (void) path;
#if 1
 free(rxp);
#else
 memset(rxp->data, 0xa5, sizeof(rxp->data));
 rxp->next = rx_freelist;
 rx_freelist = rxp;
#endif
}

static void delivery(void *arg)
{
 HOP *hop = arg;
 NETPACKET *np = hop->pktq;
 hop->pktq = np->nextpkt;
 if (!hop->pktq) hop->qnextp = &hop->pktq;
 if (time_sgn(time_until(np->deltime)) > 0) {
	printf("Internal error: delivery time not reached for %p\n", np);
	abort();
 }
 if (ran2(&hop->randgen) < hop->ploss) {
	hop->nlost++;
	printf("Packet lost\n");
	netpacket_dtor(np);
 } else {
 	hop->rxbits += np->dlen * 8;
 	hop->deliver(hop->deliver_par, np);
 }
}

static void deliver_assp(void *dest, NETPACKET *np)
{
 PACKET *pkt = rxalloc(0, np->dlen);
 if (!pkt) abort();
 memcpy(pkt->rawstart, np->data, np->dlen);
 pkt->dataend = pkt->rawstart + np->dlen;
 assp_handle_rx(dest, pkt);
 netpacket_dtor(np);
}

/* delivery model
 * A packet passing over a hop is delayed by three things: delay, latency,
 * and bandwidth. Delay represents the contention for the link. It means
 * that a packet might be delayed even though the link is not busy
 * with other packets belonging to this connection. Latency is a
 * fixed delay experienced by every packet, but which can be
 * overlapped from one packet to the next. Bandwidth causes a delay
 * proportional to the size of the packet.
 * When a packet reaches the head of the transmission queue, it
 * experiences the contention delay. After this, it starts being
 * transmitted. After the time dictated by the link bandwidth, the
 * next packet can be started. After the further delay specified by
 * the latency, the packet is delivered.
 */
static void hop_queue(HOP *hop, NETPACKET *npkt)
{
 for (;;) {
	double delay = gasdev(&hop->delay_dist) * hop->delay_dev + hop->delay_mean;
	int isdup = ran2(&hop->randgen) < hop->probdup;
	NETPACKET *np = npkt;
	uint_fast64_t v;
	ABSTIME t;
	ABSTIME t2;
	long d;
	if (isdup) {
		np = netpacket();
		np->dlen = npkt->dlen;
		np->data = dupmem(npkt->data, np->dlen);
	}
	if (delay < 0) delay = 0;
	if (++hop->pktno > 2) {
//		delay /= 20; // uncomment for periodic delay
		if (hop->pktno == 10) hop->pktno = 0;
	}
		// pretend link is busy for 'delay' to represent contention
	v = delay * 1000000;
//	printf("hop %s delay %llu\n", hop->name, v);
//	v = 0;
	if (time_sgn(time_until(hop->linkfree)) >= 0) {
		// link is busy: measure delay from when it becomes free
		t = time_add(hop->linkfree, time_longuS2rel(v));
	} else {
			// link is idle: measure delay from now
		hop->idle_uS += -time_rel2longuS(time_until(hop->linkfree));
		t = time_when(time_longuS2rel(v));
	}
	d = time_rel2longuS(time_until(t));
/*	// if a packet will be delayed for too long, drop it as though the buffer space is exhausted
	if (d > (((50 * 1200) + 64 + 96) * 8) / (hop->bps / (1000*1000)) ) {
		printf("Packet drop (delay %ld)\n", d);
		break;
	}
*/
	hop->blocked_uS += v;
	if (hop->maxtx < np->dlen) hop->maxtx = np->dlen;
		// and it really is busy for the time taken to send this
	v = (np->dlen + 14 + 20 + 8 + 8 + 12) * 8 / (hop->bps / (1000*1000));
	hop->busy_uS += v;
	t = time_add(t, time_longuS2rel(v));
	hop->linkfree = t;	// then it's ready to start the next packet
		// and this one arrives at that time plus the latency
	t = time_add(t, time_longuS2rel(hop->latency_uS));
	hop->linkfree = t;	// then it's ready to start the next packet
/*	delay = gasdev(&hop->delay_dist) * hop->delay_dev + hop->delay_mean;
	if (delay < 0.000010) delay = 0.000010;
	v = delay * 1000000;
	t2 = time_add(hop->prevdel, time_longuS2rel(v));
	if (time_cmp(t2, t) > 0) {
		hop->blocked_uS += time_rel2longuS(time_sub(t2, t));
		t = t2;
	}*/
	np->deltime = t;
	hop->prevdel = t;
	hop->nsent++;
	*hop->qnextp = np;
	hop->qnextp = &np->nextpkt;
	np->nextpkt = 0;
	eventqueue(t, delivery, hop, "deliver");
	if (!isdup) break;
	printf("Duplicated packet\n");
 }
}

static void deliver_hop(void *dest, NETPACKET *npkt)
{
 HOP *hop = dest;
 hop_queue(hop, npkt);
}

// note: should have function which returns delay, or infinite if lost
// this could also take into account correlations between sending rate
// and loss probability etc.

static int netsendpkt(PATH *path, uint8_t *data, uint8_t *end)
{
 NETPACKET *np = netpacket();
 np->dlen = end - data;
 np->data = dupmem(data, np->dlen);
 hop_queue(path->firsthop, np);
 return 0;
}

static void pathsim_dtor(PATH *path)
{
 (void) path;
}

struct tcbpath_vtbl simpath_tcbpath_vtbl = {
	validate_cookie,
	generate_cookie,
	netsendpkt,
	rxalloc,
	rxfree,
	pathsim_dtor,
	make_keycode,
};

void hop_init(HOP *hop, char *name, HOP *nexthop, TCB *tcb, double bps, double ploss, double probdup, double delay_mean, double delay_dev, double latency)
{
 hop->name = name;
 hop->pktq = 0;
 hop->qnextp = &hop->pktq;
 hop->linkfree = time_when(time_longS2rel(0));
 hop->prevdel = time_when(time_longS2rel(0));
 ran2_init(&hop->randgen, 1);
 gasdev_init(&hop->delay_dist);
 if (nexthop) {
	if (tcb) abort();
	hop->deliver = deliver_hop;
	hop->deliver_par = nexthop;
 } else {
	hop->deliver = deliver_assp;
	hop->deliver_par = tcb;
 }
 hop->bps = bps;
 hop->ploss = ploss;
 hop->delay_mean = delay_mean;
 hop->delay_dev = delay_dev;
 hop->latency_uS = latency * 1000 * 1000;
 hop->probdup = probdup;
 hop->nlost = 0;
 hop->nsent = 0;
 hop->maxtx = 0;
 hop->busy_uS = 0;
 hop->idle_uS = 0;
 hop->blocked_uS = 0;
 hop->rxbits = 0;
 hop->pktno = 0;
}

void tcbpath_sim_init(TCBPATH *tp, TCBPATH_IMPL *path, HOP *firsthop)
{
 path->firsthop = firsthop;
 tp->pi = path;
 tp->vtbl = &simpath_tcbpath_vtbl;
}

static void closehop(HOP *hop)
{
 double tot = hop->nlost + hop->nsent;
 double tim = hop->busy_uS + hop->idle_uS + hop->blocked_uS;
 double tim_S = tim / 1000 / 1000;
 printf("\tHop %s:\n", hop->name);
 printf("\t\tSpeed: %g bps\n", hop->bps);
 printf("\t\tLatency: %g\n", hop->latency_uS / 1000.0 / 1000.0);
 printf("\t\tDelay: %g +%g\n", hop->delay_mean, hop->delay_dev);
 printf("\t\tp(loss): %g\n", hop->ploss);
 printf("\t\tp(dup): %g\n", hop->probdup);
 printf("\t\tLoss: %d (%g)\n", hop->nlost, hop->nlost / tot);
 printf("\t\tSent: %d\n", hop->nsent);
 printf("\t\tMax Tx: %u (bits)\n", hop->maxtx * 8);
 printf("\t\tRx bits: %lu\n", hop->rxbits);
 printf("\t\tRx bps: %g (%g)\n", hop->rxbits / tim_S, hop->rxbits / tim_S / hop->bps);
 printf("\t\tBusy: %g\n", hop->busy_uS / tim);
 printf("\t\tIdle: %g\n", hop->idle_uS / tim);
 printf("\t\tBlocked: %g\n", hop->blocked_uS / tim);
 printf("\t\tTime: %g S\n", tim_S);
}

void closepath(PATH *path, char *name)
{
 HOP *hop;
 printf("\tPath: %s\n", name);
 for (hop = path->firsthop; hop; ) {
 	closehop(hop);
 	if (hop->deliver != deliver_hop) break;
 	hop = hop->deliver_par;
 }
}
