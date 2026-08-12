/*rtpclerk_cli.c : rtpclerk client library*/
#include "rtpclerk_cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>

#ifdef TiNGTYPE_LINUX
#define TiNG_POSIX_SOCKETS
#endif

#ifdef TiNGTYPE_QNX
#define TiNG_POSIX_SOCKETS
#endif


#ifdef TiNG_POSIX_SOCKETS
	// for sleep()
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket close
#endif

#ifdef TiNGTYPE_WINNT
#include "../../libutil/WINNT/wind.h"
#include <ws2tcpip.h>
#define sleep(x) Sleep((x)*1000)
#endif

#include "pthread.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

static int getSockErr(void)
{
#ifdef TiNG_POSIX_SOCKETS
 perror("socket error:");
 return errno;
#endif
#ifdef TiNGTYPE_WINNT
 return WSAGetLastError();
#endif
}

static int shutSocket(RTPCLERK_SOCKET sock)
{
 return closesocket(sock);
}

#ifdef TiNGTYPE_WINNT
static int WSAInit()
{
 WSADATA wsaData;
 WORD wVersionRequested = MAKEWORD( 2, 2 );
 int err = WSAStartup( wVersionRequested, &wsaData );
 if (err) fprintf(stderr, "WSAStartup failed: [%d]\n",err);
 return err;
}
#endif

RTPCLERK_SOCKET rtpclerk_cli_connect(char *ip_addr, unsigned short port)
{
 struct sockaddr_in sockAddr;
 struct sockaddr_in6 sockAddr6;
 struct sockaddr *addr;
 int err;
#ifdef TiNGTYPE_LINUX
 socklen_t iLen;
#endif
#ifdef TiNGTYPE_QNX
 socklen_t iLen;
#endif
#ifdef TiNGTYPE_WINNT
 int iLen;
#endif
 RTPCLERK_SOCKET sock;
 if (strchr(ip_addr, ':') != NULL) {
	err = inet_pton(AF_INET6, ip_addr, &sockAddr6.sin6_addr);
	sockAddr6.sin6_family = PF_INET6;
	addr = (struct sockaddr*)&sockAddr6;
	sockAddr6.sin6_port = htons(port);
	iLen = sizeof(sockAddr6);
 } else {
	err = inet_pton(AF_INET, ip_addr, &sockAddr.sin_addr);
	sockAddr.sin_family = PF_INET;
	addr = (struct sockaddr*)&sockAddr;
	sockAddr.sin_port = htons(port);
	iLen = sizeof(sockAddr);
 }
 if (err <= 0) {
	return err ? err : -1;
 }
 sock = socket(addr->sa_family, SOCK_STREAM, 0); 
 if (sock == INVALID_SOCKET) {
	err = getSockErr();
	return err;
 }
 err = connect(sock, addr, iLen);
 if (err == SOCKET_ERROR) {
	err = getSockErr();
	shutSocket(sock);
	return -1;
 }
 return sock;
}

int rtpclerk_cli_post_port(RTPCLERK_SOCKET sock, char *myip, unsigned short port)
{
 int err, len;
 char *msg = malloc(256);
 if (!msg) {
 	fprintf(stderr, "malloc() failed for msg\n");
	return -1;
 }
 err = sprintf(msg, "+%s\t%hu\n", myip, port);
 if (err < 0) {
	fprintf(stderr, "unable to construct port msg\n");
	return -1;
 }
 len = strlen(msg);
 for(;;) {
	err = send(sock, msg, len, 0);
	if (err < 0) {
		err = getSockErr();
		return err;
	} else if (err > len) {
		fprintf(stderr, "sent more octects than were available\n");
		return -1;
	} else if (err < len) len -= err;	
	else if (err == len) break;
 }
 //FIXME: could wait for the response here ... 
 return 0;
}

int rtpclerk_cli_request_port(RTPCLERK_SOCKET sock, char **msgp)
{
 int err, nc, max_len = 256;
 char *m;
 *msgp = malloc(max_len);
 if (!(*msgp)) {
 	fprintf(stderr, "malloc() failed. unable to recv message\n");
	return -1;
 }
 m = *msgp;
 err = sprintf(m, "Aallocate\n");
 if (err < 0) {
	fprintf(stderr, "unable to construct msg for port request\n");
	return -1;
 }
 nc = strlen(m);
 for (;;) {
	err = send(sock, m, nc, 0);
	if (err < 0) {
		err = getSockErr();
		return err;
	} else if (err > nc) {
		fprintf(stderr, "sent more octects than were available\n");
		return -1;
	} else if (err < nc) nc -= err;	
	else if (err == nc) break;

 }
 memset(m, 0, max_len);
 for (;;) { 
	nc = recv(sock, m, max_len, 0);
	if (nc == -1) {
		err = getSockErr();
		return err;
	} else if (nc >= max_len) {
		fprintf(stderr, "Unable to recv port: exceeded max msg len\n");
		return -1;
	} else if (m[nc-1] == '\n') {
		m[nc] = 0; //null term. the string
		break; 
	}
 }
 return 0;
}

int rtpclerk_cli_msg2port(char *msg, char *ipaddr, short *port)
{
 char *tmp, *cp = msg;
 int i, j, max_len;
 if (cp[0] != '+') {
 	fprintf(stderr, "invalid command: %c\n", *cp);
	return -1;
 }
 max_len = 256;
 for (i=1,j=0;;i++,j++) {
	if (i >= max_len) {
		fprintf(stderr, "insufficient space for ip address\n");
		return -1;
	} else if (!cp[i] || cp[i] == '\n') {
		fprintf(stderr, "invalid msg\n");		
		return -1;
	} else if (cp[i] == '\t') break;
	ipaddr[j] = cp[i];	
 }
 ipaddr[j] = 0; 
 tmp = malloc(256);
 if (!tmp) {
 	fprintf(stderr, "unable to malloc temp. string\n");
	return -1;
 }
 	//we're sitting with 'i' on a ':' at the moment so move it
 i++;
 for (j=0;;i++,j++) {
	if (i >= max_len) {
		fprintf(stderr, "insufficient space for ip address\n");
		return -1;
	} else if (!cp[i]) {
		fprintf(stderr, "invalid msg\n");		
		return -1;
	} else if (cp[i] == '\n') break;
	tmp[j] = cp[i];
 }
 tmp[j] = 0;
 *port = atoi(tmp);
 free(tmp);
 return 0;
}
