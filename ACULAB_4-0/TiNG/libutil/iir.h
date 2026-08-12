#ifndef INCLUDED_IIR_H
#define INCLUDED_IIR_H

typedef struct {
	unsigned n2;	// floor((N+1)/2)
	float gain;
	struct coeff {
		float a1, a2, b1, b2;
	} c[1];	// really [n2]
} IIR;

typedef struct {
	const IIR *iir;
	float w[1][2];	// really [iir->n2*][2]
} IIR_FILTER;

// n = filter order, rej = ratio of signal at frequency (f * sampling rate)
IIR *create_iir(unsigned n, float rej, float f);

IIR_FILTER *create_iir_filter(IIR *iir);

float iir_filter(IIR_FILTER *ifp, float x);

void delete_iir(IIR *iir);

void delete_iir_filter(IIR_FILTER *ifp);

#endif
