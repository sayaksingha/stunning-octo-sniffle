/* conf.h - messages passed between host and sharc for conferencing */

#ifndef INCLUDED_CONF_H
#define INCLUDED_CONF_H

#define MSG_INMASK 0xe05
	/*
	 * INMASK(mask-L, mask-H[, mask-HH[,mask-HHH[...]]])
	 * specifies the bitmask of inputs to be combined into this output
	 * h->s: sets the inputs to be used
	 * s->h: reports which inputs are currently active
	 */

#define MSG_INMASK_PART 0xe40
	/*
	 * INMASK_PART(index, mask-H[, mask-HH[,mask-HHH[...]]])
	 * specifies a part of the bitmask of inputs to be combined into this output.
	 * There can be several INMASK_PART messages but they will be followed by a
	 * single INMASK message. index is such that mask-L of INMASK is index==0.
	 * The INMASK_PART with highest index is sent first.
	 * Parts that would be all zeros may be omitted.
	 * h->s: sets the inputs to be used
	 */

#define MSG_CONFMAN 0xe06
	/*
	 * CONFMAN(task-id)
	 * specifies the confman task to be used by a confin subtask
	 * h->s only
	 */

#define MSG_AGC_MIN_LMIN 0xe07
	/*
	 * AGC_MIN_LMIN(val)
	 * sets the Min_Lmin value for AGC
	 * h->s only
	 */

#define MSG_AGC_SPEECH_THRESH 0xe0b
	/*
	 * AGC_SPEECH_THRESH(val)
	 * sets the Min_Lmin value for AGC
	 * h->s only
	 */

#define MSG_CONF_ACTIVE 0xe23
	/*
	 * CONF_ACTIVE(delay)
	 * CONF_ACTIVE(input[4])
	 * h->s: configures active input report min delay
	 * s->h: reports active inputs list
	 */

#define MSG_CIVFAC 0xe0c
	/*
	 * CIVFAC(slot, val)
	 * sets the multiplication factor for a specified input slot
	 * h->s only
	 */

#define MSG_CONF_ADJ_INPUT_SETTINGS 0xe35
	/*
	 * CONF_ADJ_INPUT_SETTINGS(lmax_decay, target)
	 * c.f. MSG_AGC_ADJ_SETTINGS(lmax_decay, target) in play.h
	 * lmax_decay is decay rate scaled by 32768
	 * target is scaled as MIN_LMIN, which is exp((dB + 90)/10 * log(10))
	 * h->s only
	 */

#endif
