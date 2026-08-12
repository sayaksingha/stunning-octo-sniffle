/* activertptx.hpp - general vmptx using eventsets */

#ifndef INCLUDED_ACTIVERTPTX_HPP
#define INCLUDED_ACTIVERTPTX_HPP

#include "../../class/eventset.hpp"
#include "../../class/prosody.hpp"
#include "../../class/prosodywaitable.hpp"


class ActiveRtptx : public ActiveChannel {
public:
	enum State {
		IDLE,
		RUN,
		TONECONT,
		TONE,
		SSRC,
		DONE,
	};
private:
	Prosody::Vmptx *vt_;
	enum State state_;
public:
	ActiveRtptx(Prosody::Vmptx *vt, Prosody::Event *ev)
		: ActiveChannel(ProsodyWaitable(*ev)), vt_(vt), state_(IDLE) { }
	enum State state() { return state_; }
	void idle() { state_ = IDLE; }
	operator Prosody::Vmptx &() { return *vt_; }
	int handler();
	void cleanup();
	~ActiveRtptx() {
		delete vt_;
	}
private:
	void state(enum State ns) {
		if (state_ < ns) state_ = ns;
	}
};

#endif
