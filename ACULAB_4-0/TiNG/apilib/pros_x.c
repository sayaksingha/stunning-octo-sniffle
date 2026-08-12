#include "pros_x.h"
#include "cardconn_px.h"
#include "event.h"
#include "globaltime.h"
#include "mutx.h"
#include "refcnt.h"
#include "TiNGcommon.h"
#include "TiNGevent.h"
#include "smgroove.h"
#include "timestamp.h"
#include "trace.h"
#define PROVIDE_TRACE
#include "../assp/assp_packet.h"
#include "../assp/randent.h"
#include "../hsif/kernel.h"
#include "../hsif/typecode.h"
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#define arlen(x) (sizeof(x)/sizeof(*(x)))

#ifdef TiNG_EVENTS_ARE_POSIX_PIPES
#define USE_POLL_EVENT
#include <sys/poll.h>
#endif

typedef struct {
	EVNT evnt;
} PX_EVENT_VTBL_DATA;

typedef struct {
	REFCNT ref;
		// reserved value for 'pos' field
#define SERTS_NONE ~0u
	int cf;	// datafeed was created implicitly by sm_switch_channel_*()
	int direct;
	struct {
		unsigned pos;
		enum TiNG_COMPAND_TYPE type;
	} ser_ts;
	CARDCONN cx;	// to datafeed task
} PX_DATAFEED_VTBL_DATA;

typedef struct {
	EVNT linkedevent[EVENT_NUM];
} PX_CHAN_VTBL_DATA;

#define PXSTTS_TO_SERTS(d, s, t) ((((s)&1)<<7) | (t) | ((d)<<8) | ((s)<<16))

#define PX_SERTS_TO_STREAM(s) ((s) >> 7)
#define PX_SERTS_TO_TS(s) ((s) & 0x7f)

LOCALDEF const char *version = "@(#)$from: " __FILE__ " "
#include "../gen/version.i"
	" $";

STATIC int taskstart(CARDCONN *cx, unsigned long typecode, unsigned long ver)
{
 struct ting_task task;
 task.ioctl_length = sizeof(task);
 task.tasktypecode = typecode;
 task.version = ver;
 if (ERR(cx_ioctl(cx, IOCTL_TiNG_WRTASK, &task, sizeof(task))))
	return err(ERR_SM_DEVERR);
 return 0;
}

STATIC int open_cardconn(CARDCONN *cx, struct module *mp)
{
 struct ting_destination dst;
 dst.ioctl_length = sizeof(dst);
 dst.dest = TiNG_DEST_CHANNEL;
 dst.modmask = mp->modmask;
 if (ERR(cx_clone(cx, &mp->cx))) return err(ERR_SM_DEVERR);
 if (ERR(cx_ioctl(cx, IOCTL_TiNG_WRDEST, &dst, sizeof(dst)))) {
	cx_close(cx);
	return err(ERR_SM_DEVERR);
 }
 return 0;
}

STATIC int px_setfriend(CARDCONN *cx, CARDCONN *fr, unsigned type)
{
 struct ting_destination dest;
 struct ting_friend friend;
 U32 modmask;
 dest.ioctl_length = sizeof(dest);
 if (fr) {
	if (!cardconn_valid(fr)) return 0;
	if (ERR(cx_ioctl(fr, IOCTL_TiNG_RDDEST, &dest, sizeof(struct ting_destination)))) {
		return err(ERR_SM_DEVERR);
	}
	friend.channo = dest.channo;
	friend.magic = dest.magic;
	modmask = dest.modmask;
	if (ERR(cx_ioctl(cx, IOCTL_TiNG_RDDEST, &dest, sizeof(struct ting_destination)))) {
		return err(ERR_SM_DEVERR);
	}
	if (modmask != dest.modmask) return err(ERR_SM_NOT_SAME_MODULE);
 } else {
	friend.channo = 0;
	friend.magic = 0;
 }
 friend.ioctl_length = sizeof(friend);
 friend.type = type;
 if (ERR(cx_ioctl(cx,
	IOCTL_TiNG_WRFRIEND, &friend, sizeof(struct ting_friend))))
	return err(ERR_SM_DEVERR);
 return 0;
}

STATIC int px_linkevent(tSMChannelId chan, enum event_types etyp, tSMEventId ev)
{
 PX_CHAN_VTBL_DATA *psvp = chan->vtbl_data;
#ifdef TiNG_EVENTS_ARE_POSIX_PIPES
 PX_EVENT_VTBL_DATA *evdp = ev.eip->vtbl_data;
 return evnt_assign(&psvp->linkedevent[etyp], evdp->evnt);
#endif
#ifdef TiNGTYPE_WINNT
 EVNT evn;
 evn.evh = ev;
 return evnt_assign(&psvp->linkedevent[etyp], evn);
#endif
}

STATIC int px_unlinkevent(tSMChannelId chan, enum event_types etyp)
{
 PX_CHAN_VTBL_DATA *psvp = chan->vtbl_data;
 evnt_destroy(&psvp->linkedevent[etyp]);
 return 0;
}

static const struct card_vtbl card_px_vtbl;
static const struct card_vtbl card_psv3_vtbl;
STATIC int px_card_ctor(struct card *card, struct ting_cardinfo *cip)
{
 switch (cip->cardtype) {
 case TiNG_CARDTYPE_SOFT_S:
	card->vtbl = &card_psv3_vtbl;
	break;
 case TiNG_CARDTYPE_X:
	card->vtbl = &card_px_vtbl;
	break;
 default:
	// leave vtbl as passed in
	break;
 }
 card->vtbl_data = 0;
 return 0;
}

STATIC void free_px_dfd(void *p)
{
 PX_DATAFEED_VTBL_DATA *dfd = p;
 if (cardconn_valid(&dfd->cx)) {
	taskstart(&dfd->cx, 0, 0);
	cx_close(&dfd->cx);
 }
 free(dfd);
}

STATIC DATAFEED px_datafeed(void)
{
 PX_DATAFEED_VTBL_DATA *dfd = alloc(0, sizeof(*dfd));
 DATAFEED df;
 if (!dfd) return NULL_DATAFEED;
 refcnt(&dfd->ref, free_px_dfd, dfd);
 dfd->ser_ts.pos = SERTS_NONE;
 dfd->cf = 0;
 dfd->direct = 0;
 cardconn(&dfd->cx);
 df.vtbl_data = dfd;
 return df;
}

STATIC int px_datafeed_dtor(DATAFEED df)
{
 PX_DATAFEED_VTBL_DATA *dfd = df.vtbl_data;
 if (dfd) refcnt_dec(&dfd->ref);
 return 0;
}

STATIC int px_chan_ctor(tSMChannelId chan)
{
 PX_CHAN_VTBL_DATA *psvp = alloc(0, sizeof(*psvp));
 unsigned u;
 if (!psvp) return 1;
 chan->vtbl_data = psvp;
 for (u=0; u < arlen(psvp->linkedevent); u++) {
	evnt(&psvp->linkedevent[u]);
 }
 return 0;
}

STATIC int px_swstreamno(struct module *mod, unsigned pos)
{
 unsigned stream = PX_SERTS_TO_STREAM(pos);
 unsigned mpos = 0;
 unsigned i;
 stream &= 1;		// doesn't distinguish between input and output
 for (i=mod->modmask; i > 1; i>>=1) mpos++;
 if (mpos >= 4) return stream + 72 + 2 * mpos;
 return stream + 64 + 2 * mpos;
}

STATIC int ps_swstreamno(struct module *mod, unsigned pos)
{
 (void) mod;
 return pos >> 16;
}

STATIC int px_swtimeslot(struct module *mod, unsigned pos)
{
 (void) mod;
 return PX_SERTS_TO_TS(pos);
}

STATIC int px_engage_df(DATAFEED df, CARDCONN *cx, enum dfconntyp type)
{
 uint32_t msg;
 if (!cardconn_valid(cx)) return 0;
 switch (type) {
 case DFC_OUT: msg = FRIEND_TYPE_OUT_DATAFEED_RX; break;
 case DFC_IN: msg = FRIEND_TYPE_IN_DATAFEED_RX; break;
 case DFC_ECREF: msg = FRIEND_TYPE_ECREF_DATAFEED_RX; break;
 case DFC_BG: msg = FRIEND_TYPE_BG_DATAFEED_RX; break;
 case DFC_H223_H245_OUT: msg = FRIEND_TYPE_H223_DEMUX_H245_DATAFEED_RX; break;
 case DFC_H223_AUDIO_OUT: msg = FRIEND_TYPE_H223_DEMUX_AUDIO_DATAFEED_RX; break;
 case DFC_H223_VIDEO_OUT: msg = FRIEND_TYPE_H223_DEMUX_VIDEO_DATAFEED_RX; break;
 case DFC_H223_H245_IN: msg = FRIEND_TYPE_H223_MUX_H245_DATAFEED_RX; break;
 case DFC_H223_AUDIO_IN: msg = FRIEND_TYPE_H223_MUX_AUDIO_DATAFEED_RX; break;
 case DFC_H223_VIDEO_IN: msg = FRIEND_TYPE_H223_MUX_VIDEO_DATAFEED_RX; break;
 case DFC_I460_0_IN: msg = FRIEND_TYPE_I460_MUX_0_DATAFEED_RX; break;
 case DFC_I460_1_IN: msg = FRIEND_TYPE_I460_MUX_1_DATAFEED_RX; break;
 case DFC_I460_2_IN: msg = FRIEND_TYPE_I460_MUX_2_DATAFEED_RX; break;
 case DFC_I460_3_IN: msg = FRIEND_TYPE_I460_MUX_3_DATAFEED_RX; break;
 case DFC_I460_4_IN: msg = FRIEND_TYPE_I460_MUX_4_DATAFEED_RX; break;
 case DFC_I460_5_IN: msg = FRIEND_TYPE_I460_MUX_5_DATAFEED_RX; break;
 case DFC_I460_6_IN: msg = FRIEND_TYPE_I460_MUX_6_DATAFEED_RX; break;
 case DFC_I460_7_IN: msg = FRIEND_TYPE_I460_MUX_7_DATAFEED_RX; break;
 case DFC_REDIRECT_RX: msg = FRIEND_TYPE_REDIRECT_RX_DATAFEED_RX; break;
 case DFC_REDIRECT_TX: msg = FRIEND_TYPE_REDIRECT_TX_DATAFEED_RX; break;
 case DFC_FRAMER_RX_IN: msg = FRIEND_TYPE_FRAMER_RX_IN_DATAFEED_RX; break;
 case DFC_FRAMER_RX_OUT: msg = FRIEND_TYPE_FRAMER_RX_OUT_DATAFEED_RX; break;
 case DFC_FRAMER_TX_IN: msg = FRIEND_TYPE_FRAMER_TX_IN_DATAFEED_RX; break;
 case DFC_FRAMER_TX_OUT: msg = FRIEND_TYPE_FRAMER_TX_OUT_DATAFEED_RX; break;
 default:
	 return 0;
 }
 if (nonnull_datafeed(df)) {
	PX_DATAFEED_VTBL_DATA *dfd = df.vtbl_data;
		//calling engage before switching channel
	if (!cardconn_valid(&dfd->cx)) return 0;
	if (!dfd->direct) msg++;
	return px_setfriend(cx, &dfd->cx, msg);
 } else {
	return px_setfriend(cx, 0, msg);
 }
}

STATIC int px_setup_df(DATAFEED df, struct module *mod, int direct)
{
 PX_DATAFEED_VTBL_DATA *dfd = df.vtbl_data;
 if (!cardconn_valid(&dfd->cx)) {
	int sts = open_cardconn(&dfd->cx, mod);
	if (sts) return sts;
	sts = taskstart(&dfd->cx, direct ?  TASKTC_DATAFEEDRX : TASKTC_DATAFEEDTX , 0);
	if (!sts) cx_discardmsgs(&dfd->cx, 1);
	if (sts) {
		cx_close(&dfd->cx);
		cardconn(&dfd->cx);
		return sts;
	}
	dfd->direct = direct;
	dfd->cf = 0;
 }
 return 0;
}

STATIC int px_get_df(DATAFEED df, struct module* module, CARDCONN* cx)
{
 PX_DATAFEED_VTBL_DATA *dfd = df.vtbl_data;
 if (!cardconn_valid(&dfd->cx)) {
	int sts = px_setup_df(df, module, 0);
	if (sts) return sts;
		// if the channel has an output operation in
		// progress, connect it up
	if (cardconn_valid(cx)) {
		return px_engage_df(df, cx, DFC_OUT);
	}
 }
 return 0;
}

STATIC int px_unbind_df(DATAFEED df, CARDCONN* cx)
{
	(void) df;
	(void) cx;
	return 0;
}

STATIC int px_connect_df(DATAFEED *targ, DATAFEED df)
{
 PX_DATAFEED_VTBL_DATA *dfd = df.vtbl_data;
 if (dfd) refcnt_inc(&dfd->ref);	// do first in case old == new
 if (nonnull_datafeed(*targ)) {
	px_datafeed_dtor(*targ);
 }
 *targ = df;
 return 0;
}

STATIC int pxs_ts_info(tSMChannelId chan, int *stp, int *tsp, enum TiNG_COMPAND_TYPE *typp, int direct, int ps)
{
 DATAFEED *dfp = &chan->df[direct];
 *typp = kSMTimeslotTypeData;
 *stp = -1;
 *tsp = -1;
 if (nonnull_datafeed(*dfp)) {
	PX_DATAFEED_VTBL_DATA *dfd = dfp->vtbl_data;
	if (dfd->cf && dfd->ser_ts.pos != SERTS_NONE) {
		unsigned pos = dfd->ser_ts.pos;
		struct module *mod = chan->mod;
		if (!ps) {
			*stp = px_swstreamno(mod, pos);
		} else {
			*stp = ps_swstreamno(mod, pos);
		}
		*tsp = px_swtimeslot(mod, pos);
		*typp = dfd->ser_ts.type;
 	}
 }
 return 0;
}

STATIC int px_ts_info(tSMChannelId chan, int *stp, int *tsp, enum TiNG_COMPAND_TYPE *typp, int direct)
{
 return pxs_ts_info(chan, stp, tsp, typp, direct, 0);
}

STATIC int ps_ts_info(tSMChannelId chan, int *stp, int *tsp, enum TiNG_COMPAND_TYPE *typp, int direct)
{
 return pxs_ts_info(chan, stp, tsp, typp, direct, 1);
}

STATIC int px_tdm_assign(DATAFEED df, unsigned int stream, unsigned int timeslot, enum TiNG_COMPAND_TYPE type, int direct)
{
 PX_DATAFEED_VTBL_DATA *dfd;
 struct ting_message msg;
 unsigned comp = 0;
 if (nonnull_datafeed(df)) dfd = df.vtbl_data;
 else return err(ERR_SM_DEVERR);
 if (dfd->direct != direct) return err(ERR_SM_NOT_IMPLEMENTED); //TDM->TDM unsupported
 if (stream == (unsigned)-1) { 
	// assert SERTS_NONE == MSG_SET_TSC_DISCONNECT
	dfd->ser_ts.pos = SERTS_NONE;
 } else {
	if (stream < 64 || stream > 65535 || timeslot >= 128) return err(ERR_SM_BAD_PARAMETER);
	dfd->ser_ts.pos = PXSTTS_TO_SERTS(direct, stream, timeslot);
	dfd->ser_ts.type = type;
	switch (type) {
	case TiNG_TSTYPE_NONE: comp = 0; break;
	case TiNG_TSTYPE_COMPAND_ALAW: comp = MSG_SET_TSC_ALAW; break;
	case TiNG_TSTYPE_COMPAND_uLAW: comp = MSG_SET_TSC_MULAW; break;
	}
 }
		// FIXME: get status messages here
 msg.ioctl_length = sizeof(msg);
 msg.flags = 0;
 msg.msg[0] = MESSAGE(direct ?  MSG_SET_INTSC : MSG_SET_OUTTSC, 3);
 msg.msg[1] = dfd->ser_ts.pos;
 msg.msg[2] = comp;
 if (ERR(cx_ioctl(&dfd->cx, IOCTL_TiNG_WRMSG,
	&msg, sizeof(struct ting_message))))
	return err(ERR_SM_DEVERR);
 return 0;
}

STATIC int px_auto_assign(tSMChannelId chan, unsigned int st0, unsigned int st1, enum TiNG_COMPAND_TYPE type, int direct)
{
 (void) chan;
 (void) st0;
 (void) st1;
 (void) type; 
 (void) direct;
 return 0;
}

STATIC int px_manual_assign(tSMChannelId chan, unsigned int stream, unsigned int timeslot, enum TiNG_COMPAND_TYPE type, int direct)
{
 PX_DATAFEED_VTBL_DATA *dfd;
 struct ting_message msg;
 unsigned comp = 0;
 if (stream == (unsigned) -1) {
	px_datafeed_dtor(chan->df[direct]);
	chan->df[direct] = NULL_DATAFEED;
	return 0;
 }
 if (stream < 64 || stream > 65535 || timeslot >= 128) return err(ERR_SM_BAD_PARAMETER);
 	// we've got a valid stream, prepare the datafeed
 if (nonnull_datafeed(chan->df[direct])) {
	dfd = chan->df[direct].vtbl_data;
	if (!dfd->cf) {	// can only re-use datafeed if auto-generated
		px_datafeed_dtor(chan->df[direct]);
		chan->df[direct] = px_datafeed();
	}
 } else {
	chan->df[direct] = px_datafeed();
 }
 if (!nonnull_datafeed(chan->df[direct])) {
	return err(ERR_SM_OS_RESOURCE_PROBLEM);
 }
 	// now we have both the timeslot and the datafeed
 dfd = chan->df[direct].vtbl_data;
 dfd->cf = 1;
 if (!cardconn_valid(&dfd->cx)) {
	int sts = open_cardconn(&dfd->cx, chan->mod);
	if (sts) return sts;
	sts = taskstart(&dfd->cx,
		direct ? TASKTC_DATAFEEDRX : TASKTC_DATAFEEDTX, 0);
	if (!sts) cx_discardmsgs(&dfd->cx, 1);
	if (sts) {
		cx_close(&dfd->cx);
		cardconn(&dfd->cx);
		return sts;
	}
	dfd->direct = direct;
 }
 dfd->ser_ts.pos = PXSTTS_TO_SERTS(direct, stream, timeslot);
 dfd->ser_ts.type = type;
 switch (type) {
 case TiNG_TSTYPE_NONE: comp = 0; break;
 case TiNG_TSTYPE_COMPAND_ALAW: comp = 1; break;
 case TiNG_TSTYPE_COMPAND_uLAW: comp = 2; break;
 }
		// FIXME: get status messages here
 msg.ioctl_length = sizeof(msg);
 msg.flags = 0;
 msg.msg[0] = MESSAGE(direct ?  MSG_SET_INTSC : MSG_SET_OUTTSC, 3);
 msg.msg[1] = dfd->ser_ts.pos;
 msg.msg[2] = comp;
 if (ERR(cx_ioctl(&dfd->cx, IOCTL_TiNG_WRMSG,
	&msg, sizeof(struct ting_message))))
	return err(ERR_SM_DEVERR);
 return 0;
}


STATIC int px_smd_ev_free(tSMEventId ep)
{
#ifdef USE_POLL_EVENT
 PX_EVENT_VTBL_DATA *hep = ep.eip->vtbl_data;
 evnt_destroy(&hep->evnt);
 free(hep);
#endif
 return 0;
}

STATIC int px_event_create(tSMEventId *ep, EVNT *evp)
{
#ifdef USE_POLL_EVENT
 PX_EVENT_VTBL_DATA *hep;
#endif
 if (evnt_create(evp)) return err(ERR_SM_DEVERR);
#ifdef USE_POLL_EVENT
 ep->fd = evnt_to_listen(*evp);
 ep->mode = POLLIN | POLLPRI; 
 hep = alloc(0, sizeof(*hep));
 if (!hep) return 1;
 hep->evnt = *evp;
 ep->eip->vtbl_data = hep;
 ep->eip->smd_ev_free = px_smd_ev_free;
#endif
#ifdef TiNGTYPE_WINNT
 *ep = evnt_to_sig(*evp);
#endif
 return 0;
}

STATIC int px_event_engage(CARDCONN *cx, EVNT evn)
{
 struct ting_chevent ev;
 if (!evnt_valid(evn)) return 0;
#ifdef TiNG_EVENTS_ARE_POSIX_PIPES
 ev.event = evnt_to_listen(evn);
 ev.magic = evnt_to_sig(evn);
#endif
#ifdef TiNGTYPE_WINNT
 ev.evtype = 0;		// ignored - maybe use for something in future version
 ev.event = evnt_to_sig(evn);
#endif
 ev.ioctl_length = sizeof(ev);
 if (ERR(cx_ioctl(cx, IOCTL_TiNG_WRCHEVENT, &ev, sizeof(ev)))) return err(ERR_SM_DEVERR);
 return 0;
}

STATIC int px_smd_ev_create(tSMEventId *ep, tSMChannelId chan, enum event_types etyp, int ni)
{
 EVNT ev;
 (void) chan;
 (void) etyp;
 (void) ni;
 return px_event_create(ep, &ev);
}

STATIC int px_smd_evcx_create(tSMEventId *ep, CARDCONN *cx)
{
 EVNT ev;
 int e = px_event_create(ep, &ev);
 if (e) return e;
 return px_event_engage(cx, ev);
}

STATIC int px_smd_evmod_create(tSMEventId *ep, struct module *mp)
{
 EVNT ev, evcx;
 int sts;
#ifdef USE_POLL_EVENT
 PX_EVENT_VTBL_DATA *hep;
#endif
 evnt(&ev);
 if (ERR(cx_event(&mp->cx, &evcx))) return err(ERR_SM_DEVERR);
 sts = evnt_assign(&ev, evcx);
 if (!sts) {
#ifdef USE_POLL_EVENT
	hep = alloc(0, sizeof(*hep));
	if (!hep) {
		evnt_destroy(&ev);
		return err(ERR_SM_OS_RESOURCE_PROBLEM);
	}
	ep->fd = evnt_to_listen(ev);
	ep->mode = POLLIN | POLLPRI; 
	hep->evnt = ev;
	ep->eip->vtbl_data = hep;
	ep->eip->smd_ev_free = px_smd_ev_free;
#endif
#ifdef TiNGTYPE_WINNT
	*ep = evnt_to_sig(ev);
#endif
 }
 return sts;
}

STATIC int px_mod_sec(struct module *mp, int level, int keyId, void* context, tSMSecCallback encdec)
{
 if (cardconn_valid(&mp->cx)) cx_sec(&mp->cx,level,keyId,context,encdec);
 return 0;
}


STATIC int px_mod_shutdown(struct module *mp)
{
 if (cardconn_valid(&mp->cx)) cx_shutdown(&mp->cx);
 return 0;
}

STATIC int px_engage_event(tSMChannelId chan, enum event_types etyp, CARDCONN *cx)
{
 PX_CHAN_VTBL_DATA *psvp = chan->vtbl_data;
 if (!cardconn_valid(cx)) {
	return evnt_signal(psvp->linkedevent[etyp]);
 }
 return px_event_engage(cx, psvp->linkedevent[etyp]);
}

STATIC int px_disengage_event(tSMChannelId chan, enum event_types etyp, CARDCONN *cx)
{
 PX_CHAN_VTBL_DATA *psvp = chan->vtbl_data;
 struct ting_chevent ev;
 if (!evnt_valid(psvp->linkedevent[etyp])) return 0;
 ev.ioctl_length = sizeof(ev);
#ifdef TiNG_EVENTS_ARE_POSIX_PIPES
 (void) chan;
 (void) etyp;
 ev.event = -1;
 ev.magic = -1;
#endif
#ifdef TiNGTYPE_WINNT
 ev.evtype = 0;		// unused - for compatibility with future drivers
 ev.event = (HANDLE) -1;
#endif
 if (cardconn_valid(cx)) {
	if (ERR(cx_ioctl(cx, IOCTL_TiNG_WRCHEVENT, &ev, sizeof(ev)))) return err(ERR_SM_DEVERR);
 }
 evnt_clear(psvp->linkedevent[etyp]);
 return 0;
}

STATIC int px_chan_dtor(tSMChannelId chan)
{
 PX_CHAN_VTBL_DATA *psvp = chan->vtbl_data;
 unsigned u;
 for (u=0; u < arlen(psvp->linkedevent); u++) {
	evnt_destroy(&psvp->linkedevent[u]);
 }
 free(psvp);
 return 0;
}

STATIC int px_card_dtor(struct card *card)
{
 (void) card;
#ifdef TiNGTYPE_WINNT
 WSACleanup();
#endif
 return 0;
}

static const struct card_vtbl card_px_vtbl = {
	px_cardconn_open,
	px_linkevent,
	px_unlinkevent,
	px_card_ctor,
	px_chan_ctor,
	px_swstreamno,
	px_swtimeslot,
	px_datafeed,
	px_datafeed_dtor,
	px_setup_df,
	px_engage_df,
	px_get_df,
	px_unbind_df,
	px_connect_df,
	px_ts_info,
	px_tdm_assign,
	px_auto_assign,
	px_manual_assign,
	px_smd_ev_create,
	px_smd_evcx_create,
	px_smd_evmod_create,
	px_mod_shutdown, 
	px_mod_sec,
	px_engage_event,
	px_disengage_event,
	px_chan_dtor,
	px_card_dtor,
};

static const struct card_vtbl card_psv3_vtbl = {
	psv3_cardconn_open,
	px_linkevent,
	px_unlinkevent,
	px_card_ctor,
	px_chan_ctor,
	ps_swstreamno,
	px_swtimeslot,
	px_datafeed,
	px_datafeed_dtor,
	px_setup_df,
	px_engage_df,
	px_get_df,
	px_unbind_df,
	px_connect_df,
	ps_ts_info,
	px_tdm_assign,
	px_auto_assign,
	px_manual_assign,
	px_smd_ev_create,
	px_smd_evcx_create,
	px_smd_evmod_create,
	px_mod_shutdown,
	px_mod_sec,
	px_engage_event,
	px_disengage_event,
	px_chan_dtor,
	px_card_dtor,
};

STATIC int forward_trace(const char *fmt, va_list ap)
{
 return TiNG_showtrace(fmt, ap);
}

void assp_trace(int trace_level);
extern int (*assp_tracefn)(const char *fmt, va_list ap);

STATIC int pros_x_init(void)
{
#include "gen/checkcore_decl.i"
 int v =
#include "gen/checkcore_call.i"
 if (TiNGtrace && v) olog("%s\n(TiNG: using modified core)\n", version);
 assp_tracefn = forward_trace;
#ifndef ASSP_SHARE_TINGTRACE
 if (TiNGtrace & 0xfffff0) assp_trace(0xfffff & (TiNGtrace >> 4));
 TiNGtrace &= 0xf;
#endif
 randent_init();
 return 0;
}

STATIC RUN_ONCE px_once = RUN_ONCE_INITIALISER(pros_x_init);

STATIC int do_sm_open_prosody_x(SM_OPEN_PROSODY_X_PARMS *openp, const struct card_vtbl* pvtbl);

LOCALDEF int real_sm_open_prosody_x(SM_OPEN_PROSODY_X_PARMS *openp)
{
 return do_sm_open_prosody_x(openp, &card_px_vtbl);
}

STATIC int do_sm_open_prosody_x(SM_OPEN_PROSODY_X_PARMS *openp, const struct card_vtbl* pvtbl)
{
 struct card *cardp;
 int sts = run_once(&px_once);
 if (sts) return err(ERR_SM_OS_OTHER_PROBLEM);
#ifdef TiNGTYPE_WINNT
 {
  WSADATA wsadata;
  int e = WSAStartup(MAKEWORD(2, 0), &wsadata);
  if (e) {
	if (TiNGtrace) olog("WSAStartup failed: %d\n", e);
	return 1;
  }
 }
#endif
 cardp = aculab_TiNG_card_init(pvtbl, openp->px_locator);
 if (!cardp) return err(ERR_SM_DEVERR);
 sts = aculab_TiNG_cardmap_add(&aculab_TiNG_cardmap, openp->card_id, cardp);
 if (sts) cardp->dtor(cardp);
 return sts;
}

LOCALDEF int real_sm_open_prosody_s_v3(SM_OPEN_PROSODY_S_V3_PARMS *openp)
{
	SM_OPEN_PROSODY_X_PARMS px;
	memset(&px,0,sizeof(px));
	px.px_locator = openp->ps_locator;
	px.card_id = openp->card_id;
	return do_sm_open_prosody_x(&px, &card_psv3_vtbl);
}




#include "../pubdoc/gen/pros_x.i"
