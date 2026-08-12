#ifndef INCLUDED_TING_CORE_H
#define INCLUDED_TING_CORE_H

#define PROSODY_TiNG

#ifdef TiNGTYPE_WINNT

#ifdef _MT
	// multi-threaded
#ifndef _REENTRANT
#define _REENTRANT
#endif
#else
#ifdef _MSC_VER
	// single-threaded
#ifdef _REENTRANT
#error building single threaded but _REENTRANT is #defined
#endif
#endif
#endif
#endif

	 //only used on Windows for DLLs, so provide a default
#ifndef ACUAPI
#define ACUAPI
#endif

	// check consistency of options
#ifndef _REENTRANT
	// check for anything which requires multi-threaded code

#ifndef OMIT_ANYCHAN
	// anychan needs at least one extra helper thread, so cannot be
	// used single-threaded
#error single-threaded library cannot support 'any channel' operation \
	- build multi-threaded or #define OMIT_ANYCHAN
#endif
#endif

	// assertions
		// include for abort()
#include <stdlib.h>
#include <stdio.h>
#define die(why) do { olog("@%s:%d %s\n", __FILE__, __LINE__, why); abort(); } while (0)
	// debugging
#define HERE fprintf(stderr, "@%s:%d\n", __FILE__, __LINE__)

#ifdef _MSC_VER

#include "../libutil/WINNT/wind.h"
#include <ws2tcpip.h>

/* disable stupid warnings about doubles being converted to floats and
 * floats to unsigned long etc.
 */

#pragma warning(disable:4244)
#pragma warning(disable:4267)
#pragma warning(disable:4305)
///* and about argument conversions (does anyone still use a
//   non-prototype compiler?) */
#pragma warning(disable:4761)

#endif

#include "trace.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

enum dettype {
	DETTYPE_GRUNT,
	DETTYPE_TONE_NORMAL,
	DETTYPE_TONE_BEEP,
	DETTYPE_TONE_CATSIG,
	DETTYPE_TONEREP,
	DETTYPE_CONF,
	DETTYPE_CIV,
	DETTYPE_IWR,
	DETTYPE_CATSIG,
	DETTYPE_CATSIGTONE,
	DETTYPE_CATSIGPLUGIN,
	DETTYPE_ANSAM,
	DETTYPE_BEEP,
	DETTYPE_ONHOOK,
	DETTYPE_REMOTE,
};

#define TONEDET_DETMASK ((1<<DETTYPE_TONE_NORMAL)|(1<<DETTYPE_TONE_BEEP)|(1<<DETTYPE_TONE_CATSIG))


#include "../TiNG.h"
#include "smtypes.h"
#include "cardconn.h"
#ifdef TiNGTYPE_LINUX
#include "POSIX/intdecl.h"
#endif

#ifdef TiNGTYPE_QNX
#include "POSIX/intdecl.h"
#endif


#ifdef TiNGTYPE_WINNT
#include "WINNT/intdecl.h"
#endif

#define FORMAT_SOCKADDR_IN "%d.%d.%d.%d:%d"
#define ARGS_SOCKADDR_IN(x) \
	((ntohl((x).sin_addr.s_addr) >> 24) & 0xff), \
	((ntohl((x).sin_addr.s_addr) >> 16) & 0xff), \
	((ntohl((x).sin_addr.s_addr) >> 8) & 0xff), \
	(ntohl((x).sin_addr.s_addr) & 0xff), \
	ntohs((x).sin_port)

#define FORMAT_IN_ADDR "%d.%d.%d.%d"
#define ARGS_IN_ADDR(x) \
	((ntohl((x).s_addr) >> 24) & 0xff), \
	((ntohl((x).s_addr) >> 16) & 0xff), \
	((ntohl((x).s_addr) >> 8) & 0xff), \
	(ntohl((x).s_addr) & 0xff)

#define FORMAT_IN6_ADDR "%x:%x:%x:%x:%x:%x:%x:%x"
#define ARGS_IN6_ADDR(x) \
	((((x).s6_addr[0])<<8)+((x).s6_addr[1])), \
	((((x).s6_addr[2])<<8)+((x).s6_addr[3])), \
	((((x).s6_addr[4])<<8)+((x).s6_addr[5])), \
	((((x).s6_addr[6])<<8)+((x).s6_addr[7])), \
	((((x).s6_addr[8])<<8)+((x).s6_addr[9])), \
	((((x).s6_addr[10])<<8)+((x).s6_addr[11])), \
	((((x).s6_addr[12])<<8)+((x).s6_addr[13])), \
	((((x).s6_addr[14])<<8)+((x).s6_addr[15]))

#define FORMAT_SOCKADDR_IN6 "[%x:%x:%x:%x:%x:%x:%x:%x]:%d"
#define ARGS_SOCKADDR_IN6(x) \
	((((x).sin6_addr.s6_addr[0])<<8)+((x).sin6_addr.s6_addr[1])), \
	((((x).sin6_addr.s6_addr[2])<<8)+((x).sin6_addr.s6_addr[3])), \
	((((x).sin6_addr.s6_addr[4])<<8)+((x).sin6_addr.s6_addr[5])), \
	((((x).sin6_addr.s6_addr[6])<<8)+((x).sin6_addr.s6_addr[7])), \
	((((x).sin6_addr.s6_addr[8])<<8)+((x).sin6_addr.s6_addr[9])), \
	((((x).sin6_addr.s6_addr[10])<<8)+((x).sin6_addr.s6_addr[11])), \
	((((x).sin6_addr.s6_addr[12])<<8)+((x).sin6_addr.s6_addr[13])), \
	((((x).sin6_addr.s6_addr[14])<<8)+((x).sin6_addr.s6_addr[15])), \
	ntohs((x).sin6_port)

#if !defined(TiNG_BIG_ENDIAN) && !defined(TiNG_LITTLE_ENDIAN)

#ifdef TiNGTYPE_LINUX
#ifdef TiNGTYPE_PXPPC
#define TiNG_BIG_ENDIAN
#else
#define TiNG_LITTLE_ENDIAN
#endif
#endif

#ifdef TiNGTYPE_QNX
#include <gulliver.h>
#ifdef __LITTLEENDIAN__
#define TiNG_LITTLE_ENDIAN
#endif
#ifdef __BIGENDIAN__
#define TiNG_BIG_ENDIAN
#endif
#endif


#ifdef TiNGTYPE_WINNT
#define TiNG_LITTLE_ENDIAN
#endif

#endif // !BIG and !LITTLE

#if !defined(TiNG_BIG_ENDIAN) && !defined(TiNG_LITTLE_ENDIAN)
#error Endianness not defined
#endif

	// by historical accident the ipv4 addresses passed
	// in messages are in little endian network order
	// these macros work around this problem
#ifdef TiNG_BIG_ENDIAN
#define ip2msg(x) (((((x)>>24) & 0xff)<<0) \
				 + ((((x)>>16) & 0xff)<<8) \
				 + ((((x)>>8) & 0xff)<<16) \
				 + ((((x)>>0) & 0xff)<<24))

#define msg2ip(y) (((((y)>>24) & 0xff)<<0) \
				 + ((((y)>>16) & 0xff)<<8) \
				 + ((((y)>>8) & 0xff)<<16) \
				 + ((((y)>>0) & 0xff)<<24))
#endif

#ifdef TiNG_LITTLE_ENDIAN
#define ip2msg(x) (x)
#define msg2ip(y) (y)
#endif


// for IPv6 addresses each 32bit chunk is in host order
// so the macro/function are the same regardless of endianness
#define ipv6tomsg(x) \
	(((x).s6_addr[0]<<24) + ((x).s6_addr[1]<<16) + ((x).s6_addr[2]<<8) + ((x).s6_addr[3])), \
	(((x).s6_addr[4]<<24) + ((x).s6_addr[5]<<16) + ((x).s6_addr[6]<<8) + ((x).s6_addr[7])), \
	(((x).s6_addr[8]<<24) + ((x).s6_addr[9]<<16) + ((x).s6_addr[10]<<8) + ((x).s6_addr[11])), \
	(((x).s6_addr[12]<<24) + ((x).s6_addr[13]<<16) + ((x).s6_addr[14]<<8) + ((x).s6_addr[15]))

#define msgtoipv6 aculab_TiNGcore_msgtoipv6
void msgtoipv6(struct in6_addr *ipv6, uint32_t w0, uint32_t w1, uint32_t w2, uint32_t w3);

#include "smcore.h"

#include "once.h"

#include "TiNGo_cardmap.h"

	// the cardmap to support tSMCardId being an arbitrary value
extern struct cardmap aculab_TiNG_cardmap;

	// we have to let this be global, so avoid an unqualified name
#define showmsg aculab_TiNG_showmsg
void showmsg(U32 *msg);

	// we have to let this be global, so avoid an unqualified name
#define alloc aculab_TiNGcore_alloc
void *alloc(void *old, unsigned size);

	// duplicate an area of memory, return 0 for failure
LOCALDEC void *dupmem(const void *mem, unsigned len);

	// lock a module
#define lock_module aculab_TiNGcore_lock_module
void lock_module(struct module *mp);
	// unlock a module
#define unlock_module(mp) ((mp)->unlock((mp)))
	// unlock a card
LOCALDEC void unlock_card(struct card *cardp);
	// convert API-visible tSMModuleId to an internal module already locked
#define modid2lockedmodule aculab_TiNG_modid2lockedmodule 
struct module *modid2lockedmodule(tSMModuleId modno);
	// set the timeslot for the channel to use
LOCALDEC int manual_assign(tSMChannelId chan, unsigned int stream, unsigned int timeslot, enum TiNG_COMPAND_TYPE type, int direct);
	// set the timeslot for the channel to use
LOCALDEC int auto_assign(tSMChannelId chan, unsigned int st0, unsigned int st1, enum TiNG_COMPAND_TYPE type, int direct);
	// open a connection to a module
LOCALDEC ERRCODE open_module(CARDCONN *cx, struct card *card, U32 modmask);
	// open a connection to a channel
LOCALDEC int open_channel(RAWCHAN *cp, struct module *mp);
	// force a channel to close by telling module to abort operations
	// in progress. Then invoke channel dtor
LOCALDEC void delete_channel(tSMChannelId chan);
	// break connection between channel and event (e.g. for non idle)
LOCALDEC int disengage_event(tSMChannelId chan, enum event_types etyp, int hand);
	// make connection between channel and event (e.g. for non idle)
LOCALDEC int engage_event(tSMChannelId chan, enum event_types etyp, int hand);
	// find the magic number for a channel
LOCALDEC ERRCODE getmagic(U32 *mp, CARDCONN *cx);
	// make specified channel a friend of the task on this connection
LOCALDEC int setfriend(CARDCONN *cx, RAWCHAN *cp, unsigned type);
	// make specified channel a friend of the task on this connection
LOCALDEC int setfriend_cx(CARDCONN *cx, CARDCONN *frcx, unsigned type);
	// start a task
LOCALDEC int starttask(CARDCONN *cx, unsigned long typecode, unsigned long ver);
	// update transfer threshold to current setting
LOCALDEC int chan_setxfer(tSMChannelId chan, int isrx);
	// update transfer threshold to provided setting
#define cx_setxfer aculab_TiNGcore_cx_setxfer
int cx_setxfer(CARDCONN *cx, S32 minimum_bits, U32 timeout_milliseconds);
	// update the cardconn to expect no more data
#define cx_nomore aculab_TiNGcore_cx_nomore
ERRCODE cx_nomore(CARDCONN *cx);
	// start an output task
LOCALDEC int startout(tSMChannelId chan, unsigned long typecode);
	// start an input task
LOCALDEC int startin(tSMChannelId chan, unsigned long typecode);
	// send one-word message
LOCALDEC int sendmsg1(CARDCONN *cx, U16 message);
	// send two-word message
LOCALDEC int sendmsg2(CARDCONN *cx, U16 message, U32 p1);
	// send three-word message
LOCALDEC int sendmsg3(CARDCONN *cx, U16 message, U32 p1, U32 p2);
	// send four-word message
LOCALDEC int sendmsg4(CARDCONN *cx, U16 message, U32 p1, U32 p2, U32 p3);
	// start a type of detection, creating inchan as necessary
LOCALDEC int startdet(tSMChannelId chan, unsigned hand, enum dettype dettype, int isrec);
	// stop a type of detection, deleting inchan if necessary
LOCALDEC int stopdet(tSMChannelId chan, unsigned hand, enum dettype dettype, int isrec);
	// fetch next message from queue
LOCALDEC int rdmsg(struct ting_message *msg, CARDCONN *cx);
	// convert API-visible companding to driver's code
LOCALDEC int tst2compand(enum TiNG_COMPAND_TYPE *ct, enum kSMTimeslotType tt);
	// convert driver's companding code to API-visible code
LOCALDEC int compand2tst(enum kSMTimeslotType *tt, enum TiNG_COMPAND_TYPE ct);
    // round float value and convert to U32 for putting in a message
LOCALDEC U32 roundfu(float f);

LOCALDEC const char *version;
#endif
