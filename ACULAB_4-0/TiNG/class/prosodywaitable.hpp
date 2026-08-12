#ifndef INCLUDED_PROSODYWAITABLE_HPP
#define INCLUDED_PROSODYWAITABLE_HPP

#include "eventset.hpp"

class ProsodyWaitable : public Waitable {
public:
#ifdef EVENTSET_USE_POLL
	ProsodyWaitable(Prosody::tSMEventId ev) : Waitable(ev.fd, ev.mode) {}
#endif
#ifdef EVENTSET_USE_WFMO
	ProsodyWaitable(Prosody::tSMEventId ev) : Waitable(ev) {}
#endif
};

#endif
