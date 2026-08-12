/* cardconn_px.h - connection to a Prosody X card */

#ifndef INCLUDED_CARDCONN_PX_H
#define INCLUDED_CARDCONN_PX_H

#include "smtypes.h"
#include "cardconn.h"
#include "TiNGo_card.h"
#include "TiNGo_datafeed.h"

	// open a connection to a card
LOCALDEC ERRCODE px_cardconn_open(CARDCONN *cx, struct card *card);
LOCALDEC ERRCODE psv3_cardconn_open(CARDCONN *cx, struct card *card);
#endif
