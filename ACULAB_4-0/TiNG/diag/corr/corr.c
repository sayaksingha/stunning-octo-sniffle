#include <stdio.h>
#include <stdlib.h>

#include "../../libutil/floio.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

static int corr(float *lsig, unsigned llen, float *rsig, unsigned rlen)
{
 unsigned olen = rlen + llen;
 unsigned opos;
 /*
  *    |---- l ----|   -->   |---- l ----|
  *    		   |--- r ---|
  *    |--------------- o ---------------|
  *
  */
 for (opos = 0; opos < olen; opos++) {
	unsigned len = rlen+llen;
	unsigned i = opos;
	unsigned j;
	float c[32];
	for (j=0; j < arlen(c); j++) c[j] = 0;
	// signals are zero outside sampled area, so 
	// from lsig: 0 <= i-opos < llen
	// from rsig: 0 <= i-llen < rlen
	// from lsig: opos <= i < llen+opos
	// from rsig: llen <= i < rlen+llen
	if (i < llen) i = llen;
	if (llen+opos < len) len = llen+opos;
	for (; i<len; i++) {
		unsigned t;
		c[0] += lsig[i - opos] * rsig[i - llen];
		for (j=0, t=opos; t & 1; t>>=1, j++) {
			c[j+1] += c[j];
			c[j] = 0;
		}
	}
	for (j=1; j < arlen(c); j++) c[0] += c[j];
	putflo(stdout, c[0]);
 }
 return 0;
}

static int readflo(float **fpp, unsigned *len, FILE *f)
{
 unsigned maxlen = 0;
 *fpp = 0;
 *len = 0;
 for (;;) {
	float samp;
	if (getflo(f, &samp)) break;
	if (*len >= maxlen) {
		float *newf;
		if (!maxlen) maxlen = 1;
		maxlen *= 2;
		newf = realloc(*fpp, sizeof(*newf) * maxlen);
		if (!newf) return 1;
		*fpp = newf;
	}
	(*fpp)[*len] = samp;
	++*len;
 }
 return 0;
}

static int readfile(float **fpp, unsigned *len, char *fname)
{
 FILE *f = fopen(fname, "rb");
 int i;
 if (!f) {
 	perror("fopen failed");
 	i = 1;
 } else {
	i = readflo(fpp, len, f);
	if (ferror(f)) i = 1;
	if (fclose(f)) {
		perror("fclose failed");
		i = 1;
	}
 }
 if (i) fprintf(stderr, "Error reading file: %s\n", fname);
 return i;
}

int main(int argc, char **argv)
{
 unsigned llen, rlen;
 float *lval, *rval;
 if (argc != 3) {
 	fprintf(stderr, "Usage: corr in1 in2\n");
 	return 1;
 }
 if (readfile(&lval, &llen, argv[1])) return 1;
 if (readfile(&rval, &rlen, argv[2])) return 1;
 return corr(lval, llen, rval, rlen);
}
