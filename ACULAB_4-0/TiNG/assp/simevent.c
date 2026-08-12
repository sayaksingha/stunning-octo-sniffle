/* simevent.c - events in simulator */

#include <stdlib.h>
#include <stdio.h>
#include "simevent.h"
#include "simtimer.h"

static EVENT *evq;

void eventqueue(ABSTIME when, void (*fn)(void *arg), void *arg, char *desc)
{
 EVENT *ev = malloc(sizeof(*ev));
 EVENT **evp;
 if (!ev) {
 	perror("malloc failed");
	printf("Cannot allocate %d bytes for event\n", sizeof(*ev));
	exit(1);
 }
 ev->when = when;
 ev->fn = fn;
 ev->arg = arg;
 ev->desc = desc;
 {
  char buff[80];
  simtime_string(buff, buff+sizeof(buff), ev->when);
//  printf("Queue event: %s @%s\n", ev->desc, buff);
  if (time_sgn(time_until(when)) < 0) {
	printf("Event in the past!\n");
	abort();
  }
 }
 for (evp = &evq; ; ) {
 	EVENT *t = *evp;
	if (!t || time_cmp(when, t->when) < 0) {
		ev->next = t;
		*evp = ev;
		break;
	}
	evp = &t->next;
 }
}

char *runevent(void)
{
 EVENT *ev = evq;
 char *desc;
 if (!ev) return 0;
 evq = ev->next;
 time_advance(ev->when);
// printf("%s\n", ev->desc);
 ev->fn(ev->arg);
 desc = ev->desc;
 free(ev);
 return desc;
}
