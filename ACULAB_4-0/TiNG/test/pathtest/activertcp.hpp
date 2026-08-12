/* activertcp.hpp - general rtcp using eventsets */

#ifndef INCLUDED_ACTIVERTCP_HPP
#define INCLUDED_ACTIVERTCP_HPP

#include "../../class/eventset.hpp"
#include "../../class/prosody.hpp"
#include "../../class/prosodywaitable.hpp"
#include "../../rtcplib/rtcp_accumulate.h"


class ActiveRtcp : public ActiveChannel {
public:
	enum State {
		IDLE,
		BUSY,
		DONE,
	};
private:
	RTCP_BUFFER rb_;
	Prosody::Rtcphand *hand_;
	Prosody::Event *ev_;
	FILE *fp_;
	enum State state_;
public:
	ActiveRtcp(Prosody::Rtcphand *hand, Prosody::Event *ev, FILE *fp)
		: ActiveChannel(ProsodyWaitable(*ev)), hand_(hand), ev_(ev), fp_(fp), state_(IDLE) {
		rtcp_buffer_init(&rb_);
	}
	enum State state() { return state_; }
	void idle() { state_ = IDLE; }
	operator Prosody::Rtcphand &() { return *hand_; }
	int handler();
	void cleanup();
	~ActiveRtcp() {
		delete ev_;
		delete hand_;
		if (fp_) fclose(fp_);
	}
private:
	void state(enum State ns) {
		if (state_ < ns) state_ = ns;
	}
};

#endif
