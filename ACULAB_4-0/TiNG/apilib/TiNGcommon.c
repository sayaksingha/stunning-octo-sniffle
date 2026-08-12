/* TiNGlib.c - implements Prosody API in terms of TiNG primitive API */

#include "TiNGcommon.h"
#include "TiNGanyevent.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "TiNGo_chan.h"
#include "../TiNG.h"
#include "../hsif/kernel.h"
#include "../hsif/samplerate.h"
#include "../hsif/subtask.h"
#include "../hsif/typecode.h"

LOCALDEF const char *version = "@(#)$from: " __FILE__ " "
#include "../gen/version.i"
	" $";

#include "gen/coreversion.i"

	// the actual run-once block for the global init function
RUN_ONCE run_once_init;

#if 0
STATIC void *dbg_realloc(void *old, unsigned size, char *file, unsigned line)
{
 void *p = realloc(old, size * 4 + 1024);
 printf("malloc(%d, %s:%d) = %p\n", size, file, line, p);
 return p;
}

STATIC void dbg_free(void *p, char *file, unsigned line)
{
 printf("free(%p, %s:%d)\n", p, file, line);
 free(p);
}

#define malloc(x) dbg_realloc(0, x, __FILE__, __LINE__)
#define free(x) dbg_free(x, __FILE__, __LINE__)
#endif

void msgtoipv6(struct in6_addr *ipv6, uint32_t w0, uint32_t w1, uint32_t w2, uint32_t w3)
{
	ipv6->s6_addr[0] = (w0>>24) & 0xff;
	ipv6->s6_addr[1] = (w0>>16) & 0xff;
	ipv6->s6_addr[2] = (w0>>8) & 0xff;
	ipv6->s6_addr[3] = (w0) & 0xff;

	ipv6->s6_addr[4] = (w1>>24) & 0xff;
	ipv6->s6_addr[5] = (w1>>16) & 0xff;
	ipv6->s6_addr[6] = (w1>>8) & 0xff;
	ipv6->s6_addr[7] = (w1) & 0xff;

	ipv6->s6_addr[8] = (w2>>24) & 0xff;
	ipv6->s6_addr[9] = (w2>>16) & 0xff;
	ipv6->s6_addr[10] = (w2>>8) & 0xff;
	ipv6->s6_addr[11] = (w2) & 0xff;

	ipv6->s6_addr[12] = (w3>>24) & 0xff;
	ipv6->s6_addr[13] = (w3>>16) & 0xff;
	ipv6->s6_addr[14] = (w3>>8) & 0xff;
	ipv6->s6_addr[15] = (w3) & 0xff;
}

#include "trace.h"

void *alloc(void *old, unsigned size)
{
 void *p = realloc(old, size);
 if (!p && size && TiNGtrace) {
 	olog("realloc(%p, %u) failed\n", old, size);
 }
 return p;
}

#ifndef OMIT_ANYCHAN
#define REQUIRE_EVENTS
#endif

#include "errcode.h"

#include "condvar.h"
#include "event.h"
#include "mutx.h"
#include "once.h"
#include "timestamp.h"

#ifdef TiNGTYPE_LINUX
#include <poll.h>
#define Sleep(x) poll(0, 0, x)
#endif

#ifdef TiNGTYPE_QNX
#include <sys/poll.h>
#define Sleep(x) poll(0, 0, x)
#endif


#ifdef TiNGTYPE_WINNT
	// uses mutex
#define vsnprintf _vsnprintf
#endif

#include "smdrvr.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

#include "paranoia.h"

LOCALDEF void *dupmem(const void *mem, unsigned len)
{
 void *p = alloc(0, len);
 if (p) memcpy(p, mem, len);
 return p;
}

#if TiNG_TRACE > 0
int aculab_TiNG_trace_error_log(int e, char *file, unsigned line)
{
 if (TiNGtrace) olog("TiNGlib: @%s:%d error %d\n", file, line, e);
 return e;
}
#endif

struct module;

struct cardmap aculab_TiNG_cardmap;

void showmsg(U32 *msg)
{
 char buff[160], *cp = buff;
 if (*msg) {
	unsigned int i = MSG2LEN(*msg);
	if (!i) i = 16;
	for (; i; i--) {
		*cp++ = ' ';
		cp += sprintf(cp, "%08lx", (long) *msg++);
	}
 }
 *cp = 0;
 olog("[msg%s]", buff);
}

STATIC void unlock_mod(struct module *mp)
{
 unlock_object(mp);
}

void lock_module(struct module *mp)
{
 lock_object(mp);
 mp->unlock = unlock_mod;
}

LOCALDEF void unlock_card(struct card *cardp)
{
 unlock_object(cardp);
}

struct module *modid2lockedmodule(tSMModuleId modno)
{
 struct module *mp = (struct module *) modno;
 lock_module(mp);
 return mp;
}

// set the channel's timeslot. This can be either its serial port timeslot
// or an external timeslot. If it's external, the serial port timeslot must
// be auto-allocated first.
STATIC int do_manual_assign(tSMChannelId chan, unsigned int stream, unsigned int timeslot, enum TiNG_COMPAND_TYPE type, int direct)
{
 int i = chan->mod->card->vtbl->manual_assign(chan, stream, timeslot, type, direct);
 if (i) return i;
 if (direct) {
	enum handles h;
	for (h=HANDLE_ECHO; h < HANDLE_NUM; h++) {
		if (chan->mod->card->vtbl->engage_df(chan->df[1],
			&chan->chan[h].cx, DFC_IN)) {
			return err(ERR_SM_DEVERR);
		}
	}
 } else {
	if (chan->mod->card->vtbl->engage_df(chan->df[0], &chan->chan[HANDLE_WRITE].cx, DFC_OUT)) {
		return err(ERR_SM_DEVERR);
	}
 }
 return 0;
}

LOCALDEF int manual_assign(tSMChannelId chan, unsigned int stream, unsigned int timeslot, enum TiNG_COMPAND_TYPE type, int direct)
{
 int i = do_manual_assign(chan, (unsigned) -1, (unsigned) -1, type, direct);
 if (i) return i;
 return do_manual_assign(chan, stream, timeslot, type, direct);
}


LOCALDEF int auto_assign(tSMChannelId chan, unsigned int st0, unsigned int st1, enum TiNG_COMPAND_TYPE type, int direct)
{
 return chan->mod->card->vtbl->auto_assign(chan, st0, st1, type, direct);
}

LOCALDEF ERRCODE open_module(CARDCONN *cx, struct card *card, U32 modmask)
{
 struct ting_destination dst;
 ERRCODE e;
 dst.ioctl_length = sizeof(dst);
 dst.dest = TiNG_DEST_MODULE;
 dst.modmask = modmask;
 e = card->vtbl->cardconn_ctor(cx, card);
 if (ERR(e)) return e;
 e = cx_ioctl(cx, IOCTL_TiNG_WRDEST, &dst, sizeof(dst));
 if (ERR(e)) {
	cx_close(cx);
	return e;
 }
 return OK;
}

LOCALDEF int open_channel(RAWCHAN *cp, struct module *mp)
{
 struct ting_destination dst;
 dst.ioctl_length = sizeof(dst);
 dst.dest = TiNG_DEST_CHANNEL;
 dst.modmask = mp->modmask;
 if (ERR(cx_clone(&cp->cx, &mp->cx))) return err(ERR_SM_DEVERR);
 if (ERR(cx_ioctl(&cp->cx, IOCTL_TiNG_WRDEST, &dst, sizeof(dst)))) {
	cx_close(&cp->cx);
	return err(ERR_SM_DEVERR);
 }
 cp->channo = dst.channo;
 return 0;
}

LOCALDEF void delete_channel(tSMChannelId chan)
{
 chan_dtor(chan);
}

LOCALDEF int disengage_event(tSMChannelId chan, enum event_types etyp, int hand)
{
 return chan->mod->card->vtbl->disengage_event(chan, etyp, &chan->chan[hand].cx);
}

LOCALDEF int engage_event(tSMChannelId chan, enum event_types etyp, int hand)
{
 return chan->mod->card->vtbl->engage_event(chan, etyp, &chan->chan[hand].cx);
}

LOCALDEF ERRCODE getmagic(U32 *mp, CARDCONN *cx)
{
 struct ting_destination dest;
 ERRCODE e;
 dest.ioctl_length = sizeof(dest);
 e = cx_ioctl(cx, IOCTL_TiNG_RDDEST, &dest, sizeof(struct ting_destination));
 if (ERR(e)) return e;
 *mp = dest.magic;
 return OK;
}

LOCALDEF int setfriend(CARDCONN *cx, RAWCHAN *cp, unsigned type)
{
 struct ting_friend friend;
 friend.ioctl_length = sizeof(friend);
 if (cp) {
	ERRCODE e;
	friend.channo = cp->channo;
	e = getmagic(&friend.magic, &cp->cx);
	if (ERR(e)) return err(ERR_SM_DEVERR);
 } else {
	friend.channo = 0;
	friend.magic = 0;
 }
 friend.type = type;
 if (ERR(cx_ioctl(cx,
	IOCTL_TiNG_WRFRIEND, &friend, sizeof(struct ting_friend))))
	return err(ERR_SM_DEVERR);
 return 0;
}

LOCALDEF int setfriend_cx(CARDCONN *cx, CARDCONN *frcx, unsigned type)
{
 struct ting_friend friend;
 friend.ioctl_length = sizeof(friend);
 if (frcx) {
	struct ting_destination dest;
	ERRCODE e;
	dest.ioctl_length = sizeof(dest);
	e = cx_ioctl(frcx, IOCTL_TiNG_RDDEST, &dest, sizeof(struct ting_destination));
	if (ERR(e)) return err(ERR_SM_DEVERR);
	friend.channo = dest.channo;
	friend.magic = dest.magic;
 } else {
	friend.channo = 0;
	friend.magic = 0;
 }
 friend.type = type;
 if (ERR(cx_ioctl(cx,
	IOCTL_TiNG_WRFRIEND, &friend, sizeof(struct ting_friend))))
	return err(ERR_SM_DEVERR);
 return 0;
}

LOCALDEF int starttask(CARDCONN *cx, unsigned long typecode, unsigned long ver)
{
 struct ting_task task;
 task.ioctl_length = sizeof(task);
 task.tasktypecode = typecode;
 task.version = ver;
 if (ERR(cx_ioctl(cx, IOCTL_TiNG_WRTASK, &task, sizeof(task))))
	return err(ERR_SM_DEVERR);
 return 0;
}

LOCALDEF int chan_setxfer(tSMChannelId chan, int isrx)
{
 struct ting_xfer xfer;
 xfer.ioctl_length = sizeof(xfer);
 xfer.minimum_bits = chan->rxtx[isrx].minimum_bits;
 xfer.timeout_milliseconds = chan->rxtx[isrx].timeout_milliseconds;
 xfer.nomore = 0;
 if (ERR(cx_ioctl(&chan->chan[isrx ? HANDLE_READ : HANDLE_WRITE].cx,
	IOCTL_TiNG_WRXFER, &xfer, sizeof(xfer))))
	return err(ERR_SM_DEVERR);
 return 0;
}

int cx_setxfer(CARDCONN *cx, S32 minimum_bits, U32 timeout_milliseconds)
{
 struct ting_xfer xfer;
 xfer.ioctl_length = sizeof(xfer);
 xfer.minimum_bits = minimum_bits;
 xfer.timeout_milliseconds = timeout_milliseconds;
 xfer.nomore = 0;
 if (ERR(cx_ioctl(cx,IOCTL_TiNG_WRXFER, &xfer, sizeof(xfer))))
	return err(ERR_SM_DEVERR);
 return 0;
}

ERRCODE cx_nomore(CARDCONN *cx)
{
 struct ting_xfer xfer;
 xfer.ioctl_length = sizeof(xfer);
 xfer.minimum_bits = 1<<24;	// very large number
 xfer.timeout_milliseconds = 0;
 xfer.nomore = 1;
 return cx_ioctl(cx, IOCTL_TiNG_WRXFER, &xfer, sizeof(xfer));
}

LOCALDEF int startout(tSMChannelId chan, unsigned long typecode)
{
 CARDCONN *cx = &chan->chan[HANDLE_WRITE].cx;
 int sts = starttask(cx, typecode, 0);
 if (sts) return sts;
 if (chan->out_sample_rate != 8000) {
 	sts = sendmsg2(cx, MSG_SAMPLERATE, chan->out_sample_rate);
	if (sts) return sts;
 }
 sts = chan->mod->card->vtbl->engage_df(chan->df[0], cx, DFC_OUT);
 return sts;
}

LOCALDEF int startin(tSMChannelId chan, unsigned long typecode)
{
 CARDCONN *cx = &chan->chan[HANDLE_READ].cx;
 int sts;
 sts = starttask(cx, typecode, 0);
 if (sts) return sts;
 if (nonnull_datafeed(chan->df[1])) {
	sts = chan->mod->card->vtbl->engage_df(chan->df[1], cx, DFC_IN);
	if (!sts && chan->ecloopback) {
		sts = setfriend(&chan->chan[HANDLE_READ].cx, &chan->chan[HANDLE_ECHO], FRIEND_TYPE_ECHOMAIN);
	}
 }
 return sts;
}

LOCALDEF int sendmsg1(CARDCONN *cx, U16 message)
{
 struct ting_message msg;
 msg.ioctl_length = sizeof(msg);
 msg.flags = 0;
 msg.msg[0] = MESSAGE(message, 1);
 if (ERR(cx_ioctl(cx, IOCTL_TiNG_WRMSG,
	&msg, sizeof(struct ting_message))))
	return err(ERR_SM_DEVERR);
 return 0;
}

LOCALDEF int sendmsg2(CARDCONN *cx, U16 message, U32 p1)
{
 struct ting_message msg;
 msg.ioctl_length = sizeof(msg);
 msg.flags = 0;
 msg.msg[0] = MESSAGE(message, 2);
 msg.msg[1] = p1;
 if (ERR(cx_ioctl(cx, IOCTL_TiNG_WRMSG,
	&msg, sizeof(struct ting_message))))
	return err(ERR_SM_DEVERR);
 return 0;
}

LOCALDEF int sendmsg3(CARDCONN *cx, U16 message, U32 p1, U32 p2)
{
 struct ting_message msg;
 msg.ioctl_length = sizeof(msg);
 msg.flags = 0;
 msg.msg[0] = MESSAGE(message, 3);
 msg.msg[1] = p1;
 msg.msg[2] = p2;
 if (ERR(cx_ioctl(cx, IOCTL_TiNG_WRMSG,
	&msg, sizeof(struct ting_message))))
	return err(ERR_SM_DEVERR);
 return 0;
}

LOCALDEF int sendmsg4(CARDCONN *cx, U16 message, U32 p1, U32 p2, U32 p3)
{
 struct ting_message msg;
 msg.ioctl_length = sizeof(msg);
 msg.flags = 0;
 msg.msg[0] = MESSAGE(message, 4);
 msg.msg[1] = p1;
 msg.msg[2] = p2;
 msg.msg[3] = p3;
 if (ERR(cx_ioctl(cx, IOCTL_TiNG_WRMSG,
	&msg, sizeof(struct ting_message))))
	return err(ERR_SM_DEVERR);
 return 0;
}

	// must be in same order as enum dettype
static unsigned long detcode[] = {
	SUBTASKTC_GRUNT,
	SUBTASKTC_TONEDET, // Normal
	SUBTASKTC_TONEDET, // Beep
	SUBTASKTC_TONEDET, // catsig with tone reset
	SUBTASKTC_TONEREP,
	SUBTASKTC_CONFIN,
	SUBTASKTC_CIVIN,
	SUBTASKTC_IWRDET,
	SUBTASKTC_ANSDET,
	SUBTASKTC_ANSDETTONE,
	SUBTASKTC_ANSDETPLUGIN,
	SUBTASKTC_ANSAMDET,
	SUBTASKTC_BEEPDET,
	SUBTASKTC_ONHOOKDET,
};

LOCALDEF int startdet(tSMChannelId chan, unsigned hand, enum dettype dettype, int isrec)
{
 int sts = 0;
 if (!isrec && !chan->recog[isrec].detmask) {
	RAWCHAN *rcp = &chan->chan[hand];
	if (!cardconn_valid(&rcp->cx)) sts = open_channel(rcp, chan->mod);
	if (sts) return sts;
	sts = starttask(&rcp->cx, TASKTC_INCHAN, 0);
	if (sts) return sts;
	sts = chan->mod->card->vtbl->engage_df(chan->df[1], &rcp->cx, DFC_IN);
	if (sts) return sts;
	if (chan->ecloopback) {
		sts = setfriend(&chan->chan[hand].cx, &chan->chan[HANDLE_ECHO], FRIEND_TYPE_ECHOMAIN);
		if (sts) return sts;
	}
	sts=engage_event(chan, EVENT_NOTIFY, hand);
	if (sts) return sts;
 }
 if (!sts && ~chan->recog[isrec].detmask & (1<< dettype)) {
	if ( !(TONEDET_DETMASK & (1<<dettype)) || !(chan->recog[isrec].detmask & TONEDET_DETMASK) ) {
		// not a tonedet, or tonedet not already added
		sts = sendmsg2(&chan->chan[hand].cx, MSG_ADDSUB, detcode[dettype]);
	}
	chan->recog[isrec].detmask |= 1<<dettype;
 }
 return sts;
}

LOCALDEF int stopdet(tSMChannelId chan, unsigned hand, enum dettype dettype, int isrec)
{
 if (chan->recog[isrec].detmask & (1<<dettype)) {
	chan->recog[isrec].detmask &= ~(1<<dettype);
	if ( (TONEDET_DETMASK & (1<<dettype)) && (chan->recog[isrec].detmask & TONEDET_DETMASK) ) {
		// removing one of the tonedet types, but tonedet is still needed
		return 0;
	}
	if (!isrec && !chan->recog[isrec].detmask) {
		int sts;
		if (chan->event[EVENT_NOTIFY].nonidle) {
			sts = disengage_event(chan, EVENT_NOTIFY, hand);
			if (sts) return sts;
		}
		sts = starttask(&chan->chan[hand].cx, 0, 0);
		return sts;
	} else {
		return sendmsg2(&chan->chan[hand].cx,
			MSG_DELSUB, detcode[dettype]);
	}
 }
 return 0;
}

LOCALDEF int rdmsg(struct ting_message *msg, CARDCONN *cx)
{
 msg->ioctl_length = sizeof(*msg);
 if (ERR(cx_ioctl(cx, IOCTL_TiNG_RDMSG, msg, sizeof(*msg))))
	return err(ERR_SM_DEVERR);
 if (TiNGtrace > 1) showmsg(&msg->msg[0]);
 return 0;
}

LOCALDEF int tst2compand(enum TiNG_COMPAND_TYPE *ct, enum kSMTimeslotType tt)
{
 switch (tt) {
 case kSMTimeslotTypeALaw: *ct = TiNG_TSTYPE_COMPAND_ALAW; break;
 case kSMTimeslotTypeMuLaw: *ct = TiNG_TSTYPE_COMPAND_uLAW; break;
 case kSMTimeslotTypeData: *ct = TiNG_TSTYPE_NONE; break;
 default: return err(ERR_SM_BAD_PARAMETER);
 }
 return 0;
}

LOCALDEF int compand2tst(enum kSMTimeslotType *tt, enum TiNG_COMPAND_TYPE ct)
{
 switch (ct) {
 case TiNG_TSTYPE_COMPAND_ALAW: *tt = kSMTimeslotTypeALaw; break;
 case TiNG_TSTYPE_COMPAND_uLAW: *tt = kSMTimeslotTypeMuLaw; break;
 case TiNG_TSTYPE_NONE: *tt = kSMTimeslotTypeData; break;
 default: return err(ERR_SM_BAD_PARAMETER);
 }
 return 0;
}

LOCALDEF U32 roundfu(float f)
{
 return (U32) ((S32)floor(f + 0.5));
}

