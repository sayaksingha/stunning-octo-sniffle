#include "pathnet.h"
#include "inttypes.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef TiNGTYPE_LINUX
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif


#ifdef TiNGTYPE_WINNT
#include "../libutil/WINNT/wind.h"
#include "../libutil/WINNT/lasterr.h"
#define reperr(x) formaterr(x, WSAGetLastError())
#define ssize_t size_t
#endif

#ifndef reperr
#define reperr(x) perror(x)
#endif

#include "assp.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

typedef TCBPATH_IMPL PATH;	// synonym for brevity

static uint8_t *validate_cookie(PATH *path, TCB *tcb, MD5_STREAM *msp, uint8_t *sp, uint8_t *ecoo)
{
 (void) tcb;
 (void) ecoo;
 md5s_include(msp, (uint8_t *)&path->rem, sizeof(path->rem));
 // FIXME: should include this for extra security, but too fiddly
 //md5s_include(msp, (uint8_t *)&path->loc, sizeof(path->loc));
 return sp;
}

static uint8_t *generate_cookie(PATH *path, TCB *tcb, MD5_STREAM *msp, uint8_t *sp)
{
 (void) tcb;
 md5s_include(msp, (uint8_t *)&path->rem, sizeof(path->rem));
 // FIXME: should include this for extra security, but too fiddly
 //md5s_include(msp, (uint8_t *)&path->loc, sizeof(path->loc));
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

static int genlen(int i)
{
 i += 8;			// udp
 i += 6 + 6 + 2 + 4;		// dest, src, type, crc
 if (i < 64) i = 64;		// min ethernet frame
 i += 8;			// preamble
 return i;
}

#if 0
static int difftimems(struct timeb *a, struct timeb *b)
{
 return (b->time - a->time) * 1000 + b->millitm - a->millitm;
}
#endif

static int netsendpkt(PATH *path, uint8_t *data, uint8_t *end)
{
#ifdef MSG_DONTWAIT
 const int flags = MSG_DONTWAIT;
#else
 const int flags = 0;
#endif
 ssize_t len = end - data;
 ssize_t i = sendto(path->sock, (char *) data, len, flags, (struct sockaddr *) &path->rem, sizeof(path->rem));
 if (i != len) {
#ifdef TiNGTYPE_WINNT
	DWORD err = WSAGetLastError();
	printf("send() sent %d of %d, err=%d\n", i, len, err);
	if (err == WSAEWOULDBLOCK || err == WSAENOBUFS) {
		path->discpkts++;		
		return 0;
	}
#endif
	if (i < 0) reperr("sendto() failed");
	else printf("sendto() sent %d of %d\n", i, len);
	printf("Cannot send to %s:%d\n",
		inet_ntoa(path->rem.sin_addr), path->rem.sin_port);
	return 1;
 }
 len *= 8;		// bits
 path->sendbits += genlen(len);
 path->sendpkts++;
#if 0
 {
  static struct timeb laststats;
  ftime(&t);
  if (difftimems(&laststats, &t) >= 100) {
	printf("send_stats: disc_pkt=%d sent_pkt=%d sent_bytes=%d\n",
		path->discpkts, path->sendpkts, path->sendbits);
  	laststats = t;
  }
 }
#endif
 return 0;
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
 rxp->rawstart = rxp->data;
 rxp->dataend = rxp->data + arlen(rxp->data);
 return rxp;
}

static void rxfree(PATH *path, PACKET *rxp)
{
 (void) path;
 rxp->next = rx_freelist;
 rx_freelist = rxp;
}

static void pathnet_dtor(PATH *path)
{
 printf("Txbits: %" PRIuFAST64 "u\n", path->sendbits);
 printf("Txpkts: %" PRIuFAST64 "u\n", path->sendpkts);
 printf("Rxbits: %" PRIuFAST64 "u\n", path->recvbits);
 printf("Rxpkts: %" PRIuFAST64 "u\n", path->recvpkts);
}

static struct tcbpath_vtbl pathnet_tcbpath_vtbl = {
	validate_cookie,
	generate_cookie,
	netsendpkt,
	rxalloc,
	rxfree,
	pathnet_dtor,
	make_keycode,
};

void pathnet_init(TCBPATH *tcbpath, TCBPATH_IMPL *pathnet, int sock)
{
 pathnet->sock = sock;
 pathnet->discpkts = 0;
 pathnet->sendpkts = 0;
 pathnet->sendbits = 0;
 pathnet->recvpkts = 0;
 pathnet->recvbits = 0;
 tcbpath->pi = pathnet;
 tcbpath->vtbl = &pathnet_tcbpath_vtbl;
}
