/* checkrec.h - handle a record channel becoming ready */

#ifndef INCLUDED_CHECKREC_H
#define INCLUDED_CHECKREC_H

#include "bfile.h"
#include "smbesp.h"

typedef struct {
	int done;		// set when record finishes - initialise to 0
	tSMChannelId chan;	// the Prosody channel in use
	BFILE *bf;		// the file to record into
	unsigned bytes;		// total bytes transferred (not essential)
	char *dbgname;		// a channel name to print in error messages
} RECORDING;

/*
 * check the status of a recording and act accordingly. Returns 0, if nothing
 * interesting happened, 1 for an error.
 */
int checkrec(RECORDING *cp);

#endif
