#include "smgroove.h"
#include "../TiNG.h"
#include "TiNGcommon.h"
#include "TiNGevent.h"
#include "TiNGo_mod.h"
#include "trace.h"
#include "../hsif/kernel.h"

#include <stdarg.h>
#include <string.h>

#ifdef TiNGTYPE_LINUX
#include <poll.h>
#endif
#ifdef TiNGTYPE_QNX
#include <sys/poll.h>
#endif

#define TST_IDLE 0 
#define TST_STARTING 1 
#define TST_STARTED 2 

struct tSMGroove_data {
	CARDCONN cx;
	struct module *mod;
	tSMEventId ev;
};

STATIC int groove_starttask(tSMGroove *gp, uint32_t typecode, uint32_t ver)
{
 CARDCONN *cx = &gp->gdp->cx;
 struct ting_task task;
 task.ioctl_length = sizeof(task);
 task.tasktypecode = typecode;
 task.version = ver;
 if (ERR(cx_ioctl(cx, IOCTL_TiNG_WRTASK, &task, sizeof(task))))
	return err(ERR_SM_DEVERR);
 return 0;
}

STATIC int groove_read(tSMGroove *gp, void *buf, unsigned rqlen, unsigned *donelen)
{
 CARDCONN *cx = &gp->gdp->cx;
 if (ERR(cx_read(cx, buf, rqlen, donelen))) return err(ERR_SM_DEVERR);
 return 0;
}

STATIC int groove_write(tSMGroove *gp, void *buf, unsigned rqlen, unsigned *donelen)
{
 CARDCONN *cx = &gp->gdp->cx;
 if (ERR(cx_write(cx, buf, rqlen, donelen))) return err(ERR_SM_DEVERR);
 return 0;
}

STATIC CARDCONN *groove_cardconn(tSMGroove *gp)
{
 CARDCONN *cx = &gp->gdp->cx;
 return cx;
}

STATIC int groove_setfriendcx(tSMGroove *gp, CARDCONN *fr, unsigned type)
{
 struct ting_destination dest;
 struct ting_friend friend;
 CARDCONN *cx = &gp->gdp->cx;
 dest.ioctl_length = sizeof(dest);
 if (fr) {
	if (!cardconn_valid(fr)) return 0;
	if (ERR(cx_ioctl(fr, IOCTL_TiNG_RDDEST, &dest, sizeof(struct ting_destination)))) {
		return err(ERR_SM_DEVERR);
	}
	friend.channo = dest.channo;
	friend.magic = dest.magic;
 } else {
	friend.channo = 0;
	friend.magic = 0;
 }
 friend.ioctl_length = sizeof(friend);
 friend.type = type;
 if (!cardconn_valid(cx)) return err(ERR_SM_DEVERR);
 if (ERR(cx_ioctl(cx,
	IOCTL_TiNG_WRFRIEND, &friend, sizeof(struct ting_friend))))
	return err(ERR_SM_DEVERR);
 return 0;
}

STATIC int groove_setfriend(tSMGroove *gp, tSMGroove *gf, unsigned type)
{
 CARDCONN *fr = gf ? &gf->gdp->cx : 0;
 return groove_setfriendcx(gp, fr, type);
}

STATIC int groove_sendmsg(tSMGroove *gp, uint32_t msg, ...)
{
 CARDCONN *cx = &gp->gdp->cx;
 struct ting_message m;
 unsigned mlen = MSG2CODE(msg) & 0xf;
 unsigned u;
 va_list ap;
 if (!mlen) mlen = 17; //was 16
 m.flags = 0;
 m.msg[0] = msg;
 va_start(ap, msg);
 for (u=1; u < mlen; u++) {
 	uint32_t v = va_arg(ap, uint32_t);
	m.msg[u] = v;
 }
 va_end(ap);
 m.ioctl_length = sizeof(m);
 if (ERR(cx_ioctl(cx, IOCTL_TiNG_WRMSG, &m, sizeof(m)))) return err(ERR_SM_DEVERR);
 return 0;
}

STATIC int groove_sendmsgstring(tSMGroove *gp, uint32_t id, uint32_t slen, char *s)
{
 CARDCONN *cx = &gp->gdp->cx;
 if (ERR(cx_msgstring(cx, id, s, slen))) return err(ERR_SM_DEVERR);
 return 0;
}

STATIC int groove_rdmsg_ex(tSMGroove *gp, uint32_t msg[16], int flags, uint32_t* cap, unsigned* has_msg)
{
 CARDCONN *cx = &gp->gdp->cx;
 struct ting_message m;
 unsigned u;
 m.ioctl_length = sizeof(m);
 m.flags = (flags & SMGROOVE_RDMSG_FLAG_NOWAIT) ? IOCTL_TiNG_RDMSG_FLAG_NOWAIT : 0;
 if (flags & SMGROOVE_RDMSG_FLAG_KEEPSTRINGS) m.flags |= IOCTL_TiNG_RDMSG_FLAG_KEEPSTRINGS;
 if (ERR(cx_ioctl(cx, IOCTL_TiNG_RDMSG, &m, sizeof(m)))) {
 	return err(ERR_SM_DEVERR);
 }
 if (TiNGtrace > 1) showmsg(&m.msg[0]);
 for (u=0; u < 16; u++) {
	msg[u] = m.msg[u];
 }
 *cap = m.capacity;
 *has_msg = m.hasmsg;
 return 0;
}

STATIC int groove_rdmsg(tSMGroove *gp, uint32_t msg[16], int flags)
{
 uint32_t cap;
 unsigned has_msg;
 return groove_rdmsg_ex(gp, msg, flags, &cap, &has_msg);
}

STATIC int groove_rdmsgstring(tSMGroove *gp, uint32_t id, uint32_t id_mask, uint32_t maxlen, char *s, uint32_t *len)
{
 CARDCONN *cx = &gp->gdp->cx;
 if (ERR(cx_rdmsgstring(cx, id, id_mask, maxlen, s, len))) {
 	return err(ERR_SM_DEVERR);
 }
 return 0;
}

STATIC int groove_getev(tSMGroove *gp, tSMEventId *ep)
{
 *ep = gp->gdp->ev;
 return 0;
}

STATIC int groove_get_df(tSMGroove *gp, DATAFEED df, struct module *module)
{
 return gp->gdp->mod->card->vtbl->get_df(df, module, &gp->gdp->cx);
}

STATIC int groove_unbind_df(tSMGroove *gp, DATAFEED df)
{
 return gp->gdp->mod->card->vtbl->unbind_df(df, &gp->gdp->cx);
}

STATIC int groove_engage_df(tSMGroove *gp, DATAFEED df, enum dfconntyp type)
{
 return gp->gdp->mod->card->vtbl->engage_df(df, &gp->gdp->cx, type);
}

STATIC int groove_connect_df(tSMGroove *gp, DATAFEED *targ, DATAFEED df)
{
 return gp->gdp->mod->card->vtbl->connect_df(targ, df);
}

STATIC void groove_dtor(tSMGroove *gp)
{
 tSMEventId *ep = &gp->gdp->ev;
#ifdef TiNGTYPE_LINUX
 ep->eip->smd_ev_free(*ep);
 free(ep->eip);
#endif
#ifdef TiNGTYPE_QNX
 ep->eip->smd_ev_free(*ep);
 free(ep->eip);
#endif
#ifdef TiNGTYPE_WINNT
 if (!CloseHandle(*ep)) {
	ERRCODE e;
	e.text = "CloseHandle failed";
	e.errnum = GetLastError();
	if (TiNGtrace) printerr(e);
 }
#endif
 cx_close(&gp->gdp->cx);
 if (TiNGtrace > 2) olog("[groove dtor %p]\n", gp);
 free(gp->gdp);
}

static const tSMGroove gtemplate = {
	groove_starttask,
	groove_read,
	groove_write,
	groove_sendmsg,
	groove_sendmsgstring,
	groove_rdmsg,
	groove_rdmsg_ex,
	groove_rdmsgstring,
	groove_getev,
	groove_get_df,
	groove_unbind_df,
	groove_engage_df,
	groove_connect_df,
	groove_cardconn,
	groove_setfriend,
	groove_setfriendcx,
	groove_dtor,
	0,
};

STATIC int mkevent(tSMGroove *gp)
{
 int r;
 tSMEventId *ep = &gp->gdp->ev;
#ifdef TiNGTYPE_LINUX
 ep->eip = alloc(0, sizeof(struct tSMEventId_internals));
 if (!ep->eip) return err(ERR_SM_NO_RESOURCES);
 ep->eip->evbind = kSMChannelSpecificEvent;
#endif
#ifdef TiNGTYPE_QNX
 ep->eip = alloc(0, sizeof(struct tSMEventId_internals));
 if (!ep->eip) return err(ERR_SM_NO_RESOURCES);
 ep->eip->evbind = kSMChannelSpecificEvent;
#endif
 r = gp->gdp->mod->card->vtbl->smd_evcx_create(ep, &gp->gdp->cx);
 if (r) {
#ifdef TiNGTYPE_LINUX
 	free(ep->eip);
 	ep->eip = 0;
#endif
#ifdef TiNGTYPE_QNX
 	free(ep->eip);
 	ep->eip = 0;
#endif
	return r;
 }
 return r;
}

int SMGroove_module(tSMGroove *gp, struct module *mod)
{
 int r = 0;
 *gp = gtemplate;
 gp->gdp = alloc(0, sizeof(*gp->gdp));
 if (!gp->gdp) {
 	r = err(ERR_SM_NO_RESOURCES);
 } else {
	struct ting_destination dst;
	dst.ioctl_length = sizeof(dst);
	dst.dest = TiNG_DEST_CHANNEL;
	dst.modmask = mod->modmask;
	gp->gdp->mod = mod;
	if (ERR(cx_clone(&gp->gdp->cx, &mod->cx))) r = err(ERR_SM_DEVERR);
	else {
		if (ERR(cx_ioctl(&gp->gdp->cx, IOCTL_TiNG_WRDEST, &dst, sizeof(dst)))) {
			r = err(ERR_SM_DEVERR);
		} else r = mkevent(gp);
		if (r) cx_close(&gp->gdp->cx);
	}
	if (r) free(gp->gdp);
 }
 if (TiNGtrace > 2) olog("[groove ctor %p]\n", gp);
 return r;
}

ACUAPI int ACUTiNG_WINAPI SMGroove(tSMGroove *gp, tSMModuleId mod)
{
 struct module *mp = modid2lockedmodule(mod);
 int r;
 if (!mp) return err(ERR_SM_NO_SUCH_MODULE);
 r = SMGroove_module(gp, mp);
 unlock_module(mp);
 return r;
}
