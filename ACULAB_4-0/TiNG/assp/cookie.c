#include <pthread.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#ifdef USE_GETTIMEOFDAY
#include <sys/time.h>
#include <unistd.h>
#endif

#include "assp.h"
#include "cookie.h"
#include "randent.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

#define MAX_TXTS_DIFF 16
#define MAX_COOKIE_LIFE 16

#ifdef _MSC_VER

/* disable stupid warnings about conversions
 */

#pragma warning(disable:4244)
#pragma warning(disable:4305)
/* and about argument conversions (does anyone still use a
   non-prototype compiler?) */
#pragma warning(disable:4761)

#endif

static pthread_mutex_t cookie_mx = PTHREAD_MUTEX_INITIALIZER;	// protects the static data

static struct {
	int pid;
#if 0
#ifdef USE_GETTIMEOFDAY
	struct timeval start;
#endif
	char devrand[16];
#endif
} cookie_dough;

	// part-ready cookies
	// Each one is prepared when first required, and re-used until
	// it's too old. we need at least as many of these as necessary
	// to avoid overflow before cookies up to MAX_COOKIE_LIFE expire
static struct {
	MD5_STREAM md5;
	time_t when;
	unsigned issued;
} cookie_hist[16];			// or however many we want

static uint8_t *decode32(uint32_t *v, uint8_t *sp)
{
 *v = *sp++ << 24;
 *v += *sp++ << 16;
 *v += *sp++ << 8;
 *v += *sp++;
 return sp;
}

static uint8_t *encode32r(uint8_t *sp, uint32_t v)
{
 *--sp = v & 0xff;
 *--sp = (v >> 8) & 0xff;
 *--sp = (v >> 16) & 0xff;
 *--sp = (v >> 24) & 0xff;
 return sp;
}

static unsigned current_cookie(void)
{
 time_t now = time(0);
 unsigned lastc = 0;
 unsigned u;
 for (u=1; u < arlen(cookie_hist); u++) {
	if (cookie_hist[u].when < cookie_hist[lastc].when) lastc = u;
	if (cookie_hist[u].when == now) return u;
 }
 //if (!lastc && !cookie_dough.pid) cookie_dough.pid = 0x123456789u;
 cookie_hist[lastc].when = now;
 md5_stream(&cookie_hist[lastc].md5);
 md5s_include(&cookie_hist[lastc].md5, (uint8_t *) &cookie_dough, sizeof(cookie_dough));
 md5s_include(&cookie_hist[lastc].md5, (uint8_t *) &lastc, sizeof(lastc));
 cookie_hist[lastc].issued = 0;
 return lastc;
}

// Cookie:
//	CN
//	TXTS of request
//	when: time of request
//	stuff from path
//	16-byte hash

// included in MD5: initial stuff, plus:
//	CID, SEQ, ACK, TXTS, REFTS: from cookie request [as in pkt]
//	when: time of request (field in cookie) [as local var]
//	other stuff from path

uint8_t *bake_cookie(TCB *tcb, uint8_t *sp, RPACKET *pkt)
{
 int e = pthread_mutex_lock(&cookie_mx);
 if (!e) {
	uint32_t when = -time_rel2longuS(time_until(time_zero()));
	uint32_t cn = current_cookie();
	uint8_t *md5pos;
		// here's some I prepared earlier
	MD5_STREAM ms = cookie_hist[cn].md5;
	md5s_include(&ms, pkt->pkt->rawstart + ASSP_PKTOFS_CID, 20);	// CID..REFTS
	md5s_include(&ms, (uint8_t *) &when, 4/*sizeof(when)*/);
	sp -= 16;		// space for MD5 hash
	md5pos = sp;
	sp = tcb->path->vtbl->generate_cookie(tcb->path->pi, tcb, &ms, sp);
	sp = encode32r(sp, when);
	sp = encode32r(sp, pkt->txts);
	sp = encode32r(sp, cn);
	md5s_final(md5pos, &ms);
	cookie_hist[cn].issued++;
	e = pthread_mutex_unlock(&cookie_mx);
	if (e) printf("pthread_mutex_unlock returned %d\n", e);
 } else printf("pthread_mutex_lock returned %d\n", e);
 if (e) return 0;
 return sp;
}

// check if this is a nice cookie
static int unsafe_nice_cookie(TCB *tcb, PACKET *pkt, uint8_t *scoo, uint8_t *ecoo)
{
 unsigned char cookie[16];
 unsigned char ack_buf[4];
 time_t now = time(0);
 uint32_t txts;
 uint32_t when;
 uint32_t cn;
 uint32_t dt;
 uint32_t ack;
 MD5_STREAM ms;
 scoo = decode32(&cn, scoo);
 if (cn >= arlen(cookie_hist)) {
		// Stale? This cookie has got mould growing on it!
	tcb->stats.numrx_coo_invtime++;
 	return 0;
 }
 if (!cookie_hist[cn].issued) {
	tcb->stats.numrx_coo_invtime++;
 	return 0;
 }
 if (now - cookie_hist[cn].when > MAX_COOKIE_LIFE) {
	tcb->stats.numrx_coo_stale++;
 	return ERR_COOKIE_EXPIRED;
 }
 scoo = decode32(&txts, scoo);
 scoo = decode32(&when, scoo);
 ms = cookie_hist[cn].md5;
 md5s_include(&ms, pkt->rawstart + ASSP_PKTOFS_CID, 8); // CID, SEQ

 decode32(&ack,pkt->rawstart + ASSP_PKTOFS_ACK);
 encode32r(&ack_buf[4],--ack); // usecookie is sequenced so ack is one more than use to make cookie
 md5s_include(&ms, ack_buf, 4);

 md5s_include(&ms, scoo - 8, 4);	// TXTS
 md5s_include(&ms, pkt->rawstart + ASSP_PKTOFS_REFTS, 4);
 md5s_include(&ms, (uint8_t *) &when, 4);	// when
 scoo = tcb->path->vtbl->validate_cookie(tcb->path->pi, tcb, &ms, scoo, ecoo);
 md5s_final(cookie, &ms);
 if (ecoo - scoo != 16) {	// should be only hash itself left
	tcb->stats.numrx_coo_badsize++;
 	return 0;
 }
 if (memcmp(scoo, cookie, sizeof(cookie))) {
	tcb->stats.numrx_coo_bogus++;
 	return 0;
 }
 dt = -time_rel2longuS(time_until(time_zero())) - when;
 if (!dt) dt = 1;
 return dt;
}

int nice_cookie(TCB *tcb, PACKET *pkt, uint8_t *scoo, uint8_t *ecoo)
{
 int e = pthread_mutex_lock(&cookie_mx);
 int r = 0;
 if (!e) {
 	r = unsafe_nice_cookie(tcb, pkt, scoo, ecoo);
	e = pthread_mutex_unlock(&cookie_mx);
	if (e) r = 0;
 }
 return r;
}
