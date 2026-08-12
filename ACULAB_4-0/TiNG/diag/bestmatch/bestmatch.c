#include <stdio.h>
#include <stdlib.h>

#define arlen(x) (sizeof(x)/sizeof(*(x)))

static int bestmatch(int offset, unsigned maxwin, char *lsig, unsigned llen, char *rsig, unsigned rlen)
{
 unsigned obest=0, obestmat=0;
 unsigned opos, olen;
 /*
  *    |---- l ----|   -->   |---- l ----|
  *    		   |--- r ---|
  *    |--------------- o ---------------|
  *
  */
 if (maxwin) {
	if (llen >= maxwin - offset) opos = llen + offset - maxwin;
	else opos = 0;
	if (offset + maxwin > rlen) olen = llen + rlen;
	else olen = llen + offset + maxwin;
 } else {
	opos = 0;
	olen = maxwin = rlen + llen;
 }
 for (; opos < olen; opos++) {
	unsigned len = rlen+llen;
	unsigned i = opos;
	unsigned nmat = 0;
	// signals don't match outside sampled area, so 
	// from lsig: 0 <= i-opos < llen
	// from rsig: 0 <= i-llen < rlen
	// from lsig: opos <= i < llen+opos
	// from rsig: llen <= i < rlen+llen
	if (i < llen) i = llen;
	if (llen+opos < len) len = llen+opos;
	for (; i<len; i++) {
		nmat += lsig[i - opos] == rsig[i - llen];
	}
	if (nmat > obestmat) {
		obestmat = nmat;
		obest = opos;
	}
 }
 if (obestmat) {
	unsigned mlen = rlen < llen ? rlen : llen;
	printf("%d: %d (%g)\n", obest - llen, obestmat, obestmat / (double) mlen);
 }
 return 0;
}

static int readblk(char **fpp, unsigned *len, FILE *f)
{
 unsigned maxlen = 0;
 *fpp = 0;
 *len = 0;
 for (;;) {
	int c = getc(f);
	if (c == EOF) return 0;
	if (*len >= maxlen) {
		char *newf;
		if (!maxlen) maxlen = 1;
		maxlen *= 2;
		newf = realloc(*fpp, sizeof(*newf) * maxlen);
		if (!newf) return 1;
		*fpp = newf;
	}
	(*fpp)[*len] = c;
	++*len;
 }
}

static int readfile(char **fpp, unsigned *len, char *fname)
{
 FILE *f = fopen(fname, "rb");
 int i;
 if (!f) {
 	perror("fopen failed");
 	i = 1;
 } else {
	i = readblk(fpp, len, f);
	if (ferror(f)) i = 1;
	if (fclose(f)) {
		perror("fclose failed");
		i = 1;
	}
 }
 if (i) fprintf(stderr, "Error reading file: %s\n", fname);
 return i;
}

#include "gen/bestmatch.args.i"

int main(int argc, char **argv)
{
 unsigned llen, rlen;
 char *lval, *rval;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || !*argv || !argv[1] || argv[2]) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE " in1 in2\n", progname);
	return 1;
 }
 if (readfile(&lval, &llen, *argv++)) return 1;
 if (readfile(&rval, &rlen, *argv)) return 1;
 return bestmatch(arg.offset, arg.maxwin, lval, llen, rval, rlen);
}
