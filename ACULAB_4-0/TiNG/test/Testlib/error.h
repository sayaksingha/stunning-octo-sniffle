/* error.h - generic error handling */
#ifndef INCLUDED_TESTLIB_ERROR_H
#define INCLUDED_TESTLIB_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif


	/* the value 0 represents no error */
typedef struct error_report *err_t;

#include <stdio.h>

/*
 * Construct an error report. 'cause' represents the reason why we
 * are reporting this error. For example, if we are reporting that
 * we cannot display a file, the cause might be that we failed to
 * open the file, which in turn has a cause that the open failed
 * because the access was not permitted. 'text' is the text of the
 * error message.
 */
err_t error(err_t cause, const char *text);

/* We got an error while trying to recover from a previous error. Compose
 * a report saying this. We return the newest error, after appending the
 * previous error and an explanation to its list of causes.
 * In principle we shouldn't need this function since the final error
 * could have been built up just like a normal error, but that would
 * require the functions like error_errno() to take an extra parameter
 * specifying prior cause, which isn't very neat since it would almost
 * never get used.
 */
err_t error_join(err_t orig, err_t newerr);

/*
 * Construct an error report based on an errno value, 'eno'. 'text'
 * is another error message explaining the consequences. For example,
 * 'Permission denied' may have the consequence 'cannot create
 * listing file' (alternatively, you can consider 'text' to the the
 * error and 'eno' its cause).
 */
err_t error_errno(int eno, const char *text);

#ifdef TiNGTYPE_WINNT
/* Make error report from GetLastError()
 */
err_t error_last(char *text);
#endif

/*
 * Print an error report to the specified file. If 'err' is 0 (no
 * error), no output is produced, so it is suitable for use
 * unconditionally where no further checking is required (e.g. in an
 * interactive command interpreting loop which will always read the
 * next command, even if the last one has an error).
 * Note: this destroys the error.
 */
void error_log(FILE *errfile, err_t err);

/* Fetch the cause of an error - note that this is only valid while the
 * original err_r is valid
 */
err_t error_cause(err_t err);

/* Fetch the text of an error - note that this is only valid while the
 * original err_r is valid
 */
char *error_text(err_t err);

/* Destroy an error. This is an alternative to using error_log() to
 * print it.
 */
void error_dtor(err_t err);

#ifdef __cplusplus
}
#endif
#endif
