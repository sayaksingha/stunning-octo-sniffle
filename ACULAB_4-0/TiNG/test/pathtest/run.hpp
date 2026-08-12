#ifndef INCLUDED_RUN_HPP
#define INCLUDED_RUN_HPP

#include "card.hpp"
#include "mod.hpp"
#include "path.hpp"
#include "play.hpp"
#include "rtcp.hpp"
#include "rtprx.hpp"
#include "rtptx.hpp"
#include "rec.hpp"
#include "tdmrx.hpp"
#include "tdmtx.hpp"

	// the whole collection of things which are running
//struct RUNSTATE {
class RUNSTATE {
public:
	CARD card;
	MOD mod;
	PATH path;
	PLAY play;
	REC rec;
	RTCP rtcp;
	RTPRX rtprx;
	RTPTX rtptx;
	TDMRX tdmrx;
	TDMTX tdmtx;
		// find something by name
	RUNNABLE *lookup(const char *type) {
		if (!strcmp(card.name(), type)) return &card;
		if (!strcmp(mod.name(), type)) return &mod;
		if (!strcmp(path.name(), type)) return &path;
		if (!strcmp(play.name(), type)) return &play;
		if (!strcmp(rec.name(), type)) return &rec;
		if (!strcmp(rtcp.name(), type)) return &rtcp;
		if (!strcmp(rtprx.name(), type)) return &rtprx;
		if (!strcmp(rtptx.name(), type)) return &rtptx;
		if (!strcmp(tdmrx.name(), type)) return &tdmrx;
		if (!strcmp(tdmtx.name(), type)) return &tdmtx;
		return 0;
	}
	int init() {
		return card.init()
		|| mod.init()
		|| path.init()
		|| play.init()
		|| rec.init()
		|| rtcp.init()
		|| rtprx.init()
		|| rtptx.init()
		|| tdmrx.init()
		|| tdmtx.init();
	}
	void fini() {
		card.fini();
		mod.fini();
		path.fini();
		play.fini();
		rec.fini();
		rtcp.fini();
		rtprx.fini();
		rtptx.fini();
		tdmrx.fini();
		tdmtx.fini();
	}
};

#endif
