#include "eventset.hpp"

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef TiNGTYPE_LINUX
#include <poll.h>
#include <unistd.h>
#endif

#ifdef TiNGTYPE_QNX
#include <sys/poll.h>
#include <unistd.h>
#endif


/* see also the application note "Prosody application note: waiting on
 * multiple channels"
 */

/*
 * To wake up the worker we use a pipe on Unix style systems. A write to
 * the pipe wakes the worker, who reads from the pipe before waiting
 * again.
 */
EventSet::EventSet() : head_(0), tailp_(&head_), state_(STATE_RUN), qlen_(0)
{
 int e = pthread_mutex_init(&mx_, 0);
 if (e) {
	err_ = error(e, "pthread_mutex_init() failed");
 } else {
	e = pthread_cond_init(&runchange_, 0);
	if (e) {
		err_ = error(e, "pthread_cond_init() failed");
	} else if (pipe(changed_)) {
		err_ = error(errno, "pipe() failed");
		pthread_cond_destroy(&runchange_);
	}
	if (err_) pthread_mutex_destroy(&mx_);
 }
 if (err_) err_ = error(err_, "cannot create eventset");
}

error EventSet::mainwait(unsigned *nwaitp, ActiveChannel **listp)
{
	// first, build list of things to wait for
 struct pollfd *waitfor = (struct pollfd *) malloc(*nwaitp * sizeof(*waitfor));
 if (!waitfor) {
	return error(error(errno, "malloc() failed"), "cannot make event list");
 }
 unsigned nwait;
 {	// prepare waiting list of channels
  ActiveChannel *cp = *listp;
  int i = 0;
  for (; cp; i++, cp = cp->next_) {
	waitfor[i] = cp->event();
  }
  if (!i && state_ != STATE_RUN) {
	free(waitfor);
	return error();
  }
	// and add the pipe
  waitfor[i].fd = changed_[0];
  waitfor[i].events = POLLIN;
  *nwaitp = nwait = i + 1;
 }
 for (;;) {
		// loop using same list until it changes
	int n = poll(waitfor, nwait, -1);
	if (n == -1) {
		if (errno == EINTR) continue;
		free(waitfor);
		return error(errno, "poll() failed");
	}
		// find signalled channels
	int changed = 0;
	ActiveChannel **ncp = listp;
	int i = 0;
	for (; n; i++) {
		ActiveChannel *cp = *ncp;
		if (!cp) {
			if (waitfor[nwait - 1].revents) {
					// a work order is waiting
				char c;
				int nr = read(changed_[0], &c, 1);
				if (nr == -1) {
					err_ = error(errno, "read() failed");
					break;
				}
				if (!nr) {
					err_ = error("read() got EOF");
					break;
				}
				free(waitfor);
				return error();
			}
			abort();
		}
		if (waitfor[i].revents) {
			n--;
			if (cp->handler()) {
				changed = 1;
				*ncp = cp->next_;	// delete from list
				--*nwaitp;
				cp->cleanup();
				continue;
			}
		}
		ncp = &cp->next_;
	}
	if (changed) {
		free(waitfor);
		return error();
	}
 }
}

error EventSet::wait()
{
 ActiveChannel *chanlist = 0;		// the list of channels we are handling
 /* The number of items to wait for. This is used to allocate a big
  * enough list and may be an over-estimate after a deletion
  */
 unsigned nwait = 1;
 error err;
 for (;;) {
	// loop used when changes to list may have happened
	err = mainwait(&nwait, &chanlist);
	if (err) break;
	int e = pthread_mutex_lock(&mx_);
	if (e) {
		err = error(e, "pthread_mutex_lock() failed");
		break;
	}
	*tailp_ = chanlist;
	chanlist = head_;
	nwait += qlen_;
	head_ = 0;
	tailp_ = &head_;
	qlen_ = 0;
	if (!chanlist && state_ != STATE_RUN) {
		state_ = STATE_STOPPED;
		e = pthread_mutex_unlock(&mx_);
		if (e) {
			err = error(e, "pthread_mutex_unlock() failed");
			break;
		}
		e = pthread_cond_broadcast(&runchange_);
		if (e) return error(e, "pthread_cond_broadcast() failed");
		return error();
	}
	e = pthread_mutex_unlock(&mx_);
	if (e) {
		err = error(e, "pthread_mutex_unlock() failed");
		break;
	}
 }
	/* we get here only on error.
	 * Maybe we should do something here to the list of channels?
	 */
 return error(err, "eventset_wait() failed");
}

static error kick(int fd)
{
 char c = '*';
 int n = write(fd, &c, 1);
 if (n < 0) {
	return error(errno, "erite() failed");
 } else if (!n) {
	return error(0, "write() wrote nothing");
 }
 return error();
}

error EventSet::stop()
{
 error err;
 int e = pthread_mutex_lock(&mx_);
 if (e) {
	err = error(e, "pthread_mutex_lock() failed");
 } else {
	err = kick(changed_[1]);
	state_ = STATE_STOPPING;
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
 if (err) err = error(err, "EventSet::stop cannot stop worker");
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
	qlen_++;
	e = pthread_mutex_unlock(&mx_);
	if (e) {
		error nerr = error(e, "pthread_mutex_unlock() failed");
		if (!err) err = nerr;
		else err = error(err, nerr);
	} else {
		err = kick(changed_[1]);
	}
 }
 if (err) err = error(err, "eventset_insert cannot modify wait set");
 return err;
}
