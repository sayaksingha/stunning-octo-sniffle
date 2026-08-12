/* switch.c - command-line control of aculab switch driver */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef TiNGTYPE_WINNT
#define WIN32
#endif

#include "acu_type.h"
#include "res_lib.h"
#include "sw_lib.h"
typedef ACU_CARD_ID SWITCH_CARD;

#define arlen(x) (sizeof(x)/sizeof(*(x)))

#ifdef TiNGTYPE_WINNT
/* disable stupid warnings about conversions to smaller types */

#pragma warning(disable:4761)
#pragma warning(disable:4244)
#pragma warning(disable:4305)

#endif

static const char version[] = "@(#)$from: " __FILE__ " "
#include "../../gen/version.i"
	" $";

static void pbin(unsigned long i)
{
 char buff[42], *bp = &buff[arlen(buff)];
 int n=0;
 *--bp = 0;
 do {
	*--bp = '0' + (i & 1);
	if (!(++n & 3)) *--bp = ' ';
 } while (i >>= 1);
 if (*bp == ' ') bp++;
 printf(bp);
}

static int showone(SWITCH_CARD card, struct output_parms *p)
{
 int i = sw_query_output(card, p);
 if (i) {
	if (i == -210 || i == -211) return i;
	fprintf(stderr, "sw_query_output(card=%d, %d:%d) returned %d\n",
		card, p->ist, p->its, i);
	return 1;
 }
 switch (p->mode) {
 default:
	printf("%d:%d mode=%d, pattern=%02x, in=%d:%d\n",
		p->ost, p->ots, p->mode, p->pattern, p->ist, p->its);
	break;
 case DISABLE_MODE: break;
 case PATTERN_MODE:
	printf("%d:%d 0x%02x\n", p->ost, p->ots, p->pattern);
	break;
 case CONNECT_MODE:
	printf("%d:%d %d:%d\n", p->ost, p->ots, p->ist, p->its);
	break;
 }
 return 0;
}


static int show(SWITCH_CARD card, char **argv)
{
 struct output_parms p;
 INIT_ACU_STRUCT(&p);
 if (!*argv) {
 	int maxstream = 64;
 	p.ost = 0;
 	p.ots = 0;
 	for (;;) {
 		switch (showone(card, &p)) {
 		case -211:
			p.ots = 0;
			p.ost++;
			break;
 		case -210:
			if (p.ost >= maxstream) return 0;
 			p.ost++;
 			break;
 		case 0:
			maxstream = p.ost;
			p.ots++;
			break;
 		default:
			return 1;
 		}
	}
 } else {
 	char *ep;
 	p.ost = strtoul(*argv, &ep, 0);
	if (*ep == ':') {
		p.ots = strtoul(ep+1, &ep, 0);
		if (*ep) {
			fprintf(stderr, "junk after number: '%s'\n", ep);
			return 2;
		}
		if (argv[1]) {
			fprintf(stderr, "too many arguments\n");
			return 2;
		}
 		switch (showone(card, &p)) {
 		case -211:
			fprintf(stderr, "Invalid timeslot: %d\n", p.ots);
			return 1;
 		case -210:
			fprintf(stderr, "Invalid stream: %d\n", p.ost);
			return 1;
 		case 0: break;
 		default: return 1;
 		}
	} else if (*ep) {
		fprintf(stderr, "junk after number: '%s'\n", ep);
		return 2;
	} else {
 		p.ots = 0;
		if (argv[1]) {
			fprintf(stderr, "too many arguments\n");
			return 2;
		}
 		for (;;) {
			switch (showone(card, &p)) {
			case -211: return 0;
			case -210: 
				fprintf(stderr, "Invalid stream: %d\n", p.ost);
				return 1;
			case 0: p.ots++; break;
			default: return 1;
			}
		}
	}
 }
 return 0;
}

static int sample(SWITCH_CARD card, char **argv)
{
 int sts;
 struct sample_parms p;
 char *ep;
 INIT_ACU_STRUCT(&p);
 if (!*argv) return 2;
 p.ist = strtoul(*argv, &ep, 0);
 if (*ep != ':') {
 	fprintf(stderr, "expected ':' after stream, got '%s'\n", ep);
 	return 2;
 }
 p.its = strtoul(ep+1, &ep, 0);
 if (*ep) {
 	fprintf(stderr, "junk after number: '%s'\n", ep);
 	return 2;
 }
 if (*++argv) {
	fprintf(stderr, "too many arguments\n");
	return 2;
 }
 sts = sw_sample_input(card, &p);
 if (sts) fprintf(stderr, "sw_sample_input(card=%d, %d:%d) returned %d\n",
 	card, p.its, p.ist, sts);
 printf("%d:%d sample=%02x\n", p.ist, p.its, p.sample & 0xff);
 return 0;
}

static int set(SWITCH_CARD card, char **argv)
{
 int sts;
 struct output_parms p;
 char *ep;
 INIT_ACU_STRUCT(&p);
 if (!*argv) return 2;
 p.ost = strtoul(*argv, &ep, 0);
 if (*ep != ':') {
 	fprintf(stderr, "expected ':' after stream, got '%s'\n", ep);
 	return 2;
 }
 p.ots = strtoul(ep+1, &ep, 0);
 if (*ep) {
 	fprintf(stderr, "junk after number: '%s'\n", ep);
 	return 2;
 }
 ++argv;
 if (!*argv) p.mode = DISABLE_MODE;
 else {
	p.ist = p.pattern = strtoul(*argv, &ep, 0);
	if (*ep == ':') {
		p.its = strtoul(ep+1, &ep, 0);
		if (*ep) {
			fprintf(stderr, "junk after number: '%s'\n", ep);
			return 2;
		}
		p.mode = CONNECT_MODE;
	} else if (*ep) {
		fprintf(stderr, "expected ':' after stream, got '%s'\n", ep);
		return 2;
	} else {
		p.mode = PATTERN_MODE;
	}
	if (*++argv) {
		fprintf(stderr, "too many arguments\n");
		return 2;
	}
 }
 sts = sw_set_output(card, &p);
 if (sts) fprintf(stderr, "sw_set_output(card=%d, mode=%d, %d:%d) returned %d\n",
 	card, p.mode, p.ost, p.ots, sts);
 return sts;
}

static int caps(SWITCH_CARD card, char **argv)
{
 struct capabilities_parms p;
 unsigned i;
 int sts;
 if (*argv) {
	fprintf(stderr, "too many arguments\n");
	return 2;
 }
 INIT_ACU_STRUCT(&p);
 sts = sw_query_switch_caps(card, &p);
 printf("sw_query_switch_caps(%d) returned %d\n", card, sts);
 printf("\tsize =\t%d\n\trevision =\t%d\n\tdomain =\t", p.size, p.revision);
 pbin(p.domain);
 printf("\n\trouting =\t");
 pbin(p.routing);
 printf("\n\tblocking =\t");
 pbin(p.blocking);
 printf("\n\tnetworks =\t%d\n", p.networks);
 for (i=0; i<arlen(p.channels); i++) {
	printf("\tchannels[%d] = %2d\n", i, p.channels[i]);
 }
 return 0;
}

static int cmdeq(char *n, char *c)
{
 for (;;) {
 	if (*n == ' ') return !*c;
 	if (!*c) return 1;
 	if (*n != *c) return 0;
 	n++;
 	c++;
 }
}

static int V6OpenSwitch(ACU_CARD_ID *card, char *serial_num)
{
 {
  ACU_OPEN_CARD_PARMS ocparms;
  int err;
  INIT_ACU_STRUCT(&ocparms);
  strcpy(ocparms.serial_no, serial_num);
  err = acu_open_card(&ocparms);
  if (err) {
	fprintf(stderr, "failed to open card '%s': %d\n", serial_num, err);
	return err;
  }
  *card = ocparms.card_id;
 }
 {
  ACU_OPEN_SWITCH_PARMS osparms;
  int err;
  INIT_ACU_STRUCT(&osparms);
  osparms.card_id = *card;
  err = acu_open_switch(&osparms);
  if (err) {
	fprintf(stderr, "failed to open switch: %d\n",err);
	return err;
  }
 }
 return 0;
}

static int V6CloseSwitch(ACU_CARD_ID card)
{
 {
  ACU_CLOSE_SWITCH_PARMS csparms;
  int err;
  INIT_ACU_STRUCT(&csparms);
  csparms.card_id = card;
  err = acu_close_switch(&csparms);
  if (err) {
	fprintf(stderr, "failed to close switch, err = %d\n",err);
	return err;
  }
 }
 {
  ACU_CLOSE_CARD_PARMS ccparms;
  int err;
  INIT_ACU_STRUCT(&ccparms);
  ccparms.card_id = card;
  err = acu_close_card(&ccparms);
  if (err) {
	fprintf(stderr, "failed to close card, err = %d\n",err);
	return err;
  }
 }
 return 0;
}

static int args(char ***argv, SWITCH_CARD *card)
{
 int nocard = 1;
 for (; **argv && ***argv == '-'; ++*argv) {
	if (!strcmp(**argv, "-x")) {
		++*argv;
		nocard = 0;
		if (V6OpenSwitch(card, **argv)) return 1;
	}
	 else return 1;
 }
 return nocard;
}

struct {
	char *name;
	int (*fn)(SWITCH_CARD, char **);
} fntbl[] = {
	{"capabilities", caps, },
	{"sample in:in", sample, },
	{"set out:out", set, },
	{"set out:out pattern", set, },
	{"set out:out i:i", set, },
	{"show", show, },
	{"show stream", show, },
	{"show stream:timeslot", show, },
};

int main(int argc, char **argv)
{
 SWITCH_CARD card = 0;
 int err = 0;
 unsigned i;
 (void) argc;
 ++argv;
 if (args(&argv, &card) || !*argv) {
 	fprintf(stderr, "Usage: switch -x serialno command\n");
 	return 1;
 }
 for (i=0; ; i++) {
 	if (i >= arlen(fntbl)) {
	 	fprintf(stderr, "Usage: switch -x serialno command\nCommands:\n");
		for (i=0; i<arlen(fntbl); i++) {
			fprintf(stderr, "\t%s\n", fntbl[i].name);
		}
		err = 1;
		break;
 	}
 	if (cmdeq(fntbl[i].name, *argv)) {
 		switch ((*fntbl[i].fn)(card, argv+1)) {
 		case 2:
 			fprintf(stderr, "Usage: switch -x serialno %s\n",
 				fntbl[i].name);
		case 1:
			err = 1;
			break;
		case 0:
			break;
		}
		break;
	}
 }
 if (V6CloseSwitch(card)) err = 1;
 return err;
}
