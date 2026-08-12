#include "pathipv4.h"
#include "trace.h"
#include "TiNGcommon.h"

#include <errno.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef TiNGTYPE_LINUX
#include <arpa/inet.h>
#endif

#ifdef TiNGTYPE_QNX
#include <arpa/inet.h>
#endif


#ifdef TiNGTYPE_WINNT
#define ssize_t int
#endif

#define arlen(x) (sizeof(x)/sizeof(*(x)))

#include "../assp/assp.h"

typedef TCBPATH_IMPL PATH;	// synonym for brevity

static uint8_t *validate_cookie(PATH *path, TCB *tcb, MD5_STREAM *msp, uint8_t *sp, uint8_t *ecoo)
{
 (void) tcb;
 (void) ecoo;
 md5s_include(msp, (uint8_t*)&path->rem, strlen(path->rem));
 md5s_include(msp, (uint8_t*)&path->asspport, sizeof(path->asspport));
 return sp;
}

static uint8_t *generate_cookie(PATH *path, TCB *tcb, MD5_STREAM *msp, uint8_t *sp)
{
 (void) tcb;
 md5s_include(msp, (uint8_t*)&path->rem, strlen(path->rem));
 md5s_include(msp, (uint8_t*)&path->asspport, sizeof(path->asspport));
 return sp;
}

static uint8_t *make_keycode(PATH *path, TCB *tcb, uint8_t *sp, uint8_t *ep, uint8_t *op)
{
 if (!path->key || !path->key[0]) {
	memset(op, 0, 16);
 } else {
	MD5_STREAM ms;
	(void) tcb;
	md5_stream(&ms);
	md5s_include(&ms, sp, ep - sp);
	md5s_include(&ms, (uint8_t*)path->key, strlen(path->key));
	md5s_final(op, &ms);
 }
 return op + 16;
}

static int netsendpkt(PATH *path, uint8_t *data, uint8_t *end)
{
 ssize_t len = end - data;
 ssize_t i = send(path->sock, (void *) data, len, 0);
 if (i != len) {
#ifdef TiNGTYPE_LINUX
	if (errno == EPERM) return 0;	// cope with wierdness
#endif
#ifdef TiNGTYPE_WINNT
	DWORD err = WSAGetLastError();
	if (TiNGtrace > 4) olog("send() sent %d of %d, err=%d\n", i, len, err);
	if (err == WSAEWOULDBLOCK || err == WSAENOBUFS) {
		return 0;
	}
#endif
	if (i < 0) osockerr("send() failed");
	else if (TiNGtrace > 0) olog("send() sent %d of %d\n", i, len);
	if (TiNGtrace > 0) olog("Cannot send to [%s]:%d\n",
		path->rem, path->asspport);
	return 1;
 }
 return 0;
}

static PACKET *rxalloc(PATH *path, unsigned maxrx)
{
 PACKET *rxp;
 const unsigned maxlen = 1500 - 20 /* IP */ - 8 /* UDP */;
 (void) maxrx;
 mutx_enter(&path->freelistmx);
 rxp = path->freelist;
 if (rxp) {
	path->freelist = path->freelist->next;
	mutx_leave(&path->freelistmx);
 } else {
	mutx_leave(&path->freelistmx);
 	rxp = alloc(0, sizeof(*rxp));
	if (!rxp) return 0;
 }
 rxp->rawstart = rxp->data;
 if (arlen(rxp->data) > maxlen) rxp->rawend = rxp->data + maxlen;
 else rxp->rawend = rxp->data + arlen(rxp->data);
 rxp->dbg_bc = 0;
 return rxp;
}

static void rxfree(PATH *path, PACKET *rxp)
{
 if (rxp->dbg_bc) die("Freeing packet that holds a block");
#if 1
 mutx_enter(&path->freelistmx);
 rxp->next = path->freelist;
 path->freelist = rxp;
 mutx_leave(&path->freelistmx);
#else
	// real alloc/free for leak detection
 alloc(rxp, 0);
#endif
}

static void pathipv4_dtor(PATH *path)
{
 for (;;) {
 	PACKET *rxp = path->freelist;
 	if (!rxp) break;
 	path->freelist = rxp->next;
 	free(rxp);
 }
 mutx_dtor(&path->freelistmx);
}

static unsigned ethsize_ipv4(TCBPATH_IMPL *path, unsigned bytes)
{
 (void) path;
 return (bytes + /* UDP */ 8 + /* IP */ 20 + /* eth */ 14) * 8
	 	+ /* preamble */ 64 + /* gap */ 96;
}

static unsigned ethsize_ipv6(TCBPATH_IMPL *path, unsigned bytes)
{
 (void) path;
 return (bytes + /* UDP */ 8 + /* IPv6 */ 40 + /* eth */ 14) * 8
	 	+ /* preamble */ 64 + /* gap */ 96;
}

static struct tcbpath_vtbl pathipv4_tcbpath_vtbl = {
	validate_cookie,
	generate_cookie,
	netsendpkt,
	rxalloc,
	rxfree,
	pathipv4_dtor,
	make_keycode,
	ethsize_ipv4,
};

static struct tcbpath_vtbl pathipv6_tcbpath_vtbl = {
	validate_cookie,
	generate_cookie,
	netsendpkt,
	rxalloc,
	rxfree,
	pathipv4_dtor,
	make_keycode,
	ethsize_ipv6,
};

LOCALDEF void pathipv4_init(TCBPATH *tcbpath, TCBPATH_IMPL *pathipv4, char *key)
{
 pathipv4->key = key;
 pathipv4->freelist = 0;
 tcbpath->pi = pathipv4;
 tcbpath->vtbl = &pathipv4_tcbpath_vtbl;
 mutx_init(&pathipv4->freelistmx);
}

// IPv6 here! The code would be identical apart from ethsize, so why duplicate?
LOCALDEF void pathipv6_init(TCBPATH *tcbpath, TCBPATH_IMPL *pathipv4, char *key)
{
 pathipv4->key = key;
 pathipv4->freelist = 0;
 tcbpath->pi = pathipv4;
 tcbpath->vtbl = &pathipv6_tcbpath_vtbl;
 mutx_init(&pathipv4->freelistmx);
}
