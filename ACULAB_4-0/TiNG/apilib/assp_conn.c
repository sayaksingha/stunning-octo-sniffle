#include "assp_conn.h"
#include "create_socket.h"
#include <string.h>
#include "../hsif/kernel.h"
#include "../hsif/kernel_x.h"
#include "../assp/assp.h"
#include "timequeue_pq_evnt.h"
#include "pathipv4.h"
#include "px_assp.h"
#include "px_util.h"
#include "trace.h"
#include "TiNGcommon.h"
#include <pthread.h>

#ifdef TiNGTYPE_LINUX
#define TiNG_POSIX_SOCKETS
#define USE_POLL
#include <errno.h>
#endif

#ifdef TiNGTYPE_QNX
#define TiNG_POSIX_SOCKETS
#define USE_POLL
#include <errno.h>
#endif


#define arlen(x) (sizeof(x)/sizeof(*(x)))

#ifdef TiNG_POSIX_SOCKETS
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#ifndef closesocket
#define closesocket(fd) close(fd)
#endif
#include "POSIX/socket.h"
#endif


#ifdef TiNGTYPE_WINNT
#define USE_WIN_EVENTS
typedef int socklen_t;
#endif

#define arlen(x) (sizeof(x)/sizeof(*(x)))


STATIC int conn_sof(ASSP_CONN *cnx)
{
 (void) cnx;
 return 0;
}

STATIC void conn_rx_peek(ASSP_CONN *cnx, unsigned ofs, PACKET *pkt)
{
 (void) cnx;
 (void) ofs;
 (void) pkt;
}


STATIC void conn_deliver(ASSP_CONN *cnx, PACKET *pkt)
{
 mutx_enter(&cnx->rx_mx);

 *cnx->rxq_endptr = pkt;
 pkt->next = 0;
 cnx->rxq_endptr = &pkt->next;
 signal_event(evnt_to_sig(cnx->evnt));
 mutx_leave(&cnx->rx_mx);
}


STATIC void conn_eof(ASSP_CONN *cnx)
{
 // FIXME: generate infinite number of fake DEAD messages
 (void) cnx;
}


STATIC void conn_stopped(ASSP_CONN *cnx)
{
 PACKET *pkt;
 mutx_enter(&cnx->rx_mx);
 mutx_enter(&cnx->tx_mx);
 cnx->protostate = AST_STOPPED;
 	// not that these will ever get transmitted, of course, but
 	// we'd better not lose them
 while (cnx->txq) {
 	pkt = cnx->txq;
 	cnx->txq = pkt->next;
	cnx->path.vtbl->rxfree(cnx->path.pi, pkt);
 }
 cnx->txq_last = 0;
 mutx_leave(&cnx->tx_mx);
 while (cnx->rxq) {
	pkt = cnx->rxq;
	cnx->rxq = pkt->next;
	cnx->tcb.path->vtbl->rxfree(cnx->tcb.path->pi, pkt);
 }
 cnx->rxq_endptr = &cnx->rxq;
 if (cnx->status < kAsspConnDisconnected) {
	cnx->status = kAsspConnDisconnected;
	signal_event(evnt_to_sig(cnx->evnt));
 }
 mutx_leave(&cnx->rx_mx);
}

STATIC void conn_closed(ASSP_CONN *cnx)
{
 PACKET *pkt;
 mutx_enter(&cnx->rx_mx);
 mutx_enter(&cnx->tx_mx);
 cnx->protostate = AST_CLOSED;
 	// not that these will ever get transmitted, of course, but
 	// we'd better not lose them
 while (cnx->txq) {
 	pkt = cnx->txq;
 	cnx->txq = pkt->next;
	cnx->path.vtbl->rxfree(cnx->path.pi, pkt);
 }
 cnx->txq_last = 0;
 mutx_leave(&cnx->tx_mx);
 while (cnx->rxq) {
	pkt = cnx->rxq;
	cnx->rxq = pkt->next;
	cnx->tcb.path->vtbl->rxfree(cnx->tcb.path->pi, pkt);
 }
 cnx->rxq_endptr = &cnx->rxq;
 if (cnx->status < kAsspConnDisconnected) {
	cnx->status = kAsspConnDisconnected;
	signal_event(evnt_to_sig(cnx->evnt));
 } else if (cnx->status == kAsspConnStopping) {
	cnx->status = kAsspConnFinished;
	signal_event(evnt_to_sig(cnx->evnt));
 }
 mutx_leave(&cnx->rx_mx);
}

STATIC PACKET *conn_nexttx(ASSP_CONN *cnx, int *flags)
{
 PACKET *pkt;
 mutx_enter(&cnx->tx_mx);

 pkt = cnx->txq;
 *flags = 0;
 if (pkt) {
 	cnx->txq = pkt->next;
	if (!cnx->txq) cnx->txq_last = 0;
	pkt->next = 0;
 }
 mutx_leave(&cnx->tx_mx);
 if (pkt && cnx->crypt) {
	uint8_t* p  = pkt->datastart;
	int len;
	struct crypt_parms* crypt = cnx->crypt;
	int changeOverAfterMessage = 0;

	if (crypt->firstKeyIdPending || crypt->keyChangePending) {
		if (pkt->u.tx.changeover) {
			if (crypt->firstKeyIdPending) {
				crypt->firstKeyIdPending = 0;
				// This first message must not be encrypted
				p += 4*sizeof(uint32_t);
			} else {
				// This must be encrypted with old key, then we changeover
				changeOverAfterMessage = 1;
			}
		}
	}

	len = pkt->dataend-p;

	if (len) {
		if (pkt->u.tx.payloadplaintext) {
			// these will have 8 octet data header to encrypt
			// and just data following (no messages appended)
			(crypt->encdec)(kSMSecCryptEncode,8,crypt->context,p);
		} else {
			int remnant = (len & 7);

			if (remnant == 4) {
				 // pad with NULL message 0x800
				 // to multiple of 64 bits
		 		pkt->dataend = mkmsg(pkt->dataend, MESSAGE(MSG_PAD, 1));

				len += 4;
			} else if (remnant != 0) {
				abort();
			}
			(crypt->encdec)(kSMSecCryptEncode,len,crypt->context,p);
		}
	}

	if (changeOverAfterMessage) {
		// subsequent encryption to take place with new key
		crypt->keyChangePending = 0;
		crypt->keyId = crypt->newKeyId;
		(crypt->encdec)(kSMSecCryptSetEKeyToDKey,0,crypt->context,0);
	}
 }
 return pkt;
}

STATIC void conn_txack(ASSP_CONN *cnx, PACKET *txp, int acked)
{
 (void) cnx;
 (void) txp;
 (void) acked;	// don't care - can't keep waiting if it will never be acked
}

struct assp_tcb_vtbl assp_conn_vtbl = {
	conn_sof,
	conn_rx_peek,
	conn_deliver,
	conn_eof,
	conn_stopped,
	conn_closed,
	conn_nexttx,
	conn_txack,
};

static int handle_frame(ASSP_CONN *cnx)
{
	// should use path MTU, but try getting that out of a UDP socket!
 PACKET *rxp;
 struct sockaddr_storage from;
 socklen_t fromlen = sizeof(from);
 int rxlen;
#ifdef TiNGTYPE_WINNT
 DWORD err;
#endif
 rxp = cnx->path.vtbl->rxalloc(cnx->path.pi, 1500);
 if (!rxp) return 1;
 rxlen = recvfrom(cnx->pathimpl.sock, (void *) rxp->rawstart,
 	rxp->rawend - rxp->rawstart, 0, (struct sockaddr *) &from, &fromlen);
 if (rxlen == rxp->rawend - rxp->rawstart) {
 	if (TiNGtrace) olog("over-long packet (>=%d)\n", rxlen);
 	rxlen = 0;
 } else if (rxlen > 0) {
	uint32_t cid;
	rxp->dataend = rxp->rawstart + rxlen;
	cid = assp_rx_cid(rxp);
	if (cid != 0) {
		;
	} else {
		// now we have seen a good packet (first time or again), tollerate sporadic ICMPs
	    cnx->max_recv_fail_count = kMaxASSPConnRecvFailsBurst;
		
		// reset burst counter
		if (cnx->protostate == AST_PERTURBED) {
			cnx->protostate = AST_RUN;
			cnx->rcv_fail_count = 0;
		}		
		assp_handle_rx(&cnx->tcb, rxp);
		return 0;
	}
	rxlen = 0;	// just ignore it
#ifdef TiNGTYPE_WINNT
 } else if ((err=WSAGetLastError()) == WSAEWOULDBLOCK) {
 	rxlen = 0;	// spurious wakeup from winsock
#endif
#ifdef TiNGTYPE_LINUX
 } else if (errno == EAGAIN) {	// bad checksum
 	rxlen = 0;
#endif
#ifdef TiNG_POSIX_SOCKETS
 } else if (errno == ECONNREFUSED || errno == ECONNRESET) {	// gone away
#endif
#ifdef TiNGTYPE_WINNT
 } else if (err == WSAECONNREFUSED || err == WSAECONNRESET) {// gone away
#endif
	/* Ugh! Windows uses ECONNRESET and the Single Unix Specification
	 * seems to agree, even though ECONNREFUSED would be more intuitive.
	 * Linux only uses ECONNREFUSED for UDP (checked in 2.6.11 sources).
	 */
 	if (TiNGtrace) {
 		olog("ASSP got connection refused from [%s]:%d\n",
			cnx->pathimpl.rem,
			cnx->pathimpl.asspport);
	}
 } else {
	osockerr("recvfrom() failed");
 }
 cnx->path.vtbl->rxfree(cnx->path.pi, rxp);
 if (rxlen) {
    if (cnx->protostate == AST_RUN) {
		cnx->protostate = AST_PERTURBED;
	}
 	if (cnx->protostate == AST_PERTURBED) {
		cnx->rcv_fail_count += 1;
		// provokes more ICMPs if genuine
		assp_validate(&cnx->tcb);

		if (cnx->rcv_fail_count > cnx->max_recv_fail_count) {
			if (TiNGtrace && (cnx->max_recv_fail_count != 0)) {
				olog("aborting ASSP connection after %d connection refusals from [%s]:%d\n",
					cnx->rcv_fail_count,
					cnx->pathimpl.rem,
					cnx->pathimpl.asspport);
			}
			assp_abort(&cnx->tcb, 0, "recvfrom() failed");
		}
	}
 }
 return 0;
}

static void asspmon_send(SOCKET sock, ASSP_CONN *cnx)
{
#ifdef MSG_NOSIGNAL
 const int flags = MSG_NOSIGNAL;
#else
 const int flags = 0;
#endif
 int mlen;
 int ns;
 mlen = strlen(cnx->buff);
 ns = send(sock, cnx->buff, mlen, flags);
 if (ns < 0) {
 	osockerr("send(asspmon) failed");
 } else if (ns != mlen) {
 	osockerr("send(asspmon) failed - short write");
 }
}

static int asspmon_cnx(ASSP_CONN *cnx, SOCKET *sock)
{
 struct sockaddr_storage srca;
 socklen_t srcalen = sizeof(srca);
 char srcname[256];
 char srcport[16];
 int s;
 *sock = create_connected_socket(cnx->pathimpl.asspmonaddr, 1, cnx->pathimpl.asspmonport, 1, 0, NULL, NULL);
 if (*sock == -1) {
	return 1;
 }

 if (getsockname(cnx->pathimpl.sock, (struct sockaddr *) &srca, &srcalen)) {
#ifdef TiNGTYPE_WINNT
	/* the windows documentation says that the local address should be known for connected 
		sockets, like this one. However, it then says that the address may not be known for
		datagram sockets until I/O has occurred. So this code is to handle that situation.
		NB. It has never been unknown in testing so far though!
	*/
	if (WSAGetLastError() == WSAEINVAL) { // address not known yet
		uint8_t b;
		cnx->path.vtbl->netsendpkt(&cnx->pathimpl, &b, &b); // send zero length packet so local address gets chosen
		if (getsockname(cnx->pathimpl.sock, (struct sockaddr *) &srca, &srcalen)) {
			osockerr("getsockname(asspmon) failed");
			return 1;
		}
	} else {
		osockerr("getsockname(asspmon) failed");
		return 1;
	}
#else
 	osockerr("getsockname(asspmon) failed");
 	return 1;
#endif
 }
 s = getnameinfo((struct sockaddr*)&srca, srcalen, srcname, sizeof(srcname), srcport, sizeof(srcport), NI_NUMERICHOST|NI_NUMERICSERV);
 if (s) {
	olog("getnameinfo failed: %s\n", gai_strerror(s));
	return 1;
 }
 cnx->buff[0] = 'a';
 if (srca.ss_family == AF_INET) {
	 // both are IPv4 (?)
	sprintf(cnx->buff + 1, "%s:%s-%s:%d\n", srcname, srcport, cnx->pathimpl.rem, cnx->pathimpl.asspport);
 } else if (srca.ss_family == AF_INET6) {
	char *percent;
	if (IN6_IS_ADDR_V4MAPPED(&((struct sockaddr_in6*)&srca)->sin6_addr)) { // pass IPv4 MAPPED addresses in the IPv4 format
		// strlen("::ffff:") == 7
		sprintf(cnx->buff + 1, "%s:%s-%s:%d\n", srcname + 7, srcport, cnx->pathimpl.rem + 7, cnx->pathimpl.asspport);
	} else if ((percent = strchr(srcname, '%')) != NULL) { // remove scope as it only applies locally!
		char remote[256];
		*percent = 0;
		strncpy(remote, cnx->pathimpl.rem, sizeof(remote) - 1);
		remote[255] = 0;
		percent = strchr(remote, '%');
		if (percent) *percent = 0;
		sprintf(cnx->buff + 1, "[%s]:%s-[%s]:%d\n", srcname, srcport, remote, cnx->pathimpl.asspport);
	} else {
		sprintf(cnx->buff + 1, "[%s]:%s-[%s]:%d\n", srcname, srcport, cnx->pathimpl.rem, cnx->pathimpl.asspport);
	}
 } else {
	if (TiNGtrace) olog("asspmon_cnx unknown family %d\n", srca.ss_family);
	return 1;
 }
 if (TiNGtrace > 4) olog("ASSP:%s", cnx->buff);
 asspmon_send(*sock, cnx);
 return 0;
}

static void asspmon_close(ASSP_CONN *cnx, SOCKET sock)
{
 if (sock == -1) return;
 cnx->buff[0] = 'd';
 if (TiNGtrace > 4) olog("ASSP:%s", cnx->buff);
 asspmon_send(sock, cnx);
 closesocket(sock);
}

static int handle_asspmon(ASSP_CONN *cnx, SOCKET sock)
{
 char buff[1024];
 int nc = recv(sock, buff, sizeof(buff), 0);
 if (nc <= 0) {
 	if (nc < 0) osockerr("recv(asspmon) failed");
 	else if (TiNGtrace > 3) olog("[asspmon=EOF]\n");
	assp_validate(&cnx->tcb);
 	return 1;
 }
 if (TiNGtrace > 3) {
	olog("[asspmon:%s]\n", buff);
 }
 return 0;
}

#ifdef USE_POLL
static int gcctsp_poll(struct pollfd *fds, unsigned nfds, int timeout)
{
 return poll(fds, nfds, timeout);
}
#endif


STATIC int conn_is_idle(ASSP_CONN *cnx)
{
 int sts = 1;
 if (cnx->protostate != AST_CLOSED) {
	if (TiNGtrace > 4) {
		olog("%p: assp_thread: ps=%d\n", cnx, cnx->protostate);
	}
	sts = 0;
 }
 return sts;
}


static void stop_tcb(TCB *tcb)
{
 PACKET *pkt = assp_txpacket(tcb, 9);
 if (pkt) {
 	strcpy((char*)pkt->datastart, "stopping");
 	pkt->dataend += strlen((char*)pkt->datastart);
 }
 assp_abort(tcb, pkt, "stopping");
}

static int conn_is_stopping(ASSP_CONN *cnx)
{
 int stop;
 mutx_enter(&cnx->tx_mx);
 stop = cnx->stop;
 mutx_leave(&cnx->tx_mx);
 return stop;
}

static void *assp_thread(void *arg)
{
 ASSP_CONN *cnx = arg;
 SOCKET asspmon_sock = -1;
 int sts = 0;
 int aborted_tcb = 0;
#define ASSP_EV 0
#define TQ_EV 1
#define ASSPMON_EV 2
#define COMMAND_EV 3
#ifdef USE_POLL
 struct pollfd fds[4];
 fds[ASSP_EV].fd = cnx->pathimpl.sock;
 fds[ASSP_EV].events = POLLIN;
 fds[TQ_EV].fd = evnt_to_listen(cnx->tq_event);
 fds[TQ_EV].events = POLLIN;
 fds[COMMAND_EV].fd = evnt_to_listen(cnx->command_event);
 fds[COMMAND_EV].events = POLLIN;
#endif
#ifdef USE_WIN_EVENTS
 HANDLE events[4];
 events[ASSPMON_EV] = NULL;
 events[ASSP_EV] = CreateEvent(0,0,0,0);
 if (events[ASSP_EV] == NULL) {
	osockerr("CreateEvent() failed");
	sts = 1;
 } else {
	events[ASSPMON_EV] = CreateEvent(0,0,0,0);
	if (events[ASSPMON_EV] == NULL) {
		osockerr("CreateEvent() failed");
		sts = 1;
	} else {
		sts = WSAEventSelect(cnx->pathimpl.sock, events[ASSP_EV], FD_READ);
		if (sts) {
			osockerr("WSAEventSelect() failed");
			sts = 1;
		}
	}
 }
 events[TQ_EV] = evnt_to_listen(cnx->tq_event);
 events[COMMAND_EV] = evnt_to_listen(cnx->command_event);
#endif
#ifdef USE_WIN_EVENTS
// SetThreadPriority(GetCurrentThread() ,THREAD_PRIORITY_TIME_CRITICAL);
#endif
 {
	int buflen = 128 * 2048; // max window is 128, pkts are less than 2048 but we want a multiple of page size
	if (setsockopt(cnx->pathimpl.sock, SOL_SOCKET, SO_SNDBUF, (char*)&buflen, sizeof(buflen))) {
		if (TiNGtrace > 4) osockerr("setsockopt failed to set send buffer size");
	}
	buflen = 128 * 2048;
	if (setsockopt(cnx->pathimpl.sock, SOL_SOCKET, SO_RCVBUF, (char*)&buflen, sizeof(buflen))) {
		if (TiNGtrace > 4) osockerr("setsockopt failed to set receive buffer size");
	}
 }
 if (!sts) {
	 if (asspmon_cnx(cnx, &asspmon_sock)) {
		if (asspmon_sock != -1) closesocket(asspmon_sock);
		asspmon_sock = -1;
	#ifdef USE_WIN_EVENTS
	 } else {
		sts = WSAEventSelect(asspmon_sock, events[ASSPMON_EV], FD_READ);
		if (sts) {
			osockerr("WSAEventSelect() failed");
			closesocket(asspmon_sock);
			asspmon_sock = -1;
		}
	#endif
	 }
 }
#ifdef USE_POLL
 fds[ASSPMON_EV].fd = asspmon_sock;
 fds[ASSPMON_EV].events = POLLIN;
#endif
#ifdef USE_POLL
 for (;!sts;) {
	int e;
	e = poll(&fds[COMMAND_EV], 1, -1);
	if (e == 1) break;
	if (e >= 0 || errno != EINTR) {
		sts = 1;
	}
 }
#endif
#ifdef USE_WIN_EVENTS
 sts = WaitForSingleObject(events[COMMAND_EV], INFINITE);
 if (sts != WAIT_OBJECT_0) {
	sts = 1;
 } else {
	sts = 0;
 }
#endif
 evnt_clear(cnx->command_event);
 if (!sts && !conn_is_stopping(cnx)) {
	 assp_connect(&cnx->tcb, 0);
 } else {
	 conn_closed(cnx);
 }
 if (!sts) for (;;) {
	long tmo = cnx->tq.vtbl->next_timer_mS(cnx->tq.tqi);
#ifdef USE_WIN_EVENTS
	int ev;
#endif
	int i;
	if (TiNGtrace > 4 && tmo > 4000) {
		olog("ASSP long timeout %d\n", tmo);
	}
	if (conn_is_stopping(cnx)) {
		sts = !conn_is_idle(cnx);
		if (tmo != -1) {
			if (TiNGtrace > 4) {
				olog("%p: assp_thread tmo=%d\n", cnx, tmo);
			}
			sts = 1;
		}
		if (!sts) break;
	}
#ifdef USE_POLL
	i = gcctsp_poll(fds, arlen(fds), tmo);
	if (i < 0) {
		if (errno == EINTR) i = 0;
		else {
			oerrno("poll() failed");
			sts = 1;
			break;
		}
	}
#endif
#ifdef USE_WIN_EVENTS
	ev = WaitForMultipleObjects(4, events, 0, tmo);
	if (ev == WAIT_FAILED) {
		oerrno("WaitForMultipleObjects() failed");
		sts = 1;
		break;
	}
	i = ev != WAIT_TIMEOUT;
#endif
	if (i) {
#ifdef USE_POLL
		i = fds[ASSP_EV].revents;
#endif
#ifdef USE_WIN_EVENTS
		i = (ev == (WAIT_OBJECT_0 + ASSP_EV));
#endif
		if (i) {
			sts = handle_frame(cnx);
			if (sts) break;
		}
#ifdef USE_POLL
		i = fds[TQ_EV].revents;
#endif
#ifdef USE_WIN_EVENTS
		i = (ev == (WAIT_OBJECT_0 + TQ_EV));
#endif
		if (i) evnt_clear(cnx->tq_event);
#ifdef USE_POLL
		i = fds[ASSPMON_EV].revents;
#endif
#ifdef USE_WIN_EVENTS
		i = (ev == (WAIT_OBJECT_0 + ASSPMON_EV));
#endif
		if (i && handle_asspmon(cnx, asspmon_sock)) {
#ifdef USE_POLL
			fds[ASSPMON_EV].fd = -1;
#endif
			closesocket(asspmon_sock);
			asspmon_sock = -1;
		}
#ifdef USE_WIN_EVENTS
		i = (ev == (WAIT_OBJECT_0 + COMMAND_EV));
#endif
#ifdef USE_POLL
		i = fds[COMMAND_EV].revents;
#endif
		if (i) {
			int stop;
			mutx_enter(&cnx->tx_mx);
			evnt_clear(cnx->command_event);
			stop = cnx->stop;
			mutx_leave(&cnx->tx_mx);
			if ((cnx->protostate == AST_RUN) || (cnx->protostate == AST_PERTURBED)) {
				if (!stop) {
					assp_txready(&cnx->tcb);
				} else if (!aborted_tcb) {
					stop_tcb(&cnx->tcb);
					aborted_tcb = 1;
				}
			}
		}
	}
 }
 if (!conn_is_stopping(cnx) && TiNGtrace > 1) olog("%p: assp_thread stopping", cnx);
 if (sts) {
 	if ((cnx->protostate == AST_RUN) || (cnx->protostate == AST_PERTURBED)) {
		assp_abort(&cnx->tcb, 0, "thread exiting");
		conn_stopped(cnx);
	}
 }
 asspmon_close(cnx, asspmon_sock);
#ifdef USE_WIN_EVENTS
 if (events[ASSP_EV] != NULL) CloseHandle(events[ASSP_EV]);
 if (events[ASSPMON_EV] != NULL) CloseHandle(events[ASSPMON_EV]);
#endif
 return (void *) (intptr_t) sts;
#undef ASSP_EV
#undef TQ_EV
#undef ASSPMON_EV
#undef COMMAND_EV
}

LOCALDEF struct assp_conn *new_conn(void)
{
 struct assp_conn *p = (struct assp_conn*)alloc(0, sizeof(struct assp_conn));
 memset(p, 0, sizeof(struct assp_conn));
 return p;
}

LOCALDEF int assp_conn_ctor(ASSP_CONN *cnx, char *remaddr, char *asspmonaddr, int asspmonport, int port, char *key)
{
 int e;
 struct sockaddr_storage ss;
 socklen_t sl;
 SOCKET sock = create_connected_socket(remaddr, 1, port, 0, 1, &ss, &sl);
 if (sock == -1) {
	if (TiNGtrace > 0) olog("Cannot init assp socket\n");
	return 1;
 }
 cnx->pathimpl.sock = sock;
 strcpy(cnx->pathimpl.rem, remaddr);
 cnx->pathimpl.asspport = port;
 strcpy(cnx->pathimpl.asspmonaddr, asspmonaddr);
 cnx->pathimpl.asspmonport = asspmonport;
 if (ss.ss_family == AF_INET) {
	pathipv4_init(&cnx->path, &cnx->pathimpl, key);
 } else {
	pathipv6_init(&cnx->path, &cnx->pathimpl, key);
 }
 assp_tcb_init(&cnx->tcb, cnx, &cnx->tq, &cnx->path, &assp_conn_vtbl);
 cnx->txq_last = cnx->txq = 0;
 cnx->crypt = 0;
 cnx->rcv_fail_count = 0;
 cnx->max_recv_fail_count = 0;
 mutx_init(&cnx->rx_mx);
 mutx_init(&cnx->tx_mx);
 cnx->rxq = 0;
 cnx->rxq_endptr = &cnx->rxq;
 evnt(&cnx->evnt);
 e = evnt_create(&cnx->evnt);
 if (e) {
	if (TiNGtrace > 0) olog("Cannot init assp conn event\n");
	closesocket(sock);
 	return 1;
 }
 if (evnt_create(&cnx->tq_event)) {
	if (TiNGtrace > 0) olog("Cannot init timequeue event\n");
	evnt_destroy(&cnx->evnt);
	mutx_dtor(&cnx->rx_mx);
	mutx_dtor(&cnx->tx_mx);
	closesocket(sock);
 	return 1;
 }
 if (evnt_create(&cnx->command_event)) {
	if (TiNGtrace > 0) olog("Cannot init assp thread command event\n");
	evnt_destroy(&cnx->tq_event);
	evnt_destroy(&cnx->evnt);
	mutx_dtor(&cnx->rx_mx);
	mutx_dtor(&cnx->tx_mx);
	closesocket(sock);
 	return 1;
 }
 timequeue_pq_init(&cnx->tq, &cnx->tqi, cnx->tq_event);
 
 e = pthread_create(&cnx->tid, 0, assp_thread, cnx);
 if (e) {
 	if (TiNGtrace > 0) olog("pthread_create() failed: %d\n", e);
	evnt_destroy(&cnx->command_event);
	evnt_destroy(&cnx->tq_event);
	evnt_destroy(&cnx->evnt);
	mutx_dtor(&cnx->rx_mx);
	mutx_dtor(&cnx->tx_mx);
	closesocket(sock);
 	return 1;
 }
 return 0;
}

LOCALDEF void assp_conn_start(ASSP_CONN *cnx)
{
 mutx_enter(&cnx->tx_mx);
 cnx->protostate = AST_RUN;
 signal_event(evnt_to_sig(cnx->command_event));
 mutx_leave(&cnx->tx_mx);
}

LOCALDEF void assp_conn_shutdown(ASSP_CONN *cnx)
{
 mutx_enter(&cnx->rx_mx);
 if (cnx->status < kAsspConnDisconnected) {
	cnx->status = kAsspConnStopping;
 } else if (cnx->status == kAsspConnDisconnected) {
	cnx->status = kAsspConnFinished;
	signal_event(evnt_to_sig(cnx->evnt));
 }
 mutx_enter(&cnx->tx_mx);
 cnx->stop = 1;
 signal_event(evnt_to_sig(cnx->command_event));
 mutx_leave(&cnx->tx_mx);
 mutx_leave(&cnx->rx_mx);
}

LOCALDEF void assp_conn_dtor(ASSP_CONN *cnx)
{
 void *sts;
 int e;
 e = pthread_join(cnx->tid, &sts);
 // FIXME free any packets in either queue?
 cnx->tq.vtbl->dtor(cnx->tq.tqi);
 cnx->path.vtbl->path_dtor(cnx->path.pi);
 evnt_destroy(&cnx->command_event);
 evnt_destroy(&cnx->tq_event);
 evnt_destroy(&cnx->evnt);
 mutx_dtor(&cnx->rx_mx);
 mutx_dtor(&cnx->tx_mx);
 closesocket(cnx->pathimpl.sock);
 switch (cnx->protostate) {
 case AST_CLOSED:
	break;
 case AST_STOPPED:
	die("ASSP_CONN stopped in dtor");
	break;
 case AST_RUN:
	die("ASSP_CONN running in dtor");
	break;
 case AST_PERTURBED:
	die("ASSP_CONN perturbed in dtor");
	break;
 }
 free(cnx);
}

STATIC int mergepacket(PACKET *fp, PACKET *np)
{
 unsigned flen = fp->dataend - fp->datastart;
 unsigned nlen = np->dataend - np->datastart;
 unsigned fspc = fp->rawend - fp->dataend;
 if (flen + nlen > 1024) return 0;	// would be too big
 if (fspc < nlen) return 0;	// no space
	// can be merged
 memcpy(fp->dataend, np->datastart, nlen);
 fp->dataend += nlen;
 if (np->u.tx.changeover) fp->u.tx.changeover = 1;
 return 1;
} 

STATIC void unsafe_assp_conn_send(ASSP_CONN *cnx, PACKET *pkt)
{
 PACKET* txl;
 if ((cnx->protostate != AST_RUN) && (cnx->protostate != AST_PERTURBED)) {
	if (TiNGtrace > 4) olog("assp_send: discarding tx as not running");
	cnx->path.vtbl->rxfree(cnx->path.pi, pkt);
	return;
 }
 if (pkt->datastart == pkt->dataend) die("assp_send: empty TX packet");
 if (TiNGtrace & 8) dump_assp_pkt(cnx, pkt, "Tx");
 pkt->next = 0;
 txl = cnx->txq_last;

 if (txl) {
	if (!txl->u.tx.mergeinhibit && (pkt->u.tx.mergeinhibit < 2) && mergepacket(txl, pkt)) {
		cnx->path.vtbl->rxfree(cnx->path.pi, pkt);
		return;
	} else {
		txl->next = pkt;
	}
 } else {
	cnx->txq = pkt;
 }
 cnx->txq_last = pkt;
 if (cnx->txq == pkt) {
	signal_event(evnt_to_sig(cnx->command_event));
 }
}

LOCALDEF void assp_conn_send(ASSP_CONN *cnx, PACKET *pkt)
{
 mutx_enter(&cnx->tx_mx);
 unsafe_assp_conn_send(cnx,pkt);
 mutx_leave(&cnx->tx_mx);
}


LOCALDEF PACKET *assp_conn_recv(ASSP_CONN *cnx)
{
 PACKET *pkt;
 mutx_enter(&cnx->rx_mx);
 pkt = cnx->rxq;
 if (pkt) {
 	cnx->rxq = pkt->next;
	if (!cnx->rxq) cnx->rxq_endptr = &cnx->rxq;
	pkt->next = 0;
 }
 if (!pkt && !cnx->rxq && cnx->read_status == cnx->status) {
	evnt_clear(cnx->evnt);
 }
 mutx_leave(&cnx->rx_mx);
 if (pkt && cnx->crypt)
 {
	 // we know module will pad messages to 8-div lengths with MSG_PAD messages 
	int len = pkt->dataend-pkt->datastart;
	if (cnx->crypt->level & (1<<1)) {
		// level indicates record data is plaintext, so check if DATA
  	    uint8_t *dp;
		uint32_t msg;
		uint32_t type;
		(cnx->crypt->encdec)(kSMSecCryptDecode,8,cnx->crypt->context,pkt->datastart);
		len -= 8;
	    dp = pkt->datastart;
		msg  = getmsg(&dp);
		type = getmsg(&dp);
		if (len && ((msg & 0x80000000) || (msg & 0x7fff0000) || ((type>>16) != 0))) {
			// not DATA message so decrypt rest	
			(cnx->crypt->encdec)(kSMSecCryptDecode,len,cnx->crypt->context,pkt->datastart+8);
		}
	} else {
		(cnx->crypt->encdec)(kSMSecCryptDecode,pkt->dataend-pkt->datastart,cnx->crypt->context,pkt->datastart);
	}
 }
 return pkt;
}

LOCALDEF enum assp_conn_status assp_conn_status(ASSP_CONN *cnx)
{
 enum assp_conn_status s;
 mutx_enter(&cnx->rx_mx);
 s = cnx->status;
 cnx->read_status = s;
 if (!cnx->rxq) {
	evnt_clear(cnx->evnt);
 }
 mutx_leave(&cnx->rx_mx);
 return s; 
}

LOCALDEF PACKET *assp_conn_new_packet(ASSP_CONN *cnx, unsigned int size)
{
	PACKET *pkt = assp_txpacket(&cnx->tcb, size);

	if (pkt) {
		pkt->u.tx.changeover = 0;
		pkt->u.tx.payloadplaintext = 0;
		pkt->u.tx.mergeinhibit = 0;
	}

	return pkt;
}

LOCALDEF void assp_conn_free_packet(ASSP_CONN *cnx, PACKET *pkt)
{
 cnx->path.vtbl->rxfree(cnx->path.pi, pkt);
}

LOCALDEF EVNT *assp_conn_get_event(struct assp_conn *cnx)
{
 return &cnx->evnt;
}
