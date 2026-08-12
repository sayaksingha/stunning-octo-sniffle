/* TiNGo_card.h - card objects */

#ifndef INCLUDED_TINGO_CARD_H
#define INCLUDED_TINGO_CARD_H

#include "paranoia.h"

#include "TiNGo_chan.h"
#include "TiNGo_datafeed.h"

struct card {
	PARAMUTX pm;
	void (*dtor)(struct card *card);
	const struct card_vtbl *vtbl;
	void *vtbl_data;
	char *cardname;
	int swdrvr;
	unsigned int nmod;
	unsigned int modmask;
};

/* Note that the 'card' type is best understood as a virtual base class.
 * Each kind of Prosody implements a derived class. The 'vtbl' field
 * allows the virtual functions to be called, while 'vtbl_data'
 * points to the member data of the instance of the derived class.
 * Unlike in C++, the derived class's constructor (vtbl->card_ctor)
 * is called during (not after) the construction of the base class.
 *
 * Each card class has a corresponding channel class, but since a
 * channel can only be created by a card, the virtual function table
 * for a channel is shared with the one for its card.
 */

struct card_vtbl {
		// open a new connection to this card
	ERRCODE (*cardconn_ctor)(CARDCONN *cx, struct card *card);
		// setup a connection between an event (tSMEventId)
		// and a channel. (but do not let channel signal the event)
	int (*linkevent)(tSMChannelId chan, enum event_types etyp, tSMEventId ep);
		// break a connection between an event (tSMEventId)
		// and a channel
	int (*unlinkevent)(tSMChannelId chan, enum event_types etyp);
		// derived card class constructor
	int (*card_ctor)(struct card *card, struct ting_cardinfo *cip);
		// derived channel class constructor
	int (*chan_ctor)(tSMChannelId chan);
		// convert a module-relative stream/timeslot to a stream
		// value that the (V5) switch driver uses
	int (*swstreamno)(struct module *mod, unsigned pos);
		// convert a module-relative stream/timeslot to a timeslot
		// value that the (V5) switch driver uses
	int (*swtimeslot)(struct module *mod, unsigned pos);
		// construct a datafeed
	DATAFEED (*datafeed)(void);
		// destroy a datafeed
	int (*datafeed_dtor)(DATAFEED df);
		// create datafeed on given module
	int (*setup_df)(DATAFEED df, struct module *mod, int direct);
		// connect an datafeed to an operation running on a cardconn
	int (*engage_df)(DATAFEED df, CARDCONN *cx, enum dfconntyp type);
		// retrieve the datafeed being provided by this channel's output
	int (*get_df)(DATAFEED df, struct module* module, CARDCONN* cx);
		// the datafeed is no longer associated with this provider
	int (*unbind_df)(DATAFEED df, CARDCONN* cx);
		// assign datafeed to target (with reference count handling)
	int (*connect_df)(DATAFEED *targ, DATAFEED df);
		// find out stuff needed for sm_channel_info()
	int (*ts_info)(tSMChannelId chan, int *stp, int *tsp, enum TiNG_COMPAND_TYPE *typp, int direct);
		//assign a tdm timeslot to a datafeed
	int (*tdm_assign)(DATAFEED df, unsigned int stream, unsigned int timeslot, enum TiNG_COMPAND_TYPE type, int direct);
		// allocate and connect an external timeslot direct to a
		// channel (only possible on ISA S2 card)
	int (*auto_assign)(tSMChannelId chan, unsigned int st0, unsigned int st1, enum TiNG_COMPAND_TYPE type, int direct);
		// connect an external timeslot direct to a channel
		// stream -1 leaves no timeslot connected
	int (*manual_assign)(tSMChannelId chan, unsigned int stream, unsigned int timeslot, enum TiNG_COMPAND_TYPE type, int direct);
		// create a tSMEventId suitable for this channel
	int (*ev_create)(tSMEventId *ep, tSMChannelId chan, enum event_types etyp, int ni);
		// create in a tSMEventId suitable for this cardconn
	int (*smd_evcx_create)(tSMEventId *ep, CARDCONN *cx);
		// create in a tSMEventId suitable for this module
	int (*smd_evmod_create)(tSMEventId *ep, struct module *mp);
		// start closing this module
	int (*mod_shutdown)(struct module *mp);
		// secure module comms
	int (*mod_sec)(struct module *mp,int x,int y,void *ctx,tSMSecCallback encdec);
		// make a channel start controlling its event based
		// on its readiness
	int (*engage_event)(tSMChannelId chan, enum event_types etyp, CARDCONN *cx);
		// stop a channel controlling its event based
		// on its readiness, and clear the event on Windows
	int (*disengage_event)(tSMChannelId chan, enum event_types etyp, CARDCONN *cx);
		// channel's destructor - called before any base
		// class destruction takes place (like C++)
	int (*chan_dtor)(tSMChannelId chan);
		// card destructor - called before any base
		// class destruction takes place (like C++)
	int (*card_dtor)(struct card *card);
};

	// card constructor - cardname is an arbitrary string whose
	// interpretation depends on the type of card
struct card *aculab_TiNG_card_init(const struct card_vtbl *lvtbl, char *cardname);

#endif
