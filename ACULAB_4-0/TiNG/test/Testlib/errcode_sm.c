#include <stdio.h>

#define arlen(s) (sizeof(s)/sizeof(*(s)))

#include "smdrvr.h"

#include "errcode_sm.h"

const char *errcode_sm(int err)
{
 switch (err) {
#include "gen/errtable.i"
 default: return 0;
 }
}

int printerr_sm(const char *name, int r)
{
 const char *err = errcode_sm(r);
 if (!err) err = "Unknown error";
 fprintf(stderr, "%s() returns %d (%s)\n", name, r, err);
 return r;
}
