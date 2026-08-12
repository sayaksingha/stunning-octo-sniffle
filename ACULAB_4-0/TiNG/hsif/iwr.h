/* iwr.h - messages passed between host and sharc for isolated word recognition */

#ifndef INCLUDED_IWR_H
#define INCLUDED_IWR_H

#define MSG_IWR_MODEL_SIZE 0xcc0
	/*
	 * IWR_MODEL_SIZE(size)
	 * indicates size of model about to be downloaded. This is the
	 * number of 32-bit words it occupies, including the length, which
	 * must also be the first word. Therefore the smallest possible
	 * model is one word, consisting only of a length word with the
	 * value 1.
	 *
	 * h->s only
	 */

#define MSG_IWR_MODEL 0xcc1
	/*
	 * IWR_MODEL(id)
	 * reports ID of ASR model downloaded
	 *
	 * s->h only
	 */

#define MSG_IWR_DETECT 0xcc2
	/*
	 * IWR_DETECT(status,bestid,nextid)
	 * reports a status value and the IDs of the best model and the
	 * next best (if appropriate).
	 *
	 * s->h only
	 */

    /*
     * Status codes returned by IWR (more detailed than required by API)
     */
#define IWR_RESULT_ACCEPT       0x0
#define IWR_RESULT_UNCERTAIN    0x1
#define IWR_RESULT_REJECT       0x2
#define IWR_WARNING_TIMEOUT     0x3
#define IWR_WARNING_FAILED      0x4
#define IWR_ERROR_NONOISE       0x5
#define IWR_ERROR_EXCESSNOISE   0x6
#define IWR_ERROR_NOMODELS      0x7
#define IWR_ERROR_EXCESSMODELS  0x8
#define IWR_ERROR_EXCESSSTATES  0x9
#define IWR_ERROR_MODELCHANGED  0xA
#define IWR_ERROR_MISSING       0xB

#define MSG_IWR_SIDETONE 0xcc3
	/*
	 * IWR_DETECT(timeslot)
	 * configures the timeslot to use for sidetone suppression
	 *
	 * h->s only
	 */

#define MSG_IWR_SETPAR 0xcc4
	/*
	 * IWR_SETPAR(vfr_max_frames,
	 *	vfr_diff_threshold
	 *	pse_max_frames
	 *	pse_min_frames
	 *	vit_soft_threshold
	 *	vit_hard_threshold
	 *	vit_snr_adjust)
	 * configures IWR parameters. The thresholds and snr adjust are
	 * scaled by 65536. The others are integers.
	 *
	 * h->s only
	 */

#endif
