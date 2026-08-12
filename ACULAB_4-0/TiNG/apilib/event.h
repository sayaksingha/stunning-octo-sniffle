/* event.h - an event mechanism */
#ifndef INCLUDED_EVENT_H
#define INCLUDED_EVENT_H

#define evnt_from_listen aculab_TiNG_evnt_from_listen

#ifdef TiNGTYPE_LINUX
#define TiNG_EVENTS_ARE_POSIX_PIPES
#endif

#ifdef TiNGTYPE_QNX
#define TiNG_EVENTS_ARE_POSIX_PIPES
#endif


#ifdef TiNG_EVENTS_ARE_POSIX_PIPES
typedef struct {
	int fd[2];
} EVNT;

typedef int SIG_EVNT;
#define SIG_EVNT_INVALID 0
	// returns an operating system native object that can be signalled
#define evnt_to_sig(e) ((e).fd[1])
	// returns an operating system native object that can be waited on
#define evnt_to_listen(e) ((e).fd[0])
int evnt_from_listen(EVNT *ev, int fd);
#endif


#ifdef TiNGTYPE_WINNT

#include "../libutil/WINNT/wind.h"

typedef struct {
	HANDLE evh;
} EVNT;

typedef HANDLE SIG_EVNT;
#define SIG_EVNT_INVALID 0
	// returns an operating system native object that can be signalled
#define evnt_to_sig(e) ((e).evh)
	// returns an operating system native object that can be waited on
#define evnt_to_listen(e) ((e).evh)
int evnt_from_listen(EVNT *ev, HANDLE h);

	/* on Windows a HANDLE leaves the bottom two bits spare for
	 * a user-defined tag. We use the least significant bit of 
	 * event handles to indicate that the event is the
	 * any-channel event.
	 */
#define ANYCHAN_EVENT_MASK 1

#endif

#define evnt_signal(ev) signal_event(evnt_to_sig(ev))

	// we have to let all these be global, so avoid unqualified names
#define evnt aculab_TiNGcore_evnt
#define evnt_create aculab_TiNGcore_evnt_create
#define evnt_valid aculab_TiNGcore_evnt_valid
#define evnt_assign aculab_TiNGcore_evnt_assign
#define signal_event aculab_TiNGcore_signal_event
#define evnt_clear aculab_TiNGcore_evnt_clear
#define evnt_destroy aculab_TiNGcore_evnt_destroy

void evnt(EVNT *ev);
int evnt_create(EVNT *ev);
int evnt_valid(EVNT ev);
int evnt_assign(EVNT *ev, EVNT rhs);
int signal_event(SIG_EVNT ev);
int evnt_clear(EVNT ev);
void evnt_destroy(EVNT *ev);

#endif
