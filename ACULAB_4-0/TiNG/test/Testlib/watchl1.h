/* watchl1.h - watch layer 1 on port */

#ifndef INCLUDED_WATCHL1_H
#define INCLUDED_WATCHL1_H

#include "acu_type.h"
#include "cl_lib.h"

typedef struct {
	char dbgname[32];	// printed with each status message
	int stop;		// makes the loop exit
	ACU_PORT_ID net;		// the port to watch
} LAYER1;

	/*
	 * watches layer 1 on a trunk and reports (by printing on stdout)
	 * all status changes seen. Returns when the 'stop' field is
	 * non-zero.
	 */
int watchl1(LAYER1 *l1);

#endif
