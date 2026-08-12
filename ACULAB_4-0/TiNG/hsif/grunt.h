/* grunt.h - grunt detection messages passed between host and sharc */

#ifndef INCLUDED_GRUNT_H
#define INCLUDED_GRUNT_H

#define MSG_GRUNT 0xc93
	/*
	 * GRUNT(0, duration) for grunt start
	 * GRUNT(duration, energy) for grunt end
	 * duration is in mS, energy is in dBm0
	 *
	 * s->h only
	 */

#define MSG_GRUNT_LATENCY 0xc94
	/*
	 * GRUNT_LATENCY(duration[, reminder[, holdoff[, maxi]]])
	 * sets grunt latency in mS. If reminder is specified, a
	 * reminder message (GRUNT_CONT) is sent at that time (measured
	 * in mS from start of silence). If holdoff time specified,
	 * noise floor estimation held off for that time in ms.
	 * If maxi specified, max silence before grunt (initial reminder
	 * time, reminder is used for subsequent reminder times)
	 *
	 * h->s only
	 */

#define MSG_GRUNT_CONT 0xc95
	/*
	 * GRUNT_CONT(bits_recorded)
	 * indicates continued silence, bits_recorded is number of bits
	 * recorded during the silence
	 *
	 * s->h only
	 */

#define MSG_GRUNT_LEVELS 0xc96
	/*
	 * GRUNT_LEVELS(min_noise_level,grunt_threshold)
	 * sets min_noise_level in dB scaled by 10^9
	 * and grunt_threshold as a factor scaled by 65536
	 * h->s only
	 */

#endif
