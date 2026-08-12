#ifndef INCLUDED_CREATE_SOCKET_H
#define INCLUDED_CREATE_SOCKET_H

#ifdef TiNGTYPE_LINUX
#include "POSIX/socket.h"
#include <netdb.h>
#endif

#ifdef TiNGTYPE_QNX
#include "POSIX/socket.h"
#include <netdb.h>
#endif


#ifdef TiNGTYPE_WINNT
#include "../libutil/WINNT/wind.h"
#include <ws2tcpip.h>
typedef int socklen_t;
#endif

// allow statically linked programs (e.g. modload) to use this function
#define create_connected_socket aculab_TiNG_create_connected_socket
SOCKET create_connected_socket(const char* name, int name_is_number, unsigned int port, int tcp, int is_assp, struct sockaddr_storage *pss, socklen_t *psl);

#endif
