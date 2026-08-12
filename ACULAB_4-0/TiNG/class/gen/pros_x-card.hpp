private:
	class OpenProsodyX {
		mutable int used_;
		SM_OPEN_PROSODY_X_PARMS openp_;
	public:
		OpenProsodyX(const OpenProsodyX &old) {
			*this = old;
			old.used_ = 1;
		};
		OpenProsodyX() : used_(0) {
			memset(&openp_, 0, sizeof(openp_));
		};
		operator Error() {
			Error e(Prosody::sm_open_prosody_x(&openp_));
			used_ = 1;
			return e;
		};
		~OpenProsodyX() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_open_prosody_x not called\n");
				abort();
			};
		};
		OpenProsodyX &px_locator(char* v) {
			openp_.px_locator = v;
			return *this;
		};
		OpenProsodyX &card_id(tSMCardId v) {
			openp_.card_id = v;
			return *this;
		};
	};
public:
	OpenProsodyX open_prosody_x() {
		return OpenProsodyX().card_id(card_);
	}
private:
	class OpenProsodySV3 {
		mutable int used_;
		SM_OPEN_PROSODY_S_V3_PARMS openp_;
	public:
		OpenProsodySV3(const OpenProsodySV3 &old) {
			*this = old;
			old.used_ = 1;
		};
		OpenProsodySV3() : used_(0) {
			memset(&openp_, 0, sizeof(openp_));
		};
		operator Error() {
			Error e(Prosody::sm_open_prosody_s_v3(&openp_));
			used_ = 1;
			return e;
		};
		~OpenProsodySV3() {
			if (!used_) {
				fprintf(stderr, "Prosody function sm_open_prosody_s_v3 not called\n");
				abort();
			};
		};
		OpenProsodySV3 &ps_locator(char* v) {
			openp_.ps_locator = v;
			return *this;
		};
		OpenProsodySV3 &card_id(tSMCardId v) {
			openp_.card_id = v;
			return *this;
		};
	};
public:
	OpenProsodySV3 open_prosody_s_v3() {
		return OpenProsodySV3().card_id(card_);
	}
