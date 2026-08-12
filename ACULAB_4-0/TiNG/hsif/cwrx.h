/* cwrx.h - messages passed between host and sharc for cw rx */

#ifndef INCLUDED_CWRX_H
#define INCLUDED_CWRX_H

#define MSG_CWRX 0xc93
	/*
	 * Original algorithm
	 * CWRX(f_c1, gt_c1, lt_c1)
	 * sets filter coefficieints.
	 * f_cq = 2 * cos(f)
	 * gt_c1 = 2 * cos(f+50Hz)
	 * lt_c1 = 2 * cos(f-50Hz)
	 * Each value is scaled by 0x01000000
	 *
	 * Enhanced algorithm
	 * CWRX(spc, f_qfi, f_qfr, spd)
	 * sets quad filter coefficieints.
	 * spc = samples per cycle (e.g. 8 for 1kHz)
	 * f_qfi = 2 * sin(pi/spc)
	 * f_qfr = 2 * cos(pi/spc)
	 * spd = speed indication in bits per second
	 * Above f_qfi and f_qfr values are scaled by 0x01000000
	 * h->s only
	 */
#endif
