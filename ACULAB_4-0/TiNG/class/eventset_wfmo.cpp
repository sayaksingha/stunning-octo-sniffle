#include "eventset.hpp"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "error_class.hpp"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

/* see also the application note "Prosody application note: waiting on
 * multiple channels"
 */

EventSet::Waiter::Waiter(EventSet *set) :
	set_(set),
	cap_(EVENTSET_LIMIT),
	internal_(0)
{
 changed_ = CreateEvent(0, 0, 0, 0);
 if (!changed_) {
	set->err_ = error(error(GetLastError()), "CreateEvent() failed");
 }
 inshead(&set_->avail_waiters_);
}

EventSet::EventSet() : head_(0),
	tailp_(&head_),
	state_(STATE_RUN),
	full_waiters_(0),
	avail_waiters_(0)
{
 if (!err_) {
	int e = pthread_mutex_init(&mx_, 0);
	if (e) {
		err_ = error(e, "pthread_mutex_init() failed");
	} else {
		e = pthread_cond_init(&runchange_, 0);
		if (e) {
			err_ = error(e, "pthread_cond_init() failed");
			pthread_mutex_destroy(&mx_);
		}
	}
 }
 if (err_) err_ = error(err_, "cannot create eventset");
}

error EventSet::Waiter::mainwait(unsigned *nwaitp, ActiveChannel **chanlist, HANDLE *waitfor)
{
 // loop used when changes to list may have happened
 for (;;) {
		// loop using same list until it changes
	DWORD n = WaitForMultipleObjects(*nwaitp, waitfor, 0, INFINITE);
	if (n > WAIT_OBJECT_0 && n < WAIT_OBJECT_0 + *nwaitp) {
		const int last = *nwaitp - 1;
		int i = n - WAIT_OBJECT_0;
		ActiveChannel *cp = chanlist[i];
		if (i != last) {
			// swap with last
			// - to avoid starvation and make deletion easy
			HANDLE w = waitfor[i];
			chanlist[i] = chanlist[last];
			waitfor[i] = waitfor[last];
			chanlist[last] = cp;
			waitfor[last] = w;
		}
		if (cp->handler()) {
			--*nwaitp;	// delete from set
			cap_++;
			cp->cleanup();
			return error();
		}
	} else if (n == WAIT_OBJECT_0) {
		return error();
	} else {
		return error(error(GetLastError()), "WaitForMultipleObjects() failed");
	}
 }
}

static error kick(HANDLE h)
{
 if (!SetEvent(h)) {
	return error(error(GetLastError()), "SetEvent() failed");
 }
 return error();
}

static void *waiter_thread(void *p)
{
 error e = EventSet::wait(p);
 if (e) {
 	e.print(stderr);
	return (void *) 1;
 }
 return 0;
}

error EventSet::newwaiter()
{
 if (avail_waiters_) {
	return kick(avail_waiters_->changed_);
 }
 Waiter *w = new Waiter(this);
 w->internal_ = 1;
 pthread_t tid;
 int e = pthread_create(&tid, 0, waiter_thread, (void *) w);
 if (e) {
	return error(e, "pthread_create() failed");
 }
 return error();
}

error EventSet::wait()
{
 Waiter *w = new Waiter(this);
 error e = w->wait();
 delete w;
 return e;
}

error EventSet::Waiter::wait()
{
 ActiveChannel *chanlist[EVENTSET_LIMIT + 1];
 HANDLE waitfor[EVENTSET_LIMIT + 1];
 unsigned nwait = 1;	// The number of items to wait for.
 error err;
 waitfor[0] = changed_;
 for (;;) {
	err = mainwait(&nwait, chanlist, waitfor);
	if (err) break;
	int e = pthread_mutex_lock(&set_->mx_);
	if (e) {
		err = error(e, "pthread_mutex_lock() failed");
		break;
	}
	for (;;) {
		ActiveChannel *head = set_->head_;
		if (!head) {
			set_->tailp_ = &set_->head_;
			if (nwait < arlen(chanlist)) {
				unlink();
				inshead(&set_->avail_waiters_);
			}
			if (nwait == 1 && set_->state_ == STATE_STOPPING) {
				unlink();
				if (!set_->full_waiters_ && !set_->avail_waiters_) {
					set_->state_ = STATE_STOPPED;
					e = pthread_cond_broadcast(&set_->runchange_);
					if (e) {
						error nerr = error(e, "pthread_cond_broadcast() failed");
						if (!err) err = nerr;
						else err = error(err, nerr);
					}
				}
				e = pthread_mutex_unlock(&set_->mx_);
				if (e) {
					error nerr = error(e, "pthread_mutex_unlock() failed");
					if (!err) err = nerr;
					else err = error(err, nerr);
				}
				return error();
			}
			break;
		}
		if (nwait >= arlen(chanlist)) {
			err = set_->newwaiter();
			break;
		}
		waitfor[nwait] = head->event();
		chanlist[nwait++] = head;
		set_->head_ = nextchan(head);
		if (nwait >= arlen(chanlist)) {
			unlink();
			inshead(&set_->full_waiters_);
		}
	}
	if (err) break;
	e = pthread_mutex_unlock(&set_->mx_);
	if (e) {
		error nerr = error(e, "pthread_mutex_unlock() failed");
		if (!err) err = nerr;
		else err = error(err, nerr);
		break;
	}
 }
	/* we get here only on error.
	 * Maybe we should do something here to the list of channels?
	 */
 return error(err, "eventset_wait() failed");
}

error EventSet::stop()
{
 error err;
 int e = pthread_mutex_lock(&mx_);
 if (e) {
	err = error(e, "pthread_mutex_lock() failed");
 } else {
	state_ = STATE_STOPPING;
	if (avail_waiters_) err = kick(avail_waiters_->changed_);
	if (full_waiters_) err = kick(full_waiters_->changed_);
	while (state_ != STATE_STOPPED) {
		e = pthread_cond_wait(&runchange_, &mx_);
		if (e) {
			error nerr = error(e, "pthread_cond_wait() failed");
			if (!err) err = nerr;
			else err = error(err, nerr);
			break;
		}
	}
	state_ = STATE_RUN;
	e = pthread_mutex_unlock(&mx_);
	if (e) {
		error nerr = error(e, "pthread_mutex_unlock() failed");
		if (!err) err = nerr;
		else err = error(err, nerr);
	}
 }
 if (err) err = error(err, "eventset_insert cannot modify wait set");
 return err;
}

error EventSet::insert(ActiveChannel *acp)
{
 error err;
 int e = pthread_mutex_lock(&mx_);
 if (e) {
	err = error(e, "pthread_mutex_lock() failed");
 } else {
		// link it onto the list
	acp->next_ = 0;
	*tailp_ = acp;
	tailp_ = &acp->next_;
	e = pthread_mutex_unlock(&mx_);
	if (e) {
		error nerr = error(e, "pthread_mutex_unlock() failed");
		if (!err) err = nerr;
		else err = error(err, nerr);
	} else {
		if (avail_waiters_) err = kick(avail_waiters_->changed_);
		else if (full_waiters_) err = kick(full_waiters_->changed_);
		else {
			fprintf(stderr, "Internal error: no waiters\n");
			abort();
		}
	}
 }
 if (err) err = error(err, "eventset_insert cannot modify wait set");
 return err;
}
