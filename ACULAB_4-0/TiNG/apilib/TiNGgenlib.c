/* TiNGgenlib.i - generic library */

#include "smdrvr.h"
#include <string.h>
#include <stdio.h>
#include "TiNGanyevent.h"
#include "TiNGevent.h"
#include "TiNGo_cardmap.h"
#include "TiNGo_coll.h"
#include "TiNGo_disp.h"
#include "TiNGo_mod.h"
#include "smgroove.h"
#include "TiNGcommon.h"
#include "TiNGo_tdmrx.h"
#include "TiNGo_tdmtx.h"
#include "TiNGo_datafeed.h"
#include "timestamp.h"
#include "trace.h"
#include "../hsif/kernel.h"
#include "../hsif/subtask.h"
#include "../hsif/typecode.h"
#include "../hsif/vmp.h"
#include "../hsif/ipv6.h"

#ifdef TiNG_EVENTS_ARE_POSIX_PIPES
#include <sys/poll.h>
#include <unistd.h>
#endif

#define arlen(x) (sizeof(x)/sizeof(*(x)))

STATIC int real_sm_reset_module(tSMModuleId m)
{
 struct module *mp = modid2lockedmodule(m);
 struct ting_mstate ms;
 CARDCONN mod;
 int sts = 0;
 if (!mp) return err(ERR_SM_NO_SUCH_MODULE);
 if (ERR(open_module(&mod, mp->card, mp->modmask))) {
 	sts =  err(ERR_SM_DEVERR);
 } else {
	ms.ioctl_length = sizeof(ms);
	ms.mstate = MS_RESET;
	if (ERR(cx_ioctl(&mod, IOCTL_TiNG_WRMSTATE,
		&ms, sizeof(ms)))) {
		sts = err(ERR_SM_DEVERR);
	}
	if (ERR(cx_close(&mod)) && !sts) sts = err(ERR_SM_DEVERR);
 }
 unlock_module(mp);
 return sts;
}



STATIC int real_sm_get_module_info(struct sm_module_info_parms *pp)
{
 struct module *mp = modid2lockedmodule(pp->module);
 struct ting_mstate ms;
 struct ting_modinfo mi;
 CARDCONN mod;
 int sts = 0;
 if (!mp) return err(ERR_SM_NO_SUCH_MODULE);
 if (ERR(open_module(&mod, mp->card, mp->modmask))) {
	sts = err(ERR_SM_DEVERR);
 } else {
	pp->firmware_id = mp->fwid;
	ms.ioctl_length = sizeof(ms);
	if (ERR(cx_ioctl(&mod, IOCTL_TiNG_RDMSTATE, &ms, sizeof(ms)))) {
		sts = err(ERR_SM_DEVERR);
	} else pp->firmware_running = ms.mstate == MS_RUNNING;
	mi.ioctl_length = sizeof(mi);
	mi.modmask = mp->modmask;
	if (ERR(cx_ioctl(&mod, IOCTL_TiNG_RDMODINFO, &mi, sizeof(mi)))) {
		sts = err(ERR_SM_DEVERR);
	} else if (mi.modfirststream == 0) {
		pp->module_slot = 0;
		pp->min_stream    = 0;
		pp->stream_count  = 0;
	} else {
		pp->module_slot = mi.modslot;
		pp->min_stream    = mi.modfirststream;
		pp->stream_count  = 1+(mi.modlaststream - mi.modfirststream);
	}
	if (ms.avgcpuload == ~0u) {
		pp->average_loading = -1;
	} else {
		pp->average_loading = ms.avgcpuload * 100 / 320000;
	}
	if (ms.maxcpuload  == ~0u) {
		pp->max_loading = -1;
	} else {
		pp->max_loading = ms.maxcpuload * 100 / 320000;
	}
	if (ERR(cx_close(&mod)) && !sts) sts = err(ERR_SM_DEVERR);
 }
 unlock_module(mp);
 return sts;
}

STATIC int real_sm_get_stream_info(struct sm_stream_info_parms *pp)
{
 struct module *mp = modid2lockedmodule(pp->module);
 struct ting_modinfo mi;
 struct ting_strinfo si;
 CARDCONN mod;
 int sts = 0;
 if (!mp) return err(ERR_SM_NO_SUCH_MODULE);
 if (ERR(open_module(&mod, mp->card, mp->modmask))) {
	sts = err(ERR_SM_DEVERR);
 } else {
	mi.ioctl_length = sizeof(mi);
	mi.modmask = mp->modmask;
	if (ERR(cx_ioctl(&mod, IOCTL_TiNG_RDMODINFO, &mi, sizeof(mi)))) {
		sts = err(ERR_SM_DEVERR);
	} else {
		si.ioctl_length = sizeof(si);
		si.modstrix = pp->stream - mi.modfirststream;
		if (ERR(cx_ioctl(&mod, IOCTL_TiNG_RDSTRINFO, &si, sizeof(si)))) {
			sts = err(ERR_SM_NO_SUCH_STREAM);
		} else {
			pp->serial_port_ix = si.serialportix;
			pp->timeslot_count = si.timeslots;
			strcpy(&pp->serial_no[0],&si.tdmcard[0]);
		}
	}
	if (ERR(cx_close(&mod)) && !sts) sts = err(ERR_SM_DEVERR);
 }
 unlock_module(mp);
 return sts;
}

STATIC int real_sm_get_firmware_info(struct sm_fwinfo_parms *fp)
{
 struct module *mp = modid2lockedmodule(fp->module);
 CARDCONN mod;
 int sts = 0;
 if (!mp) return err(ERR_SM_NO_SUCH_MODULE);
 if (ERR(open_module(&mod, mp->card, mp->modmask))) {
 	sts = err(ERR_SM_DEVERR);
 } else {
	struct ting_taskitem ti;
	ti.slot = fp->position++;
	ti.ioctl_length = sizeof(ti);
	if (ERR(cx_ioctl(&mod, IOCTL_TiNG_RDTASKLIST,
		&ti, sizeof(ti)))) {
		sts = err(ERR_SM_DEVERR);
	} else {
		fp->tasktypecode = ti.tasktypecode;
		fp->version = ti.version;
		if (!ti.tasktypecode) fp->position = 0;
	}
	if (ERR(cx_close(&mod)) && !sts) sts = err(ERR_SM_DEVERR);
 }
 unlock_module(mp);
 return sts;
}

STATIC int real_sm_config_module_switching(struct sm_config_module_sw_parms *sp)
{
 enum TiNG_COMPAND_TYPE out_type = TiNG_TSTYPE_NONE;
 enum TiNG_COMPAND_TYPE in_type = TiNG_TSTYPE_NONE;
 struct module *mp;
 if (sp->auto_assign_in_ts) {
	int sts = tst2compand(&in_type, sp->in_type);
	if (sts) return sts;
 }
 if (sp->auto_assign_out_ts) {
	int sts = tst2compand(&out_type, sp->out_type);
	if (sts) return sts;
 }
 mp = modid2lockedmodule(sp->module);
 if (!mp) return err(ERR_SM_NO_SUCH_MODULE);
 if (sp->auto_assign_out_ts) {
	mp->out_st0 = sp->out_st0;
	mp->out_st1 = sp->out_st1;
	mp->out_type = out_type; 
 } else {
	mp->out_st0 = STREAM_NONE;
	mp->out_st1 = STREAM_NONE;
	mp->out_type = TiNG_TSTYPE_COMPAND_ALAW; 
 }
 if (sp->auto_assign_in_ts) {
	mp->in_st0 = sp->in_st0;
	mp->in_st1 = sp->in_st1;
	mp->in_type = in_type; 
 } else {
	mp->in_st0 = STREAM_NONE;
	mp->in_st1 = STREAM_NONE;
	mp->in_type = TiNG_TSTYPE_COMPAND_ALAW; 
 }
 unlock_module(mp);
 return 0;
}

STATIC int channel_alloc_mod(tSMChannelId *cpp, int type, struct module *mod)
{
 tSMChannelId cp;
 unsigned int i;
 cp = init_chan(mod);
 *cpp = 0;
 if (!cp) return err(ERR_SM_NO_RESOURCES);
 cp->type = type;
 cp->caps_mask = 0;
 i = 0;
 switch (type) {
 case kSMChannelTypeInput:
	i = open_channel(&cp->chan[HANDLE_READ], mod);
 	break;
 case kSMChannelTypeOutput:
	i = open_channel(&cp->chan[HANDLE_WRITE], mod);
 	break;
 case kSMChannelTypeHalfDuplex:
	i = open_channel(&cp->chan[HANDLE_WRITE], mod);
	cp->chan[HANDLE_READ] = cp->chan[HANDLE_WRITE];
 	break;
 case kSMChannelTypeFullDuplex:
	i = open_channel(&cp->chan[HANDLE_READ], mod);
	if (!i) i = open_channel(&cp->chan[HANDLE_WRITE], mod);
 	break;
 default:
	i = err(ERR_SM_BAD_PARAMETER);
	break;
 }
 if (i) {
 	delete_channel(cp);
 	return i;
 }
#ifndef OMIT_ANYCHAN
 if (type != kSMChannelTypeOutput) {
#ifdef TiNG_EVENTS_ARE_POSIX_PIPES
	EVNT ev;
	cx_event(&cp->chan[HANDLE_READ].cx, &ev);
	cp->anyev[EVENT_READ].intev.fd = evnt_to_listen(ev);
	cp->anyev[EVENT_READ].intev.mode = POLLIN|POLLPRI;
#endif
 }
 if (type != kSMChannelTypeInput) {
#ifdef TiNG_EVENTS_ARE_POSIX_PIPES
	EVNT ev;
	cx_event(&cp->chan[HANDLE_WRITE].cx, &ev);
	cp->anyev[EVENT_WRITE].intev.fd = evnt_to_listen(ev);
	cp->anyev[EVENT_WRITE].intev.mode = POLLOUT|POLLPRI;
#endif
 }
#endif
 switch (type) {
 case kSMChannelTypeInput:
 case kSMChannelTypeHalfDuplex:
 case kSMChannelTypeFullDuplex:
	cp->rxtx[1].func = RXTXF_NONE;
	if (cp->mod->in_st0 != STREAM_NONE
		&& auto_assign(cp,
			cp->mod->in_st0,
			cp->mod->in_st1,
			cp->mod->in_type,
			1)) {
		delete_channel(cp);
		return err(ERR_SM_DEVERR);
	}
 }
 switch (type) {
 case kSMChannelTypeOutput:
 case kSMChannelTypeHalfDuplex:
 case kSMChannelTypeFullDuplex:
	cp->rxtx[0].func = RXTXF_NONE;
	if (cp->mod->out_st0 != STREAM_NONE
		&& auto_assign(cp,
			cp->mod->out_st0,
			cp->mod->out_st1,
			cp->mod->out_type,
			0)) {
		delete_channel(cp);
		return err(ERR_SM_DEVERR);
	}
 }
 *cpp = cp;
 make_object(cp);
 return 0;
}

STATIC int real_sm_channel_alloc_placed(struct sm_channel_alloc_placed_parms *ap)
{
 struct module *mp = modid2lockedmodule(ap->module);
 unsigned channo;
 int sts = 0;
 int i;
 if (!mp) return err(ERR_SM_NO_SUCH_MODULE);
 sts = channel_alloc_mod(&ap->channel, ap->type, mp);
 unlock_module(mp);
 if (sts) return sts;
 channo = ap->channel->chan
 	[ap->type != kSMChannelTypeOutput ? HANDLE_READ : HANDLE_WRITE]
	.channo;
 for (i=0; ; i++) {
	if (ap->channel->mod->modmask & (1 << i)) break;
 }
 i += ap->channel->mod->card->nmod * channo;
 ap->channel->channel_ix = i;
 make_object(ap->channel);	// for paranoia
 return sts;
}

STATIC int real_sm_channel_set_output_rate(struct sm_channel_set_output_rate_parms *rp)
{
 const tSMChannelId cp = rp->channel;
 int sts = 0;
 lock_object(cp);
 if (cp->rxtx[0].func != RXTXF_NONE) {
	 if (cp->rxtx[0].func == RXTXF_INVALID) {
		sts = err(ERR_SM_WRONG_CHANNEL_TYPE);
	 } else {
		sts = err(ERR_SM_WRONG_CHANNEL_STATE);
	}
 } else {
	cp->out_sample_rate = rp->sample_rate;
 }
 unlock_object(cp);
 return sts;
}

STATIC int real_sm_switch_channel_output(struct sm_switch_channel_parms *sp)
{
 enum TiNG_COMPAND_TYPE type = TiNG_TSTYPE_NONE;
 int sts = tst2compand(&type, sp->type);
 if (sts) return sts;
 lock_object(sp->channel);
 sts = manual_assign(sp->channel, sp->st, sp->ts, type, 0);
 unlock_object(sp->channel);
 return sts;
}

STATIC int real_sm_switch_channel_input(struct sm_switch_channel_parms *sp)
{
 enum TiNG_COMPAND_TYPE type = TiNG_TSTYPE_NONE;
 int sts = tst2compand(&type, sp->type);
 if (sts) return sts;
 lock_object(sp->channel);
 sts = manual_assign(sp->channel, sp->st, sp->ts, type, 1);
 unlock_object(sp->channel);
 return sts;
}

STATIC int real_sm_channel_info(struct sm_channel_info_parms *ip)
{
 const tSMChannelId chan = ip->channel;
 enum TiNG_COMPAND_TYPE itype, otype;
 int sts;
 lock_object(chan);
 ip->caps_mask = 0;
 ip->card = chan->mod->card->swdrvr;
 sts = chan->mod->card->vtbl->ts_info(chan, &ip->ist, &ip->its, &itype, 1);
 if (!sts) sts = chan->mod->card->vtbl->ts_info(chan, &ip->ost, &ip->ots, &otype, 0);
 if (!sts) {
	compand2tst(&ip->otype, otype);
	compand2tst(&ip->itype, itype);
 }
 unlock_object(chan);
 return sts;
}

STATIC int real_sm_get_channel_ix(tSMChannelId chan)
{
 return chan->channel_ix;
}

STATIC int real_sm_get_channel_type(tSMChannelId chan)
{
 return chan->type;
}

STATIC int linkevent(tSMChannelId cp, enum event_types etyp, tSMEventId ev, int nonidle)
{
 int err;
 int h;
 RAWCHAN *rcp;
 err = cp->mod->card->vtbl->linkevent(cp, etyp, ev);
 if (err) return err;
 switch (etyp) {
 case EVENT_READ:
	h = HANDLE_READ;
	break;
 case EVENT_WRITE:
	h = HANDLE_WRITE;
	break;
 case EVENT_NOTIFY:
	h = HANDLE_DET;
	rcp = &cp->chan[h];
	// open the channel, so we have something to engage the event to
	if (!cardconn_valid(&rcp->cx)) err = open_channel(rcp, cp->mod);
	if (err) return err;
	break;
 default:
	return err(ERR_SM_DEVERR);
 }
 if (nonidle) return 0;
 return engage_event(cp, etyp, h);
}

STATIC int chan_unlinkevent(tSMChannelId cp, enum event_types etyp)
{
 int sts = cp->mod->card->vtbl->unlinkevent(cp, etyp);
 if (sts) return sts;
 switch (etyp) {
 case EVENT_READ:
 case EVENT_WRITE:
#ifdef TiNGTYPE_WINNT
	sts = disengage_event(cp, etyp, etyp == EVENT_READ
		? HANDLE_READ : HANDLE_WRITE);
#endif
	break;
 case EVENT_NOTIFY:
	break;
 default:
	sts =  err(ERR_SM_BAD_PARAMETER);
 }
#ifndef OMIT_ANYCHAN
 if (!sts) {
	sts = real_smd_ev_free(cp->anyev[etyp].intev);
#ifdef TiNG_EVENTS_ARE_POSIX_PIPES
	cp->anyev[etyp].intev.eip = NULL;
#endif
 #ifdef TiNGTYPE_WINNT
	cp->anyev[etyp].intev = (HANDLE) -1;
#endif
 }
#endif
 return sts;
}

//holds lock : anychan
STATIC int real_sm_channel_set_event(struct sm_channel_set_event_parms *ep)
{
 const tSMChannelId cp = ep->channel;
#ifndef OMIT_ANYCHAN
 int lockany;
#endif
 int sts = 0;
 enum event_types etyp;
 switch (ep->event_type) {
 case kSMEventTypeReadData: etyp = EVENT_READ; break;
 case kSMEventTypeWriteData: etyp = EVENT_WRITE; break;
 case kSMEventTypeRecog: etyp = EVENT_NOTIFY; break;
 default: return err(ERR_SM_BAD_PARAMETER);
 }
#ifndef OMIT_ANYCHAN
 lockany = anychan.inuse & (1 << etyp);
 if (lockany) lock_object(&anychan);
#endif
 if (cp) {
#ifndef OMIT_ANYCHAN
	// disable anychan or events on this channel
	sts = anychan_newstate(&cp->anyev[etyp], ANYEV_NOTANY);
#endif
	lock_object(cp);
	sts = chan_unlinkevent(cp, etyp);
		// now channel has no event
	if (!sts) {
		switch (ep->issue_events) {
		case kSMChannelNoEvent:
			break;
		case kSMChannelSpecificEvent:
			sts = linkevent(cp, etyp, ep->event, 0);
			break;
		case kSMChannelSpecificEventNonIdle:
			sts = linkevent(cp, etyp, ep->event, 1);
			break;
		case kSMAnyChannelEvent:
#ifndef OMIT_ANYCHAN
			// allocate internal event for anychan operation
#ifdef TiNG_EVENTS_ARE_POSIX_PIPES
			cp->anyev[etyp].intev.eip = alloc(0, sizeof(struct tSMEventId_internals));
			if (!cp->anyev[etyp].intev.eip) return err(ERR_SM_NO_RESOURCES);
			cp->anyev[etyp].intev.eip->evbind = kSMChannelSpecificEvent;
#endif
			sts = cp->mod->card->vtbl->ev_create(&cp->anyev[etyp].intev,
				cp, etyp, 0);
			if (sts) {
#ifdef TiNG_EVENTS_ARE_POSIX_PIPES
				free(cp->anyev[etyp].intev.eip);
				cp->anyev[etyp].intev.eip = NULL;
#endif
			}
			if (!sts) sts = linkevent(cp, etyp, cp->anyev[etyp].intev, 0);
				// enable this channel for anychan
			if (!sts) sts = anychan_newstate(&cp->anyev[etyp], ANYEV_WAIT);
#else
			return err(ERR_SM_NOT_IMPLEMENTED);
#endif
			break;
		default:
			sts =  err(ERR_SM_BAD_PARAMETER);
		}
	}
	unlock_object(cp);
 } else switch (ep->issue_events) {	// for anychan
 case kSMChannelNoEvent:
	// FIXME: disable anychan event
	break;
 case kSMAnyChannelEvent:
	// set the anychan event
	// FIXME: enable anychan event
	break;
 default:
	sts = err(ERR_SM_BAD_PARAMETER);	// global events
 }
#ifndef OMIT_ANYCHAN
 if (lockany) unlock_object(&anychan);
#endif
 return sts;
}

STATIC int real_sm_channel_release(tSMChannelId chan)
{
 int sts = 0;
#ifndef OMIT_ANYCHAN
 lock_object(&anychan);
#endif
 if (chan) {
	struct module *const mod = chan->mod;
#ifndef OMIT_ANYCHAN
	// disable anychan or events on this channel
	sts = anychan_newstate(&chan->anyev[EVENT_WRITE], ANYEV_NOTANY);
	sts = anychan_newstate(&chan->anyev[EVENT_READ], ANYEV_NOTANY);
	sts = anychan_newstate(&chan->anyev[EVENT_NOTIFY], ANYEV_NOTANY);
#endif
	lock_module(mod);	// locking hierarchy rule
	lock_object(chan);
	sts = chan_unlinkevent(chan, EVENT_READ);
	if (!sts) sts = chan_unlinkevent(chan, EVENT_WRITE);
	if (!sts) sts = chan_unlinkevent(chan, EVENT_NOTIFY);
	unlock_object(chan);
	if (!sts) delete_channel(chan);
	unlock_module(mod);
 }
#ifndef OMIT_ANYCHAN
 unlock_object(&anychan);
#endif
 return sts;
}

STATIC int real_sm_get_card_info(struct sm_card_info_parms *cp)
{
 struct card *cardp = cardid2lockedcard(&aculab_TiNG_cardmap, cp->card);
 struct ting_cardinfo ci;
 CARDCONN dev;
 int sts = 0;
 ci.ioctl_length = sizeof(ci);
 if (!cardp) return err(ERR_SM_BAD_PARAMETER);
 if (ERR(cardp->vtbl->cardconn_ctor(&dev, cardp)))
	sts = err(ERR_SM_DEVERR);
 else if (ERR(cx_ioctl(&dev, IOCTL_TiNG_RDCARDINFO, &ci, sizeof(ci)))) {
	cx_close(&dev);
	sts = err(ERR_SM_DEVERR);
 } else {
	cp->card_detected = 1;
	cp->module_count = cardp->nmod;
	cp->physical_address = ci.memaddress;	
	cp->io_address = ci.ioaddress;
	cp->physical_irq = ci.interrupt;
	strncpy(cp->serial_no, ci.serialno, sizeof(cp->serial_no));
	cp->serial_no[sizeof(cp->serial_no)-1] = 0;
	switch (ci.cardtype) {
	case TiNG_CARDTYPE_S2: cp->card_type = kSMCarrierCardTypeISAS2; break;
	case TiNG_CARDTYPE_P1: cp->card_type = kSMCarrierCardTypePCIP1; break;
	case TiNG_CARDTYPE_C1: cp->card_type = kSMCarrierCardTypePCIC1; break;
	case TiNG_CARDTYPE_BR8: cp->card_type = kSMCarrierCardTypeBR8; break;
	case TiNG_CARDTYPE_SOFT_S: cp->card_type = kSMCarrierCardTypePS; break;
	case TiNG_CARDTYPE_X: cp->card_type = kSMCarrierCardTypePX; break;
	default: cp->card_type = kSMCarrierCardTypeYetUnknown; break;
	}
	if (ERR(cx_close(&dev))) sts = err(ERR_SM_DEVERR);
 }
 unlock_card(cardp);
 return sts;
}


STATIC int real_sm_get_card_rev(struct sm_card_rev_parms *cp)
{
 struct card *cardp = cardid2lockedcard(&aculab_TiNG_cardmap, cp->card);
 CARDCONN dev;
 int sts = 0;
 if (!cardp) return err(ERR_SM_BAD_PARAMETER);
 if (ERR(cardp->vtbl->cardconn_ctor(&dev, cardp)))
	sts = err(ERR_SM_DEVERR);
 else {
	struct ting_cardreg io;
	io.ioctl_length = sizeof(io);
	io.nreg = 1;
	io.reg[0].ofs = 2;
	if (ERR(cx_ioctl(&dev, IOCTL_TiNG_RDIO, &io, sizeof(io)))) {
		sts = err(ERR_SM_DEVERR);
	}
	cp->rev[0] = io.reg[0].val;
	io.reg[0].ofs = 0;
	if (!sts && ERR(cx_ioctl(&dev, IOCTL_TiNG_RDIO, &io, sizeof(io)))) {
		sts = err(ERR_SM_DEVERR);
	}
	cp->rev[1] = io.reg[0].val;
	cp->rev_additional[0] = 0;
	if (ERR(cx_close(&dev)) && !sts) sts = err(ERR_SM_DEVERR);
 }
 unlock_card(cardp);
 return sts;
}

static const struct {
	unsigned long vno;
	char code;
} apiversion = {
#include "gen/apiversion.i"
};

STATIC int real_sm_get_driver_info(struct sm_driver_info_parms *dp)
{
 static const struct {
	int maj, mid, min;
	char code;
	int level;
 } relver = {
#include "gen/relver.i"
 };
 dp->major = relver.maj;
 dp->minor = relver.mid;
 dp->step = relver.min;
 dp->custom = 0;
 dp->quality = apiversion.code == '.' ? relver.code : '?';
 dp->buildno0 = apiversion.vno & 0xffff;
 dp->buildno1 = (apiversion.vno>>16) & 0xffff;
 return 0;
}

STATIC int module2modid(tSMModuleId *modidp, struct module *mp)
{
 *modidp = (tSMModuleId) mp;
 return 0;
}

STATIC tSMCardId real_sm_get_module_card_id(tSMModuleId mod_id)
{
 struct module *mp = modid2lockedmodule(mod_id);
 tSMCardId id = 0;
 int sts = cardmap_findid(&aculab_TiNG_cardmap, &id, mp->card);
 unlock_module(mp);
 	// ambiguous return, unfortunately
 return sts ? sts : id;
}

STATIC ACU_CARD_ID real_sm_get_card_switch_id(tSMCardId card_id)
{
 struct card *sm_card_data = cardid2lockedcard(&aculab_TiNG_cardmap, card_id);
 if (sm_card_data == NULL) return 0;
 unlock_card(sm_card_data);
 return card_id;
}

STATIC int real_sm_close_prosody(SM_CLOSE_PROSODY_PARMS *closep)
{
 return cardmap_del(&aculab_TiNG_cardmap, closep->card_id);
}

STATIC int real_sm_open_module(struct sm_open_module_parms *pp)
{
 struct card *cardp = cardid2lockedcard(&aculab_TiNG_cardmap, pp->card_id);
 struct module *mp = 0;
 int sts = 0;
 if (!cardp) return err(ERR_SM_BAD_PARAMETER);
 if (pp->module_ix < 8 * sizeof(cardp->modmask)
	&& cardp->modmask & (1<<pp->module_ix)) {
	mp = init_mod(cardp, 1<<pp->module_ix);
	if (!mp) {
		sts = err(ERR_SM_OS_RESOURCE_PROBLEM);
	} else {
		if (ERR(open_module(&mp->cx, mp->card, mp->modmask))) {
			sts = err(ERR_SM_DEVERR);
			mod_dtor(mp);
		}
	}
 } else {
	sts = err(ERR_SM_NO_SUCH_MODULE);
 }
 unlock_card(cardp);
 if (sts) return sts;
 return module2modid(&pp->module_id, mp);
}

STATIC int real_sm_module_get_event(struct sm_module_event_parms *pp)
{
 int sts;
 tSMEventId *ep;
 struct module *mp = modid2lockedmodule(pp->module_id);
 if (!mp) return err(ERR_SM_NO_SUCH_MODULE);
 ep = alloc(0, sizeof(tSMEventId));
 if (!ep) {
	sts = err(ERR_SM_OS_RESOURCE_PROBLEM);
 } else {
#ifdef TiNGTYPE_LINUX
	ep->eip = alloc(0, sizeof(struct tSMEventId_internals));
	if (!ep->eip) return err(ERR_SM_OS_RESOURCE_PROBLEM);
	ep->eip->evbind = kSMChannelSpecificEvent;
#endif
#ifdef TiNGTYPE_QNX
	ep->eip = alloc(0, sizeof(struct tSMEventId_internals));
	if (!ep->eip) return err(ERR_SM_OS_RESOURCE_PROBLEM);
	ep->eip->evbind = kSMChannelSpecificEvent;
#endif
	sts = mp->card->vtbl->smd_evmod_create(ep, mp);
	if (sts) {
#ifdef TiNGTYPE_LINUX
		free(ep->eip);
		ep->eip = 0;
#endif
#ifdef TiNGTYPE_QNX
		free(ep->eip);
		ep->eip = 0;
#endif
		free(ep);
	} else {
		mp->ev = ep;
		pp->event = *ep;
	}
 }
 unlock_module(mp);
 return sts;
}

STATIC int real_sm_module_status(struct sm_module_status_parms *pp)
{
 int sts = 0;
 struct module *mp = modid2lockedmodule(pp->module_id);
 if (!mp) return err(ERR_SM_NO_SUCH_MODULE);
 pp->status = kSMModuleStatusShutdown;
 if (cardconn_valid(&mp->cx)) {
	struct ting_mstate ms;
	ms.ioctl_length = sizeof(ms);
	if (ERR(cx_ioctl(&mp->cx, IOCTL_TiNG_RDMSTATE, &ms, sizeof(ms)))) {
		sts = err(ERR_SM_DEVERR);
	} else if (!ms.connstate) pp->status = kSMModuleStatusRunning;
 }
 unlock_module(mp);
 return sts;
}

STATIC int real_sm_shutdown_module(struct sm_shutdown_module_parms *pp)
{
 struct module *mp = modid2lockedmodule(pp->module_id);
 int sts = 0;
 if (!mp) return err(ERR_SM_NO_SUCH_MODULE);
 close_tonesets(mp);
 mp->card->vtbl->mod_shutdown(mp);
 unlock_module(mp);
 return sts;
}

STATIC int real_sm_sec_module(struct sm_sec_module_parms *pp)
{
 struct module *mp = modid2lockedmodule(pp->module_id);
 int sts = 0;
 int level = 0;
 if (!mp) return err(ERR_SM_NO_SUCH_MODULE);
 if (pp->exclude_data_to_module) level |= (1<<0);
 if (pp->exclude_data_from_module) level |= (1<<1);
 mp->card->vtbl->mod_sec(mp,level,pp->key_id,pp->context,pp->encdec);
 unlock_module(mp);
 return sts;
}

STATIC int real_sm_close_module(struct sm_close_module_parms *pp)
{
 struct module *mp = modid2lockedmodule(pp->module_id);
 int sts = 0;
 if (!mp) return err(ERR_SM_NO_SUCH_MODULE);
 unlock_module(mp);
 mod_dtor(mp);
 return sts;
}

STATIC int real_sm_get_channel_module_id(tSMChannelId chan)
{
 (void) chan;
 return err(ERR_SM_NOT_IMPLEMENTED);
}

//Start of TDM API
STATIC int real_sm_tdmrx_create(struct sm_tdmrx_create_parms *tdmrxp)
{
 struct module *mod = modid2lockedmodule(tdmrxp->module);
 tSMTDMrxId tdmrx;
 int sts = 0;
 if (!mod) return err(ERR_SM_NO_SUCH_MODULE);
 tdmrx = init_tdmrx(mod);
 if (!tdmrx) sts = err(ERR_SM_NO_RESOURCES);
 if (!sts) {
	tdmrx->stream = tdmrxp->stream;
	tdmrx->timeslot = tdmrxp->timeslot;
	sts = tst2compand(&tdmrx->type,tdmrxp->type);
	if (!sts) sts = mod->card->vtbl->setup_df(tdmrx->dataf, mod, 1);
	if (!sts) {
		sts = mod->card->vtbl->tdm_assign(tdmrx->dataf, tdmrx->stream,
						tdmrx->timeslot, tdmrx->type, 1);
	}
	if (sts) tdmrx_dtor(tdmrx);
	else {
		make_object(tdmrx);
 		tdmrxp->tdmrx = tdmrx;
	}
}
 unlock_module(mod);
 return sts;
}

STATIC int real_sm_tdmtx_create(struct sm_tdmtx_create_parms *tdmtxp)
{
 struct module *mod = modid2lockedmodule(tdmtxp->module);
 tSMTDMtxId tdmtx;
 int sts = 0;
 if (!mod) return err(ERR_SM_NO_SUCH_MODULE);
 tdmtx = init_tdmtx(mod);
 if (!tdmtx) sts = err(ERR_SM_NO_RESOURCES);
 else {
	tdmtx->stream = tdmtxp->stream;
	tdmtx->timeslot = tdmtxp->timeslot;
	sts = tst2compand(&tdmtx->type,tdmtxp->type);
	if (sts) tdmtx_dtor(tdmtx);
	else {
		make_object(tdmtx);
		tdmtxp->tdmtx = tdmtx;
	}
 }
 unlock_module(mod);
 return sts;
}

STATIC int real_sm_tdmrx_destroy(tSMTDMrxId tdmrx)
{
 if (tdmrx) tdmrx_dtor(tdmrx);
 return 0;
}

STATIC int real_sm_tdmtx_destroy(tSMTDMtxId tdmtx)
{
 if (tdmtx) tdmtx_dtor(tdmtx);
 return 0;
}

STATIC int real_sm_tdmrx_get_datafeed(struct sm_tdmrx_datafeed_parms *datafeedp)
{
 tSMTDMrxId tdmrx = datafeedp->tdmrx;
 lock_object(tdmrx);
 datafeedp->datafeed = tdmrx->dataf.vtbl_data;
 unlock_object(tdmrx);
 return 0;
}

STATIC int real_sm_tdmtx_datafeed_connect(struct sm_tdmtx_datafeed_connect_parms *datafeedp)
{
 tSMTDMtxId tdmtx = datafeedp->tdmtx;
 int sts = 0;
 DATAFEED df;
 lock_object(tdmtx);
	// unassign the old one
 if (nonnull_datafeed(tdmtx->dataf)) {
	sts = tdmtx->mod->card->vtbl->tdm_assign(tdmtx->dataf, -1,-1,-1,0);
 }
 if (!sts && datafeedp->data_source != kSMNullDatafeedId) {
 	df.vtbl_data = datafeedp->data_source;
 	sts = tdmtx->mod->card->vtbl->tdm_assign(df, tdmtx->stream, tdmtx->timeslot, tdmtx->type, 0);
 } else {
	df = NULL_DATAFEED;
 }
 tdmtx->mod->card->vtbl->connect_df(&tdmtx->dataf, df);
 unlock_object(tdmtx);
 return sts;
}

STATIC int real_sm_udp_collector_create(struct sm_udp_collector_create_parms *colp)
{
 struct module *mod;
 tSMCollectorId collector;
 int sts = 0;
 if (colp->type != kSMCollectorTypeIPv4 && colp->type != kSMCollectorTypeIPv6) return err(ERR_SM_BAD_PARAMETER);
 mod = modid2lockedmodule(colp->module);
 if (!mod) return err(ERR_SM_NO_SUCH_MODULE);
 collector = init_collector(mod);
 if (!collector) sts = err(ERR_SM_NO_RESOURCES);
 else {
	tSMGroove *grv = &collector->groove;
	sts = grv->starttask(grv, TASKTC_COLLUDP, 0);
	if (colp->type == kSMCollectorTypeIPv4) {
		if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_SET_INIPV4, 2), ip2msg(colp->address.s_addr));
	} else { // IPv6
		if (!sts) sts = grv->sendmsg(grv, MESSAGE(MSG_SET_INIPV6, 5), ipv6tomsg(colp->ipv6_address));
	}
	if (sts) collector_dtor(collector);
	else {
		make_object(collector);
		colp->collector = collector;
	}
 }
 unlock_module(mod);
 return sts;
}

STATIC int real_sm_collector_destroy(tSMCollectorId col)
{
 if (col) collector_dtor(col);
 return 0;
}

STATIC int unsafe_sm_collector_status(struct sm_collector_status_parms *statusp)
{
 tSMCollectorId collector = statusp->collector;
 tSMGroove *grv = &collector->groove;
 uint32_t msg[16];
 unsigned long code;
 int sts = grv->rdmsg(grv, msg, SMGROOVE_RDMSG_FLAG_NOWAIT);
 if (sts) return sts;
 statusp->status = kSMCollectorStatusRunning;
 code = msg[0] & 0xffff0000;	
 if (code == MESSAGE(MSG_GET_UDPPORTS, 3)) {
	statusp->status = kSMCollectorStatusGotPorts;
	statusp->u.ports.port = msg[1];
	statusp->u.ports.address.s_addr = msg2ip(msg[2]);
 } else if (code == MESSAGE(MSG_GET_UDPPORTS, 6)) {
	statusp->status = kSMCollectorStatusGotPortsIPv6;
	statusp->u.ports_ipv6.port = msg[1];
	msgtoipv6(&statusp->u.ports_ipv6.address, msg[2], msg[3], msg[4], msg[5]);
 } else if (MSG2ANYCODE(code) == MSG_DEAD) {
	if ((code & 0xffff0000) != MESSAGE(MSG_DEAD, 1)) {
		switch (msg[1]) {
		case MSG_DEAD_CAUSE_BADFUNC:
			return err(ERR_SM_NO_SUCH_FIRMWARE);
		case MSG_DEAD_CAUSE_NOMEM:
			return err(ERR_SM_NO_RESOURCES);
		case MSG_DEAD_CAUSE_PARAM:
			return err(ERR_SM_BAD_PARAMETER);
		case MSG_DEAD_CAUSE_NOINIT:
		case MSG_DEAD_CAUSE_FAILED:
		case MSG_DEAD_CAUSE_BADGEN:
		case MSG_DEAD_CAUSE_DUPID:
			return err(ERR_SM_DEVERR);
		case MSG_DEAD_CAUSE_DISCO:
			return err(ERR_SM_DISCONNECTED);
		}
	}
 } else if (code == MESSAGE(MSG_BADMSG, 3)) {
	if ((msg[1] & 0xffff0000) == MESSAGE(MSG_CREATE, 3)
		|| (msg[1] & 0xffff0000) == MESSAGE(MSG_ADDSUB, 2)) {
		switch (msg[2]) {
		case MSG_BADMSG_CAUSE_NOTASK:
			return err(ERR_SM_NO_SUCH_CHANNEL);
		case MSG_BADMSG_CAUSE_UNHANDLED:
			return err(ERR_SM_WRONG_FIRMWARE_TYPE);
		case MSG_BADMSG_CAUSE_RANGE:
			return err(ERR_SM_NO_SUCH_FIRMWARE);
		case MSG_BADMSG_CAUSE_MEMORY:
			return err(ERR_SM_NO_RESOURCES);
		}
		return err(ERR_SM_DEVERR);
	} else if ((msg[1] & 0xffff0000) == MESSAGE(MSG_FRIEND, 2)) {
		//This means there was no valid datafeed - shouldn't happen
	}
 }
 return 0;
}

STATIC int real_sm_collector_status(struct sm_collector_status_parms *statusp)
{
 int sts;
 lock_object(statusp->collector);
 sts = unsafe_sm_collector_status(statusp);
 unlock_object(statusp->collector);
 return sts;
}

STATIC int real_sm_collector_get_event(struct sm_collector_event_parms *eventp)
{
 tSMCollectorId collector = eventp->collector;
 int sts;
 lock_object(collector);
 sts = collector->groove.getev(&collector->groove, &eventp->event);
 unlock_object(collector);
 return sts;
}

STATIC int real_sm_udp_dispatcher_create(struct sm_udp_dispatcher_create_parms *dispp)
{
 struct module *mod = modid2lockedmodule(dispp->module);
 tSMDispatcherId dispatcher;
 int sts = 0;
 if (!mod) return err(ERR_SM_NO_SUCH_MODULE);
 dispatcher = init_dispatcher(mod);
 if (!dispatcher) sts = err(ERR_SM_NO_RESOURCES);
 else {
	tSMGroove *grv = &dispatcher->groove;
	sts = grv->starttask(grv, TASKTC_DISPUDP, 0);
	if (!sts) {
		if (dispp->type == kSMDispatcherTypeIPv4) {
			sts = grv->sendmsg(grv, MESSAGE(MSG_SET_OUTIPV4, 5),
				ip2msg(dispp->destination.sin_addr.s_addr),
				ntohs(dispp->destination.sin_port),
				ip2msg(dispp->source.sin_addr.s_addr),
				ntohs(dispp->source.sin_port));
		} else {
			sts = grv->sendmsg(grv, MESSAGE(MSG_SET_OUTIPV6, 11),
				ipv6tomsg(dispp->destination_ipv6.sin6_addr),
				ntohs(dispp->destination_ipv6.sin6_port),
				ipv6tomsg(dispp->source_ipv6.sin6_addr),
				ntohs(dispp->source_ipv6.sin6_port));
		}
	}
	if (sts) dispatcher_dtor(dispatcher);
	else {
		make_object(dispatcher);
		dispp->dispatcher = dispatcher;
	}
 }
 unlock_module(mod);
 return sts;
}

STATIC int real_sm_dispatcher_destroy(tSMDispatcherId disp)
{
 if (disp) dispatcher_dtor(disp);
 return 0;
}

STATIC int real_sm_channel_get_datafeed(struct sm_channel_datafeed_parms *datafeedp)
{
 tSMChannelId cp = datafeedp->channel;
 int sts;
 lock_object(cp);
 if (cp->type == kSMChannelTypeInput) {
	sts = err(ERR_SM_WRONG_CHANNEL_TYPE);
 } else {
	sts = cp->mod->card->vtbl->get_df(cp->df[0], cp->mod, &cp->chan[HANDLE_WRITE].cx);
	if (!sts) datafeedp->datafeed = cp->df[0].vtbl_data;
 }
 unlock_object(cp);
 return sts;
}

STATIC int real_sm_channel_collector_connect(struct sm_channel_collector_connect_parms *colp)
{
 tSMChannelId cp = colp->channel;
 int sts = 0;
 lock_object(cp);
 if (cp->rxtx[0].func != RXTXF_NONE) {
	if (cp->rxtx[0].func == RXTXF_INVALID) {
		 sts = err(ERR_SM_WRONG_CHANNEL_TYPE);
	} else sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else if (nonnull_datafeed(cp->redirect_tx)) {
	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else {
	cp->collector = colp->source;
 }
 unlock_object(cp);
 return sts;
}

STATIC int real_sm_channel_dispatcher_connect(struct sm_channel_dispatcher_connect_parms *dispp)
{
 tSMChannelId cp = dispp->channel;
 int sts = 0;
 lock_object(cp);
 if (cp->rxtx[1].func != RXTXF_NONE) {
	if (cp->rxtx[1].func == RXTXF_INVALID) {
		 sts = err(ERR_SM_WRONG_CHANNEL_TYPE);
	} else sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else if (nonnull_datafeed(cp->redirect_rx)) {
	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else {
	cp->dispatcher = dispp->dest;
 }
 unlock_object(cp);
 return sts;
}

STATIC int real_sm_channel_datafeed_connect(struct sm_channel_datafeed_connect_parms *datafeedp)
{
 tSMChannelId cp = datafeedp->channel;
 DATAFEED df;
 int sts;
 lock_object(cp);
 if (cp->type == kSMChannelTypeOutput) {
	sts = err(ERR_SM_WRONG_CHANNEL_TYPE);
 } else {
	df.vtbl_data = datafeedp->data_source;
	sts = cp->mod->card->vtbl->connect_df(&cp->df[1], df);
 }
 if (!sts) {
	enum handles h;
		// if the channel has any input operations in
		// progress, connect them up
	for (h=HANDLE_ECHO; h < HANDLE_NUM; h++) {
		CARDCONN *cx = &cp->chan[h].cx;
		if (cardconn_valid(cx)) {
			sts = cp->mod->card->vtbl->engage_df(cp->df[1], cx, DFC_IN);
			if (sts) break;
		}
	}
 }
 unlock_object(cp);
 return sts;
}

STATIC char *real_sm_error_name(struct sm_error_name_parms *erp)
{
 char *name;
 switch (erp->code) {
#include "gen/errorname.i"
 default:
	sprintf(erp->name, "Unknown Prosody error: %d 0x%x", erp->code,
		erp->code);
	return erp->name;
 }
 strcpy(erp->name, name);
 return erp->name;
}

STATIC int real_sm_channel_input_data_redirect(struct sm_channel_input_data_redirect_parms *redp)
{
 tSMChannelId cp = redp->channel;
 int sts = 0;
 lock_object(cp);
 if (cp->rxtx[1].func != RXTXF_NONE) {
	if (cp->rxtx[1].func == RXTXF_INVALID) {
		 sts = err(ERR_SM_WRONG_CHANNEL_TYPE);
	} else sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else if (cp->dispatcher) {
	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else {
	cp->mod->card->vtbl->datafeed_dtor(cp->redirect_rx);
	cp->redirect_rx = NULL_DATAFEED;
	if (!sts && redp->enable) {
		cp->redirect_rx = cp->mod->card->vtbl->datafeed();
		sts = cp->mod->card->vtbl->setup_df(cp->redirect_rx, cp->mod, 0);
		if (!sts) redp->datafeed = cp->redirect_rx.vtbl_data;
	}
 }
 unlock_object(cp);
 return sts;
}

STATIC int real_sm_channel_output_source_datafeed_connect(struct sm_channel_output_source_datafeed_connect_parms *outsrcp)
{
 tSMChannelId cp = outsrcp->channel;
 DATAFEED df;
 int sts = 0;
 lock_object(cp);
 if (cp->rxtx[0].func != RXTXF_NONE) {
	if (cp->rxtx[0].func == RXTXF_INVALID) {
		 sts = err(ERR_SM_WRONG_CHANNEL_TYPE);
	} else sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else if (cp->collector) {
	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else {
	df.vtbl_data = outsrcp->data_source;
	cp->mod->card->vtbl->connect_df(&cp->redirect_tx, df);
	cp->redirect_tx_threshold = outsrcp->threshold;
	cp->redirect_tx_timeout = outsrcp->timeout;
 }
 unlock_object(cp);
 return sts;
 return err(ERR_SM_NOT_IMPLEMENTED);
}

#include "trace_gen_show.h"
#include "../pubdoc/gen/prosgen.i"
