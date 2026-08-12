#ifndef INCLUDED_RECORDFILE_HPP
#define INCLUDED_RECORDFILE_HPP

#include "eventset.hpp"
#include "prosody.hpp"
#include "prosodywaitable.hpp"

class RecordFile : public ActiveChannel {
private:
	Prosody::Channel &chan_;
	FILE *pf_;
	pthread_mutex_t mx_;
	int unsafe_handler();	// called with lock held
public:
	RecordFile(Prosody::Channel &chan, Prosody::Event &ev, FILE *pf)
		: ActiveChannel(ProsodyWaitable(ev)), chan_(chan), pf_(pf) {
		pthread_mutex_init(&mx_, 0);
	}
	Prosody::Error record_abort() {
		pthread_mutex_lock(&mx_);
		Prosody::Error e = chan_.record_abort();
		pthread_mutex_unlock(&mx_);
		return e;
	}
	int handler();
	void cleanup();
};

#endif
