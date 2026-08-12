/* rawchan.h - raw channel as understood by driver */

#ifndef INCLUDED_RAWCHAN_H
#define INCLUDED_RAWCHAN_H

#include "smtypes.h"
#include "cardconn.h"

typedef struct {
	CARDCONN cx;
	unsigned long channo;
} RAWCHAN;

#define RAWCHAN_INIT { CARDCONN_INIT, 0 }

#endif
