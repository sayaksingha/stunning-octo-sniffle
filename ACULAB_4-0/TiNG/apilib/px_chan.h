#ifndef INCLUDED_PX_CHAN_H
#define INCLUDED_PX_CHAN_H

#include "assp_conn.h"
#include "px_mod.h"
#include "TiNGo_datafeed.h"

//#define PX_CHAN_EVENT_DURATION_LOGGING

typedef struct {
	enum {		// note that these are similar to the driver states
			// but an api channel cannot exist if it is not open,
			// so the NONE and CLOSEWAIT states do not exist
		CT_IDLE,	// idle but open
		CT_RUNNING,	// sent CREATE command, running
		CT_STOPPING,	// sent DEAD command, awaiting response
	} ctstate;
	enum {
		CD_NONE,	// no data transfer
		CD_READ,	// uses read(), fetching from DSP
		CD_WRITE,	// uses write(), sending to DSP
	}  chandir;
	unsigned dspbufsize;
	unsigned writecap;	// capacity for write() in bytes
	unsigned minready;	// in bits
	int minready_set;	// in bits (may be negative)
	int minready_dsp;	// DSP's idea of minready (in bits)
	int no_capacity_ev; // if non zero, event is only set when messages are pending
	unsigned timeout_milliseconds;
	unsigned timeout_dsp;	// DSP's idea of timeout_milliseconds
	ASSP_CHAN *chancx;
	PX_MOD *pxmod;
#ifdef PX_CHAN_EVENT_DURATION_LOGGING
	uint_fast64_t event_set_time;
#endif
} PX_CHAN;

LOCALDEC PX_CHAN *allocateProsodyXChannel(PX_MOD *pxmod);
LOCALDEC void handle_dead(PX_CHAN *pxchan);

#ifdef PX_CHAN_EVENT_DURATION_LOGGING
LOCALDEC px_chan_event_log(PX_CHAN* pxchan, int set);
#endif

#endif
