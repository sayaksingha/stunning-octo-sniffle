/* aculog.c - ACULAB standard logging */

#ifdef TiNGTYPE_LINUX
#define TiNG_POSIX_SOCKETS
#endif

#ifdef TiNGTYPE_QNX
#define TiNG_POSIX_SOCKETS
#endif


#ifdef TiNGTYPE_WINNT
#include <io.h>
#include "WINNT/wind.h"
#include <ws2tcpip.h>
#include <stdio.h>
#include <time.h>
#endif

#ifdef TiNG_POSIX_SOCKETS
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
typedef int SOCKET;
#define closesocket close
#include <string.h>
#endif

#include <stdlib.h>
#include <sys/timeb.h>

#include "aculog.h"
#include "vseprintf.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

static int aculog_fd = -1;
static SOCKET aculog_sock = -1;
static struct tm prevtim;
static time_t prevsec;
static unsigned short prevmilli;
static char aculog_buf[16384];		// static only to save stack space

static char *aculog_tag;

int aculog_init_fd(int fd, char *tag)
{
 struct timeb t;
 aculog_fd = fd;
 aculog_tag = tag;
 ftime(&t);
 prevsec = t.time;
 prevmilli = t.millitm;
 prevtim = *gmtime(&prevsec);
 prevtim.tm_year += 1900;
 {
  char *ep = seprintf(aculog_buf, aculog_buf+arlen(aculog_buf),
	"%04d-%02d-%02d_%02d:%02d:%02d.%03d %s: Starting logging\n",
	prevtim.tm_year, prevtim.tm_mon+1, prevtim.tm_mday,
	prevtim.tm_hour, prevtim.tm_min, prevtim.tm_sec,
	prevmilli, tag);
#ifdef TiNGTYPE_WINNT
  if (aculog_sock != -1) send(aculog_sock, aculog_buf, ep - aculog_buf, 0);
  else _write(aculog_fd, aculog_buf, ep - aculog_buf);
#else
  write(aculog_fd, aculog_buf, ep - aculog_buf);
#endif
 }
 return 0;
}

static SOCKET create_connected_socket(const char* name, const char* port)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int s;
	SOCKET sock = -1;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;	/* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

#ifdef AI_NUMERICSERV
	hints.ai_flags = AI_NUMERICSERV;
#else
	hints.ai_flags = 0;
#endif

	s = getaddrinfo(name, port, &hints, &result);
	if (s != 0) {
		fprintf(stderr, "create_connected_socket: getaddrinfo(%s, %s..) : %s\n", name, port, gai_strerror(s));
		return -1;
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sock == -1) {
			continue;
		}
		if (connect(sock, rp->ai_addr, rp->ai_addrlen) != -1) {
			break;		/* Success */
		}
		closesocket(sock);
	}
	freeaddrinfo(result);
	if (rp == NULL) {				/* No address succeeded */
		fprintf(stderr, "Could not connect to %s port %s\n", name, port);
		return -1;
	}
	return sock;
}

int aculog_init(char *addr, char *port, char *tag)
{
 SOCKET sock;
#ifdef WIN32
 WSADATA ver;
 int i;
 i = WSAStartup(MAKEWORD(2, 0), &ver);
 if (i) {
	return i;
 }
#endif
 sock = create_connected_socket(addr, port);
 if (sock == -1) {
 	return -1;
 }
 aculog_sock = sock;
 return aculog_init_fd((int)sock, tag);
}

static void timeadj(unsigned long d, unsigned short dm)
{
 static unsigned daylim[13] = {0,
	31, 28, 31, 30, 31, 30,
	31, 31, 30, 31, 30, 31, };
 prevmilli += dm;
 if (prevmilli >= 1000) d++, prevmilli -= 1000;
 if (!d) return;
 d += prevtim.tm_sec;
 prevtim.tm_sec = d % 60;
 d /= 60;
 if (!d) return;
 d += prevtim.tm_min;
 prevtim.tm_min = d % 60;
 d /= 60;
 if (!d) return;
 d += prevtim.tm_hour;
 prevtim.tm_hour = d % 24;
 d /= 24;
 if (!d) return;
 d += prevtim.tm_mday;
 for (;;) {
	unsigned dlim = daylim[prevtim.tm_mon];
	if (prevtim.tm_mon == 2) {
		unsigned ym = prevtim.tm_year % 400;
		if (! (ym % 4)
			&& ym != 100 && ym != 200 && ym != 300) dlim = 29;
	}
	if (d <= dlim) break;
	d -= dlim;
	if (++prevtim.tm_mon == 12) prevtim.tm_mon=0, prevtim.tm_year++;
 }
 prevtim.tm_mday = d;
 return;
}

void aculog(char *fmt, ...)
{
 unsigned long d;
 unsigned short dm;
 char *ep;
 va_list ap;
 struct timeb t;
 if (aculog_fd == -1) return;
 ftime(&t);
 d = t.time - prevsec;
 if (t.millitm < prevmilli) {
 	d--;
 	dm = 1000 + t.millitm - prevmilli;
 } else dm = t.millitm - prevmilli;
 prevsec = t.time;
 timeadj(d, dm);
 ep = seprintf(aculog_buf, aculog_buf+arlen(aculog_buf),
	"%04d-%02d-%02d_%02d:%02d:%02d.%03d ",
	prevtim.tm_year, prevtim.tm_mon+1, prevtim.tm_mday,
	prevtim.tm_hour, prevtim.tm_min, prevtim.tm_sec,
	t.millitm);
 ep = seprintf(ep, aculog_buf+arlen(aculog_buf), "%s: +%d.%03d ", aculog_tag, d, dm);
 va_start(ap, fmt);
 ep = vseprintf(ep, aculog_buf+arlen(aculog_buf), fmt, ap);
 va_end(ap);
 if (ep < aculog_buf+arlen(aculog_buf)) *ep++ = '\n', *ep = 0;
#ifdef TiNGTYPE_WINNT
 if (aculog_sock) send(aculog_sock, aculog_buf, ep - aculog_buf, 0);
 else _write(aculog_fd, aculog_buf, ep - aculog_buf);
#else
 write(aculog_fd, aculog_buf, ep - aculog_buf);
#endif
}

void aculog_close(void)
{
 if (aculog_sock == -1) return;
#ifdef WIN32
 closesocket(aculog_sock);
 WSACleanup();
#else
 close(aculog_sock);
#endif
}
