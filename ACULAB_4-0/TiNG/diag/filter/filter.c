/* filter.c - apply a filter to a signal stream */

#include "../../libutil/floio.h"
#include "../../libutil/iir.h"

#ifdef _MSC_VER

/* disable stupid warnings about doubles being converted to floats.
 */

#pragma warning(disable:4244)
#pragma warning(disable:4305)
/* and about argument conversions (does anyone still use a
   non-prototype compiler?) */
#pragma warning(disable:4761)

#endif

static int filter(FILE *of, unsigned n, double rej, double freq, FILE *inf)
{
 IIR *iir = create_iir(n, rej, freq);
 IIR_FILTER *ifp;
 if (!iir) {
 	fprintf(stderr, "Cannot create IIR filter coefficients\n");
 	return 1;
 }
 ifp = create_iir_filter(iir);
 if (!ifp) {
 	delete_iir(iir);
 	fprintf(stderr, "Cannot create IIR filter\n");
 	return 1;
 }
 for (;;) {
	float f;
	if (getflo(inf, &f)) break;
	putflo(of, iir_filter(ifp, f));
 }
 delete_iir_filter(ifp);
 return 0;
}

#include "gen/filter.args.i"

int main(int argc, char **argv)
{
 FILE *outf;
 FILE *inf;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || *argv) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
	return 1;
 }
 if (arg.infile) {
	inf = fopen(arg.infile, "rb");
	if (!inf) {
		perror("fopen");
		fprintf(stderr, "Cannot open file: %s\n", *argv);
		return 1;
	}
 } else inf = stdin;
 if (arg.outfile) {
	outf = fopen(arg.outfile, "wb");
	if (!outf) {
		perror("fopen");
		fprintf(stderr, "Cannot create file: %s\n", arg.outfile);
		return 1;
	}
 } else outf = stdout;
 return filter(outf, arg.order, arg.rej_ratio, arg.freq_ratio, inf);
}
