#include <stdlib.h>
#include <string.h>

#ifdef TiNGTYPE_LINUX
#include <string.h>
#endif

#ifdef TiNGTYPE_QNX
#include <string.h>
#endif


#ifdef TiNGTYPE_WINNT
#include <string.h>
#include <windows.h>

#define strdup _strdup
#endif

#include "error.h"

struct error_report {
	struct error_report *cause;
	char *text;
};

/* it's possible that we cannot malloc space while trying to report
 * an error. Unfortunately, there's not much we can do about that,
 * but at least we can have this constant to report what has
 * happened.
 */
static struct error_report mem_report =
	{0, "Out of memory while reporting an error"};

/* most error reports will use a literal string, so we might try to
 * simply save a pointer to the string, but it's essential to allow
 * reports to have parameters in them, which must be built at run
 * time by something like sprintf(), and for re-entrancy and
 * thread-safety this must use automatic storage, so we must copy the
 * error string as it will probably have gone out of scope by the
 * time error_log() is called.
 */
err_t error(err_t cause, const char *text)
{
 err_t err = malloc(sizeof(*err));
 if (!err) return &mem_report;
 err->cause = cause;
 err->text = strdup(text);	// may be null - checked in error_log()
 return err;
}

/* we got an error while trying to recover from a previous error. Compose
 * a report saying this. We return the newest error, after appending the
 * previous error and an explanation to its list of causes.
 * In principle we shouldn't need this function since the final error
 * could have been built up just like a normal error, but that would
 * require the functions like error_errno() to take an extra parameter
 * specifying prior cause, which isn't very neat since it would almost
 * never get used.
 */
err_t error_join(err_t orig, err_t newerr)
{
 err_t err = malloc(sizeof(*err));
 err_t *ep;
 if (!err) return &mem_report;
 err->cause = orig;
 err->text = strdup("Got error recovering from error");
 for (ep = &newerr; *ep; ep = &(*ep)->cause);
 *ep = err; 
 return newerr;
}

err_t error_errno(int eno, const char *text)
{
 return error(error(0, strerror(eno)), text);
}

#ifdef TiNGTYPE_WINNT
static const char winderr[] = {
	"Windows error %ld and got error %d trying to get text for this error"
};

err_t error_last(char *text)
{
 DWORD gle = GetLastError();
 char *bp;
 long e2 = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
 	0,
 	gle,
 	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
 	(LPTSTR) &bp,
 	0,
 	0);
 err_t err;
 if (!e2) {
	bp = malloc(sizeof(winderr) + 40);
	if (bp) sprintf(bp, winderr, gle, GetLastError());
 	err = error(0, bp);
 	free(bp);
 } else {
 	int i = strlen(bp);
 	if (i && bp[--i] == '\n') bp[i] = 0;
 	err = error(0, bp);
 	LocalFree(bp);
 }
 return error(err, text);
}
#endif

err_t error_cause(err_t err)
{
 return err->cause;
}

char *error_text(err_t err)
{
 char *text = err->text;
	// check if strdup() in error() failed
 if (!text) text = "(insufficient memory - error description lost";
 return text;
}

void error_log(FILE *errfile, err_t err)
{
 char *fmt = "Error: %s\n";
 for (; err; ) {
	err_t cause = error_cause(err);
	fprintf(errfile, fmt, error_text(err));
	fmt = "\t%s\n";
	if (err != &mem_report) {
		free(err->text);
		free(err);
	}
	err = cause;
 }
}

void error_dtor(err_t err)
{
 for (; err; ) {
	err_t cause = error_cause(err);
	if (err != &mem_report) {
		free(err->text);
		free(err);
	}
	err = cause;
 }
}
