#ifndef INCLUDED_FIR_H
#define INCLUDED_FIR_H

typedef struct {
	unsigned n;
	float *co;
} FIR;

typedef struct {
	FIR *fir;
	float *hist;
	unsigned ofs;
} FIR_FILTER;

FIR *create_fir_remez(unsigned order, float atten, float ripple, float lowlim, float highlim, float trband, unsigned samprate);

FIR *create_fir_coeffs(unsigned order, float *coeff);

void delete_fir(FIR *fir);

FIR_FILTER *create_fir_filter(FIR *fir);

float fir_filter(FIR_FILTER *ffp, float x);

void delete_fir_filter(FIR_FILTER *ifp);

#endif
