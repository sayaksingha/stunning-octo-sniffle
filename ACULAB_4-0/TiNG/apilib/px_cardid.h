#ifndef INCLUDED_PX_CARDID_H
#define INCLUDED_PX_CARDID_H

#ifdef TiNGTYPE_LINUX
#include <netinet/in.h>
#include <sys/socket.h>
#endif

#ifdef TiNGTYPE_QNX
#include <netinet/in.h>
#include <sys/socket.h>
#endif


#ifdef TiNGTYPE_WINNT
#include "../libutil/WINNT/wind.h"
#endif

#include "refcnt.h"

typedef struct {
	REFCNT ref;
	char *name;	// actually contains: name ['\0' port] '\0' key '\0'
	char *key;	// points to key (in same malloc() block as name)
	char remaddr[256];	// address of card in numeric form
	int remport; // port of cardinfod
	int asspmonport; // port of asspmon
	int card_type; // currently PX or PS(v3)
	char serialno[16];	 //The serial number of this card
	char verinfo[64];	 //Various version information.
	unsigned nmod;
	struct {
		unsigned physmod;
		unsigned firstport;
		unsigned lastport;
		char addrname[256]; // the modules address if provided (otherwise use the card name)
	} mod[32];
	int ntdmcards; // -1 if unknown (tdmlist unsupported by older PS)
	struct {
		unsigned firststream;
		unsigned laststream;
		unsigned timeslots;
		char serialno[16]; // serial number of PS TDM card for stream, eg. thin card for PS
	} tdmcards[32];
} PX_CARDID;

#endif
