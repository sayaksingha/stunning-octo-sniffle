/* ectest.h - messages used by echo cancellation tester */

#ifndef INCLUDED_ECTEST_H
#define INCLUDED_ECTEST_H

#define MSG_ECTEST_DELAY 0xe20
	/*
	 * ECTEST_DELAY(samples)
	 * sets the delay of the echo to be added to the input signal
	 * This may be negative to indicate that the input signal should
	 * be delayed rather than the echo.
	 * h->s only
	 */

#define MSG_ECTEST_GAIN 0xe21
	/*
	 * ECTEST_GAIN(gain)
	 * Amount of echo to add to test signal. Test signal used
	 * is input + gain/2^24 * reference.
	 * h->s only
	 */

#endif
