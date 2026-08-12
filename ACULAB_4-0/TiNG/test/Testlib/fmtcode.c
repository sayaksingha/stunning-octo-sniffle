/* fmtcode.c - convert format name to format code */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fmtcode.h"
#include "smbesp.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

static const struct {
	const char *name;
	unsigned code;
} fmtname[] = {
#ifdef PROSODY_TiNG
	{ "OKIADPCM", kSMDataFormatOKIADPCM, },
	{ "oki", kSMDataFormatOKIADPCM, },
	{ "ACUBLKPCM", kSMDataFormatACUBLKPCM, },
	{ "acu", kSMDataFormatACUBLKPCM, },
	{ "ULawPCM", kSMDataFormatULawPCM, },
	{ "mu", kSMDataFormatULawPCM, },
	{ "ALawPCM", kSMDataFormatALawPCM, },
	{ "a", kSMDataFormatALawPCM, },
	{ "8bit", kSMDataFormat8bit, },
	{ "8", kSMDataFormat8bit, },
	{ "Signed8bit", kSMDataFormatSigned8bit, },
	{ "8s", kSMDataFormatSigned8bit, },
	{ "16bit", kSMDataFormat16bit, },
	{ "16", kSMDataFormat16bit, },
	{ "CELP8KBPS", kSMDataFormatCELP8KBPS, },
	{ "IMAADPCM", kSMDataFormatIMAADPCM, },
	{ "IMA", kSMDataFormatIMAADPCM, },
	{ "None", kSMDataFormatNone, },
	{ "n", kSMDataFormatNone, },
	{ "SPEEX", kSMDataFormatSpeex, },
#else
	{ "a-mu", kSMDataFormat8KHzPCM, },
	{ "oki", kSMDataFormat8KHzOKIADPCM, },
	{ "acu", kSMDataFormat8KHzACUBLKPCM, },
	{ "6a-mu", kSMDataFormat6KHzPCM, },
	{ "6oki", kSMDataFormat6KHzOKIADPCM, },
	{ "6acu", kSMDataFormat6KHzACUBLKPCM, },
	{ "mu", kSMDataFormat8KHzULawPCM, },
	{ "a", kSMDataFormat8KHzALawPCM, },
	{ "CELP", kSMDataFormatCELP8KBPS, },
	{ "726-48", kSMDataFormatG726_48KBPS, },
	{ "726-32", kSMDataFormatG726_32KBPS, },
	{ "726-24", kSMDataFormatG726_24KBPS, },
	{ "726-16", kSMDataFormatG726_16KBPS, },
	{ "721", kSMDataFormatG721, },
	{ "16", kSMDataFormat8KHz16bitMono, },
	{ "8", kSMDataFormat8KHz8bitMono, },
	{ "ima", kSMDataFormatIMAADPCM, },
#endif
};

static const char *prefix(const char *p, const char *s)
{
 for (;;) {
 	if (!*p) return s;
 	if (*p++ != *s++) return 0;
 }
}

int fmtcode(enum kSMDataFormat *fp, tSM_UT32 *ratep, const char *fmt)
{
 unsigned i;
 if (!*fmt) {
	fprintf(stderr, "Null format\n");
	return 1;
 }
 if (*fmt == '#') {
	char *t;
	*fp = strtol(fmt+1, &t, 0);
	fmt = t;
 } else for (i=0; i < arlen(fmtname); i++) {
	const char *s;
	if (i >= arlen(fmtname)) return 1;
	s = prefix(fmtname[i].name, fmt);
	if (s && (!*s || *s == '@')) {
		*fp = fmtname[i].code;
		fmt = s;
		break;
	}
 }
 if (*fmt && *fmt != '@') {
	fprintf(stderr, "Junk after format: '%s'\n", fmt);
	return 1;
 }
 if (!*fmt) *ratep = 8000;		// supply default
 else {
	char *t;
	*ratep = strtol(fmt+1, &t, 0);
	if (*t) {
		fprintf(stderr, "Junk after rate: '%s'\n", t);
		return 1;
	}
 }
 return 0;
}

int fmtlist_open(FMTLIST *fl)
{
 fl->pos = 0;
 return 0;
}

const char *fmtlist_name(FMTLIST *fl)
{
 if (fl->pos >= arlen(fmtname)) return 0;
 return fmtname[fl->pos].name;
}

int fmtlist_next(FMTLIST *fl)
{
 if (fl->pos >= arlen(fmtname)-1) return 1;
 fl->pos++;
 return 0;
}

int fmtlist_close(FMTLIST *fl)
{
 (void) fl;
 return 0;
}

int fmtlist_dump(FILE *outf)
{
 const unsigned colwid = 16;
 unsigned col = 0;
 unsigned i;
 for (i=0; i < arlen(fmtname); i++) {
	unsigned spc;
	if (!(i & 3)) {
		putc('\n', outf);
		col = 0;
	}
	spc = colwid - (col-8) % colwid;
	col += spc;
	while (spc--) putc(' ', outf);
	fputs(fmtname[i].name, outf);
	col += strlen(fmtname[i].name);
 }
 putc('\n', outf);
 fprintf(outf, "All formats can be followed by '@' and sampling rate in Hz\n");
 return 0;
}
