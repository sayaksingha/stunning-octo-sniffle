/* datacomms.h - messages passed between host and sharc for any datacomms */

#ifndef INCLUDED_DATACOMMS_H
#define INCLUDED_DATACOMMS_H

#define MSG_SUSPEND 0xc98
	/*
	 * SUSPEND
	 * suspend expensive signal analysis, while preserving any
	 * training information
	 */

#define MSG_RESUME 0xc9a
	/*
	 * RESUME
	 * resume expensive signal analysis, using any training which was
	 * preserved by SUSPEND
	 */

#define MSG_XORVAL 0xc9b
	/*
	 * XORVAL(val)
	 * sets value to be XOR'd with timeslot data
	 * h->s only
	 */

#define MSG_RESUME_MONITOR 0xc9d
	/*
	 * RESUME_MONITOR
	 * resume signal analysis but only report carrier (no data), using state which was
	 * preserved by SUSPEND
	 */

#endif
