/* checkplay.h - handle a replay channel becoming ready */

#ifndef INCLUDED_CHECKPLAY_H
#define INCLUDED_CHECKPLAY_H

#include "bfile.h"
#include "smbesp.h"

typedef struct {
	enum {
		PLAY_RUN,
		PLAY_EOF,
		PLAY_DONE,
	} pstate;		// updated by checkplay(), you must initialise
	tSMChannelId chan;	// the Prosody channel in use
	BFILE *bf;		// the file to be played
	unsigned bytes;		// total bytes transferred (not essential)
	char *dbgname;		// a channel name to print in error messages
} REPLAY;

/*
 * check the status of a replay and act accordingly. Returns 0 if ok,
 * non-zero for an error.
 */
int checkplay(REPLAY *cp);

#endif
