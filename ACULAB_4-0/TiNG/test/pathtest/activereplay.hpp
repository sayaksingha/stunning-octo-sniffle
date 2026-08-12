/* activereplay.hpp - general replay using eventsets */

#ifndef INCLUDED_ACTIVEREPLAY_HPP
#define INCLUDED_ACTIVEREPLAY_HPP

#include "../../class/eventset.hpp"
#include "../../class/prosody.hpp"
#include "../../class/prosodywaitable.hpp"


class ActiveReplay : public ActiveChannel {
public:
	enum Once {
		ONCE_NOT,		// play file repeatedly
		ONCE_ONLY,		// play file exactly once
		ONCE_REPEAT,		// play once, then restart
	};
	enum State {
		IDLE,
		NOCAP,
		STARTING,
		BUSY,
		UNDERRUN,
		LAST,
		COMPDATA,
		DONE,
	};
private:
	Prosody::Channel *chan_;
	Prosody::Event *ev_;
	FILE *fp_;
	Once once_;
	unsigned xfersize_;
	enum State state_;
public:
	ActiveReplay(Prosody::Channel *chan, Prosody::Event *ev, FILE *fp, unsigned xfersize, Once once)
		: ActiveChannel(ProsodyWaitable(*ev)), chan_(chan), ev_(ev), fp_(fp), once_(once), xfersize_(xfersize), state_(IDLE) {
	}
	enum State state() { return state_; }
	void idle() { state_ = IDLE; }
	operator Prosody::Channel &() { return *chan_; }
	error abort(int nowait);
	int handler();
	void cleanup();
	~ActiveReplay() {
		delete ev_;
		delete chan_;
		if (fp_) fclose(fp_);
	}
private:
	void state(enum State ns) {
		if (state_ < ns) state_ = ns;
	}
	int handle_capacity(int had_eof = 0);
	error checksync();
};

#endif
