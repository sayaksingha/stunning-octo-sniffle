#ifndef INCLUDED_PX_CONN_H
#define INCLUDED_PX_CONN_H

#include "px_cardid.h"
#include "px_chan.h"
#include "px_mod.h"

typedef struct {
	PX_CARDID *id;
	int dest_type;	// an enum TiNG_DEST_*
	union {
		PX_CHAN *chan;	// if TiNG_DEST_CHANNEL
		PX_MOD *mod;	// if TiNG_DEST_MODULE
	} u;
} PX_CARDCONN_VTBL_DATA;

#endif
