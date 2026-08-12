/* hdlc.c - interpret HDLC data */

/*
 * Prints:
 *	'F <n>' a number of flags
 *	'S <n>' a number of flags with shared zeros
 *	'A <n>' abort (n bits long)
 *	'junk <n>' n bits while awaiting flag
 *	<hex> a frame of data ending with:
 *		<hex>:<n>	n odd bits (lsb first)
 *		d=<n>		total data bits in frame
 *		s=<n>		number of bit-stuffed zeros removed
 */

#include <stdio.h>
#include <stdlib.h>

static int getbit(FILE *f, unsigned *inpos)
{
 for (;;) {
	int c = getc(f);
	if (c == EOF || c == '0' || c == '1') return c;
	for (;;) {
		if (c == '@') {
			*inpos = 0;
			for (;;) {
				c = getc(f);
				if (c == EOF) return c;
				if (c < '0' || c > '9') break;
				*inpos *= 10;
				*inpos += c - '0';
			}
		} else putchar(c);
		if (c == '\n') break;
		c = getc(f);
		if (c == EOF) return c;
	}
 }
}

struct framestate {
	unsigned long flags;
	unsigned long sflags;
	unsigned *inpos;
};

static void flushfrm(struct framestate *fs)
{
 if (fs->flags) {
	printf("F %ld\n", fs->flags);
	fs->flags=0;
	if (*fs->inpos) printf("@%u\n", *fs->inpos);
 }
 if (fs->sflags) {
	printf("S %ld\n", fs->sflags);
	fs->sflags=0;
	if (*fs->inpos) printf("@%u\n", *fs->inpos);
 }
}

/* reads frames and returns having read
 *	a) the first 0 after an abort
 *	b) EOF
 */
static enum { RF_EOF, RF_HAD0, } readframe(FILE *inf, unsigned *inpos)
{
 unsigned rawrx=0xfc0, strx=0;
 int gotbits = -7;
 unsigned long framedbits=0, framestuffbits=0;
 struct framestate fs;
 fs.inpos = inpos;
 fs.flags = 1;
 fs.sflags = 0;
 for (;;) {
	int i;
	/* rawrx holds the last 8+5 bits of raw input data (oldest in lsb)
	 * strx holds processed input data (after bit stuffing) from lsb
	 * gotbits is number of valid bits in strx
	 * framedbits is total number of bits already printed
	 * framestuffbits is number of stuffed zeros deleted
	 * flags is number of non-shared flags not yet printed
	 * sflags is number of shared flags not yet printed
	 *	note that only one of sflags and flags can be non-zero
	 */
	if ((i = getbit(inf, inpos)) == EOF) break;
	i = i=='1';
 	if (i) rawrx |= 0x2000;
 	rawrx >>= 1;
 	if ((rawrx & 0xfe0) == 0xfc0) { // flag or abort: got ?1111110ddddd
		if (rawrx & 0x1000) {	// abort
			// we haven't yet processed the last 0 before the abort
			if (gotbits >= 0 && (rawrx & 0x1f) != 0x1f) {	// not bit stuffing
				strx |= ((rawrx >> 5) & 1) << gotbits;
				++gotbits;
			}
		}
		if (gotbits > 0) {			// got more than flag
			flushfrm(&fs);
			if (gotbits == 8) printf(" %02x", strx);
			else printf(" %02x:%d", strx, gotbits);
			framedbits += gotbits;
		}
		if (framedbits) printf(" d=%ld, s=%ld\n", framedbits, framestuffbits);
		if (rawrx & 0x1000) {	// abort
			flushfrm(&fs);
			gotbits = 7;
			for (;;) {
				i = getbit(inf, inpos);
				if (i != '1') break;
				gotbits++;
			}
			printf("A %d\n", gotbits);
			if (*inpos) printf("@%u\n", *inpos);
			return i==EOF ? RF_EOF : RF_HAD0;
		}
		if (gotbits > 0 || rawrx != 0xfdf) {	// non-shared flag
			if (fs.sflags) {
				printf("S %ld\n", fs.sflags);
				fs.sflags=0;
				if (*inpos) printf("@%u\n", *inpos);
			}
			fs.flags++;
		} else {
			if (fs.flags) {
				printf("F %ld\n", fs.flags);
				fs.flags=0;
				if (*inpos) printf("@%u\n", *inpos);
			}
			fs.sflags++;
		}
		framedbits = framestuffbits = 0;
		strx = 0;
		gotbits = -7;
		// end of frame
	} else if (gotbits < 0) gotbits++;
	else if ((rawrx & 0x1f) != 0x1f) {	// not bit stuffing
		strx |= ((rawrx >> 5) & 1) << gotbits;
		if (++gotbits == 8) {
			flushfrm(&fs);
			printf(" %02x", strx);
			strx = 0;
			gotbits = 0;
			framedbits += 8;
		}
	} else {
		framestuffbits++;
	}
 }
 flushfrm(&fs);
 if (gotbits > 0) {
	framedbits += gotbits;
 	printf(" %02x:%d", strx, gotbits);
 }
 if (framedbits) printf(" d=%ld, s=%ld\n", framedbits, framestuffbits);
 return RF_EOF;
}

static int hdlc(FILE *inf)
{
 unsigned long junkbits = 0;
 unsigned inpos = 0;
 int i;
 goto getzero;
 for (;;) {
 	// got 0
 await_flg1:
 	if ((i = getbit(inf, &inpos)) == EOF) break;
 	junkbits++;
 	if (i == '0') goto await_flg1;
 	// got 01
 	if ((i = getbit(inf, &inpos)) == EOF) break;
 	junkbits++;
 	if (i == '0') goto await_flg1;
 	// got 011
 	if ((i = getbit(inf, &inpos)) == EOF) break;
 	junkbits++;
 	if (i == '0') goto await_flg1;
 	// got 0111
 	if ((i = getbit(inf, &inpos)) == EOF) break;
 	junkbits++;
 	if (i == '0') goto await_flg1;
 	// got 01111
 	if ((i = getbit(inf, &inpos)) == EOF) break;
 	junkbits++;
 	if (i == '0') goto await_flg1;
 	// got 011111
 	if ((i = getbit(inf, &inpos)) == EOF) break;
 	junkbits++;
 	if (i == '0') goto await_flg1;
 	// got 0111111
 	if ((i = getbit(inf, &inpos)) == EOF) break;
 	junkbits++;
 	if (i == '1') {
		if (junkbits > 6) printf("junk %ld\n", junkbits-6);
		junkbits = 6;
		if (i == EOF) break;
		if (inpos) printf("@%u\n", inpos);
 getzero:
		while ((i = getbit(inf, &inpos)) == '1') junkbits++;
		if (junkbits) printf("A %ld\n", junkbits);
		if (inpos) printf("@%u\n", inpos);
 	} else {
		// got 01111110 - flag
		junkbits -= 8;
		if (junkbits) {
			printf("junk %ld\n", junkbits);
			if (inpos) printf("@%u\n", inpos);
		}
		if (readframe(inf, &inpos) == RF_EOF) i = EOF;
	}
	junkbits = 0;
	if (i == EOF) break;
 }
 if (junkbits) printf("junk %ld\n", junkbits);
 return 0;
}

#include "gen/hdlc.args.i"

int main(int argc, char **argv)
{
 FILE *inf;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || (*argv && argv[1])) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE " infile\n", progname);
	return 1;
 }
 if (*argv) {
	inf = fopen(*argv, "rb");
	if (!inf) {
		perror("fopen");
		fprintf(stderr, "Cannot open file: %s\n", *argv);
		return 1;
	}
 } else inf = stdin;
 return hdlc(inf);
}
