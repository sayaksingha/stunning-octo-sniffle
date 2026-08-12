/* error.h - generic error handling */
#ifndef INCLUDED_ERROR_H
#define INCLUDED_ERROR_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#ifdef TiNGTYPE_WINNT
#include "../libutil/WINNT/wind.h"
#endif

class error {
private:
	struct error_report {
		mutable unsigned refs;
		char *text;
		error_report *cause;
	} *err_;
	static error_report mem_report;
	static error_report *makerror(const char *text, const error &cause);
	void dispose() {
		for (;err_;) {
			if (--err_->refs) break;
			if (err_ == &mem_report) break;
			error_report *nx = err_->cause;
			free(err_->text);
			free(err_);
			err_ = nx;
		}
	}
public:
	error() : err_(0) {}			// no error
	operator bool() const {
		return err_ != 0;
	}
	error(const error &rhs) {
		if ((err_ = rhs.err_)) err_->refs++;
	}
	error &operator=(const error &rhs) {
		if (this != &rhs) {
			this->dispose();
			if ((this->err_ = rhs.err_)) this->err_->refs++;
		}
		return *this;
	}
	~error() {
		dispose();
	}
	explicit error(const char *text);		// simple error
/*
 * Construct an error report. 'cause' represents the reason why we
 * are reporting this error. For example, if we are reporting that
 * we cannot display a file, the cause might be that we failed to
 * open the file, which in turn has a cause that the open failed
 * because the access was not permitted. 'text' is the text of the
 * error message.
 */
	error(error cause, const char *text);
/*
 * Construct an error report based on an errno value, 'eno'. 'text'
 * is another error message explaining the consequences. For example,
 * 'Permission denied' may have the consequence 'cannot create
 * listing file' (alternatively, you can consider 'text' to the the
 * error and 'eno' its cause).
 */
	error(int e, const char *text);		// error eith errno

#ifdef TiNGTYPE_WINNT
/* Make error report from GetLastError()
 */
	explicit error(DWORD gle);
#endif

/* We got an error while trying to recover from a previous error. Compose
 * a report saying this. We return the newest error, after appending the
 * previous error and an explanation to its list of causes.
 * In principle we shouldn't need this function since the final error
 * could have been built up just like a normal error, but that would
 * require the functions like error_errno() to take an extra parameter
 * specifying prior cause, which isn't very neat since it would almost
 * never get used.
 */
	error(error orig, error newerr);	// error while recovering

/*
 * Print an error report to the specified file. If 'err' is 0 (no
 * error), no output is produced, so it is suitable for use
 * unconditionally where no further checking is required (e.g. in an
 * interactive command interpreting loop which will always read the
 * next command, even if the last one has an error).
 */
	void print(FILE *errfile) {
		char *fmt = "Error: %s\n";
		error_report *err = err_;
		for (; err; ) {
				// check if srtdup() in error() failed
			const char *text = err->text;
			if (!text) text = "(insufficient memory - error description lost)";
			fprintf(errfile, fmt, text);
			fmt = "\t%s\n";
			err = err->cause;
		}
	};
	friend std::ostream &operator<<(std::ostream &, error);
};

#endif
