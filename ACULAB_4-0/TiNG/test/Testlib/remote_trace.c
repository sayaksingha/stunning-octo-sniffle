/* remote_trace.c - allow remote tracing of TiNG API calls */

#include "remote_trace.h"

#ifdef TiNGTYPE_LINUX
#define TiNG_POSIX_SOCKETS
#endif

#ifdef TiNGTYPE_PXPPC
#define TiNG_POSIX_SOCKETS
#endif

#ifdef TiNGTYPE_QNX
#define TiNG_POSIX_SOCKETS
#endif


#ifdef TiNG_POSIX_SOCKETS
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#define closesocket(x) close(x)
#endif

#ifdef TiNGTYPE_LINUX
#include <errno.h>
#include <asm/types.h>
#include <linux/errqueue.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#define closesocket(x) close(x)
#endif

#ifdef TiNGTYPE_PXPPC
#include <errno.h>
#include <asm/types.h>
#include <linux/errqueue.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#define closesocket(x) close(x)
#endif

#ifdef TiNGTYPE_WINNT
#include <winsock2.h>
#include "../../libutil/WINNT/lasterr.h"
#define reperr(x) formaterr(x, WSAGetLastError())
#if _MSC_VER < 1500
#define vsnprintf _vsnprintf
#endif
#endif

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "smdrvr.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

#ifndef reperr
#define reperr(x) perror(x)
#endif

#ifdef TiNGTYPE_WINNT
#define sleep(x) Sleep((x)*1000)
#endif

static int prepinsock(char *port)
{
 int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
 if (sock == -1) {
 	// report error
 	reperr("socket() failed");
 	return -1;
 }
 if (port) {
	struct sockaddr_in sa;
	struct servent *sp;
	char *ep;
	long pl;
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr= htonl(INADDR_ANY);
	pl = strtol(port, &ep, 0);
	if (!*ep && pl >= 1024 && pl <= 65535)
		sa.sin_port = htons((unsigned short) pl);
	else {
		sp = getservbyname(port, "tcp");
		if (sp) {
			sa.sin_port = sp->s_port;
		} else {
			fprintf(stderr, "Cannot interpret port: %s\n", port);
			return -1;
		}
	}
	if (bind(sock, (struct sockaddr *) &sa, sizeof(sa))) {
		reperr("bind() failed");
		closesocket(sock);
		return -1;
	}
 }
 return sock;
}

static int trace_socket;

static int olog_socket(const char *fmt, va_list ap)
{
 char buff[1024];
 unsigned len;
 len = vsnprintf(buff, sizeof(buff), fmt, ap);	// watch out for len == -1
 if (len > sizeof(buff)) {	// very unlikely, but handle correctly anyway
	unsigned rqlen = sizeof(buff);
	for (;;) {
		char *b2;
		rqlen *= 2;
		b2 = malloc(rqlen);
		if (!b2) return 1;	// give up
		len = vsnprintf(b2, rqlen, fmt, ap);
		if (len <= rqlen) {
			send(trace_socket, b2, len, 0);
			free(b2);
			return 0;
		}
		free(b2);
	}

 }
 send(trace_socket, buff, len, 0);
 return 0;
}

static int olog(const char *fmt, ...)
{
 va_list ap;
 int r;
 va_start(ap, fmt);
 r = olog_socket(fmt, ap);
 va_end(ap);
 return r;
}

static int remote_trace_listen(int sock)
{
 if (listen(sock, 4) < 0) {
	reperr("listen() failed");
	return 1;
 }
 for (;;) {
	int (*oldtrace)(const char *fmt, va_list ap);
	int tsock = accept(sock, 0, 0);
	unsigned v = 0;
	int sign = 0;
	if (tsock < 0) {
		reperr("accept() failed ");
		return 1;
	}
	trace_socket = tsock;
	oldtrace = TiNG_showtrace;
	TiNG_showtrace = olog_socket;
	olog("[TiNGtrace]");
	for (;;) {
		char c;
		int nc = recv(tsock, &c, 1, 0);
		if (!nc) break;	// EOF - revert to old trace
		if (nc < 0) {
			perror("read(trace socket) failed");
			break;
		}
		if (c >= '0' && c <= '9') {
			v *= 10;
			v -= c - '0';
			if (!sign) sign = '+';
		} else switch (c) {
		case '\r':
		case ' ':
			break;
		case '\n':
			if (sign != '?') {
				if (sign != '-') v = -v;
				TiNGtrace = v;
				olog("[trace command: TiNGtrace = %d]", v);
			}
				// ready to read another number
			sign = 0;
			v = 0;
			break;
		case '+':
		case '-':
			sign = sign ? '?' : c;
			break;
		default:
			olog("[trace command: bogus char in number: %c]", c);
			sign = '?';
		}
	}
	TiNG_showtrace = oldtrace;
	closesocket(tsock);
 }
}

int remote_trace(char *port)
{
 int sock = prepinsock(port);
 if (sock != -1) remote_trace_listen(sock);
 return 0;
}
