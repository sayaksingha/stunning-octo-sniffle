/* server.c - ASSP over UDP server */

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef TiNGTYPE_LINUX
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>
#define USE_POLL
#endif


#ifdef TiNGTYPE_WINNT
#include <io.h>
#include "../libutil/WINNT/wind.h"
#include "../libutil/WINNT/lasterr.h"
#define reperr(x) formaterr(x, WSAGetLastError())
#define open _open
#define write _write
#define USE_WFMO
#define in_addr_t long
#endif

#include "assp.h"
#include "pathnet.h"
#include "randent.h"
#include "testtimer.h"
#include "timequeue_pq_basic.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

#ifndef reperr
#define reperr(x) perror(x)
#endif

//#define FAKE_FILE (256 * 1024 * 1024)

static void *err_malloc(unsigned len, char *what)
{
 void *p = malloc(len);
 if (!p) {
	perror("malloc() failed");
	printf("Cannot allocate %d bytes for %s\n", len, what);
 }
 return p;
}

struct assp_conn {
	struct assp_conn *next, **pprev;
	TCB *tcb;
	TCBPATH_IMPL pathnet;
	TCBPATH tcbpath;
#ifdef FAKE_FILE
	unsigned todo;
#else
	FILE *file;
#endif
} *allcnx, *cnx_listen;

static void cnx_txack(ASSP_CONN *cnx, PACKET *txp, int acked)
{
 (void) cnx;
 (void) txp;
 (void) acked;
 	// don't care
}

static PACKET *cnx_nexttx(ASSP_CONN *cnx, int *flags)
{
 const unsigned chunk = 1400;
 PACKET *txp = assp_txpacket(cnx->tcb, chunk);
#ifdef FAKE_FILE
 size_t nr = cnx->todo;
 if (nr > chunk) nr = chunk;
#else
 size_t nr = fread(txp->datastart, 1, chunk, cnx->file);
#endif
 if (!nr) *flags = ASSP_F_EOF;
 txp->dataend = txp->datastart + nr;
 return txp;
}

static int cnx_sof(ASSP_CONN *cnx)
{
 (void) cnx;
 printf("%p: SOF\n", cnx->tcb);
 return 1;
}

static void cnx_deliver(ASSP_CONN *cnx, PACKET *pkt)
{
 (void) cnx;
 if (pkt->dataend != pkt->datastart) {
	printf("%p: Ignoring %d bytes of data\n", cnx->tcb, pkt->dataend - pkt->datastart);
 }
 cnx->tcbpath.vtbl->rxfree(&cnx->pathnet, pkt);
}

static void cnx_eof(ASSP_CONN *cnx)
{
 (void) cnx;
 printf("%p: EOF\n", cnx->tcb);
}

static void cnx_rx_peek(ASSP_CONN *cnx, unsigned ofs, PACKET *pkt)
{
 (void) cnx;
 (void) ofs;
 (void) pkt;
}

static void cnx_stopped(ASSP_CONN *cnx)
{
 assp_tcb_stats(cnx->tcb);
}

static void cnx_closed(ASSP_CONN *cnx)
{
 assp_tcb_dtor(cnx->tcb);
 *cnx->pprev = cnx->next;
 if (cnx->next) cnx->next->pprev = cnx->pprev;
 free(cnx->tcb);
 free(cnx);
}

static struct assp_tcb_vtbl assp_vtbl = {
	cnx_sof,
	cnx_rx_peek,
	cnx_deliver,
	cnx_eof,
	cnx_stopped,
	cnx_closed,
	cnx_nexttx,
	cnx_txack,
};

static int setup_listen(char *filename, int sock, TIMEQUEUE *tq)
{
 cnx_listen = err_malloc(sizeof(*cnx_listen), "connection");
 if (!cnx_listen) return 1;
 cnx_listen->tcb = err_malloc(sizeof(*cnx_listen->tcb), "TCB");
 if (!cnx_listen->tcb) {
 	free(cnx_listen);
	cnx_listen = 0;
	return 1;
 }
 pathnet_init(&cnx_listen->tcbpath, &cnx_listen->pathnet, sock);
#ifdef FAKE_FILE
 (void) filename;
 cnx_listen->todo = FAKE_FILE;
#else
 cnx_listen->file = fopen(filename, "r");
 if (!cnx_listen->file) {
	perror("fopen() failed");
	printf("%p: Cannot open file: %s\n", cnx_listen->tcb, filename);
 	free(cnx_listen->tcb);
 	free(cnx_listen);
	cnx_listen = 0;
	return 1;
 }
#endif
 assp_tcb_init(cnx_listen->tcb, cnx_listen, tq, &cnx_listen->tcbpath, &assp_vtbl);
 return 0;
}

static int sameport(struct sockaddr_in *l, struct sockaddr_in *r)
{
 return l->sin_addr.s_addr == r->sin_addr.s_addr
	&& l->sin_port == r->sin_port;
}

static int handle_frame(int sock, char *filename, TIMEQUEUE *tq)
{
 PACKET *rxp = cnx_listen->tcbpath.vtbl->rxalloc(&cnx_listen->pathnet, 1500);
 struct sockaddr_in from;
 int fromlen = sizeof(from);
 ASSP_CONN *cxp;
 int rxlen;
 uint_fast32_t cid;
 if (!rxp) return 1;
 rxlen = recvfrom(sock, (char *) rxp->rawstart, rxp->dataend - rxp->rawstart, 0,
	(struct sockaddr *) &from, &fromlen);
 if (rxlen <= 0) {
#ifdef USE_WFMO
	if (WSAGetLastError() == WSAEWOULDBLOCK) {
		cnx_listen->tcb->path->vtbl->rxfree(cnx_listen->tcb->path->pi, rxp);
		return 0;
	}
#endif
	reperr("recvfrom() failed");
	return 1;
 }
 rxp->dataend = rxp->rawstart + rxlen;
 cid = assp_rx_cid(rxp);
 for (cxp = allcnx; ; cxp = cxp->next) {
	if (!cxp) {
			// FIXME: discard if broadcast
		cnx_listen->pathnet.rem = from;
		assp_handle_rx(cnx_listen->tcb, rxp);
		if (cnx_listen->tcb->pstate != PS_CLOSED) {
			printf("%p: Connect from %s:%d\n", cnx_listen->tcb,
				inet_ntoa(from.sin_addr), from.sin_port);
			if (allcnx) allcnx->pprev = &cnx_listen->next;
			cnx_listen->next = allcnx;
			cnx_listen->pprev = &allcnx;
			allcnx = cnx_listen;
			if (setup_listen(filename, sock, tq)) return 1;
		}
		return 0;
	}
	if (sameport(&cxp->pathnet.rem, &from)
		&& cxp->tcb->cid == cid) {
		assp_handle_rx(cxp->tcb, rxp);
		return 0;
	}
 }
}

#ifdef USE_POLL
static int serve(int sock, char *filename)
{
 TIMEQUEUE_IMPL tqi;
 struct pollfd fds[2];
 TIMEQUEUE tq;
 timequeue_pq_init(&tq, &tqi);
 fds[0].fd = sock;
 fds[0].events = POLLIN;
 fds[1].fd = 0;
 fds[1].events = POLLIN;
 if (setup_listen(filename, sock, &tq)) return 1;
 for (;;) {
	long tmo = tq.vtbl->next_timer_mS(tq.tqi);
	ASSP_CONN *cnx;
	int i;
	i = poll(fds, arlen(fds), tmo);
#ifdef USE_GLOBALTIME
	systick();
#endif
	if (i) {
		if (i < 0) {
			if (errno == EINTR) continue;
			perror("poll() failed");
			return 1;
		}
		if (fds[0].revents) {
			if (handle_frame(sock, filename, &tq)) return 1;
		} else if (fds[1].revents) {
			for (cnx=allcnx; cnx; ) {
				ASSP_CONN *nx = cnx->next;
				assp_tcb_stats(cnx->tcb);
				cnx = nx;
			}
			assp_tcb_stats(cnx_listen->tcb);
			return 0;
		}
	}
 }
}
#endif

#ifdef USE_WFMO
static int serve(int sock, char *filename)
{
 TIMEQUEUE_IMPL tqi;
 HANDLE hlist[2];
 TIMEQUEUE tq;
 timequeue_pq_init(&tq, &tqi);
 hlist[0] = CreateEvent(0, 1, 0, 0);
 if (!hlist[0]) {
	lasterr("CreateEvent failed");
	return 1;
 }
 if (WSAEventSelect(sock, hlist[0], FD_READ)) {
	printf("WSAEventSelect failed, code %d", WSAGetLastError());
	return 1;
 }
 hlist[1] = GetStdHandle(STD_INPUT_HANDLE);
 if (setup_listen(filename, sock, &tq)) return 1;
 for (;;) {
	long tmo = tq.vtbl->next_timer_mS(tq.tqi);
	ASSP_CONN *cnx;
	DWORD n;
	n = WaitForMultipleObjects(1, hlist, 0, tmo);
#ifdef USE_GLOBALTIME
	systick();
#endif
	switch (n) {
		WSANETWORKEVENTS ev;
	case WAIT_OBJECT_0:
		if (WSAEnumNetworkEvents(sock, hlist[0], &ev)) {
			reperr("WSAEnumNetworkEvents() failed");
		}
		if (ev.lNetworkEvents & FD_READ) {
			ev.lNetworkEvents &= ~FD_READ;
			if (handle_frame(sock, filename, &tq)) return 1;
		}
		if (ev.lNetworkEvents) {
			printf("Unknown event ignored: 0x%x\n", ev.lNetworkEvents);
		}
		break;
	case WAIT_OBJECT_0+1:
		for (cnx=allcnx; cnx; ) {
			ASSP_CONN *nx = cnx->next;
			assp_tcb_stats(cnx->tcb);
			cnx = nx;
		}
		assp_tcb_stats(cnx_listen->tcb);
		return 0;
	case WAIT_TIMEOUT:
		break;
	case WAIT_FAILED:
		lasterr("WaitForMultipleObjects failed");
		return 1;
	default:
		printf("WaitForMultipleObjects returned 0x%x!\n", n);
		return 1;
	}
 }
}
#endif

static int bindlocal(int sock, char *addr, char *port)
{
 struct servent *sp = getservbyname(port, "udp");
 struct sockaddr_in sa;
 sa.sin_family = AF_INET;
 if (addr) {
	struct hostent *hp = gethostbyname(addr);
	if (hp) sa.sin_addr.s_addr = *(unsigned long *) hp->h_addr;
	else {
		sa.sin_addr.s_addr = inet_addr(addr);
		if (sa.sin_addr.s_addr == INADDR_NONE) {
			printf("Cannot interpret address: %s\n", addr);
			return -2;
		}
	}
 } else {
	sa.sin_addr.s_addr= htonl(INADDR_ANY);
 }
 if (sp) sa.sin_port = sp->s_port;
 else {
 	char *ep;
	sa.sin_port = htons((short) strtol(port, &ep, 0));
	if (*ep) {
		printf("Cannot interpret port: %s\n", port);
		return -1;
	}
 }
 if (bind(sock, (struct sockaddr *) &sa, sizeof(sa))) {
 	reperr("bind() failed");
 	return -1;
 }
 return 0;
}

static int server(char *addr, char *port, unsigned rcvbuf, char *filename)
{
 int sock;
 sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
 if (sock == -1) {
 	// report error
 	reperr("socket() failed");
 	return -1;
 }
 if (bindlocal(sock, addr, port)) return 1;
 if (rcvbuf) {
	int optlen = sizeof(rcvbuf);
	if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void *) &rcvbuf, sizeof(rcvbuf))) {
		reperr("setsockopt() failed");
		return -1;
	}
	if (getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void *) &rcvbuf, &optlen)) {
		reperr("getsockopt() failed");
		return -1;
	}
	printf("rcvbuf = %d\n", rcvbuf);
 }
 printf("ready\n");
 fflush(stdout);
 randent_init();
 return serve(sock, filename);
}

static const char version[] = "@(#)$from: " __FILE__ " "
#include "../gen/version.i"
	" $";
#include "gen/server.args.i"

int main(int argc, char **argv)
{
#ifdef TiNGTYPE_WINNT
 WSADATA ver;
 int i;
#endif
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || !argv[0] || !argv[1] || argv[2]) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE " port file\n", progname);
	return 1;
 }
#ifdef TiNGTYPE_WINNT
 i = WSAStartup(MAKEWORD(2, 0), &ver);
 if (i) {
	fprintf(stderr, "Cannot initialise socket system: error %d\n", i);
	return 1;
 }
#endif
 assp_trace(arg.trace);
 if (server(arg.addr, argv[0], arg.rcvbuf, argv[1])) {
	printf("assp_server failed\n");
	return 1;
 }
 return 0;
}
