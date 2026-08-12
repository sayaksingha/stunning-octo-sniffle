#ifndef INCLUDED_RUNNABLE_HPP
#define INCLUDED_RUNNABLE_HPP

class RUNSTATE;
#include "cfg.hpp"

#include <vector>
#include "../../class/prosody.hpp"

	// an instance of something running (e.g. play, record)
class RUNONE {
public:
		// get the datafeed, or a null datafeed on failure
	virtual Prosody::tSMDatafeedId datafeed() = 0;
		// adjust this item to the lates set of values
	virtual int adjust(CFG *cfg, RUNSTATE *runs) = 0;
		// halt this item
	virtual int halt(CFG *cfg, RUNSTATE *runs) = 0;
		// return a printable character indicating the current status
		// A space indicates the item can be deleted
	virtual char status() = 0;
	virtual ~RUNONE() { };
};

	// a category of runnable things - a bit like vector<RUNONE> but with
	// extra infrastructure to manage it (e.g. replays need a thread to service them)
class RUNNABLE {
public:
		// perform any general initialisation, return 0 if successful
	virtual int init() = 0;
		// the name of this type of item
	virtual const char *name() = 0;
		// start a new instance of this type of item, returns non-zero
		// on failure
	virtual int start(CFG *cfg, RUNSTATE *runs, Prosody::Module *mod, Prosody::tSMDatafeedId *idp) = 0;
		// return the specified item of this type, or 0 if no such item
	virtual RUNONE *nth(RUNSTATE *runs, unsigned long n) = 0;
		// stop the last item started - used to allow parser to roll-back creation
		// of a pipeline when an error occurs part way through
	virtual void stop(CFG *cfg, RUNSTATE *runs) = 0;
		// show status of all items running
	virtual void status(std::ostream &os) = 0;
		// undo any general initialisation
	virtual void fini() = 0;
	virtual ~RUNNABLE() { };
};

#endif
