#ifndef INCLUDED_EVENTSET_H
#define INCLUDED_EVENTSET_H


#include "error_class.hpp"

#ifdef TiNGTYPE_LINUX
#define EVENTSET_USE_POLL
#endif
#ifdef TiNGTYPE_QNX
#define EVENTSET_USE_POLL
#endif
#ifdef TiNGTYPE_WINNT
#define EVENTSET_USE_WFMO
#define EVENTSET_LIMIT 63
#endif

#ifdef EVENTSET_USE_POLL
#include <pthread.h>
#include <sys/poll.h>
class Waitable {
private:
	struct pollfd w_;
public:
	Waitable(int fd, int events) {
		w_.fd = fd;
		w_.events = events;
	}
	operator pollfd() {
		return w_;
	}
};
#endif

#ifdef EVENTSET_USE_WFMO
#include "../libutil/WINNT/pthread.h"
#include "../libutil/WINNT/wind.h"
class Waitable {
private:
	HANDLE w_;
public:
	Waitable(HANDLE h) : w_(h) { }
	operator HANDLE() {
		return w_;
	}
};
#endif

class ActiveChannel {
private:
	ActiveChannel *next_;
	Waitable wevent_;
public:
	Waitable event() { return wevent_; }
	ActiveChannel(Waitable ev) :
		next_(0),
		wevent_(ev) {}
		// handle a channel with an active event, must return
		// non-zero if the channel is to be deleted from the
		// list of active channels
	virtual int handler() = 0;
		// run when the channel is deleted from the list of
		// active channels. It could delete the channel or
		// put it into a pool of channels awaiting re-use.
	virtual void cleanup() = 0;
	virtual ~ActiveChannel() {}
	friend class EventSet;
};

class EventSet {
private:
	pthread_mutex_t mx_;		// protects head and nextp
	ActiveChannel *head_, **tailp_;	// linked list of channels to be added
	enum { STATE_RUN, STATE_STOPPING, STATE_STOPPED } state_;
	error err_;
	pthread_cond_t runchange_;
#ifdef EVENTSET_USE_POLL
	error mainwait(unsigned *nwaitp, ActiveChannel **listp);
	unsigned qlen_;
	int changed_[2];		// for waking the worker
#endif
#ifdef EVENTSET_USE_WFMO
	error newwaiter();
	class Waiter {
		class Waiter **prevp_, *next_;
		EventSet *set_;
		HANDLE changed_;		// for waking the worker
		unsigned cap_;			// capacity remaining
		int internal_;
		Waiter(EventSet *set);
		error mainwait(unsigned *nwaitp, ActiveChannel **chanlist, HANDLE *waitfor);
	public:
		friend class EventSet;
		error wait();
		void unlink() {
			if (next_) next_->prevp_ = prevp_;
			*prevp_ = next_;
			prevp_ = &next_;
			next_ = this;
		}
		void inshead(Waiter **headp) {
			next_ = *headp;
			if (next_) next_->prevp_ = &next_;
			prevp_ = headp;
			*headp = this;
		}
	} *full_waiters_, *avail_waiters_;
	friend class Waiter;
#endif
	static ActiveChannel *nextchan(ActiveChannel *chan) {
		return chan->next_;
	}
public:
	EventSet();
	error wait();
	error stop();
	error insert(ActiveChannel *acp);
#ifdef EVENTSET_USE_WFMO
	static error wait(void *p) {
		EventSet::Waiter *w = (EventSet::Waiter *) p;
		return w->wait();
	}
#endif
};

#endif
