/* iir.h - IIR order 4 filter implementation */

#ifndef INCLUDED_IIR4_H
#define INCLUDED_IIR4_H

	// calculate IIR coefficients a1..b2
LOCALDEC void create_iir_4(double a1[2], double a2[2], double b1[2],
double b2[2], double *gain, double rej, double f);

#endif
