#ifndef INCLUDED_PROSODY_CARD_HPP
#define INCLUDED_PROSODY_CARD_HPP

#include <string.h>

#include "prosody_error.hpp"

namespace Prosody {

#include "prosody_x.h"
#include "smbesp.h"
#include "smdrvr.h"
#include "smdc.h"

class Card {
	//static const tSMCardId kSMNullCardId = 0x5a5b5c5d;
	tSMCardId card_;
	Card(Card &);
	Card &operator=(Card &) const;
public:
	Card(tSMCardId id) : card_(id) { }
	operator tSMCardId() { return card_; }
	Error free() {
		return Error(0);
	}
	~Card() {
		free().ok();
	}
#include "gen/prosgen-card.hpp"
#include "gen/pros_x-card.hpp"
};

}

#endif
