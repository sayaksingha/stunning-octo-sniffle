/* activerecord.hpp - general record using eventsets */

#ifndef INCLUDED_ACTIVERECORD_HPP
#define INCLUDED_ACTIVERECORD_HPP

#include "../../class/eventset.hpp"
#include "../../class/prosody.hpp"
#include "../../class/prosodywaitable.hpp"


class ActiveRecord : public ActiveChannel {
public:
	enum State {
		IDLE,
		NODATA,
		STARTING,
		BUSY,
		OVERRUN,
		LAST,
		COMPDATA,
		DONE,
	};
private:
	Prosody::Channel *chan_;
	Prosody::Event *ev_;
	FILE *fp_;
	unsigned xfersize_;
	enum State state_;
public:
	ActiveRecord(Prosody::Channel *chan, Prosody::Event *ev, FILE *fp, unsigned xfersize)
		: ActiveChannel(ProsodyWaitable(*ev)), chan_(chan), ev_(ev), fp_(fp), xfersize_(xfersize), state_(IDLE) { }
	enum State state() { return state_; }
	void idle() { state_ = IDLE; }
	operator Prosody::Channel &() { return *chan_; }
	error abort(int discard);
	int handler();
	void cleanup();
	~ActiveRecord() {
		delete ev_;
		delete chan_;
		if (fp_) fclose(fp_);
	}
private:
	void state(enum State ns) {
		if (state_ < ns) state_ = ns;
	}
	int handle_data();
	error checksync();
};

#endif
