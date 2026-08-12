/* pathipv4.h - IP v4 path for ASSP connections */

#ifndef INCLUDED_PATHIPV4_H
#define INCLUDED_PATHIPV4_H

#ifdef TiNGTYPE_LINUX
#include <netinet/in.h>
#include <sys/socket.h>
#include "POSIX/socket.h"
#endif

#ifdef TiNGTYPE_QNX
#include <netinet/in.h>
#include <sys/socket.h>
#include "POSIX/socket.h"
#endif


#ifdef TiNGTYPE_WINNT
#include "../libutil/WINNT/wind.h"
#endif

#include "mutx.h"
#include "../assp/assp.h"

struct tcbpath_impl {
	SOCKET sock;
	char rem[256];
	int asspport;
	char asspmonaddr[256];
	int asspmonport;
	char *key;
	PACKET *freelist;
	MUTX freelistmx;
};

LOCALDEC void pathipv4_init(TCBPATH *tcbpath, TCBPATH_IMPL *pathipv4, char *key);

// IPv6 here! The code would be identical apart from ethsize, so why duplicate?
LOCALDEC void pathipv6_init(TCBPATH *tcbpath, TCBPATH_IMPL *pathipv4, char *key);

#endif
