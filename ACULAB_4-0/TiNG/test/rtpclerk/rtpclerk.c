/* rtpclerk */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>
#include "pthread.h"

#ifdef TiNGTYPE_LINUX
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#define POSIX_SOCKETS
#endif

#ifdef TiNGTYPE_QNX
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#define POSIX_SOCKETS
#endif


#ifdef TiNGTYPE_WINNT
#define sleep(x) Sleep((x)*1000)
#include "../../libutil/WINNT/wind.h"
#include <ws2tcpip.h>
#endif

#ifdef POSIX_SOCKETS
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
typedef int SOCKET;
#endif

#define arlen(x) (sizeof(x)/sizeof(*(x)))

#define CLERK_CLIENT_TYPE_NONE 0
#define CLERK_CLIENT_TYPE_RX 1
#define CLERK_CLIENT_TYPE_TX 2


typedef struct request {
	struct request *next;
	struct request *prev;
	char *msg;
	unsigned msg_ofs;
	unsigned msg_len;
	unsigned msg_max_len;
} REQUEST;

typedef struct client_info {
 struct client_info *next;
 struct client_info *prev;
 SOCKET cli_sock;
 unsigned type;
 char* ip;
 unsigned short port;
 REQUEST *rx_rqueue;
 REQUEST *tx_rqueue;
 REQUEST *cur_rq;
	// ...
} CLIENT_INFO;

typedef struct {
 CLIENT_INFO *open;
 CLIENT_INFO *rx_ready;
 CLIENT_INFO *tx_wait;
 CLIENT_INFO *tx_ready;
} open_clients;

#define MAX_MSG_SIZE 2048
static int getSockErr(void)
{
#ifdef TiNGTYPE_WINNT
 return WSAGetLastError();
#endif
#ifdef POSIX_SOCKETS
 perror("socket error:");
 return errno;
#endif
}

static int shutSocket(SOCKET sock)
{
#ifdef TiNGTYPE_WINNT
 return closesocket(sock);
#endif
#ifdef POSIX_SOCKETS
 return close(sock);
#endif
}

#ifdef TiNGTYPE_WINNT
static int InitWSA()
{
 WSADATA wsaData;
 WORD wVersionRequested = MAKEWORD( 2, 2 );
 int err = WSAStartup( wVersionRequested, &wsaData );
 if (err) fprintf(stderr, "WSAStartup failed: [%d]\n",err);
 return err;
}
#endif

static void rq_dump_list(REQUEST *list, char *name)
{
 REQUEST *entry = list;
 fprintf(stdout, "%s:\n", name);
 for (;;) {
 	if (!entry) break;
	fprintf(stdout, "\t%s", entry->msg); 
	entry = entry->next;
 }
 fprintf(stdout, "End:%s\n",name);
}

static void cl_dump_list(CLIENT_INFO *list, char *name)
{
 CLIENT_INFO *entry = list;
 fprintf(stdout, "%s:\n", name);
 for (;;) {
 	if (!entry) break;
	fprintf(stdout, "\t[%s:%u]\n", entry->ip, entry->port);
	rq_dump_list(entry->rx_rqueue, "rx queue");
	rq_dump_list(entry->tx_rqueue, "tx queue");
	entry = entry->next;
 }
 fprintf(stdout, "End:%s\n",name);
}

	//add to the head of the list for now
static void cl_list_add(CLIENT_INFO *entry, CLIENT_INFO **list)
{ 
  if (*list) {
	entry->next = *list;
	(*list)->prev = entry;
 }
 *list = entry;
}

static void cl_list_ulink(CLIENT_INFO *entry, CLIENT_INFO **list)
{
 if (entry->prev) entry->prev->next = entry->next;
 if (entry->next) entry->next->prev = entry->prev;
 if (*list == entry) *list = entry->next; 
 entry->next = entry->prev = 0;
}

static void rq_list_add(REQUEST *entry, REQUEST **list)
{ 
  if (*list) {
	entry->next = *list;
	(*list)->prev = entry;
 }
 *list = entry;
}

static void rq_list_ulink(REQUEST *entry, REQUEST **list)
{
 if (entry->prev) entry->prev->next = entry->next;
 if (entry->next) entry->next->prev = entry->prev;
 if (*list == entry) *list = entry->next;
 entry->next = entry->prev = 0;
}

static int rq_ctor(REQUEST *rq)
{
 memset(rq, 0, sizeof(*rq));
 rq->msg_max_len = MAX_MSG_SIZE;
 rq->msg = malloc(rq->msg_max_len);
 if (!rq->msg) {
	fprintf(stderr, "Malloc failed - unable to allocate space for request msg\n");
	return -1;
 }
 rq->msg[0] = 0;
 return 0;
}

static int rq_dtor(REQUEST *rq)
{
 free(rq->msg);
 return 0;
}

static int client_ctor(CLIENT_INFO *client)
{
 memset(client, 0, sizeof(*client));
 client->ip = malloc(256);
 if (!client->ip) return -1;
 return 0;
}

static void client_dtor(CLIENT_INFO *client)
{
 REQUEST *rq;
 shutSocket(client->cli_sock);
 fprintf(stdout, "[%s:%u]: Disconnect Local\n", client->ip, client->port);
	//delete any outstanding items from the rqueues
 rq = client->rx_rqueue;
 for(;;) {
 	REQUEST *tmp;
 	if (!rq) break;
	tmp = rq->next;
	rq_list_ulink(rq, &client->rx_rqueue);
	rq_dtor(rq);
	free(rq);
	rq = tmp; 
 }
 rq = client->tx_rqueue;
 for(;;) {
 	REQUEST *tmp;
 	if (!rq) break;
	tmp = rq->next;
	rq_list_ulink(rq, &client->tx_rqueue);
	rq_dtor(rq);
	free(rq);
	rq = tmp; 
 }
 free(client->ip);
}

static int create_listen_sock(SOCKET *sock, char *ip_addr, unsigned short port)
{
 struct sockaddr_in sockAddr;
 struct sockaddr_in6 sockAddr6;
 struct sockaddr *addr;
 int err;
#ifdef POSIX_SOCKETS
 socklen_t iLen;
#endif
#ifdef TiNGTYPE_WINNT
 int iLen;
#endif
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
 *sock = socket(addr->sa_family, SOCK_STREAM, 0); 
 if (*sock == INVALID_SOCKET) {
	err = getSockErr();
	return err;
 }
	//bind sock
 err = bind(*sock, addr, iLen);
 if (err == SOCKET_ERROR) {
	err = getSockErr();
	shutSocket(*sock);
	return err;
 }
	//start listen
 err = listen(*sock, SOMAXCONN);
 if (err == SOCKET_ERROR) {
	err = getSockErr();
	shutSocket(*sock);
	return err;
 }
 return err;
}

static int add_client(SOCKET sock, CLIENT_INFO **list)
{
 CLIENT_INFO *new_client = 0;
 struct sockaddr_storage name;
 int err;
 char buf[256];
#ifdef POSIX_SOCKETS
 socklen_t len;
#endif
#ifdef TiNGTYPE_WINNT
 int len;
#endif
 new_client = malloc(sizeof(*new_client));
 if (!new_client) {
 	fprintf(stderr, "malloc() failed for new client\n");
	return -1;
 }
 err = client_ctor(new_client);
 if (err) {
 	fprintf(stderr, "Unable to construct new client\n");
	return err;
 }
 new_client->cli_sock = accept(sock, 0, 0);
 if (new_client->cli_sock == INVALID_SOCKET) {
 	err = getSockErr();
	return err;
 }
#ifdef POSIX_SOCKETS
 err = fcntl(new_client->cli_sock, F_SETFL, O_NONBLOCK);
 if (err) {
	perror("socket error:");
	fprintf(stderr, "unable to change socket options. Err[%d]\n", errno);
	return errno;
 }
#endif
 len = sizeof(name);
 err = getpeername(new_client->cli_sock, (struct sockaddr*)&name, &len);  
 if (err) {
 	err = getSockErr();
	return err;
 }
 if (name.ss_family == AF_INET) {
	strncpy(new_client->ip, inet_ntop(name.ss_family, &((struct sockaddr_in*)&name)->sin_addr, buf, 256),256);
	new_client->port = ntohs(((struct sockaddr_in*)&name)->sin_port);
 } else {
	strncpy(new_client->ip, inet_ntop(name.ss_family, &((struct sockaddr_in6*)&name)->sin6_addr, buf, 256),256);
	new_client->port = ntohs(((struct sockaddr_in6*)&name)->sin6_port);
 }
 	//log connection - could write a log function
 fprintf(stdout, "[%s:%u]: Connected\n", new_client->ip, new_client->port);
 cl_list_add(new_client, list);
 return 0;
}

static int send_msg(CLIENT_INFO *client)
{
 int err, len;
 REQUEST *rq;
 char *msg;
 if (!client->tx_rqueue) {
	fprintf(stderr, "Invaild state - no msg to send\n");
	return -1;
 } else rq = client->tx_rqueue;
 len = rq->msg_len - rq->msg_ofs;
 if (len <= 0) {
 	fprintf(stderr, "sending invalid message\n");
	return -1;
 }
 msg = rq->msg + rq->msg_ofs;
 err = send(client->cli_sock, msg, len, 0);
 if (err < 0) {
 	err = getSockErr();

#ifdef POSIX_SOCKETS
	if (err != EWOULDBLOCK && err != EAGAIN) {
#endif
#ifdef TiNGTYPE_WINNT
	if (err != WSAEWOULDBLOCK) {
#endif		
		fprintf(stderr, "Failed to send msg: %s\n", rq->msg);
		return err;
	} else {
		fprintf(stderr, "Write would block\n");
	}
 } else if (err < len) {
	fprintf(stderr, "Msg: %s truncated! Sent %d of %d characters\n", msg, err, len);
	rq->msg_ofs += err;
 } else {
	fprintf(stdout, "[%s:%u]: Sent %s", client->ip, client->port, msg);
	rq_list_ulink(rq, &client->tx_rqueue);
	rq_dtor(rq);
	free(rq);
 }
 return 0;
}
	//handle msg only called if we have a request with a potentially valid message 
static int handle_msg(CLIENT_INFO *client)
{
 char *c = client->rx_rqueue->msg;
 for(;;c++) {
	if (!c || *c == 0) break;
	else if (*c == '+') {
		if (client->type == CLERK_CLIENT_TYPE_TX) {
			fprintf(stderr, "[%s:%u]: Recv: Invalid command for client type TX: %s", client->ip, client->port, client->rx_rqueue->msg);
			*c = 0;
		} else {
			client->type = CLERK_CLIENT_TYPE_RX;
		}
		break;
	} else if (*c == 'A') {
		if (client->type == CLERK_CLIENT_TYPE_RX) {
			fprintf(stderr, "[%s:%u]: Recv: Invalid command for client type RX: %s", client->ip, client->port, client->rx_rqueue->msg);
			*c = 0;
		} else {
			client->type = CLERK_CLIENT_TYPE_TX;
		}
		break;
	}
 }
 if(!c || *c == 0) {
 		//FIXME: do we unlink this client?
	fprintf(stderr, "[%s:%u]: Recv: unrecognised command\n\tmsg: %s\n", client->ip, client->port, client->rx_rqueue->msg);
	return -1;
 } else {
	fprintf(stdout, "[%s:%u]: Recv %s", client->ip, client->port, client->rx_rqueue->msg);
 }
 return 0;
}

static int check_msg(REQUEST *rq, int n)
{
 int i;
 for(i=0; i<n; i++) {
	if (rq->msg[i] == '\n') return ++i; //increment to reflect length
 }
 return -1;
}

// Returns: number of bytes read or an error
static int read_msg(CLIENT_INFO *client)
{
 int nc ,len;
 char *msg;
 if (!client->cur_rq) { 
 	int err;
 	client->cur_rq = malloc(sizeof(*client->cur_rq)); 
	if (!client->cur_rq) {
		fprintf(stderr, "Malloc failed - unable to allocate space for msg\n");
		return -1;
	}
	err = rq_ctor(client->cur_rq);
	if (err) {
		fprintf(stderr, "Failed to create REQUEST object\n");
		return -1;
	}
 }
 len = (client->cur_rq->msg_max_len - 1) - client->cur_rq->msg_ofs; //max 2047
 if (len <= 0) {
 	fprintf(stderr, "received invalid message: Len %d\n",len);
	return -1;
 }
 msg = &client->cur_rq->msg[client->cur_rq->msg_ofs];
 
 nc = recv(client->cli_sock, msg, len, 0);
 if (nc < 0) {
 	int err = getSockErr();
	if (err != EAGAIN) { 
		rq_dtor(client->cur_rq);
		free(client->cur_rq);
		client->cur_rq = 0;
		return -1;
	}
 } else if (!nc) {
	fprintf(stderr, "Read 0 bytes - EOF\n");
	rq_dtor(client->cur_rq);
	free(client->cur_rq);
	client->cur_rq = 0;
	return -1;
 }
 //take offset into account when reporting length
 nc += client->cur_rq->msg_ofs;
 client->cur_rq->msg_ofs = 0;
 return nc;
}


/*
 * static int process_msg(CLIENT_INFO *client, int *len) : process a received msg
 *
 * check msg looks for a "\n" to terminate a single msg. On return from check_msg
 * sts indicates the position of a "\n" or is set to -1 to indicate an incomplete
 * message. If we find an incomplete message - i.e. we hit the end of our data
 * before we find a "\n" - set msg_ofs so that read_msg knows not to overwrite
 * the partial message. Next time process_msg is called len will be set such that
 * it encompasses the new data and any saved data
 */
static int process_msg(CLIENT_INFO *client, int *len)
{
 int sts = check_msg(client->cur_rq, *len);
 fprintf(stdout, "len was %d sts was %d\n", *len, sts);
 
 if (sts <= 0) { 
	client->cur_rq->msg_ofs += *len; //set ofs so read_msg won't overwrite the partial message
	return 1; //end of processing
 } else {  //we have at least one request
	unsigned tsize = 0;
	char *tm = 0;
	client->cur_rq->msg_len = sts;
	if (client->cur_rq->msg_len < *len) { //if we have one message + some data
		tm = malloc(client->cur_rq->msg_max_len);
		if (!tm) {
			fprintf(stderr, "Failed to malloc memory for temporary message\n");
			return -1;
		}
		tsize = *len - client->cur_rq->msg_len;
		memcpy(tm, &client->cur_rq->msg[client->cur_rq->msg_len], tsize); //preserve the remainder of the data
	}
	*len -= client->cur_rq->msg_len; //update our idea of the remainder
	client->cur_rq->msg_ofs = -1; //reset offset
	client->cur_rq->msg[client->cur_rq->msg_len] = 0; //null terminate the string
	rq_list_add(client->cur_rq, &client->rx_rqueue);
	client->cur_rq = 0;
	if (tsize) {
		int err;
		client->cur_rq = malloc(sizeof(*client->cur_rq)); 
		if (!client->cur_rq) {
			fprintf(stderr, "Malloc failed - unable to allocate space for msg\n");
			return -1;
		}
		err = rq_ctor(client->cur_rq);
		if (err) {
			fprintf(stderr, "Failed to create REQUEST object\n");
			return -1;
		}
		memcpy(client->cur_rq->msg, tm, tsize);
		free(tm);
	}
 }
 return 0;
}

static int construct_readable(SOCKET listen_sock, open_clients *lists, fd_set *rdset, SOCKET *max_sock)
{
 CLIENT_INFO *client = lists->open;
 unsigned sock_count = 0;

 FD_ZERO(rdset);
 FD_SET(listen_sock, rdset);
 if (listen_sock > *max_sock) *max_sock = listen_sock;
 sock_count++;
 for(;;) {
 	if (!client) break;
	FD_SET(client->cli_sock, rdset);
	if (client->cli_sock > *max_sock) *max_sock = client->cli_sock;
	sock_count++;
	client = client->next;
	//FIXME: Should check sock_count < FD_SETSIZE
 }
 client = lists->rx_ready;
 for(;;) {
 	if (!client) break;
	FD_SET(client->cli_sock, rdset);
	if (client->cli_sock > *max_sock) *max_sock = client->cli_sock;
	sock_count++;
	client = client->next;
	//FIXME: Should check sock_count < FD_SETSIZE
 }
 client = lists->tx_wait;
 for(;;) {
 	if (!client) break;
	FD_SET(client->cli_sock, rdset);
	if (client->cli_sock > *max_sock) *max_sock = client->cli_sock;
	sock_count++;
	client = client->next;
	//FIXME: Should check sock_count < FD_SETSIZE
 }
 return sock_count;
}

static int construct_writeable(open_clients *lists, fd_set *wrset, SOCKET *max_sock)
{
 CLIENT_INFO *client = lists->tx_ready;
 unsigned sock_count = 0;
 FD_ZERO(wrset);
 for(;;) {
 	if (!client) break;
	FD_SET(client->cli_sock, wrset);
	if (client->cli_sock > *max_sock) *max_sock = client->cli_sock;
	sock_count++;
	client = client->next;
	//FIXME: Should check sock_count < FD_SETSIZE
 }
 return sock_count;
}

static int construct_exception(SOCKET listen_sock, open_clients *lists, fd_set *except, SOCKET *max_sock)
{
 CLIENT_INFO *client = lists->open;
 unsigned sock_count = 0;

 FD_ZERO(except);
 FD_SET(listen_sock, except);
 if (listen_sock > *max_sock) *max_sock = listen_sock;
 sock_count++;
 for(;;) {
 	if (!client) break;
	FD_SET(client->cli_sock, except);
	if (client->cli_sock > *max_sock) *max_sock = client->cli_sock;
	sock_count++;
	client = client->next;
	//FIXME: Should check sock_count < FD_SETSIZE
 }
 client = lists->rx_ready;
 for(;;) {
 	if (!client) break;
	FD_SET(client->cli_sock, except);
	if (client->cli_sock > *max_sock) *max_sock = client->cli_sock;
	sock_count++;
	client = client->next;
	//FIXME: Should check sock_count < FD_SETSIZE
 }
 client = lists->tx_wait;
 for(;;) {
 	if (!client) break;
	FD_SET(client->cli_sock, except);
	if (client->cli_sock > *max_sock) *max_sock = client->cli_sock;
	sock_count++;
	client = client->next;
	//FIXME: Should check sock_count < FD_SETSIZE
 }
 client = lists->tx_ready;
 for(;;) {
 	if (!client) break;
	FD_SET(client->cli_sock, except);
	if (client->cli_sock > *max_sock) *max_sock = client->cli_sock;
	sock_count++;
	client = client->next;
	//FIXME: Should check sock_count < FD_SETSIZE
 }
 return sock_count;
}

static void service_alloc_rqs(open_clients *lists)
{
 CLIENT_INFO *tx_w = lists->tx_wait;
 CLIENT_INFO *rx_r = lists->rx_ready;
 CLIENT_INFO *tmp;
 REQUEST *rq;
 for(;;) {
	if (!tx_w || !rx_r) break;
		//remove one port number from an rx client
	rq = rx_r->rx_rqueue;
	rq_list_ulink(rq, &rx_r->rx_rqueue);
	rq->msg_ofs = 0; 
		//add the port number to a tx client
	rq_list_add(rq, &tx_w->tx_rqueue);
	tmp = tx_w->next;
	cl_list_ulink(tx_w, &lists->tx_wait);
		//remove one port request from the tx client
	rq = tx_w->rx_rqueue;
	rq_list_ulink(rq, &tx_w->rx_rqueue);
	rq_dtor(rq);
	free(rq);
		//put the tx client on the send queue
	cl_list_add(tx_w, &lists->tx_ready);
	tx_w = tmp;
	if (!rx_r->rx_rqueue) {
		tmp = rx_r->next;
		cl_list_ulink(rx_r, &lists->rx_ready);
		cl_list_add(rx_r, &lists->open);
		rx_r = tmp;
	}
 }
}

static int queue_response(CLIENT_INFO* client, char *resp)
{
 REQUEST *rq;
 rq = malloc(sizeof(*rq));
 if (!rq) {
 	fprintf(stderr, "malloc() failed. Unable to queue request\n");
	return -1;
 }
 rq_ctor(rq);
 strcpy(rq->msg, resp); //we MUST null terminate messages - however, we could use a safer function here
 rq->msg_len = strlen(resp);
 rq_list_add(rq, &client->tx_rqueue);
 return 0;
}

static int service_sockets(SOCKET listen_sock, open_clients *lists)
{
 int rdcount = 0, wrcount = 0, excount = 0;
 fd_set rdset, wrset, exset;
 SOCKET max_sock = -1;
 struct timeval tv;
 int ret;
 rdcount = construct_readable(listen_sock, lists, &rdset, &max_sock); 
 wrcount = construct_writeable(lists, &wrset, &max_sock); 
 excount = construct_exception(listen_sock, lists, &exset, &max_sock); 
 tv.tv_sec = 0;
 tv.tv_usec = 2500;
 ret = select((int)(max_sock+1), &rdset, &wrset, &exset, &tv);
 //////////////////////////////////////////////////////
 //Handle triggered  sockets
 //The order MUST be:
 //1. Sockets in the RX_READY queue and TX_WAIT queue
 //2. Sockets in the OPEN queue
 //3. Listener socket requests
 //4. Sockets in the TX_READY queue
 //////////////////////////////////////////////////////
 if (ret == SOCKET_ERROR) {
 	ret = getSockErr();
	return ret;
 } else if (ret > 0) {
 	CLIENT_INFO *client = lists->rx_ready;
	int fc = 0;
		//1
	for (;fc < ret;) {
		if (!client) break;
		if (FD_ISSET(client->cli_sock, &rdset) || FD_ISSET(client->cli_sock, &exset)) {
			int len = read_msg(client);
			if (len > 0) {
				for (;;) {
					int res = process_msg(client, &len);
					if (!res) {
						int err = handle_msg(client);
						if (err) {
							REQUEST *rq = client->rx_rqueue;
							rq_list_ulink(rq, &client->rx_rqueue);
							free(rq);
							break;
						} else {
							//may disappear
						}
					} else {
						printf("no more work\n");	
						break; //otherwise we had no futher messages
					}
					if (len == 0) {
						int err = queue_response(client, "OK\n");
						cl_list_ulink(client, &lists->rx_ready);
						cl_list_add(client, &lists->tx_ready);
						if (err) return -1;
						printf("All data used");
						break; //no more to process
					}
					printf("len = %d\n", len);
				}
			} else {
				cl_list_ulink(client, &lists->rx_ready);
				client_dtor(client);
				free(client);
				printf("Freeing client\n");
				break;
			}
			fc++;
		}
		client = client->next;
	}
		//1
	client = lists->tx_wait;
	for (;fc < ret;) {
		if (!client) break;
		if (FD_ISSET(client->cli_sock, &rdset) || FD_ISSET(client->cli_sock, &exset)) {
			int len = read_msg(client);
			if (len > 0) {
				for (;;) {
					int res = process_msg(client, &len);
					if (!res) {
						int err = handle_msg(client);
						if (err) {
							REQUEST *rq = client->rx_rqueue;
							rq_list_ulink(rq, &client->rx_rqueue);
							free(rq);
							break;
						}
					} else {
						printf("no more work\n");	
						break; //otherwise we had no futher messages
					}
					if (len == 0) {
						printf("used all data");	
						break; //no more to process
					}
					printf("len = %d\n", len);
				}
			} else {
				cl_list_ulink(client, &lists->tx_wait);	
				client_dtor(client);
				free(client);
				printf("Freeing client\n");
				break;
			}
			fc++;
		}
		client = client->next;
	}
		//2
	client = lists->open;
	for (;fc < ret;) {
		if (!client) break;
		if (FD_ISSET(client->cli_sock, &rdset) || FD_ISSET(client->cli_sock, &exset)) {
			int len = read_msg(client);
			if (len > 0) {
				for (;;) {
					int res = process_msg(client, &len);
					if (!res) {
						int err = handle_msg(client);
						if (err) {
							REQUEST *rq = client->rx_rqueue;
							rq_list_ulink(rq, &client->rx_rqueue);
							free(rq);
							break;
						} else {
						}
					} else break; //otherwise we had no futher messages
					if (len == 0) {
						if (client->type == CLERK_CLIENT_TYPE_RX) {
							int err = queue_response(client, "OK\n");
							if (err) return -1;
							cl_list_ulink(client, &lists->open);
							cl_list_add(client, &lists->tx_ready);
						} else if (client->type == CLERK_CLIENT_TYPE_TX) {
							cl_list_ulink(client, &lists->open);
							cl_list_add(client, &lists->tx_wait); 
						}
						break; //no more to process
						printf("len = %d\n", len);
					}
				}
			} else {
				cl_list_ulink(client, &lists->open);	
				client_dtor(client);
				free(client);
				printf("Freeing client\n");
				break;
			}
			fc++;
		}
		client = client->next;
	}
		//3
	if (FD_ISSET(listen_sock, &rdset) || FD_ISSET(listen_sock, &exset)) {
		int err = add_client(listen_sock, &lists->open);
		if (err) return err;
		cl_dump_list(lists->open, "OPEN");
		cl_dump_list(lists->rx_ready, "RX_READY");
		cl_dump_list(lists->tx_wait, "TX_WAIT");
		cl_dump_list(lists->tx_ready, "TX_READY");
	}
		//4
	client = lists->tx_ready;
	for (;fc < ret;) {
		if (!client) break;
		if (FD_ISSET(client->cli_sock, &wrset) || FD_ISSET(client->cli_sock, &exset)) {
			int err = send_msg(client);
			if (err) {
				cl_list_ulink(client, &lists->tx_ready);
				client_dtor(client);
				free(client);
				printf("Freeing client\n");
				break;
			} else {
				if (!client->tx_rqueue) {
					cl_list_ulink(client, &lists->tx_ready);
					if (client->rx_rqueue) {
						if (client->type == CLERK_CLIENT_TYPE_TX) {
							cl_list_add(client, &lists->tx_wait);
						} else if (client->type == CLERK_CLIENT_TYPE_RX) {
							cl_list_add(client, &lists->rx_ready);
						}
					} else {
						cl_list_add(client, &lists->open);
					}
				}
			}
			fc++;
		}
		client = client->next;
	}
 } else {
	//fprintf(stdout, "dbg - select timeout\n");
 }
 return 0;
}

static int rtpclerk(char *ip_addr, short port)
{
 SOCKET listen_sock;
 int rc;
 open_clients lists;
 rc = create_listen_sock(&listen_sock, ip_addr, port);
 if (rc) {
 	fprintf(stderr, "Err: %d: failed to create listener socket on port %d\n", rc, port);
	return rc;
 }
 fprintf(stdout, "Listening on port %d\n", port);
 lists.open = 0;
 lists.rx_ready = 0;
 lists.tx_wait = 0;
 lists.tx_ready = 0;

 for (;;) {
	rc = service_sockets(listen_sock, &lists);
	if (rc) return rc;
	service_alloc_rqs(&lists);
 }
 return 0;
}

#include "gen/rtpclerk.args.i"

int main(int argc, char **argv)
{
 int sts;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
	return 1;
 }
 if (!arg.ip) {
 	fprintf(stderr, "-i ip option required\n");
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
 	return 1;
 }
#ifdef TiNGTYPE_WINNT
 InitWSA();
#endif
 sts = rtpclerk(arg.ip, arg.port);
#ifdef TiNGTYPE_WINNT
 WSACleanup();
#endif
 return sts;
}
