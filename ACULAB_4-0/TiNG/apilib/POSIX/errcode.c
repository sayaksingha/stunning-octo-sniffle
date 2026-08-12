/* errcode.c - printing error codes */
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "../trace.h"
#include "../errcode.h"

void print_errno(int err, char *msg)
{
 if (!err) olog("Error: %s\n", msg);
 else {
	char *txt = strerror(err);
	if (!txt) txt = "<Unknown error code>";
	olog("Error: %d: %s: %s\n", err, txt, msg);
 }
}

void printerr(ERRCODE e)
{
 if (ERR(e)) print_errno(e.errnum, e.text);
}
