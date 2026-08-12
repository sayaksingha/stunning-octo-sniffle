private:
	class GetInfo {
		mutable int used_;
		SM_CARD_INFO_PARMS cardinfop_;
		enum kSMCarrierCardType *card_typeP_;
		tSM_INT *card_detectedP_;
		tSM_INT *module_countP_;
		tSM_UT32 *physical_addressP_;
		tSM_UT32 *io_addressP_;
		tSM_UT32 *physical_irqP_;
	public:
		GetInfo(const GetInfo &old) {
			*this = old;
			old.used_ = 1;
		};
		GetInfo() : used_(0) {
			memset(&cardinfop_, 0, sizeof(cardinfop_));
			card_typeP_ = 0;
			card_detectedP_ = 0;
			module_countP_ = 0;
			physical_addressP_ = 0;
			io_addressP_ = 0;
			physical_irqP_ = 0;
		};
		operator Error() {
			Error e(Prosody::sm_get_card_info(&cardinfop_));
			used_ = 1;
			if (card_typeP_) *card_typeP_ = cardinfop_.card_type;
			if (card_detectedP_) *card_detectedP_ = cardinfop_.card_detected;
			if (module_countP_) *module_countP_ = cardinfop_.module_count;
			if (physical_addressP_) *physical_addressP_ = cardinfop_.physical_address;
			if (io_addressP_) *io_addressP_ = cardinfop_.io_address;
			if (physical_irqP_) *physical_irqP_ = cardinfop_.physical_irq;
			return e;
		};
		~GetInfo() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_get_card_info not called\n");
				abort();
			};
		};
		GetInfo &card(tSMCardId v) {
			cardinfop_.card = v;
			return *this;
		};
		GetInfo &card_type(enum kSMCarrierCardType *v) {
			card_typeP_ = v;
			return *this;
		};
		GetInfo &card_detected(tSM_INT *v) {
			card_detectedP_ = v;
			return *this;
		};
		GetInfo &module_count(tSM_INT *v) {
			module_countP_ = v;
			return *this;
		};
		GetInfo &physical_address(tSM_UT32 *v) {
			physical_addressP_ = v;
			return *this;
		};
		GetInfo &io_address(tSM_UT32 *v) {
			io_addressP_ = v;
			return *this;
		};
		GetInfo &physical_irq(tSM_UT32 *v) {
			physical_irqP_ = v;
			return *this;
		};
	};
public:
	GetInfo get_info() {
		return GetInfo().card(card_);
	}
private:
	class GetRev {
		mutable int used_;
		SM_CARD_REV_PARMS cardrevp_;
	public:
		GetRev(const GetRev &old) {
			*this = old;
			old.used_ = 1;
		};
		GetRev() : used_(0) {
			memset(&cardrevp_, 0, sizeof(cardrevp_));
		};
		operator Error() {
			Error e(Prosody::sm_get_card_rev(&cardrevp_));
			used_ = 1;
			return e;
		};
		~GetRev() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_get_card_rev not called\n");
				abort();
			};
		};
		GetRev &card(tSMCardId v) {
			cardrevp_.card = v;
			return *this;
		};
	};
public:
	GetRev get_rev() {
		return GetRev().card(card_);
	}
private:
	class CloseProsody {
		mutable int used_;
		SM_CLOSE_PROSODY_PARMS closep_;
	public:
		CloseProsody(const CloseProsody &old) {
			*this = old;
			old.used_ = 1;
		};
		CloseProsody() : used_(0) {
			memset(&closep_, 0, sizeof(closep_));
		};
		operator Error() {
			Error e(Prosody::sm_close_prosody(&closep_));
			used_ = 1;
			return e;
		};
		~CloseProsody() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_close_prosody not called\n");
				abort();
			};
		};
		CloseProsody &card_id(tSMCardId v) {
			closep_.card_id = v;
			return *this;
		};
	};
public:
	CloseProsody close_prosody() {
		return CloseProsody().card_id(card_);
	}
private:
	class GetSwitchId {
		mutable int used_;
		tSMCardId card_;
	public:
		GetSwitchId(const GetSwitchId &old) {
			*this = old;
			old.used_ = 1;
		};
		GetSwitchId(tSMCardId card) : used_(0) {
			card_ = card;
		};
		operator Error() {
			Error e(Prosody::sm_get_card_switch_id(card_));
			used_ = 1;
			return e;
		};
		~GetSwitchId() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_get_card_switch_id not called\n");
				abort();
			};
		};
	};
public:
	GetSwitchId get_switch_id() {
		return GetSwitchId(card_);
	}
