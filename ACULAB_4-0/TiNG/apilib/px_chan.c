/* px_chan.c - Prosody X channels */

#include "px_chan.h"
#include "px_event.h"
#include "TiNGcommon.h"
#include "TiNGo_datafeed.h"
#include "smgroove.h"
#include "../hsif/kernel.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

STATIC int px_chan_update_event(void* context)
{
 return px_update_event((PX_CHAN*)context);
}

#ifdef PX_CHAN_EVENT_DURATION_LOGGING
#define EDL_NUM_BINS 20
static int bins[EDL_NUM_BINS];
static int num_logged;
static int mutx_inited = 0;
MUTX log_mutx;

LOCALDEC px_chan_event_log(PX_CHAN* pxchan, int set)
{
	if (set) {
		if (pxchan->event_set_time == 0) {
			pxchan->event_set_time = now_u64();
		}
	} else if (pxchan->event_set_time != 0) { // don't count clear when not set
		int binno = 0;
		uint_fast64_t t = now_u64() - pxchan->event_set_time;
		pxchan->event_set_time = 0;
		while (t > 0) {
			binno++;
			t >>= 1;
			if (binno == EDL_NUM_BINS - 1) break;
		}
		// now put the time into the right bin!
		mutx_enter(&log_mutx);
		bins[binno]++;
		if (++num_logged == 200000) {
			int i;
			for (i = 0; i < EDL_NUM_BINS; i++) {
				olog("bin % 2d: %d\n", i, bins[i]);
				bins[i] = 0;
				num_logged = 0;
			}
		}
		mutx_leave(&log_mutx);

	}
}
#endif

LOCALDEF PX_CHAN *allocateProsodyXChannel(PX_MOD *pxmod)
{
 PX_CHAN *pxchan = alloc(0, sizeof(*pxchan));
 if (pxchan) {
	lock_object(&pxmod->modgroup);
#ifdef PX_CHAN_EVENT_DURATION_LOGGING
	if (!mutx_inited) {
		mutx_inited = 1;
		mutx_init(&log_mutx);
	}
#endif
	pxchan->chancx = alloc_chan(&pxmod->modgroup);
	unlock_object(&pxmod->modgroup);
	if (!pxchan->chancx) {
		free(pxchan);
		return 0;
	}
	pxchan->chancx->ready_fn = px_chan_update_event;
	pxchan->chancx->ready_fn_context = pxchan;
	pxchan->pxmod = pxmod;
	pxchan->minready_set = 1;
	pxchan->dspbufsize = ~0u;
	pxchan->minready = 1;
	pxchan->minready_dsp = 1;
	pxchan->no_capacity_ev = 0;
	pxchan->timeout_milliseconds = 1;
	pxchan->timeout_dsp = 1;
	pxchan->chandir = CD_NONE;
	pxchan->writecap = 0;
	pxchan->ctstate = CT_IDLE;
#ifdef PX_CHAN_EVENT_DURATION_LOGGING
	pxchan->event_set_time = 0;
#endif
 }
 return pxchan;
}

LOCALDEF void handle_dead(PX_CHAN *pxchan)
{
 if (pxchan->ctstate != CT_IDLE) task_nextgen(pxchan->chancx);
 pxchan->chandir = CD_NONE;
 pxchan->ctstate = CT_IDLE;
 pxchan->writecap = 0;
 pxchan->dspbufsize = ~0u;
 pxchan->minready_dsp = 1;
 pxchan->no_capacity_ev = 0;
 pxchan->timeout_dsp = 1;
}
