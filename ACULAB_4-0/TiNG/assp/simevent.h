/* simevent.h - events in simulator */

#ifndef INCLUDED_SIMEVENT_H
#define INCLUDED_SIMEVENT_H

#include "timefn.h"

typedef struct event {
	struct event *next;
	ABSTIME when;
	void (*fn)(void *arg);
	void *arg;
	char *desc;
} EVENT;

void eventqueue(ABSTIME when, void (*fn)(void *arg), void *arg, char *desc);

char *runevent(void);

#endif
