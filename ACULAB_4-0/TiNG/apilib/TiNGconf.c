/* TiNGconf.c - conferencing */

#include "TiNGconf.h"
#include "TiNGcommon.h"
#include "TiNGo_mod.h"
#include "../hsif/conf.h"
#include "../hsif/kernel.h"
#include "../hsif/subtask.h"
#include "../hsif/typecode.h"
#include "../hsif/play.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define arlen(x) (sizeof(x)/sizeof(*(x)))

STATIC void confin_dtor(tSMChannelId cp)
{
 struct module *const mod = cp->mod;
 unsigned oid;
 confmask_dtor(&cp->confin.outmask);
 mod->confman.id2in[cp->confin.id] = 0;
 cp->confin.id = CONFIN_ID_NONE;
 cp->confin.volume = 0;
 cp->confin.agc = 0;
 cp->confin.min_lmin = -55;
 cp->confin.speech_thresh = 0.01;
 cp->rxtx[1].dtor = 0;
 for (oid=0; oid < mod->confman.maxconf; oid++) {
	if (mod->confman.id2out[oid]) return;	// someone is using it
	if (mod->confman.id2in[oid]) return;	// someone is using it
 }
 cx_close(&mod->confman.chan.cx);
}

STATIC void civin_dtor(tSMChannelId cp)
{
 struct module *const mod = cp->mod;
 unsigned oid;
 confmask_dtor(&cp->civin.outmask);
 cp->mod->civman.id2in[cp->civin.id] = 0;
 cp->civin.id = CONFIN_ID_NONE;
 cp->rxtx[1].dtor = 0;
 for (oid=0; oid < mod->civman.maxconf; oid++) {
	if (mod->civman.id2out[oid]) return;	// someone is using it
	if (mod->civman.id2in[oid]) return;	// someone is using it
 }
 cx_close(&mod->civman.chan.cx);
}

STATIC int setoutmask(tSMChannelId cp)
{
 struct ting_message msg;
 unsigned len;
 unsigned i;
 msg.ioctl_length = sizeof(msg);
 msg.flags = 0;
 len = (cp->ut.besp.confout.inmask.maxconf + 0x1f) >> 5;
 while (len > 2 && cp->ut.besp.confout.inmask.mask[len - 1] == 0) len--;
 while (len > 14) {
	unsigned start = ((len - 15) / 13) * 13 + 15; // 1 - 14 go in INMASK and 13 fit in INMASK_PART
	unsigned pos = start - 1;
	msg.msg[1] = pos;
	for (i = 2; pos < len; i++, pos++) {
		msg.msg[i] = cp->ut.besp.confout.inmask.mask[pos];
	}
	msg.msg[0] = MESSAGE(MSG_INMASK_PART, pos - start + 3);
	if (ERR(cx_ioctl(&cp->chan[HANDLE_WRITE].cx, IOCTL_TiNG_WRMSG,
		&msg, sizeof(struct ting_message))))
		return err(ERR_SM_DEVERR);
	len = start - 1;
 }
 for (i = 0; i < len; i++) {
	msg.msg[1 + i] = cp->ut.besp.confout.inmask.mask[i];
 }
 for (; i < 2; i++) { // min INMASK has two 32bit words
	msg.msg[1 + i] = 0;
 }
 msg.msg[0] = MESSAGE(MSG_INMASK, i + 1);
 if (ERR(cx_ioctl(&cp->chan[HANDLE_WRITE].cx, IOCTL_TiNG_WRMSG,
	&msg, sizeof(struct ting_message))))
	return err(ERR_SM_DEVERR);
 return 0;
}

STATIC int civsetout(tSMChannelId cp, int iid)
{
 int sts = sendmsg3(&cp->chan[HANDLE_WRITE].cx, MSG_CIVFAC,
	iid, (int) (cp->ut.besp.civout.factor[iid] * (1 << 24)));
 return sts;
}

STATIC void civin_leave(tSMChannelId icp, tSMChannelId ocp)
{
 unsigned oid = ocp->ut.besp.civout.id;
 unsigned iid = icp->civin.id;
 confmask_clr(&ocp->ut.besp.civout.inmask, iid);
 ocp->ut.besp.civout.factor[iid] = 0;
 confmask_clr(&icp->civin.outmask, oid);
}

// called with module lock already held
STATIC void confin_chan_dtor(tSMChannelId cp)
{
 unsigned iid = cp->confin.id;
 struct module *const mod = cp->mod;
 /* Note: no need to update outputs to inform them that this input is
  * being removed. When the handle is closed, the input subtask will
  * do the necessary cleanup in its dtor function.
  */
 if (iid != CONFIN_ID_NONE) {
	unsigned oid;
	for (oid=0; oid < mod->confman.maxconf; oid++) {
		tSMChannelId ocp = mod->confman.id2out[oid];
		if (ocp) {
			if (ocp != cp) lock_object(ocp);
			confmask_clr(&ocp->ut.besp.confout.inmask, iid);
				// setoutmask(ocp);
			if (ocp != cp) unlock_object(ocp);
		}
	}
	confin_dtor(cp);
 }
 iid = cp->civin.id;
 if (iid != CONFIN_ID_NONE) {
	unsigned oid;
	for (oid=0; oid < mod->civman.maxconf; oid++) {
		tSMChannelId ocp = mod->civman.id2out[oid];
		if (ocp) {
			if (ocp != cp) lock_object(ocp);
			if (confmask_test(&ocp->ut.besp.civout.inmask, iid)) {
				civin_leave(cp, ocp);
				// civsetout(ocp, cp->civin.id);
			}
			if (ocp != cp) unlock_object(ocp);
		}
	}
	civin_dtor(cp);
 }
}

STATIC int confin_leave(tSMChannelId icp, tSMChannelId ocp)
{
 unsigned oid = ocp->ut.besp.confout.id;
 unsigned iid = icp->confin.id;
 confmask_clr(&ocp->ut.besp.confout.inmask, iid);
 /* output isn't stopped if it has no inputs because:
  * a) there is sm_conf_prim_abort() to do so explicitly
  * b) it is needed to keep the timeslot filled
  */
 confmask_clr(&icp->confin.outmask, oid);
 if (confmask_empty(&icp->confin.outmask) && !icp->confin.attached) {
	confin_dtor(icp);
	return stopdet(icp, HANDLE_DET, DETTYPE_CONF, 0);
 }
 return 0;
}

STATIC int needconfman(struct module *mod)
{
 int sts;
 if (cardconn_valid(&mod->confman.chan.cx)) return 0;
 sts = open_channel(&mod->confman.chan, mod);
 if (sts) return sts;
 sts = starttask(&mod->confman.chan.cx, TASKTC_CONFMAN, 0);
 return sts;
}

STATIC int needcivman(struct module *mod)
{
 int sts;
 if (cardconn_valid(&mod->civman.chan.cx)) return 0;
 sts = open_channel(&mod->civman.chan, mod);
 if (sts) return sts;
 sts = starttask(&mod->civman.chan.cx, TASKTC_CIVMAN, 0);
 return sts;
}

STATIC int adjio(CARDCONN *cx, int volume, int agc)
{
 int sts;
 sts = sendmsg2(cx, MSG_VOLUME, volume);
 if (sts) return sts;
 sts = sendmsg2(cx, MSG_AGC, agc);
 if (sts) return sts;
 return 0;
}

STATIC int adjin(tSMChannelId cp)
{
 CARDCONN *cx = &cp->chan[HANDLE_DET].cx;
 double ml;
 int sts;
 if (!cardconn_valid(cx)) return 0;
 sts = sendmsg2(cx, MSG_AGC_SPEECH_THRESH, cp->confin.speech_thresh * 65536);
 if (sts) return sts;
 ml = exp((cp->confin.min_lmin + 90) / 10 * log(10));
 sts = sendmsg2(cx, MSG_AGC_MIN_LMIN, ml);
 if (sts) return sts;
 return adjio(cx, cp->confin.volume, cp->confin.agc);
}

STATIC int adjout(tSMChannelId cp)
{
 return adjio(&cp->chan[HANDLE_WRITE].cx,
 	cp->ut.besp.confout.volume,
 	cp->ut.besp.confout.agc);
}

STATIC void confout_dtor(tSMChannelId cp)
{
 struct module *const mod = cp->mod;
 unsigned oid;
 mod->confman.id2out[cp->ut.besp.confout.id] = 0;
 confmask_dtor(&cp->ut.besp.confout.inmask);
 cp->rxtx[0].func = RXTXF_NONE;
 cp->rxtx[0].dtor = 0;
 for (oid=0; oid < mod->confman.maxconf; oid++) {
	if (mod->confman.id2out[oid]) return;	// someone is using it
	if (mod->confman.id2in[oid]) return;	// someone is using it
 }
 cx_close(&mod->confman.chan.cx);
}

STATIC void civout_dtor(tSMChannelId cp)
{
 struct module *const mod = cp->mod;
 unsigned oid;
 mod->civman.id2out[cp->ut.besp.civout.id] = 0;
 confmask_dtor(&cp->ut.besp.civout.inmask);
 cp->rxtx[0].func = RXTXF_NONE;
 cp->rxtx[0].dtor = 0;
 for (oid=0; oid < mod->civman.maxconf; oid++) {
	if (mod->civman.id2out[oid]) return;	// someone is using it
	if (mod->civman.id2in[oid]) return;	// someone is using it
 }
 cx_close(&mod->civman.chan.cx);
}

// called with module lock already held
STATIC void confout_chan_dtor(tSMChannelId cp)
{
 struct module *const mod = cp->mod;
 unsigned iid;
 for (iid=0; iid < mod->confman.maxconf; iid++) {
	tSMChannelId icp = mod->confman.id2in[iid];
	if (icp) {
		if (icp != cp) lock_object(icp);
		confin_leave(icp, cp);
		if (icp != cp) unlock_object(icp);
	}
 }
 confout_dtor(cp);
}

// called with module lock already held
STATIC void civout_chan_dtor(tSMChannelId cp)
{
 struct module *const mod = cp->mod;
 unsigned iid;
 for (iid=0; iid < mod->civman.maxconf; iid++) {
	tSMChannelId icp = mod->civman.id2in[iid];
	if (icp) {
		if (icp != cp) lock_object(icp);
		civin_leave(icp, cp);
		if (icp != cp) unlock_object(icp);
	}
 }
 civout_dtor(cp);
}

STATIC int extend_confman(struct module *mod)
{
 unsigned n = mod->confman.maxconf * 2;
 unsigned id;
 tSMChannelId *np;
 if (!n) n = 64;
 np = alloc(mod->confman.id2in, n * sizeof(np));
 if (!np) return err(ERR_SM_OS_RESOURCE_PROBLEM);
 for (id = mod->confman.maxconf; id < n; id++) np[id] = 0;	// clear extra
 mod->confman.id2in = np;
 np = alloc(mod->confman.id2out, n * sizeof(np));
 if (!np) return err(ERR_SM_OS_RESOURCE_PROBLEM);
 for (id = mod->confman.maxconf; id < n; id++) np[id] = 0;	// clear extra
 mod->confman.id2out = np;
 mod->confman.maxconf = n;
 return 0;
}

STATIC int extend_civman(struct module *mod)
{
 unsigned n = mod->civman.maxconf * 2;
 unsigned id;
 tSMChannelId *np;
 if (!n) n = 64;
 np = alloc(mod->civman.id2in, n * sizeof(np));
 if (!np) return err(ERR_SM_OS_RESOURCE_PROBLEM);
 for (id = mod->civman.maxconf; id < n; id++) np[id] = 0;	// clear extra
 mod->civman.id2in = np;
 np = alloc(mod->civman.id2out, n * sizeof(np));
 if (!np) return err(ERR_SM_OS_RESOURCE_PROBLEM);
 for (id = mod->civman.maxconf; id < n; id++) np[id] = 0;	// clear extra
 mod->civman.id2out = np;
 mod->civman.maxconf = n;
 return 0;
}

STATIC int initout(tSMChannelId cp, int volume, int agc, int civ)
{
 CARDCONN *cx = &cp->chan[HANDLE_WRITE].cx;
 struct module *mod = cp->mod;
 int sts = 0;
 unsigned oid;
 if (cp->rxtx[0].func != RXTXF_NONE) {
	 if (cp->rxtx[0].func == RXTXF_INVALID) return err(ERR_SM_WRONG_CHANNEL_TYPE);
	 return err(ERR_SM_WRONG_CHANNEL_STATE);
 }
 if (civ) {
	sts = needcivman(mod);
	if (sts) return sts;
	sts = startout(cp, TASKTC_OUTCHAN);
	if (sts) return sts;
	sts = sendmsg2(cx, MSG_ADDSUB, SUBTASKTC_CIVOUT);
	if (sts) return sts;
	sts = setfriend(&cp->chan[HANDLE_WRITE].cx, &mod->civman.chan, FRIEND_TYPE_CIVMAN);
	if (!sts) {
		cp->rxtx[0].func = RXTXF_CIVOUT;
		confmask(&cp->ut.besp.civout.inmask);
		for (oid=0; ; oid++) {
			if (oid >= mod->civman.maxconf
				&& (sts = extend_civman(mod))) {
				break;
			}
			if (!mod->civman.id2out[oid]) {
				cp->ut.besp.civout.id = oid;
				mod->civman.id2out[oid] = cp;
				break;
			}
		}
		cp->rxtx[0].dtor = civout_chan_dtor;
	}
 } else {
	sts = needconfman(mod);
	if (sts) return sts;
	sts = startout(cp, TASKTC_CONFOUT);
	if (sts) return sts;
	sts = setfriend(&mod->confman.chan.cx, &cp->chan[HANDLE_WRITE], 0);
	if (!sts){
		cp->rxtx[0].func = RXTXF_CONFOUT;
		confmask(&cp->ut.besp.confout.inmask);
		for (oid=0; ; oid++) {
			if (oid >= mod->confman.maxconf
				&& (sts = extend_confman(mod))) {
				break;
			}
			if (!mod->confman.id2out[oid]) {
				cp->ut.besp.confout.id = oid;
				mod->confman.id2out[oid] = cp;
				break;
			}
		}
		cp->ut.besp.confout.volume = volume;
		cp->ut.besp.confout.agc = agc;
		cp->rxtx[0].dtor = confout_chan_dtor;
	}
 }
 if (sts) {
	starttask(cx, 0, 0);
	cp->rxtx[0].func = RXTXF_NONE;
	return sts;
 }
 cp->rxtx[0].state = RXTXS_RUN;
 if (volume || agc) return adjout(cp);
 return 0;
}

STATIC int civin_attach(tSMChannelId icp)
{
 struct module *const mod = icp->mod;
 RAWCHAN *rc = &mod->civman.chan;
 struct ting_confin confin;
 int sts;
 if (icp->civin.id != CONFIN_ID_NONE) return 0;
 sts = needcivman(mod);
 if (sts) return sts;
 sts = startdet(icp, HANDLE_DET, DETTYPE_CIV, 0);
 if (sts) return sts;
 	/* con't just use normal message - if we read messages we might
 	 * get a detection report, which would confuse the detector and
 	 * may result in missed detections
 	 */
 confin.ioctl_length = sizeof(confin);
 confin.channo = rc->channo;
 if (ERR(getmagic(&confin.magic, &rc->cx))) return err(ERR_SM_DEVERR);
 for (confin.id = 0; confin.id < mod->civman.maxconf; confin.id++) {
	 if (mod->civman.id2in[confin.id] == 0) break;
 }
 if (confin.id == mod->civman.maxconf) sts = extend_civman(mod); // after extending civman, old maxconf is next free slot
 if (sts) return sts;
 if (ERR(cx_ioctl(&icp->chan[HANDLE_DET].cx,
	IOCTL_TiNG_WRCONFIN, &confin, sizeof(struct ting_confin))))
	return err(ERR_SM_DEVERR);
 if (confin.id == ~0u) return err(ERR_SM_NO_RESOURCES);
 while (confin.id >= mod->civman.maxconf) {
	sts = extend_civman(mod);
	if (sts) return sts;
 }
 mod->civman.id2in[confin.id] = icp;
 icp->civin.id = confin.id;
 confmask(&icp->civin.outmask);
 icp->rxtx[1].dtor = confin_chan_dtor;
 return 0;
}

STATIC int confin_attach(tSMChannelId icp)
{
 struct module *const mod = icp->mod;
 RAWCHAN *rc = &mod->confman.chan;
 struct ting_confin confin;
 int sts;
 if (icp->confin.id != CONFIN_ID_NONE) return 0;
 sts = needconfman(mod);
 if (sts) return sts;
 sts = startdet(icp, HANDLE_DET, DETTYPE_CONF, 0);
 if (sts) return sts;
 sts = adjin(icp);
 if (sts) return sts;
 confin.ioctl_length = sizeof(confin);
 confin.channo = rc->channo;
 if (ERR(getmagic(&confin.magic, &rc->cx))) return err(ERR_SM_DEVERR);
 // alternatives for picking id
#if 1
 // bottom up
 for (confin.id = 0; confin.id < mod->confman.maxconf; confin.id++) {
	 if (mod->confman.id2in[confin.id] == 0) break;
 }
 if (confin.id == mod->confman.maxconf) sts = extend_confman(mod); // after extending confman, old maxconf is next free slot
#else
 // top down
 confin.id = mod->confman.maxconf;
 for (;;) {
	if (confin.id == 0) {
		sts = extend_confman(mod);
		confin.id = mod->confman.maxconf-1; // after extending confman, maxconf-1 is next free slot
		break;
	}
	if (mod->confman.id2in[--confin.id] == 0) break;
 }
#endif
 if (sts) return sts;
 if (ERR(cx_ioctl(&icp->chan[HANDLE_DET].cx,
	IOCTL_TiNG_WRCONFIN, &confin, sizeof(struct ting_confin))))
	return err(ERR_SM_DEVERR);
 if (confin.id == ~0u) return err(ERR_SM_NO_RESOURCES);
 while (confin.id >= mod->confman.maxconf) {
	sts = extend_confman(mod);
	if (sts) return sts;
 }
 mod->confman.id2in[confin.id] = icp;
 icp->confin.id = confin.id;
 confmask(&icp->confin.outmask);
 icp->rxtx[1].dtor = confin_chan_dtor;
 return 0;
}

LOCALDEF int real_sm_conf_prim_attach(struct sm_conf_prim_attach_parms *cp)
{
 tSMChannelId chan = cp->channel;
 int sts;
 lock_module(chan->mod);
 lock_object(chan);
 switch (cp->conf_type) {
 case kSMConfTypeStandard: sts = confin_attach(chan);
	if (!sts) chan->confin.attached = 1;
	break;
 case kSMConfTypeIndividualVolume: sts = civin_attach(chan); break;
 default: sts = err(ERR_SM_BAD_PARAMETER);
 }
 unlock_object(chan);
 unlock_module(chan->mod);
 return sts;
}

LOCALDEF int real_sm_conf_prim_detach(struct sm_conf_prim_detach_parms *cp)
{
 tSMChannelId chan = cp->channel;
 int sts;
 lock_module(chan->mod);
 lock_object(chan);
 switch (cp->conf_type) {
 case kSMConfTypeStandard:
	if (!chan->confin.attached) sts = err(ERR_SM_WRONG_CHANNEL_STATE);
	else if (!confmask_empty(&chan->confin.outmask)) {
		sts = err(ERR_SM_WRONG_CHANNEL_STATE);
	} else {
		confin_dtor(chan);
		sts = stopdet(chan, HANDLE_DET, DETTYPE_CONF, 0);
	}
	if (!sts) chan->confin.attached = 0;
	break;
 case kSMConfTypeIndividualVolume:
		// FIXME: check that input is not in use
	if (chan->civin.id == CONFIN_ID_NONE) sts = err(ERR_SM_WRONG_CHANNEL_STATE);
	else {
		civin_dtor(chan);
		sts = stopdet(chan, HANDLE_DET, DETTYPE_CIV, 0);
	}
	break;
 default: sts = err(ERR_SM_BAD_PARAMETER);
 }
 unlock_object(chan);
 unlock_module(chan->mod);
 return sts;
}

LOCALDEF int real_sm_conf_prim_start(struct sm_conf_prim_start_parms *cp)
{
 tSMChannelId chan = cp->channel;
 int sts;
 lock_module(chan->mod);
 lock_object(chan);
 switch (cp->conf_type) {
 case kSMConfTypeStandard:
	sts = initout(chan, cp->volume, cp->agc, 0);
	break;
 case kSMConfTypeIndividualVolume:
	sts = initout(chan, cp->volume, cp->agc, 1);
	break;
 default: sts = err(ERR_SM_BAD_PARAMETER);
 }
 unlock_object(chan);
 unlock_module(chan->mod);
 return sts;
}

STATIC int confin_add(tSMChannelId icp, tSMChannelId ocp)
{
 int sts;
 if (icp->confin.id == CONFIN_ID_NONE) die("lost confin id");
 sts = confmask_set(&ocp->ut.besp.confout.inmask, icp->confin.id);
 if (sts) return sts;
 sts = confmask_set(&icp->confin.outmask, ocp->ut.besp.confout.id);
 if (sts) return sts;
 return 0;
}

STATIC int civin_add(tSMChannelId icp, tSMChannelId ocp, float factor)
{
 int sts;
 if (icp->civin.id == CONFIN_ID_NONE) die("lost civin id");
 sts = confmask_set(&ocp->ut.besp.civout.inmask, icp->civin.id);
 if (sts) return sts;
 ocp->ut.besp.civout.factor[icp->civin.id] = factor;
 sts = confmask_set(&icp->civin.outmask, ocp->ut.besp.civout.id);
 if (sts) return sts;
 return 0;
}

LOCALDEF int real_sm_conf_prim_clone(struct sm_conf_prim_clone_parms *cp)
{
 const tSMChannelId chanp = cp->channel;
 const tSMChannelId model = cp->model;
 struct module *mod = chanp->mod;
 int sts;
 lock_module(chanp->mod);
 lock_object(chanp);
 if (!model) {
	sts = initout(chanp, 0, 0, 0);
 } else if (model == chanp) {
	sts = err(ERR_SM_BAD_PARAMETER);
 } else {
	lock_object(model);
	if (model->mod != mod) {
		sts = err(ERR_SM_BAD_PARAMETER);
	} else if (model->rxtx[0].func == RXTXF_CONFOUT) {
		unsigned iid = CONFIN_ID_NONE;
		sts = initout(chanp,
			model->ut.besp.confout.volume,
			model->ut.besp.confout.agc, 0);
		for (;;) {
			tSMChannelId incp;
			if (sts) break;
			iid = confmask_next(&model->ut.besp.confout.inmask, iid);
			if (iid == CONFIN_ID_NONE) break;
			incp = mod->confman.id2in[iid];
			if (incp != chanp && incp != model) lock_object(incp);
			sts = confin_add(incp, chanp);
			if (incp != chanp && incp != model) unlock_object(incp);
		}
		if (!sts) sts = setoutmask(chanp);
	} else if (model->rxtx[0].func == RXTXF_CIVOUT) {
		unsigned iid = CONFIN_ID_NONE;
		sts = initout(chanp, 0, 0, 1);
		for (;;) {
			tSMChannelId incp;
			float factor;
			if (sts) break;
			iid = confmask_next(&model->ut.besp.civout.inmask, iid);
			if (iid == CONFIN_ID_NONE) break;
			incp = mod->civman.id2in[iid];
			if (incp != chanp && incp != model) lock_object(incp);
			factor = model->ut.besp.civout.factor[iid];
			sts = civin_add(incp, chanp, factor);
			if (incp != chanp && incp != model) unlock_object(incp);
			if (sts) break;
			if (factor) {
				sts = civsetout(chanp, iid);
			}
		}
	} else {
		sts = err(ERR_SM_WRONG_CHANNEL_STATE);
	}
	unlock_object(model);
 }
 unlock_object(chanp);
 unlock_module(chanp->mod);
 return sts;
}

LOCALDEF int real_sm_conf_prim_add(struct sm_conf_prim_add_parms *cp)
{
 const tSMChannelId chanp = cp->channel;
 struct module *mod = chanp->mod;
 int sts = 0;
 lock_module(mod);
 lock_object(chanp);
 if (cp->participant != chanp) lock_object(cp->participant);
 if (!sts && mod != cp->participant->mod) sts = err(ERR_SM_BAD_PARAMETER);
 if (chanp->rxtx[0].func == RXTXF_CONFOUT) {
	if (!sts && cp->participant->confin.id == CONFIN_ID_NONE) {
			// auto attach
		sts = confin_attach(cp->participant);
	}
	if (!sts) sts = confin_add(cp->participant, chanp);
	if (!sts) cp->id = cp->participant->confin.id;
	if (!sts) sts = setoutmask(chanp);
 } else if (chanp->rxtx[0].func == RXTXF_CIVOUT) {
	if (!sts && cp->participant->civin.id == CONFIN_ID_NONE) {
		sts = err(ERR_SM_WRONG_CHANNEL_STATE);
	}
	if (!sts) sts = civin_add(cp->participant, chanp, cp->factor);
	if (!sts) cp->id = cp->participant->civin.id;
	if (!sts) sts = civsetout(chanp, cp->id);
 } else {
 	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } 
 if (cp->participant != chanp) unlock_object(cp->participant);
 unlock_object(chanp);
 unlock_module(mod);
 return sts;
}

LOCALDEF int real_sm_conf_prim_leave(struct sm_conf_prim_leave_parms *cp)
{
 struct module *mod = cp->channel->mod;
 tSMChannelId incp;
 int sts;
 lock_module(mod);
 lock_object(cp->channel);
 if (cp->channel->rxtx[0].func == RXTXF_CONFOUT) {
	if ((unsigned) cp->id >= mod->confman.maxconf) {
		sts = err(ERR_SM_BAD_PARAMETER);
	} else {
		incp = mod->confman.id2in[cp->id];
		if (!incp) sts = err(ERR_SM_WRONG_CHANNEL_STATE);
		else {
			if (cp->channel != incp) lock_object(incp);
			sts = confin_leave(incp, cp->channel);
			if (cp->channel != incp) unlock_object(incp);
		}
		if (!sts) sts = setoutmask(cp->channel);
	}
 } else if (cp->channel->rxtx[0].func == RXTXF_CIVOUT) {
	if ((unsigned) cp->id >= mod->civman.maxconf) {
		sts = err(ERR_SM_BAD_PARAMETER);
	} else {
		incp = mod->civman.id2in[cp->id];
		if (!incp) sts = err(ERR_SM_WRONG_CHANNEL_STATE);
		else {
			if (cp->channel != incp) lock_object(incp);
			civin_leave(incp, cp->channel);
			if (cp->channel != incp) unlock_object(incp);
			sts = civsetout(cp->channel, cp->id);
		}
	}
 } else {
 	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 }
 unlock_object(cp->channel);
 unlock_module(mod);
 return sts;
}

LOCALDEF int real_sm_conf_prim_info(struct sm_conf_prim_info_parms *cp)
{
 struct module *mod = cp->channel->mod;
 struct ting_confinfo confinfo;
 unsigned i = 0;
 confinfo.ioctl_length = sizeof(confinfo);
 lock_module(mod);
 lock_object(cp->channel);
 if (cp->channel->rxtx[0].func != RXTXF_CONFOUT) {
 	i = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else {
	if (ERR(cx_ioctl(&mod->confman.chan.cx, IOCTL_TiNG_RDCONFINFO,
		&confinfo, sizeof(struct ting_confinfo)))) {
		i = err(ERR_SM_DEVERR);
 	}
 }
 unlock_object(cp->channel);
 unlock_module(mod);
 if (i) return i;
 cp->participant_count = 0;
 for (i=0; i<arlen(cp->speakers); i++) cp->speakers[i] = 0;
 i = CONFIN_ID_NONE;
 for (;;) {
 	unsigned ofs;
 	unsigned bit;
 	i = confmask_next(&cp->channel->ut.besp.confout.inmask, i);
	if (i == CONFIN_ID_NONE) break;
	ofs = i >> 5;
	if (ofs >= arlen(confinfo.speakers)) break;
	bit = i & 0x1f;
	if (confinfo.speakers[ofs] & (1 << bit)) {
		ofs = i >> 3;
		bit = i & 7;
		if (ofs < arlen(cp->speakers)) cp->speakers[ofs] |= 1 << bit;
		cp->participant_count++;
	}
 }
 return 0;
}

LOCALDEF int real_sm_conf_prim_adj_input(struct sm_conf_prim_adj_input_parms *cp)
{
 const tSMChannelId chanp = cp->channel;
 int sts;
 lock_module(chanp->mod);
 lock_object(chanp);
 if (chanp->confin.id == CONFIN_ID_NONE && chanp->civin.id == CONFIN_ID_NONE) {
	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else {
	chanp->confin.volume = cp->volume;
	chanp->confin.agc = cp->agc;
	sts = adjin(chanp);
 }
 unlock_object(chanp);
 unlock_module(chanp->mod);
 return sts;
}

LOCALDEF int real_sm_conf_prim_adj_input_settings(struct sm_conf_prim_adj_input_settings_parms *cp)
{
 const tSMChannelId chanp = cp->channel;
 int sts;
 lock_module(chanp->mod);
 lock_object(chanp);
 if (chanp->confin.id == CONFIN_ID_NONE && chanp->civin.id == CONFIN_ID_NONE) {
	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else {
	U32 lmax_decay;
	U32 target;
	if (cp->max_level_decay > 1.0 || cp->max_level_decay < 0.0) sts = err(ERR_SM_BAD_PARAMETER);
	if (cp->target_level > 3.0 || cp->target_level < -90.0) sts = err(ERR_SM_BAD_PARAMETER);
	lmax_decay = 32768.0 * cp->max_level_decay;
	target = exp((cp->target_level + 90.0) / 10.0 * log(10));
	sts = sendmsg3(&chanp->chan[HANDLE_DET].cx, MSG_CONF_ADJ_INPUT_SETTINGS, lmax_decay, target);
 }
 unlock_object(chanp);
 unlock_module(chanp->mod);
 return sts;
}

LOCALDEF int real_sm_conf_prim_adj_tracking(struct sm_conf_prim_adj_tracking_parms *cp)
{
 const tSMChannelId chanp = cp->channel;
 int sts;
 lock_module(chanp->mod);
 lock_object(chanp);
 if (chanp->confin.id == CONFIN_ID_NONE && chanp->civin.id == CONFIN_ID_NONE) {
	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else {
	chanp->confin.min_lmin = cp->min_noise_level;
	chanp->confin.speech_thresh = cp->speech_thresh;
	sts = adjin(chanp);
 }
 unlock_object(chanp);
 unlock_module(chanp->mod);
 return sts;
}

LOCALDEF int real_sm_conf_prim_adj_output(struct sm_conf_prim_adj_output_parms *cp)
{
 const tSMChannelId chanp = cp->channel;
 int sts;
 lock_module(chanp->mod);
 lock_object(chanp);
 if (cp->channel->rxtx[0].func != RXTXF_CONFOUT) {
 	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else {
	chanp->ut.besp.confout.volume = cp->volume;
	chanp->ut.besp.confout.agc = cp->agc;
	sts = adjout(chanp);
 }
 unlock_object(chanp);
 unlock_module(chanp->mod);
 return sts;
}

LOCALDEF int real_sm_conf_prim_abort(tSMChannelId cp)
{
	struct sm_conf_prim_stop_parms stop;
	memset(&stop,0,sizeof(stop));
	stop.channel = cp;
	//stop.no_wait = 0; // done in memset
	return real_sm_conf_prim_stop(&stop);
}

LOCALDEF int real_sm_set_sidetone_channel(struct sm_set_sidetone_channel_parms *cp)
{
 tSMChannelId chan = cp->channel;
 int sts;
 if (cp->output && cp->output->rxtx[0].func != RXTXF_CONFOUT) {
 	return err(ERR_SM_WRONG_CHANNEL_STATE);
 }
 lock_module(chan->mod);
 lock_object(chan);
 if (chan->confin.id == CONFIN_ID_NONE) {
 	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else if (cp->output) {
	if (cp->output != chan) lock_object(cp->output);
	sts = setfriend(&chan->chan[HANDLE_DET].cx,
		&cp->output->chan[HANDLE_WRITE], 0);
	if (cp->output != chan) unlock_object(cp->output);
 } else {
	sts = setfriend(&chan->chan[HANDLE_DET].cx, 0, 0);
 }
 unlock_object(chan);
 unlock_module(chan->mod);
 return sts;
}

STATIC int unsafe_conf_prim_status(struct sm_conf_prim_status_parms *statusp, int nowait)
{
 tSMChannelId cp = statusp->channel;
 int sts;
 for (;;) {
	struct ting_message msg;
	unsigned long code; 
	memset(&msg, 0, sizeof(msg));
	if (nowait) msg.flags = IOCTL_TiNG_RDMSG_FLAG_NOWAIT;
	sts = rdmsg(&msg, &cp->chan[HANDLE_WRITE].cx);
	if (sts) break;
	code = MSG2CODE(msg.msg[0]);
	if (!code) {
		statusp->status = kSMConfStatusRunning;
		break;
	} else 	if ((code >> 4) == MSG_DEAD) {
		if (code == MSG2CODE(MESSAGE(MSG_DEAD, 2))) {
			/* These leave the operation running, so the user
			 * is forced to check the status again, when the
			 * synthesised DEAD message will indicate that
			 * the operation is finished.
			 */
			switch (msg.msg[1]) {
			case MSG_DEAD_CAUSE_BADFUNC:
				return err(ERR_SM_NO_SUCH_FIRMWARE);
			case MSG_DEAD_CAUSE_NOMEM:
				return err(ERR_SM_NO_RESOURCES);
			case MSG_DEAD_CAUSE_DISCO:
				return err(ERR_SM_DISCONNECTED);
			}
		}
		statusp->status = kSMConfStatusStopped;
		if (cp->rxtx[0].func == RXTXF_CONFOUT) {
			confout_chan_dtor(cp);
		} else if (cp->rxtx[0].func == RXTXF_CIVOUT) {
			civout_chan_dtor(cp);
		}
		break;
	} else if (code == MSGCODE(MSG_CONF_ACTIVE,9)) {
		statusp->status = kSMConfStatusActiveInputs;
		statusp->u.active_inputs.input[0].id = msg.msg[1];
		statusp->u.active_inputs.input[1].id = msg.msg[2];
		statusp->u.active_inputs.input[2].id = msg.msg[3];
		statusp->u.active_inputs.input[3].id = msg.msg[4];
		statusp->u.active_inputs.input[0].power = msg.msg[5];
		statusp->u.active_inputs.input[1].power = msg.msg[6];
		statusp->u.active_inputs.input[2].power = msg.msg[7];
		statusp->u.active_inputs.input[3].power = msg.msg[8];
		break;
	}
 }
 return sts;
}

LOCALDEF int real_sm_conf_prim_stop(struct sm_conf_prim_stop_parms *stopp)
{
 tSMChannelId cp = stopp->channel;
 struct module *mod = cp->mod;
 int sts;
 lock_module(mod);
 lock_object(cp);
 if (cp->rxtx[0].func == RXTXF_CONFOUT) {
	sts = starttask(&cp->chan[HANDLE_WRITE].cx, 0, 0);
 } else if (cp->rxtx[0].func == RXTXF_CIVOUT) {
	sts = starttask(&cp->chan[HANDLE_WRITE].cx, 0, 0);
 } else {
 	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 }
 if (!sts && !stopp->no_wait) {
	 for(;;) {
		struct sm_conf_prim_status_parms stat;
		memset(&stat,0,sizeof(stat));
		stat.channel = cp;
		sts = unsafe_conf_prim_status(&stat,0);
		if (sts) break;
		if (stat.status == kSMConfStatusStopped) break;
	 }

 }
 unlock_object(cp);
 unlock_module(mod);
 return sts;
}

LOCALDEF int real_sm_conf_prim_status(struct sm_conf_prim_status_parms *statusp)
{
 tSMChannelId cp = statusp->channel;
 struct module *mod = cp->mod;
 int sts;
 lock_module(mod);
 lock_object(cp);
 if (cp->rxtx[0].func != RXTXF_CONFOUT && cp->rxtx[0].func != RXTXF_CIVOUT) {
 	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 } else {
	sts = unsafe_conf_prim_status(statusp,1);
 }
 unlock_object(cp);
 unlock_module(mod);
 return sts;
}

LOCALDEF int real_sm_conf_prim_config_activity_reporting(struct sm_conf_prim_config_activity_reporting_parms *activityp)
{
 tSMChannelId cp = activityp->channel;
 struct module *mod = cp->mod;
 int sts = 0;
 if (activityp->sensitivity > 100u) return err(ERR_SM_BAD_PARAMETER);
 lock_module(mod);
 lock_object(cp);
 if (cp->rxtx[0].func != RXTXF_CONFOUT && cp->rxtx[0].func != RXTXF_CIVOUT) {
 	sts = err(ERR_SM_WRONG_CHANNEL_STATE);
 }
 if (!sts) sts = sendmsg3(&cp->chan[HANDLE_WRITE].cx, MSG_CONF_ACTIVE, activityp->delay, (32768*activityp->sensitivity)/100);
 unlock_object(cp);
 unlock_module(mod);
 return sts;
}

