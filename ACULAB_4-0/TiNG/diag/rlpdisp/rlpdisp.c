/* rlpdisp.c - decode RLP frames */

#include <malloc.h>
#include <stdio.h>
#include <string.h>

typedef struct {
	char *bufstart, *bufend, *sp, *ep;
} STRING;

static int readmore(STRING *s, FILE *f)
{
 int d = s->sp - s->bufstart;
 if (d) {
 	d = s->ep - s->sp;
 	memmove(s->bufstart, s->sp, d);
 	s->sp = s->bufstart;
 	s->ep = s->sp + d;
 }
 d = 0;
 while (s->bufend - s->ep >= 8) {
 	int c = getc(f);
 	int i;
 	if (c == EOF) return !d;
 	for (i=0; i<8; i++) {
 		*s->ep++ = (c & (0x80 >> i)) ? '1' : '0';
	}
	d++;
 }
 return 0;
}

static int scmpstr(STRING *l, char *rsp, char *rep)
{
 char *lsp = l->sp;
 for (;;) {
 	if (lsp == l->ep) {
 		if (rsp == rep) return 0;
 		else return -1;
	} else if (rsp == rep) return 1;
	else if (*lsp != *rsp) return *lsp - *rsp;
	lsp++;
	rsp++;
 }
}

static int slen(STRING *s)
{
 return s->ep - s->sp;
}

static void c_string(STRING *s)
{
 s->bufstart = s->bufend = s->sp = s->ep = 0;
}

static int reserve_string(STRING *s, int initlen)
{
 if (s->bufend - s->bufstart < initlen) {
	free(s->bufstart);
	s->sp = s->ep = s->bufstart = malloc(initlen);
	if (!s->bufstart) return 1;
	s->bufend = s->bufstart + initlen;
 }
 return 0;
}

static int repl_string(STRING *s, char *initsp, char *initep)
{
 unsigned len;
 if (initep) len = initep - initsp;
 else len = strlen(initsp);
 if (reserve_string(s, len)) return 1;
 memcpy(s->sp, initsp, len);
 s->ep = s->sp + len;
 return 0;
}

static void printrlp(char *sp, char *ep, char *pp)
{
 unsigned crc = 0xffffff00;
 unsigned nbytes = 0;
 unsigned nbits = 0;
 unsigned byte = 0;
 unsigned dtx = 0;
 printf("\n");
 for (; sp < ep; sp++, pp++) {
	if (*pp == 'd') {
		dtx = (dtx << 4) + (*sp == '1');
	} else if (*pp == '.') {
		if (*sp == '1') {
			byte |= 1 << nbits;
			crc ^= 0x80000000;
		}
		if (crc & 0x80000000) crc ^= 0xbba1b500 >> 1;
		crc <<= 1;
		if (++nbits == 8) {
			printf("%02x", byte);
			byte = 0;
			nbits = 0;
			if ((++nbytes & 3) == 2 || nbytes == 27) printf(" ");
		}
	}
 }
 printf(crc == 0x6d893000 ? "+" : "-");
 printf("%04x", dtx);
}

static int rlpdisp(FILE *inf)
{
 unsigned copies = 0;
 int junk = 0;
 STRING prev;
 STRING pat;
 STRING s;
 c_string(&s);
 c_string(&pat);
 c_string(&prev);
 repl_string(&pat, "000000001.......1.......1.......1.......1d00....1.......1.......1.......1.......000000001.......1.......1.......1.......1d01....1.......1.......1.......1.......000000001.......1.......1.......1.......1d10....1.......1.......1.......1.......000000001.......1.......1.......1.......1d11....1.......1.......1.......1.......", 0);
 reserve_string(&s, 1024);
 for (;;) {
 	char *sp, *pp;
 	if (slen(&s) < slen(&pat)) {
 		if (readmore(&s, inf)) {
 			if (copies > 1) printf(" x%d", copies);
			junk += s.ep - s.sp;
			if (junk) printf("\n %d junk", junk);
			printf("\n");
 			return 0;
 		}
 	}
 	for (sp = s.sp, pp=pat.sp; ; ) {
 		if (pp == pat.ep) {
 			if (copies && !scmpstr(&prev, s.sp, sp)) copies++;
 			else {
 				if (copies > 1) printf(" x%d", copies);
 				copies = 1;
 				if (junk) {
 					if (junk <= s.sp - s.bufstart) {
 						printf("\n junk:%.*s",
							junk, s.sp - junk);
					} else printf("\n %d junk", junk);
				}
 				printrlp(s.sp, sp, pat.sp);
 				repl_string(&prev, s.sp, sp);
 				junk = 0;
 			}
 			s.sp = sp;
 			break;
 		} else if (*pp == '.' || *pp == 'd') {
 			pp++;
 			sp++;
		} else if (*sp == *pp) {
			pp++;
			sp++;
		} else {
			s.sp++;
			junk++;
 			if (copies > 1) printf(" x%d", copies);
 			copies = 0;
			break;
		}
	}
 }
}

static const char version[] = "@(#)$from: " __FILE__ " "
#include "../../gen/version.i"
	" $";

int main(int argc, char **argv)
{
 (void) argc;
 argv++;
 if (*argv && argv[1]) {
	fprintf(stderr, "Usage: rlpdisps [file]\n");
	return 1;
 }
 if (*argv) {
 	FILE *f = fopen(*argv, "rb");
 	if (!f) {
 		perror("fopen failed");
 		fprintf(stderr, "File: %s\n", *argv);
 		return 1;
	}
	return rlpdisp(f);
 }
 return rlpdisp(stdin);
}
