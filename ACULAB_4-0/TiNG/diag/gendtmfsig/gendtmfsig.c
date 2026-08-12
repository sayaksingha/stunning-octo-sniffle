/* gendtmfsig.c - Generate a DTMF test signal to a file */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../libutil/alaw.h"
#include "../../libutil/gasdev.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

#define AMP_0dB (2*2853.5)      // amplitude of a cosine at 0dBm0

#define SAMPRATE 8000
#define F2W(x) ((x) * M_PI * 2 / SAMPRATE)

#ifndef LOG10
#define LOG10(X) (log(X)/2.302585092994)
#define EXP10(X) exp( (X)*2.302585092994 )
#endif

static double dtmflow[] = { 697.0, 770.0, 852.0, 941.0 };
static double dtmfhigh[] = { 1209.0, 1336.0, 1477.0, 1633.0 };

typedef struct {
	double percent;
	double offset;
	double a1;		// amplitude of low tone
	double a2;		// amplitude of high tone
	double f3;		// extra frequency
	double a3;		// amplitude of extra frequency
	double an;		// amplitude of noise
	double chirp;		// frequency of chirp Hz/second
	unsigned mark;
	unsigned space;
} TONESPEC;

static void gendigit(FILE *of, int l, int h, TONESPEC *tp, GASDEV *gd, LIN2ALAW_TABLE *atp)
{
 const double wc = tp->chirp ? F2W(tp->chirp) / SAMPRATE : 0;
 double w2 = F2W(dtmfhigh[h] * tp->percent + tp->offset);
 double w1 = F2W(dtmflow[l] * tp->percent + tp->offset);
 const double w3 = F2W(tp->f3);
 unsigned i;
 for(i=0; i < tp->mark; i++) {
	double temp = tp->a1 * cos(i * w1)
			+ tp->a2 * cos(i * w2)
			+ tp->a3 * cos(i * w3)
			+ tp->an * gasdev(gd);
	putc(linear_to_alaw(temp, atp), of);
	w1 += wc;
	w2 += wc;
 }
 for(i=0; i<tp->space; i++) {
	putc(linear_to_alaw(tp->an * gasdev(gd), atp), of);
 }
}

static void genpattern(FILE *of, TONESPEC *tp, LIN2ALAW_TABLE *atp)
{
 const unsigned char alaw0 = linear_to_alaw(0, atp);
 TONESPEC ws = *tp;
 GASDEV gd;
 unsigned l;
 gasdev_init(&gd);
 ws.percent = 1 + ws.percent/100;
 ws.a1 = AMP_0dB*EXP10(-ws.a1/20.0);     // convert from -dB to amplitude
 ws.a2 = AMP_0dB*EXP10(-ws.a2/20.0);     // convert from -dB to amplitude
 ws.a3 = AMP_0dB*EXP10(-ws.a3/20.0);     // convert from -dB to amplitude
	 // cosine energy is sqr(amp)/2.
 ws.an = (float) sqrt( (ws.a1 * ws.a1 + ws.a2 * ws.a2) * EXP10(-ws.an/10)/2 );
 // write 4000 samples of zero
 for (l=0; l<4000; l++) {
	putc(alaw0, of);
 }
 for (l=0; l < arlen(dtmflow); l++) {
	unsigned h;
	for(h=0; h < arlen(dtmfhigh); h++) {
		gendigit(of, l, h, &ws, &gd, atp);
		gendigit(of, l, h, &ws, &gd, atp);
	}
 }
}

static char *sksp(char *s)
{
 while (*s == ' ' || *s == '\t' || *s == '\n') s++;
 return s;
}

static int gendtmfsig(FILE *of)
{
 LIN2ALAW_TABLE lin2atab;
 unsigned char alaw0;
 char buff[1024];
 TONESPEC ts;
 int lno = 0;
 int i;
 init_alaw_encode(&lin2atab);
 alaw0 = linear_to_alaw(0, &lin2atab);
 while (fgets(buff, sizeof(buff), stdin)) {
	char *ep = sksp(buff);
	lno++;
	if (*ep == '#' || !*ep) continue;
	ts.percent = (100 + strtod(ep, &ep)) / 100;
	ts.offset = strtod(ep, &ep);
	ts.a1 = strtod(ep, &ep);
	ts.a2 = strtod(ep, &ep);
	ts.chirp = strtod(ep, &ep);
	ts.an = strtod(ep, &ep);
	ts.f3 = strtod(ep, &ep);
	ts.a3 = strtod(ep, &ep);
	ts.mark = strtol(ep, &ep, 0);
        ts.space = strtol(ep, &ep, 0);
        if (*ep != '\n') {
        	if (*ep) {
			fprintf(stderr, "Line %d: junk after number: '%s'\n", lno, ep);
        	} else {
			fprintf(stderr, "Line %d too long: '%s'\n", lno, buff);
        	}
        	return 1;
	}
	genpattern(of, &ts, &lin2atab);
 }
 for (i=0; i<SAMPRATE; i++) {
	putc(alaw0, of);
 }
 return 0;
}

#include "gen/gendtmfsig.args.i"

int main(int argc, char **argv)
{
 FILE *f;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || *argv) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
	return 1;
 }
 if (arg.filename) {
	f = fopen(arg.filename, "wb");
	if (!f) {
		perror("fopen");
		fprintf(stderr, "Cannot create file: %s\n", arg.filename);
		return 1;
	}
 } else f = stdout;
 return gendtmfsig(f);
}
