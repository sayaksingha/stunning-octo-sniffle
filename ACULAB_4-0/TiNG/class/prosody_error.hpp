#ifndef INCLUDED_PROSODY_ERROR_HPP
#define INCLUDED_PROSODY_ERROR_HPP

#include <stdio.h>
#include <stdlib.h>

namespace Prosody {

class Error {
	int err_;
	mutable int ok_;
public:
	Error(const Error &old) : err_(old.err_), ok_(0) {
		old.ok_ = 1;
	}
	Error(int e) : err_(e), ok_(0) { }
	void operator =(int err) {
		err_ = err;
		ok_ = 0;
	}
	void operator =(const Error &e) {
		e.ok_ = 1;
		err_ = e.err_;
		ok_ = 0;
	}
	operator int() { ok_ = 1; return err_; }
	operator bool() { ok_ = 1; return err_; }
	void ok() {		// ignore any error
		if (err_) {
			printf("Ignoring Prosody error %d\n", err_);
		}
		ok_ = 1;
	}
	~Error() {
		if (!ok_) {
			fprintf(stderr, "Prosody error %d ignored\n", err_);
			abort();
		}
	}
};

}

#endif
