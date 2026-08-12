/* TiNGo_chan.c - channel objects */

#include "TiNGo_chan.h"
#include "TiNGcommon.h"
#include "TiNGo_mod.h"
#include "TiNGsmdc_speech.h"
#include <stdlib.h>
#include <string.h>
#include "smdrvr.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

LOCALDEF void chan_dtor(struct tSMChannelId_struct *cp)
{
 unsigned u;
 if (nonnull_datafeed(cp->df[1])) cp->mod->card->vtbl->manual_assign(cp, -1, 0, 0, 1);
 for (u=0; u < arlen(cp->rxtx); u++) {
	 if (cp->rxtx[u].dtor) cp->rxtx[u].dtor(cp);
 }
 cp->mod->card->vtbl->unbind_df(cp->df[0], &cp->chan[HANDLE_WRITE].cx);
 if (cp->type == kSMChannelTypeHalfDuplex) {
	cardconn(&cp->chan[HANDLE_READ].cx);
 }
 for (u=0; u < arlen(cp->chan); u++) {
	if (cardconn_valid(&cp->chan[u].cx)) {
		cx_close(&cp->chan[u].cx);
	}
 }
 cp->mod->card->vtbl->chan_dtor(cp);
 cp->mod->card->vtbl->datafeed_dtor(cp->df[0]);
 cp->mod->card->vtbl->datafeed_dtor(cp->df[1]);
 cp->mod->card->vtbl->datafeed_dtor(cp->ecref);
 cp->mod->card->vtbl->datafeed_dtor(cp->ecaltdest);
 for (u=0; u < arlen(cp->rxtx); u++) {
	 cp->mod->card->vtbl->datafeed_dtor(cp->rxtx[u].auxdf);
 }
 cp->mod->card->vtbl->datafeed_dtor(cp->redirect_rx);
 cp->mod->card->vtbl->datafeed_dtor(cp->redirect_tx);
 for (u=0; u < arlen(cp->recog); u++) {
	free(cp->recog[u].hist);
	free(cp->recog[u].recog_id);
 }
 mutx_dtor(&cp->pm.mx);
 memset(cp, 0xa5, sizeof(*cp));		// for safety
 free(cp);
}

LOCALDEF struct tSMChannelId_struct *init_chan(struct module *mod)
{
 struct tSMChannelId_struct *cp = alloc(0, sizeof(*cp));
 unsigned int u;
 if (!cp) return 0;
 memset(cp, 0, sizeof(*cp));
 cp->mod = mod;
 for (u=0; u < arlen(cp->chan); u++) cardconn(&cp->chan[u].cx);
#ifndef OMIT_ANYCHAN
 for (u=0; u < arlen(cp->anyev); u++) {
	cp->anyev[u].chan = cp;
	cp->anyev[u].etyp = u;
	cp->anyev[u].evstate = ANYEV_NOTANY;
	cp->anyev[u].next = &cp->anyev[u];
	cp->anyev[u].prevp = &cp->anyev[u].next;
#ifdef TiNG_EVENTS_ARE_POSIX_PIPES
	cp->anyev[u].waitpos = 0;
	cp->anyev[u].intev.eip = NULL;
	cp->anyev[u].intev.fd = -1;
#endif
#ifdef TiNGTYPE_WINNT
	cp->anyev[u].intev = (HANDLE) -1;
#endif
 }
#endif
 for (u=0; u < arlen(cp->event); u++) {
	cp->event[u].nonidle = 0;
 }
 cp->confin.volume = 0;
 cp->confin.agc = 0;
 cp->confin.min_lmin = -55;
 cp->confin.speech_thresh = 0.01;
 cp->confin.id = CONFIN_ID_NONE;
 cp->civin.id = CONFIN_ID_NONE;
 cp->df[1] = NULL_DATAFEED;
 cp->df[0] = mod->card->vtbl->datafeed();
 cp->ecref = NULL_DATAFEED;;
 cp->ecaltdest = NULL_DATAFEED;;
 cp->ecloopback = 0;
 cp->collector = kSMNullCollectorId;
 cp->dispatcher = kSMNullDispatcherId;
 cp->redirect_rx = NULL_DATAFEED;
 cp->redirect_tx = NULL_DATAFEED;
 cp->out_sample_rate = 8000;
 if (!nonnull_datafeed(cp->df[0])) {
 	free(cp);
 	return 0;
 }
 for (u=0; u < arlen(cp->rxtx); u++) {
	cp->rxtx[u].func = RXTXF_INVALID;
	cp->rxtx[u].protocol = &protable_speech;
	cp->rxtx[u].dataready = 0;
	cp->rxtx[u].minimum_bits = 2048 * 8;	// 0.25 seconds
	cp->rxtx[u].timeout_milliseconds = 0;
	cp->rxtx[u].dtor = 0;
	cp->rxtx[u].gotready = 0;
	cp->rxtx[u].auxdf = NULL_DATAFEED;
 }
 for (u=0; u < arlen(cp->recog); u++) {
	cp->recog[u].detmask = 0;
	cp->recog[u].tone_detect_end = 0;
 }
 cp->catsig = 0;
 mutx_init(&cp->pm.mx);
 if (cp->mod->card->vtbl->chan_ctor(cp)) {
 	mod->card->vtbl->datafeed_dtor(cp->df[0]);
 	free(cp);
 	return 0;
 }
 make_object(cp);
 return cp;
}
