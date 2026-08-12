/* activertprx.hpp - general vmprx using eventsets */

#ifndef INCLUDED_ACTIVERTPRX_HPP
#define INCLUDED_ACTIVERTPRX_HPP

#include "../../class/eventset.hpp"
#include "../../class/prosody.hpp"
#include "../../class/prosodywaitable.hpp"


class ActiveRtprx : public ActiveChannel {
public:
	enum State {
		IDLE,
		RUN,
		LOST,
		PORTS,
		NEWSSRC,
		TONE,
		ENDTONE,
		PAYLOAD,
		DONE,
	};
private:
	Prosody::Vmprx *vr_;
	enum State state_;
public:
	ActiveRtprx(Prosody::Vmprx *vr, Prosody::Event *ev)
		: ActiveChannel(ProsodyWaitable(*ev)), vr_(vr), state_(IDLE) { }
	enum State state() { return state_; }
	void idle() { state_ = IDLE; }
	operator Prosody::Vmprx &() { return *vr_; }
	int handler();
	void cleanup();
	~ActiveRtprx() {
		delete vr_;
	}
private:
	void state(enum State ns) {
		if (state_ < ns) state_ = ns;
	}
};

#endif
