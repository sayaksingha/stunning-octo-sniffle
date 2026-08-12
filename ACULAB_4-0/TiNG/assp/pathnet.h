/* pathnet.h - network path */

#ifndef INCLUDED_PATHNET_H
#define INCLUDED_PATHNET_H

#include <stdint.h>

#ifdef TiNGTYPE_LINUX
#include <netinet/in.h>
#include <sys/socket.h>
#endif


#ifdef TiNGTYPE_WINNT
#include "../libutil/WINNT/wind.h"
#endif

struct tcbpath_impl {
	int sock;
	struct sockaddr_in rem;
	//struct sockaddr_in loc;
	uint_fast64_t sendbits;
	uint_fast64_t sendpkts;
	uint_fast64_t discpkts;
	uint_fast64_t recvbits;
	uint_fast64_t recvpkts;
};

#include "assp.h"

void pathnet_init(TCBPATH *tcbpath, TCBPATH_IMPL *pathnet, int sock);

#endif
