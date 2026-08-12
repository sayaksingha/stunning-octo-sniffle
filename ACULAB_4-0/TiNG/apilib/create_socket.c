#include "create_socket.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

#ifdef TiNGTYPE_LINUX
#define TiNG_POSIX_SOCKETS
#endif

#ifdef TiNGTYPE_QNX
#define TiNG_POSIX_SOCKETS
#endif


#ifdef TiNG_POSIX_SOCKETS
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#ifndef closesocket
#define closesocket(fd) close(fd)
#endif
#include "POSIX/socket.h"
#endif


#ifdef TiNGTYPE_WINNT
#include "../libutil/WINNT/wind.h"
#include <ws2tcpip.h>
typedef int socklen_t;

#ifndef IPV6_V6ONLY
// if this is missing we are probably using an old SDK, so define the value from the v7.1 SDK
#define IPV6_V6ONLY           27 // Treat wildcard bind as AF_INET6-only.
#endif

#endif

#include "trace.h"

STATIC int bind_socket(SOCKET sock, int family)
{
	if (family == AF_INET) {
		struct sockaddr_in sa;
		memset(&sa, 0, sizeof(sa));
		sa.sin_family = AF_INET;
		sa.sin_addr.s_addr = htonl(INADDR_ANY);
		sa.sin_port = 0;
		if (bind(sock, (struct sockaddr *) &sa, sizeof(sa)) && TiNGtrace) {
			oerrno("bind() IPv4 failed");
			return -1;
		}
	} else if (family == AF_INET6) {
		struct sockaddr_in6 sa;
		memset(&sa, 0, sizeof(sa));
		sa.sin6_family = AF_INET6;
		memcpy(&sa.sin6_addr.s6_addr, &in6addr_any, sizeof(in6addr_any));
		sa.sin6_port = 0;
		if (bind(sock, (struct sockaddr *) &sa, sizeof(sa)) && TiNGtrace) {
			oerrno("bind() IPv6 failed");
			return -1;
		}
	} else {
		if (TiNGtrace) {
			olog("can't bind unknown family %d\n", family);
		}
		return -1;
	}
	return 0;
}

SOCKET create_connected_socket(const char* name, int name_is_number, unsigned int port, int tcp, int is_assp, struct sockaddr_storage *pss, socklen_t *psl)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int s;
	char portstr[8];
	SOCKET sock = -1;
	int flg;

	sprintf(portstr, "%u", port);

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;	/* Allow IPv4 or IPv6 */
	if (tcp) {
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
	} else {
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_protocol = IPPROTO_UDP;
	}

#ifdef AI_NUMERICSERV
	hints.ai_flags = AI_NUMERICSERV;
#else
	hints.ai_flags = 0;
#endif

	if (name_is_number) hints.ai_flags |= AI_NUMERICHOST;

	s = getaddrinfo(name, portstr, &hints, &result);
	if (s != 0) {
		if (TiNGtrace > 3) olog("create_connected_socket: getaddrinfo(%s, %s..) : %s\n", name, portstr, gai_strerror(s));
		return -1;
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sock == -1) {
			osockerr("socket() failed");
			continue;
		}
#ifdef USE_CLOSE_ON_EXEC
		/*
		* Since Prosody channels use structures in memory, if an
		* exec() is done the Prosody file descriptors become
		* useless, so we arrange for them to be closed.
		*/
		flg = fcntl(sock, F_GETFD, 0);
		if (flg < 0) {
			oerrno("fcntl(F_GETFD) on socket failed");
			freeaddrinfo(result);
			return 1;
		}
		flg = fcntl(sock, F_SETFD, flg | FD_CLOEXEC);
		if (flg == -1) {
			oerrno("fcntl(F_SETFD) on socket failed");
			freeaddrinfo(result);
			return 1;
		}
#endif
#ifdef TiNGTYPE_WINNT
		flg = 1;
		if(setsockopt(sock, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (const char *)&flg, sizeof(flg)))
		{
			oerrno("setsockopt() SO_EXCLUSIVEADDRUSE failed");
			if (TiNGtrace > 0) olog("Cannot enable SO_EXCLUSIVEADDRUSE option\n", 0);
			closesocket(sock);
			freeaddrinfo(result);
			return 1;
		}
#endif
#ifdef IPV6_V6ONLY
		if (rp->ai_family == AF_INET6) {
			int v6only = IN6_IS_ADDR_V4MAPPED(&((struct sockaddr_in6*)rp->ai_addr)->sin6_addr) ? 0 : 1;
			setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&v6only, sizeof(v6only));
		}
#endif
		if (tcp || bind_socket(sock, rp->ai_family) == 0) {
			if (connect(sock, rp->ai_addr, rp->ai_addrlen) != -1) {
#ifdef LINUX
				if (!tcp && is_assp) {
					dummysock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
					if (dummysock == -1) {
 						osockerr("socket() failed");
						if (TiNGtrace > 0) olog("Ignoring failure to create dummy socket\n");
					} else if (bind_socket(dummysock, rp->ai_family) != 0) {
						if (TiNGtrace > 0) olog("Ignoring failure to bind dummy socket to port %d\n", sa.sin_port);
						closesocket(dummysock);
					}
				}
#else
				(void)is_assp;
#endif
				if (pss) memcpy(pss, rp->ai_addr, rp->ai_addrlen);
				if (psl) *psl = rp->ai_addrlen;
				break;		/* Success */
			}
			if (TiNGtrace > 3) {
				osockerr("connect() failed");
			}
		}
		closesocket(sock);
	}
	freeaddrinfo(result);
	if (rp == NULL) {				/* No address succeeded */
		if (TiNGtrace > 1) olog("Could not connect to %s port %s\n", name, portstr);
		return -1;
	}
	return sock;
}
