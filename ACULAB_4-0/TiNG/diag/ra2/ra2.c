#include <stdio.h>
#include <stdlib.h>

static const char version[] = "@(#)$from: " __FILE__ " "
#include "../../gen/version.i"
	" $";

static void ra2(FILE *out, FILE *in, unsigned start, unsigned nbits)
{
 unsigned long pos;
 unsigned hadbits = 0;
 int c;
 for (pos=0; (c=fgetc(in)) != EOF; pos++) {
	unsigned i = nbits;
	if (hadbits >= 64) {
		fprintf(out, "\n@%lu\n", pos);
		hadbits = 0;
	}
	c <<= start;
	for (; i--; c <<= 1) fputc(c & 0x80 ? '1' : '0', out);
	hadbits += nbits;
	pos++;
 }
}

static char *readlong(long *l, char *s)
{
 *l = strtol(s, &s, 0);
 return s;
}

int main(int argc, char **argv)
{
 long start = 0, nbits;
 (void) argc;
 if (!*++argv
 	|| *readlong(&nbits, *argv++)
 	|| (*argv && *readlong(&start, *argv++))
 	|| *argv) {
 	fprintf(stderr, "Usage: ra2 nbits [startpos]\n");
 	return 1;
 }
 ra2(stdout, stdin, start, nbits);
 return 0;
}
