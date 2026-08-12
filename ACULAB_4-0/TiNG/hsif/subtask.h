/* subtask.h - subtask messages passed between host and sharc */

#ifndef INCLUDED_SUBTASK_H
#define INCLUDED_SUBTASK_H

#define MSG_ADDSUB 0xe09
	/*
	 * ADDSUB(code)
	 * adds a subtask to a subtask chain
	 * h->s only
	 */

#define MSG_ADDHSUB 0xe19
	/*
	 * ADDHSUB(code)
	 * adds a subtask to a subtask chain
	 * this is the same as ADDSUB and only needed for backward
	 * compatibility - when adding a HBUF subtask, failure due to
	 * lack of firmware (which is ok as long as we have received READY)
	 * can be distinguished from failure due to lack of any other
	 * firmware (which is not ok).
	 * h->s only
	 */

#define MSG_DELSUB 0xe0a
	/*
	 * DELSUB(code)
	 * deletes a subtask from a subtask chain
	 * h->s only
	 */


#endif
