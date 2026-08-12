/* errcode.h - O/S independent error decoding */
#ifndef INCLUDED_ERRCODE_H
#define INCLUDED_ERRCODE_H

#ifdef TiNGTYPE_WINNT
#include "../libutil/WINNT/wind.h"
#endif

typedef struct {
	char *text;
#ifdef TiNGTYPE_LINUX
	int errnum;
#endif
#ifdef TiNGTYPE_QNX
	int errnum;
#endif
#ifdef TiNGTYPE_WINNT
	DWORD errnum;
#endif
} ERRCODE;

	// check for an error
#define ERR(x) (!!(x).text)

	// the global 'OK' value. This is just a convenience - any value
	// with a null pointer for its string component represents OK, so
	// if you ever see code like this:
	//	if (e == OK) ...
	// you know it is wrong and should be
	//	if (!ERR(e)) ...
static const ERRCODE OK;

	// we have to let these be global, so avoid unqualified names
#define print_errno aculab_TiNGcore_print_errno
#define printerr aculab_TiNGcore_printerr
#define print_lasterr aculab_TiNGcore_print_lasterr
#define print_winerr aculab_TiNGcore_print_winerr

	// print an error message with errno value
void print_errno(int err, char *msg);
	// print the error
void printerr(ERRCODE e);

#ifdef TiNGTYPE_WINNT
	// print an error message with GetLastError() value
void print_lasterr(char *msg);

void print_winerr(char *msg, DWORD err);
#endif

#endif
