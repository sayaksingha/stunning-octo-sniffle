/* client.c - ASSP over UDP client */

#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>
#include <sys/types.h>

#ifdef TiNGTYPE_LINUX
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#define USE_POLL
#define USE_GETTIMEOFDAY
#endif


#ifdef TiNGTYPE_WINNT
#include <io.h>
#include "../libutil/WINNT/wind.h"
#include "../libutil/WINNT/lasterr.h"
#define reperr(x) formaterr(x, WSAGetLastError())
#define open _open
#define write _write
#define USE_WFMO
#define USE_FTIME
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
	struct connection *next, **pprev;
	uint_fast64_t bytes;
#ifdef USE_FTIME
	struct timeb start;
#endif
#ifdef USE_GETTIMEOFDAY
	struct timeval start;
#endif
	TCB *tcb;
	TCBPATH_IMPL pathnet;
	TCBPATH tcbpath;
	FILE *file;
	FILE *log;
};

static void mark_start(ASSP_CONN *cnx)
{
#ifdef USE_FTIME
 ftime(&cnx->start);
#endif
#ifdef USE_GETTIMEOFDAY
 gettimeofday(&cnx->start, 0);
#endif
}

static double since_start(ASSP_CONN *cnx)
{
#ifdef USE_FTIME
 struct timeb end;
 ftime(&end);
 return end.time - cnx->start.time
	+ (end.millitm - cnx->start.millitm) / 1000.0;
#endif
#ifdef USE_GETTIMEOFDAY
 struct timeval end;
 gettimeofday(&end, 0);
 return end.tv_sec - cnx->start.tv_sec
	+ (end.tv_usec - cnx->start.tv_usec) / 1000000.0;
#endif
}

static void cnx_txack(ASSP_CONN *cnx, PACKET *txp, int acked)
{
 (void) cnx;
 (void) txp;
 (void) acked;
 	// don't care
}

static PACKET *cnx_nexttx(ASSP_CONN *cnx, int *flags)
{
 PACKET *txp = assp_txpacket(cnx->tcb, 0);
 txp->dataend = txp->datastart;
 *flags = ASSP_F_EOF;
 return txp;
}

static int cnx_sof(ASSP_CONN *cnx)
{
 (void) cnx;
 printf("sof\n");
 return 0;
}

static void cnx_deliver(ASSP_CONN *cnx, PACKET *pkt)
{
 size_t len = pkt->dataend - pkt->datastart;
 size_t nw;
 cnx->bytes += len;
 if (cnx->log) {
#ifdef USE_FTIME
	struct timeb now;
	ftime(&now);
	fprintf(cnx->log, "%u.%03u\t%" PRIu64 "\n", now.time, now.millitm, cnx->bytes);
#endif
#ifdef USE_GETTIMEOFDAY
	struct timeval now;
	gettimeofday(&now, 0);
	fprintf(cnx->log, "%lu.%06lu\t%" PRIu64 "\n", (unsigned long) now.tv_sec, (unsigned long) now.tv_usec, cnx->bytes);
#endif
 }
 nw = fwrite(pkt->datastart, 1, len, cnx->file);
 if (nw != len) {
	perror("fwrite");
	printf("fwrite() wrote %d, not %d\n", nw, len);
	abort();
 }
 cnx->tcbpath.vtbl->rxfree(&cnx->pathnet, pkt);
}

static void cnx_eof(ASSP_CONN *cnx)
{
 printf("EOF\n");
 if (!cnx->file) return;
 fflush(cnx->file);
 if (ferror(cnx->file)) {
	printf("ferror() reports error writing file\n");
 }
}

static void cnx_rx_peek(ASSP_CONN *cnx, unsigned ofs, PACKET *pkt)
{
 (void) cnx;
 (void) ofs;
 (void) pkt;
}

static void cnx_stopped(ASSP_CONN *cnx)
{
 double dtim = since_start(cnx);
 assp_tcb_stats(cnx->tcb);
 if (dtim) {
		// cast to signed only for Microsoft compiler, which is
		// inexplicably unable to convert unsigned 64-bit numbers
		// to floating-point!
	printf("Speed: %g/%g = %g bytes/sec = %g bps\n",
		(double) (int_fast64_t) cnx->bytes, dtim,
		(int_fast64_t) cnx->bytes / dtim,
		8.0 * (int_fast64_t) cnx->bytes / dtim);
 }
}

static void cnx_closed(ASSP_CONN *cnx)
{
 if (cnx->file && fclose(cnx->file)) perror("fclose() failed");
 assp_tcb_dtor(cnx->tcb);
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

static ASSP_CONN *setup_cnx(char *filename, int sock, TIMEQUEUE *tq, char *logfile)
{
 ASSP_CONN *cnx = err_malloc(sizeof(*cnx), "connection");
 if (cnx) {
	cnx->tcb = err_malloc(sizeof(*cnx->tcb), "TCB");
	if (!cnx->tcb) {
		free(cnx);
		return 0;
	}
	pathnet_init(&cnx->tcbpath, &cnx->pathnet, sock);
	cnx->file = fopen(filename, "w");
	if (!cnx->file) {
		perror("fopen() failed");
		printf("Cannot create file: %s\n", filename);
		free(cnx->tcb);
		free(cnx);
		return 0;
	}
	if (!logfile) cnx->log = 0;
	else {
		cnx->log = fopen(logfile, "w");
		if (!cnx->log) {
			perror("fopen() failed");
			printf("Cannot create log file: %s\n", logfile);
			fclose(cnx->file);
			free(cnx->tcb);
			free(cnx);
			return 0;
		}
	}
	assp_tcb_init(cnx->tcb, cnx, tq, &cnx->tcbpath, &assp_vtbl);
 }
 return cnx;
}

static int sameport(struct sockaddr_in *l, struct sockaddr_in *r)
{
 return l->sin_addr.s_addr == r->sin_addr.s_addr
	&& l->sin_port == r->sin_port;
}

static int handle_frame(ASSP_CONN *cnx)
{
 struct sockaddr_in from;
 int fromlen = sizeof(from);
 PACKET *rxp = cnx->tcbpath.vtbl->rxalloc(&cnx->pathnet, 1500);
 int rxlen;
 uint_fast32_t cid;
 if (!rxp) {
 	perror("malloc() failed");
 	return 1;
 }
 rxlen = recvfrom(cnx->pathnet.sock, (char *) rxp->rawstart, rxp->dataend - rxp->rawstart, 0,
	(struct sockaddr *) &from, &fromlen);
 if (rxlen <= 0) {
	reperr("recvfrom() failed");
	return 1;
 }
 rxp->dataend = rxp->rawstart + rxlen;
 cid = assp_rx_cid(rxp);
 if (!sameport(&cnx->pathnet.rem, &from)) {
	char bfrom[32];
	char bpeer[32];
	strcpy(bfrom, inet_ntoa(from.sin_addr));
	strcpy(bpeer, inet_ntoa(cnx->pathnet.rem.sin_addr));
	printf("Frame from %s:%d, not %s:%d\n", 
		bfrom, ntohs(from.sin_port),
		bpeer, ntohs(cnx->pathnet.rem.sin_port));
 } else if (cnx->tcb->cid != cid) {
	printf("Frame with CID=%x, not %x\n", cid, cnx->tcb->cid);
 } else {
	assp_handle_rx(cnx->tcb, rxp);
 }
 return 0;
}

#ifdef USE_POLL
static int fetchfile(ASSP_CONN *cnx, TIMEQUEUE *tq, uint_fast32_t cid, unsigned rep)
{
 struct pollfd fds[1];
 fds[0].fd = cnx->pathnet.sock;
 fds[0].events = POLLIN;
 for (;;) {
	assp_connect(cnx->tcb, cid);
	mark_start(cnx);
	for (;;) {
		long tmo = tq->vtbl->next_timer_mS(tq->tqi);
		int i;
		if (cnx->tcb->pstate == PS_TIME_WAIT && rep) break;
		if (cnx->tcb->pstate == PS_CLOSED) return 0;
		i = poll(fds, 1, tmo);
#ifdef USE_GLOBALTIME
		systick();
#endif
		if (i) {
			if (i < 0) {
				if (errno == EINTR) continue;
				perror("poll() failed");
				return 1;
			}
			if (handle_frame(cnx)) return 1;
		}
	}
	poll(0, 0, rep);
 }
}
#endif

#ifdef USE_WFMO
static int fetchfile(ASSP_CONN *cnx, TIMEQUEUE *tq, uint_fast32_t cid, unsigned rep)
{
 HANDLE hlist[2];
 hlist[0] = CreateEvent(0, 1, 0, 0);
 if (!hlist[0]) {
	lasterr("CreateEvent failed");
	return 1;
 }
 if (WSAEventSelect(cnx->pathnet.sock, hlist[0], FD_READ)) {
	printf("WSAEventSelect failed, code %d", WSAGetLastError());
	return 1;
 }
 hlist[1] = GetStdHandle(STD_INPUT_HANDLE);
 for (;;) {
	assp_connect(cnx->tcb, cid);
	mark_start(cnx);
	for (;;) {
		long tmo = tq->vtbl->next_timer_mS(tq->tqi);
		DWORD n;
		if (cnx->tcb->pstate == PS_TIME_WAIT && rep) break;
		if (cnx->tcb->pstate == PS_CLOSED) return 0;
		n = WaitForMultipleObjects(1, hlist, 0, tmo);
#ifdef USE_GLOBALTIME
		systick();
#endif
		switch (n) {
			WSANETWORKEVENTS ev;
		case WAIT_OBJECT_0:
			if (WSAEnumNetworkEvents(cnx->pathnet.sock, hlist[0], &ev)) {
				reperr("WSAEnumNetworkEvents() failed");
			}
			if (ev.lNetworkEvents & FD_READ) {
				ev.lNetworkEvents &= ~FD_READ;
				if (handle_frame(cnx)) return 1;
			}
			if (ev.lNetworkEvents) {
				printf("Unknown event ignored: 0x%x\n", ev.lNetworkEvents);
			}
			break;
		case WAIT_OBJECT_0+1:
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
	Sleep(rep);
 }
}
#endif

static int bindlocal(int sock)
{
 struct sockaddr_in sa;
 struct timeb now;
 int v;
 ftime(&now);
 v = now.time ^ now.millitm;
 sa.sin_family = AF_INET;
 sa.sin_port = v ^ (v >> 16);
 sa.sin_port |= 0x8000;
 sa.sin_addr.s_addr= htonl(INADDR_ANY);
 if (bind(sock, (struct sockaddr *) &sa, sizeof(sa))) {
 	reperr("bind() failed");
	fprintf(stderr, "Cannot bind to port %d\n", sa.sin_port);
 	return -1;
 }
 return 0;
}

static int connrem(int sock, char *addr, char *port, struct sockaddr_in *sap)
{
 struct hostent *hp;
 struct servent *sp = getservbyname(port, "udp");
 sap->sin_family = AF_INET;
 if (sp) sap->sin_port = sp->s_port;
 else {
 	char *ep;
	sap->sin_port = htons((short) strtol(port, &ep, 0));
	if (*ep) {
		printf("Cannot interpret port: %s\n", port);
		return -1;
	}
 }
 hp = gethostbyname(addr);
 if (hp) sap->sin_addr.s_addr = *(unsigned long *) hp->h_addr;
 else {
	sap->sin_addr.s_addr = inet_addr(addr);
	if (sap->sin_addr.s_addr == INADDR_NONE) {
		printf("Cannot interpret address: %s\n", addr);
		return -2;
	}
 }
#if 0
 if (connect(sock, (struct sockaddr *) sap, sizeof(*sap))) {
	reperr("connect() failed");
	return 1;
 }
#else
 (void) sock;
#endif
 return 0;
}

static int client(char *addr, char *port, uint_fast32_t cid, unsigned rep, unsigned rcvbuf, char *filename, char *logfile)
{
 TIMEQUEUE_IMPL tqi;
 ASSP_CONN *cnx;
 TIMEQUEUE tq;
 int sock;
 randent_init();
 timequeue_pq_init(&tq, &tqi);
 sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
 if (sock == -1) {
	reperr("socket() failed");
 }
 if (rcvbuf) {
	int optlen = sizeof(rcvbuf);
	if (getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void *) &rcvbuf, &optlen)) {
		reperr("getsockopt() failed");
		return -1;
	}
	printf("default rcvbuf = %d\n", rcvbuf);
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
 if (bindlocal(sock)) return 1;
#ifdef TiNGTYPE_LINUXz
  {
	int opt = 1;
	if (setsockopt(sock, SOL_IP, IP_RECVERR, &opt, sizeof(opt))) {
		reperr("setsockopt() failed");
		return -1;
	}
 }
#endif
 cnx = setup_cnx(filename, sock, &tq, logfile);
 if (!cnx) {
	printf("Cannot setup connection\n");
 	return 1;
 }
 if (connrem(sock, addr, port, &cnx->pathnet.rem)) return 1;
 printf("ready\n");
 fflush(stdout);
 return fetchfile(cnx, &tq, cid, rep);
}

static const char version[] = "@(#)$from: " __FILE__ " "
#include "../gen/version.i"
	" $";
#include "gen/client.args.i"

int main(int argc, char **argv)
{
#ifdef TiNGTYPE_WINNT
 WSADATA ver;
 int i;
#endif
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || !argv[0] || !argv[1] || !argv[2] || argv[3]) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE " addr port file\n", progname);
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
 if (client(argv[0], argv[1], arg.cid, arg.repwait, arg.rcvbuf, argv[2], arg.logfile)) {
	printf("assp_client failed\n");
	return 1;
 }
 return 0;
}
