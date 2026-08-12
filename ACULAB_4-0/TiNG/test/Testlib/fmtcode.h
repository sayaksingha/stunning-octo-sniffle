/* fmtcode.h - declarations for converting format name to format code */

#ifndef INCLUDED_FMTCODE_H
#define INCLUDED_FMTCODE_H

#include <stdio.h>
#include "smbesp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	unsigned pos;
} FMTLIST;

int fmtcode(enum kSMDataFormat *fp, tSM_UT32 *ratep, const char *fmt);
int fmtlist_open(FMTLIST *fl);
const char *fmtlist_name(FMTLIST *fl);
int fmtlist_next(FMTLIST *fl);
int fmtlist_close(FMTLIST *fl);
int fmtlist_dump(FILE *outf);

#ifdef __cplusplus
}
#endif
#endif
