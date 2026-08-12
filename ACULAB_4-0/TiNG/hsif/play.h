/* play.h - messages passed between host and sharc for play/record etc */

#ifndef INCLUDED_PLAY_H
#define INCLUDED_PLAY_H

#define MSG_VOLUME 0xe01
	/*
	 * VOLUME(gain)
	 * sets the volume for an output channel (in dB) -32 < gain < 32
	 * h->s only
	 */

#define MSG_AGC 0xe02
	/*
	 * VOLUME(gain)
	 * sets the volume for an output channel (in dB) -32 < gain < 32
	 * h->s only
	 */

#define MSG_AGC_SETGAIN 0xe04
	/*
	 * AGC_SETGAIN(gain)
	 * sets the current gain (which will be modified by the AGC algorithm)
	 * Values are in dB: -32 < gain < 32
	 * h->s only
	 */

#define MSG_AUXOUT 0xe03
	/*
	 * AUXOUT(timeslot)
	 * sets the timeslot to be added to the output
	 * h->s only
	 */

#define MSG_AGC_ADJ_SETTINGS 0xe35
	/*
	 * MSG_AGC_ADJ_SETTINGS(lmax_decay, target)
	 * c.f. CONF_ADJ_INPUT_SETTINGS(lmax_decay, target) in conf.h
	 * lmax_decay is decay rate scaled by 32768
	 * target is scaled as MIN_LMIN, which is exp((dB + 90)/10 * log(10))
	 * h->s only
	 */

#endif
