#include <stdlib.h>

#ifdef TiNGTYPE_LINUX
#include <string.h>
#endif

#ifdef TiNGTYPE_QNX
#include <string.h>
#endif


#ifdef TiNGTYPE_WINNT
#include <string.h>
#include "../libutil/WINNT/wind.h"

#define strdup _strdup
#endif

#include "error_class.hpp"

/* it's possible that we cannot malloc space while trying to report
 * an error. Unfortunately, there's not much we can do about that,
 * but at least we can have this constant to report what has
 * happened.
 */
error::error_report error::mem_report = {
	0, "Out of memory while reporting an error", 0
};

error::error_report *error::makerror(const char *text, const error &cause)
{
 error::error_report *err = (error::error_report *) malloc(sizeof(*err));
 if (!err) return &mem_report;
 err->refs = 1;
 if (cause) {
 	err->cause = cause.err_;
	err->cause->refs++;
 } else {
 	err->cause = 0;
 }
 err->text = strdup(text);	// may be null - checked in error_log()
 return err;
}

/* most error reports will use a literal string, so we might try to
 * simply save a pointer to the string, but it's essential to allow
 * reports to have parameters in them, which must be built at run
 * time by something like sprintf(), and for re-entrancy and
 * thread-safety this must use automatic storage, so we must copy the
 * error string as it will probably have gone out of scope by the
 * time error_log() is called.
 */
error::error(const char *text)
{
 if (!text) abort();
 err_ = makerror(text, error());
}

error::error(error cause, const char *text)
{
 if (!text) abort();
 err_ = makerror(text, cause);
}

error::error(error orig, error newerr)
{
 err_ = newerr.err_;
 if (err_ != &mem_report) {
	orig.err_->refs++;
	newerr.err_->refs++;
	error_report **ep = &err_;
	for (; *ep; ep = &(*ep)->cause);
	*ep = makerror("Got error recovering from previous error", orig);
 }
}

error::error(int eno, const char *text)
{
 char buff[1024];
 char *e = strerror(eno);
 if (e) {
	sprintf(buff, "%s: errno=%d", e, eno);
 } else {
	sprintf(buff, "Unknown error: errno=%d", eno);
 }
 err_ = makerror(text, error(buff));
}

std::ostream &operator<<(std::ostream &s, error e)
{
 char *fmt = "Error: ";
 error::error_report *err = e.err_;
 for (; err; ) {
		// check if srtdup() in error() failed
	const char *text = err->text;
	if (!text) text = "(insufficient memory - error description lost)";
	s << fmt << text << std::endl;
	err = err->cause;
	fmt = "\t";
 }
 return s;
}

#ifdef TiNGTYPE_WINNT
static const char winderr[] = {
	"Windows error %ld and got error %d trying to get text for this error"
};

error::error(DWORD gle)
{
 char *bp;
 long e2 = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
 	0,
 	gle,
 	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
 	(LPTSTR) &bp,
 	0,
 	0);
 if (!e2) {
	bp = (char *) malloc(sizeof(winderr) + 40);
	if (bp) sprintf(bp, winderr, gle, GetLastError());
 	err_ = makerror(bp, error());
 	free(bp);
 } else {
 	int i = strlen(bp);
 	if (i && bp[--i] == '\n') bp[i] = 0;
 	err_ = makerror(bp, error());
 	LocalFree(bp);
 }
}
#endif
